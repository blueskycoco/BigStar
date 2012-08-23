//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//

//
// This module contains a stub implementation of the battery PDD.  OEMs
// that want to support the battery APIs on their platform can copy this
// file to their platform and link it into their own version of the power
// manager DLL.
//
// If the platform-specific power manager provides its own version of these
// entry points, this module will not be pulled into the link map from
// the pm_battapi library.
//

//--------------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------------
#include <battimpl.h>
#include <ceddk.h>
#include <bsp.h>
#include <Drvlib.h>
#include "iic.h"
#define DBGBAT 0
static HANDLE                        hI2C;   // I2C Bus Driver
volatile static BSP_ARGS *v_pBSPArgs;

unsigned __int8 GetChargePercent(void);

#define MUTEX_TIMEOUT 5000
#define ADC_FREQ 2500000
#define CHARGE_LOW_LIMIT   250
#define CHARGE_HIGH_LIMIT  1475
static HANDLE ghMutex;
int ReadAdc( BYTE ch );
static volatile S3C6410_GPIO_REG * g_pGPIOReg;
static volatile S3C6410_ADC_REG  * g_pADCReg;

#define BATTERY_CAMERA_MUTEX  TEXT("Battery Camera Mutex")
#define BAT_SLAVE_ID  0x6C
#define BAT_WRITE    (BAT_SLAVE_ID | 0x0)
#define BAT_READ     (BAT_SLAVE_ID | 0x1)
#define MIN_BATT_MIN_LEVEL 2457
#define MAX_BATT_MAX_LEVEL 3360   //3439
static HANDLE ghBattTouchMutex;
//-------------------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------------------
void Delay(UINT32 count)
{
    volatile int i, j = 0;
    volatile static int loop = S3C6410_ACLK / 100000;

    for(; count > 0; count--)
        for(i = 0; i < loop; i++) { j++; }
}

// this routine takes the battery mutex
DWORD Lock(void)
{
	DWORD dwStatus;

    DEBUGCHK(ghBattTouchMutex != NULL);

    dwStatus = WaitForSingleObject(ghBattTouchMutex, INFINITE);
    if(dwStatus == WAIT_OBJECT_0) {
        dwStatus = ERROR_SUCCESS;
    } else {
        dwStatus = GetLastError();
    //    DEBUGCHK(dwStatus != ERROR_SUCCESS);
    }
   // DEBUGCHK(dwStatus == ERROR_SUCCESS);
	//RETAILMSG(1,(TEXT("BT Locked..............................error:%d\r\n"),dwStatus));
    return dwStatus;
}

DWORD Unlock(void)
{
	DWORD dwStatus = ERROR_SUCCESS;
    BOOL fOk;
   // SETFNAME(_T("UnlockBattery"));

   // DEBUGCHK(ghBattTouchMutex != NULL);

    fOk = ReleaseMutex(ghBattTouchMutex);
    if(!fOk) {
        dwStatus = GetLastError();
   //     DEBUGCHK(dwStatus != ERROR_SUCCESS);
    }
  //  DEBUGCHK(dwStatus == ERROR_SUCCESS);
	//RETAILMSG(1,(TEXT("BT Unlocked.............................error:%d\r\n"),dwStatus));
    return dwStatus;
}

DWORD
LockBattery(void)
{
    DWORD dwStatus;
    SETFNAME(_T("LockBattery"));

    DEBUGCHK(ghMutex != NULL);

    dwStatus = WaitForSingleObject(ghMutex, MUTEX_TIMEOUT);
    if(dwStatus == WAIT_OBJECT_0) {
        dwStatus = ERROR_SUCCESS;
    } else {
        dwStatus = GetLastError();
        DEBUGCHK(dwStatus != ERROR_SUCCESS);
    }
    
    DEBUGMSG(dwStatus != ERROR_SUCCESS && ZONE_WARN,
        (_T("%s: WaitForSingleObject() failed %d\r\n"), pszFname, 
        GetLastError()));
    DEBUGCHK(dwStatus == ERROR_SUCCESS);

    return dwStatus;
}

