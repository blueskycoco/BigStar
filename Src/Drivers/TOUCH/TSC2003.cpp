

//TSC2003 driver for wince6.0 ,by kreal@163.com



#include <bsp.h>
#include <tchddsi.h>
#include "TSC2003.h"
#include "iic.h"

#define BATTERY_CAMERA_MUTEX  TEXT("Battery Camera Mutex")


static HANDLE                        hI2C;   // I2C Bus Driver

struct TSC_XY
{

	DWORD   x;
	DWORD   y;
	DWORD  z1;
	DWORD  z2;

}  ;

static   TSC_XY   SampleData[MAX_SAMPLE_NUM ];
volatile S3C6410_GPIO_REG *pGPIOReg ;
static HANDLE ghBattTouchMutex=NULL;
#define BATTERY_CAMERA_MUTEX  TEXT("Battery Camera Mutex")
static void Delay()
{
	int i,j=0;
	for(i=0;i < 100; i++) { j++; }
}
DWORD Lock(void)
{
	DWORD dwStatus;

	DEBUGCHK(ghBattTouchMutex != NULL);

	dwStatus = WaitForSingleObject(ghBattTouchMutex, INFINITE);
	if(dwStatus == WAIT_OBJECT_0) {
		dwStatus = ERROR_SUCCESS;
	} else {
		dwStatus = GetLastError();
		DEBUGCHK(dwStatus != ERROR_SUCCESS);
	}
	DEBUGCHK(dwStatus == ERROR_SUCCESS);

	return dwStatus;
}

DWORD Unlock(void)
{
	DWORD dwStatus = ERROR_SUCCESS;
	BOOL fOk;
	DEBUGCHK(ghBattTouchMutex != NULL);

	fOk = ReleaseMutex(ghBattTouchMutex);
	if(!fOk) {
		dwStatus = GetLastError();
		DEBUGCHK(dwStatus != ERROR_SUCCESS);
	}
	DEBUGCHK(dwStatus == ERROR_SUCCESS);

	return dwStatus;
}

//TSC2003 wince init ,not chip initalize
void TSC_Init(void)
{
	UINT32  IICClock = 400000;
	UINT32  uiIICDelay;
	DWORD dwErr,bytes;
	PHYSICAL_ADDRESS    ioPhysicalBase = {0,0};

	DEBUGMSG(TSP_ZONE_FUNCTION,(_T("[TSP] ++TSP_VirtualAlloc()\r\n")));
	RETAILMSG(1,(_T("<Test 7>================>\r\n")));

	ioPhysicalBase.LowPart = S3C6410_BASE_REG_PA_GPIO;
	pGPIOReg = (S3C6410_GPIO_REG *)MmMapIoSpace(ioPhysicalBase, sizeof(S3C6410_GPIO_REG), FALSE);
	if (pGPIOReg == NULL)
	{
		RETAILMSG(1,(_T("[GPIO fail\r\n")));

	}

	// Initialize I2C Driver
	hI2C = CreateFile( L"IIC0:",
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, 0);

	if ( INVALID_HANDLE_VALUE == hI2C ) 
	{
		dwErr = GetLastError();
		RETAILMSG(1, (TEXT("Error %d opening device '%s' \r\n"), dwErr, L"I2C0:" ));
		return;
	}
	if ( !DeviceIoControl(hI2C,
				IOCTL_IIC_SET_CLOCK, 
				&IICClock, sizeof(UINT32), 
				NULL, 0,
				&bytes, NULL) ) 
	{
		dwErr = GetLastError();
		RETAILMSG(1,(TEXT("TSC IOCTL_IIC_SET_CLOCK ERROR: %u \r\n"), dwErr));
		return;
	}       

	uiIICDelay = Clk_0;

	if ( !DeviceIoControl(hI2C,
				IOCTL_IIC_SET_DELAY, 
				&uiIICDelay, sizeof(UINT32), 
				NULL, 0,
				&bytes, NULL) )
	{
		dwErr = GetLastError();
		RETAILMSG(1,(TEXT("IOCTL_IIC_SET_DELAY ERROR: %u \r\n"), dwErr));
		return;
	}

	if(ghBattTouchMutex==NULL)
	{
		ghBattTouchMutex = CreateMutex( NULL, FALSE, BATTERY_CAMERA_MUTEX );
	}

	//Init Interrupt for TSC2003 ,use eint0
	pGPIOReg->EINT0MASK  |= (1<<18) ;

	EnableTSC_Interrupt();

	pGPIOReg->GPLCON1 &= ~(0xF<<8);
	pGPIOReg->GPLCON1 |= (3<<8);     
	pGPIOReg->EINT0FLTCON2 = (pGPIOReg->EINT0FLTCON2 & ~(0xF<<8)) | (0xF<<8); //18 19 filter
	pGPIOReg->EINT0CON1 = (pGPIOReg->EINT0CON1& ~(0xF<<4))|(2<<4);  //18 19 Falling triggle
	pGPIOReg->EINT0PEND |= (1<<18);   //clear eint18 pending
	pGPIOReg->EINT0MASK &= ~(1<<18);  //unmask EINT18 interrupt


}


