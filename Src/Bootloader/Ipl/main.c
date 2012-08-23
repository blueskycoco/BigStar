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
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name: main.c

Abstract: OEM IPL routines for the Samsung SMDK6410 hardware platform.

Functions:

Notes:

--*/
#include <windows.h>
#include <iplcommon.h>
//#include <bootpart.h>   // Needed for partition types...
#include <bsp.h>
#include <fmd.h>
#include "loader.h"

// Virtual address of bootpart work buffer.
//
#define BPART_BUFFER_U_VIRTUAL    0xA1121000
#define BPART_BUFFER_LENGTH_BYTES 0x00021000
UCHAR			g_TOC[LB_SECTOR_SIZE];
const PTOC 		g_pTOC = (PTOC)&g_TOC;

// Global variables.
//
// --- These are used by iplcommon (start) ---
UINT32 g_ulFlashBase        = 0;                            // Flash start and length.
UINT32 g_ulFlashLengthBytes = NAND_FLASH_SIZE - IMAGE_IPL_SIZE;

UINT32 g_ulBPartBase        = BPART_BUFFER_U_VIRTUAL;       // BootPart work buffer start and length.
UINT32 g_ulBPartLengthBytes = BPART_BUFFER_LENGTH_BYTES;
// --- These are used by iplcommon (end)   ---


// External variables.
//
extern PFN_MessageHandler g_pfnMessageHandler;

// To change Voltage for higher clock.
extern void LTC3714_Init();
extern void LTC3714_VoltageSet(UINT32,UINT32,UINT32);


// Function prototypes.
//
void OEMInitDebugSerial(void);
void OEMWriteDebugString(unsigned short *str);
static void OEMMessageHandler(IPL_MESSAGE_CODE MessageCode, LPWSTR pMessageString);
void Launch(UINT32 ulLaunchAddr);
void InitBSPArgs();
void delay()
{
	volatile long i,j,k;
	for(i=0;i<2000;i++)
		for(j=0;j<1000;j++)
			k=1;
}

void main(void)
{
	OEMInitDebugSerial();
    OALMSG(TRUE,(TEXT("IPLmain\r\n")));
	//InitializeDisplay();
	OALMSG(OAL_ERROR, (TEXT("Start: IPLmain start \r\n")));
    IPLmain();

}


static void OEMMessageHandler(IPL_MESSAGE_CODE MessageCode, LPWSTR pMessageString)
{

    // Write a hex LED code.
    //
    OEMWriteDebugString((UINT16 *)pMessageString);

}


/*
    @func   BOOL | ReadBootConfig | Read bootloader settings from flash.
    @rdesc  TRUE = Success, FALSE = Failure.
    @comm   This routine came from eboot\main.c
    @xref
*/

static BOOL ReadBootConfig(PBOOT_CFG pBootCfg)
{
	BOOLEAN bResult = FALSE;
	unsigned char tbuf[2048];
	SectorInfo si;
	BSP_ARGS *iplpBSPArgs = ((BSP_ARGS *) IMAGE_SHARE_ARGS_UA_START);

	OALMSG(OAL_FUNC, (TEXT("+ReadBootConfig.\r\n")));

	// Valid caller buffer?
	if (!pBootCfg)
	{
		OALMSG(OAL_ERROR, (TEXT("ERROR: Bad caller buffer.\r\n")));
		goto CleanUp;
	}

	// Read settings from flash...
	//
	if ( !FMD_ReadSector(TOC_SECTOR, tbuf, &si, 1) ) {
		OALMSG(OAL_ERROR, (TEXT("TOC_Read ERROR: Unable to read TOC\r\n")));
		goto CleanUp;
	}

	memcpy(g_pTOC, tbuf, sizeof(g_TOC));
	// is it a valid TOC?
	if ( !VALID_TOC(g_pTOC) ) {
		OALMSG(OAL_ERROR, (TEXT("TOC_Read ERROR: INVALID_TOC Signature: 0x%x\r\n"), g_pTOC->dwSignature));
		goto CleanUp;
	}

	// is it an OEM block?
	if ( (si.bBadBlock != BADBLOCKMARK) || !(si.bOEMReserved & (OEM_BLOCK_RESERVED | OEM_BLOCK_READONLY)) ) {
		OALMSG(OAL_ERROR, (TEXT("TOC_Read ERROR: SectorInfo verify failed: %x %x %x %x\r\n"),
		si.dwReserved1, si.bOEMReserved, si.bBadBlock, si.wReserved2));
		goto CleanUp;
	}

	// update our boot config
	pBootCfg = &g_pTOC->BootCfg;
		iplpBSPArgs->Device_ID[0]=pBootCfg->Device_ID[0];
		iplpBSPArgs->Device_ID[1]=pBootCfg->Device_ID[1];
		iplpBSPArgs->Device_ID[2]=pBootCfg->Device_ID[2];
		iplpBSPArgs->Device_ID[3]=pBootCfg->Device_ID[3];
//OALLog(L"\r\n<><>DeviceID %x %x %x %x : ",pBootCfg->Device_ID[0],pBootCfg->Device_ID[1],pBootCfg->Device_ID[2],pBootCfg->Device_ID[3]);
	bResult = TRUE;

CleanUp:

    OALMSG(OAL_FUNC, (TEXT("-ReadBootConfig.\r\n")));
    return(bResult);

}


