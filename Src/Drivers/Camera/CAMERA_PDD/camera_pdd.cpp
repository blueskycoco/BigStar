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
#include "SensorFormats.h"
#include "SensorProperties.h"
#include "PinDriver.h"

#include "CameraPDD.h"
#include "camera_pdd.h"
#include "wchar.h"



#define	MSG_ERROR		1
#define	MSG_INFO		1
#define MSG_INOUT		1
#define MSG_ON			1

// by Mickey
#define OV_CAPTURE_FORMATS_COUNT   1 //5
#define OV_STILLSHOT_FORMATS_COUNT 1 // 7
#define OV_PREVIEW_FORMATS_COUNT   1 //5

#define NOON_CAPTURE_FORMATS_COUNT   3
#define NOON_STILLSHOT_FORMATS_COUNT 3
#define NOON_PREVIEW_FORMATS_COUNT   3

// Pointer to camera driver instance which we will send back with callback functions
extern DWORD dwCameraDriverContext;    

// Signals the application that the video or still image frame is available
extern PFNCAMHANDLEFRAME pfnCameraHandleVideoFrame;
extern PFNCAMHANDLEFRAME pfnCameraHandleStillFrame;
extern PFNCAMHANDLEFRAME pfnCameraHandlePreviewFrame;

PDDFUNCTBL FuncTbl = {
    sizeof(PDDFUNCTBL),
    PDD_Init,
    PDD_DeInit,
    PDD_OpenInit,
    PDD_Closed,
    PDD_GetAdapterInfo,
    PDD_HandleVidProcAmpChanges,
    PDD_HandleCamControlChanges,
    PDD_HandleVideoControlCapsChanges,
    PDD_SetPowerState,
    PDD_HandleAdapterCustomProperties,
    PDD_InitSensorMode,
    PDD_DeInitSensorMode,
    PDD_SetSensorState,
    PDD_TakeStillPicture,
    PDD_GetSensorModeInfo,
    PDD_SetSensorModeFormat,
    PDD_AllocateBuffer,
    PDD_DeAllocateBuffer,
    PDD_RegisterClientBuffer,
    PDD_UnRegisterClientBuffer,
    PDD_FillBuffer,
    PDD_HandleModeCustomProperties
};

const POWER_CAPABILITIES s_PowerCaps = 
{
    // DeviceDx:    Supported power states
    DX_MASK(D0 )| DX_MASK(D4),

    0,              // WakeFromDx:
    0,              // InrushDx:    No inrush of power

    {               // Power: Maximum milliwatts in each state
        0x00000001, //        D0 = 0
        0x00000001, //        D1 = 0
        0x00000001, //        D2 = 0
        0x00000001, //        D3 = 0
        0x00000001  //        D4 = 0 (off)
    },

    {               // Latency
        0x00000000, //        D0 = 0
        0x00000000, //        D1 = 0
        0x00000000, //        D2 = 0
        0x00000000, //        D3 = 0
        0x00000000  //        D4 = 0
    },

    0,                    // Flags: None
};


_inline	UINT	Trunc(UINT value)
{ 
	return ((int) value) < 0 ? 0 : ((int) value) > 0xff00 ? 0xff00 : value; 
}


static DWORD JpegDataProcess(PUCHAR pData, PUCHAR* pJpgData, UINT32 nWidth, UINT32 nHeight)
{
    PBYTE pSecJpg=NULL,pDstPixel = NULL,pTemPixel = NULL;
	BYTE  TemPixel=0;
	UINT32  JpegDataSize=0,i=0,dwPos=0;
	UINT32 iJPGHeaderSize=0, iMaxsize=JPG_OUT_SIZE;//nWidth*abs(nHeight);
	const UINT32 JpgSkipSize = 0;
	
	//RETAILMSG(MSG_INFO, ( L"#OV3640_JPG##\r\n" ));
	pSecJpg = pData;

	RETAILMSG(dbg , ( L"#pSecJpg first byte is %x ###\r\n",*pSecJpg ));
	if(*pSecJpg==0)
	{
		pSecJpg++ ;
	}

	//pSecJpg += JpgSkipSize;		//we skip the first jpg
	i = 0;
	while(i<(iMaxsize-JpgSkipSize))
	{
		if(0xFF==*(pSecJpg+i))
		{
			if(0xD8==*(pSecJpg+i+1))
			{
				if(0x4A==*(pSecJpg+i+6) && 0x46==*(pSecJpg+i+7) && 0x49==*(pSecJpg+i+8) && 0x46==*(pSecJpg+i+9))
				RETAILMSG(1, ( L"#Found the jpg head i=%d %2x %2x %2x %2x@@@@@@@@\r\n",i,*(pSecJpg+i),*(pSecJpg+i+1),*(pSecJpg+i+7),*(pSecJpg+i+8) ));
				*pJpgData = pSecJpg+i;
				break;
			}
		}
		i++;
	}
	while(i<(iMaxsize-JpgSkipSize))
	{
		if(0xFF==*(pSecJpg+i))
		{
			if(0xD9==*(pSecJpg+i+1))
			{
				JpegDataSize =  dwPos+2;
				RETAILMSG(1, ( L"#Found the jpg end i=%d %2x %2x %2x %2x@@size=%d@@@@@@\r\n",i,*(pSecJpg+i),*(pSecJpg+i+1),*(pSecJpg+i+2),*(pSecJpg+i+3), JpegDataSize ));
				break;
			}
		}
		dwPos++;
		i++;
	}

	if(i==(iMaxsize-JpgSkipSize))	//failed to get jpg data
	{
		pSecJpg = pData;
		JpegDataSize=10000;
		*pJpgData = pData;
		RETAILMSG(dbg, ( L"#failed to get jpg data########\r\n" ));
	}
	
	i = 0;

	RETAILMSG(1, ( L"#To dshow JpegDataSize=%d ptr=%x %d %d##\r\n",JpegDataSize,pSecJpg,nWidth,nHeight ));
	return JpegDataSize;
}


static void WriteJPGDataFile(BYTE* lpSrc, int size)
{
	HANDLE hFile = NULL;
	DWORD dwBytes;
	LPWSTR wsStr;
	char buf[256] = {0,};
	int alen =0, wlen = 0;
	sprintf(buf, "\\Temp\\campic%d.jpg", size);
	alen = strlen(buf);
	wlen = MultiByteToWideChar(CP_ACP, 0, buf, alen, 0, 0);
	wsStr = (LPWSTR)calloc(wlen+1, sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, 0, buf, alen, wsStr, wlen);

	//hFile = CreateFile((TEXT(name)), GENERIC_WRITE, 0, NULL, CREATE_NEW/*|OPEN_EXISTING*/, 0, NULL);
	hFile = CreateFile(wsStr, GENERIC_WRITE, 0, NULL, CREATE_NEW/*|OPEN_EXISTING*/, 0, NULL);
	//SetFilePointer(hFile, 0, 0, FILE_END );
	WriteFile(hFile, lpSrc, size/*1024*1536*2*/, &dwBytes, NULL);
	//WriteFile(hFile, (PUCHAR)" \r\n\r\n", 8/*1024*1536*2*/, &dwBytes, NULL);
	CloseHandle(hFile);
	free(wsStr);
	return;
}

CCameraPdd::CCameraPdd()
{
    m_ulCTypes = 2;
    m_bStillCapInProgress = false;
    m_hContext = NULL;
    m_pModeVideoFormat = NULL;
    m_pModeVideoCaps = NULL;
    m_ppModeContext = NULL;
    m_pCurrentFormat = NULL;
	m_iPinUseCount	 = 0;
	m_PowerState = D4;	
    m_bCameraPreviewRunning = FALSE;
    m_bCameraVideoRunning = FALSE;
	m_bCameraPreviewWasRunning = FALSE;
	m_bCameraVideoWasRunning = FALSE;    
    InitializeCriticalSection( &m_csPddDevice );

    memset( &m_CsState, 0x0, sizeof(m_CsState));
    memset( &m_SensorModeInfo, 0x0, sizeof(m_SensorModeInfo));
    memset( &m_SensorProps, 0x0, sizeof(m_SensorProps));
    memset( &PowerCaps, 0x0, sizeof(PowerCaps));

	m_dwCurrentDeviceIndex = 1;

	m_bLtkMode = FALSE;

	m_pMappedStruct = NULL;
	m_bNewPFramedata = FALSE;
	m_bNewSFramedata = FALSE;
	m_hPFrameDataOKEvent = NULL;
	m_hSFrameDataOKEvent = NULL;
	m_hPreStill = NULL;
	m_bDShowPreviewDataEnable = TRUE;
	m_bGetStillDataDirectEnable = FALSE;
	m_bContinousStillEnable = FALSE;

}

CCameraPdd::~CCameraPdd()
{

	DeleteCriticalSection( &m_csPddDevice );

    if( NULL != m_pModeVideoCaps )
    {
        delete [] m_pModeVideoCaps;
        m_pModeVideoCaps = NULL;
    }

    if( NULL != m_pCurrentFormat )
    {
        delete [] m_pCurrentFormat;
        m_pCurrentFormat = NULL;
    }

    if( NULL != m_pModeVideoFormat )
    {
        delete [] m_pModeVideoFormat;
        m_pModeVideoFormat = NULL;
    }

    if( NULL != m_ppModeContext )
    {
        delete [] m_ppModeContext;
        m_ppModeContext = NULL;
    }

}