// this routine releases the battery mutex
DWORD
UnlockBattery(void)
{
    DWORD dwStatus = ERROR_SUCCESS;
    BOOL fOk;
    SETFNAME(_T("UnlockBattery"));

    DEBUGCHK(ghMutex != NULL);

    fOk = ReleaseMutex(ghMutex);
    if(!fOk) {
        dwStatus = GetLastError();
        DEBUGCHK(dwStatus != ERROR_SUCCESS);
    }
    
    DEBUGMSG(dwStatus != ERROR_SUCCESS && ZONE_WARN,
        (_T("%s: ReleaseMutex() failed %d\r\n"), pszFname, GetLastError()));
    DEBUGCHK(dwStatus == ERROR_SUCCESS);

    return dwStatus;
}

DWORD
HW_WriteRegisters(
    PUCHAR pBuff,   // Optional buffer
    DWORD nRegs     // number of registers
    )
{
    DWORD dwErr=0;
    DWORD bytes;
    IIC_IO_DESC IIC_Data;
    
    IIC_Data.SlaveAddress = BAT_WRITE;
    IIC_Data.Count    = nRegs;
    IIC_Data.Data = pBuff;
    
    // use iocontrol to write
    if ( !DeviceIoControl(hI2C,
                          IOCTL_IIC_WRITE, 
                          &IIC_Data, sizeof(IIC_IO_DESC), 
                          NULL, 0,
                          &bytes, NULL) ) 
    {
        dwErr = GetLastError();
    }   

   
    return dwErr;
}


DWORD
HW_ReadRegisters(
    PUCHAR pBuff,       // Optional buffer
    UCHAR StartReg,     // Start Register
    DWORD nRegs         // Number of Registers
    )
{
    DWORD dwErr=1;
    DWORD bytes;
    IIC_IO_DESC IIC_AddressData, IIC_Data;
	
    IIC_AddressData.SlaveAddress = BAT_WRITE;
    IIC_AddressData.Data = &StartReg;
    IIC_AddressData.Count = 1;
    
    IIC_Data.SlaveAddress = BAT_READ;
    IIC_Data.Data = pBuff;
    IIC_Data.Count = nRegs;
    
    // use iocontrol to read    
    if ( !DeviceIoControl(hI2C,
                          IOCTL_IIC_READ, 
                          &IIC_AddressData, sizeof(IIC_IO_DESC), 
                          &IIC_Data, sizeof(IIC_IO_DESC),
                          &bytes, NULL) ) 
    {
    	dwErr = GetLastError();
		RETAILMSG(1,(TEXT("BAT::IOCTL_IIC_READ ERROR: %u \r\n"), dwErr));
        dwErr = 0;
    }   

    
    return dwErr;
}
int ReadAdc( BYTE ch )
{	
	int	preScaler = S3C6410_PCLK/ADC_FREQ -1;
	DWORD rADCCON_save = g_pADCReg->ADCCON;
	int iRet;

	/*	
	if(ghBattTouchMutex )
	{
	WaitForSingleObject(ghBattTouchMutex, MUTEX_TIMEOUT);
	}
	*/
	g_pADCReg->ADCCON =(1<<16)|(1<<14)|(preScaler<<6)|(ch<<3);   //channel 0

	g_pADCReg->ADCCON|=0x1;   //start ADC
	while(g_pADCReg->ADCCON & 0x1);	//check if Enable_start is low
	while(!(g_pADCReg->ADCCON & 0x8000));	//check if EC(End of Conversion) flag is high

	iRet = (int)g_pADCReg->ADCDAT0 & 0x3ff;
	g_pADCReg->ADCCON = rADCCON_save;
	/*
	if(ghBattTouchMutex)
	{
	ReleaseMutex(ghBattTouchMutex);
	}
	*/
	return iRet; 

}
unsigned __int8 ChargePercent;

DWORD Bat_ReadData()
{
	DWORD dwErr=0;
	UCHAR cmd[2];
	DWORD bytes;
	UCHAR pBuff[2];
	IIC_IO_DESC IIC_AddressData, IIC_Data;

	//RETAILMSG(1,(TEXT("HW_ReadRegisters++ \r\n"))); 
	cmd[0]=0x02;
	cmd[1]=0x03;
	IIC_AddressData.SlaveAddress = 0x6c;
	IIC_AddressData.Data = cmd;
	IIC_AddressData.Count = 2;

	IIC_Data.SlaveAddress = 0x6d;
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
		RETAILMSG(1,(TEXT("BAT::IOCTL_IIC_READ ERROR: %u \r\n"), dwErr));
	}   



	// if(dwErr)
	// {        
	//     RETAILMSG(1,(TEXT("I2CRead ERROR: %u \r\n"), dwErr));
	// }            


	RETAILMSG(1,(TEXT("Bat_ReadData %x  %x\r\n"),pBuff[0],pBuff[1])); 

	return    ((pBuff[1]<<8)|(pBuff[0]));
}