#define COMPILE_TIME_ASSERT(condition) { int compile_time_assert[(condition)]; compile_time_assert; }


void InitBSPArgs()
{
    UINT8 maccount = 0;
    BSP_ARGS *iplpBSPArgs = ((BSP_ARGS *) IMAGE_SHARE_ARGS_UA_START);
	PBOOT_CFG pBootCfg;

    // create the header
    iplpBSPArgs->header.signature       = OAL_ARGS_SIGNATURE;
    iplpBSPArgs->header.oalVersion      = OAL_ARGS_VERSION;
    iplpBSPArgs->header.bspVersion      = BSP_ARGS_VERSION;

    // Clear BSP Reserved Parameter.
    iplpBSPArgs->fUpdateMode = FALSE ;
    iplpBSPArgs->fUldrReboot = FALSE ;
    iplpBSPArgs->dwExtensionRAMFMDSize = 0;
    iplpBSPArgs->pvExtensionRAMFMDBaseAddr = NULL;


    if (ReadBootConfig(pBootCfg))
    {
	//	iplpBSPArgs->Device_ID[0]=pBootCfg->Device_ID[0];
	//	iplpBSPArgs->Device_ID[1]=pBootCfg->Device_ID[1];
	//	iplpBSPArgs->Device_ID[2]=pBootCfg->Device_ID[2];
	//	iplpBSPArgs->Device_ID[3]=pBootCfg->Device_ID[3];
	//	OALLog(L"\r\nDeviceID %x %x %x %x : ",iplpBSPArgs->Device_ID[0],iplpBSPArgs->Device_ID[1],iplpBSPArgs->Device_ID[2],iplpBSPArgs->Device_ID[3]);
    }

    // store a base name for the device and signal to KITL to extend it later
    //strncpy(pBSPArgs->deviceId, BSP_DEVICE_PREFIX, OAL_KITL_ID_SIZE);
    //pBSPArgs->kitl.flags |= OAL_KITL_FLAGS_EXTNAME;
}



///////////////////////////////////////////////////////////////////////////////
// Required OEM IPL routines.
///////////////////////////////////////////////////////////////////////////////
// --- These are used by iplcommon (start) ---
BOOLEAN OEMIPLInit(void)
{

    // Initialize the UART.
    //
    OEMInitDebugSerial();

    OALLog(L"\r\nMicrosoft Windows CE IPL for SMDK6410 Ref board\r\n");

    // Set the flash address
    g_ulFlashBase = NAND_FLASH_START_UA;

    // Messaging handler callback.
    //
    g_pfnMessageHandler = OEMMessageHandler;

    // Check if Current ARM speed is not matched to Target Arm speed
    // then To get speed up, set Voltage
#if (APLL_CLK == CLK_1332MHz)
    LTC3714_Init();
    LTC3714_VoltageSet(1,1200,100);     // ARM
    LTC3714_VoltageSet(2,1300,100);     // INT
#elif (APLL_CLK == CLK_800MHz)
    LTC3714_Init();
    LTC3714_VoltageSet(1,1300,100);     // ARM
    LTC3714_VoltageSet(2,1200,100);     // INT
#endif

    return(TRUE);
}


