/**************************************************************************************
*
*	Project Name : IIC Driver
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is PDD layer for IIC Samsung driver.
*
*--------------------------------------------------------------------------------------
*
*	File Name : s3c6410_iic_lib.cpp
*
*	File Description : This file implements PDD layer functions
*
*	Author : JeGeon.Jung
*	Dept. : AP Development Team
*	Created Date : 2007/06/12
*	Version : 0.1
*
*	History
*	- Created(JeGeon.Jung 2007/06/12)
*   - Add Power Management (JeGeon.Jung 2007/06/25)
*	Todo
*
*
*	Note
*
**************************************************************************************/

#include <windows.h>
#include <types.h>
#include <linklist.h>
#include <nkintr.h>
#include <devload.h>
#include <pm.h>

#include <iic_mdd.h>
#include <iic_pdd.h>
#include <bsp_cfg.h>	// for reference S3C6410_PCLK
#include <s3c6410.h>
#include <DrvLib.h>
#include <oal_intr.h>

#define	MSG_ERROR		1
#define	MSG_FUNCTION	0
#define	MSG_INFO		0
#define	MSG_WARN		0
#define MSG_THREAD_INFO 0

#define DEFAULT_FILTER_ENABLE		1
#define DEFAULT_DELAY				Clk_15
#define DEFAULT_CLOCK				3000
#define DEFAULT_MODE				Master_transmit

#define TIMEOUT_MS					300//3000
#define TIMEOUT_MS_RX				300//3000

#define IIC_POWER_ON   (1<<17)    // PCLK_GATE bit 17

static volatile S3C6410_GPIO_REG 	*g_pGPIOReg 	= NULL;
static volatile S3C6410_SYSCON_REG 	*g_pSYSCONReg 	= NULL;
static volatile S3C6410_IIC_REG 	*g_pIICReg		= NULL;

static HANDLE				g_hTransferDone;		// transmit event done, both rx and tx
static HANDLE				g_hTransferEvent;		// IIC event, both rx and tx
static HANDLE				g_hTransferThread;		// transmit thread, both rx and tx

static DWORD 				g_IntrIIC 		= SYSINTR_NOP;

static PHW_OPEN_INFO		g_OwnerContext = NULL;

static PUCHAR 				g_pcIIC_BUFFER;
static UINT32 				g_uIIC_PT;
static UINT32 				g_uIIC_DATALEN;

