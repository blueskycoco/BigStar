//
// Copyright (C) 2007 by Cambridge Silicon Radio Ltd.
//

#include "sdio_now.h"

#define TRACE(msg)  //RETAILMSG(TRUE,msg)

static PSDIO_NOW_CONTEXT SdNowContext = NULL;

// some platforms may return this for a BlockTransfer BUT we build in a
// platform that doesnt define it
#if !defined SD_API_STATUS_FAST_PATH_SUCCESS
# define SD_API_STATUS_FAST_PATH_SUCCESS                ((SD_API_STATUS)0x00000002L)
#endif

//
// convert_csr_sdio_error 
//  Trys to map a SD_API_STATUS code to a Posix error code
//
// Arguments
//  Status - SD_API_STATUS code
//
// Returns
//   INT32 
//
__inline INT32 convert_csr_sdio_error(SD_API_STATUS Status)
{
    switch(Status)
    {
        case SD_API_STATUS_SUCCESS:
        case SD_API_STATUS_FAST_PATH_SUCCESS:
            //TRACE(( L"+" ));
            return 0;
        break;

        case SD_API_STATUS_RESPONSE_TIMEOUT:
        case SD_API_STATUS_DATA_TIMEOUT:
            TRACE(( L"return -ETIMEOUT\n" ));
            return -ETIMEDOUT;
        break;

        case SD_API_STATUS_CRC_ERROR:
            TRACE(( L"return -EIO\n" ));
            return -EIO;
        break;

        default:
            TRACE(( L"return -EINVAL(%d)\n", Status ));
            return -EINVAL;
        break;
    }
}

//#define MAX_PATH_VALUE_LEN 128

//
// SdNowInitialize
//
// Arguments
//
// Returns
//  UniFi driver error code.
//
INT SdNowInitialize( PVOID ClientInitContext )
{
    SDCARD_CLIENT_REGISTRATION_INFO ClientRegInfo;
    PWCHAR ClientName = TEXT("UniFi SDIO WLAN Adapter");
    SD_API_STATUS Status = SD_API_STATUS_UNSUCCESSFUL;

    TRACE(( L"SdNowInitialize(%s)\n", ClientInitContext ));

    if(NULL == ClientInitContext)
    {
        Status = SD_API_STATUS_NO_SUCH_DEVICE;
        goto Leave;
    }

    SdNowContext = (PSDIO_NOW_CONTEXT)LocalAlloc(LPTR, sizeof(SDIO_NOW_CONTEXT));

    if(NULL == SdNowContext)
    {
        Status = SD_API_STATUS_INSUFFICIENT_RESOURCES;
        goto Leave;
    }

    SdNowContext->ClientInitContext = ClientInitContext;
    SdNowContext->RegPath = NULL;
    SdNowContext->SdBusContext = SDGetDeviceHandle((DWORD)ClientInitContext, &SdNowContext->RegPath);

    if(NULL == SdNowContext->SdBusContext)
    {
        Status = SD_API_STATUS_NO_SUCH_DEVICE;
        goto Leave;
    }

    //
    // Set client options and register as a client device
    //
    memset(&ClientRegInfo, 0, sizeof(ClientRegInfo));
    memcpy(ClientRegInfo.ClientName, ClientName, wcslen(ClientName));
    ClientRegInfo.ClientFlags |= SD_CLIENT_HANDLES_RETRY;
    
    Status = SDRegisterClient(SdNowContext->SdBusContext, SdNowContext, &ClientRegInfo);
    if(Status != SD_API_STATUS_SUCCESS)
    {
        goto Leave;
    }

    //
    // Will this always be so ?
    //
    SdNowContext->FunctionNumber = 1;
    SdNowContext->BusWidth = SD_INTERFACE_SD_4BIT;
    SdNowContext->ClockRate = 100; // KHz 

Leave:

    if(SD_API_STATUS_SUCCESS != Status)
    {
        if(NULL != SdNowContext)
        {
            LocalFree(SdNowContext);
            SdNowContext = NULL;
        }
    }

    TRACE(( L"SdNowInitialize - OK\n" ));
    return convert_csr_sdio_error(Status);
}


