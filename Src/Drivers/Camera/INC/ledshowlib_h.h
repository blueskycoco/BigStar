/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Apr 09 19:18:37 2007
 */
/* Compiler settings for .\LeDShowLib.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __LeDShowLib_h_h__
#define __LeDShowLib_h_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ILeCamera_FWD_DEFINED__
#define __ILeCamera_FWD_DEFINED__
typedef interface ILeCamera ILeCamera;
#endif 	/* __ILeCamera_FWD_DEFINED__ */


#ifndef __ILeCamControl_FWD_DEFINED__
#define __ILeCamControl_FWD_DEFINED__
typedef interface ILeCamControl ILeCamControl;
#endif 	/* __ILeCamControl_FWD_DEFINED__ */


#ifndef __ILeCamLight_FWD_DEFINED__
#define __ILeCamLight_FWD_DEFINED__
typedef interface ILeCamLight ILeCamLight;
#endif 	/* __ILeCamLight_FWD_DEFINED__ */


#ifndef __ILeCamPreview_FWD_DEFINED__
#define __ILeCamPreview_FWD_DEFINED__
typedef interface ILeCamPreview ILeCamPreview;
#endif 	/* __ILeCamPreview_FWD_DEFINED__ */


#ifndef __ILeCamWindow_FWD_DEFINED__
#define __ILeCamWindow_FWD_DEFINED__
typedef interface ILeCamWindow ILeCamWindow;
#endif 	/* __ILeCamWindow_FWD_DEFINED__ */


#ifndef __ILeCamStill_FWD_DEFINED__
#define __ILeCamStill_FWD_DEFINED__
typedef interface ILeCamStill ILeCamStill;
#endif 	/* __ILeCamStill_FWD_DEFINED__ */


#ifndef __ILeCamCapture_FWD_DEFINED__
#define __ILeCamCapture_FWD_DEFINED__
typedef interface ILeCamCapture ILeCamCapture;
#endif 	/* __ILeCamCapture_FWD_DEFINED__ */


#ifndef __ILeCamResoluion_FWD_DEFINED__
#define __ILeCamResoluion_FWD_DEFINED__
typedef interface ILeCamResoluion ILeCamResoluion;
#endif 	/* __ILeCamResoluion_FWD_DEFINED__ */


#ifndef __ILeImageProcess_FWD_DEFINED__
#define __ILeImageProcess_FWD_DEFINED__
typedef interface ILeImageProcess ILeImageProcess;
#endif 	/* __ILeImageProcess_FWD_DEFINED__ */


#ifndef __ILeOverlay_FWD_DEFINED__
#define __ILeOverlay_FWD_DEFINED__
typedef interface ILeOverlay ILeOverlay;
#endif 	/* __ILeOverlay_FWD_DEFINED__ */


#ifndef __IOverlayGraph_FWD_DEFINED__
#define __IOverlayGraph_FWD_DEFINED__
typedef interface IOverlayGraph IOverlayGraph;
#endif 	/* __IOverlayGraph_FWD_DEFINED__ */


#ifndef __ILeCamEvent_FWD_DEFINED__
#define __ILeCamEvent_FWD_DEFINED__
typedef interface ILeCamEvent ILeCamEvent;
#endif 	/* __ILeCamEvent_FWD_DEFINED__ */


#ifndef __ILeDShowPlayer_FWD_DEFINED__
#define __ILeDShowPlayer_FWD_DEFINED__
typedef interface ILeDShowPlayer ILeDShowPlayer;
#endif 	/* __ILeDShowPlayer_FWD_DEFINED__ */


#ifndef __ILeDSLLog_FWD_DEFINED__
#define __ILeDSLLog_FWD_DEFINED__
typedef interface ILeDSLLog ILeDSLLog;
#endif 	/* __ILeDSLLog_FWD_DEFINED__ */


#ifndef __LeCamera_FWD_DEFINED__
#define __LeCamera_FWD_DEFINED__

#ifdef __cplusplus
typedef class LeCamera LeCamera;
#else
typedef struct LeCamera LeCamera;
#endif /* __cplusplus */

#endif 	/* __LeCamera_FWD_DEFINED__ */


#ifndef __LeOverlay_FWD_DEFINED__
#define __LeOverlay_FWD_DEFINED__

#ifdef __cplusplus
typedef class LeOverlay LeOverlay;
#else
typedef struct LeOverlay LeOverlay;
#endif /* __cplusplus */

#endif 	/* __LeOverlay_FWD_DEFINED__ */


#ifndef __LeImageProcess_FWD_DEFINED__
#define __LeImageProcess_FWD_DEFINED__

#ifdef __cplusplus
typedef class LeImageProcess LeImageProcess;
#else
typedef struct LeImageProcess LeImageProcess;
#endif /* __cplusplus */

#endif 	/* __LeImageProcess_FWD_DEFINED__ */


#ifndef __LeDShowPlayer_FWD_DEFINED__
#define __LeDShowPlayer_FWD_DEFINED__

#ifdef __cplusplus
typedef class LeDShowPlayer LeDShowPlayer;
#else
typedef struct LeDShowPlayer LeDShowPlayer;
#endif /* __cplusplus */

#endif 	/* __LeDShowPlayer_FWD_DEFINED__ */


#ifndef __LeDSLLog_FWD_DEFINED__
#define __LeDSLLog_FWD_DEFINED__

#ifdef __cplusplus
typedef class LeDSLLog LeDSLLog;
#else
typedef struct LeDSLLog LeDSLLog;
#endif /* __cplusplus */

#endif 	/* __LeDSLLog_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_LeDShowLib_0000 */
/* [local] */ 

#pragma once
#pragma once
typedef 
enum WindowState
    {	WindowShow	= 0,
	WindowHide	= WindowShow + 1,
	WindowFullScreen	= WindowHide + 1,
	StateUnknown	= WindowFullScreen + 1
    }	WindowState;

typedef 
enum PreviewStatus
    {	PreviewDisable	= 0,
	PreviewEnable	= 1
    }	PreviewStatus;

typedef 
enum StillStatus
    {	StillDisable	= 2,
	StillEnable	= 3
    }	StillStatus;

typedef 
enum ResolutionType
    {	ResolutionPreview	= 0,
	ResolutionStill	= ResolutionPreview + 1,
	ResolutionCapture	= ResolutionStill + 1,
	ResolutionUnknown	= ResolutionCapture + 1
    }	ResolutionType;

typedef 
enum CameraProperty
    {	PropertyBrightness	= 0,
	PropertyContrast	= PropertyBrightness + 1,
	PropertyWhiteBlan	= PropertyContrast + 1,
	PropertyColorMode	= PropertyWhiteBlan + 1,
	PropertyAutoFocus	= PropertyColorMode + 1,
	PropertyDigitalZoom	= PropertyAutoFocus + 1,
	PropertyStillQuailty	= PropertyDigitalZoom + 1,
	PropertySceneMode	= PropertyStillQuailty + 1,
	PropertyPreview	= PropertySceneMode + 1,
	PropertyStill	= PropertyPreview + 1,
	PropertyVideoRotate	= PropertyStill + 1,
	ProperyWorkMode	= PropertyVideoRotate + 1
    }	CameraProperty;

typedef 
enum CamWorkMode
    {	WMForeground	= 0,
	WMBackground	= 1
    }	CamWorkMode;

typedef 
enum SceneModeValue
    {	SceneNormal	= 0x101,
	ScenePortrait	= 0x102,
	SceneLandscape	= 0x103,
	SceneMove	= 0x104,
	SceneMicro	= 0x105,
	SceneInDoor	= 0x106,
	SceneOutDoor	= 0x107,
	SceneBackLighting	= 0x108,
	SceneDay	= 0x109,
	SceneDawn	= 0x10a,
	SceneForenoon	= 0x10b,
	SceneNoon	= 0x10c,
	SceneAfternoon	= 0x10d,
	SceneSunDown	= 0x10e,
	SceneDusk	= 0x10f,
	SceneNight	= 0x110,
	SceneSnow	= 0x111,
	SceneNeedFire	= 0x112
    }	SceneModeValue;

typedef 
enum WhiteBalanceValue
    {	WbAuto	= 0x201,
	WbTungstenLamp	= 0x202,
	WbFluorescentLamp	= 0x203,
	WbFlashLamp	= 0x204,
	WbDaylight	= 0x205,
	WbFineDay	= 0x206,
	WbCloudy	= 0x207,
	WbShadow	= 0x208,
	WbNight	= 0x209,
	WbStarLight	= 0x20a
    }	WhiteBalanceValue;

typedef 
enum ColorModeValue
    {	ColorSepia	= 0x301,
	ColorBlackWhite	= 0x302,
	ColorMutiColor	= 0x303,
	ColorVivid	= 0x304,
	ColorNegative	= 0x305,
	ColorWarm	= 0x306,
	ColorCold	= 0x307,
	ColorFilm	= 0x308,
	ColorRelievo	= 0x309,
	ColorWaterPowder	= 0x30a
    }	ColorModeValue;

typedef 
enum LightStatus
    {	LightFlashOn	= 0,
	LightFlashOff	= LightFlashOn + 1,
	LightAlwaysOn	= LightFlashOff + 1,
	LightAlwaysOff	= LightAlwaysOn + 1
    }	LightStatus;

typedef 
enum StillQuality
    {	QualityLow	= 0,
	QualityHigh	= 100
    }	StillQuality;

typedef 
enum ColorSpaceSel
    {	CSRGB	= 0,
	CSYUV	= CSRGB + 1,
	CSJPEG	= CSYUV + 1
    }	ColorSpaceSel;

typedef 
enum CodecFormat
    {	CodecWMV	= 0,
	CodecMpeg4	= CodecWMV + 1,
	CodecH263	= CodecMpeg4 + 1,
	CodecMJpeg	= CodecH263 + 1
    }	CodecFormat;

typedef 
enum CodecType
    {	Video	= 0,
	Audio	= Video + 1,
	Mux	= Audio + 1
    }	CodecType;

typedef 
enum CaptureType
    {	VideoAudio	= 0,
	VideoOnly	= VideoAudio + 1
    }	CaptureType;

typedef 
enum VideoRotate
    {	DisableRotate	= 0,
	EnableRotate	= DisableRotate + 1
    }	VideoRotate;

typedef 
enum SysPlayState
    {	PlayStateUndefined	= 0,
	PlayStateStopped	= 1,
	PlayStatePaused	= 2,
	PlayStatePlaying	= 3,
	PlayStateScanForward	= 4,
	PlayStateScanReverse	= 5,
	PlayStateBuffering	= 6,
	PlayStateWaiting	= 7,
	PlayStateMediaEnded	= 8,
	PlayStateTransitioning	= 9,
	PlayStateReady	= 10,
	PlayStateReconnecting	= 11,
	PlayStateLast	= 12
    }	SysPlayState;

typedef 
enum LeCamEvent
    {	EventAll	= 0,
	EventAutoFocusStart	= EventAll + 1,
	EventAutoFocusSuccess	= EventAutoFocusStart + 1,
	EventAutoFocusFailed	= EventAutoFocusSuccess + 1,
	EventStillDataReady	= EventAutoFocusFailed + 1,
	EventStillFileCaptured	= EventStillDataReady + 1,
	EventVideoStreamStarted	= EventStillFileCaptured + 1,
	EventVideoStreamStoped	= EventVideoStreamStarted + 1,
	EventVideoSampleProcessed	= EventVideoStreamStoped + 1,
	EventVideoBufferFull	= EventVideoSampleProcessed + 1,
	EventErrorOccurred	= EventVideoBufferFull + 1,
	EventPowerSuspend	= EventErrorOccurred + 1,
	EventPowerResume	= EventPowerSuspend + 1
    }	LeCamEvent;

typedef 
enum AutoFocusStatus
    {	FocusControlDisable	= 0,
	FocusControlEnable	= 1,
	EnterAutoFocusMode	= 2,
	CmdAutoFocusStart	= 3,
	CmdAutoFocusStop	= 4,
	EnterMicroFocusMode	= 5
    }	AutoFocusStatus;

typedef 
enum OverlayType
    {	OverlayGX	= 1,
	OverlayDDRAW	= 2,
	OverlayHW	= 4,
	OverlayDS	= 8
    }	OverlayType;

typedef 
enum ResType
    {	RtUINT	= 0,
	RtHBITMAP	= RtUINT + 1,
	RtHICON	= RtHBITMAP + 1,
	RtWCHAR	= RtHICON + 1
    }	ResType;

typedef 
enum BlendAlgorithm
    {	BlendAlpha	= 10,
	BlendHalfAlpha	= BlendAlpha + 1,
	BlendChannel	= BlendHalfAlpha + 1,
	BlendAdd	= BlendChannel + 1,
	BlendSub	= BlendAdd + 1,
	BlendQuarter	= BlendSub + 1,
	BlendNone	= BlendQuarter + 1
    }	BlendAlgorithm;

