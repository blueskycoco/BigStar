//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
/*++


Module Name:

    PinDriver.cpp

Abstract:

    MDD Implementation of Pin subdevice for Sample camera driver.

Notes:
    

Revision History:

--*/

#include <windows.h>
#include <pm.h>
#include <devload.h>
#include <nkintr.h>

#include "Cs.h"
#include "Csmedia.h"

#include "CameraPDDProps.h"
#include "dstruct.h"
#include "dbgsettings.h"
#include <camera.h>

#include "CameraDriver.h"
#include "PinDriver.h"


EXTERN_C
DWORD
PIN_Init(
    LPCTSTR Context,
    LPVOID lpvBusContext
    )
{    
    RETAILMSG( dbg, ( _T("PIN_Init: context %s\n"), Context ) );
    PPININITHANDLE pPinInitDev = NULL;


    if( OEM_CERTIFY_TRUST != CeGetCallerTrust() )
    {
        SetLastError(ERROR_ACCESS_DENIED);
    }
    else
    {
	    pPinInitDev = new PININITHANDLE;
	    
	    if ( NULL != pPinInitDev )
	    {
	        pPinInitDev->pCamDevice = reinterpret_cast<PCAMERADEVICE>( lpvBusContext );
	
	        if ( NULL == pPinInitDev->pCamDevice )
	        {
	            SetLastError( ERROR_INVALID_HANDLE );
	            SAFEDELETE( pPinInitDev );
	            RETAILMSG( dbg, ( _T("PIN_Init: Initialization Failed") ) );
	        }
	    }
	    else
	    {
	        SetLastError( ERROR_OUTOFMEMORY );
	    }
	}
    
    RETAILMSG( dbg, ( _T("PIN_Init: returning 0x%08x\r\n"), reinterpret_cast<DWORD>( pPinInitDev ) ) );
    
    return reinterpret_cast<DWORD>( pPinInitDev );
}


EXTERN_C
BOOL
PIN_Deinit(
    DWORD dwContext
    )
{
    RETAILMSG( dbg, ( _T("PIN_Deinit\r\n") ) );

    PPININITHANDLE pPinInitDev = reinterpret_cast<PPININITHANDLE>( dwContext );
    SAFEDELETE( pPinInitDev ) ;

    return TRUE;
}