//
// SdNowDeinitialize
//
// Arguments
//  VOID
//
// Returns
//  UniFi driver error code.
//
INT SdNowDeinitialize(VOID)
{
    SD_API_STATUS Status = SD_API_STATUS_SUCCESS;
    TRACE(( L"SdNowDeinitialize()\n" ));

    if(NULL != SdNowContext)
    {
        if(NULL != SdNowContext->SdBusContext)
        {
            SDIODisconnectInterrupt(SdNowContext->SdBusContext);
        }
        LocalFree(SdNowContext);
        SdNowContext = NULL;
    }

    return convert_csr_sdio_error(Status);
}



//
// SdNowSetClockSpeed
//
// Arguments
//  Khz - clock speed in kHz
//
// Returns
//  The clock speed set in kHz or a UniFi driver error code.
//
INT SdNowSetClockSpeed(INT Khz)
{
    ULONG ClockRate = (ULONG)Khz;
    SD_CARD_INTERFACE SdInterface;
    SD_API_STATUS Status;

    TRACE(( L"SdNowSetClockSpeed()\n" ));
    //
    // We can't _just_ set the clock <sigh>
    //
    SdInterface.InterfaceMode = SdNowContext->BusWidth;
    SdInterface.ClockRate = ClockRate * 1000; // MHz 
    SdInterface.WriteProtected = FALSE;

    Status = SDSetCardFeature(SdNowContext->SdBusContext, SD_SET_CARD_INTERFACE, &SdInterface, sizeof(SdInterface));
    if(Status == SD_API_STATUS_SUCCESS)
    {
        SdNowContext->ClockRate = ClockRate;
        return (INT)ClockRate;
    }
    else
    {
        return convert_csr_sdio_error(Status);
    }
}

//
// SdNowSetBusWidth
//
// Arguments
//  BusWidth - 1 or 4
//
// Returns
//   UniFi driver error code.
//
INT SdNowSetBusWidth(INT BusWidth)
{
    SD_CARD_INTERFACE SdInterface;
    SD_API_STATUS Status;

    TRACE(( L"SdNowSetBusWidth()\n" ));
    SdNowContext->BusWidth = (1 == BusWidth) ? SD_INTERFACE_SD_MMC_1BIT : SD_INTERFACE_SD_4BIT;

    //
    // We can't _just_ set the bus width <sigh>
    //
    SdInterface.InterfaceMode = SdNowContext->BusWidth;
    SdInterface.ClockRate = SdNowContext->ClockRate * 1000; // MHz 
    SdInterface.WriteProtected = FALSE;

    Status = SDSetCardFeature(SdNowContext->SdBusContext, SD_SET_CARD_INTERFACE, &SdInterface, sizeof(SdInterface));

    return convert_csr_sdio_error(Status);
}

//
// SdNowSetBlockSize
//
// Arguments
//  Blocksize - CMD53 Block size in bytes
//
//  Returns
//   UniFi driver error code.
//
INT SdNowSetBlockSize(INT BlockSize)
{
    SD_API_STATUS Status;

    TRACE(( L"SdNowSetBlockSize()\n" ));
    SdNowContext->BlockSize = BlockSize; 
    Status = SDSetCardFeature(SdNowContext->SdBusContext, 
                              SD_IO_FUNCTION_SET_BLOCK_SIZE, 
                              (PVOID)&SdNowContext->BlockSize, 
                              sizeof(SdNowContext->BlockSize));

    return convert_csr_sdio_error(Status);
}

//
// SdNowSetInterruptHandler
//
// Arguments
//  InterruptHandler - Pointer to a function to call to handle interrupts
//
//  Returns
//   UniFi driver error code.
//
INT SdNowSetInterruptHandler(PUNIFI_INTERRUPT_HANDLER InterruptHandler, PVOID CardContext)
{
    SD_API_STATUS Status;

    TRACE(( L"SdNowSetInterruptHandler(%p, %p)\n", InterruptHandler, CardContext ));
    if(NULL != InterruptHandler)
    {
        if(NULL != CardContext)
        {
            TRACE(( L"SDIOConnectInterrupt()\n" ));
            SdNowContext->InterruptHandler = InterruptHandler;
            SdNowContext->CardContext = CardContext;
            Status = SDIOConnectInterrupt(SdNowContext->SdBusContext, (PSD_INTERRUPT_CALLBACK)IntInterruptCallback);
        }
        else
        {
            Status = SD_API_STATUS_INVALID_PARAMETER;
        }
    }
    else
    {
        TRACE(( L"SDIODisconnectInterrupt()\n" ));
        SDIODisconnectInterrupt(SdNowContext->SdBusContext );
        Status = SD_API_STATUS_SUCCESS;
    }

    return convert_csr_sdio_error(Status);
}