typedef 
enum LineStyle
    {	LineSolid	= 0,
	LineDash	= LineSolid + 1
    }	LineStyle;

typedef 
enum RGBFormat
    {	RGB555	= 0,
	RGB565	= RGB555 + 1,
	RGB888	= RGB565 + 1,
	ALPHARGB	= RGB888 + 1
    }	RGBFormat;

typedef 
enum YCbCrFormat
    {	YCbCr422	= 0,
	YCbCr420	= YCbCr422 + 1
    }	YCbCrFormat;

typedef 
enum JPEGFormat
    {	JPEG	= 0,
	JPEG2000	= JPEG + 1
    }	JPEGFormat;

typedef 
enum Rotate
    {	RotateDisable	= 0,
	Rotate90L	= RotateDisable + 1,
	Rotate90R	= Rotate90L + 1,
	Rotate180	= Rotate90R + 1,
	Horizontal	= Rotate180 + 1,
	Vertical	= Horizontal + 1
    }	Rotate;

typedef 
enum Interpolate
    {	Nearest	= 0,
	Bilinear	= Nearest + 1,
	ThreeOrder	= Bilinear + 1
    }	Interpolate;

#pragma once
#pragma once
typedef union ResVal
    {
    UINT uVal;
    HBITMAP hbmVal;
    WCHAR __RPC_FAR *pwzVal;
    }	ResVal;

typedef union ColorFormatSel
    {
    RGBFormat eRGBVal;
    YCbCrFormat eYCbCrVal;
    JPEGFormat eJPEGVal;
    }	ColorFormatSel;

typedef struct  InitPara
    {
    BOOL bPreview;
    HWND hOwner;
    RECT rcWindowPos;
    DWORD dwReserved;
    }	InitPara;

typedef struct  ResolutionInfo
    {
    DWORD dwTotalCount;
    DWORD dwCurIndex;
    int nWidth;
    int nHeight;
    WCHAR wzDesc[ 20 ];
    }	ResolutionInfo;

typedef struct  CamPropertyRange
    {
    CameraProperty Property;
    long lMin;
    long lMax;
    long lSteppingDelta;
    long lDefault;
    long lCapsFlags;
    }	CamPropertyRange;

typedef struct  LightColor
    {
    BYTE byRed;
    BYTE byGreen;
    BYTE byBlue;
    BYTE byAlpha;
    }	LightColor;

typedef struct  ColorFormat
    {
    ColorSpaceSel csSel;
    ColorFormatSel cfSel;
    }	ColorFormat;

typedef struct  FrameData
    {
    BYTE __RPC_FAR *pDataBuf;
    DWORD dwDataLen;
    ColorFormat stuCF;
    DWORD dwBytesWritten;
    }	FrameData;

typedef struct  PlayInfo
    {
    SysPlayState lPlayStatus;
    double dCurPos;
    double dTotal;
    WCHAR wzCurPos[ 15 ];
    WCHAR wzTotal[ 15 ];
    }	PlayInfo;

typedef struct  CodecInfo
    {
    UINT uCodecIndex;
    CodecType eCodecType;
    WCHAR wzCodecFormat[ 10 ];
    GUID stuGuid;
    WCHAR wzVendor[ 50 ];
    }	CodecInfo;

typedef struct  CodecDesc
    {
    CaptureType eCapType;
    int nResolution;
    BSTR pwzVideoType;
    GUID __RPC_FAR *pClsidVideoEncoder;
    BSTR pwzAudioType;
    GUID __RPC_FAR *pClsidAudioEncoder;
    GUID __RPC_FAR *pClsidMux;
    }	CodecDesc;

typedef struct  CameraCap
    {
    DWORD dwIndex;
    WCHAR wzName[ 50 ];
    GUID devGuid;
    }	CameraCap;

typedef struct  WindowStatus
    {
    WindowState wndState;
    RECT rcWndPos;
    }	WindowStatus;

typedef struct  OverlayInitPara
    {
    OverlayType ovlyType;
    HWND hWnd;
    HINSTANCE hResInst;
    int nLeft;
    int nTop;
    int nWidth;
    int nHeight;
    BOOL bCameraUsed;
    LPVOID pPara;
    DWORD dwReserved;
    }	OverlayInitPara;

typedef struct  OverlayCap
    {
    BOOL bColorKey;
    BOOL bHalfAlphaBlend;
    BOOL bAlphaBlend;
    BOOL bAlphaChannel;
    BOOL bWindowMode;
    }	OverlayCap;

typedef struct  ResVariant
    {
    ResType resType;
    ResVal resVal;
    BOOL bShow;
    }	ResVariant;

typedef struct  ResUUID
    {
    ULONGLONG resID;
    DWORD dwReserved;
    }	ResUUID;

typedef struct  SysFont
    {
    LONG lfHeight;
    LONG lfWidth;
    LONG lfEscapement;
    LONG lfOrientation;
    LONG lfWeight;
    BYTE lfItalic;
    BYTE lfUnderline;
    BYTE lfStrikeOut;
    BYTE lfCharSet;
    BYTE lfOutPrecision;
    BYTE lfClipPrecision;
    BYTE lfQuality;
    BYTE lfPitchAndFamily;
    WCHAR lfFaceName[ 32 ];
    }	SysFont;

typedef struct  FontInfo
    {
    SysFont sysFont;
    DWORD dwBkgRGB;
    DWORD dwFrontRGB;
    BOOL bTransparent;
    int nAlphaVal;
    BlendAlgorithm bldAlgorithm;
    }	FontInfo;

typedef struct  ColorKeyAlpha
    {
    DWORD dwClrKey;
    int nAlpha;
    }	ColorKeyAlpha;

typedef struct  LinePara
    {
    POINT ptStart;
    POINT ptEnd;
    LineStyle lineStyle;
    int nLineWidth;
    DWORD dwColor;
    int nAlphaVal;
    BlendAlgorithm bldAlgorithm;
    BOOL bShow;
    }	LinePara;

typedef struct  RectPara
    {
    int left;
    int top;
    int width;
    int height;
    int nAlphaVal;
    BlendAlgorithm bldAlgorithm;
    DWORD dwLineColor;
    LineStyle lineStyle;
    int nLineWidth;
    BOOL bFill;
    DWORD dwFillColor;
    BOOL bShow;
    }	RectPara;

typedef struct  EllipsePara
    {
    int left;
    int top;
    int width;
    int height;
    LineStyle lineStyle;
    int nLineWidth;
    DWORD dwLineColor;
    int nAlphaVal;
    BlendAlgorithm bldAlgorithm;
    BOOL bShow;
    }	EllipsePara;

typedef void ( __RPC_FAR *EnumDeviceCallback )( 
    DWORD dwIndex,
    WCHAR __RPC_FAR *pwzName,
    GUID __RPC_FAR *pGuid,
    LPVOID pContext);

typedef void ( __RPC_FAR *EnumCodecCallback )( 
    CodecInfo __RPC_FAR *pCodecInfo,
    LPVOID pContext);

typedef void ( __RPC_FAR *EnumResolutionCallback )( 
    ResolutionType resType,
    ResolutionInfo resInfo,
    LPVOID pContext);

typedef void ( __RPC_FAR *EnumOverlayCallback )( 
    OverlayType overlayType,
    OverlayCap ovlyCap,
    LPVOID pContext);

typedef void ( __RPC_FAR *LeCamEventCallback )( 
    LeCamEvent camEvent,
    LPVOID pPara,
    LPVOID pContext);



extern RPC_IF_HANDLE __MIDL_itf_LeDShowLib_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_LeDShowLib_0000_v0_0_s_ifspec;

#ifndef __ILeCamera_INTERFACE_DEFINED__
#define __ILeCamera_INTERFACE_DEFINED__

