#include <windows.h>

#include <nkintr.h>
#include <pm.h>
#include "pmplatform.h"
#include <ceddk.h>
#include <S3c6410.h>
#include <bsp.h>
#include "iic.h"
#include "setting.h"
#include "Module.h"
#include "smdk6410_camera.h"

//--------------------------------------------------------------------------------------
//Definitions---------------------------------------------------------------------------
#define MSG_ERROR		1
#define dbg 1
#define OV5642_CAMERA_WRITE	(0x78 + 0)
#define OV5642_CAMERA_READ 	(0x78 + 1)

#define DEFAULT_MODULE_ITUXXX		CAM_ITU601
#define DEFAULT_MODULE_YUVORDER		CAM_ORDER_YCBYCR
#define DEFAULT_MODULE_HSIZE		640
#define DEFAULT_MODULE_VSIZE		480
#define DEFAULT_MODULE_HOFFSET		0
#define DEFAULT_MODULE_VOFFSET		0
#define DEFAULT_MODULE_UVOFFSET		CAM_UVOFFSET_0
#define DEFAULT_MODULE_CLOCK_OV5642	24000000
#define DEFAULT_MODULE_CLOCK_NOON	6000000
#define DEFAULT_MODULE_CODEC		CAM_CODEC_422
#define DEFAULT_MODULE_HIGHRST		0
#define DEFAULT_MODULE_INVPCLK		0
#define	DEFAULT_MODULE_INVVSYNC		0 
#define DEFAULT_MODULE_INVHREF 		0

static HANDLE ghBattTouchMutex;
#define BATTERY_CAMERA_MUTEX  TEXT("Battery Camera Mutex")

//--------------------------------------------------------------------------------------
//Variables-----------------------------------------------------------------------------
static MODULE_DESCRIPTOR 			gModuleDesc;
static HANDLE						hI2C = NULL;   // I2C Bus Driver


static void Delay()
{
	int i,j=0;
	for(i=0;i < 1000; i++) { j++; }
}

//--------------------------------------------------------------------------------------
//Functions-----------------------------------------------------------------------------
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


//Module 1------------------------------------------------------------------------------
BYTE OV5642ReadReg(USHORT reg)
{
    DWORD dwErr=0;
    DWORD bytes;
    BYTE res=0;
    UCHAR udata[2];
    IIC_IO_DESC IIC_AddressData, IIC_Data;

    udata[0] = reg>>8;
    udata[1] = reg&0xff;
    
    IIC_AddressData.SlaveAddress = OV5642_CAMERA_WRITE;
    IIC_AddressData.Data = udata;
    IIC_AddressData.Count = 2;
    
    IIC_Data.SlaveAddress = OV5642_CAMERA_READ;
    IIC_Data.Data = &res;
    IIC_Data.Count =1;
    
    // use iocontrol to read    
    if ( !DeviceIoControl(hI2C,
                          IOCTL_IIC_READ, 
                          &IIC_AddressData, sizeof(IIC_IO_DESC), 
                          &IIC_Data, sizeof(IIC_IO_DESC),
                          &bytes, NULL) ) 
    {
        dwErr = GetLastError();
        RETAILMSG(MSG_ERROR,(TEXT("OV5642 OV5642_CAMERA_READ ERROR: reg=0x%x \r\n"), reg));
    }   

    
    if ( !dwErr ) {

    } else {        
        RETAILMSG(dbg,(TEXT("I2CRead ERROR: %u \r\n"), dwErr));
    }            
    RETAILMSG(dbg,(TEXT("I2CRead : %x=%x \r\n"), reg,*(IIC_Data.Data)));
    return *(BYTE *)(IIC_Data.Data);
}

BOOL OV5642WriteReg( USHORT reg,  BYTE data )
{

    UCHAR udata[3];
    DWORD dwErr=1;
    DWORD bytes;
    IIC_IO_DESC IIC_Data;
    
    //RETAILMSG(1,(TEXT("OV5642WriteReg  %x  : %x\r\n"),reg,data));    
    udata[0] = reg>>8;
    udata[1] = reg&0xff;
    udata[2] = data;
	
    IIC_Data.SlaveAddress = OV5642_CAMERA_WRITE;
    IIC_Data.Data = udata;
    IIC_Data.Count	= 3;
    
    // use iocontrol to write
    if ( !DeviceIoControl(hI2C,
                          IOCTL_IIC_WRITE, 
                          &IIC_Data, sizeof(IIC_IO_DESC), 
                          NULL, 0,
                          &bytes, NULL) ) 
    {
    	    dwErr = 0;
        	RETAILMSG(MSG_ERROR,(TEXT("%x OV5642 IOCTL_IIC_WRITE ERROR: reg=0x%x  data=0x%x \r\n"),GetTickCount(), reg,data));
    		
    }   

    return dwErr;
}


