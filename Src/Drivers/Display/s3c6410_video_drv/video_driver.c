#include <bsp.h>
#include <DrvLib.h>
#include "SVEngine.h"

BOOL
WINAPI
DllEntry(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		//DEBUGREGISTER(hinstDll);
		//DisableThreadLibraryCalls ((HMODULE)hinstDll);
		VDE_MSG((_T("[VDE] DllEntry() : Process Attach\r\n")));
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		VDE_MSG((_T("[VDE] DllEntry() : Process Detach\r\n")));
	}

	return TRUE;
}

DWORD
VDE_Init(
	LPCTSTR pContext
	)
{
	VDE_MSG((_T("[VDE] ++VDE_Init(0x%08x)\r\n"), pContext));

	if (SVE_initialize_video_engine() == FALSE)
	{
		VDE_ERR((_T("[VDE:ERR] VDE_Init() : SVE_initialize_video_engine() Failed \n\r")));
		goto CleanUp;
	}

	VDE_MSG((_T("[VDE] --VDE_Init()\r\n")));

	return SVE_get_driver_signature();	// hDeviceContext

CleanUp:

	VDE_ERR((_T("[VDE:ERR] --VDE_Init() : Failed\r\n")));

	return 0;
}

BOOL
VDE_Deinit(
	DWORD hDeviceContext
	)
{
	BOOL bRet;

	VDE_MSG((_T("[VDE] ++VDE_Deinit(0x%08x)\r\n"), hDeviceContext));

	if (hDeviceContext != SVE_get_driver_signature())
	{
		VDE_ERR((_T("[VDE:ERR] VDE_Deinit() : Invalid Driver Handle[0x%08x]\r\n"), hDeviceContext));
		return FALSE;
	}

	bRet = SVE_deinitialize_video_engine();

	if (!bRet)
	{
		VDE_ERR((_T("[VDE:ERR] VDE_Deinit() : SVE_deinitialize_video_engine() Failed\n\r")));
	}

	VDE_MSG((_T("[VDE] --VDE_Deinit()\r\n")));

	return (bRet);
}

DWORD
VDE_Open(
	DWORD hDeviceContext,
	DWORD AccessCode,
	DWORD ShareMode
	)
{
	DWORD dwOpenContext;

	VDE_MSG((_T("[VDE] ++VDE_Open(0x%08x, 0x%08x, 0x%08x)\r\n"), hDeviceContext, AccessCode, ShareMode));

	if (hDeviceContext != SVE_get_driver_signature())
	{
		VDE_ERR((_T("[VDE:ERR] VDE_Open() : Invalid Driver Handle[0x%08x]\r\n"), hDeviceContext));
		goto CleanUp;
	}

	dwOpenContext = SVE_add_open_context();
	if (dwOpenContext == 0)
	{
		VDE_ERR((_T("[VDE:ERR] VDE_Open() : Allocating Open Context Failed\r\n")));
		goto CleanUp;
	}

	VDE_MSG((_T("[VDE] --VDE_Open()\r\n")));

	return dwOpenContext;

CleanUp:

	VDE_ERR((_T("[VDE:ERR] --VDE_Init() : Failed\r\n")));

	return 0;
}

BOOL
VDE_Close(
	DWORD hOpenContext
	)
{
	BOOL bRet;

	VDE_MSG((_T("[VDE] ++VDE_Close(0x%08x)\r\n"), hOpenContext));

	bRet = SVE_remove_open_context(hOpenContext);
	if (!bRet)
	{
		VDE_ERR((_T("[VDE:ERR] VDE_Close() : Invalid Open Context !!!\r\n")));
	}

	VDE_MSG((_T("[VDE] --VDE_Close()\r\n")));

	return bRet;
}

DWORD
VDE_Read(
	DWORD hOpenContext,
	LPVOID pBuffer,
	DWORD Count
	)
{
	VDE_MSG((_T("[VDE] VDE_Read(0x%08x, 0x%08x, 0x%08x)\r\n"), hOpenContext, pBuffer, Count));

	return (0);	// End of File
}

DWORD
VDE_Write(
	DWORD hOpenContext,
	LPCVOID pBuffer,
	DWORD Count
	)
{
	VDE_MSG((_T("[VDE] VDE_Write(0x%08x, 0x%08x, 0x%08x)\r\n"), hOpenContext, pBuffer, Count));

	return (0);	// Number of Byte
}