/* interface ILeCamera */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeCamera;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D5106DBA-2B1A-42cb-9334-DD260CD8DD3B")
    ILeCamera : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetNumDevs( 
            /* [out][in] */ UINT __RPC_FAR *uNum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDevCap( 
            /* [in] */ UINT uIndex,
            /* [out] */ LPVOID pCap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCamDev( 
            /* [in] */ EnumDeviceCallback pCallback,
            /* [in] */ LPVOID pContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCurCamDev( 
            /* [in] */ UINT uCurDev) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ UINT uDevIndex,
            /* [in] */ InitPara __RPC_FAR *pInitPara) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnInit( 
            /* [in] */ UINT uDevIndex) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeCameraVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeCamera __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeCamera __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeCamera __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNumDevs )( 
            ILeCamera __RPC_FAR * This,
            /* [out][in] */ UINT __RPC_FAR *uNum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDevCap )( 
            ILeCamera __RPC_FAR * This,
            /* [in] */ UINT uIndex,
            /* [out] */ LPVOID pCap);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumCamDev )( 
            ILeCamera __RPC_FAR * This,
            /* [in] */ EnumDeviceCallback pCallback,
            /* [in] */ LPVOID pContext);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetCurCamDev )( 
            ILeCamera __RPC_FAR * This,
            /* [in] */ UINT uCurDev);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            ILeCamera __RPC_FAR * This,
            /* [in] */ UINT uDevIndex,
            /* [in] */ InitPara __RPC_FAR *pInitPara);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnInit )( 
            ILeCamera __RPC_FAR * This,
            /* [in] */ UINT uDevIndex);
        
        END_INTERFACE
    } ILeCameraVtbl;

    interface ILeCamera
    {
        CONST_VTBL struct ILeCameraVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeCamera_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeCamera_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeCamera_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeCamera_GetNumDevs(This,uNum)	\
    (This)->lpVtbl -> GetNumDevs(This,uNum)

#define ILeCamera_GetDevCap(This,uIndex,pCap)	\
    (This)->lpVtbl -> GetDevCap(This,uIndex,pCap)

#define ILeCamera_EnumCamDev(This,pCallback,pContext)	\
    (This)->lpVtbl -> EnumCamDev(This,pCallback,pContext)

#define ILeCamera_SetCurCamDev(This,uCurDev)	\
    (This)->lpVtbl -> SetCurCamDev(This,uCurDev)

#define ILeCamera_Init(This,uDevIndex,pInitPara)	\
    (This)->lpVtbl -> Init(This,uDevIndex,pInitPara)

#define ILeCamera_UnInit(This,uDevIndex)	\
    (This)->lpVtbl -> UnInit(This,uDevIndex)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeCamera_GetNumDevs_Proxy( 
    ILeCamera __RPC_FAR * This,
    /* [out][in] */ UINT __RPC_FAR *uNum);


void __RPC_STUB ILeCamera_GetNumDevs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamera_GetDevCap_Proxy( 
    ILeCamera __RPC_FAR * This,
    /* [in] */ UINT uIndex,
    /* [out] */ LPVOID pCap);


void __RPC_STUB ILeCamera_GetDevCap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamera_EnumCamDev_Proxy( 
    ILeCamera __RPC_FAR * This,
    /* [in] */ EnumDeviceCallback pCallback,
    /* [in] */ LPVOID pContext);


void __RPC_STUB ILeCamera_EnumCamDev_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamera_SetCurCamDev_Proxy( 
    ILeCamera __RPC_FAR * This,
    /* [in] */ UINT uCurDev);


void __RPC_STUB ILeCamera_SetCurCamDev_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamera_Init_Proxy( 
    ILeCamera __RPC_FAR * This,
    /* [in] */ UINT uDevIndex,
    /* [in] */ InitPara __RPC_FAR *pInitPara);


void __RPC_STUB ILeCamera_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamera_UnInit_Proxy( 
    ILeCamera __RPC_FAR * This,
    /* [in] */ UINT uDevIndex);


void __RPC_STUB ILeCamera_UnInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeCamera_INTERFACE_DEFINED__ */


#ifndef __ILeCamControl_INTERFACE_DEFINED__
#define __ILeCamControl_INTERFACE_DEFINED__

/* interface ILeCamControl */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeCamControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D4AC908C-571D-4e02-B8A6-8F327F0C4D5B")
    ILeCamControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [in] */ CameraProperty camProperty,
            /* [out][in] */ LPVOID pProperty) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPropertyRange( 
            /* [in] */ CameraProperty camProperty,
            /* [out][in] */ CamPropertyRange __RPC_FAR *pCamProRange) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetProperty( 
            /* [in] */ CameraProperty camProperty,
            /* [in] */ LPVOID pProperty) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeCamControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeCamControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeCamControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeCamControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetProperty )( 
            ILeCamControl __RPC_FAR * This,
            /* [in] */ CameraProperty camProperty,
            /* [out][in] */ LPVOID pProperty);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPropertyRange )( 
            ILeCamControl __RPC_FAR * This,
            /* [in] */ CameraProperty camProperty,
            /* [out][in] */ CamPropertyRange __RPC_FAR *pCamProRange);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetProperty )( 
            ILeCamControl __RPC_FAR * This,
            /* [in] */ CameraProperty camProperty,
            /* [in] */ LPVOID pProperty);
        
        END_INTERFACE
    } ILeCamControlVtbl;

    interface ILeCamControl
    {
        CONST_VTBL struct ILeCamControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeCamControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeCamControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeCamControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeCamControl_GetProperty(This,camProperty,pProperty)	\
    (This)->lpVtbl -> GetProperty(This,camProperty,pProperty)

#define ILeCamControl_GetPropertyRange(This,camProperty,pCamProRange)	\
    (This)->lpVtbl -> GetPropertyRange(This,camProperty,pCamProRange)

#define ILeCamControl_SetProperty(This,camProperty,pProperty)	\
    (This)->lpVtbl -> SetProperty(This,camProperty,pProperty)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeCamControl_GetProperty_Proxy( 
    ILeCamControl __RPC_FAR * This,
    /* [in] */ CameraProperty camProperty,
    /* [out][in] */ LPVOID pProperty);


void __RPC_STUB ILeCamControl_GetProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamControl_GetPropertyRange_Proxy( 
    ILeCamControl __RPC_FAR * This,
    /* [in] */ CameraProperty camProperty,
    /* [out][in] */ CamPropertyRange __RPC_FAR *pCamProRange);


void __RPC_STUB ILeCamControl_GetPropertyRange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamControl_SetProperty_Proxy( 
    ILeCamControl __RPC_FAR * This,
    /* [in] */ CameraProperty camProperty,
    /* [in] */ LPVOID pProperty);


void __RPC_STUB ILeCamControl_SetProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeCamControl_INTERFACE_DEFINED__ */


#ifndef __ILeCamLight_INTERFACE_DEFINED__
#define __ILeCamLight_INTERFACE_DEFINED__

/* interface ILeCamLight */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeCamLight;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7C5DDA3-E87A-4967-BE22-9CA0F48DA569")
    ILeCamLight : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFlashColor( 
            /* [in] */ LightColor __RPC_FAR *pLightColor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAlwaysOnColor( 
            /* [in] */ LightColor __RPC_FAR *pLightColor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFlashBrightness( 
            /* [in] */ DWORD dwVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAlwaysOnBrightness( 
            /* [in] */ DWORD dwVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FlashControl( 
            /* [in] */ BOOL bFlash) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AlwaysOnControl( 
            /* [in] */ BOOL bAlwaysOn) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeCamLightVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeCamLight __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeCamLight __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeCamLight __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFlashColor )( 
            ILeCamLight __RPC_FAR * This,
            /* [in] */ LightColor __RPC_FAR *pLightColor);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAlwaysOnColor )( 
            ILeCamLight __RPC_FAR * This,
            /* [in] */ LightColor __RPC_FAR *pLightColor);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFlashBrightness )( 
            ILeCamLight __RPC_FAR * This,
            /* [in] */ DWORD dwVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAlwaysOnBrightness )( 
            ILeCamLight __RPC_FAR * This,
            /* [in] */ DWORD dwVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FlashControl )( 
            ILeCamLight __RPC_FAR * This,
            /* [in] */ BOOL bFlash);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AlwaysOnControl )( 
            ILeCamLight __RPC_FAR * This,
            /* [in] */ BOOL bAlwaysOn);
        
        END_INTERFACE
    } ILeCamLightVtbl;

    interface ILeCamLight
    {
        CONST_VTBL struct ILeCamLightVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeCamLight_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeCamLight_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeCamLight_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeCamLight_SetFlashColor(This,pLightColor)	\
    (This)->lpVtbl -> SetFlashColor(This,pLightColor)

#define ILeCamLight_SetAlwaysOnColor(This,pLightColor)	\
    (This)->lpVtbl -> SetAlwaysOnColor(This,pLightColor)

#define ILeCamLight_SetFlashBrightness(This,dwVal)	\
    (This)->lpVtbl -> SetFlashBrightness(This,dwVal)

#define ILeCamLight_SetAlwaysOnBrightness(This,dwVal)	\
    (This)->lpVtbl -> SetAlwaysOnBrightness(This,dwVal)

#define ILeCamLight_FlashControl(This,bFlash)	\
    (This)->lpVtbl -> FlashControl(This,bFlash)

#define ILeCamLight_AlwaysOnControl(This,bAlwaysOn)	\
    (This)->lpVtbl -> AlwaysOnControl(This,bAlwaysOn)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeCamLight_SetFlashColor_Proxy( 
    ILeCamLight __RPC_FAR * This,
    /* [in] */ LightColor __RPC_FAR *pLightColor);


void __RPC_STUB ILeCamLight_SetFlashColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamLight_SetAlwaysOnColor_Proxy( 
    ILeCamLight __RPC_FAR * This,
    /* [in] */ LightColor __RPC_FAR *pLightColor);


void __RPC_STUB ILeCamLight_SetAlwaysOnColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamLight_SetFlashBrightness_Proxy( 
    ILeCamLight __RPC_FAR * This,
    /* [in] */ DWORD dwVal);


void __RPC_STUB ILeCamLight_SetFlashBrightness_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamLight_SetAlwaysOnBrightness_Proxy( 
    ILeCamLight __RPC_FAR * This,
    /* [in] */ DWORD dwVal);


void __RPC_STUB ILeCamLight_SetAlwaysOnBrightness_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamLight_FlashControl_Proxy( 
    ILeCamLight __RPC_FAR * This,
    /* [in] */ BOOL bFlash);


void __RPC_STUB ILeCamLight_FlashControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamLight_AlwaysOnControl_Proxy( 
    ILeCamLight __RPC_FAR * This,
    /* [in] */ BOOL bAlwaysOn);


void __RPC_STUB ILeCamLight_AlwaysOnControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeCamLight_INTERFACE_DEFINED__ */


#ifndef __ILeCamPreview_INTERFACE_DEFINED__
#define __ILeCamPreview_INTERFACE_DEFINED__

/* interface ILeCamPreview */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeCamPreview;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EDE2B917-143E-41ea-8B7A-AB589BC39EA9")
    ILeCamPreview : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFrameData( 
            /* [in] */ ColorFormat __RPC_FAR *pstuCF,
            /* [out][in] */ FrameData __RPC_FAR *pFrmData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFrameYData( 
            /* [out][in] */ FrameData __RPC_FAR *pFrmData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryDataRegister( 
            /* [in] */ BOOL bRegister,
            /* [in] */ LPVOID pReseved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeCamPreviewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeCamPreview __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeCamPreview __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeCamPreview __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            ILeCamPreview __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            ILeCamPreview __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            ILeCamPreview __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFrameData )( 
            ILeCamPreview __RPC_FAR * This,
            /* [in] */ ColorFormat __RPC_FAR *pstuCF,
            /* [out][in] */ FrameData __RPC_FAR *pFrmData);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFrameYData )( 
            ILeCamPreview __RPC_FAR * This,
            /* [out][in] */ FrameData __RPC_FAR *pFrmData);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryDataRegister )( 
            ILeCamPreview __RPC_FAR * This,
            /* [in] */ BOOL bRegister,
            /* [in] */ LPVOID pReseved);
        
        END_INTERFACE
    } ILeCamPreviewVtbl;

    interface ILeCamPreview
    {
        CONST_VTBL struct ILeCamPreviewVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeCamPreview_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeCamPreview_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeCamPreview_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeCamPreview_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define ILeCamPreview_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define ILeCamPreview_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define ILeCamPreview_GetFrameData(This,pstuCF,pFrmData)	\
    (This)->lpVtbl -> GetFrameData(This,pstuCF,pFrmData)

#define ILeCamPreview_GetFrameYData(This,pFrmData)	\
    (This)->lpVtbl -> GetFrameYData(This,pFrmData)

#define ILeCamPreview_QueryDataRegister(This,bRegister,pReseved)	\
    (This)->lpVtbl -> QueryDataRegister(This,bRegister,pReseved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeCamPreview_Start_Proxy( 
    ILeCamPreview __RPC_FAR * This);


void __RPC_STUB ILeCamPreview_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamPreview_Pause_Proxy( 
    ILeCamPreview __RPC_FAR * This);


void __RPC_STUB ILeCamPreview_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamPreview_Stop_Proxy( 
    ILeCamPreview __RPC_FAR * This);


void __RPC_STUB ILeCamPreview_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamPreview_GetFrameData_Proxy( 
    ILeCamPreview __RPC_FAR * This,
    /* [in] */ ColorFormat __RPC_FAR *pstuCF,
    /* [out][in] */ FrameData __RPC_FAR *pFrmData);


void __RPC_STUB ILeCamPreview_GetFrameData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamPreview_GetFrameYData_Proxy( 
    ILeCamPreview __RPC_FAR * This,
    /* [out][in] */ FrameData __RPC_FAR *pFrmData);


void __RPC_STUB ILeCamPreview_GetFrameYData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamPreview_QueryDataRegister_Proxy( 
    ILeCamPreview __RPC_FAR * This,
    /* [in] */ BOOL bRegister,
    /* [in] */ LPVOID pReseved);


void __RPC_STUB ILeCamPreview_QueryDataRegister_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeCamPreview_INTERFACE_DEFINED__ */


#ifndef __ILeCamWindow_INTERFACE_DEFINED__
#define __ILeCamWindow_INTERFACE_DEFINED__

/* interface ILeCamWindow */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeCamWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B55BED84-4E13-4cfb-B5AE-3ED48379DFDF")
    ILeCamWindow : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ BOOL bShow) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangePos( 
            /* [in] */ RECT __RPC_FAR *pPosRect) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOwner( 
            /* [in] */ HWND hParent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStatus( 
            /* [in] */ WindowStatus __RPC_FAR *pWndStatus) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeCamWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeCamWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeCamWindow __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeCamWindow __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Show )( 
            ILeCamWindow __RPC_FAR * This,
            /* [in] */ BOOL bShow);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangePos )( 
            ILeCamWindow __RPC_FAR * This,
            /* [in] */ RECT __RPC_FAR *pPosRect);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetOwner )( 
            ILeCamWindow __RPC_FAR * This,
            /* [in] */ HWND hParent);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetStatus )( 
            ILeCamWindow __RPC_FAR * This,
            /* [in] */ WindowStatus __RPC_FAR *pWndStatus);
        
        END_INTERFACE
    } ILeCamWindowVtbl;

    interface ILeCamWindow
    {
        CONST_VTBL struct ILeCamWindowVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeCamWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeCamWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeCamWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeCamWindow_Show(This,bShow)	\
    (This)->lpVtbl -> Show(This,bShow)

#define ILeCamWindow_ChangePos(This,pPosRect)	\
    (This)->lpVtbl -> ChangePos(This,pPosRect)

#define ILeCamWindow_SetOwner(This,hParent)	\
    (This)->lpVtbl -> SetOwner(This,hParent)

#define ILeCamWindow_GetStatus(This,pWndStatus)	\
    (This)->lpVtbl -> GetStatus(This,pWndStatus)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeCamWindow_Show_Proxy( 
    ILeCamWindow __RPC_FAR * This,
    /* [in] */ BOOL bShow);


void __RPC_STUB ILeCamWindow_Show_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamWindow_ChangePos_Proxy( 
    ILeCamWindow __RPC_FAR * This,
    /* [in] */ RECT __RPC_FAR *pPosRect);


void __RPC_STUB ILeCamWindow_ChangePos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamWindow_SetOwner_Proxy( 
    ILeCamWindow __RPC_FAR * This,
    /* [in] */ HWND hParent);


void __RPC_STUB ILeCamWindow_SetOwner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamWindow_GetStatus_Proxy( 
    ILeCamWindow __RPC_FAR * This,
    /* [in] */ WindowStatus __RPC_FAR *pWndStatus);


void __RPC_STUB ILeCamWindow_GetStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeCamWindow_INTERFACE_DEFINED__ */


#ifndef __ILeCamStill_INTERFACE_DEFINED__
#define __ILeCamStill_INTERFACE_DEFINED__

/* interface ILeCamStill */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeCamStill;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6B5C7E8C-4C66-47d0-BFBE-81E0D2CD7EAA")
    ILeCamStill : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetSnapSound( 
            /* [in] */ BSTR pwzSoundFile) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SnapshotSync( 
            /* [in] */ BSTR pwzFile,
            /* [in] */ DWORD dwTimeOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SnapshotAsyn( 
            /* [in] */ BSTR pwzFile) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SnapshotAFSync( 
            /* [in] */ BSTR pwzFile,
            /* [in] */ DWORD dwTimeOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SnapshotAFAsyn( 
            /* [in] */ BSTR pwzFile) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SnapshotBuffer( 
            /* [in] */ ColorFormat __RPC_FAR *pCF,
            /* [out][in] */ FrameData __RPC_FAR *pFrmData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SnapshotYBuffer( 
            /* [out][in] */ FrameData __RPC_FAR *pFrmData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StartMutiSnapshot( 
            /* [in] */ DWORD dwPara) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopMutiSnapshot( 
            /* [in] */ DWORD dwPara) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeCamStillVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeCamStill __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeCamStill __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeCamStill __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetSnapSound )( 
            ILeCamStill __RPC_FAR * This,
            /* [in] */ BSTR pwzSoundFile);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SnapshotSync )( 
            ILeCamStill __RPC_FAR * This,
            /* [in] */ BSTR pwzFile,
            /* [in] */ DWORD dwTimeOut);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SnapshotAsyn )( 
            ILeCamStill __RPC_FAR * This,
            /* [in] */ BSTR pwzFile);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SnapshotAFSync )( 
            ILeCamStill __RPC_FAR * This,
            /* [in] */ BSTR pwzFile,
            /* [in] */ DWORD dwTimeOut);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SnapshotAFAsyn )( 
            ILeCamStill __RPC_FAR * This,
            /* [in] */ BSTR pwzFile);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SnapshotBuffer )( 
            ILeCamStill __RPC_FAR * This,
            /* [in] */ ColorFormat __RPC_FAR *pCF,
            /* [out][in] */ FrameData __RPC_FAR *pFrmData);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SnapshotYBuffer )( 
            ILeCamStill __RPC_FAR * This,
            /* [out][in] */ FrameData __RPC_FAR *pFrmData);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartMutiSnapshot )( 
            ILeCamStill __RPC_FAR * This,
            /* [in] */ DWORD dwPara);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopMutiSnapshot )( 
            ILeCamStill __RPC_FAR * This,
            /* [in] */ DWORD dwPara);
        
        END_INTERFACE
    } ILeCamStillVtbl;

    interface ILeCamStill
    {
        CONST_VTBL struct ILeCamStillVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeCamStill_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeCamStill_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeCamStill_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeCamStill_SetSnapSound(This,pwzSoundFile)	\
    (This)->lpVtbl -> SetSnapSound(This,pwzSoundFile)

#define ILeCamStill_SnapshotSync(This,pwzFile,dwTimeOut)	\
    (This)->lpVtbl -> SnapshotSync(This,pwzFile,dwTimeOut)

#define ILeCamStill_SnapshotAsyn(This,pwzFile)	\
    (This)->lpVtbl -> SnapshotAsyn(This,pwzFile)

#define ILeCamStill_SnapshotAFSync(This,pwzFile,dwTimeOut)	\
    (This)->lpVtbl -> SnapshotAFSync(This,pwzFile,dwTimeOut)

#define ILeCamStill_SnapshotAFAsyn(This,pwzFile)	\
    (This)->lpVtbl -> SnapshotAFAsyn(This,pwzFile)

#define ILeCamStill_SnapshotBuffer(This,pCF,pFrmData)	\
    (This)->lpVtbl -> SnapshotBuffer(This,pCF,pFrmData)

#define ILeCamStill_SnapshotYBuffer(This,pFrmData)	\
    (This)->lpVtbl -> SnapshotYBuffer(This,pFrmData)

#define ILeCamStill_StartMutiSnapshot(This,dwPara)	\
    (This)->lpVtbl -> StartMutiSnapshot(This,dwPara)

#define ILeCamStill_StopMutiSnapshot(This,dwPara)	\
    (This)->lpVtbl -> StopMutiSnapshot(This,dwPara)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeCamStill_SetSnapSound_Proxy( 
    ILeCamStill __RPC_FAR * This,
    /* [in] */ BSTR pwzSoundFile);


void __RPC_STUB ILeCamStill_SetSnapSound_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamStill_SnapshotSync_Proxy( 
    ILeCamStill __RPC_FAR * This,
    /* [in] */ BSTR pwzFile,
    /* [in] */ DWORD dwTimeOut);


void __RPC_STUB ILeCamStill_SnapshotSync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamStill_SnapshotAsyn_Proxy( 
    ILeCamStill __RPC_FAR * This,
    /* [in] */ BSTR pwzFile);


void __RPC_STUB ILeCamStill_SnapshotAsyn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamStill_SnapshotAFSync_Proxy( 
    ILeCamStill __RPC_FAR * This,
    /* [in] */ BSTR pwzFile,
    /* [in] */ DWORD dwTimeOut);


void __RPC_STUB ILeCamStill_SnapshotAFSync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamStill_SnapshotAFAsyn_Proxy( 
    ILeCamStill __RPC_FAR * This,
    /* [in] */ BSTR pwzFile);


void __RPC_STUB ILeCamStill_SnapshotAFAsyn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamStill_SnapshotBuffer_Proxy( 
    ILeCamStill __RPC_FAR * This,
    /* [in] */ ColorFormat __RPC_FAR *pCF,
    /* [out][in] */ FrameData __RPC_FAR *pFrmData);


void __RPC_STUB ILeCamStill_SnapshotBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamStill_SnapshotYBuffer_Proxy( 
    ILeCamStill __RPC_FAR * This,
    /* [out][in] */ FrameData __RPC_FAR *pFrmData);


void __RPC_STUB ILeCamStill_SnapshotYBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamStill_StartMutiSnapshot_Proxy( 
    ILeCamStill __RPC_FAR * This,
    /* [in] */ DWORD dwPara);


void __RPC_STUB ILeCamStill_StartMutiSnapshot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamStill_StopMutiSnapshot_Proxy( 
    ILeCamStill __RPC_FAR * This,
    /* [in] */ DWORD dwPara);


void __RPC_STUB ILeCamStill_StopMutiSnapshot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeCamStill_INTERFACE_DEFINED__ */


#ifndef __ILeCamCapture_INTERFACE_DEFINED__
#define __ILeCamCapture_INTERFACE_DEFINED__

/* interface ILeCamCapture */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeCamCapture;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A078E505-FCA9-4884-BB1A-C2656E7F795D")
    ILeCamCapture : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumCodec( 
            /* [in] */ EnumCodecCallback callbackFunc,
            /* [in] */ LPVOID pContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddCodec( 
            /* [in] */ CodecDesc __RPC_FAR *pCodecDesc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangeCodec( 
            /* [in] */ CodecType eCT,
            /* [in] */ BSTR pwzType,
            /* [in] */ GUID __RPC_FAR *pClsidEncoder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurCodec( 
            /* [out][in] */ CodecInfo __RPC_FAR *pCodecInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFile( 
            /* [in] */ BSTR pwzFile) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StartCap( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopCap( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCapRate( 
            /* [in] */ DWORD dwRate) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeCamCaptureVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeCamCapture __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeCamCapture __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeCamCapture __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumCodec )( 
            ILeCamCapture __RPC_FAR * This,
            /* [in] */ EnumCodecCallback callbackFunc,
            /* [in] */ LPVOID pContext);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddCodec )( 
            ILeCamCapture __RPC_FAR * This,
            /* [in] */ CodecDesc __RPC_FAR *pCodecDesc);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeCodec )( 
            ILeCamCapture __RPC_FAR * This,
            /* [in] */ CodecType eCT,
            /* [in] */ BSTR pwzType,
            /* [in] */ GUID __RPC_FAR *pClsidEncoder);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurCodec )( 
            ILeCamCapture __RPC_FAR * This,
            /* [out][in] */ CodecInfo __RPC_FAR *pCodecInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFile )( 
            ILeCamCapture __RPC_FAR * This,
            /* [in] */ BSTR pwzFile);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartCap )( 
            ILeCamCapture __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopCap )( 
            ILeCamCapture __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resume )( 
            ILeCamCapture __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetCapRate )( 
            ILeCamCapture __RPC_FAR * This,
            /* [in] */ DWORD dwRate);
        
        END_INTERFACE
    } ILeCamCaptureVtbl;

    interface ILeCamCapture
    {
        CONST_VTBL struct ILeCamCaptureVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeCamCapture_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeCamCapture_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeCamCapture_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeCamCapture_EnumCodec(This,callbackFunc,pContext)	\
    (This)->lpVtbl -> EnumCodec(This,callbackFunc,pContext)

#define ILeCamCapture_AddCodec(This,pCodecDesc)	\
    (This)->lpVtbl -> AddCodec(This,pCodecDesc)

#define ILeCamCapture_ChangeCodec(This,eCT,pwzType,pClsidEncoder)	\
    (This)->lpVtbl -> ChangeCodec(This,eCT,pwzType,pClsidEncoder)

#define ILeCamCapture_GetCurCodec(This,pCodecInfo)	\
    (This)->lpVtbl -> GetCurCodec(This,pCodecInfo)

#define ILeCamCapture_SetFile(This,pwzFile)	\
    (This)->lpVtbl -> SetFile(This,pwzFile)

#define ILeCamCapture_StartCap(This)	\
    (This)->lpVtbl -> StartCap(This)

#define ILeCamCapture_StopCap(This)	\
    (This)->lpVtbl -> StopCap(This)

#define ILeCamCapture_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#define ILeCamCapture_SetCapRate(This,dwRate)	\
    (This)->lpVtbl -> SetCapRate(This,dwRate)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeCamCapture_EnumCodec_Proxy( 
    ILeCamCapture __RPC_FAR * This,
    /* [in] */ EnumCodecCallback callbackFunc,
    /* [in] */ LPVOID pContext);


void __RPC_STUB ILeCamCapture_EnumCodec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamCapture_AddCodec_Proxy( 
    ILeCamCapture __RPC_FAR * This,
    /* [in] */ CodecDesc __RPC_FAR *pCodecDesc);


void __RPC_STUB ILeCamCapture_AddCodec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamCapture_ChangeCodec_Proxy( 
    ILeCamCapture __RPC_FAR * This,
    /* [in] */ CodecType eCT,
    /* [in] */ BSTR pwzType,
    /* [in] */ GUID __RPC_FAR *pClsidEncoder);


void __RPC_STUB ILeCamCapture_ChangeCodec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamCapture_GetCurCodec_Proxy( 
    ILeCamCapture __RPC_FAR * This,
    /* [out][in] */ CodecInfo __RPC_FAR *pCodecInfo);


void __RPC_STUB ILeCamCapture_GetCurCodec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamCapture_SetFile_Proxy( 
    ILeCamCapture __RPC_FAR * This,
    /* [in] */ BSTR pwzFile);


void __RPC_STUB ILeCamCapture_SetFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamCapture_StartCap_Proxy( 
    ILeCamCapture __RPC_FAR * This);


void __RPC_STUB ILeCamCapture_StartCap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamCapture_StopCap_Proxy( 
    ILeCamCapture __RPC_FAR * This);


void __RPC_STUB ILeCamCapture_StopCap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamCapture_Resume_Proxy( 
    ILeCamCapture __RPC_FAR * This);


void __RPC_STUB ILeCamCapture_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamCapture_SetCapRate_Proxy( 
    ILeCamCapture __RPC_FAR * This,
    /* [in] */ DWORD dwRate);


void __RPC_STUB ILeCamCapture_SetCapRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeCamCapture_INTERFACE_DEFINED__ */


#ifndef __ILeCamResoluion_INTERFACE_DEFINED__
#define __ILeCamResoluion_INTERFACE_DEFINED__

/* interface ILeCamResoluion */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeCamResoluion;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A91CD4D5-1792-46f4-AAE1-1A02C6CE9475")
    ILeCamResoluion : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetResolutionCount( 
            /* [in] */ ResolutionType eResType,
            /* [out][in] */ UINT __RPC_FAR *uResCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResolutionInfo( 
            /* [in] */ ResolutionType eResType,
            /* [in] */ UINT uResIndex,
            /* [out][in] */ ResolutionInfo __RPC_FAR *pResInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResolutionCur( 
            /* [in] */ ResolutionType eResType,
            /* [out][in] */ ResolutionInfo __RPC_FAR *pResInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumResolution( 
            /* [in] */ ResolutionType eResType,
            /* [in] */ EnumResolutionCallback enumResCallback,
            /* [in] */ LPVOID pContex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangeResolution( 
            /* [in] */ ResolutionType eResType,
            /* [in] */ int nCurSel) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeCamResoluionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeCamResoluion __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeCamResoluion __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeCamResoluion __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetResolutionCount )( 
            ILeCamResoluion __RPC_FAR * This,
            /* [in] */ ResolutionType eResType,
            /* [out][in] */ UINT __RPC_FAR *uResCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetResolutionInfo )( 
            ILeCamResoluion __RPC_FAR * This,
            /* [in] */ ResolutionType eResType,
            /* [in] */ UINT uResIndex,
            /* [out][in] */ ResolutionInfo __RPC_FAR *pResInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetResolutionCur )( 
            ILeCamResoluion __RPC_FAR * This,
            /* [in] */ ResolutionType eResType,
            /* [out][in] */ ResolutionInfo __RPC_FAR *pResInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumResolution )( 
            ILeCamResoluion __RPC_FAR * This,
            /* [in] */ ResolutionType eResType,
            /* [in] */ EnumResolutionCallback enumResCallback,
            /* [in] */ LPVOID pContex);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeResolution )( 
            ILeCamResoluion __RPC_FAR * This,
            /* [in] */ ResolutionType eResType,
            /* [in] */ int nCurSel);
        
        END_INTERFACE
    } ILeCamResoluionVtbl;

    interface ILeCamResoluion
    {
        CONST_VTBL struct ILeCamResoluionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeCamResoluion_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeCamResoluion_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeCamResoluion_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeCamResoluion_GetResolutionCount(This,eResType,uResCount)	\
    (This)->lpVtbl -> GetResolutionCount(This,eResType,uResCount)

#define ILeCamResoluion_GetResolutionInfo(This,eResType,uResIndex,pResInfo)	\
    (This)->lpVtbl -> GetResolutionInfo(This,eResType,uResIndex,pResInfo)

#define ILeCamResoluion_GetResolutionCur(This,eResType,pResInfo)	\
    (This)->lpVtbl -> GetResolutionCur(This,eResType,pResInfo)

#define ILeCamResoluion_EnumResolution(This,eResType,enumResCallback,pContex)	\
    (This)->lpVtbl -> EnumResolution(This,eResType,enumResCallback,pContex)

#define ILeCamResoluion_ChangeResolution(This,eResType,nCurSel)	\
    (This)->lpVtbl -> ChangeResolution(This,eResType,nCurSel)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeCamResoluion_GetResolutionCount_Proxy( 
    ILeCamResoluion __RPC_FAR * This,
    /* [in] */ ResolutionType eResType,
    /* [out][in] */ UINT __RPC_FAR *uResCount);


void __RPC_STUB ILeCamResoluion_GetResolutionCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamResoluion_GetResolutionInfo_Proxy( 
    ILeCamResoluion __RPC_FAR * This,
    /* [in] */ ResolutionType eResType,
    /* [in] */ UINT uResIndex,
    /* [out][in] */ ResolutionInfo __RPC_FAR *pResInfo);


void __RPC_STUB ILeCamResoluion_GetResolutionInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamResoluion_GetResolutionCur_Proxy( 
    ILeCamResoluion __RPC_FAR * This,
    /* [in] */ ResolutionType eResType,
    /* [out][in] */ ResolutionInfo __RPC_FAR *pResInfo);


void __RPC_STUB ILeCamResoluion_GetResolutionCur_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamResoluion_EnumResolution_Proxy( 
    ILeCamResoluion __RPC_FAR * This,
    /* [in] */ ResolutionType eResType,
    /* [in] */ EnumResolutionCallback enumResCallback,
    /* [in] */ LPVOID pContex);


void __RPC_STUB ILeCamResoluion_EnumResolution_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamResoluion_ChangeResolution_Proxy( 
    ILeCamResoluion __RPC_FAR * This,
    /* [in] */ ResolutionType eResType,
    /* [in] */ int nCurSel);


void __RPC_STUB ILeCamResoluion_ChangeResolution_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeCamResoluion_INTERFACE_DEFINED__ */


#ifndef __ILeImageProcess_INTERFACE_DEFINED__
#define __ILeImageProcess_INTERFACE_DEFINED__

/* interface ILeImageProcess */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeImageProcess;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C3583582-69ED-4d23-B671-DBBF1A25FBE7")
    ILeImageProcess : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE YCbCr422toYCbCr420Rotate_P3( 
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ BYTE __RPC_FAR *lpSrc,
            /* [out][in] */ BYTE __RPC_FAR *lpDst,
            /* [in] */ Rotate nRotateDegree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE YCbCrRszRot_P3( 
            /* [in] */ YCbCrFormat eYCbCrFormat,
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ int nDstWidth,
            /* [in] */ int nDstHeight,
            /* [in] */ BYTE __RPC_FAR *lpSrc,
            /* [out][in] */ BYTE __RPC_FAR *lpDst,
            /* [in] */ Interpolate eInterpolate,
            /* [in] */ Rotate eRotateDegree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE YCbCrtoRGB_P3C3( 
            /* [in] */ YCbCrFormat eYCbCrFormat,
            /* [in] */ RGBFormat eRGBFormat,
            /* [in] */ int nWidth,
            /* [in] */ int nHeight,
            /* [in] */ BYTE __RPC_FAR *lpSrc,
            /* [out][in] */ LPVOID lpDst) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE YCbCrtoRGBRszRot_P3C3( 
            /* [in] */ YCbCrFormat eYCbCrFormat,
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ RGBFormat eRGBFormat,
            /* [in] */ int nDstWidth,
            /* [in] */ int nDstHeight,
            /* [in] */ LPVOID lpSrc,
            /* [out][in] */ LPVOID lpDst,
            /* [in] */ Interpolate eInterpolate,
            /* [in] */ Rotate eRotateDegree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RGBRotate( 
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ LPVOID lpSrc,
            /* [out][in] */ LPVOID lpDst,
            /* [in] */ int nPixelLen,
            /* [in] */ Rotate eRotateDegree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RGBConvert( 
            /* [in] */ RGBFormat eInRGBFormat,
            /* [in] */ RGBFormat eOutRGBFormat,
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ LPVOID lpSrc,
            /* [out][in] */ LPVOID lpDst) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeImageProcessVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeImageProcess __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeImageProcess __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeImageProcess __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *YCbCr422toYCbCr420Rotate_P3 )( 
            ILeImageProcess __RPC_FAR * This,
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ BYTE __RPC_FAR *lpSrc,
            /* [out][in] */ BYTE __RPC_FAR *lpDst,
            /* [in] */ Rotate nRotateDegree);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *YCbCrRszRot_P3 )( 
            ILeImageProcess __RPC_FAR * This,
            /* [in] */ YCbCrFormat eYCbCrFormat,
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ int nDstWidth,
            /* [in] */ int nDstHeight,
            /* [in] */ BYTE __RPC_FAR *lpSrc,
            /* [out][in] */ BYTE __RPC_FAR *lpDst,
            /* [in] */ Interpolate eInterpolate,
            /* [in] */ Rotate eRotateDegree);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *YCbCrtoRGB_P3C3 )( 
            ILeImageProcess __RPC_FAR * This,
            /* [in] */ YCbCrFormat eYCbCrFormat,
            /* [in] */ RGBFormat eRGBFormat,
            /* [in] */ int nWidth,
            /* [in] */ int nHeight,
            /* [in] */ BYTE __RPC_FAR *lpSrc,
            /* [out][in] */ LPVOID lpDst);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *YCbCrtoRGBRszRot_P3C3 )( 
            ILeImageProcess __RPC_FAR * This,
            /* [in] */ YCbCrFormat eYCbCrFormat,
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ RGBFormat eRGBFormat,
            /* [in] */ int nDstWidth,
            /* [in] */ int nDstHeight,
            /* [in] */ LPVOID lpSrc,
            /* [out][in] */ LPVOID lpDst,
            /* [in] */ Interpolate eInterpolate,
            /* [in] */ Rotate eRotateDegree);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RGBRotate )( 
            ILeImageProcess __RPC_FAR * This,
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ LPVOID lpSrc,
            /* [out][in] */ LPVOID lpDst,
            /* [in] */ int nPixelLen,
            /* [in] */ Rotate eRotateDegree);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RGBConvert )( 
            ILeImageProcess __RPC_FAR * This,
            /* [in] */ RGBFormat eInRGBFormat,
            /* [in] */ RGBFormat eOutRGBFormat,
            /* [in] */ int nSrcWidth,
            /* [in] */ int nSrcHeight,
            /* [in] */ LPVOID lpSrc,
            /* [out][in] */ LPVOID lpDst);
        
        END_INTERFACE
    } ILeImageProcessVtbl;

    interface ILeImageProcess
    {
        CONST_VTBL struct ILeImageProcessVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeImageProcess_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeImageProcess_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeImageProcess_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeImageProcess_YCbCr422toYCbCr420Rotate_P3(This,nSrcWidth,nSrcHeight,lpSrc,lpDst,nRotateDegree)	\
    (This)->lpVtbl -> YCbCr422toYCbCr420Rotate_P3(This,nSrcWidth,nSrcHeight,lpSrc,lpDst,nRotateDegree)

#define ILeImageProcess_YCbCrRszRot_P3(This,eYCbCrFormat,nSrcWidth,nSrcHeight,nDstWidth,nDstHeight,lpSrc,lpDst,eInterpolate,eRotateDegree)	\
    (This)->lpVtbl -> YCbCrRszRot_P3(This,eYCbCrFormat,nSrcWidth,nSrcHeight,nDstWidth,nDstHeight,lpSrc,lpDst,eInterpolate,eRotateDegree)

#define ILeImageProcess_YCbCrtoRGB_P3C3(This,eYCbCrFormat,eRGBFormat,nWidth,nHeight,lpSrc,lpDst)	\
    (This)->lpVtbl -> YCbCrtoRGB_P3C3(This,eYCbCrFormat,eRGBFormat,nWidth,nHeight,lpSrc,lpDst)

#define ILeImageProcess_YCbCrtoRGBRszRot_P3C3(This,eYCbCrFormat,nSrcWidth,nSrcHeight,eRGBFormat,nDstWidth,nDstHeight,lpSrc,lpDst,eInterpolate,eRotateDegree)	\
    (This)->lpVtbl -> YCbCrtoRGBRszRot_P3C3(This,eYCbCrFormat,nSrcWidth,nSrcHeight,eRGBFormat,nDstWidth,nDstHeight,lpSrc,lpDst,eInterpolate,eRotateDegree)

#define ILeImageProcess_RGBRotate(This,nSrcWidth,nSrcHeight,lpSrc,lpDst,nPixelLen,eRotateDegree)	\
    (This)->lpVtbl -> RGBRotate(This,nSrcWidth,nSrcHeight,lpSrc,lpDst,nPixelLen,eRotateDegree)

#define ILeImageProcess_RGBConvert(This,eInRGBFormat,eOutRGBFormat,nSrcWidth,nSrcHeight,lpSrc,lpDst)	\
    (This)->lpVtbl -> RGBConvert(This,eInRGBFormat,eOutRGBFormat,nSrcWidth,nSrcHeight,lpSrc,lpDst)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeImageProcess_YCbCr422toYCbCr420Rotate_P3_Proxy( 
    ILeImageProcess __RPC_FAR * This,
    /* [in] */ int nSrcWidth,
    /* [in] */ int nSrcHeight,
    /* [in] */ BYTE __RPC_FAR *lpSrc,
    /* [out][in] */ BYTE __RPC_FAR *lpDst,
    /* [in] */ Rotate nRotateDegree);


void __RPC_STUB ILeImageProcess_YCbCr422toYCbCr420Rotate_P3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeImageProcess_YCbCrRszRot_P3_Proxy( 
    ILeImageProcess __RPC_FAR * This,
    /* [in] */ YCbCrFormat eYCbCrFormat,
    /* [in] */ int nSrcWidth,
    /* [in] */ int nSrcHeight,
    /* [in] */ int nDstWidth,
    /* [in] */ int nDstHeight,
    /* [in] */ BYTE __RPC_FAR *lpSrc,
    /* [out][in] */ BYTE __RPC_FAR *lpDst,
    /* [in] */ Interpolate eInterpolate,
    /* [in] */ Rotate eRotateDegree);


void __RPC_STUB ILeImageProcess_YCbCrRszRot_P3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeImageProcess_YCbCrtoRGB_P3C3_Proxy( 
    ILeImageProcess __RPC_FAR * This,
    /* [in] */ YCbCrFormat eYCbCrFormat,
    /* [in] */ RGBFormat eRGBFormat,
    /* [in] */ int nWidth,
    /* [in] */ int nHeight,
    /* [in] */ BYTE __RPC_FAR *lpSrc,
    /* [out][in] */ LPVOID lpDst);


void __RPC_STUB ILeImageProcess_YCbCrtoRGB_P3C3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeImageProcess_YCbCrtoRGBRszRot_P3C3_Proxy( 
    ILeImageProcess __RPC_FAR * This,
    /* [in] */ YCbCrFormat eYCbCrFormat,
    /* [in] */ int nSrcWidth,
    /* [in] */ int nSrcHeight,
    /* [in] */ RGBFormat eRGBFormat,
    /* [in] */ int nDstWidth,
    /* [in] */ int nDstHeight,
    /* [in] */ LPVOID lpSrc,
    /* [out][in] */ LPVOID lpDst,
    /* [in] */ Interpolate eInterpolate,
    /* [in] */ Rotate eRotateDegree);


void __RPC_STUB ILeImageProcess_YCbCrtoRGBRszRot_P3C3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeImageProcess_RGBRotate_Proxy( 
    ILeImageProcess __RPC_FAR * This,
    /* [in] */ int nSrcWidth,
    /* [in] */ int nSrcHeight,
    /* [in] */ LPVOID lpSrc,
    /* [out][in] */ LPVOID lpDst,
    /* [in] */ int nPixelLen,
    /* [in] */ Rotate eRotateDegree);


void __RPC_STUB ILeImageProcess_RGBRotate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeImageProcess_RGBConvert_Proxy( 
    ILeImageProcess __RPC_FAR * This,
    /* [in] */ RGBFormat eInRGBFormat,
    /* [in] */ RGBFormat eOutRGBFormat,
    /* [in] */ int nSrcWidth,
    /* [in] */ int nSrcHeight,
    /* [in] */ LPVOID lpSrc,
    /* [out][in] */ LPVOID lpDst);


void __RPC_STUB ILeImageProcess_RGBConvert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeImageProcess_INTERFACE_DEFINED__ */


#ifndef __ILeOverlay_INTERFACE_DEFINED__
#define __ILeOverlay_INTERFACE_DEFINED__

/* interface ILeOverlay */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeOverlay;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("893031C3-EF1B-4ee1-91FC-8607281744BA")
    ILeOverlay : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumOverlay( 
            /* [in] */ EnumOverlayCallback enumCallback,
            /* [in] */ LPVOID pContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOverlayTypeCount( 
            /* [out][in] */ UINT __RPC_FAR *uCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOverLayTypeCap( 
            /* [in] */ UINT uTypeIndex,
            /* [out][in] */ OverlayType __RPC_FAR *pOverlayType,
            /* [out][in] */ OverlayCap __RPC_FAR *pCap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOverlayCount( 
            /* [out][in] */ UINT __RPC_FAR *uCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOverLayCap( 
            /* [in] */ UINT uIndex,
            /* [out][in] */ OverlayType __RPC_FAR *pOverlayType,
            /* [out][in] */ OverlayCap __RPC_FAR *pCap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ OverlayInitPara stuOvlyPara) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnInit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Suspend( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangePosition( 
            /* [in] */ int nLeft,
            /* [in] */ int nTop,
            /* [in] */ int nWidth,
            /* [in] */ int nHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateData( 
            BOOL bUpdate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPreviewFrameRate( 
            /* [in] */ int nFrameRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPreviewFrameRate( 
            /* [out][in] */ int __RPC_FAR *pCurVal,
            /* [out][in] */ int __RPC_FAR *pMinVal,
            /* [out][in] */ int __RPC_FAR *pMaxVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFrameData( 
            /* [in] */ ColorFormat __RPC_FAR *pstuCF,
            /* [out][in] */ FrameData __RPC_FAR *pFrmData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Draw( 
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DrawString( 
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ FontInfo __RPC_FAR *pFont,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DrawAlpha( 
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [in] */ int nAlpha,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DrawAlphaEx( 
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ ColorKeyAlpha __RPC_FAR *pcolClrKeyAlpha,
            /* [in] */ int nCount,
            /* [in] */ int nAlpha,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DrawHalfAlpha( 
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DrawAlphaChannel( 
            /* [in] */ ResVariant stuResVarSrc,
            /* [in] */ ResVariant stuResVarDst,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DrawAdditive( 
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DrawSubtractive( 
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DrawQuarter( 
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ ResUUID resUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ ResUUID resUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShowAll( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Hide( 
            /* [in] */ ResUUID resUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HideAll( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HideAllExclude( 
            /* [in] */ ResUUID resUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PosChange( 
            /* [in] */ ResUUID resUID,
            /* [in] */ int left,
            /* [in] */ int top) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BlendChange( 
            /* [in] */ ResUUID resUD,
            /* [in] */ BlendAlgorithm bldAlgNew,
            /* [in] */ DWORD dwColorFilterNew,
            /* [in] */ int nAlphaNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChannelChange( 
            /* [in] */ ResUUID resUD,
            /* [in] */ ResVariant resChannelNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveToHead( 
            /* [in] */ ResUUID resUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveToTail( 
            /* [in] */ ResUUID resUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveToBefore( 
            /* [in] */ ResUUID resSrcUID,
            /* [in] */ ResUUID resDstUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveToAfter( 
            /* [in] */ ResUUID resSrcUID,
            /* [in] */ ResUUID resDstUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FillBkColor( 
            /* [in] */ BOOL bFill,
            /* [in] */ DWORD dwColor) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeOverlayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeOverlay __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeOverlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumOverlay )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ EnumOverlayCallback enumCallback,
            /* [in] */ LPVOID pContext);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOverlayTypeCount )( 
            ILeOverlay __RPC_FAR * This,
            /* [out][in] */ UINT __RPC_FAR *uCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOverLayTypeCap )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ UINT uTypeIndex,
            /* [out][in] */ OverlayType __RPC_FAR *pOverlayType,
            /* [out][in] */ OverlayCap __RPC_FAR *pCap);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOverlayCount )( 
            ILeOverlay __RPC_FAR * This,
            /* [out][in] */ UINT __RPC_FAR *uCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOverLayCap )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ UINT uIndex,
            /* [out][in] */ OverlayType __RPC_FAR *pOverlayType,
            /* [out][in] */ OverlayCap __RPC_FAR *pCap);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ OverlayInitPara stuOvlyPara);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnInit )( 
            ILeOverlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            ILeOverlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            ILeOverlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Suspend )( 
            ILeOverlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resume )( 
            ILeOverlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangePosition )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ int nLeft,
            /* [in] */ int nTop,
            /* [in] */ int nWidth,
            /* [in] */ int nHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateData )( 
            ILeOverlay __RPC_FAR * This,
            BOOL bUpdate);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetPreviewFrameRate )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ int nFrameRate);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPreviewFrameRate )( 
            ILeOverlay __RPC_FAR * This,
            /* [out][in] */ int __RPC_FAR *pCurVal,
            /* [out][in] */ int __RPC_FAR *pMinVal,
            /* [out][in] */ int __RPC_FAR *pMaxVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFrameData )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ColorFormat __RPC_FAR *pstuCF,
            /* [out][in] */ FrameData __RPC_FAR *pFrmData);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Draw )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawString )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ FontInfo __RPC_FAR *pFont,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawAlpha )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [in] */ int nAlpha,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawAlphaEx )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ ColorKeyAlpha __RPC_FAR *pcolClrKeyAlpha,
            /* [in] */ int nCount,
            /* [in] */ int nAlpha,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawHalfAlpha )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawAlphaChannel )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResVariant stuResVarSrc,
            /* [in] */ ResVariant stuResVarDst,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawAdditive )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawSubtractive )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawQuarter )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResVariant stuResVar,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ DWORD dwColorFilter,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Show )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowAll )( 
            ILeOverlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Hide )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HideAll )( 
            ILeOverlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HideAllExclude )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PosChange )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resUID,
            /* [in] */ int left,
            /* [in] */ int top);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BlendChange )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resUD,
            /* [in] */ BlendAlgorithm bldAlgNew,
            /* [in] */ DWORD dwColorFilterNew,
            /* [in] */ int nAlphaNew);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChannelChange )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resUD,
            /* [in] */ ResVariant resChannelNew);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveToHead )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveToTail )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveToBefore )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resSrcUID,
            /* [in] */ ResUUID resDstUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveToAfter )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ ResUUID resSrcUID,
            /* [in] */ ResUUID resDstUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FillBkColor )( 
            ILeOverlay __RPC_FAR * This,
            /* [in] */ BOOL bFill,
            /* [in] */ DWORD dwColor);
        
        END_INTERFACE
    } ILeOverlayVtbl;

    interface ILeOverlay
    {
        CONST_VTBL struct ILeOverlayVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeOverlay_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeOverlay_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeOverlay_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeOverlay_EnumOverlay(This,enumCallback,pContext)	\
    (This)->lpVtbl -> EnumOverlay(This,enumCallback,pContext)

#define ILeOverlay_GetOverlayTypeCount(This,uCount)	\
    (This)->lpVtbl -> GetOverlayTypeCount(This,uCount)

#define ILeOverlay_GetOverLayTypeCap(This,uTypeIndex,pOverlayType,pCap)	\
    (This)->lpVtbl -> GetOverLayTypeCap(This,uTypeIndex,pOverlayType,pCap)

#define ILeOverlay_GetOverlayCount(This,uCount)	\
    (This)->lpVtbl -> GetOverlayCount(This,uCount)

#define ILeOverlay_GetOverLayCap(This,uIndex,pOverlayType,pCap)	\
    (This)->lpVtbl -> GetOverLayCap(This,uIndex,pOverlayType,pCap)

#define ILeOverlay_Init(This,stuOvlyPara)	\
    (This)->lpVtbl -> Init(This,stuOvlyPara)

#define ILeOverlay_UnInit(This)	\
    (This)->lpVtbl -> UnInit(This)

#define ILeOverlay_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define ILeOverlay_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define ILeOverlay_Suspend(This)	\
    (This)->lpVtbl -> Suspend(This)

#define ILeOverlay_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#define ILeOverlay_ChangePosition(This,nLeft,nTop,nWidth,nHeight)	\
    (This)->lpVtbl -> ChangePosition(This,nLeft,nTop,nWidth,nHeight)

#define ILeOverlay_UpdateData(This,bUpdate)	\
    (This)->lpVtbl -> UpdateData(This,bUpdate)

#define ILeOverlay_SetPreviewFrameRate(This,nFrameRate)	\
    (This)->lpVtbl -> SetPreviewFrameRate(This,nFrameRate)

#define ILeOverlay_GetPreviewFrameRate(This,pCurVal,pMinVal,pMaxVal)	\
    (This)->lpVtbl -> GetPreviewFrameRate(This,pCurVal,pMinVal,pMaxVal)

#define ILeOverlay_GetFrameData(This,pstuCF,pFrmData)	\
    (This)->lpVtbl -> GetFrameData(This,pstuCF,pFrmData)

#define ILeOverlay_Draw(This,stuResVar,left,top,dwColorFilter,pResUID)	\
    (This)->lpVtbl -> Draw(This,stuResVar,left,top,dwColorFilter,pResUID)

#define ILeOverlay_DrawString(This,stuResVar,left,top,pFont,pResUID)	\
    (This)->lpVtbl -> DrawString(This,stuResVar,left,top,pFont,pResUID)

#define ILeOverlay_DrawAlpha(This,stuResVar,left,top,dwColorFilter,nAlpha,pResUID)	\
    (This)->lpVtbl -> DrawAlpha(This,stuResVar,left,top,dwColorFilter,nAlpha,pResUID)

#define ILeOverlay_DrawAlphaEx(This,stuResVar,left,top,pcolClrKeyAlpha,nCount,nAlpha,pResUID)	\
    (This)->lpVtbl -> DrawAlphaEx(This,stuResVar,left,top,pcolClrKeyAlpha,nCount,nAlpha,pResUID)

#define ILeOverlay_DrawHalfAlpha(This,stuResVar,left,top,dwColorFilter,pResUID)	\
    (This)->lpVtbl -> DrawHalfAlpha(This,stuResVar,left,top,dwColorFilter,pResUID)

#define ILeOverlay_DrawAlphaChannel(This,stuResVarSrc,stuResVarDst,left,top,pResUID)	\
    (This)->lpVtbl -> DrawAlphaChannel(This,stuResVarSrc,stuResVarDst,left,top,pResUID)

#define ILeOverlay_DrawAdditive(This,stuResVar,left,top,dwColorFilter,pResUID)	\
    (This)->lpVtbl -> DrawAdditive(This,stuResVar,left,top,dwColorFilter,pResUID)

#define ILeOverlay_DrawSubtractive(This,stuResVar,left,top,dwColorFilter,pResUID)	\
    (This)->lpVtbl -> DrawSubtractive(This,stuResVar,left,top,dwColorFilter,pResUID)

#define ILeOverlay_DrawQuarter(This,stuResVar,left,top,dwColorFilter,pResUID)	\
    (This)->lpVtbl -> DrawQuarter(This,stuResVar,left,top,dwColorFilter,pResUID)

#define ILeOverlay_Remove(This,resUID)	\
    (This)->lpVtbl -> Remove(This,resUID)

#define ILeOverlay_Show(This,resUID)	\
    (This)->lpVtbl -> Show(This,resUID)

#define ILeOverlay_ShowAll(This)	\
    (This)->lpVtbl -> ShowAll(This)

#define ILeOverlay_Hide(This,resUID)	\
    (This)->lpVtbl -> Hide(This,resUID)

#define ILeOverlay_HideAll(This)	\
    (This)->lpVtbl -> HideAll(This)

#define ILeOverlay_HideAllExclude(This,resUID)	\
    (This)->lpVtbl -> HideAllExclude(This,resUID)

#define ILeOverlay_PosChange(This,resUID,left,top)	\
    (This)->lpVtbl -> PosChange(This,resUID,left,top)

#define ILeOverlay_BlendChange(This,resUD,bldAlgNew,dwColorFilterNew,nAlphaNew)	\
    (This)->lpVtbl -> BlendChange(This,resUD,bldAlgNew,dwColorFilterNew,nAlphaNew)

#define ILeOverlay_ChannelChange(This,resUD,resChannelNew)	\
    (This)->lpVtbl -> ChannelChange(This,resUD,resChannelNew)

#define ILeOverlay_MoveToHead(This,resUID)	\
    (This)->lpVtbl -> MoveToHead(This,resUID)

#define ILeOverlay_MoveToTail(This,resUID)	\
    (This)->lpVtbl -> MoveToTail(This,resUID)

#define ILeOverlay_MoveToBefore(This,resSrcUID,resDstUID)	\
    (This)->lpVtbl -> MoveToBefore(This,resSrcUID,resDstUID)

#define ILeOverlay_MoveToAfter(This,resSrcUID,resDstUID)	\
    (This)->lpVtbl -> MoveToAfter(This,resSrcUID,resDstUID)

#define ILeOverlay_FillBkColor(This,bFill,dwColor)	\
    (This)->lpVtbl -> FillBkColor(This,bFill,dwColor)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeOverlay_EnumOverlay_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ EnumOverlayCallback enumCallback,
    /* [in] */ LPVOID pContext);


