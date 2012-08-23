#ifndef __ARGS_H
#define __ARGS_H

//------------------------------------------------------------------------------
//
// File:        args.h
//
// Description: This header file defines device structures and constant related
//              to boot configuration. BOOT_CFG structure defines layout of
//              persistent device information. It is used to control boot
//              process. BSP_ARGS structure defines information passed from
//              boot loader to kernel HAL/OAL. Each structure has version
//              field which should be updated each time when structure layout
//              change.
//
//------------------------------------------------------------------------------

#include "oal_args.h"
#include "oal_kitl.h"

#define BSP_ARGS_VERSION    1

typedef struct {
	OAL_ARGS_HEADER header;
	UINT8 deviceId[16];                 // Device identification
	OAL_KITL_ARGS kitl;

	BOOL fUpdateMode;                 // Is the device in update mode?
	BOOL fUldrReboot;

	// record the size and location of the RAM FMD if present
	DWORD dwExtensionRAMFMDSize;
	PVOID pvExtensionRAMFMDBaseAddr;
	DWORD nfsblk;

	BOOL bDCDetected;		// 1:Inserted. 0:extracted.
	BOOL bPowerStatus;       // 1:AC status 0:DC status

	BOOL bDisplayOff;
	BOOL bUSBPlug;
	BOOL bWaveDown;	
	ULONG	Device_ID[4];
} BSP_ARGS;

//------------------------------------------------------------------------------

#endif
