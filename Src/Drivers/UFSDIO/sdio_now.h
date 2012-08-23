//
// Copyright (C) 2007 by Cambridge Silicon Radio Ltd.
//

#include <Sdcardddk.h>
#include <winreg.h>

#define EIO       5
#define EINVAL    22
#define ETIMEDOUT 56


typedef VOID (*PUNIFI_INTERRUPT_HANDLER) (PVOID);

typedef struct _SDIO_NOW_CONTEXT
{
    PVOID ClientInitContext;
    PWCHAR RegPath;
    SD_DEVICE_HANDLE SdBusContext;
	ULONG BlockSize;
    SD_INTERFACE_MODE BusWidth;
    PVOID CardContext;
    PUNIFI_INTERRUPT_HANDLER InterruptHandler;
    ULONG ClockRate;
	UCHAR FunctionNumber;
}
SDIO_NOW_CONTEXT, *PSDIO_NOW_CONTEXT;


SD_API_STATUS IntInterruptCallback(SD_DEVICE_HANDLE H, PVOID Context);

SD_API_STATUS SdNowBlockModeTransfer(SD_DEVICE_HANDLE SdBusContext,
                                     UINT32 Address,
                                     PUCHAR Buffer, 
                                     UINT16 Blocks,
                                     ULONG BlockLength, 
                                     UCHAR Direction);

SD_API_STATUS SdNowByteModeTransfer(SD_DEVICE_HANDLE SdBusContext,
                                    UINT32 Address,
                                    PUCHAR Buffer, 
                                    UINT16 BufLength,
                                    UCHAR Direction);