void __RPC_STUB ILeOverlay_EnumOverlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_GetOverlayTypeCount_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [out][in] */ UINT __RPC_FAR *uCount);


void __RPC_STUB ILeOverlay_GetOverlayTypeCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_GetOverLayTypeCap_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ UINT uTypeIndex,
    /* [out][in] */ OverlayType __RPC_FAR *pOverlayType,
    /* [out][in] */ OverlayCap __RPC_FAR *pCap);


void __RPC_STUB ILeOverlay_GetOverLayTypeCap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_GetOverlayCount_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [out][in] */ UINT __RPC_FAR *uCount);


void __RPC_STUB ILeOverlay_GetOverlayCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_GetOverLayCap_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ UINT uIndex,
    /* [out][in] */ OverlayType __RPC_FAR *pOverlayType,
    /* [out][in] */ OverlayCap __RPC_FAR *pCap);


void __RPC_STUB ILeOverlay_GetOverLayCap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_Init_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ OverlayInitPara stuOvlyPara);


void __RPC_STUB ILeOverlay_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_UnInit_Proxy( 
    ILeOverlay __RPC_FAR * This);


void __RPC_STUB ILeOverlay_UnInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_Start_Proxy( 
    ILeOverlay __RPC_FAR * This);


void __RPC_STUB ILeOverlay_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_Stop_Proxy( 
    ILeOverlay __RPC_FAR * This);