BOOLEAN OEMGetUpdateMode(void)
{
    BOOL *pfUpdateMode = NULL;
    BSP_ARGS *iplpBSPArgs = ((BSP_ARGS *) IMAGE_SHARE_ARGS_UA_START);
    BOOL fUpdateMode = iplpBSPArgs->fUpdateMode;

    OALLog(L"OEMGetUpdateMode\r\n");

    if(!fUpdateMode){
        pfUpdateMode = (BOOL *) OALArgsQuery(OAL_ARGS_QUERY_UPDATEMODE);
        if (pfUpdateMode == NULL)
        {
            OEMWriteDebugString(L"Invalid BSP Args - initializing to good values\r\n");
            InitBSPArgs();
        }
        else
        {
            fUpdateMode = *pfUpdateMode;
        }
	}
    else
    {
        // create the header
    	iplpBSPArgs->header.signature       = OAL_ARGS_SIGNATURE;
    	iplpBSPArgs->header.oalVersion      = OAL_ARGS_VERSION;
    	iplpBSPArgs->header.bspVersion      = BSP_ARGS_VERSION;
    	RETAILMSG(1,(TEXT("updateTRUE but set ARG\r\n")));
    }



    if (fUpdateMode)
    {
        OEMWriteDebugString(L"Update Mode RAM flag is set\r\n");
    }
    else
    {
        // RAM flag is not set - are we recovering from power failure?
        if (!BP_GetUpdateModeFlag(&fUpdateMode))
        {
            OEMWriteDebugString(L"Error in BP_GetUpdateModeFlag\r\n");
        }
        if (fUpdateMode)
        {
            OEMWriteDebugString(L"Update Mode Persistent flag is set\r\n");
        }
    }

    if (fUpdateMode)
    {
        OEMWriteDebugString(L"----------------------------------------\r\n");
        OEMWriteDebugString(L"--------------- LOAD ULDR --------------\r\n");
        OEMWriteDebugString(L"----------------------------------------\r\n");
    }
	else
	{
        OEMWriteDebugString(L"----------------------------------------\r\n");
        OEMWriteDebugString(L"---------------  LOAD OS  --------------\r\n");
        OEMWriteDebugString(L"----------------------------------------\r\n");
    }
    
    return(fUpdateMode);
}


BOOLEAN OEMTranslateBaseAddress(UINT32 ulPartType, UINT32 ulAddr, UINT32 *pulTransAddr)
{

    if (pulTransAddr == NULL)
    {
        return(FALSE);
    }

    switch(ulPartType)
    {
    case PART_BOOTSECTION:
        // No translation.
        *pulTransAddr = ulAddr;
        break;

    case PART_XIP:
        *pulTransAddr = ulAddr | 0x20000000;
        break;
/*
    case PART_RAMIMAGE:
        *pulTransAddr = ulAddr | 0x20000000;
        break;
*/
    default:
        // No translation.
        *pulTransAddr = ulAddr;
        break;
    }

    return(TRUE);
}


void OEMLaunchImage(UINT32 ulLaunchAddr)
{
    UINT32 ulPhysicalJump = 0;

    // The IPL is running with the MMU on - before we jump to the loaded image, we need to convert
    // the launch address to a physical address and turn off the MMU.
    //

    // Convert jump address to a physical address.
    ulPhysicalJump = OALVAtoPA((void *)ulLaunchAddr);

    RETAILMSG(1, (TEXT("Jumping to VA 0x%x PA 0x%x...\r\n"), ulLaunchAddr, ulPhysicalJump));

    // Jump...
    Launch(ulPhysicalJump);
}

// --- These are used by iplcommon (end)   ---