BOOL 			MapVirtualAddress(void);
void 			InitializeGPIOPort(void);
void 			CalculateClockSet(PHW_OPEN_INFO pOpenContext);
static 	DWORD 	IIC_IST(LPVOID Context);
BOOL			WaitForReg(PVOID pRegAddress, UINT32 tMask, UINT32 tWaitForEqual, DWORD dwTimeout);
//////////
// Function Name : HW_Init
// Function Description : IIC device H/W initialization.
// Input : 	PHW_INIT_INFO	pInitContext
// Output :The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.1
BOOL 		HW_Init (PHW_INIT_INFO pInitContext)
{
    BOOL    RetVal           = TRUE;        // Initialize to success
	UINT32 	Irq;

    RETAILMSG (1,
              (TEXT("+ IIC 2 HW_Init(0x%X)\r\n"),
               pInitContext));

    if(!MapVirtualAddress())
    {
    	return FALSE;
    }

    InitializeGPIOPort();

    /* Create tx and rx events. Check return.
     */
	g_hTransferEvent = CreateEvent(0,FALSE,FALSE,NULL);
    if ( !g_hTransferEvent ) {
        RETAILMSG(MSG_ERROR,
                 (TEXT("Error creating event, HW_Init failed\n\r")));
        return(FALSE);
    }

	g_hTransferDone = CreateEvent(0,FALSE,FALSE,NULL);
    if ( !g_hTransferDone ) {
        RETAILMSG(MSG_ERROR,
                 (TEXT("Error creating done event, HW_Init failed\n\r")));
        return(FALSE);
    }

	// Obtain sysintr values from the OAL for the IIC interrupt.
	//
	Irq = IRQ_I2C;
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &Irq, sizeof(UINT32), &g_IntrIIC, sizeof(UINT32), NULL))
	{
		RETAILMSG(MSG_ERROR, (TEXT("ERROR: Failed to request the IIC sysintr.\r\n")));
		g_IntrIIC = SYSINTR_UNDEFINED;
		return(FALSE);
	}

	RETAILMSG(MSG_INFO, (TEXT("IIC IRQ mapping: [IRQ:%d->sysIRQ:%d].\r\n"), Irq, g_IntrIIC));

    // initialize the interrupt
    if( !InterruptInitialize(g_IntrIIC, g_hTransferEvent, NULL, 0) )
    {
        RETAILMSG(MSG_ERROR,(TEXT("HW_Init ERROR: Unable to initialize interrupt: %u\r\n"), GetLastError()));
        return(FALSE);
    }


    // create the IST
    if ( (g_hTransferThread = CreateThread(NULL, 0, IIC_IST, (LPVOID)pInitContext, 0, NULL)) == NULL)
    {
        RETAILMSG(MSG_ERROR,(TEXT("HW_Init ERROR: Unable to create IST: %u\r\n"), GetLastError()));
        return(FALSE);
    }

    // TODO: registry override
    if ( !CeSetThreadPriority(g_hTransferThread, pInitContext->Priority256)) {

        RETAILMSG(MSG_ERROR,(TEXT("HW_Init ERROR: CeSetThreadPriority ERROR:%d\n"), GetLastError()));
        return(FALSE);
    }

    RETAILMSG (MSG_FUNCTION|(RetVal == FALSE?MSG_ERROR:0),
              (TEXT("-HW_Init %s Ecode=%d\r\n"),
               (RetVal == TRUE) ? TEXT("Success") : TEXT("Error"),
               GetLastError()));
	return (RetVal);
}


//////////
// Function Name : HW_Deinit
// Function Description : IIC device H/W de-initialization.
// Input : 	PHW_INIT_INFO	pInitContext
// Output :The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.1
BOOL 		HW_Deinit (PHW_INIT_INFO pInitContext)
{
    BOOL            RetVal           = TRUE;        // Initialize to success

    RETAILMSG (MSG_FUNCTION,
              (TEXT("+HW_Deinit(0x%X)\r\n"),
               pInitContext));

	if (g_hTransferDone) {
		CloseHandle(g_hTransferDone);
		g_hTransferDone = NULL;
	}
	if (g_hTransferEvent) {
		CloseHandle(g_hTransferEvent);
		g_hTransferEvent = NULL;
	}
	if (g_hTransferThread) {
		CloseHandle(g_hTransferThread);
		g_hTransferThread = NULL;
	}

	if (g_pGPIOReg)
	{
		DrvLib_UnmapIoSpace((PVOID)g_pGPIOReg);
		g_pGPIOReg = NULL;
	}

	if (g_pSYSCONReg)
	{
		DrvLib_UnmapIoSpace((PVOID)g_pSYSCONReg);
		g_pSYSCONReg = NULL;
	}

	if (g_pIICReg)
	{
		DrvLib_UnmapIoSpace((PVOID)g_pIICReg);
		g_pIICReg = NULL;
	}

    RETAILMSG (MSG_FUNCTION,
              (TEXT("-HW_Deinit\r\n")));
	return (RetVal);
}