void __RPC_STUB ILeOverlay_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_Suspend_Proxy( 
    ILeOverlay __RPC_FAR * This);


void __RPC_STUB ILeOverlay_Suspend_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_Resume_Proxy( 
    ILeOverlay __RPC_FAR * This);


void __RPC_STUB ILeOverlay_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_ChangePosition_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ int nLeft,
    /* [in] */ int nTop,
    /* [in] */ int nWidth,
    /* [in] */ int nHeight);


void __RPC_STUB ILeOverlay_ChangePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_UpdateData_Proxy( 
    ILeOverlay __RPC_FAR * This,
    BOOL bUpdate);


void __RPC_STUB ILeOverlay_UpdateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_SetPreviewFrameRate_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ int nFrameRate);


void __RPC_STUB ILeOverlay_SetPreviewFrameRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_GetPreviewFrameRate_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [out][in] */ int __RPC_FAR *pCurVal,
    /* [out][in] */ int __RPC_FAR *pMinVal,
    /* [out][in] */ int __RPC_FAR *pMaxVal);


void __RPC_STUB ILeOverlay_GetPreviewFrameRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_GetFrameData_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ColorFormat __RPC_FAR *pstuCF,
    /* [out][in] */ FrameData __RPC_FAR *pFrmData);


void __RPC_STUB ILeOverlay_GetFrameData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_Draw_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResVariant stuResVar,
    /* [in] */ int left,
    /* [in] */ int top,
    /* [in] */ DWORD dwColorFilter,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB ILeOverlay_Draw_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_DrawString_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResVariant stuResVar,
    /* [in] */ int left,
    /* [in] */ int top,
    /* [in] */ FontInfo __RPC_FAR *pFont,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB ILeOverlay_DrawString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_DrawAlpha_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResVariant stuResVar,
    /* [in] */ int left,
    /* [in] */ int top,
    /* [in] */ DWORD dwColorFilter,
    /* [in] */ int nAlpha,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB ILeOverlay_DrawAlpha_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_DrawAlphaEx_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResVariant stuResVar,
    /* [in] */ int left,
    /* [in] */ int top,
    /* [in] */ ColorKeyAlpha __RPC_FAR *pcolClrKeyAlpha,
    /* [in] */ int nCount,
    /* [in] */ int nAlpha,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB ILeOverlay_DrawAlphaEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_DrawHalfAlpha_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResVariant stuResVar,
    /* [in] */ int left,
    /* [in] */ int top,
    /* [in] */ DWORD dwColorFilter,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB ILeOverlay_DrawHalfAlpha_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_DrawAlphaChannel_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResVariant stuResVarSrc,
    /* [in] */ ResVariant stuResVarDst,
    /* [in] */ int left,
    /* [in] */ int top,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB ILeOverlay_DrawAlphaChannel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_DrawAdditive_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResVariant stuResVar,
    /* [in] */ int left,
    /* [in] */ int top,
    /* [in] */ DWORD dwColorFilter,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB ILeOverlay_DrawAdditive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_DrawSubtractive_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResVariant stuResVar,
    /* [in] */ int left,
    /* [in] */ int top,
    /* [in] */ DWORD dwColorFilter,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB ILeOverlay_DrawSubtractive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_DrawQuarter_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResVariant stuResVar,
    /* [in] */ int left,
    /* [in] */ int top,
    /* [in] */ DWORD dwColorFilter,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB ILeOverlay_DrawQuarter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_Remove_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resUID);