DWORD
VDE_Seek(
	DWORD hOpenContext,
	long Amount,
	WORD Type
	)
{
	VDE_MSG((_T("[VDE] VDE_Seek(0x%08x, 0x%08x, 0x%08x)\r\n"), hOpenContext, Amount, Type));

	return (DWORD)-1;	// Failure
}

BOOL
VDE_IOControl(
	DWORD hOpenContext,
	DWORD dwCode,
	PBYTE pBufIn,
	DWORD dwLenIn,
	PBYTE pBufOut,
	DWORD dwLenOut,
	PDWORD pdwActualOut
	)
{
	SVEngineContext *pCtxt;
	BOOL bRet = TRUE;

	pCtxt = SVE_get_context();

	//VDE_MSG((_T("[VDE] VDE_IOControl(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\r\n"),
	//			hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));

	EnterCriticalSection(&pCtxt->csProc);

	switch(dwCode)
	{
	case IOCTL_POWER_CAPABILITIES:
	case IOCTL_POWER_GET:
	case IOCTL_POWER_QUERY:
	case IOCTL_POWER_SET:
	case IOCTL_REGISTER_POWER_RELATIONSHIP:
		break;

	case IOCTL_SVE_RSC_REQUEST_FIMD_INTERFACE:
	case IOCTL_SVE_RSC_RELEASE_FIMD_INTERFACE:
	case IOCTL_SVE_RSC_REQUEST_FIMD_WIN0:
	case IOCTL_SVE_RSC_RELEASE_FIMD_WIN0:
	case IOCTL_SVE_RSC_REQUEST_FIMD_WIN1:
	case IOCTL_SVE_RSC_RELEASE_FIMD_WIN1:
	case IOCTL_SVE_RSC_REQUEST_FIMD_WIN2:
	case IOCTL_SVE_RSC_RELEASE_FIMD_WIN2:
	case IOCTL_SVE_RSC_REQUEST_FIMD_WIN3:
	case IOCTL_SVE_RSC_RELEASE_FIMD_WIN3:
	case IOCTL_SVE_RSC_REQUEST_FIMD_WIN4:
	case IOCTL_SVE_RSC_RELEASE_FIMD_WIN4:
	case IOCTL_SVE_RSC_REQUEST_POST:
	case IOCTL_SVE_RSC_RELEASE_POST:
	case IOCTL_SVE_RSC_REQUEST_ROTATOR:
	case IOCTL_SVE_RSC_RELEASE_ROTATOR:
	case IOCTL_SVE_RSC_REQUEST_TVSCALER_TVENCODER:
	case IOCTL_SVE_RSC_RELEASE_TVSCALER_TVENCODER:
		bRet = SVE_Resource_API_Proc(hOpenContext, SVE_get_api_function_code(dwCode), pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
		break;

	case IOCTL_SVE_FIMD_SET_INTERFACE_PARAM:
	case IOCTL_SVE_FIMD_SET_OUTPUT_RGBIF:
	case IOCTL_SVE_FIMD_SET_OUTPUT_TV:
	case IOCTL_SVE_FIMD_SET_OUTPUT_ENABLE:
	case IOCTL_SVE_FIMD_SET_OUTPUT_DISABLE:
	case IOCTL_SVE_FIMD_SET_WINDOW_MODE:
	case IOCTL_SVE_FIMD_SET_WINDOW_POSITION:
	case IOCTL_SVE_FIMD_SET_WINDOW_FRAMEBUFFER:
	case IOCTL_SVE_FIMD_SET_WINDOW_COLORMAP:
	case IOCTL_SVE_FIMD_SET_WINDOW_ENABLE:
	case IOCTL_SVE_FIMD_SET_WINDOW_DISABLE:
	case IOCTL_SVE_FIMD_SET_WINDOW_BLEND_DISABLE:
	case IOCTL_SVE_FIMD_SET_WINDOW_BLEND_COLORKEY:
	case IOCTL_SVE_FIMD_SET_WINDOW_BLEND_ALPHA:
	case IOCTL_SVE_FIMD_WAIT_FRAME_INTERRUPT:
	case IOCTL_SVE_FIMD_GET_OUTPUT_STATUS:
	case IOCTL_SVE_FIMD_GET_WINDOW_STATUS:
		bRet = SVE_DispCon_API_Proc(hOpenContext, SVE_get_api_function_code(dwCode), pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
		break;

	case IOCTL_SVE_POST_SET_PROCESSING_PARAM:
	case IOCTL_SVE_POST_SET_SOURCE_BUFFER:
	case IOCTL_SVE_POST_SET_NEXT_SOURCE_BUFFER:
	case IOCTL_SVE_POST_SET_DESTINATION_BUFFER:
	case IOCTL_SVE_POST_SET_NEXT_DESTINATION_BUFFER:
	case IOCTL_SVE_POST_SET_PROCESSING_START:
	case IOCTL_SVE_POST_SET_PROCESSING_STOP:
	case IOCTL_SVE_POST_WAIT_PROCESSING_DONE:
	case IOCTL_SVE_POST_GET_PROCESSING_STATUS:
		bRet = SVE_Post_API_Proc(hOpenContext, SVE_get_api_function_code(dwCode), pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
		break;


	case IOCTL_SVE_LOCALPATH_SET_WIN0_START:
	case IOCTL_SVE_LOCALPATH_SET_WIN0_STOP:
	case IOCTL_SVE_LOCALPATH_SET_WIN1_START:
	case IOCTL_SVE_LOCALPATH_SET_WIN1_STOP:
	case IOCTL_SVE_LOCALPATH_SET_WIN2_START:
	case IOCTL_SVE_LOCALPATH_SET_WIN2_STOP:
		bRet = SVE_LocalPath_API_Proc(hOpenContext, SVE_get_api_function_code(dwCode), pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
		break;

	case IOCTL_SVE_ROTATOR_SET_OPERATION_PARAM:
	case IOCTL_SVE_ROTATOR_SET_SOURCE_BUFFER:
	case IOCTL_SVE_ROTATOR_SET_DESTINATION_BUFFER:
	case IOCTL_SVE_ROTATOR_SET_OPERATION_START:
	case IOCTL_SVE_ROTATOR_SET_OPERATION_STOP:
	case IOCTL_SVE_ROTATOR_WAIT_OPERATION_DONE:
	case IOCTL_SVE_ROTATOR_GET_STATUS:
		bRet = SVE_Rotator_API_Proc(hOpenContext, SVE_get_api_function_code(dwCode), pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
		break;

	case IOCTL_SVE_TVSC_SET_PROCESSING_PARAM:
	case IOCTL_SVE_TVSC_SET_SOURCE_BUFFER:
	case IOCTL_SVE_TVSC_SET_NEXT_SOURCE_BUFFER:
	case IOCTL_SVE_TVSC_SET_DESTINATION_BUFFER:
	case IOCTL_SVE_TVSC_SET_NEXT_DESTINATION_BUFFER:
	case IOCTL_SVE_TVSC_SET_PROCESSING_START:
	case IOCTL_SVE_TVSC_SET_PROCESSING_STOP:
	case IOCTL_SVE_TVSC_WAIT_PROCESSING_DONE:
	case IOCTL_SVE_TVSC_GET_PROCESSING_STATUS:
		bRet = SVE_TVScaler_API_Proc(hOpenContext, SVE_get_api_function_code(dwCode), pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
		break;

	case IOCTL_SVE_TVENC_SET_INTERFACE_PARAM:
	case IOCTL_SVE_TVENC_SET_ENCODER_ON:
	case IOCTL_SVE_TVENC_SET_ENCODER_OFF:
	case IOCTL_SVE_TVENC_GET_INTERFACE_STATUS:
		bRet = SVE_TVEncoder_API_Proc(hOpenContext, SVE_get_api_function_code(dwCode), pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
		break;


	case IOCTL_SVE_PM_SET_POWER_ON:
		bRet = SVE_video_engine_power_on();
		break;

	case IOCTL_SVE_PM_SET_POWER_OFF:
		bRet = SVE_video_engine_power_off();
		break;

	case IOCTL_SVE_PM_GET_POWER_STATUS:

	default:
		VDE_ERR((_T("[VDE:ERR] VDE_IOControl() : Unknown IOCTL [0x%08x]\r\n"), dwCode));
		SetLastError (ERROR_INVALID_ACCESS);
		bRet = FALSE;
		break;
	}

	LeaveCriticalSection(&pCtxt->csProc);

	//VDE_MSG((_T("[VDE] --VDE_IOControl()\r\n")));

	return bRet;
}

