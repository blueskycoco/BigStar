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

#include <windows.h>
#include <pm.h>

#include "Cs.h"
#include "Csmedia.h"

#include "CameraPDDProps.h"
#include "dstruct.h"
#include "dbgsettings.h"

#include "smdk6410_camera.h"
#include <camera.h>
#include "CameraDriver.h"
#include "PinDriver.h"

#include "CameraPDD.h"
#include "camera_pdd.h"
#include "wchar.h"



#define	MSG_ERROR		1
#define MSG_INOUT		1
#define MSG_ON			1

PVOID PDD_Init( PVOID MDDContext, PPDDFUNCTBL pPDDFuncTbl )
{
	RETAILMSG(dbg,(TEXT("+++++++++++PDD_Init\n")));
    DWORD dwRet = ERROR_SUCCESS;
    CCameraPdd *pDD = new CCameraPdd();
    if ( pDD == NULL )
    {
        return NULL;
    }
    dwRet = pDD->PDDInit( MDDContext, pPDDFuncTbl );
    if( ERROR_SUCCESS != dwRet )
    {
        return NULL;
    }
    RETAILMSG(dbg,(TEXT("+----------PDD_Init\n")));
    return pDD;    

}

DWORD PDD_OpenInit( LPVOID PDDContext, DWORD dwDeviceIndex )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if( NULL == pDD )
    {
        return ERROR_INVALID_PARAMETER;
    }

	 pDD->PDDPropsInit(dwDeviceIndex);	
	 pDD->PDDVideoFormatsInit(dwDeviceIndex);	

    return ERROR_SUCCESS;
}

DWORD PDD_Closed( LPVOID PDDContext )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if( NULL == pDD )
    {
        return ERROR_INVALID_PARAMETER;
    }

	 pDD->PDDClosed();	

    return ERROR_SUCCESS;
}

DWORD PDD_DeInit( LPVOID PDDContext )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    pDD->PDD_DeInit();	
    if ( pDD != NULL )
    {
        delete pDD;
    }
    return ERROR_SUCCESS;
}

DWORD PDD_GetAdapterInfo( LPVOID PDDContext, PADAPTERINFO pAdapterInfo )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if( NULL == pDD )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->GetAdapterInfo( pAdapterInfo );
}

DWORD PDD_HandleVidProcAmpChanges( LPVOID PDDContext, DWORD dwPropId, LONG lFlags, LONG lValue)
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->HandleVidProcAmpChanges( dwPropId, lFlags, lValue );
}

DWORD PDD_HandleCamControlChanges( LPVOID PDDContext, DWORD dwPropId, LONG lFlags, LONG lValue )
{    
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->HandleCamControlChanges( dwPropId, lFlags, lValue );
}

DWORD PDD_HandleVideoControlCapsChanges( LPVOID PDDContext, LONG lModeType ,ULONG ulCaps )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->HandleVideoControlCapsChanges( lModeType, ulCaps );
}

DWORD PDD_SetPowerState( LPVOID PDDContext, CEDEVICE_POWER_STATE PowerState )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->SetPowerState( PowerState );
}


DWORD PDD_HandleAdapterCustomProperties( LPVOID PDDContext, PUCHAR pInBuf, DWORD  InBufLen, PUCHAR pOutBuf, DWORD  OutBufLen, PDWORD pdwBytesTransferred )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->HandleAdapterCustomProperties( pInBuf, InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
}


DWORD PDD_InitSensorMode( LPVOID PDDContext, ULONG ulModeType, LPVOID ModeContext )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->InitSensorMode( ulModeType, ModeContext );
}

DWORD PDD_DeInitSensorMode( LPVOID PDDContext, ULONG ulModeType )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->DeInitSensorMode( ulModeType );
}

DWORD PDD_SetSensorState( LPVOID PDDContext, ULONG ulModeType, CSSTATE CsState )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->SetSensorState( ulModeType, CsState );
}

DWORD PDD_TakeStillPicture( LPVOID PDDContext, LPVOID pBurstModeInfo )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->TakeStillPicture( pBurstModeInfo );
}

DWORD PDD_GetSensorModeInfo( LPVOID PDDContext, ULONG ulModeType, PSENSORMODEINFO pSensorModeInfo )
{
	RETAILMSG(dbg,(TEXT("PDD_GetSensorModeInfo\r\n")));
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->GetSensorModeInfo( ulModeType, pSensorModeInfo );
}

DWORD PDD_SetSensorModeFormat( LPVOID PDDContext, ULONG ulModeType, PCS_DATARANGE_VIDEO pCsDataRangeVideo )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->SetSensorModeFormat( ulModeType, pCsDataRangeVideo );
}

PVOID PDD_AllocateBuffer( LPVOID PDDContext, ULONG ulModeType )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return NULL;
    }

    return pDD->AllocateBuffer( ulModeType );
}

DWORD PDD_DeAllocateBuffer( LPVOID PDDContext, ULONG ulModeType, PVOID pBuffer )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->DeAllocateBuffer( ulModeType, pBuffer );
}

DWORD PDD_RegisterClientBuffer( LPVOID PDDContext, ULONG ulModeType, PVOID pBuffer )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->RegisterClientBuffer( ulModeType, pBuffer );
}

DWORD PDD_UnRegisterClientBuffer( LPVOID PDDContext, ULONG ulModeType, PVOID pBuffer )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->UnRegisterClientBuffer( ulModeType, pBuffer );
}

DWORD PDD_FillBuffer( LPVOID PDDContext, ULONG ulModeType, PUCHAR pImage )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->FillBuffer( ulModeType, pImage );
}

DWORD PDD_HandleModeCustomProperties( LPVOID PDDContext, ULONG ulModeType, PUCHAR pInBuf, DWORD  InBufLen, PUCHAR pOutBuf, DWORD  OutBufLen, PDWORD pdwBytesTransferred )
{
    CCameraPdd *pDD = (CCameraPdd *)PDDContext;
    if ( pDD == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return pDD->HandleSensorModeCustomProperties( ulModeType, pInBuf, InBufLen, pOutBuf, OutBufLen, pdwBytesTransferred );
}