//------------------------------------------------------------------------------------------------------------
// Function: GetChargePercent
// 
// Purpose:  Get Charge capacity from battery check hardware
// Returns:  unsigned __int8
//
//-------------------------------------------------------------------------------------------------------------
unsigned __int8 GetChargePercent(void)
{
	static unsigned __int8 ChargePercetValue; // sample data
	UCHAR SOCMSB=0;
	UCHAR SOCLSB=0;
	UCHAR VCELLMSB=0;
	UCHAR VCELLLSB=0;
	UCHAR StartRegs=0x0B;
	USHORT VCELL;
	UCHAR atemp=0;
	static int SkipFirstTime = 18;
	RETAILMSG(DBGBAT,(TEXT("++GetChargePercent\r\n")));	
	//ChargePercetValue = 80;  //Sample data
	if(SkipFirstTime >= 5)
	{
		SkipFirstTime = 0; 
		Lock();
		HW_ReadRegisters(&VCELLMSB,0x02,1);
		//		Sleep(1);
		Delay(5);
		HW_ReadRegisters(&VCELLLSB,0x03,1);
		Delay(5);
		//VCELL=Bat_ReadData();
		//		Sleep(1);
		//		HW_ReadRegisters(&SOCMSB,0x04,1);
		//		Delay(5);
		//		HW_ReadRegisters(&SOCLSB,0x05,1);

		Unlock();
		VCELL = (VCELLMSB<<4) + (VCELLLSB>>4);


		//		atemp = ((SOCLSB>>7)&0x1)*50+((SOCLSB>>6)&0x1)*25+((SOCLSB>>5)&0x1)*13+((SOCLSB>>4)&0x1)*7+((SOCLSB>>3)&0x1)*4;
		//		RETAILMSG(1,(TEXT("The battary quanlity adc value is %d  \r\n"),VCELL));
		///		RETAILMSG(1,(TEXT("The quantity is %d.%d  \r\n"),SOCMSB,atemp));
		//		RETAILMSG(1,(TEXT("Am I called???: \r\n")));
		//		BatteryLifePercent = Quantity*100/0xFF;
		//		
		ChargePercetValue = (VCELL-MIN_BATT_MIN_LEVEL)*100/(MAX_BATT_MAX_LEVEL-MIN_BATT_MIN_LEVEL);
		RETAILMSG(0,(TEXT("%x GetChargePercent %x %x %x\r\n"),GetTickCount(),ChargePercetValue,VCELLMSB,VCELLLSB));
	}
	SkipFirstTime++;
	//Bat_ReadData();
	RETAILMSG(0,(TEXT("GetChargePercent %x %x %x\r\n"),ChargePercetValue,VCELLMSB,VCELLLSB));
	return 	ChargePercetValue;
}



//------------------------------------------------------------------------------------------------------------
// Function: InitBatteryDriver
// 
// Purpose:  Initialises the battery driver and allocates the necessary memory.
// Returns:  void
//
//-------------------------------------------------------------------------------------------------------------

void InitBatteryDriver()
{
	UINT32  IICClock = 80000;
	UINT32  uiIICDelay;
	DWORD dwErr,bytes;

	RETAILMSG(DBGBAT,(TEXT("++1InitBatteryDriver\r\n"))); 
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

	uiIICDelay = Clk_15;

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
	//mf Bat_ReadData();
	if(g_pGPIOReg == NULL) 
	{
	    	g_pGPIOReg = (volatile S3C6410_GPIO_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_GPIO, sizeof(S3C6410_GPIO_REG), FALSE);
	    	if(g_pGPIOReg == NULL) 
	    	{
			RETAILMSG(0,(TEXT("BATTERY ALLOC: GPOI failed\r\n")));
			}
	}
	if(g_pADCReg == NULL) 
	{
	    	g_pADCReg = (volatile S3C6410_ADC_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_ADC, sizeof(S3C6410_ADC_REG), FALSE);
	    	if(g_pADCReg == NULL) {
		    	RETAILMSG(0,(TEXT("BATTERY ALLOC: ADC failed\r\n")));
		    }
	}
	if(ghBattTouchMutex==NULL)
	{
		ghBattTouchMutex = CreateMutex( NULL, FALSE, BATTERY_CAMERA_MUTEX );
	}
	g_pGPIOReg->GPCCON  = (g_pGPIOReg->GPCCON & ~(0xF<<12) ) | (0<<12); //GPC3 as input
	RETAILMSG(1,(TEXT("BATTERY Test 2\r\n")));
	//Now, SMDK6410 have no Battery Check hardware
	//Implement initialized code for Battery driver. 
	RETAILMSG(DBGBAT,(TEXT("--InitBatteryDriver\r\n")));        

}