void __RPC_STUB ILeOverlay_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_Show_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resUID);


void __RPC_STUB ILeOverlay_Show_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_ShowAll_Proxy( 
    ILeOverlay __RPC_FAR * This);


void __RPC_STUB ILeOverlay_ShowAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_Hide_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resUID);


void __RPC_STUB ILeOverlay_Hide_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_HideAll_Proxy( 
    ILeOverlay __RPC_FAR * This);


void __RPC_STUB ILeOverlay_HideAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_HideAllExclude_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resUID);


void __RPC_STUB ILeOverlay_HideAllExclude_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_PosChange_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resUID,
    /* [in] */ int left,
    /* [in] */ int top);


void __RPC_STUB ILeOverlay_PosChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_BlendChange_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resUD,
    /* [in] */ BlendAlgorithm bldAlgNew,
    /* [in] */ DWORD dwColorFilterNew,
    /* [in] */ int nAlphaNew);


void __RPC_STUB ILeOverlay_BlendChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_ChannelChange_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resUD,
    /* [in] */ ResVariant resChannelNew);


void __RPC_STUB ILeOverlay_ChannelChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_MoveToHead_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resUID);


void __RPC_STUB ILeOverlay_MoveToHead_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_MoveToTail_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resUID);


void __RPC_STUB ILeOverlay_MoveToTail_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_MoveToBefore_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resSrcUID,
    /* [in] */ ResUUID resDstUID);