//////////
// Function Name : MapVirtualAddress
// Function Description : Mapping Virtual address of Registers.
// Input :
// Output :The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.1
BOOL 		MapVirtualAddress(void)
{
    BOOL            RetVal           = TRUE;        // Initialize to success

    RETAILMSG (MSG_FUNCTION,
              (TEXT("+MapVirtualAddress\r\n")));

	// GPIO SFR
	g_pGPIOReg = (S3C6410_GPIO_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_GPIO, sizeof(S3C6410_GPIO_REG), FALSE);
	if (g_pGPIOReg == NULL)
	{
		RETAILMSG(MSG_ERROR, (_T("MapVirtualAddress() : g_pGPIOReg DrvLib_MapIoSpace() Failed \n\r")));
		return FALSE;;
	}

	// SYSCON SFR
	g_pSYSCONReg = (S3C6410_SYSCON_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_SYSCON, sizeof(S3C6410_SYSCON_REG), FALSE);
	if (g_pSYSCONReg == NULL)
	{
		RETAILMSG(MSG_ERROR, (_T("MapVirtualAddress() : g_pSYSCONReg DrvLib_MapIoSpace() Failed \n\r")));
		return FALSE;;
	}

	// IIC SFR
	g_pIICReg = (S3C6410_IIC_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_IICBUS, sizeof(S3C6410_IIC_REG), FALSE);
	if (g_pIICReg == NULL)
	{
		RETAILMSG(MSG_ERROR, (_T("MapVirtualAddress() : g_pIICReg DrvLib_MapIoSpace() Failed \n\r")));
		return FALSE;;
	}

    RETAILMSG (MSG_FUNCTION,
              (TEXT("-MapVirtualAddress\r\n")));
	return (RetVal);
}


//////////
// Function Name : InitializeGPIOPort
// Function Description : Initializing GPIO port for IIC.
// Input :
// Output :
// Version : v0.9
void 	InitializeGPIOPort(void)
{
	// set SCL
	g_pGPIOReg->GPBCON = (g_pGPIOReg->GPBCON & ~(0xf<<20)) | (0x2<<20);

	// set SDA
	g_pGPIOReg->GPBCON = (g_pGPIOReg->GPBCON & ~(0xf<<24)) | (0x2<<24);

	// set SCL pull-up
	g_pGPIOReg->GPBPUD = (g_pGPIOReg->GPBPUD & ~(0x3<<10)) | (0x0<<10);

	// set SDA pull-up
	g_pGPIOReg->GPBPUD = (g_pGPIOReg->GPBPUD & ~(0x3<<12)) | (0x0<<12);
}

//////////
// Function Name : HW_PowerUp
// Function Description : Power control for IIC.
// Input : 	PHW_INIT_INFO pInitContext
// Output : The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.9
BOOL
HW_PowerUp(
    PHW_INIT_INFO pInitContext	/* value from I2C_Init */
   )
{
	RETAILMSG(MSG_FUNCTION,(TEXT("+[IIC]HW_PowerUp\r\n")));
	g_pSYSCONReg->PCLK_GATE |= IIC_POWER_ON;
	RETAILMSG(MSG_FUNCTION,(TEXT("-[IIC]HW_PowerUp\r\n")));
    return TRUE;
}

//////////
// Function Name : HW_PowerDown
// Function Description : Power control for IIC.
// Input : 	PHW_INIT_INFO pInitContext
// Output : The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.9
BOOL
HW_PowerDown(
    PHW_INIT_INFO pInitContext	/* value from I2C_Init */
   )
{
	RETAILMSG(MSG_FUNCTION,(TEXT("+[IIC]HW_PowerDown\r\n")));
	g_pSYSCONReg->PCLK_GATE &= ~IIC_POWER_ON;

	g_OwnerContext = NULL; // This is for Power Management. When Power up, IIC set register again.
	RETAILMSG(MSG_FUNCTION,(TEXT("-[IIC]HW_PowerDown\r\n")));
    return TRUE;
}

//////////
// Function Name : HW_OpenFirst
// Function Description : IIC device dirver is opened first.
// Input : 	PHW_OPEN_INFO	pOpenContext
// Output :The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.1
BOOL 		HW_OpenFirst (PHW_OPEN_INFO pOpenContext)
{
    BOOL    		RetVal          = 	TRUE;        // Initialize to success
	PHW_INIT_INFO	pInitContext	=	pOpenContext->pInitContext;

    RETAILMSG (MSG_FUNCTION,
              (TEXT("+HW_OpenFirst(0x%X)\r\n"),
               pOpenContext));

	// slave address setting
	pInitContext->PDDCommonVal.SlaveAddress = DEFAULT_SLAVE_ADDRESS;
	pInitContext->PDDCommonVal.InterruptEnable = DEFAULT_INTERRUPT_ENABLE;

    RETAILMSG (MSG_FUNCTION|(RetVal == FALSE?MSG_ERROR:0),
              (TEXT("-HW_OpenFirst %s Ecode=%d\r\n"),
               (RetVal == TRUE) ? TEXT("Success") : TEXT("Error"),
               GetLastError()));
	return (RetVal);
}