//------------------------------------------------------------------------------------------------------------
// Function: IsACOnline
// 
// Purpose:  Simply indicates whether we are running out of AC or out of Battery.
// Returns:  TRUE indicates we are running out of the AC wall power supply.
//           FALSE indicates we are running out of Battery.
//
//-------------------------------------------------------------------------------------------------------------
BOOL WINAPI IsACOnline(void)
{
	BOOL AcOnlineStatus = FALSE;  //TRUE:AC FALSE:DC
	int iAdLevel;
	RETAILMSG(DBGBAT,(TEXT("++IsACOnline\r\n"))); 	
	// Implement Check code for whether AC or Battery. 
	// v_pBSPArgs->bPowerStatus = AcOnlineStatus;
	iAdLevel = ReadAdc(0);
	//	RETAILMSG(1,(TEXT("The adc channel 0 value is %d  \r\n"),iAdLevel));
	if((iAdLevel>CHARGE_LOW_LIMIT)&&(iAdLevel<CHARGE_HIGH_LIMIT))   //Judge the charging situation
	{
		AcOnlineStatus        =  AC_LINE_ONLINE;
		RETAILMSG(0,(TEXT("AC Online\r\n"))); 	
	}	
	else									
	{
		AcOnlineStatus        =  AC_LINE_OFFLINE;
		RETAILMSG(0,(TEXT("AC Not Online\r\n"))); 	
	}
	RETAILMSG(DBGBAT,(TEXT("--IsACOnline\r\n"))); 
	return AcOnlineStatus;
}

BOOL WINAPI 
BatteryPDDInitialize(LPCTSTR pszRegistryContext)
{
    BOOL fOk = TRUE;
    RETAILMSG(1,(TEXT("++BatteryPDDInitialize\r\n"))); 
    
    v_pBSPArgs = (volatile BSP_ARGS *)DrvLib_MapIoSpace(IMAGE_SHARE_ARGS_PA_START, sizeof(BSP_ARGS), FALSE);
	if (v_pBSPArgs == NULL)
	{
		RETAILMSG(1, (TEXT("#####Battery: v_pBSPArgs DrvLib_MapIoSpace() Failed \n\r")));
		return FALSE;
	}
    UNREFERENCED_PARAMETER(pszRegistryContext);
    
    RETAILMSG(1,(TEXT("--BatteryPDDInitialize\r\n"))); 

    return fOk;
}

void WINAPI 
BatteryPDDDeinitialize(void)
{
    RETAILMSG(DBGBAT,(TEXT("++BatteryPDDDeinitialize\r\n"))); 
    if(ghBattTouchMutex)
    {
		CloseHandle( ghBattTouchMutex );
		ghBattTouchMutex = NULL ;
    }   
	CloseHandle(hI2C);
    RETAILMSG(DBGBAT,(TEXT("--BatteryPDDDeinitialize\r\n")));    
}

void WINAPI 
BatteryPDDResume(void)
{
    RETAILMSG(DBGBAT,(TEXT("++BatteryPDDResume\r\n"))); 
    
    RETAILMSG(DBGBAT,(TEXT("--BatteryPDDResume\r\n")));  
}

void WINAPI 
BatteryPDDPowerHandler(BOOL bOff)
{
    RETAILMSG(DBGBAT,(TEXT("++BatteryPDDPowerHandler\r\n"))); 
    
    RETAILMSG(DBGBAT,(TEXT("--BatteryPDDPowerHandler\r\n"))); 
}