void __RPC_STUB ILeOverlay_MoveToBefore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_MoveToAfter_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ ResUUID resSrcUID,
    /* [in] */ ResUUID resDstUID);


void __RPC_STUB ILeOverlay_MoveToAfter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeOverlay_FillBkColor_Proxy( 
    ILeOverlay __RPC_FAR * This,
    /* [in] */ BOOL bFill,
    /* [in] */ DWORD dwColor);


void __RPC_STUB ILeOverlay_FillBkColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeOverlay_INTERFACE_DEFINED__ */


#ifndef __IOverlayGraph_INTERFACE_DEFINED__
#define __IOverlayGraph_INTERFACE_DEFINED__

/* interface IOverlayGraph */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_IOverlayGraph;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B0CBB039-4083-4bb5-8A84-35E8C2289C20")
    IOverlayGraph : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Line( 
            /* [in] */ LinePara stuLinePara,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Rectangle( 
            /* [in] */ RectPara stuRcPara,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Ellipse( 
            /* [in] */ EllipsePara stuEllPara,
            /* [out] */ ResUUID __RPC_FAR *pResUID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOverlayGraphVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOverlayGraph __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOverlayGraph __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOverlayGraph __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Line )( 
            IOverlayGraph __RPC_FAR * This,
            /* [in] */ LinePara stuLinePara,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Rectangle )( 
            IOverlayGraph __RPC_FAR * This,
            /* [in] */ RectPara stuRcPara,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Ellipse )( 
            IOverlayGraph __RPC_FAR * This,
            /* [in] */ EllipsePara stuEllPara,
            /* [out] */ ResUUID __RPC_FAR *pResUID);
        
        END_INTERFACE
    } IOverlayGraphVtbl;

    interface IOverlayGraph
    {
        CONST_VTBL struct IOverlayGraphVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOverlayGraph_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOverlayGraph_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOverlayGraph_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOverlayGraph_Line(This,stuLinePara,pResUID)	\
    (This)->lpVtbl -> Line(This,stuLinePara,pResUID)

#define IOverlayGraph_Rectangle(This,stuRcPara,pResUID)	\
    (This)->lpVtbl -> Rectangle(This,stuRcPara,pResUID)

#define IOverlayGraph_Ellipse(This,stuEllPara,pResUID)	\
    (This)->lpVtbl -> Ellipse(This,stuEllPara,pResUID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOverlayGraph_Line_Proxy( 
    IOverlayGraph __RPC_FAR * This,
    /* [in] */ LinePara stuLinePara,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB IOverlayGraph_Line_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOverlayGraph_Rectangle_Proxy( 
    IOverlayGraph __RPC_FAR * This,
    /* [in] */ RectPara stuRcPara,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB IOverlayGraph_Rectangle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOverlayGraph_Ellipse_Proxy( 
    IOverlayGraph __RPC_FAR * This,
    /* [in] */ EllipsePara stuEllPara,
    /* [out] */ ResUUID __RPC_FAR *pResUID);


void __RPC_STUB IOverlayGraph_Ellipse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOverlayGraph_INTERFACE_DEFINED__ */


#ifndef __ILeCamEvent_INTERFACE_DEFINED__
#define __ILeCamEvent_INTERFACE_DEFINED__

/* interface ILeCamEvent */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeCamEvent;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B806276B-4C21-4258-A3FE-772C45B4C742")
    ILeCamEvent : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EventSubscribe( 
            /* [in] */ LeCamEvent eCamEvent,
            /* [in] */ LeCamEventCallback pEventCallback,
            /* [in] */ LPVOID pContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EventUnSubscribe( 
            /* [in] */ LeCamEvent eCamEvent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EventAttachWindow( 
            /* [in] */ LeCamEvent eCamEvent,
            /* [in] */ HWND hWndRecv,
            /* [in] */ UINT uMsg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EventDetachWindow( 
            /* [in] */ LeCamEvent eCamEvent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeCamEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeCamEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeCamEvent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeCamEvent __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EventSubscribe )( 
            ILeCamEvent __RPC_FAR * This,
            /* [in] */ LeCamEvent eCamEvent,
            /* [in] */ LeCamEventCallback pEventCallback,
            /* [in] */ LPVOID pContext);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EventUnSubscribe )( 
            ILeCamEvent __RPC_FAR * This,
            /* [in] */ LeCamEvent eCamEvent);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EventAttachWindow )( 
            ILeCamEvent __RPC_FAR * This,
            /* [in] */ LeCamEvent eCamEvent,
            /* [in] */ HWND hWndRecv,
            /* [in] */ UINT uMsg);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EventDetachWindow )( 
            ILeCamEvent __RPC_FAR * This,
            /* [in] */ LeCamEvent eCamEvent);
        
        END_INTERFACE
    } ILeCamEventVtbl;

    interface ILeCamEvent
    {
        CONST_VTBL struct ILeCamEventVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeCamEvent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeCamEvent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeCamEvent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeCamEvent_EventSubscribe(This,eCamEvent,pEventCallback,pContext)	\
    (This)->lpVtbl -> EventSubscribe(This,eCamEvent,pEventCallback,pContext)

#define ILeCamEvent_EventUnSubscribe(This,eCamEvent)	\
    (This)->lpVtbl -> EventUnSubscribe(This,eCamEvent)

#define ILeCamEvent_EventAttachWindow(This,eCamEvent,hWndRecv,uMsg)	\
    (This)->lpVtbl -> EventAttachWindow(This,eCamEvent,hWndRecv,uMsg)

#define ILeCamEvent_EventDetachWindow(This,eCamEvent)	\
    (This)->lpVtbl -> EventDetachWindow(This,eCamEvent)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeCamEvent_EventSubscribe_Proxy( 
    ILeCamEvent __RPC_FAR * This,
    /* [in] */ LeCamEvent eCamEvent,
    /* [in] */ LeCamEventCallback pEventCallback,
    /* [in] */ LPVOID pContext);


void __RPC_STUB ILeCamEvent_EventSubscribe_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamEvent_EventUnSubscribe_Proxy( 
    ILeCamEvent __RPC_FAR * This,
    /* [in] */ LeCamEvent eCamEvent);


void __RPC_STUB ILeCamEvent_EventUnSubscribe_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamEvent_EventAttachWindow_Proxy( 
    ILeCamEvent __RPC_FAR * This,
    /* [in] */ LeCamEvent eCamEvent,
    /* [in] */ HWND hWndRecv,
    /* [in] */ UINT uMsg);


void __RPC_STUB ILeCamEvent_EventAttachWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeCamEvent_EventDetachWindow_Proxy( 
    ILeCamEvent __RPC_FAR * This,
    /* [in] */ LeCamEvent eCamEvent);


void __RPC_STUB ILeCamEvent_EventDetachWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeCamEvent_INTERFACE_DEFINED__ */


#ifndef __ILeDShowPlayer_INTERFACE_DEFINED__
#define __ILeDShowPlayer_INTERFACE_DEFINED__

/* interface ILeDShowPlayer */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeDShowPlayer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("464EEAF5-2222-4baa-AE63-4E168592B57B")
    ILeDShowPlayer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ HWND hParent,
            /* [in] */ BOOL bShow,
            /* [in] */ RECT __RPC_FAR *pRect) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnInit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFile( 
            /* [in] */ BSTR pwzFile) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFile( 
            /* [out][in] */ BSTR pwzFile) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVolume( 
            /* [in] */ int nVolume) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVolume( 
            /* [out][in] */ int __RPC_FAR *pVolume) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLoopFlag( 
            /* [in] */ BOOL bLoop) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLoopFlag( 
            /* [out][in] */ BOOL __RPC_FAR *pbLoop) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Seek( 
            /* [in] */ int nPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PlaySectionByPercent( 
            /* [in] */ int nStartPer,
            /* [in] */ int nEndPer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PlaySectionByPos( 
            /* [in] */ int nStartPos,
            /* [in] */ int nStopPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PlaySectionByStrPos( 
            /* [in] */ BSTR pwzStartPos,
            /* [in] */ BSTR pwzStopPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurInfo( 
            /* [out][in] */ PlayInfo __RPC_FAR *pPlayInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ WindowState eWndState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangeWindowPos( 
            /* [in] */ RECT __RPC_FAR *pRect) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeDShowPlayerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeDShowPlayer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeDShowPlayer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ HWND hParent,
            /* [in] */ BOOL bShow,
            /* [in] */ RECT __RPC_FAR *pRect);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnInit )( 
            ILeDShowPlayer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFile )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ BSTR pwzFile);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFile )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [out][in] */ BSTR pwzFile);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetVolume )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ int nVolume);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVolume )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [out][in] */ int __RPC_FAR *pVolume);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetLoopFlag )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ BOOL bLoop);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLoopFlag )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [out][in] */ BOOL __RPC_FAR *pbLoop);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            ILeDShowPlayer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            ILeDShowPlayer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            ILeDShowPlayer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Seek )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ int nPos);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlaySectionByPercent )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ int nStartPer,
            /* [in] */ int nEndPer);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlaySectionByPos )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ int nStartPos,
            /* [in] */ int nStopPos);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlaySectionByStrPos )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ BSTR pwzStartPos,
            /* [in] */ BSTR pwzStopPos);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurInfo )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [out][in] */ PlayInfo __RPC_FAR *pPlayInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Show )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ WindowState eWndState);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeWindowPos )( 
            ILeDShowPlayer __RPC_FAR * This,
            /* [in] */ RECT __RPC_FAR *pRect);
        
        END_INTERFACE
    } ILeDShowPlayerVtbl;

    interface ILeDShowPlayer
    {
        CONST_VTBL struct ILeDShowPlayerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeDShowPlayer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeDShowPlayer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeDShowPlayer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeDShowPlayer_Init(This,hParent,bShow,pRect)	\
    (This)->lpVtbl -> Init(This,hParent,bShow,pRect)

#define ILeDShowPlayer_UnInit(This)	\
    (This)->lpVtbl -> UnInit(This)

#define ILeDShowPlayer_SetFile(This,pwzFile)	\
    (This)->lpVtbl -> SetFile(This,pwzFile)

#define ILeDShowPlayer_GetFile(This,pwzFile)	\
    (This)->lpVtbl -> GetFile(This,pwzFile)

#define ILeDShowPlayer_SetVolume(This,nVolume)	\
    (This)->lpVtbl -> SetVolume(This,nVolume)

#define ILeDShowPlayer_GetVolume(This,pVolume)	\
    (This)->lpVtbl -> GetVolume(This,pVolume)

#define ILeDShowPlayer_SetLoopFlag(This,bLoop)	\
    (This)->lpVtbl -> SetLoopFlag(This,bLoop)

#define ILeDShowPlayer_GetLoopFlag(This,pbLoop)	\
    (This)->lpVtbl -> GetLoopFlag(This,pbLoop)

#define ILeDShowPlayer_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define ILeDShowPlayer_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define ILeDShowPlayer_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define ILeDShowPlayer_Seek(This,nPos)	\
    (This)->lpVtbl -> Seek(This,nPos)

#define ILeDShowPlayer_PlaySectionByPercent(This,nStartPer,nEndPer)	\
    (This)->lpVtbl -> PlaySectionByPercent(This,nStartPer,nEndPer)

#define ILeDShowPlayer_PlaySectionByPos(This,nStartPos,nStopPos)	\
    (This)->lpVtbl -> PlaySectionByPos(This,nStartPos,nStopPos)

#define ILeDShowPlayer_PlaySectionByStrPos(This,pwzStartPos,pwzStopPos)	\
    (This)->lpVtbl -> PlaySectionByStrPos(This,pwzStartPos,pwzStopPos)

#define ILeDShowPlayer_GetCurInfo(This,pPlayInfo)	\
    (This)->lpVtbl -> GetCurInfo(This,pPlayInfo)

#define ILeDShowPlayer_Show(This,eWndState)	\
    (This)->lpVtbl -> Show(This,eWndState)

#define ILeDShowPlayer_ChangeWindowPos(This,pRect)	\
    (This)->lpVtbl -> ChangeWindowPos(This,pRect)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeDShowPlayer_Init_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ HWND hParent,
    /* [in] */ BOOL bShow,
    /* [in] */ RECT __RPC_FAR *pRect);