//////////
// Function Name : HW_CloseLast
// Function Description : IIC device dirver is closed last.
// Input : 	PHW_OPEN_INFO	pOpenContext
// Output :The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.1
BOOL 		HW_CloseLast (PHW_OPEN_INFO pOpenContext)
{
    BOOL    RetVal           = TRUE;        // Initialize to success

    RETAILMSG (MSG_FUNCTION,
              (TEXT("+HW_CloseLast(0x%X)\r\n"),
               pOpenContext));



    RETAILMSG (MSG_FUNCTION|(RetVal == FALSE?MSG_ERROR:0),
              (TEXT("-HW_CloseLast %s Ecode=%d\r\n"),
               (RetVal == TRUE) ? TEXT("Success") : TEXT("Error"),
               GetLastError()));
	return (RetVal);
}

//////////
// Function Name : HW_Open
// Function Description : IIC device dirver is opened.
// Input : 	PHW_OPEN_INFO	pOpenContext
// Output :The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.1
BOOL 		HW_Open		 (PHW_OPEN_INFO pOpenContext)
{
    BOOL    		RetVal       = TRUE;        // Initialize to success
    PHW_INIT_INFO	pInitContext = pOpenContext->pInitContext;

    RETAILMSG (MSG_FUNCTION,
              (TEXT("+HW_Open(0x%X)\r\n"),
               pOpenContext));

	pOpenContext->PDDContextVal.Clock 			= DEFAULT_CLOCK;
	pOpenContext->PDDContextVal.ModeSel 		= DEFAULT_MODE;
	pOpenContext->PDDContextVal.FilterEnable 	= DEFAULT_FILTER_ENABLE;
	pOpenContext->PDDContextVal.Delay 			= DEFAULT_DELAY;

	CalculateClockSet(pOpenContext);

	pOpenContext->DirtyBit = 1;

	HW_SetRegister(pOpenContext);


    RETAILMSG (MSG_FUNCTION,
              (TEXT("-HW_Open(0x%X)\r\n"),
               pOpenContext));
	return (RetVal);
}


//////////
// Function Name : HW_Close
// Function Description : IIC device dirver is closed.
// Input : 	PHW_OPEN_INFO	pOpenContext
// Output :The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.1
BOOL 		HW_Close	 (PHW_OPEN_INFO pOpenContext)
{
    BOOL    RetVal           = TRUE;        // Initialize to success
    RETAILMSG (MSG_FUNCTION,
              (TEXT("+HW_Close(0x%X)\r\n"),
               pOpenContext));

    RETAILMSG (MSG_FUNCTION,
              (TEXT("-HW_Close(0x%X)\r\n"),
               pOpenContext));
    return (RetVal);
}

//////////
// Function Name : CalculateClockSet
// Function Description : Calculate clock and save its setting value in ClkSrc, ClkDiv.
// Input : PHW_OPEN_INFO pOpenContext
// Output :
// Version : v1.0
void	CalculateClockSet(PHW_OPEN_INFO pOpenContext)
{
    RETAILMSG (MSG_FUNCTION,
              (TEXT("+CalculateClockSet(0x%X), Clock(%d)\r\n"),
               pOpenContext, pOpenContext->PDDContextVal.Clock));

	if (((S3C6410_PCLK>>4)/pOpenContext->PDDContextVal.Clock)>0xf)
	{
			pOpenContext->PDDContextVal.ClockSel	=	1;
			pOpenContext->PDDContextVal.ClockDiv	=	((S3C6410_PCLK>>9)/pOpenContext->PDDContextVal.Clock) -1;		//	PCLK/512/freq
            if(pOpenContext->PDDContextVal.ClockDiv != 0)
                pOpenContext->PDDContextVal.ClockDiv -=1;
	}
	else
	{
			pOpenContext->PDDContextVal.ClockSel	=	0;
			pOpenContext->PDDContextVal.ClockDiv	=	((S3C6410_PCLK>>4)/pOpenContext->PDDContextVal.Clock) -1;		//	PCLK/16/freq
            if(pOpenContext->PDDContextVal.ClockDiv != 0)
                pOpenContext->PDDContextVal.ClockDiv -=1;
	}

    RETAILMSG (MSG_FUNCTION,
              (TEXT("-CalculateClockSet(0x%X), ClockSel(%d), ClockDiv(%d)\r\n"),
               pOpenContext, pOpenContext->PDDContextVal.ClockSel, pOpenContext->PDDContextVal.ClockDiv));
}

