/*
 *  CSR Unifi driver.
 *
 *  Copyright (C) Cambridge Silicon Radio Ltd 2006. Confidential. http://www.csr.com
 *
 *  History
 *
 *  22-Jan-2007  Peter Kenyon   Initial coding
 *
 */

#include <windows.h>
#include <stdio.h>

// no need - if we arent debug then we wont get linked (in theory)
//#include "unifiver.h"

//-----------------------------------------------------------------

#if 1 //def UNIFI_DEBUG

// allocated on stack - actual size is twice this of course??
// no of chars we expect to be passed in
#define VPRINTF_MAX_CHARS     200

//-----------------------------------------------------------------

class UfmpTrace
{
public:
    UfmpTrace()
    {
        // try and find the export
        hDll = ::LoadLibrary( L"ufmp-trace.dll" );
        _ufmp_traceW = (UFMP_TRACEW*) ::GetProcAddress( hDll, L"ufmp_traceW" );
    }

    ~UfmpTrace()
    {
        // free up the dll
        ::FreeLibrary( hDll );
        hDll = NULL;

        // and clean up pointers???
        _ufmp_traceW = NULL;
    }

    void TraceW( const wchar_t* message, int len )
    {
        // if there is a dll then use it else dump to serial
        if ( _ufmp_traceW != NULL )
            _ufmp_traceW( message, len );
        else
            ::OutputDebugString( message );
    }

private:

    HINSTANCE hDll;

    typedef void UFMP_TRACEW( const wchar_t* message, int len );
    UFMP_TRACEW* _ufmp_traceW;
};

//-----------------------------------------------------------------

// exported function
// linking this ahead of coredll supresses output

extern "C"
void WINAPIV NKDbgPrintfW( LPCWSTR format, ... )
{
    static UfmpTrace trace;

    va_list args; va_start( args, format );

    wchar_t buffer[ VPRINTF_MAX_CHARS+1 ];
    int count = _vsnwprintf( buffer, VPRINTF_MAX_CHARS, format, args );

    trace.TraceW( buffer, count );
    
    va_end( args );
}


#endif