EXTERN_C
BOOL
PIN_IOControl(
    DWORD  dwContext,
    DWORD  Ioctl,
    PUCHAR pInBufUnmapped,
    DWORD  InBufLen, 
    PUCHAR pOutBufUnmapped,
    DWORD  OutBufLen,
    PDWORD pdwBytesTransferred
   )
{
    //RETAILMSG(ZONE_FUNCTION, ( _T("PIN_IOControl(%08x): IOCTL:0x%x, InBuf:0x%x, InBufLen:%d, OutBuf:0x%x, OutBufLen:0x%x)\r\n"), dwContext, Ioctl, pInBufUnmapped, InBufLen, pOutBufUnmapped, OutBufLen ) );

    DWORD      dwErr      = ERROR_INVALID_PARAMETER;
    BOOL       bRc        = FALSE;
    PPINDEVICE pPinDevice = reinterpret_cast<PPINDEVICE>( dwContext );
    UCHAR * pInBuf = NULL;
    UCHAR * pOutBuf = NULL;

    pInBuf = (UCHAR*) MapCallerPtr( pInBufUnmapped, InBufLen );
    if (( pInBuf == NULL ) && ( pInBufUnmapped != NULL ))
    {
        RETAILMSG( dbg, ( _T("CAM_IOControl(%08x): CAM_IOControl. MapCallerPtr failed for incoming buffer.\r\n"), dwContext ) );

        return dwErr;
    }

    pOutBuf = (UCHAR*) MapCallerPtr( pOutBufUnmapped, OutBufLen );
    if (( NULL == pOutBuf ) && ( pOutBufUnmapped != NULL ))
    {
        RETAILMSG( dbg, ( _T("CAM_IOControl(%08x): CAM_IOControl. MapCallerPtr failed for incoming buffer.\r\n"), dwContext ) );

        return dwErr;
    }

    if ( NULL == MapCallerPtr( pdwBytesTransferred, sizeof ( DWORD ) ) )
    {
        RETAILMSG( dbg, ( _T("PIN_IOControl(%08x): PIN_IOControl. MapCallerPtr failed for dwBytesTransferred.\r\n"), dwContext ) );
        return dwErr;
    }
       
    switch ( Ioctl )
    {
    case IOCTL_CS_PROPERTY:
        //RETAILMSG( ZONE_IOCTL, ( _T("PIN_IOControl(%08x): IOCTL_CS_PROPERTY\r\n"), dwContext ) );

        __try 
        {
            CSPROPERTY csProp = {0};

            if ( ( NULL == pInBuf )
                 || ( InBufLen < sizeof ( CSPROPERTY ) )
                 || ( NULL == pdwBytesTransferred ) )
            {
                break;
            }
            
            if( !CeSafeCopyMemory( &csProp, pInBuf, sizeof( CSPROPERTY )))
            {
                break;
            }
            
            dwErr = pPinDevice->PinHandleCustomRequests( pInBuf,InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
            if ( ERROR_NOT_SUPPORTED == dwErr )
            {
                if ( TRUE == IsEqualGUID( csProp.Set, CSPROPSETID_Connection ) )
                {   
                    dwErr = pPinDevice->PinHandleConnectionRequests( &csProp, pOutBuf, OutBufLen, pdwBytesTransferred );
                }
            }
        }
        __except ( EXCEPTION_EXECUTE_HANDLER )
        {
            RETAILMSG( dbg, ( _T("PIN_IOControl(%08x):Exception in IOCTL_CS_PROPERTY"), dwContext ) );
        }

        break;


    case IOCTL_CS_BUFFERS:
        //RETAILMSG( ZONE_IOCTL, ( _T("PIN_IOControl(%08x): IOCTL_CS_BUFFERS\r\n"), dwContext ) );

        __try
        {
            DWORD  dwCommand;

            if(    ( NULL == pOutBuf )
                || ( NULL == pInBuf )
                || ( NULL == pdwBytesTransferred )
                || ( InBufLen  < sizeof( DWORD ))
                || ( OutBufLen < sizeof( CS_STREAM_DESCRIPTOR ))
                || ( !CeSafeCopyMemory( &dwCommand, pInBuf, sizeof( DWORD ))))
            {
                if( pdwBytesTransferred )
                {
                    *pdwBytesTransferred = sizeof( CS_STREAM_DESCRIPTOR );
                }

                RETAILMSG( dbg, ( _T("PIN_IOControl(%08x): IOCTL_CS_BUFFERS. Invalid parameters\r\n"), dwContext ) );
                break;
            }

            dwErr = pPinDevice->PinHandleBufferRequest( dwCommand, pOutBuf, OutBufLen, pdwBytesTransferred );
        }
        __except ( EXCEPTION_EXECUTE_HANDLER )
        {
            RETAILMSG( dbg, ( _T("PIN_IOControl(%08x):Exception in IOCTL_CS_BUFFERS"), dwContext ) );
        }

        break;


    case IOCTL_STREAM_INSTANTIATE:
        __try 
        {
            RETAILMSG( dbg, ( _T("PIN_IOControl(%08x): IOCTL_STREAM_INSTANTIATE\r\n"), dwContext ) );

            CSPROPERTY_STREAMEX_S csPropStreamEx = {0};
            
            if ( ( NULL == pInBuf )
                 || ( InBufLen < sizeof ( CSPROPERTY_STREAMEX_S ) )
                 || ( NULL == pdwBytesTransferred ) )
            {
                RETAILMSG( dbg, ( _T("PIN_IOControl(%08x): IOCTL_STREAM_INSTANTIATE. Insufficient buffer\r\n"), dwContext ) );
                break;
            }
            
            CeSafeCopyMemory( &csPropStreamEx, pInBuf, sizeof( CSPROPERTY_STREAMEX_S ));

            if ( TRUE == IsEqualGUID( csPropStreamEx.CsPin.Property.Set, CSPROPSETID_StreamEx ) )
            {
                switch ( csPropStreamEx.CsPin.Property.Id )
                {
                case CSPROPERTY_STREAMEX_INIT:
                    dwErr = pPinDevice->StreamInstantiate( &csPropStreamEx, pOutBuf, OutBufLen, pdwBytesTransferred );
					RETAILMSG( dbg, ( _T("CSPROPERTY_STREAMEX_INIT %x\r\n"), dwErr ) );
                    break;

                default:
                    RETAILMSG( dbg, ( _T("PIN_IOControl(%08x): Invalid Request\r\n"), dwContext ) );
                    break;
                }
            }
            else
            {
                RETAILMSG( dbg, ( _T("PIN_IOControl(%08x): Unsupported PropertySet Request for IOCTL_STREAM_INSTANTIATE%u\r\n"), dwContext, csPropStreamEx.CsPin.Property.Set ) );
                dwErr = ERROR_NOT_SUPPORTED;
            }
        }
        __except ( EXCEPTION_EXECUTE_HANDLER )
        {
            RETAILMSG( dbg, ( _T("PIN_IOControl(%08x):Exception in IOCTL_STREAM_INSTANTIATE"), dwContext ) );
        }

        break;
    
    default:
        RETAILMSG( dbg, ( _T("PIN_IOControl(%08x): Unsupported IOCTL code %u\r\n"), dwContext, Ioctl ) );
        dwErr = ERROR_NOT_SUPPORTED;
        break;
    }

    // pass back appropriate response codes
    SetLastError(dwErr);
    return ( ( dwErr == ERROR_SUCCESS ) ? TRUE : FALSE );
}


EXTERN_C
DWORD
PIN_Open(
    DWORD Context, 
    DWORD Access,
    DWORD ShareMode
    )
{
    RETAILMSG( dbg, ( _T("PIN_Open(%x, 0x%x, 0x%x)\r\n"), Context, Access, ShareMode ) );

    UNREFERENCED_PARAMETER( ShareMode );
    UNREFERENCED_PARAMETER( Access );
   
    PPININITHANDLE pPinInit = reinterpret_cast<PPININITHANDLE>( Context );

    ASSERT( NULL != pPinInit );
    PPINDEVICE pPinDevice = new PINDEVICE;

    if ( NULL == pPinDevice )
    {
        RETAILMSG( dbg, ( _T("PIN_Open(%x): Not enough memory to create pin device\r\n"), Context ) );
    }
    else
    {
        if ( false == pPinDevice->InitializeSubDevice( pPinInit->pCamDevice ) )
        {
            SAFEDELETE( pPinDevice );
            RETAILMSG( dbg, ( _T("PIN_Init: Initialization Failed") ) );
        }

    }

    return reinterpret_cast<DWORD>( pPinDevice );
}


EXTERN_C
BOOL  
PIN_Close(
    DWORD Context
    ) 
{
    RETAILMSG( dbg, ( _T("PIN_Close(%x)\r\n"), Context ) );
    
    PPINDEVICE pPinDev = reinterpret_cast<PPINDEVICE>( Context );
    pPinDev->CloseSubDevice( );
    SAFEDELETE( pPinDev );
    
    return TRUE;
}