//
// IntInterruptCallback
//
// Arguments
//  Context - Pointer to SDIO_NOW_CONTEXT structure 
//  H       - An SD_DEVICE_HANDLE
//
// Returns
//  SD_API_STATUS_SUCCESS
//
static 
SD_API_STATUS IntInterruptCallback(SD_DEVICE_HANDLE H, PVOID Context)
{
    PSDIO_NOW_CONTEXT SdNowContext = (PSDIO_NOW_CONTEXT)Context; 
    //TRACE(( L"!" ));
    (SdNowContext->InterruptHandler)(SdNowContext->CardContext);
    return SD_API_STATUS_SUCCESS;
}

//
// SdNowEnableInterrupt
//  Enable or disable SDIO interrupt.
//
// Arguments
//  Enable - zero disable, non zero enable
//
//  Returns
//   UniFi driver error code.
//
INT SdNowEnableInterrupt(INT Enable)
{
    UCHAR IntEnable = 1; // Master enable
    SD_API_STATUS Status;

    if(0 != Enable)
    {
        IntEnable |=  (1 << SdNowContext->FunctionNumber);
    }

    Status = SDReadWriteRegistersDirect(SdNowContext->SdBusContext, 
                                        SD_IO_WRITE, 
                                        0, 
                                        SD_IO_REG_INT_ENABLE, 
                                        FALSE, 
                                        &IntEnable, 
                                        1);

    return convert_csr_sdio_error(Status);
}

//
// SdNowBlockReadWrite 
//  Performs Block transfers to/from the hw.
//
// Arguments
//  Address        - buffer number for transfer
//  TransferLength - number of bytes to transfer
//  Direction      - direction for transfer. 0 for read, non 0 for write
//
// Returns
//  UniFi driver error code.
//
INT SdNowBlockReadWrite(ULONG Address, PUCHAR Data, UINT TransferLength, INT Direction)
{
    SD_API_STATUS Status;
    ULONG Blocks = TransferLength / SdNowContext->BlockSize;
    ULONG BytesLeftOver = TransferLength % SdNowContext->BlockSize;

    if(Blocks < 512)
    {
        if(Blocks > 0)
        {
            Status = SdNowBlockModeTransfer(SdNowContext->SdBusContext, 
                                            Address, 
                                            Data, 
                                            (USHORT)Blocks, 
                                            SdNowContext->BlockSize, 
                                            Direction);
        }

        if(BytesLeftOver < 512)
        {
            if(BytesLeftOver > 0)
            {
                Data += Blocks * SdNowContext->BlockSize;
                Status = SdNowByteModeTransfer(SdNowContext->SdBusContext, 
                                               Address, 
                                               Data, 
                                               (USHORT)BytesLeftOver, 
                                               Direction);
            }
        }
        else
        {
            Status = SD_API_STATUS_INVALID_PARAMETER;
        }
    }
    else
    {
        Status = SD_API_STATUS_INVALID_PARAMETER;
    }

    return convert_csr_sdio_error(Status);
} 