//////////
// Function Name : HW_SetRegister
// Function Description : Set registers when Device Owner is changed.
// Input : PHW_OPEN_INFO pOpenContext
// Output :
// Version : v0.5
VOID 		HW_SetRegister (PHW_OPEN_INFO pOpenContext)
{
	PHW_INIT_INFO	pInitContext = pOpenContext->pInitContext;

	if(g_OwnerContext != pOpenContext || pOpenContext->DirtyBit == 1)
	{
	    RETAILMSG (MSG_FUNCTION,
	              (TEXT("+HW_SetRegister(0x%X)\r\n"),
	               pOpenContext));
		g_pIICReg->IICADD = pOpenContext->pInitContext->PDDCommonVal.SlaveAddress;
		g_pIICReg->IICSTAT = (g_pIICReg->IICSTAT & ~(0x3<<6)) | (1<<4) | (pOpenContext->PDDContextVal.ModeSel<<6);
		g_pIICReg->IICLC = (pOpenContext->PDDContextVal.FilterEnable<<2) | (pOpenContext->PDDContextVal.Delay);
		g_pIICReg->IICCON = (pOpenContext->PDDContextVal.ClockSel << 6) | (pInitContext->PDDCommonVal.InterruptEnable << 5) |
								(pOpenContext->PDDContextVal.ClockDiv & 0xf);

		g_OwnerContext = pOpenContext;
		pOpenContext->DirtyBit = 0;
	    RETAILMSG (MSG_FUNCTION,
	              (TEXT("-HW_SetRegister(0x%X)\r\n"),
	               pOpenContext));
    }
}

//////////
// Function Name : HW_SetClock
// Function Description : Set clock, this is called by IIC_IOControl
// Input : PHW_OPEN_INFO pOpenContext
// Output :
// Version : v0.5
VOID 		HW_SetClock (PHW_OPEN_INFO pOpenContext)
{
	CalculateClockSet(pOpenContext);
}

//////////
// Function Name : HW_Read
// Function Description : IIC Read Operation, Sync function, so wait for transfer done or time out.
// Input : PHW_OPEN_INFO pOpenContext
//		   PIIC_IO_DESC pOutData
// Output :
// Version : v0.5
BOOL		HW_Read		 	(PHW_OPEN_INFO pOpenContext, PIIC_IO_DESC pInData ,PIIC_IO_DESC pOutData)
{
    BOOL    retVal           = TRUE;        // Initialize to success
    RETAILMSG (MSG_FUNCTION,
              (TEXT("+HW_Read(0x%X)\r\n"),
               pOpenContext));

	HW_SetRegister(pOpenContext);
	HW_Write(pOpenContext, pInData);

	ResetEvent(g_hTransferDone);
	//	Wait until IIC bus is free.
	if(!WaitForReg((PVOID)&(g_pIICReg->IICSTAT), (1<<5), 0x0, TIMEOUT_MS_RX))
	{
		RETAILMSG(MSG_ERROR,(TEXT("[IIC Read ERROR]IIC BUS is busy.\r\n")));
		return FALSE;
	}
	
	g_pcIIC_BUFFER	=	pOutData->Data;
	g_uIIC_PT		=	0;
	g_uIIC_DATALEN	=	pOutData->Count;

	g_pIICReg->IICCON |= (1<<7);		//	Ack generation Enable

	g_pIICReg->IICDS = pOutData->SlaveAddress;

	g_pIICReg->IICSTAT = 0xB0;

	if(WaitForSingleObject(g_hTransferDone, TIMEOUT_MS_RX) == WAIT_TIMEOUT)
	{
		RETAILMSG(MSG_ERROR,(TEXT("[IIC ERROR]RX Time out.\r\n")));
		// TODO: Stop all transmit. ?? set register to stop
		retVal = FALSE;
	}

    RETAILMSG (MSG_FUNCTION,
              (TEXT("-HW_Read(0x%X)\r\n"),
               pOpenContext));
	return retVal;
}

