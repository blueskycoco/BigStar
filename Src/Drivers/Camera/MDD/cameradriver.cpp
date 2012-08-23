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

    CameraDriver.cpp

Abstract:

    MDD Adapter implementation

Notes:
    

Revision History:

--*/

#include <windows.h>
#include <devload.h>
#include <nkintr.h>
#include <pm.h>

#include "Cs.h"
#include "Csmedia.h"

#include "CameraPDDProps.h"
#include "dstruct.h"
#define CAMINTERFACE
#include "dbgsettings.h"
#include "CameraDriver.h"

static CAMERAOPENHANDLE * pOpenHandleForPM = NULL;
static PCAMERAINITHANDLE pCamInitHandle1 = NULL;
static PCAMERAINITHANDLE pCamInitHandle2 = NULL;
static CAMERADEVICE * pCamDev2in1 = NULL;


static DWORD GetCamDevIndex(VOID * pContext)
{
    DWORD           DevIndex,datasize;
    HKEY            hKey;
    ULONG           kvaluetype;

    hKey = OpenDeviceKey((LPCTSTR)pContext);
    if ( !hKey ) 
	{
		RETAILMSG(1, (_T("Failed to open context %s\r\n"), pContext));
        return -1;
    }

    datasize = sizeof(DWORD);
    if ( RegQueryValueEx(hKey, L"Index", NULL, &kvaluetype, (LPBYTE)&DevIndex, &datasize) ) 
	{
		RETAILMSG(1, (_T("Failed to get Index value\r\n")));
        RegCloseKey (hKey);
        return -1;
    }
	RETAILMSG(1, (_T("Index value = %d\r\n"), DevIndex));
	RegCloseKey (hKey);


	return DevIndex;
}


EXTERN_C
DWORD
CAM_Init(
    VOID * pContext
    )
{    
    CAMERADEVICE * pCamDev = NULL;
	DWORD dwCamDevIndex = 1;
    CAMERAINITHANDLE *pCamInitHandle = NULL;
    RETAILMSG(1, (_T("CAM_Init: context %s\r\n"), pContext));
	
    if( OEM_CERTIFY_TRUST != CeGetCallerTrust() )
    {
        SetLastError(ERROR_ACCESS_DENIED);
    }
    else
    {    
    
        pCamInitHandle = new CAMERAINITHANDLE;
        if ( NULL == pCamInitHandle )
        {
            RETAILMSG( ZONE_ERROR, ( _T("CAM_Init: Not enought memory to create open handle\r\n")) );
        }

		dwCamDevIndex = 1;//GetCamDevIndex(pContext);
		if(dwCamDevIndex == 1)
		{
		    pCamDev = new CAMERADEVICE;    
		    if ( NULL != pCamDev )
		    {
		    	pCamDev2in1 = pCamDev;
		    	pCamInitHandle->pCamDevice = pCamDev;
				pCamInitHandle1 = pCamInitHandle;
		        // NOTE: real drivers would need to validate pContext before dereferencing the pointer
		        if ( false == pCamDev->Initialize( pContext ) )
		        {
		            SetLastError( ERROR_INVALID_HANDLE );
		            SAFEDELETE( pCamDev );
		            RETAILMSG( ZONE_INIT|ZONE_ERROR, ( _T("CAM_Init: Initialization Failed\r\n") ) );
		        }
		    }
		    else
		    {
		        SetLastError( ERROR_OUTOFMEMORY );
	    	}
		}
		else if(dwCamDevIndex == 2)
		{
			pCamInitHandle->pCamDevice = pCamDev2in1;
			pCamInitHandle2 = pCamInitHandle;
		}
		else
		{
			RETAILMSG(1, (_T("CAM_Init: erro device index\r\n")));
            SetLastError( ERROR_INVALID_HANDLE );
            SAFEDELETE( pCamInitHandle );
            RETAILMSG( ZONE_INIT|ZONE_ERROR, ( _T("CAM_Init: Initialization Failed\r\n") ) );
		}

	}
    RETAILMSG( ZONE_INIT, ( _T("CAM_Init: returning 0x%08x\r\n"), reinterpret_cast<DWORD>( pCamInitHandle ) ) );

    return reinterpret_cast<DWORD>( pCamInitHandle );
}


EXTERN_C
BOOL
CAM_Deinit(
    DWORD dwContext
    )
{
    RETAILMSG( ZONE_INIT, ( _T("CAM_Deinit\r\n") ) );

    CAMERADEVICE * pCamDevice = reinterpret_cast<CAMERADEVICE *>( dwContext );
    SAFEDELETE( pCamDevice );

    return TRUE;
}