DWORD CCameraPdd::PDDInit( PVOID MDDContext, PPDDFUNCTBL pPDDFuncTbl )
{
    m_hContext = (HANDLE)MDDContext;
    // Real drivers may want to create their context

	    RETAILMSG(dbg,(TEXT("PDDInit\r\n")));
    if (!CameraInit(MDDContext))
    {
        RETAILMSG( dbg, ( _T("PddInit:FAILED to Initialize camera\r\n") ) );
        return ERROR_GEN_FAILURE;
    }

    dwCameraDriverContext = reinterpret_cast<DWORD>( this );
	
    // This callback function is used to signal the application that the video frame is now available.
    pfnCameraHandleVideoFrame = CCameraPdd::CameraVideoFrameCallback;
	
    // This callback function is used to signal the application that the still image is now available.
    pfnCameraHandleStillFrame = CCameraPdd::CameraStillFrameCallback;
	
	// This callback function is used to signal the application that the Preview image is now available.
    pfnCameraHandlePreviewFrame = CCameraPdd::CameraPreviewFrameCallback;


    m_ulCTypes = 3; // Default number of Sensor Modes is 2, Preview
	
    // Read registry to override the default number of Sensor Modes.
    ReadMemoryModelFromRegistry();  

    if( pPDDFuncTbl->dwSize  > sizeof( PDDFUNCTBL ) )
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    memcpy( pPDDFuncTbl, &FuncTbl, sizeof( PDDFUNCTBL ) );

    memset( m_SensorProps, 0x0, sizeof(m_SensorProps) );

    memcpy( &PowerCaps, &s_PowerCaps, sizeof( POWER_CAPABILITIES ) );

	//-------------------------------------------------------------
	//...	//code here has been moved to PDDPropsInit
	//-------------------------------------------------------------
	
    m_pModeVideoFormat = NULL;
    // Allocate Video Format specific array.
    m_pModeVideoFormat = new PINVIDEOFORMAT[m_ulCTypes];
    if( NULL == m_pModeVideoFormat )
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    // Video Format initialization
    m_pModeVideoFormat[CAPTURE].categoryGUID         = PINNAME_VIDEO_CAPTURE;
    m_pModeVideoFormat[CAPTURE].ulAvailFormats       = OV_CAPTURE_FORMATS_COUNT;
    m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo = new PCS_DATARANGE_VIDEO[m_pModeVideoFormat[CAPTURE].ulAvailFormats];

    if( NULL == m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo )
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    m_pModeVideoFormat[STILL].categoryGUID           = PINNAME_VIDEO_STILL;
    m_pModeVideoFormat[STILL].ulAvailFormats         = OV_STILLSHOT_FORMATS_COUNT;
    m_pModeVideoFormat[STILL].pCsDataRangeVideo = new PCS_DATARANGE_VIDEO[m_pModeVideoFormat[STILL].ulAvailFormats];

    if( NULL == m_pModeVideoFormat[STILL].pCsDataRangeVideo )
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    if( 3 == m_ulCTypes )
    {
        m_pModeVideoFormat[PREVIEW].categoryGUID         = PINNAME_VIDEO_PREVIEW;      
        m_pModeVideoFormat[PREVIEW].ulAvailFormats       = OV_PREVIEW_FORMATS_COUNT;
        m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo = new PCS_DATARANGE_VIDEO[m_pModeVideoFormat[PREVIEW].ulAvailFormats];

        if( NULL == m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo )
        {
            return ERROR_INSUFFICIENT_BUFFER;
        }
    }

	//-------------------------------------------------------------
	//...	//code here has been moved to PDDVideoFormatsInit
	//-------------------------------------------------------------

    m_pModeVideoCaps = NULL;
    // Allocate Video Control Caps specific array.
    m_pModeVideoCaps = new VIDCONTROLCAPS[m_ulCTypes];
    if( NULL == m_pModeVideoCaps )
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }
    // Video Control Caps

    // Video Control Caps
    m_pModeVideoCaps[CAPTURE].DefaultVideoControlCaps     = DefaultVideoControlCaps[CAPTURE];
    m_pModeVideoCaps[CAPTURE].CurrentVideoControlCaps     = DefaultVideoControlCaps[CAPTURE];
    m_pModeVideoCaps[STILL].DefaultVideoControlCaps       = DefaultVideoControlCaps[STILL];
    m_pModeVideoCaps[STILL].CurrentVideoControlCaps       = DefaultVideoControlCaps[STILL];
    if( 3 == m_ulCTypes )
    {
        // Note PREVIEW control caps are the same, so we don't differentiate
        m_pModeVideoCaps[PREVIEW].DefaultVideoControlCaps     = DefaultVideoControlCaps[PREVIEW];
        m_pModeVideoCaps[PREVIEW].CurrentVideoControlCaps     = DefaultVideoControlCaps[PREVIEW];
    }

   // m_SensorModeInfo[CAPTURE].MemoryModel = CSPROPERTY_BUFFER_CLIENT_UNLIMITED;
    m_SensorModeInfo[CAPTURE].MaxNumOfBuffers = 1;
    m_SensorModeInfo[CAPTURE].PossibleCount = 1;
   // m_SensorModeInfo[STILL].MemoryModel = CSPROPERTY_BUFFER_CLIENT_UNLIMITED;
    m_SensorModeInfo[STILL].MaxNumOfBuffers = 1;
    m_SensorModeInfo[STILL].PossibleCount = 1;
    if( 3 == m_ulCTypes )
    {
   		// m_SensorModeInfo[PREVIEW].MemoryModel = CSPROPERTY_BUFFER_CLIENT_UNLIMITED;
        m_SensorModeInfo[PREVIEW].MaxNumOfBuffers = 1;
        m_SensorModeInfo[PREVIEW].PossibleCount = 1;
    }

    m_ppModeContext = new LPVOID[m_ulCTypes];
    if ( NULL == m_ppModeContext )
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    m_pCurrentFormat = new CS_DATARANGE_VIDEO[m_ulCTypes];
    if( NULL == m_pCurrentFormat )
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }

	m_hPFrameDataOKEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("PREVIEW_FRAMEOK"));
	m_hSFrameDataOKEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("STILL_FRAMEOK"));
	m_hPreStill = CreateEvent(NULL,FALSE,FALSE,CAM1_PRESTILL);
	m_FrameData.dwDataLen = 0;
	m_FrameData.pDataBuf = NULL;

    return ERROR_SUCCESS;
}


void CCameraPdd::PDD_DeInit()
{
	RETAILMSG(dbg,(TEXT("PDD_DeInit\r\n")));
    CameraDeinit();

    if( NULL != m_ppModeContext )
    {
        delete [] m_ppModeContext;
        m_ppModeContext = NULL;
    }

    if( NULL != m_pModeVideoCaps )
    {
        delete [] m_pModeVideoCaps;
        m_pModeVideoCaps = NULL;
    }

    if( NULL != m_pModeVideoFormat )
    {
        if (NULL != m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo)
        {
            delete [] m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo;
            m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo = NULL;
        }

        if (NULL != m_pModeVideoFormat[STILL].pCsDataRangeVideo)
        {
            delete [] m_pModeVideoFormat[STILL].pCsDataRangeVideo;
            m_pModeVideoFormat[STILL].pCsDataRangeVideo = NULL;
        }

        if (NULL != m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo)
        {
            delete [] m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo;
            m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo = NULL;
        }

        delete [] m_pModeVideoFormat;
        m_pModeVideoFormat = NULL;
    }

    if (m_hPreStill!=NULL)
    {
        CloseHandle(m_hPreStill);
        m_hPreStill=NULL;
    }
	
    if (m_hPFrameDataOKEvent!=NULL)
    {
        CloseHandle(m_hPFrameDataOKEvent);
        m_hPFrameDataOKEvent=NULL;
    }
	
    if (m_hSFrameDataOKEvent!=NULL)
    {
        CloseHandle(m_hSFrameDataOKEvent);
        m_hSFrameDataOKEvent=NULL;
    }
}


