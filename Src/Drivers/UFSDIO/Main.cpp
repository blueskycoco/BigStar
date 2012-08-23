/*
 *  CSR Unifi driver.
 *
 *  Copyright (C) Cambridge Silicon Radio Ltd 2006. Confidential. http://www.csr.com
 *
 *  History
 *
 *  20-Jan-2007  Peter Kenyon   Initial coding
 *
 */

extern "C"
{
#include "sdio_now.h"

// add the original api fns....
extern INT SdNowInitialize( PVOID ClientInitContext );
extern INT SdNowDeinitialize(VOID);
extern INT SdNowSetClockSpeed(INT Khz);
extern INT SdNowSetBusWidth(INT BusWidth);
extern INT SdNowSetBlockSize(INT BlockSize);
extern INT SdNowSetInterruptHandler(PUNIFI_INTERRUPT_HANDLER InterruptHandler, PVOID CardContext);
extern INT SdNowEnableInterrupt(INT Enable);
extern INT SdNowBlockReadWrite(ULONG Address, PUCHAR Data, UINT TransferLength, INT Direction);
extern INT SdNowWriteByte(INT Function, ULONG Address, UCHAR Data);
extern INT SdNowReadByte(INT Function, ULONG Address, PUCHAR Data);
}

#include "Version.h"

#define DLL_EXPORT     extern "C" __declspec(dllexport)

#define TRACE(msg)  //RETAILMSG(true,msg)


//------------------------------------------------------------------------------------
// API exported to bottom edge of unifi driver

// save the active path passed in during load (NDL_Init)
// and call it on the call to initialise
static wchar_t active_registry[ 256 ];


DLL_EXPORT
void ufsdio_load( const wchar_t* registry )
{
    TRACE(( L"ufsdio_load( %s )\n", registry ));
    //SdNowInitialize( (PVOID)registry );
    // just save the active path and call this later
    wcscpy( active_registry, registry );
}

DLL_EXPORT
void ufsdio_unload( void )
{
    TRACE(( L"ufsdio_unload()\n" ));

    SdNowDeinitialize();
}

DLL_EXPORT
int ufsdio_initialise( void* info )
{
    // we can use this to kick things off
    TRACE(( L"ufsdio_initialise()\n" ));

    // ok - make the call now
    return SdNowInitialize( active_registry );
}

//------------------------------------------------------------------------------------

DLL_EXPORT
const char* ufsdio_version( void )
{
    return DRIVER_NAME " Version: " VERSION_STRING;
}

//------------------------------------------------------------------------------------

DLL_EXPORT
int ufsdio_read_byte( int func, unsigned long addr, unsigned char* pdata )
{
    //TRACE(( L"<" ));
    return SdNowReadByte( func, addr, pdata );
}

DLL_EXPORT
int ufsdio_write_byte( int func, unsigned long addr, unsigned char data )
{
    //TRACE(( L">" ));
    return SdNowWriteByte( func, addr, data );
}

//------------------------------------------------------------------------------------

DLL_EXPORT
int ufsdio_read_block( unsigned long addr, unsigned char *pdata, unsigned int count )
{
    //TRACE(( L"R(%d)", count ));
    return SdNowBlockReadWrite( addr, pdata, count, 0 );
}

DLL_EXPORT
int ufsdio_write_block( unsigned long addr, const unsigned char *pdata, unsigned int count )
{
    //TRACE(( L"W(%d)", count ));
    return SdNowBlockReadWrite( addr, (PUCHAR)pdata, count, 1 );
}

//------------------------------------------------------------------------------------

DLL_EXPORT
int ufsdio_set_interrupt_handler( void (*handler)(const void *card), const void *cardptr )
{
    TRACE(( L"ufsdio_set_interrupt_handler(%p,%p)\n", handler, cardptr ));

    return SdNowSetInterruptHandler( (PUNIFI_INTERRUPT_HANDLER)handler, (PVOID)cardptr );
}

DLL_EXPORT
int ufsdio_enable_interrupt( void )
{
    //TRACE(( L"E" ));
    return SdNowEnableInterrupt( TRUE );
}


DLL_EXPORT
int ufsdio_disable_interrupt( void )
{
    //TRACE(( L"D" ));
    return SdNowEnableInterrupt( FALSE );
}

//DLL_EXPORT
//void ufsdio_interrupt_acknowledge( void )
//{
//}

//------------------------------------------------------------------------------------

DLL_EXPORT
int ufsdio_set_block_size( int blocksize )
{
    TRACE(( L"ufsdio_set_block_size(%d)\n", blocksize ));
    return SdNowSetBlockSize( blocksize );
}

DLL_EXPORT
int ufsdio_set_clock_speed( int speed_in_khz )
{
    TRACE(( L"ufsdio_set_clock_speed(%d)\n", speed_in_khz ));
    SdNowSetClockSpeed( speed_in_khz );
    return 0;
}

DLL_EXPORT
int ufsdio_set_bus_width( int width )
{
    TRACE(( L"ufsdio_set_bus_width(%d)\n", width ));
    return SdNowSetBusWidth( width );
}

//------------------------------------------------------------------------------------

// actually we dont need to export these and it should work

DLL_EXPORT
int ufsdio_power_on( void )
{
    TRACE(( L"Gen: ufsdio_power_on()\n" ));
	SdNowSetClockSpeed(25000);
    return 1;
}

DLL_EXPORT
int ufsdio_power_off( void )
{
    TRACE(( L"ufsdio_power_off()\n" ));
	SdNowSetClockSpeed(0);
    return 1;
}


//------------------------------------------------------------------------------------

DLL_EXPORT
int ufsdio_hard_reset( void )
{
    TRACE(( L"Gen: ufsdio_hard_reset()\n" ));
    return 1;
}

//-----------------------------------------------------------------

DLL_EXPORT
void ufsdio_idle( void )
{
	//TRACE(( L"Gen: ufsdio_idle\n"));
	SdNowSetClockSpeed(0);
}

DLL_EXPORT
void ufsdio_active( void )
{
	//TRACE(( L"Gen: ufsdio_active\n"));
	SdNowSetClockSpeed(25000);
}

//-----------------------------------------------------------------

#if 0
BOOL WINAPI DllMain( HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved )
{
    // in debug builds lets see if this really works
    Trace( Info, "UfsdioDriver::DllMain(0x%x, %d, %d)\n", hinstDLL, dwReason, lpvReserved );

    if ( dwReason == DLL_PROCESS_ATTACH )
    {
        Trace( Info, "Reason: ProcessAttach\n" );
        ::DisableThreadLibraryCalls( (HMODULE)hinstDLL );
    }
    else
    if ( dwReason == DLL_PROCESS_DETACH )
    {
        Trace( Info, "Reason: ProcessDetach\n" );
    }

    return TRUE;
}
#endif