EXTERN_C
BOOL
CAM_IOControl(
    DWORD   dwContext,
    DWORD   Ioctl,
    UCHAR * pInBufUnmapped,
    DWORD   InBufLen, 
    UCHAR * pOutBufUnmapped,
    DWORD   OutBufLen,
    DWORD * pdwBytesTransferred
   )
{
    //RETAILMSG( ZONE_FUNCTION, ( _T("CAM_IOControl(%08x): IOCTL:0x%x, InBuf:0x%x, InBufLen:%d, OutBuf:0x%x, OutBufLen:0x%x)\r\n"), dwContext, Ioctl, pInBufUnmapped, InBufLen, pOutBufUnmapped, OutBufLen ) );

    UCHAR * pInBuf = NULL;
    UCHAR * pOutBuf = NULL;
    DWORD dwErr = ERROR_INVALID_PARAMETER;
    BOOL  bRc   = FALSE;
    
    // PM(Power Management) doesn't use CSPROPERTY structure. modified by JJG 07.06.25
    /*if ( ( NULL == pInBufUnmapped )
         || ( InBufLen < sizeof ( CSPROPERTY ) )
         || ( NULL == pdwBytesTransferred ) )    */
    if ( (Ioctl == IOCTL_CS_PROPERTY) && (( NULL == pInBufUnmapped )
         || ( InBufLen < sizeof ( CSPROPERTY ) )
         || ( NULL == pdwBytesTransferred )) )
    {
        SetLastError( dwErr );

        return bRc;
    }

    //All buffer accesses need to be protected by try/except
    pInBuf = (UCHAR*) MapCallerPtr( pInBufUnmapped, InBufLen );
    if(( pInBuf == NULL ) && ( pInBufUnmapped != NULL ))
    {
        RETAILMSG( ZONE_IOCTL|ZONE_ERROR, ( _T("CAM_IOControl(%08x): CAM_IOControl. MapCallerPtr failed for incoming buffer.\r\n"), dwContext ) );

        return dwErr;
    }

    pOutBuf = (UCHAR*) MapCallerPtr( pOutBufUnmapped, OutBufLen );
    if (( NULL == pOutBuf ) && ( pOutBufUnmapped != NULL ))
    {
        RETAILMSG( ZONE_IOCTL|ZONE_ERROR, ( _T("CAM_IOControl(%08x): CAM_IOControl. MapCallerPtr failed for incoming buffer.\r\n"), dwContext ) );

        return dwErr;
    }

    if ( NULL == MapCallerPtr( pdwBytesTransferred, sizeof ( DWORD ) ) )
    {
        RETAILMSG( ZONE_IOCTL|ZONE_ERROR, ( _T("CAM_IOControl(%08x): CAM_IOControl. MapCallerPtr failed for dwBytesTransferred.\r\n"), dwContext ) );

        return dwErr;
    }

    CAMERAOPENHANDLE * pCamOpenHandle = reinterpret_cast<CAMERAOPENHANDLE *>( dwContext );
    CAMERADEVICE     * pCamDevice     = pCamOpenHandle->pCamDevice;
    CSPROPERTY       * pCsProp        = reinterpret_cast<CSPROPERTY *>( MapCallerPtr( pInBuf, sizeof ( CSPROPERTY ) ) );
    
    // PM(Power Management) doesn't use CSPROPERTY structure.   modified by JJG 07.06.25
    //if ( NULL == pCsProp )
    if ( (Ioctl == IOCTL_CS_PROPERTY) && (NULL == pCsProp) )
    {
        RETAILMSG( ZONE_IOCTL|ZONE_ERROR, (_T("CAM_IOControl(%08x): Invalid Parameter.\r\n"), dwContext ) );
        return dwErr;
    }
    
    switch ( Ioctl )
    {
        // Power Management Support.
        case IOCTL_POWER_CAPABILITIES:
        case IOCTL_POWER_QUERY:
        case IOCTL_POWER_SET:
        case IOCTL_POWER_GET:
        {
            RETAILMSG( ZONE_IOCTL, ( _T("CAM_IOControl(%08x): Power Management IOCTL\r\n"), dwContext ) );
            __try 
            {
                dwErr = pCamDevice->AdapterHandlePowerRequests(Ioctl, pInBuf, InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
            }
            __except ( EXCEPTION_EXECUTE_HANDLER )
            {
                RETAILMSG( ZONE_IOCTL, ( _T("CAM_IOControl(%08x):Exception in Power Management IOCTL\r\n"), dwContext ) );
            }
            break;
        }

        case IOCTL_CS_PROPERTY:
        {
            //RETAILMSG( ZONE_IOCTL, ( _T("CAM_IOControl(%08x): IOCTL_CS_PROPERTY\r\n"), dwContext ) );

            __try 
            {
                dwErr = pCamDevice->AdapterHandleCustomRequests( pInBuf,InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );

                if ( ERROR_NOT_SUPPORTED == dwErr )
                {
                    if ( TRUE == IsEqualGUID( pCsProp->Set, CSPROPSETID_Pin ) )
                    {   
                        dwErr = pCamDevice->AdapterHandlePinRequests( pInBuf, InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
                    }
                    else if ( TRUE == IsEqualGUID( pCsProp->Set, CSPROPSETID_VERSION ) )
                    {
                        dwErr = pCamDevice->AdapterHandleVersion( pOutBuf, OutBufLen, pdwBytesTransferred );
                    }
                    else if ( TRUE == IsEqualGUID( pCsProp->Set, PROPSETID_VIDCAP_VIDEOPROCAMP ) )
                    {   
                        dwErr = pCamDevice->AdapterHandleVidProcAmpRequests( pInBuf,InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
                    }
                    else if ( TRUE == IsEqualGUID( pCsProp->Set, PROPSETID_VIDCAP_CAMERACONTROL ) )
                    {   
                        dwErr = pCamDevice->AdapterHandleCamControlRequests( pInBuf,InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
                    }else if ( TRUE == IsEqualGUID( pCsProp->Set, PROPSETID_VIDCAP_VIDEOCOMPRESSION ) )
                	{   
                    dwErr = pCamDevice->AdapterHandleCompressionRequests( pInBuf,InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
                	}
                    else if ( TRUE == IsEqualGUID( pCsProp->Set, PROPSETID_VIDCAP_VIDEOCONTROL ) )
                    {   
                        dwErr = pCamDevice->AdapterHandleVideoControlRequests( pInBuf,InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
                    }
                    else if ( TRUE == IsEqualGUID( pCsProp->Set, PROPSETID_VIDCAP_DROPPEDFRAMES) )
                    {   
                        dwErr = pCamDevice->AdapterHandleDroppedFramesRequests( pInBuf,InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
                    }
                }
            }
            __except ( EXCEPTION_EXECUTE_HANDLER )
            {
                RETAILMSG( ZONE_IOCTL, ( _T("CAM_IOControl(%08x):Exception in IOCTL_CS_PROPERTY\r\n"), dwContext ) );
            }

            break;
        }

        default:
        {
            RETAILMSG( ZONE_IOCTL, (_T("CAM_IOControl(%08x): Unsupported IOCTL code %u\r\n"), dwContext, Ioctl ) );
            dwErr = ERROR_NOT_SUPPORTED;

            break;
        }
    }
    
    // pass back appropriate response codes
    SetLastError( dwErr );

    return ( ( dwErr == ERROR_SUCCESS ) ? TRUE : FALSE );
}

extern DWORD dwTemVal;
//We wont open two camera app at the same time
EXTERN_C
DWORD
CAM_Open(
    DWORD Context, 
    DWORD Access,
    DWORD ShareMode
    )
{
    RETAILMSG( dbg, ( _T("CAM_Open(%x, 0x%x, 0x%x)\r\n"), Context, Access, ShareMode ) );

    UNREFERENCED_PARAMETER( ShareMode );
    UNREFERENCED_PARAMETER( Access );

    
    CAMERAINITHANDLE * pCamInitHandle = reinterpret_cast<CAMERAINITHANDLE *>( Context );
    CAMERADEVICE     * pCamDevice     = pCamInitHandle->pCamDevice;
    CAMERAOPENHANDLE * pCamOpenHandle = NULL;
    HANDLE             hCurrentProc   = NULL;
    
    hCurrentProc = GetCallerProcess();
    

    ASSERT( pCamDevice != NULL );
    
    if((Access == 0x0) && (ShareMode == (FILE_SHARE_READ|FILE_SHARE_WRITE)))
	{
		// PM calls
        pCamOpenHandle = new CAMERAOPENHANDLE;

        if ( NULL == pCamOpenHandle )
        {
            RETAILMSG( dbg, ( _T("CAM_Open(%x): Not enought memory to create open handle\r\n"), Context ) );
        }
        else
        {
            pCamOpenHandle->pCamDevice = pCamDevice;
            pOpenHandleForPM = pCamOpenHandle;
        }		
	}
    else if ( pCamDevice->BindApplicationProc( hCurrentProc ) )
    {
        pCamOpenHandle = new CAMERAOPENHANDLE;

        if ( NULL == pCamOpenHandle )
        {
            RETAILMSG( dbg, ( _T("CAM_Open(%x): Not enought memory to create open handle\r\n"), Context ) );
        }
        else
        {
            pCamOpenHandle->pCamDevice = pCamDevice;
			//if(pCamInitHandle1 == pCamInitHandle)
			//if(dwTemVal == 0)
				pCamDevice->CamOpenInit( 1 );
 			//else if(pCamInitHandle1 == pCamInitHandle)
			//else if(dwTemVal != 0)
			//	pCamDevice->CamOpenInit( 2 );
        }
    }
    else
    {
        SetLastError( ERROR_ALREADY_INITIALIZED );
    }

    return reinterpret_cast<DWORD>( pCamOpenHandle );
}


EXTERN_C
BOOL  
CAM_Close(
    DWORD Context
    ) 
{
    RETAILMSG( dbg, ( _T("CAM_Close(%x)\r\n"), Context ) );
    
    PCAMERAOPENHANDLE pCamOpenHandle = reinterpret_cast<PCAMERAOPENHANDLE>( Context );
    CAMERADEVICE     * pCamDevice     = pCamOpenHandle->pCamDevice;

	if(pOpenHandleForPM == pCamOpenHandle)
	{
		pOpenHandleForPM = NULL;
	}
	else
	{
    	pCamOpenHandle->pCamDevice->UnBindApplicationProc( );
		pCamDevice->CamClosed();
    }
    SAFEDELETE( pCamOpenHandle ) ;

    return TRUE;
}



#if 0
Ïà»úÇý¶¯µÄ¼¸µãË¼¿¼:
1.ÊÇ²úÉúÁ½¸öÉè±¸»¹ÊÇÒ»¸öÉè±¸å/¼´ÔÚÃ¿´Îµ÷ÓÃcam-init¶¼²úÉúÒ»¸öcameradeviceÉè±¸»¹ÊÇÖ»ÔÚµÚÒ»´Îµ÷ÓÃÊ±²úÉúÒ»¸ö£¬ÔÚºóÃæµÄµ÷ÓÃÊ±¶¼·µ»ØÇ°Ò»¸öÖ¸Õë¡£
Èç¹ûÊ¹ÓÃºóÕß£¬¿ÉÒÔ±ÜÃâÖØ¸´Ð´ºÜ¶à´úÂë£¬²¢±ãÓÚÍ¨¹ýLTK²âÊÔ¡£Ö»ÊÇcameraÉè±¸´úÂëÖÐÒªÇø·ÖÊÇµÚ¼¸¸öÉè±¸¡£
Èç¹ûÊ¹ÓÃÇ°Õß£¬ÐèÒªÐ´cameradeviceµÄÅÉÉúÀàºÍcamerapddµÄÅÉÉúÀà(¶Ô²»Í¬µÄÉè±¸¶¼ÒªÓÐ¶ÔÓ¦µÄÅÉÉúÀà)¡£
ÔÝÊ±¾ö¶¨ºóÕß¡£

VGA(Video Graphics Array),640*480,×Ýºá±È4:3 
SVGA(Super Video Graphics Array),800*600,×Ýºá±È4:3 
XGA£¨Extended Graphics Array),1024*768,×Ýºá±È4:3 
SXGA(Super Extended Graphics Array£©,1280*1024£¬×Ýºá±È5:4 
SXGA+(Super Extended Graphics Array+),1400*1050,×Ýºá±È4:3 
UXGA(Ultra Extended Graphics Array),1600*1200,×Ýºá±È4:3 

WXGA£¨Wide Extended Graphics Array),1280*800,×Ýºá±È16:10 
WXGA+(Wide Extended Graphics Array+),1280*854/1440*900,×Ýºá±È15:10/16:10 
WSXGA(Wide Super Extended Graphics Array),1600*1024,×Ýºá±È14:9 
WSXGA+(Wide Super Extended Graphics Array+),1680*1050,×Ýºá±È16:10 
WUXGA(Wide Ultra Extended Graphics Array),1920*1200,×Ýºá±È16:10
#endif