//////////
// Function Name : HW_Write
// Function Description : IIC Write Operation, Sync function, so wait for transfer done or time out.
// Input : PHW_OPEN_INFO pOpenContext
//		   PIIC_IO_DESC pInData
// Output :
// Version : v0.5
BOOL		HW_Write		(PHW_OPEN_INFO pOpenContext, PIIC_IO_DESC pInData)
{
    BOOL    retVal           = TRUE;        // Initialize to success

    RETAILMSG (MSG_FUNCTION,
              (TEXT("+HW_Write(0x%X)\r\n"),
               pOpenContext));

	HW_SetRegister(pOpenContext);

	ResetEvent(g_hTransferDone);
	//	Wait until IIC bus is free.
	if(!WaitForReg((PVOID)&(g_pIICReg->IICSTAT), (1<<5), 0x0, TIMEOUT_MS))
	{
		RETAILMSG(MSG_ERROR,(TEXT("[IIC Write ERROR]IIC BUS is busy.\r\n")));
		return FALSE;
	}

	g_pcIIC_BUFFER	=	pInData->Data;
	g_uIIC_PT		=	0;
	g_uIIC_DATALEN	=	pInData->Count;

	g_pIICReg->IICCON |= (1<<7);		//	Ack generation Enable

	g_pIICReg->IICDS = pInData->SlaveAddress;

	RETAILMSG(MSG_INFO,(TEXT("[IIC TX]Slave Address is 0x%02X\n"),pInData->SlaveAddress));

	g_pIICReg->IICSTAT = 0xf0;

	if(WaitForSingleObject(g_hTransferDone, TIMEOUT_MS) == WAIT_TIMEOUT)
	{
		RETAILMSG(MSG_ERROR,(TEXT("[IIC ERROR]TX Time out.\r\n")));
		// TODO: Stop all transmit. ?? set register to stop
		retVal = FALSE;
	}

    RETAILMSG (MSG_FUNCTION,
              (TEXT("-HW_Write(0x%X)\r\n"),
               pOpenContext));
	return retVal;
}

//////////
// Function Name : WaitForReg
// Function Description : Wait for register value, or time out.
// Input :     DWORD dwRegAddress,	 // address of register for check
//			    UINT32 tMask,		 // mask for check position
//			    UINT32 tWaitForEqual,	// compare value.
//			    DWORD dwTimeout			// time out (ms)
// Output : True (the value is set), False (Time out)
// Version : v0.5
BOOL
WaitForReg(
    PVOID pRegAddress,
    UINT32 tMask,
    UINT32 tWaitForEqual,
    DWORD dwTimeout
    )
{

    const DWORD dwStart = GetTickCount();

    UINT32 tValue;

    BOOL fRet = TRUE;
    DWORD dwIteration = 1;

    // Verify that reset has completed.
    do {
        tValue = *(volatile UINT32*)(pRegAddress);

        if ( (dwIteration % 16) == 0 ) {
            // Check time
            DWORD dwCurr = GetTickCount();

            // Unsigned arithmetic handles rollover.
            DWORD dwTotal = dwCurr - dwStart;

            if (dwTotal > dwTimeout) {
                // Timeout
                fRet = FALSE;
                RETAILMSG(MSG_WARN	, (_T("Timeout (%u ms) waiting for REGISTER & 0x%08x == 0x%08x\r\n"),
                    dwTimeout, tMask, tWaitForEqual));
                break;
            }
        }

        ++dwIteration;
    } while ((tValue & tMask) != tWaitForEqual);

    return fRet;
}