DWORD CCameraPdd::PDDPropsInit(DWORD dwDeviceIndex)
{
	RETAILMSG(dbg,(TEXT("[CAM]PDDPropsInit dwDeviceIndex=%d\r\n"),dwDeviceIndex));
	m_dwCurrentDeviceIndex = dwDeviceIndex;

    // Set all VideoProcAmp and CameraControl properties.
    //VideoProcAmp
    m_SensorProps[ENUM_BRIGHTNESS].ulCurrentValue     = BrightnessDefault;
    m_SensorProps[ENUM_BRIGHTNESS].ulDefaultValue     = BrightnessDefault;
    m_SensorProps[ENUM_BRIGHTNESS].pRangeNStep        = &BrightnessRangeAndStep[0];
    m_SensorProps[ENUM_BRIGHTNESS].ulFlags            = CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
    m_SensorProps[ENUM_BRIGHTNESS].ulCapabilities     = CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL|CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_BRIGHTNESS].fSetSupported      = VideoProcAmpProperties[ENUM_BRIGHTNESS].SetSupported;
    m_SensorProps[ENUM_BRIGHTNESS].fGetSupported      = VideoProcAmpProperties[ENUM_BRIGHTNESS].GetSupported;
    m_SensorProps[ENUM_BRIGHTNESS].pCsPropValues      = &BrightnessValues;

    m_SensorProps[ENUM_CONTRAST].ulCurrentValue       = ContrastDefault;
    m_SensorProps[ENUM_CONTRAST].ulDefaultValue       = ContrastDefault;
    m_SensorProps[ENUM_CONTRAST].pRangeNStep          = &ContrastRangeAndStep[0];
    m_SensorProps[ENUM_CONTRAST].ulFlags              = CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
    m_SensorProps[ENUM_CONTRAST].ulCapabilities       = CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL|CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_CONTRAST].fSetSupported        = VideoProcAmpProperties[ENUM_CONTRAST].SetSupported;
    m_SensorProps[ENUM_CONTRAST].fGetSupported        = VideoProcAmpProperties[ENUM_CONTRAST].GetSupported;
    m_SensorProps[ENUM_CONTRAST].pCsPropValues        = &ContrastValues;

    m_SensorProps[ENUM_HUE].ulCurrentValue            = HueDefault;
    m_SensorProps[ENUM_HUE].ulDefaultValue            = HueDefault;
    m_SensorProps[ENUM_HUE].pRangeNStep               = &HueRangeAndStep[0];
    m_SensorProps[ENUM_HUE].ulFlags                   = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_HUE].ulCapabilities            = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_HUE].fSetSupported             = VideoProcAmpProperties[ENUM_HUE].SetSupported;
    m_SensorProps[ENUM_HUE].fGetSupported             = VideoProcAmpProperties[ENUM_HUE].GetSupported;
    m_SensorProps[ENUM_HUE].pCsPropValues             = &HueValues;

    m_SensorProps[ENUM_SATURATION].ulCurrentValue     = SaturationDefault;
    m_SensorProps[ENUM_SATURATION].ulDefaultValue     = SaturationDefault;
    m_SensorProps[ENUM_SATURATION].pRangeNStep        = &SaturationRangeAndStep[0];
    m_SensorProps[ENUM_SATURATION].ulFlags            = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_SATURATION].ulCapabilities     = CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL|CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_SATURATION].fSetSupported      = VideoProcAmpProperties[ENUM_SATURATION].SetSupported;
    m_SensorProps[ENUM_SATURATION].fGetSupported      = VideoProcAmpProperties[ENUM_SATURATION].GetSupported;
    m_SensorProps[ENUM_SATURATION].pCsPropValues      = &SaturationValues;

    m_SensorProps[ENUM_SHARPNESS].ulCurrentValue      = SharpnessDefault;
    m_SensorProps[ENUM_SHARPNESS].ulDefaultValue      = SharpnessDefault;
    m_SensorProps[ENUM_SHARPNESS].pRangeNStep         = &SharpnessRangeAndStep[0];
    m_SensorProps[ENUM_SHARPNESS].ulFlags             = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_SHARPNESS].ulCapabilities      = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_SHARPNESS].fSetSupported       = VideoProcAmpProperties[ENUM_SHARPNESS].SetSupported;
    m_SensorProps[ENUM_SHARPNESS].fGetSupported       = VideoProcAmpProperties[ENUM_SHARPNESS].GetSupported;
    m_SensorProps[ENUM_SHARPNESS].pCsPropValues       = &SharpnessValues;

    m_SensorProps[ENUM_GAMMA].ulCurrentValue          = GammaDefault;
    m_SensorProps[ENUM_GAMMA].ulDefaultValue          = GammaDefault;
    m_SensorProps[ENUM_GAMMA].pRangeNStep             = &GammaRangeAndStep[0];
    m_SensorProps[ENUM_GAMMA].ulFlags                 = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_GAMMA].ulCapabilities          = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_GAMMA].fSetSupported           = VideoProcAmpProperties[ENUM_GAMMA].SetSupported;
    m_SensorProps[ENUM_GAMMA].fGetSupported           = VideoProcAmpProperties[ENUM_GAMMA].GetSupported;
    m_SensorProps[ENUM_GAMMA].pCsPropValues           = &GammaValues;

    m_SensorProps[ENUM_COLORENABLE].ulCurrentValue    = ColorEnableDefault;
    m_SensorProps[ENUM_COLORENABLE].ulDefaultValue    = ColorEnableDefault;
    m_SensorProps[ENUM_COLORENABLE].pRangeNStep       = &ColorEnableRangeAndStep[0];
    m_SensorProps[ENUM_COLORENABLE].ulFlags           = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_COLORENABLE].ulCapabilities    = CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL|CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_COLORENABLE].fSetSupported     = VideoProcAmpProperties[ENUM_COLORENABLE].SetSupported;
    m_SensorProps[ENUM_COLORENABLE].fGetSupported     = VideoProcAmpProperties[ENUM_COLORENABLE].GetSupported;
    m_SensorProps[ENUM_COLORENABLE].pCsPropValues     = &ColorEnableValues;

    m_SensorProps[ENUM_WHITEBALANCE].ulCurrentValue   = WhiteBalanceDefault;
    m_SensorProps[ENUM_WHITEBALANCE].ulDefaultValue   = WhiteBalanceDefault;
    m_SensorProps[ENUM_WHITEBALANCE].pRangeNStep      = &WhiteBalanceRangeAndStep[0];
    m_SensorProps[ENUM_WHITEBALANCE].ulFlags          = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_WHITEBALANCE].ulCapabilities   = CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL|CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_WHITEBALANCE].fSetSupported    = VideoProcAmpProperties[ENUM_WHITEBALANCE].SetSupported;
    m_SensorProps[ENUM_WHITEBALANCE].fGetSupported    = VideoProcAmpProperties[ENUM_WHITEBALANCE].GetSupported;
    m_SensorProps[ENUM_WHITEBALANCE].pCsPropValues    = &WhiteBalanceValues;

    m_SensorProps[ENUM_BACKLIGHT_COMPENSATION].ulCurrentValue = BackLightCompensationDefault;
    m_SensorProps[ENUM_BACKLIGHT_COMPENSATION].ulDefaultValue = BackLightCompensationDefault;
    m_SensorProps[ENUM_BACKLIGHT_COMPENSATION].pRangeNStep    = &BackLightCompensationRangeAndStep[0];
    m_SensorProps[ENUM_BACKLIGHT_COMPENSATION].ulFlags        = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_BACKLIGHT_COMPENSATION].ulCapabilities = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_BACKLIGHT_COMPENSATION].fSetSupported  = VideoProcAmpProperties[ENUM_BACKLIGHT_COMPENSATION].SetSupported;
    m_SensorProps[ENUM_BACKLIGHT_COMPENSATION].fGetSupported  = VideoProcAmpProperties[ENUM_BACKLIGHT_COMPENSATION].GetSupported;
    m_SensorProps[ENUM_BACKLIGHT_COMPENSATION].pCsPropValues  = &BackLightCompensationValues;

    m_SensorProps[ENUM_GAIN].ulCurrentValue           = GainDefault;
    m_SensorProps[ENUM_GAIN].ulDefaultValue           = GainDefault;
    m_SensorProps[ENUM_GAIN].pRangeNStep              = &GainRangeAndStep[0];
    m_SensorProps[ENUM_GAIN].ulFlags                  = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_GAIN].ulCapabilities           = CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    m_SensorProps[ENUM_GAIN].fSetSupported            = VideoProcAmpProperties[ENUM_GAIN].SetSupported;
    m_SensorProps[ENUM_GAIN].fGetSupported            = VideoProcAmpProperties[ENUM_GAIN].GetSupported;
    m_SensorProps[ENUM_GAIN].pCsPropValues            = &GainValues;

    //CameraControl
    m_SensorProps[ENUM_PAN].ulCurrentValue            = PanDefault;
    m_SensorProps[ENUM_PAN].ulDefaultValue            = PanDefault;
    m_SensorProps[ENUM_PAN].pRangeNStep               = &PanRangeAndStep[0];
    m_SensorProps[ENUM_PAN].ulFlags                   = CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_PAN].ulCapabilities            = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL|CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_PAN].fSetSupported             = VideoProcAmpProperties[ENUM_PAN-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
    m_SensorProps[ENUM_PAN].fGetSupported             = VideoProcAmpProperties[ENUM_PAN-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
    m_SensorProps[ENUM_PAN].pCsPropValues             = &PanValues;

    m_SensorProps[ENUM_TILT].ulCurrentValue           = TiltDefault;
    m_SensorProps[ENUM_TILT].ulDefaultValue           = TiltDefault;
    m_SensorProps[ENUM_TILT].pRangeNStep              = &TiltRangeAndStep[0];
    m_SensorProps[ENUM_TILT].ulFlags                  = CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_TILT].ulCapabilities           = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL|CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_TILT].fSetSupported            = VideoProcAmpProperties[ENUM_TILT-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
    m_SensorProps[ENUM_TILT].fGetSupported            = VideoProcAmpProperties[ENUM_TILT-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
    m_SensorProps[ENUM_TILT].pCsPropValues            = &TiltValues;

    m_SensorProps[ENUM_ROLL].ulCurrentValue           = RollDefault;
    m_SensorProps[ENUM_ROLL].ulDefaultValue           = RollDefault;
    m_SensorProps[ENUM_ROLL].pRangeNStep              = &RollRangeAndStep[0];
    m_SensorProps[ENUM_ROLL].ulFlags                  = CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_ROLL].ulCapabilities           = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL|CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_ROLL].fSetSupported            = VideoProcAmpProperties[ENUM_ROLL-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
    m_SensorProps[ENUM_ROLL].fGetSupported            = VideoProcAmpProperties[ENUM_ROLL-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
    m_SensorProps[ENUM_ROLL].pCsPropValues            = &RollValues;

    m_SensorProps[ENUM_ZOOM].ulCurrentValue           = ZoomDefault;
    m_SensorProps[ENUM_ZOOM].ulDefaultValue           = ZoomDefault;
    m_SensorProps[ENUM_ZOOM].pRangeNStep              = &ZoomRangeAndStep[0];
    m_SensorProps[ENUM_ZOOM].ulFlags                  = CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_ZOOM].ulCapabilities           = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL|CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_ZOOM].fSetSupported            = VideoProcAmpProperties[ENUM_ZOOM-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
    m_SensorProps[ENUM_ZOOM].fGetSupported            = VideoProcAmpProperties[ENUM_ZOOM-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
    m_SensorProps[ENUM_ZOOM].pCsPropValues            = &ZoomValues;

    m_SensorProps[ENUM_IRIS].ulCurrentValue           = IrisDefault;
    m_SensorProps[ENUM_IRIS].ulDefaultValue           = IrisDefault;
    m_SensorProps[ENUM_IRIS].pRangeNStep              = &IrisRangeAndStep[0];
    m_SensorProps[ENUM_IRIS].ulFlags                  = CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_IRIS].ulCapabilities           = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL|CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_IRIS].fSetSupported            = VideoProcAmpProperties[ENUM_IRIS-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
    m_SensorProps[ENUM_IRIS].fGetSupported            = VideoProcAmpProperties[ENUM_IRIS-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
    m_SensorProps[ENUM_IRIS].pCsPropValues            = &IrisValues;

    m_SensorProps[ENUM_EXPOSURE].ulCurrentValue       = ExposureDefault;
    m_SensorProps[ENUM_EXPOSURE].ulDefaultValue       = ExposureDefault;
    m_SensorProps[ENUM_EXPOSURE].pRangeNStep          = &ExposureRangeAndStep[0];
    m_SensorProps[ENUM_EXPOSURE].ulFlags              = CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_EXPOSURE].ulCapabilities       = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL|CSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    m_SensorProps[ENUM_EXPOSURE].fSetSupported        = VideoProcAmpProperties[ENUM_EXPOSURE-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
    m_SensorProps[ENUM_EXPOSURE].fGetSupported        = VideoProcAmpProperties[ENUM_EXPOSURE-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
    m_SensorProps[ENUM_EXPOSURE].pCsPropValues        = &ExposureValues;

    m_SensorProps[ENUM_FOCUS].ulCurrentValue          = FocusDefault;
    m_SensorProps[ENUM_FOCUS].ulDefaultValue          = FocusDefault;
    m_SensorProps[ENUM_FOCUS].pRangeNStep             = &FocusRangeAndStep[0];
    m_SensorProps[ENUM_FOCUS].ulFlags                 = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
    m_SensorProps[ENUM_FOCUS].ulCapabilities          = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
    m_SensorProps[ENUM_FOCUS].fSetSupported           = VideoProcAmpProperties[ENUM_FOCUS-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
    m_SensorProps[ENUM_FOCUS].fGetSupported           = VideoProcAmpProperties[ENUM_FOCUS-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
    m_SensorProps[ENUM_FOCUS].pCsPropValues           = &FocusValues;

    m_SensorProps[ENUM_FLASH].ulCurrentValue          = FlashDefault;
    m_SensorProps[ENUM_FLASH].ulDefaultValue          = FlashDefault;
    m_SensorProps[ENUM_FLASH].pRangeNStep             = &FlashRangeAndStep[0];
    m_SensorProps[ENUM_FLASH].ulFlags                 = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
    m_SensorProps[ENUM_FLASH].ulCapabilities          = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
    m_SensorProps[ENUM_FLASH].fSetSupported           = VideoProcAmpProperties[ENUM_FLASH-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
    m_SensorProps[ENUM_FLASH].fGetSupported           = VideoProcAmpProperties[ENUM_FLASH-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
    m_SensorProps[ENUM_FLASH].pCsPropValues           = &FlashValues;

	//lenovo extend ctls
	if(m_bLtkMode)
	{
	    m_SensorProps[ENUM_EFFECT].ulCurrentValue          		= EffectDefault;
	    m_SensorProps[ENUM_EFFECT].ulDefaultValue          		= EffectDefault;
	    m_SensorProps[ENUM_EFFECT].pRangeNStep             		= &EffectRangeAndStep[0];
	    m_SensorProps[ENUM_EFFECT].ulFlags                 		= CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_EFFECT].ulCapabilities          		= CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_EFFECT].fSetSupported           		= VideoProcAmpProperties[ENUM_EFFECT-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
	    m_SensorProps[ENUM_EFFECT].fGetSupported           		= VideoProcAmpProperties[ENUM_EFFECT-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
	    m_SensorProps[ENUM_EFFECT].pCsPropValues           		= &EffectValues;

	    m_SensorProps[ENUM_STILL_QUALITY].ulCurrentValue        = StillQualityDefault;
	    m_SensorProps[ENUM_STILL_QUALITY].ulDefaultValue        = StillQualityDefault;
	    m_SensorProps[ENUM_STILL_QUALITY].pRangeNStep           = &StillQualityRangeAndStep[0];
	    m_SensorProps[ENUM_STILL_QUALITY].ulFlags               = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_STILL_QUALITY].ulCapabilities        = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_STILL_QUALITY].fSetSupported         = VideoProcAmpProperties[ENUM_STILL_QUALITY-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
	    m_SensorProps[ENUM_STILL_QUALITY].fGetSupported         = VideoProcAmpProperties[ENUM_STILL_QUALITY-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
	    m_SensorProps[ENUM_STILL_QUALITY].pCsPropValues         = &StillQualityValues;

	    m_SensorProps[ENUM_GETDATADIRECTCONTROL].ulCurrentValue = GetDataDirectControlDefault;
	    m_SensorProps[ENUM_GETDATADIRECTCONTROL].ulDefaultValue = GetDataDirectControlDefault;
	    m_SensorProps[ENUM_GETDATADIRECTCONTROL].pRangeNStep    = &GetDataDirectControlRangeAndStep[0];
	    m_SensorProps[ENUM_GETDATADIRECTCONTROL].ulFlags        = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_GETDATADIRECTCONTROL].ulCapabilities = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_GETDATADIRECTCONTROL].fSetSupported  = VideoProcAmpProperties[ENUM_GETDATADIRECTCONTROL-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
	    m_SensorProps[ENUM_GETDATADIRECTCONTROL].fGetSupported  = VideoProcAmpProperties[ENUM_GETDATADIRECTCONTROL-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
	    m_SensorProps[ENUM_GETDATADIRECTCONTROL].pCsPropValues  = &GetDataDirectControlValues;

	    m_SensorProps[ENUM_STILLFRAMEREQUEST].ulCurrentValue    = StillFrameRequestDefault;
	    m_SensorProps[ENUM_STILLFRAMEREQUEST].ulDefaultValue    = StillFrameRequestDefault;
	    m_SensorProps[ENUM_STILLFRAMEREQUEST].pRangeNStep       = &StillFrameRequestRangeAndStep[0];
	    m_SensorProps[ENUM_STILLFRAMEREQUEST].ulFlags           = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_STILLFRAMEREQUEST].ulCapabilities    = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_STILLFRAMEREQUEST].fSetSupported     = VideoProcAmpProperties[ENUM_STILLFRAMEREQUEST-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
	    m_SensorProps[ENUM_STILLFRAMEREQUEST].fGetSupported     = VideoProcAmpProperties[ENUM_STILLFRAMEREQUEST-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
	    m_SensorProps[ENUM_STILLFRAMEREQUEST].pCsPropValues     = &StillFrameRequestValues;

	    m_SensorProps[ENUM_PREVIEWFRAMEREQUEST].ulCurrentValue  = PreviewFrameRequestDefault;
	    m_SensorProps[ENUM_PREVIEWFRAMEREQUEST].ulDefaultValue  = PreviewFrameRequestDefault;
	    m_SensorProps[ENUM_PREVIEWFRAMEREQUEST].pRangeNStep     = &PreviewFrameRequestRangeAndStep[0];
	    m_SensorProps[ENUM_PREVIEWFRAMEREQUEST].ulFlags         = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_PREVIEWFRAMEREQUEST].ulCapabilities  = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_PREVIEWFRAMEREQUEST].fSetSupported   = VideoProcAmpProperties[ENUM_PREVIEWFRAMEREQUEST-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
	    m_SensorProps[ENUM_PREVIEWFRAMEREQUEST].fGetSupported   = VideoProcAmpProperties[ENUM_PREVIEWFRAMEREQUEST-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
	    m_SensorProps[ENUM_PREVIEWFRAMEREQUEST].pCsPropValues   = &PreviewFrameRequestValues;

	    m_SensorProps[ENUM_HW_FEATRUE].ulCurrentValue          	= HWFeatureDefault;
	    m_SensorProps[ENUM_HW_FEATRUE].ulDefaultValue          	= HWFeatureDefault;
	    m_SensorProps[ENUM_HW_FEATRUE].pRangeNStep            	= &HWFeatureRangeAndStep[0];
	    m_SensorProps[ENUM_HW_FEATRUE].ulFlags                 	= CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_HW_FEATRUE].ulCapabilities          	= CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_HW_FEATRUE].fSetSupported           	= VideoProcAmpProperties[ENUM_HW_FEATRUE-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
	    m_SensorProps[ENUM_HW_FEATRUE].fGetSupported           	= VideoProcAmpProperties[ENUM_HW_FEATRUE-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
	    m_SensorProps[ENUM_HW_FEATRUE].pCsPropValues           	= &HWFeatureValues;

	    m_SensorProps[ENUM_WORKMODE].ulCurrentValue          	= WorkModeDefault;
	    m_SensorProps[ENUM_WORKMODE].ulDefaultValue          	= WorkModeDefault;
	    m_SensorProps[ENUM_WORKMODE].pRangeNStep             	= &WorkModeRangeAndStep[0];
	    m_SensorProps[ENUM_WORKMODE].ulFlags                 	= CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_WORKMODE].ulCapabilities          	= CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
	    m_SensorProps[ENUM_WORKMODE].fSetSupported           	= VideoProcAmpProperties[ENUM_WORKMODE-NUM_VIDEOPROCAMP_ITEMS].SetSupported;
	    m_SensorProps[ENUM_WORKMODE].fGetSupported           	= VideoProcAmpProperties[ENUM_WORKMODE-NUM_VIDEOPROCAMP_ITEMS].GetSupported;
	    m_SensorProps[ENUM_WORKMODE].pCsPropValues           	= &WorkModeValues;
	}
	
    return ERROR_SUCCESS;
}


DWORD CCameraPdd::PDDVideoFormatsInit(DWORD dwDeviceIndex)
{
	RETAILMSG(dbg,(TEXT("[CAM]PDDVideoFormatsInit dwDeviceIndex=%d\r\n"),dwDeviceIndex));

	if(dwDeviceIndex==1)
	{
	    m_pModeVideoFormat[CAPTURE].ulAvailFormats       = OV_CAPTURE_FORMATS_COUNT;
	    m_pModeVideoFormat[STILL].ulAvailFormats         = OV_STILLSHOT_FORMATS_COUNT;
	    if( 3 == m_ulCTypes )
	    {
	        m_pModeVideoFormat[PREVIEW].ulAvailFormats   = OV_PREVIEW_FORMATS_COUNT;
	    }
	}else
	{
	    m_pModeVideoFormat[CAPTURE].ulAvailFormats       = NOON_CAPTURE_FORMATS_COUNT;
	    m_pModeVideoFormat[STILL].ulAvailFormats         = NOON_STILLSHOT_FORMATS_COUNT;
	    if( 3 == m_ulCTypes )
	    {
	        m_pModeVideoFormat[PREVIEW].ulAvailFormats   = NOON_PREVIEW_FORMATS_COUNT;
	    }
	}

   // m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo[0] = &DCAM_StreamMode_5;
   // m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo[1] = &DCAM_StreamMode_6;
   // m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo[2] = &DCAM_StreamMode_7;
   // m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo[3] = &DCAM_StreamMode_8;
    m_pModeVideoFormat[CAPTURE].pCsDataRangeVideo[0] = &DCAM_StreamMode_9;//mf &DCAM_StreamMode_9;	

#if 1
	//  m_pModeVideoFormat[STILL].pCsDataRangeVideo[0]   = &DCAM_StreamMode_10;
	//  m_pModeVideoFormat[STILL].pCsDataRangeVideo[1]   = &DCAM_StreamMode_11;
	//  m_pModeVideoFormat[STILL].pCsDataRangeVideo[2]   = &DCAM_StreamMode_12;
	//m_pModeVideoFormat[STILL].pCsDataRangeVideo[0]   = &DCAM_StreamMode_13;
	/*m_pModeVideoFormat[STILL].pCsDataRangeVideo[4]   = &DCAM_StreamMode_14;
	m_pModeVideoFormat[STILL].pCsDataRangeVideo[5]   = &DCAM_StreamMode_15;*/	//	yuy2
	m_pModeVideoFormat[STILL].pCsDataRangeVideo[0]   = &DCAM_StreamMode_23;	//	
#else
    m_pModeVideoFormat[STILL].pCsDataRangeVideo[0]   = &DCAM_StreamMode_17;
    m_pModeVideoFormat[STILL].pCsDataRangeVideo[1]   = &DCAM_StreamMode_18;
    m_pModeVideoFormat[STILL].pCsDataRangeVideo[2]   = &DCAM_StreamMode_19;
    m_pModeVideoFormat[STILL].pCsDataRangeVideo[3]   = &DCAM_StreamMode_20;
    m_pModeVideoFormat[STILL].pCsDataRangeVideo[4]   = &DCAM_StreamMode_21;
    m_pModeVideoFormat[STILL].pCsDataRangeVideo[5]   = &DCAM_StreamMode_22;	//	jpg
    m_pModeVideoFormat[STILL].pCsDataRangeVideo[6]   = &DCAM_StreamMode_23;	//	
#endif

    if( 3 == m_ulCTypes )
    {
        //m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo[0] = &DCAM_StreamMode_5;
       // m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo[1] = &DCAM_StreamMode_6;
       // m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo[2] = &DCAM_StreamMode_7;
       // m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo[3] = &DCAM_StreamMode_8;
        m_pModeVideoFormat[PREVIEW].pCsDataRangeVideo[0] = &DCAM_StreamMode_9;//&DCAM_StreamMode_9;
    }
	
    return ERROR_SUCCESS;
}


DWORD CCameraPdd::PDDClosed()
{
	RETAILMSG(dbg,(TEXT("[CAM]PDDClosed\r\n")));
	m_bDShowPreviewDataEnable = TRUE;
	m_bGetStillDataDirectEnable = FALSE;
	m_bContinousStillEnable = FALSE;

    return ERROR_SUCCESS;
}


DWORD CCameraPdd::GetAdapterInfo( PADAPTERINFO pAdapterInfo )
{
	//RETAILMSG(MSG_INOUT,(TEXT("++++++++++++++++++++GetAdapterInfo\r\n")));
    pAdapterInfo->ulCTypes = m_ulCTypes;
    pAdapterInfo->PowerCaps = PowerCaps;
    pAdapterInfo->ulVersionID = DRIVER_VERSION_2; //Camera MDD and DShow support DRIVER_VERSION and DRIVER_VERSION_2. Defined in camera.h
    memcpy( &pAdapterInfo->SensorProps, &m_SensorProps, sizeof(m_SensorProps));

    return ERROR_SUCCESS;
}


DWORD CCameraPdd::HandleVidProcAmpChanges( DWORD dwPropId, LONG lFlags, LONG lValue )
{
	RETAILMSG(dbg,(TEXT("[CAM] HandleVidProcAmpChanges\r\n")));
    int	value;
    PSENSOR_PROPERTY pDevProp = NULL;

    pDevProp = m_SensorProps + dwPropId;
	
	if (m_PowerState != D0) 
	{
		RETAILMSG(dbg,(TEXT("[CAM] HandleVidProcAmpChanges camera power is not on!!  %d\r\n"),dwPropId));
		return ERROR_SUCCESS;
	}
    
    if( CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL == lFlags )
    {
        pDevProp->ulCurrentValue = lValue;
	    switch(dwPropId)
	    {
	    case ENUM_BRIGHTNESS:
	    	RETAILMSG(dbg,(TEXT("[CAM] Set brightness value = %d!!!\r\n"), pDevProp->ulCurrentValue));
			value = (int)(pDevProp->ulCurrentValue);
	    	if(!CameraBrightness(value))
	    	{
	    		RETAILMSG(dbg,(TEXT("[CAM_ERROR] brightness value %d error\r\n"),value));
	    	}
	    	break;
		case ENUM_CONTRAST:
			RETAILMSG(dbg, (TEXT("[CAM] Set contrast value = %d \r\n"), pDevProp->ulCurrentValue));
			value = (int)(pDevProp->ulCurrentValue);
			if (!CameraContrast(value))
			{
				RETAILMSG(dbg,(TEXT("[CAM_ERROR] contrast value %d error\r\n"),value));
			}
	    	break;
		case ENUM_WHITEBALANCE:
			RETAILMSG(dbg, (TEXT("[CAM] Set whitebalance value = %d \r\n"), pDevProp->ulCurrentValue));
			value = (int)(pDevProp->ulCurrentValue); 
			if (!CameraWhitebalance(value))
			{
				RETAILMSG(dbg,(TEXT("[CAM_ERROR] whitebalance value %d error\r\n"),value));
			}
	    	break;
		case ENUM_SATURATION:
			RETAILMSG(dbg, (TEXT("[CAM] Set saturation value = %d \r\n"), pDevProp->ulCurrentValue));
			value = (int)(pDevProp->ulCurrentValue);
			if (!CameraSaturation(value))
			{
				RETAILMSG(dbg,(TEXT("[CAM_ERROR] saturation value %d error\r\n"),value));
			}
	    	break;
	    default:
	    	break;
	    }        
    }

    pDevProp->ulFlags = lFlags;
    return ERROR_SUCCESS;
}

DWORD CCameraPdd::HandleCamControlChanges( DWORD dwPropId, LONG lFlags, LONG lValue )
{
	RETAILMSG(dbg,(TEXT("[CAM] HandleCamControlChanges\r\n")));
    PSENSOR_PROPERTY pDevProp = NULL;
    int	value;
	DWORD dwID;
	LONG TemVal;
	BYTE* pMappedByte = NULL;
    pDevProp = m_SensorProps + dwPropId;

	if (m_PowerState != D0) 
	{
		RETAILMSG(dbg,(TEXT("[CAM] HandleCamControlChanges camera power is not on!! %d\r\n"),dwPropId));
		return ERROR_SUCCESS;
	}
    
    if( CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL == lFlags )
    {
        pDevProp->ulCurrentValue = lValue;
	    switch(dwPropId)
	    {
	    case ENUM_ZOOM:
	    	RETAILMSG(dbg,(TEXT("[CAM] Set Zoom value = %d\r\n"), pDevProp->ulCurrentValue));
	    	value = (int)((pDevProp->ulCurrentValue-ZoomRangeAndStep[0].Bounds.UnsignedMinimum)/ZoomRangeAndStep[0].SteppingDelta);
	    	RETAILMSG(dbg,(TEXT("[CAM] Zoom value=%d\r\n"),value));
	    	if(!CameraZoom(value))
	    	{
	    		RETAILMSG(dbg,(TEXT("[CAM_ERROR] Zoom value %d error\r\n"),value));
	    	}
	    	break;
		case ENUM_EXPOSURE:
			RETAILMSG(dbg, (TEXT("[CAM] Set Exposure value = %d \r\n"), pDevProp->ulCurrentValue));
			value = (int)(pDevProp->ulCurrentValue); //Exposure value calculation needed!!!
			RETAILMSG(dbg, (TEXT("[CAM] Exposure value = %d \r\n"), pDevProp->ulCurrentValue));
			if (!CameraExposure(value))
			{
				RETAILMSG(dbg,(TEXT("[CAM_ERROR] Exposure value %d error\r\n"),value));
			}
	    	break;
		case ENUM_FOCUS:
			RETAILMSG(dbg, (TEXT("[CAM] Set Focus value = %d \r\n"), pDevProp->ulCurrentValue));
			value = (int)(pDevProp->ulCurrentValue); //Focus value calculation needed!!!
			RETAILMSG(dbg, (TEXT("[CAM] Set Focus value = %d \r\n"), pDevProp->ulCurrentValue));
			if (!CameraFocus(value))
			{
				RETAILMSG(MSG_ERROR,(TEXT("[CAM_ERROR] Focus value %d error\r\n"),value));
			}
	    	break;
		case ENUM_FLASH:
			RETAILMSG(dbg, (TEXT("[CAM] Set Flash value = %d \r\n"), pDevProp->ulCurrentValue));
			value = (int)(pDevProp->ulCurrentValue); //Flash value calculation needed!!!
			RETAILMSG(dbg, (TEXT("[CAM] Set Flash value = %d \r\n"), pDevProp->ulCurrentValue));
			if (!CameraFlash(value))
			{
				RETAILMSG(dbg,(TEXT("[CAM_ERROR] Flash value %d error\r\n"),value));
			}
	    	break;
		//lenovo extend------------
		case ENUM_EFFECT:
			RETAILMSG(dbg, (TEXT("[CAM] Set Effect value = %d \r\n"), pDevProp->ulCurrentValue));
			value = (int)(pDevProp->ulCurrentValue);
	    	dwID = (value / 0x100) + 1;
	    	switch( dwID )
			{
	    		case CAM_EFFECT_MISC_ID:
	    			break;
	    		case CAM_EFFECT_SCENE_ID:
					TemVal = value-0x100;
					//ZhanggbMSG(L"CAM_EFFECT_SCENE_ID %d\r\n",TemVal);
					RETAILMSG(dbg, (L"[CAM] CAM_EFFECT_SCENE_ID=%d)\r\n", TemVal));
					if(FALSE==CameraScene(TemVal))
						RETAILMSG(dbg, (L"[CAM] CameraScene error = %d)\r\n", TemVal));
	    			break;
	    		case CAM_EFFECT_WB_ID:
					TemVal = value-0x200;
					//ZhanggbMSG(L"CAM_EFFECT_WB_ID %d\r\n",TemVal);
					RETAILMSG(dbg, (L"[CAM] SetCameraControl CAM_EFFECT_WB_ID=%d)\r\n", TemVal));
	    			if(FALSE==CameraWhitebalance(TemVal))
						RETAILMSG(dbg, (L"[CAM] CameraWhitebalance error = %d)\r\n", TemVal));
	    			break;
	    		case CAM_EFFECT_COLOR_ID:
	    			break;
	    		case CAM_EFFECT_ISO_ID:
					break;
			    case CAM_EFFECT_PHOTOMETRY_ID:
					break;
			    case CAM_EFFECT_EXPOSURE_ID:
					break;
			    case CAM_EFFECT_LENS_ID:
					break;
				default:
					break;
	    	}
	    	break;
		//lenovo extend------------
		case ENUM_PREVIEWFRAMEREQUEST:
			//if(m_CamAppRelativeCtx.g_bContinousStillEnable)
			//	return TRUE;
			value = (int)(pDevProp->ulCurrentValue);
		    __try 
		    {
				m_pMappedStruct = (FrameData*)MapCallerPtr( (LPVOID)value, sizeof(FrameData) );
				//RETAILMSG(1 , (L"[CAM] P m_pMappedStruct->pDataBuf = 0x%08x  m_pMappedStruct->dwDataLen = %d\r\n", m_pMappedStruct->pDataBuf,m_pMappedStruct->dwDataLen));
				if (m_pMappedStruct) 
				{
					pMappedByte = (BYTE*)MapCallerPtr( (LPVOID)m_pMappedStruct->pDataBuf, m_pMappedStruct->dwDataLen );
		        	//BYTE* pMappedByte = (BYTE*)MapPtrToProcess((LPVOID)pMappedStruct->pDataBuf,GetCallerProcess());		//same to line above
					//RETAILMSG(1 , (L"[CAM] P pMappedByte = 0x%08x\r\n", pMappedByte));
					if (pMappedByte) 
					{
						m_FrameData.pDataBuf = pMappedByte;
						m_FrameData.dwDataLen= m_pMappedStruct->dwDataLen;
						ResetEvent( m_hPFrameDataOKEvent );
						m_bNewPFramedata = TRUE;
						if (WAIT_TIMEOUT == WaitForSingleObject( m_hPFrameDataOKEvent, 5000 ) ) 
						{
							RETAILMSG(dbg , (L"[CAM] Wait Preview frame data Timeout!!!!\r\n"));
							m_bNewPFramedata = FALSE;
						}
						else 
						{
							m_bNewPFramedata = FALSE;
							//RETAILMSG(1 , (L"Preview frame data OK!!!!\r\n"));
						}
					}
					else
					{
						RETAILMSG(dbg , (L"[CAM] P pMappedByte map failed!!\r\n"));
					}
				}else
				{
					RETAILMSG(dbg, (L"[CAM] P m_pMappedStruct map failed!!\r\n"));
				}
		    }
			__except ( EXCEPTION_EXECUTE_HANDLER )
		    {
		        RETAILMSG(dbg, ( _T("[CAM] MT9_PFR\r\n")) );
				//CamExceptString("MT9_PFR",_exception_code()); 
		    }
	    	break;
		//lenovo extend------------
		case ENUM_STILLFRAMEREQUEST:
		 	//if (!m_CamAppRelativeCtx.g_bGetStillDataDirectEnable)
		 	//	goto NOTSUPPORTED;
			value = (int)(pDevProp->ulCurrentValue);
			if(m_bNewPFramedata)
			{
				m_bNewPFramedata = FALSE;
				SetEvent( m_hPFrameDataOKEvent );
			}
		    __try 
		    {
				m_pMappedStruct = (FrameData*)MapCallerPtr( (LPVOID)value, sizeof(FrameData) );
				//RETAILMSG(1 , (L"[CAM] M m_pMappedStruct->pDataBuf = 0x%08x  m_pMappedStruct->dwDataLen = %d\r\n", m_pMappedStruct->pDataBuf,m_pMappedStruct->dwDataLen));
				if (m_pMappedStruct) 
				{
					pMappedByte = (BYTE*)MapCallerPtr( (LPVOID)m_pMappedStruct->pDataBuf, m_pMappedStruct->dwDataLen );
		        	//BYTE* pMappedByte = (BYTE*)MapPtrToProcess((LPVOID)pMappedStruct->pDataBuf,GetCallerProcess());		//same to line above
					//RETAILMSG(1 , (L"[CAM] M pMappedByte = 0x%08x\r\n", pMappedByte));
					if (pMappedByte) 
					{
						m_FrameData.pDataBuf = pMappedByte;
						m_FrameData.dwDataLen= m_pMappedStruct->dwDataLen;
						ResetEvent( m_hSFrameDataOKEvent );
						m_bNewSFramedata = TRUE;
						if (WAIT_TIMEOUT == WaitForSingleObject( m_hSFrameDataOKEvent, 30000 ) ) 
						{
							RETAILMSG(dbg, (L"[CAM] Wait Still frame data Timeout!!!!\r\n"));
							m_bNewSFramedata = FALSE;
						}
						else 
						{
							m_bNewSFramedata = FALSE;
							//RETAILMSG(1 , (L"Still frame data OK!!!!\r\n"));
						}
					}
					else
					{
						RETAILMSG(dbg, (L"[CAM] M pMappedByte map failed!!\r\n"));
					}
				}else
				{
					RETAILMSG(dbg, (L"[CAM] M m_pMappedStruct map failed!!\r\n"));
				}
		    }
			__except ( EXCEPTION_EXECUTE_HANDLER )
		    {
		        RETAILMSG(dbg, ( _T("[CAM] MT9_PFR\r\n")) );
				//CamExceptString("MT9_PFR",_exception_code()); 
		    }
	    	break;
		//lenovo extend------------
		case ENUM_GETDATADIRECTCONTROL:
			RETAILMSG(dbg, (TEXT("[CAM] Set GETDATADIRECTCONTROL value = %d \r\n"), pDevProp->ulCurrentValue));
			value = (int)(pDevProp->ulCurrentValue);
		 	switch(value) 
		 	{
		 		case CAM_DSHOW_PREVIEWFRAMEDATA_DISABLE	:
		 				m_bDShowPreviewDataEnable = FALSE;
		 				break;
		 		case CAM_DSHOW_PREVIEWFRAMEDATA_ENABLE:	
		 				m_bDShowPreviewDataEnable = TRUE;
		 				break;
	 	 		case CAM_GET_STILLFRAMEDATA_DISABLE:	
	 	 				m_bGetStillDataDirectEnable = FALSE;
		 				break;
		 		case CAM_GET_STILLFRAMEDATA_ENABLE:	
		 				m_bGetStillDataDirectEnable = FALSE;
		 				break;
		 		case CAM_STILL_CONTINUOUS_DISABLE	:
		 				m_bContinousStillEnable = FALSE;
		 				break;
		 		case CAM_STILL_CONTINUOUS_ENABLE:	
		 				m_bContinousStillEnable = TRUE;
		 				break;
		 	}
	    	break;
		default:
	    	break;
	    }        
    }

    pDevProp->ulFlags = lFlags;
    
    return ERROR_SUCCESS;
}

DWORD CCameraPdd::HandleVideoControlCapsChanges( LONG lModeType ,ULONG ulCaps )
{
	    RETAILMSG(dbg,(TEXT("[CAM] HandleVideoControlCapsChanges\r\n")));
    m_pModeVideoCaps[lModeType].CurrentVideoControlCaps = ulCaps;
    return ERROR_SUCCESS;
}

DWORD CCameraPdd :: SetPowerState( CEDEVICE_POWER_STATE PowerState )
{
    DWORD dwErr = ERROR_SUCCESS;	
    RETAILMSG(dbg,(TEXT("[CAM] SetPowerState\r\n")));
    switch (PowerState)
    {
    case D0:    
    case D1:    
    case D2:    
        RETAILMSG(dbg, (TEXT("[CAM]: D0\r\n")));

        if (m_PowerState != D0) 
            ResumeCamera();

        m_PowerState = D0;

        dwErr = ERROR_SUCCESS;
        break;

    case D3:    
    case D4:
        RETAILMSG(dbg, (TEXT("[CAM]: D4\r\n")));
		if (m_PowerState != D4) 
        	SuspendCamera();
       	m_PowerState = D4;
        dwErr = ERROR_SUCCESS;
        break;

    default:
        break;
    }
    return dwErr;
}

DWORD CCameraPdd::HandleAdapterCustomProperties( PUCHAR pInBuf, DWORD  InBufLen, PUCHAR pOutBuf, DWORD  OutBufLen, PDWORD pdwBytesTransferred )
{
	    //RETAILMSG(MSG_INOUT,(TEXT("++++++++++++++++++++HandleAdapterCustomProperties\r\n")));
    return ERROR_NOT_SUPPORTED;
}

DWORD CCameraPdd::InitSensorMode( ULONG ulModeType, LPVOID ModeContext )
{
    DWORD hr = ERROR_SUCCESS;
	RETAILMSG(dbg,(TEXT("[CAM] InitSensorMode\r\n")));	
	ASSERT( ModeContext );
	
	EnterCriticalSection( &m_csPddDevice );
	m_iPinUseCount += 1;
	if(m_iPinUseCount > MAX_SUPPORTED_PINS) 
		m_iPinUseCount = MAX_SUPPORTED_PINS;
	RETAILMSG(dbg,(TEXT("[CAM] m_iPinUseCount=%d\r\n"),m_iPinUseCount));
	if(m_iPinUseCount == 1)
	{
		//DevicePowerNotify(_T("CAM1:"), D0, POWER_NAME);
		SetPowerState(D0);
	}
	LeaveCriticalSection( &m_csPddDevice );
    m_ppModeContext[ulModeType] = ModeContext;

	//RETAILMSG(MSG_INOUT,(TEXT("--------------------InitSensorMode\r\n")));

    return hr;
}

DWORD CCameraPdd::DeInitSensorMode( ULONG ulModeType )
{
	RETAILMSG(dbg,(TEXT("[CAM] DeInitSensorMode\r\n")));	
	EnterCriticalSection( &m_csPddDevice );
	m_iPinUseCount -= 1;
	if(m_iPinUseCount < 0) 
		m_iPinUseCount = 0;	
	RETAILMSG(dbg,(TEXT("[CAM] m_iPinUseCount=%d\r\n"),m_iPinUseCount));	
	if(m_iPinUseCount == 0)	
	{
		//DevicePowerNotify(_T("CAM1:"), D4, POWER_NAME);
		SetPowerState(D4);
	}
	LeaveCriticalSection( &m_csPddDevice );
    return ERROR_SUCCESS;
}

DWORD CCameraPdd::SetSensorState( ULONG lModeType, CSSTATE csState )
{
    DWORD dwError = ERROR_SUCCESS;
	RETAILMSG(dbg,(TEXT("[CAM] SetSensorState\r\n")));		
	int format = (lModeType==CAPTURE)?VIDEO_CAPTURE_BUFFER:((lModeType==STILL)?STILL_CAPTURE_BUFFER:PREVIEW_CAPTURE_BUFFER);

    switch ( csState )
    {
        case CSSTATE_STOP:
			RETAILMSG(dbg,(TEXT("[CAM] %d STOP! format=%d\r\n"),lModeType,format));		
            m_CsState[lModeType] = CSSTATE_STOP;

            if( STILL == lModeType )
            {
                m_bStillCapInProgress = false;
            } 
            else if(CAPTURE == lModeType)
            {
            	m_bCameraVideoRunning = false;
            } 
            else
            {
            	m_bCameraPreviewRunning = false;
            }
            CameraCaptureControl(format, FALSE);
            break;

        case CSSTATE_PAUSE:
			RETAILMSG(dbg,(TEXT("[CAM] %d PAUSE! format=%d\r\n"),lModeType,format));
			if(CAPTURE == lModeType)
            {
            	m_bCameraVideoRunning = false;
            } 
            else
            {
            	m_bCameraPreviewRunning = false;
            }			
            m_CsState[lModeType] = CSSTATE_PAUSE;
            CameraCaptureControl(format, FALSE);
            break;

        case CSSTATE_RUN:
			RETAILMSG(dbg,(TEXT("[CAM] %d RUN! format=%d\r\n"),lModeType,format));	
            m_CsState[lModeType] = CSSTATE_RUN;

			if(CAPTURE == lModeType)
            {
            	m_bCameraVideoRunning = true;
            } 
            else
            {
            	m_bCameraPreviewRunning = true;
            }
			SetSensorFormat(lModeType);
			CameraSetRegisters(format);
			CameraCaptureControl(format,TRUE);
            break;

        default:
            RETAILMSG( dbg, ( _T("IOControl(%08x): Incorrect State\r\n"), this ) );
            dwError = ERROR_INVALID_PARAMETER;
    }

    return dwError;
}

DWORD CCameraPdd::TakeStillPicture( LPVOID pBurstModeInfo )
{
    DWORD dwError = ERROR_SUCCESS;
	RETAILMSG(dbg,(TEXT("++++++++++++++++++++TakeStillPicture\r\n")));			
	
// by Mickey
	SetSensorState(STILL, CSSTATE_STOP);
	SetSensorState(CAPTURE, CSSTATE_STOP);
	SetSensorState(PREVIEW, CSSTATE_PAUSE);
	SetSensorState(PREVIEW, CSSTATE_STOP);

    m_bStillCapInProgress = true;
    //Ignore pBurstModeInfo
    m_CsState[STILL] = CSSTATE_RUN;

	//mf Sleep(1000);

	SetSensorState(STILL, CSSTATE_RUN);

	//mf Sleep(1000);
	
    return dwError;
}


DWORD CCameraPdd::GetSensorModeInfo( ULONG ulModeType, PSENSORMODEINFO pSensorModeInfo )
{
	RETAILMSG(dbg,(TEXT("GetSensorModeInfo\r\n")));		
    pSensorModeInfo->MemoryModel = m_SensorModeInfo[ulModeType].MemoryModel;
    pSensorModeInfo->MaxNumOfBuffers = m_SensorModeInfo[ulModeType].MaxNumOfBuffers;
    pSensorModeInfo->PossibleCount = m_SensorModeInfo[ulModeType].PossibleCount;
    pSensorModeInfo->VideoCaps.DefaultVideoControlCaps = DefaultVideoControlCaps[ulModeType];
    pSensorModeInfo->VideoCaps.CurrentVideoControlCaps = m_pModeVideoCaps[ulModeType].CurrentVideoControlCaps;
    pSensorModeInfo->pVideoFormat = &m_pModeVideoFormat[ulModeType];
    //RETAILMSG(MSG_INOUT,(TEXT("--------------------GetSensorModeInfo\r\n")));		
    return ERROR_SUCCESS;
}

DWORD CCameraPdd::SetSensorModeFormat( ULONG ulModeType, PCS_DATARANGE_VIDEO pCsDataRangeVideo )
{
	    RETAILMSG(dbg,(TEXT("SetSensorModeFormat for pin %d\r\n"),ulModeType));		
    // MDD will take care of ulModeType. It will never be out of range. MDD will also ask for
    // the format we support. We need not to check it here. 

    memcpy( &m_pCurrentFormat[ulModeType], pCsDataRangeVideo, sizeof ( CS_DATARANGE_VIDEO ) );
    return ERROR_SUCCESS;
}

PVOID CCameraPdd::AllocateBuffer( ULONG ulModeType )
{  
	    RETAILMSG(dbg,(TEXT("AllocateBuffer\r\n")));		
    // Real PDD may want to save off this allocated pointer
    // in an array.
    //ULONG ulFrameSize = CS__DIBSIZE (m_pCurrentFormat[ulModeType].VideoInfoHeader.bmiHeader);
//    return RemoteLocalAlloc( LPTR, ulFrameSize );
	// In this PDD, we dont need it
	return NULL;
}

DWORD CCameraPdd::DeAllocateBuffer( ULONG ulModeType, PVOID pBuffer )
{
	    RETAILMSG(dbg,(TEXT("DeAllocateBuffer\r\n")));		
   // RemoteLocalFree( pBuffer );
    return ERROR_SUCCESS;
}

DWORD CCameraPdd::RegisterClientBuffer( ULONG ulModeType, PVOID pBuffer )
{
	    RETAILMSG(dbg,(TEXT("RegisterClientBuffer\r\n")));	
    // Real PDD may want to save pBuffer which is a pointer to buffer that DShow created.
    return ERROR_SUCCESS;
}

DWORD CCameraPdd::UnRegisterClientBuffer( ULONG ulModeType, PVOID pBuffer )
{
	    RETAILMSG(dbg,(TEXT("UnRegisterClientBuffer\r\n")));	
    // DShow is not going to use pBuffer (which was originally allocated by DShow) anymore. If the PDD
    // is keeping a cached pBuffer pointer (in RegisterClientBuffer()) then this is the right place to
    // stop using it and maybe set the cached pointer to NULL. 
    // Note: PDD must not delete this pointer as it will be deleted by DShow itself
    return ERROR_SUCCESS;
}

DWORD CCameraPdd::HandleSensorModeCustomProperties( ULONG ulModeType, PUCHAR pInBuf, DWORD  InBufLen, PUCHAR pOutBuf, DWORD  OutBufLen, PDWORD pdwBytesTransferred )
{
	//RETAILMSG(MSG_INOUT,(TEXT("++++++++++++++++++++HandleSensorModeCustomProperties\r\n")));	
    return ERROR_NOT_SUPPORTED;
}


extern "C" { WINGDIAPI HBITMAP WINAPI CreateBitmapFromPointer( CONST BITMAPINFO *pbmi, int iStride, PVOID pvBits); }

DWORD CCameraPdd::FillBuffer( ULONG ulModeType, PUCHAR pImage )
{
	DWORD dwRet = 0;
	INT CurrentFrame;
	PCS_VIDEOINFOHEADER pCsVideoInfoHdr = &m_pCurrentFormat[ulModeType].VideoInfoHeader;
	PUCHAR pJpgData = NULL, pStillData = NULL;

    //RETAILMSG(MSG_INOUT,(TEXT("++++++++++++++++++++FillBuffer\r\n")));
    ASSERT(pCsVideoInfoHdr->bmiHeader.biSizeImage != 0);
	if(pImage == NULL)
	{
		RETAILMSG(dbg,(TEXT("pImage is null!\r\n")));
		return 0;
	}
    // MDD will make sure that the buffer is sufficient for the image.   

	if (ulModeType == CAPTURE)
	{
		CurrentFrame = CameraGetCurrentFrameNum(VIDEO_CAPTURE_BUFFER);
		//RETAILMSG(MSG_ON,(TEXT("m_CameraHWVideoBuffers[%d].VirtAddr=0x%x\r\n"),CurrentFrame,m_CameraHWVideoBuffers[CurrentFrame].VirtAddr));
		dwRet = pCsVideoInfoHdr->bmiHeader.biSizeImage; 
		memcpy(pImage, (void *)m_CameraHWVideoBuffers[CurrentFrame].VirtAddr, dwRet);		
	}
	else if (ulModeType == STILL)
	{
		CurrentFrame = CameraGetCurrentFrameNum(STILL_CAPTURE_BUFFER);
		RETAILMSG(dbg,(TEXT("m_CameraHWStillBuffer.VirtAddr=0x%x, w=%d\r\n"),m_CameraHWStillBuffer.VirtAddr,pCsVideoInfoHdr->bmiHeader.biWidth));
		dwRet = pCsVideoInfoHdr->bmiHeader.biSizeImage; 
		if(JPEG_DIB == (pCsVideoInfoHdr->bmiHeader.biCompression & ~BI_SRCPREROTATE) || pCsVideoInfoHdr->bmiHeader.biWidth==1280)
		{
			//dwRet = JpegDataProcess((PUCHAR)m_CameraHWStillBuffer.VirtAddr,&pJpgData,pCsVideoInfoHdr->bmiHeader.biWidth,pCsVideoInfoHdr->bmiHeader.biHeight);
			//pStillData = pJpgData;
			//WriteJPGDataFile((BYTE*)pStillData, dwRet);
			//memset(pStillData,0x88,dwRet);
			RETAILMSG(1,(TEXT("m_JpgSize is %d\r\n"),m_JpgSize));
			dwRet=m_JpgSize;
			//WriteJPGDataFile((BYTE *)m_CameraHWStillBuffer.VirtAddr, m_JpgSize);
			CeSafeCopyMemory(pImage, (void *)m_CameraHWStillBuffer.VirtAddr, m_JpgSize);
		}else
		{
			pStillData = (PUCHAR)m_CameraHWStillBuffer.VirtAddr;
		}
	}
	else if(ulModeType == PREVIEW)
	{
		CurrentFrame = CameraGetCurrentFrameNum(PREVIEW_CAPTURE_BUFFER);
		dwRet = pCsVideoInfoHdr->bmiHeader.biSizeImage; 
		//RETAILMSG(MSG_ON,(TEXT("m_CameraHWPreviewBuffers[%d].VirtAddr=0x%x\r\n"),CurrentFrame,m_CameraHWPreviewBuffers[CurrentFrame].VirtAddr));
		if(m_bDShowPreviewDataEnable)
			memcpy(pImage, (void *)m_CameraHWPreviewBuffers[CurrentFrame].VirtAddr, dwRet);	
		else
		{
			if(m_bNewPFramedata)
			{
				memcpy(m_FrameData.pDataBuf, (void *)m_CameraHWPreviewBuffers[CurrentFrame].VirtAddr, dwRet);	
				SetEvent( m_hPFrameDataOKEvent );
			}
		}
	}
    // return the size of the image filled
    	return dwRet;
}


void CCameraPdd :: HandleCaptureInterrupt( ULONG ulModeTypeIn )
{
    ULONG ulModeType;
 	//RETAILMSG(MSG_INOUT,(TEXT("HandleCaptureInterrupt  %d\r\n"),ulModeType));
    if( m_bStillCapInProgress )
    {
    	RETAILMSG(dbg,(TEXT("HandleCaptureInterrupt  return case 0!!!!!!!!!!!!!!!!\r\n")));
        return;
    }
    
    if( ulModeTypeIn == CAPTURE)
    {
        ulModeType = CAPTURE;
    }
    else if ( m_ulCTypes == 3 && ulModeTypeIn == PREVIEW )
    {
        ulModeType = PREVIEW;
    }
    else
    {
    	RETAILMSG(dbg,(TEXT("HandleCaptureInterrupt  return case 1!!!!!!!!!!!!!!!!\r\n")));
        ASSERT(false);
        return;
    }

    MDD_HandleIO( m_ppModeContext[ulModeType], ulModeType );
}


void CCameraPdd :: HandleStillInterrupt( )
{
	static int skipnum = 0;
	RETAILMSG(dbg,(TEXT("HandleStillInterrupt\r\n")));
	if(m_bStillCapInProgress && skipnum++<3)
		goto SKIP_PIC;
	PCS_VIDEOINFOHEADER pCsVideoInfoHdr = &m_pCurrentFormat[STILL].VideoInfoHeader;
	PUCHAR pJpgData = NULL, pStillData = NULL;
	DWORD dwRet = JpegDataProcess((PUCHAR)m_CameraHWStillBuffer.VirtAddr,&pJpgData,pCsVideoInfoHdr->bmiHeader.biWidth,pCsVideoInfoHdr->bmiHeader.biHeight);
	if(dwRet==10000)
	{
		RETAILMSG(1,(TEXT("Bad jpeg ,ignore it\r\n")));
		goto SKIP_PIC;
	}
	m_JpgSize=dwRet;
	MDD_HandleIO( m_ppModeContext[STILL], STILL );
	m_bStillCapInProgress = false;
	skipnum = 0;

	// by Mickey
	// start_mickey
	SetSensorState(STILL, CSSTATE_STOP);

	SetSensorState(STILL, CSSTATE_PAUSE);	
	SetSensorState(CAPTURE, CSSTATE_PAUSE);	
	SetSensorState(PREVIEW, CSSTATE_PAUSE);
	
	//SetSensorState(PREVIEW, CSSTATE_RUN);	
	// end_mickey
SKIP_PIC:
	return;
}

bool CCameraPdd::ReadMemoryModelFromRegistry()
{
    HKEY  hKey = 0;
    DWORD dwType  = 0;
    DWORD dwSize  = sizeof ( DWORD );
    DWORD dwValue = -1;
	RETAILMSG(dbg,(TEXT("ReadMemoryModelFromRegistry\r\n")));

    if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"Drivers\\Capture\\Camera", 0, 0, &hKey ))
    {
        false;
    }

    if( ERROR_SUCCESS == RegQueryValueEx( hKey, L"MemoryModel", 0, &dwType, (BYTE *)&dwValue, &dwSize ) )
    {
		RETAILMSG(dbg,(TEXT("MemoryModel = %d\r\n"),dwValue));
        if(   ( REG_DWORD == dwType ) 
           && ( sizeof( DWORD ) == dwSize ) 
           && (( dwValue == CSPROPERTY_BUFFER_DRIVER ) || ( dwValue == CSPROPERTY_BUFFER_CLIENT_LIMITED ) || ( dwValue == CSPROPERTY_BUFFER_CLIENT_UNLIMITED )))
        {
            for( int i=0; i<MAX_SUPPORTED_PINS ; i++ )
            {
                m_SensorModeInfo[i].MemoryModel = (CSPROPERTY_BUFFER_MODE) dwValue;
            }
        }
    }

    // Find out if we should be using some other number of supported modes. The only
    // valid options are 2 or 3. Default to 2.
    if ( ERROR_SUCCESS == RegQueryValueEx( hKey, L"PinCount", 0, &dwType, (BYTE *)&dwValue, &dwSize ) )
    {
		RETAILMSG(dbg,(TEXT("PinCount = %d\r\n"),dwValue));
        if ( REG_DWORD == dwType
             && sizeof ( DWORD ) == dwSize
             && 3 == dwValue )
        {
            m_ulCTypes = 3;
        }
    }

    RegCloseKey( hKey );
    return true;
}

bool CCameraPdd::SetSensorFormat( ULONG ulModeType)
{
	INT format;
	PCS_VIDEOINFOHEADER pCsVideoInfoHdr = &m_pCurrentFormat[ulModeType].VideoInfoHeader;

	UINT biWidth        = pCsVideoInfoHdr->bmiHeader.biWidth;
	UINT biHeight       = abs(pCsVideoInfoHdr->bmiHeader.biHeight);
	DWORD biBitCount = pCsVideoInfoHdr->bmiHeader.biBitCount;
	DWORD biCompression = pCsVideoInfoHdr->bmiHeader.biCompression;
	RETAILMSG(dbg,(TEXT("SetSensorFormat biWidth=%d biHeight=%d\r\n"),biWidth,biHeight));	

	CameraModuleSetImageSize(biWidth,biHeight);
    if (ulModeType == STILL)
		CameraCaptureMode(); 
	else
		CameraPreviewRecovery();
	CameraCaptureSourceSet();

    // Prepare buffers here for Preview and Still mode.
    // Set Register for output

	if ( (FOURCC_YUY2 == (biCompression & ~BI_SRCPREROTATE)))
	{
		if(biWidth == 1280)
			format = OUTPUT_CODEC_JPEG;
		else
			format = OUTPUT_CODEC_YCBCR422;
		RETAILMSG(dbg, ( _T("InitSensorMode: format = OUTPUT_CODEC_YCBCR422\r\n") ) );
	}
	else if((FOURCC_YV12 == (biCompression & ~BI_SRCPREROTATE)))
	{
		format = OUTPUT_CODEC_YCBCR420;
		RETAILMSG(dbg, ( _T("InitSensorMode: format = OUTPUT_CODEC_YCBCR420\r\n") ) );
	}
	else if((JPEG_DIB == (biCompression & ~BI_SRCPREROTATE)))
	{
		format = OUTPUT_CODEC_JPEG;
		RETAILMSG(dbg, ( _T("InitSensorMode: format = OUTPUT_CODEC_JPEG\r\n") ) );
	}
	else
	{
		if(biBitCount == 24)
		{
			format = OUTPUT_CODEC_RGB24;
			RETAILMSG(dbg, ( _T("InitSensorMode: format = OUTPUT_CODEC_RGB24\r\n") ) );
		}
		else
		{
			format = OUTPUT_CODEC_RGB16;
			RETAILMSG(dbg, ( _T("InitSensorMode: format = OUTPUT_CODEC_RGB16\r\n") ) );
		}
	}

	
    if (ulModeType == CAPTURE)
    {
		CameraSetFormat(biWidth, biHeight, format, VIDEO_CAPTURE_BUFFER);
        if (!CameraPrepareBuffer(m_CameraHWVideoBuffers, VIDEO_CAPTURE_BUFFER) )
        {
            RETAILMSG(dbg, ( _T("InitSensorMode: CAPTURE CameraPrepareBuffer() failed\r\n") ) );
			return FALSE;
        }

    }
    else if (ulModeType == STILL)
    {
		CameraSetFormat(biWidth, biHeight, format, STILL_CAPTURE_BUFFER);
        if (!CameraPrepareBuffer(&m_CameraHWStillBuffer, STILL_CAPTURE_BUFFER))
        {
            RETAILMSG(dbg, ( _T("InitSensorMode: STILL CameraPrepareBuffer() failed\r\n") ) );
			return FALSE;
        }
    }
	else if(ulModeType == PREVIEW)
	{
		CameraSetFormat(biWidth, biHeight, format, PREVIEW_CAPTURE_BUFFER);		
		if (!CameraPrepareBuffer(m_CameraHWPreviewBuffers, PREVIEW_CAPTURE_BUFFER))
		{
		    RETAILMSG(dbg, ( _T("InitSensorMode: PREVIEW CameraPrepareBuffer() failed\r\n") ) );
			return FALSE;
		}    
    }	
    else
    {
		return FALSE;
    }

	//RETAILMSG(MSG_INOUT,(TEXT("--------------------SetSensorFormat\r\n")));
	return TRUE;
}

void CCameraPdd::SuspendCamera( )
{
	RETAILMSG(dbg,(TEXT("[CAM] SuspendCamera\r\n")));
    if(m_bCameraVideoRunning)
    {
        m_bCameraVideoWasRunning = TRUE;
	    if (m_CsState[CAPTURE]== CSSTATE_RUN)
	    {
	        SetSensorState(CAPTURE, CSSTATE_PAUSE);
	    }        
    }
    
    if(m_bCameraPreviewRunning)
    {
    	m_bCameraPreviewWasRunning = TRUE;
	    if (m_CsState[PREVIEW] == CSSTATE_RUN)
	    {
	        SetSensorState(PREVIEW, CSSTATE_PAUSE);
	    }       	
    }
	
	//if(m_iPinUseCount > 0)
	{
		CameraSleep(m_dwCurrentDeviceIndex);		
	}
}

void CCameraPdd::ResumeCamera( )
{
	RETAILMSG(dbg,(TEXT("[CAM] ResumeCamera\r\n")));
    // Restart camera sensor if it was running before
	
	//if(m_iPinUseCount > 0)
	{
		CameraResume(m_dwCurrentDeviceIndex);		
	}  	
	
	if(m_bCameraVideoWasRunning)
	{
		m_bCameraVideoWasRunning = FALSE;
		SetSensorState(CAPTURE, CSSTATE_RUN);		
	}
	
    if ( m_bCameraPreviewWasRunning )
    {
		m_bCameraPreviewWasRunning = FALSE;
		SetSensorState(PREVIEW, CSSTATE_RUN);
    }	
}

void CCameraPdd::CameraVideoFrameCallback( DWORD dwContext )
{
    CCameraPdd * pCamDevice = reinterpret_cast<CCameraPdd *>( dwContext );
    
    // Video frame is ready - put it into stream
    
    if (NULL != pCamDevice)
    {
        pCamDevice->HandleCaptureInterrupt(CAPTURE);
    }
}

void CCameraPdd::CameraStillFrameCallback( DWORD dwContext )
{
    CCameraPdd * pCamDevice = reinterpret_cast<CCameraPdd *>( dwContext );
    
    // Still image frame is ready - put it into stream

    if (NULL != pCamDevice)
    {
        pCamDevice->HandleStillInterrupt();
    }
}

void CCameraPdd::CameraPreviewFrameCallback( DWORD dwContext )
{
    CCameraPdd * pCamDevice = reinterpret_cast<CCameraPdd *>( dwContext );
    
    // Video frame is ready - put it into stream
    
    if (NULL != pCamDevice)
    {
        pCamDevice->HandleCaptureInterrupt(PREVIEW);
    }
}