void __RPC_STUB ILeDShowPlayer_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_UnInit_Proxy( 
    ILeDShowPlayer __RPC_FAR * This);


void __RPC_STUB ILeDShowPlayer_UnInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_SetFile_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ BSTR pwzFile);


void __RPC_STUB ILeDShowPlayer_SetFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_GetFile_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [out][in] */ BSTR pwzFile);


void __RPC_STUB ILeDShowPlayer_GetFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_SetVolume_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ int nVolume);


void __RPC_STUB ILeDShowPlayer_SetVolume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_GetVolume_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [out][in] */ int __RPC_FAR *pVolume);


void __RPC_STUB ILeDShowPlayer_GetVolume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_SetLoopFlag_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ BOOL bLoop);


void __RPC_STUB ILeDShowPlayer_SetLoopFlag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_GetLoopFlag_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [out][in] */ BOOL __RPC_FAR *pbLoop);


void __RPC_STUB ILeDShowPlayer_GetLoopFlag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_Start_Proxy( 
    ILeDShowPlayer __RPC_FAR * This);


void __RPC_STUB ILeDShowPlayer_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_Pause_Proxy( 
    ILeDShowPlayer __RPC_FAR * This);


void __RPC_STUB ILeDShowPlayer_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_Stop_Proxy( 
    ILeDShowPlayer __RPC_FAR * This);


void __RPC_STUB ILeDShowPlayer_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_Seek_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ int nPos);


void __RPC_STUB ILeDShowPlayer_Seek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_PlaySectionByPercent_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ int nStartPer,
    /* [in] */ int nEndPer);


void __RPC_STUB ILeDShowPlayer_PlaySectionByPercent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_PlaySectionByPos_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ int nStartPos,
    /* [in] */ int nStopPos);


void __RPC_STUB ILeDShowPlayer_PlaySectionByPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_PlaySectionByStrPos_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ BSTR pwzStartPos,
    /* [in] */ BSTR pwzStopPos);


void __RPC_STUB ILeDShowPlayer_PlaySectionByStrPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_GetCurInfo_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [out][in] */ PlayInfo __RPC_FAR *pPlayInfo);


void __RPC_STUB ILeDShowPlayer_GetCurInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_Show_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ WindowState eWndState);


void __RPC_STUB ILeDShowPlayer_Show_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDShowPlayer_ChangeWindowPos_Proxy( 
    ILeDShowPlayer __RPC_FAR * This,
    /* [in] */ RECT __RPC_FAR *pRect);


void __RPC_STUB ILeDShowPlayer_ChangeWindowPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeDShowPlayer_INTERFACE_DEFINED__ */


#ifndef __ILeDSLLog_INTERFACE_DEFINED__
#define __ILeDSLLog_INTERFACE_DEFINED__

/* interface ILeDSLLog */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ILeDSLLog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8C099B71-4EE8-4f97-BDB4-863984D6DAEB")
    ILeDSLLog : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetLastError( 
            /* [retval][out] */ BSTR __RPC_FAR *pbsError) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLogFile( 
            /* [in] */ BSTR bsFullFileName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILeDSLLogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILeDSLLog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILeDSLLog __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILeDSLLog __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLastError )( 
            ILeDSLLog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbsError);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetLogFile )( 
            ILeDSLLog __RPC_FAR * This,
            /* [in] */ BSTR bsFullFileName);
        
        END_INTERFACE
    } ILeDSLLogVtbl;

    interface ILeDSLLog
    {
        CONST_VTBL struct ILeDSLLogVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeDSLLog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeDSLLog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeDSLLog_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeDSLLog_GetLastError(This,pbsError)	\
    (This)->lpVtbl -> GetLastError(This,pbsError)

#define ILeDSLLog_SetLogFile(This,bsFullFileName)	\
    (This)->lpVtbl -> SetLogFile(This,bsFullFileName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeDSLLog_GetLastError_Proxy( 
    ILeDSLLog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbsError);


void __RPC_STUB ILeDSLLog_GetLastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeDSLLog_SetLogFile_Proxy( 
    ILeDSLLog __RPC_FAR * This,
    /* [in] */ BSTR bsFullFileName);


void __RPC_STUB ILeDSLLog_SetLogFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeDSLLog_INTERFACE_DEFINED__ */



#ifndef __LeCameraTypeLib_LIBRARY_DEFINED__
#define __LeCameraTypeLib_LIBRARY_DEFINED__

/* library LeCameraTypeLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_LeCameraTypeLib;

EXTERN_C const CLSID CLSID_LeCamera;

#ifdef __cplusplus

class DECLSPEC_UUID("860E91C0-C052-4462-BC15-89C09B9BDAB5")
LeCamera;
#endif

EXTERN_C const CLSID CLSID_LeOverlay;

#ifdef __cplusplus

class DECLSPEC_UUID("0D76B3BB-83ED-492d-AE4B-A2A64CB85FCB")
LeOverlay;
#endif

EXTERN_C const CLSID CLSID_LeImageProcess;

#ifdef __cplusplus

class DECLSPEC_UUID("A3359BD7-BD74-4363-9722-C72A8D43A81A")
LeImageProcess;
#endif

EXTERN_C const CLSID CLSID_LeDShowPlayer;

#ifdef __cplusplus

class DECLSPEC_UUID("C5EE4DFE-2E84-4411-AAEF-4023C37CF556")
LeDShowPlayer;
#endif

EXTERN_C const CLSID CLSID_LeDSLLog;

#ifdef __cplusplus

class DECLSPEC_UUID("F9B4FF4A-276A-44be-B1EC-FF3DB0BE5892")
LeDSLLog;
#endif
#endif /* __LeCameraTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