BOOL OV5642WriteSeq( OVCamRegValue *pStream )
{
    OVCamRegValue *pRegValue=pStream;
	DWORD tick=GetTickCount();
    RETAILMSG(1,(TEXT("Begin %x ##OV5642WriteSeq %x %x \r\n"),tick,pRegValue->SubAddr, pRegValue->Value));

    while (!(pRegValue->SubAddr==0x00 && pRegValue->Value==0x00))
    {
    	//RETAILMSG(1,(TEXT("Write %x %x \r\n"), pRegValue->SubAddr, pRegValue->Value));
        if(OV5642WriteReg( pRegValue->SubAddr, pRegValue->Value))
			pRegValue++;
		Sleep(1);
    }
    RETAILMSG(1,(TEXT("End %d used ==OV5642 WriteSeq %x %x \r\n"), GetTickCount()-tick,pRegValue->SubAddr, pRegValue->Value));

    return TRUE;
}



int ModuleInit()
{
	DWORD dwErr = ERROR_SUCCESS;
	UINT32  IICClock = 400000;
	UINT32	uiIICDelay;
	BYTE 	bFilterEn;
	DWORD bytes;
	RETAILMSG(dbg,(TEXT("ModuleInit  %x\n"),hI2C));
	// Initialize I2C Driver
	if(hI2C == NULL)
	{
		hI2C = CreateFile( L"IIC0:",
	                             GENERIC_READ|GENERIC_WRITE,
	                             FILE_SHARE_READ|FILE_SHARE_WRITE,
	                             NULL, OPEN_EXISTING, 0, 0);
	                   
	      if ( INVALID_HANDLE_VALUE == hI2C ) {
	        dwErr = GetLastError();
	        RETAILMSG(MSG_ERROR, (TEXT("Error %d opening device '%s' \r\n"), dwErr, L"I2C0:" ));
	        return FALSE;
	      }
	}
#if 0	
    // use iocontrol to write
    if ( !DeviceIoControl(hI2C,
                          IOCTL_IIC_SET_CLOCK, 
                          &IICClock, sizeof(UINT32), 
                          NULL, 0,
                          &bytes, NULL) ) 
    {
        dwErr = GetLastError();
        RETAILMSG(MSG_ERROR,(TEXT("IOCTL_IIC_SET_CLOCK ERROR: %u \r\n"), dwErr));
        return FALSE;
    }       
    
	uiIICDelay = Clk_15;
	if ( !DeviceIoControl(hI2C,
                      IOCTL_IIC_SET_DELAY, 
                      &uiIICDelay, sizeof(UINT32), 
                      NULL, 0,
                      &bytes, NULL) )
	{
        dwErr = GetLastError();
        RETAILMSG(MSG_ERROR,(TEXT("IOCTL_IIC_SET_DELAY ERROR: %u \r\n"), dwErr));
        return FALSE;
	}

	bFilterEn = 1;
  	if ( !DeviceIoControl(hI2C,
                      IOCTL_IIC_SET_FILTER, 
                      &bFilterEn, sizeof(UINT32), 
                      NULL, 0,
                      &bytes, NULL) )
	{
        dwErr = GetLastError();
        RETAILMSG(MSG_ERROR,(TEXT("IOCTL_IIC_SET_FILTER ERROR: %u \r\n"), dwErr));
        return FALSE;
	}
  #endif
	if(ghBattTouchMutex==NULL)
	{
		ghBattTouchMutex = CreateMutex( NULL, FALSE, BATTERY_CAMERA_MUTEX );
	}

    return TRUE;
}

void ModuleDeinit()
{	
	RETAILMSG(dbg,(TEXT("ModuleDeinit\n")));
	if(ghBattTouchMutex)
	    {
			CloseHandle( ghBattTouchMutex );
			ghBattTouchMutex = NULL ;
	    }   
	CloseHandle(hI2C);
}

void ModuleGetFormat(MODULE_DESCRIPTOR &outModuleDesc)
{
	memcpy(&outModuleDesc, &gModuleDesc, sizeof(MODULE_DESCRIPTOR));
	//RETAILMSG(1,(TEXT("[CAM] ModuleGetFormat Clock=%d   Codec=%d   Order422=%d  SourceHOffset=%d  SourceHSize=%d  SourceVOffset=%d  SourceVSize=%d  UVOffset=%d\r\n"),outModuleDesc.Clock,outModuleDesc.Codec,outModuleDesc.Order422,outModuleDesc.SourceHOffset,outModuleDesc.SourceHSize,outModuleDesc.SourceVOffset,outModuleDesc.SourceVSize,outModuleDesc.UVOffset));
}
static int Viewfinder_Update()
{
	Lock();
	RETAILMSG(dbg,(L"#Viewfinder_Update size w=%d  \r\n",gModuleDesc.SourceHSize));	
	//still,   first update regiters relates to the array output	
	if( gModuleDesc.Ov5642ArrayMode == OV5642_ARRAY_QSXGA )
	{
		RETAILMSG(dbg,(L"#Viewfinder_Update s_CameraPre2Cap finished  \r\n"));
		if(gModuleDesc.SourceHSize != 640)
		{
			OV5642WriteSeq(s_Capture);
			//OV5642WriteReg(0x3818,0xa8/*0xc1*/);
			//OV5642WriteReg(0x3621,0x29/*0x87*/);
			//OV5642WriteReg(0x3801,0xaa/*0x8a 0x87*/);
			//OV5642WriteReg(0x3503,0x00/*0x8a 0x87*/);
			//OV5642WriteReg(0x3010,0x20);
			//OV5642WriteReg(0x3815,0x02);
			RETAILMSG(1,(L"2====>Preview->STILL >>>>>>>>>>>>>>>>>\r\n"));	
		}
	}
	else if( gModuleDesc.Ov5642ArrayMode == OV5642_ARRAY_VGA )//preview or capture
	{
		RETAILMSG(1,(L"1====>STILL->Preview <<<<<<<<<<<<<<<<<\r\n"));	
	}
	else
	{
		Unlock();
		return 0;
	}
	Unlock();
	return 1;
}