//TSC2003 write command
DWORD TSC_WriteCommand(UCHAR cmd)
{

	DWORD dwErr=0;
	DWORD bytes;
	IIC_IO_DESC IIC_Data;
	IIC_Data.SlaveAddress = TSC_WRITE;
	IIC_Data.Count    = 1;
	IIC_Data.Data = &cmd ;

	// use iocontrol to write
	if ( !DeviceIoControl(hI2C,
				IOCTL_IIC_WRITE, 
				&IIC_Data, sizeof(IIC_IO_DESC), 
				NULL, 0,
				&bytes, NULL) ) 
	{
		dwErr = GetLastError();
		RETAILMSG(1,(TEXT("TSC::IOCTL_IIC_WRITE ERROR: %u \r\n"), dwErr));
	}   

	if ( dwErr ) {
		RETAILMSG(1, (TEXT("TSC::I2CWrite ERROR: %u \r\n"), dwErr));
	}            
	return dwErr;

}
//TSC2003 Read data
DWORD TSC_ReadData( UCHAR cmd )
{
	DWORD dwErr=0;
	DWORD bytes;
	UCHAR pBuff[2];
	IIC_IO_DESC IIC_AddressData, IIC_Data;
	IIC_AddressData.SlaveAddress = TSC_WRITE;
	IIC_AddressData.Data = &cmd ;
	IIC_AddressData.Count = 1;

	IIC_Data.SlaveAddress = TSC_READ;
	IIC_Data.Data = pBuff;
	IIC_Data.Count =2;

	// use iocontrol to read    
	if ( !DeviceIoControl(hI2C,
				IOCTL_IIC_READ, 
				&IIC_AddressData, sizeof(IIC_IO_DESC), 
				&IIC_Data, sizeof(IIC_IO_DESC),
				&bytes, NULL) ) 
	{
		dwErr = GetLastError();
		RETAILMSG(1,(TEXT("TSC::IOCTL_IIC_READ ERROR: %u \r\n"), dwErr));
	}   
	return    ((pBuff[1]>>4)&0x0F)|(pBuff[0]<<4);
}

//Enable TSC2003 interrupt

void EnableTSC_Interrupt( void )
{
	Lock();
	TSC_WriteCommand( 0xD0);
	Unlock();
	//Sleep(8);
	Sleep(1);

}

//TSC2003 Get X and Y

BOOL TSC_GetXY(void)
{
	UCHAR i;
	Lock();
	for(i=0;i<MAX_SAMPLE_NUM;i++)
	{
		Delay();
		SampleData[i].x = TSC_ReadData(0xC4);   //x
		Delay();
		SampleData[i].y = TSC_ReadData(0xD4);   //y
		Delay();			
		SampleData[i].z1 = TSC_ReadData(0xE4);   //y
		//SampleData[i].z2 = TSC_ReadData(0xF4);   //y
		RETAILMSG(0,(TEXT("x= %x y= %x \r\n"),	SampleData[i].x,SampleData[i].y/*,SampleData[i].z1,SampleData[i].z2*/));
	}
	Unlock();
	return TRUE;
}
static BOOL TouchEvaluateSamples(
		TSC_XY *pulSampleVal,
		UINT32 ulMaxError,
		int pressMaxError,
		int *puEvaluatedValX,
		int *puEvaluatedValY)
{
	UINT16 i;
	UINT32 ulSumX = 0;
	UINT32 ulSumY = 0;
	UINT32 ulAverageX;
	UINT32 ulAverageY;
	INT32 iDiffx,iDiffy;
	BOOL bValidSamples = TRUE;
	TSC_XY *pulSample;
	pulSample = pulSampleVal;
	TSC_XY tmp_pulSample[MAX_SAMPLE_NUM];

	for (i=0; i<MAX_SAMPLE_NUM; i++)
	{	
		ulSumX += pulSample[i].x;
		ulSumY += pulSample[i].y;
	}
	ulAverageX = ulSumX / (MAX_SAMPLE_NUM);
	ulAverageY = ulSumY / (MAX_SAMPLE_NUM);
	RETAILMSG(0, (TEXT("[CDC TCH] TouchEvaluateSamples==>\r\n")));
	*puEvaluatedValX = ulAverageX;
	*puEvaluatedValY = ulAverageY;
	#if (LCD_TYPE == LCD_TYPE_PORTRAIT)
	int orig_x = *puEvaluatedValX;
	int orig_y = *puEvaluatedValY;
	*puEvaluatedValX = orig_y;
	*puEvaluatedValY = 640 - orig_x - 1;
	#endif
	RETAILMSG(0,(TEXT("X=%x Y=%x\r\n"),*puEvaluatedValX,*puEvaluatedValY));
	for (i=0; i<MAX_SAMPLE_NUM; i++)  
	{
		iDiffx = pulSample[i].x - ulAverageX;
		iDiffx = (iDiffx>0) ? iDiffx : (-iDiffx);
		iDiffy = pulSample[i].y - ulAverageY;
		iDiffy = (iDiffy>0) ? iDiffy : (-iDiffy);
		if ((UINT32)iDiffx >= ulMaxError ||(UINT32)iDiffy >= ulMaxError)
		{
			bValidSamples = FALSE;
			RETAILMSG(0, (TEXT("[CDC TCH] Invalid X sameples: Diff:%d\r\n"), iDiffx));
			break;
		}
	}
	RETAILMSG(0, (TEXT("[CDC TCH] TouchEvaluateSamples<==\r\n")));
	return (bValidSamples);
}

#define DETAIL_SAMPLING
// Sample touch data
BOOL PDDSampleTouchScreen(int *x,int *y)
{
	int j,k;
	DWORD temp,dx,dy;
	BOOL TmpStateFlags=FALSE;
	// read touch screen sample of X and Y
	if(TSC_GetXY())
	{
		return TouchEvaluateSamples(SampleData, 85/*35*/,0x2000, x,y);
	}
	return TmpStateFlags;
}


BOOL GetTouchStatus(void)
{
	DWORD Z;
	Lock();
	Z = TSC_ReadData(0xE4);
	Unlock();

	RETAILMSG(0,(TEXT("Z==%x\r\n"), Z));
	EnableTSC_Interrupt();   //Enable TSC2003 Interrup
	if(Z<MAX_NO_Z ) return  TRUE;
	else return FALSE;

}