//
// SdNowBlockModeTransfer 
//  Perform a block mode CMD53 transfer.
//
// Arguments
//  SdBusContext - SD bus device Handle 
//  Address      - The address of the data inside function 1.
//  Buffer       - Buffer to transfer.
//  Blocks       - Number of blocks to transfer.
//  BlockLength  - Length of each block.
//  Direction    - 0 to read, non 0 to write
//  
// Returns
//  SD_AP_STATUS
//
static
SD_API_STATUS SdNowBlockModeTransfer(SD_DEVICE_HANDLE SdBusContext,
                                     UINT32 Address,
                                     PUCHAR Buffer, 
                                     UINT16 Blocks,
                                     ULONG BlockLength, 
                                     UCHAR Direction)
{
    ULONG Cmd;
    SD_COMMAND_RESPONSE CmdResponse;
    SD_TRANSFER_CLASS Tc;
    UCHAR TOp;
    SD_API_STATUS Status;

    if(0 == Direction)
    {
        Tc = SD_READ;
        TOp = SD_IO_OP_READ;
    }
    else
    {
        Tc = SD_WRITE;
        TOp = SD_IO_OP_WRITE;
    }

    Cmd = BUILD_IO_RW_EXTENDED_ARG(TOp, SD_IO_BLOCK_MODE, 1, Address, SD_IO_INCREMENT_ADDRESS, Blocks); 

    Status = SDSynchronousBusRequest(SdBusContext,
                                     SD_CMD_IO_RW_EXTENDED, 
                                     Cmd,
                                     Tc,
                                     ResponseR5, 
                                     &CmdResponse, 
                                     Blocks, 
                                     BlockLength, 
                                     Buffer, 
                                     0);

    return Status;
}

//
// SdNowByteModeTransfer 
//  Perform a byte mode CMD53 transfer.
//
// Arguments
//  SdBusContext - SD bus device Handle 
//  Address      - The address of the data inside function 1.
//  Buffer       - Buffer to transfer.
//  BufLength    - Length of the buffer to transfer.
//  Direction    - 0 to read, non 0 to write
//  
// Returns
//  SD_AP_STATUS
//
static
SD_API_STATUS SdNowByteModeTransfer(SD_DEVICE_HANDLE SdBusContext,
                                    UINT32 Address,
                                    PUCHAR Buffer, 
                                    UINT16 BufLength,
                                    UCHAR Direction)
{
    ULONG Cmd;
    SD_TRANSFER_CLASS Tc;
    UCHAR TOp;
    SD_COMMAND_RESPONSE CmdResponse;
    SD_API_STATUS Status;

    if(0 == Direction)
    {
        Tc = SD_READ;
        TOp = SD_IO_OP_READ;
    }
    else
    {
        Tc = SD_WRITE;
        TOp = SD_IO_OP_WRITE;
    }
    Cmd = BUILD_IO_RW_EXTENDED_ARG(TOp, SD_IO_BYTE_MODE, 1, Address, SD_IO_INCREMENT_ADDRESS, BufLength); 

    Status = SDSynchronousBusRequest(SdBusContext,
                                     SD_CMD_IO_RW_EXTENDED, 
                                     Cmd,
                                     Tc,
                                     ResponseR5, 
                                     &CmdResponse, 
                                     1, 
                                     BufLength, 
                                     Buffer, 
                                     0);

    return Status;
}

//
// SdNowWriteByte
//
// Arguments
//  Function     - Function within the card to write to.
//  Address      - The address of the data inside the selected function to write to
//  Data         - Pointer to data to write
//
// Returns
//  UniFi driver error code.
//
INT SdNowWriteByte(INT Function, ULONG Address, UCHAR Data)
{
    SD_API_STATUS Status;

    if(0 != Function)
    {
        Function = SdNowContext->FunctionNumber;
    }

    Status = SDReadWriteRegistersDirect(SdNowContext->SdBusContext, 
                                        SD_IO_WRITE , 
                                        Function, 
                                        Address, 
                                        FALSE, 
                                        &Data, 
                                        1);

    return convert_csr_sdio_error(Status);
}

//
// SdNowReadByte
//
// Arguments
//  Function - Function within the card to write to.
//  Address  - The address of the data inside the selected function to read from
//  Data     - Pointer to read data
//
// Returns
//  UniFi driver error code.
//
INT SdNowReadByte(INT Function, ULONG Address, PUCHAR Data)
{
    SD_API_STATUS Status;

    if(0 != Function)
    {
        Function = SdNowContext->FunctionNumber;
    }

    Status = SDReadWriteRegistersDirect(SdNowContext->SdBusContext, 
                                        SD_IO_READ, 
                                        Function, 
                                        Address, 
                                        FALSE, 
                                        Data, 
                                        1);

    return convert_csr_sdio_error(Status);
}