// This routine obtains the most current battery/power status available
// on the platform.  It fills in the structures pointed to by its parameters
// and returns TRUE if successful.  If there's an error, it returns FALSE.
BOOL WINAPI
BatteryPDDGetStatus(
                    PSYSTEM_POWER_STATUS_EX2 pstatus,
                    PBOOL pfBatteriesChangedSinceLastCall
                    )
{

    // this function is used to report the battery status
    // now, temperature, and terminal voltage
    unsigned __int8 sample;
    BOOL PowerStatus;
    static BOOL first_time = TRUE;
    BYTE BatteryFlag = 0;

	RETAILMSG(DBGBAT,(TEXT("++BatteryPDDGetStatus\r\n")));
    if(first_time)
    {
        InitBatteryDriver();
        ChargePercent = 100;
        first_time=FALSE;
    }

    PowerStatus = IsACOnline();
    sample = GetChargePercent();
    if (sample > 100) {
        sample = 100;
    }

    if (PowerStatus)
    {
        pstatus->ACLineStatus = AC_LINE_ONLINE;
        ChargePercent = sample;
        BatteryFlag |= BATTERY_FLAG_CHARGING;
    } 
    else
    {
        pstatus->ACLineStatus = AC_LINE_OFFLINE;
        ChargePercent = sample;
    }


    // Level Indicator 
    if(ChargePercent >= /*65*/30)
        BatteryFlag |= BATTERY_FLAG_HIGH;
    else if ((ChargePercent < /*65*/30) && (ChargePercent >= 20))
        BatteryFlag |= BATTERY_FLAG_LOW;
    else
        BatteryFlag |= BATTERY_FLAG_CRITICAL;

    pstatus->BatteryFlag                = BatteryFlag;
    pstatus->BatteryLifePercent         = ChargePercent;
    pstatus->Reserved1                  = 0;
    pstatus->BatteryLifeTime            = BATTERY_LIFE_UNKNOWN;
    pstatus->BatteryFullLifeTime        = BATTERY_LIFE_UNKNOWN;

    pstatus->Reserved2                  = 0;
    pstatus->BackupBatteryFlag          = BATTERY_FLAG_UNKNOWN;
    pstatus->BackupBatteryLifePercent   = 0;
    pstatus->Reserved3                  = 0;
    pstatus->BackupBatteryLifeTime      = BATTERY_LIFE_UNKNOWN;
    pstatus->BackupBatteryFullLifeTime  = BATTERY_LIFE_UNKNOWN;

    pstatus->BatteryChemistry           = BATTERY_CHEMISTRY_LION;
    pstatus->BatteryVoltage             = 1200;   //Sample Data
    pstatus->BatteryCurrent             = 0;
    pstatus->BatteryAverageCurrent      = 0;
    pstatus->BatteryAverageInterval     = 0;
    pstatus->BatterymAHourConsumed      = 0;
    pstatus->BatteryTemperature         = 0;
    pstatus->BackupBatteryVoltage       = 0;
   
   *pfBatteriesChangedSinceLastCall = FALSE;
   
	RETAILMSG(DBGBAT,(TEXT("--BatteryPDDGetStatus\r\n")));
	
    return (TRUE);
}



// This routine indicates how many battery levels will be reported
// in the BatteryFlag and BackupBatteryFlag fields of the PSYSTEM_POWER_STATUS_EX2
// filed in by BatteryPDDGetStatus().  This number ranges from 0 through 3 --
// see the Platform Builder documentation for details.  The main battery
// level count is reported in the low word of the return value; the count 
// for the backup battery is in the high word.
LONG
BatteryPDDGetLevels(
    void
        )
{
    LONG lLevels = MAKELONG (3 /* main battery levels   */,  
                             3 /* backup battery levels */);
	
	RETAILMSG(DBGBAT,(TEXT("++BatteryPDDGetLevels\r\n")));

    RETAILMSG(DBGBAT, (TEXT("(%d main levels, %d backup levels)\r\n"),LOWORD(lLevels), HIWORD(lLevels)));

	RETAILMSG(DBGBAT,(TEXT("--BatteryPDDGetLevels\r\n")));
    return lLevels;
}



// This routine returns TRUE to indicate that the pfBatteriesChangedSinceLastCall
// value filled in by BatteryPDDGetStatus() is valid.  If there is no way to
// tell that the platform's batteries have been changed this routine should
// return FALSE.
BOOL
BatteryPDDSupportsChangeNotification(
    void
        )
{
    BOOL fSupportsChange = FALSE;
    SETFNAME(_T("BatteryPDDPowerHandler"));

    DEBUGMSG(ZONE_PDD, (_T("%s: returning %d\r\n"), pszFname, fSupportsChange));

    return fSupportsChange;
}