static int regs_ae_start()
{
	Lock();
	OV5642WriteReg(0x3503, 0x00);
	Unlock();
	return 0;
}


static int regs_ae_stop()
{
	Lock();
	OV5642WriteReg(0x3503, 0x07);	
	Unlock();
	return 0;
}


int Module1Init()
{
	RETAILMSG(dbg,(TEXT("Module1Init\r\n")));	
    gModuleDesc.ITUXXX = DEFAULT_MODULE_ITUXXX;
    gModuleDesc.UVOffset = DEFAULT_MODULE_UVOFFSET;
    gModuleDesc.SourceHSize = DEFAULT_MODULE_HSIZE;
    gModuleDesc.Order422 = DEFAULT_MODULE_YUVORDER;
    gModuleDesc.SourceVSize = DEFAULT_MODULE_VSIZE;
    gModuleDesc.Clock = DEFAULT_MODULE_CLOCK_OV5642;
    gModuleDesc.Codec = DEFAULT_MODULE_CODEC;
    gModuleDesc.HighRst = DEFAULT_MODULE_HIGHRST;
    gModuleDesc.SourceHOffset = DEFAULT_MODULE_HOFFSET;
    gModuleDesc.SourceVOffset = DEFAULT_MODULE_VOFFSET;
    gModuleDesc.InvPCLK = DEFAULT_MODULE_INVPCLK;
    gModuleDesc.InvVSYNC = DEFAULT_MODULE_INVVSYNC;
    gModuleDesc.InvHREF = DEFAULT_MODULE_INVHREF;
    gModuleDesc.Ov5642ArrayMode = OV5642_ARRAY_QXGA;		//xga,for preview   	//qxga,for capture
    gModuleDesc.Ov5642StreamMode = STREAM_IDLE;
	gModuleDesc.CurZoom = 0;
    ModuleInit();
	
    return TRUE;
}


BOOL Module1SetImageSize(UINT16 nWidth, UINT16 nHeight)
{
	
	RETAILMSG(dbg,(TEXT("Module1SetImageSize  %d   %d\r\n"),nWidth,nHeight));	
	if(nWidth == 320)
	{
		gModuleDesc.SourceHSize = 640;
		gModuleDesc.SourceVSize = 480;
	}else
	{
		gModuleDesc.SourceHSize = nWidth;
		gModuleDesc.SourceVSize = nHeight;
	}
	Unlock();
	return TRUE;
}

BOOL Module1PreviewEnable(int Format)
{
	RETAILMSG(dbg,(TEXT("Module1PreviewEnable\r\n")));	
	gModuleDesc.Ov5642ArrayMode = OV5642_ARRAY_VGA;
	Viewfinder_Update();
	regs_ae_start();
	return TRUE;
}

BOOL Module1ArmGo ()
{
	BYTE CamID=0,TemVal=0,TemVal1=0;
	int count=0;
	Lock();
	while(CamID!=0x56)
	{
		CamID = OV5642ReadReg(0x300a);
		RETAILMSG(1,(TEXT("Module1ArmGo sensor id= %x\n"),CamID));
		if(count++>6)
		{
			Unlock();
			return FALSE;
		}
	}
	Sleep(10);
	RETAILMSG(dbg,(L"Module1ArmGo s_Camera Sensor reset --------------------  \r\n"));	
    OV5642WriteSeq(s_preview);	
	Unlock();
	RETAILMSG(dbg,(L"Module1ArmGo s_CameraInit para finished --------------------  \r\n"));
	return TRUE;
}



BOOL Module1CaptureMode()
{
	UINT64 exposure = 0;

	RETAILMSG(dbg,(TEXT("Module1CaptureMode\r\n")));	
	regs_ae_stop();
	gModuleDesc.Ov5642ArrayMode = OV5642_ARRAY_QSXGA;
	Viewfinder_Update();
	
	regs_ae_stop();
	Sleep(256);//256 64
	return TRUE;
}