static DWORD
IIC_IST(
    LPVOID Context
    )
{
	static DWORD	dwTimeOut = INFINITE;
    PHW_INIT_INFO pInitContext = (PHW_INIT_INFO)Context;
    BOOL bDone = FALSE;
	DWORD	iicstat;
    if ( !pInitContext ) {
        return ERROR_INVALID_PARAMETER;
    }

	SetProcPermissions((DWORD)-1);
    //DEBUGMSG(ZONE_IST|ZONE_TRACE,(TEXT("+I2C_IST[%u, %u, %d] \r\n"), pI2C->Mode, pI2C->State, pI2C->DataCount));

	while(pInitContext->State == IIC_RUN)
	{
		DWORD we;
		we = WaitForSingleObject(g_hTransferEvent, dwTimeOut);

		if(pInitContext->State == IIC_FINISH) continue;		// when we destruct IIC thread.

		iicstat = g_pIICReg->IICSTAT;
        if (iicstat & 0x8) {
            RETAILMSG(MSG_ERROR,(TEXT("%x I2C_IST[0x%x, %d]: bus arbitration failed \r\n"),
                (iicstat>>6)&0x3,g_OwnerContext, g_uIIC_PT));
        }

        if (iicstat & 0x4) {
            RETAILMSG(MSG_ERROR,(TEXT("%x I2C_IST[0x%x, %d]: slave address matches IICADD \r\n"),
                (iicstat>>6)&0x3,g_OwnerContext, g_uIIC_PT));
        }

        if (iicstat & 0x2) {
            RETAILMSG(MSG_ERROR,(TEXT("%x I2C_IST[0x%x, %d]: received slave address 0x0 \r\n"),
                (iicstat>>6)&0x3,g_OwnerContext, g_uIIC_PT));
        }

        if (iicstat & 0x1) {
            //RETAILMSG(MSG_ERROR,(TEXT("%x I2C_IST[0x%x, %d]: ACK NOT received \r\n"),
               // (iicstat>>6)&0x3,g_OwnerContext, g_uIIC_PT));
        }

		switch( (iicstat>>6)&0x3)
		{
		case Slave_receive:
			break;

		case Slave_transmit:
			break;

		case Master_receive:
			if (g_uIIC_PT>0)
			{
                bDone = FALSE;
				g_pcIIC_BUFFER[g_uIIC_PT-1] = g_pIICReg->IICDS;
			}

			g_uIIC_PT++;

			if (g_uIIC_PT==g_uIIC_DATALEN)
			{
				g_pIICReg->IICCON &= ~(1<<7);
			}
			else if (g_uIIC_PT > g_uIIC_DATALEN)
			{
                bDone = TRUE;
				g_pIICReg->IICSTAT = 0x90;
			}

			g_pIICReg->IICCON &= ~(1<<4);
			break;

		case Master_transmit:
			if (g_uIIC_PT<g_uIIC_DATALEN)
			{
				bDone = FALSE;
				g_pIICReg->IICDS = g_pcIIC_BUFFER[g_uIIC_PT];
				RETAILMSG(MSG_THREAD_INFO,(TEXT("[IIC TX THREAD]g_pIICReg->IICDS is 0x%02X\n"),g_pIICReg->IICDS));
			}
			else
			{
				bDone = TRUE;
				g_pIICReg->IICSTAT = 0xd0;		//	Stop Master Tx condition, ACK flag clear
			}

			g_uIIC_PT++;
			g_pIICReg->IICCON &= ~(1<<4);
			break;
		}

        InterruptDone(g_IntrIIC);

        if (bDone) {
            RETAILMSG(MSG_INFO, (TEXT("SetEvent DONE\r\n")));
			 bDone = FALSE;    
            SetEvent(g_hTransferDone);
        }
	}

	return ERROR_SUCCESS;
}