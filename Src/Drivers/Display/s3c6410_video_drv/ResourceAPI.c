#include <bsp.h>
#include "SVEngine.h"

BOOL SVE_Resource_API_Proc(
	DWORD hOpenContext,
	DWORD dwCode,
	PBYTE pBufIn,
	DWORD dwLenIn,
	PBYTE pBufOut,
	DWORD dwLenOut,
	PDWORD pdwActualOut
	)
{
	//SVEngineContext *pCtxt;
	BOOL bRet = TRUE;

	//VDE_MSG((_T("[VDE] SVE_Resource_API_Proc(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\r\n"),
	//			hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	VDE_MSG((_T("[VDE] ++SVE_Resource_API_Proc()\n\r")));

	//pCtxt = SVE_get_context();

	switch(dwCode)
	{
	case SVE_RSC_REQUEST_FIMD_INTERFACE:
		bRet = SVE_resource_request_FIMD_interface(hOpenContext);
		break;
	case SVE_RSC_RELEASE_FIMD_INTERFACE:
		bRet = SVE_resource_release_FIMD_interface(hOpenContext);
		break;
	case SVE_RSC_REQUEST_FIMD_WIN0:
		bRet = SVE_resource_request_FIMD_window(DISP_WIN0, hOpenContext);
		break;
	case SVE_RSC_RELEASE_FIMD_WIN0:
		bRet = SVE_resource_release_FIMD_window(DISP_WIN0, hOpenContext);
		break;
	case SVE_RSC_REQUEST_FIMD_WIN1:
		bRet = SVE_resource_request_FIMD_window(DISP_WIN1, hOpenContext);
		break;
	case SVE_RSC_RELEASE_FIMD_WIN1:
		bRet = SVE_resource_release_FIMD_window(DISP_WIN1, hOpenContext);
		break;
	case SVE_RSC_REQUEST_FIMD_WIN2:
		bRet = SVE_resource_request_FIMD_window(DISP_WIN2, hOpenContext);
		break;
	case SVE_RSC_RELEASE_FIMD_WIN2:
		bRet = SVE_resource_release_FIMD_window(DISP_WIN2, hOpenContext);
		break;
	case SVE_RSC_REQUEST_FIMD_WIN3:
		bRet = SVE_resource_request_FIMD_window(DISP_WIN3, hOpenContext);
		break;
	case SVE_RSC_RELEASE_FIMD_WIN3:
		bRet = SVE_resource_release_FIMD_window(DISP_WIN3, hOpenContext);
		break;
	case SVE_RSC_REQUEST_FIMD_WIN4:
		bRet = SVE_resource_request_FIMD_window(DISP_WIN4, hOpenContext);
		break;
	case SVE_RSC_RELEASE_FIMD_WIN4:
		bRet = SVE_resource_release_FIMD_window(DISP_WIN4, hOpenContext);
		break;
	case SVE_RSC_REQUEST_POST:
		bRet = SVE_resource_request_Post(hOpenContext);
		break;
	case SVE_RSC_RELEASE_POST:
		bRet = SVE_resource_release_Post(hOpenContext);
		break;
	case SVE_RSC_REQUEST_ROTATOR:
		bRet = SVE_resource_request_Rotator(hOpenContext);
		break;
	case SVE_RSC_RELEASE_ROTATOR:
		bRet = SVE_resource_release_Rotator(hOpenContext);
		break;
	case SVE_RSC_REQUEST_TVSCALER_TVENCODER:
		bRet = SVE_resource_request_TVScaler_TVEncoder(hOpenContext);
		break;
	case SVE_RSC_RELEASE_TVSCALER_TVENCODER:
		bRet = SVE_resource_release_TVScaler_TVEncoder(hOpenContext);
		break;
	}

	if (bRet == FALSE)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_Resource_API_Proc() : dwCode[0x%08x] Failed\n\r")));
	}

	//VDE_MSG((_T("[VDE] --SVE_Resource_API_Proc()\n\r")));

	return bRet;
}

BOOL SVE_resource_request_FIMD_interface(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();
	SVEnginePowerContext *pPMCtxt = SVE_get_power_context();

	if (pCtxt->dwOccupantFIMD == 0)
	{
		pCtxt->dwOccupantFIMD = dwOpenContext;
		SVE_hw_power_control(HWPWR_DISPLAY_ON);
		SVE_hw_clock_control(HWCLK_DISPLAY_ON);
		pPMCtxt->bFIMDOutputParam = TRUE;
		VDE_MSG((_T("[VDE] SVE_resource_request_FIMD_interface() : OpenContext[0x%08x] have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else if (pCtxt->dwOccupantFIMD == dwOpenContext)
	{
		//VDE_ERR((_T("[VDE:ERR] SVE_resource_request_FIMD_interface() : OpenContext[0x%08x] already have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_request_FIMD_interface() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_release_FIMD_interface(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();
	SVEnginePowerContext *pPMCtxt = SVE_get_power_context();

	if (pCtxt->dwOccupantFIMD == dwOpenContext)
	{
		pCtxt->dwOccupantFIMD = 0;
		SVE_hw_power_control(HWPWR_DISPLAY_OFF);
		SVE_hw_clock_control(HWCLK_DISPLAY_OFF);
		pPMCtxt->bFIMDOutputParam = FALSE;
		VDE_MSG((_T("[VDE] SVE_resource_request_FIMD_interface() : OpenContext[0x%08x] release resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_request_FIMD_interface() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_compare_FIMD_interface(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();

	if (pCtxt->dwOccupantFIMD == dwOpenContext)
	{
		VDE_MSG((_T("[VDE] SVE_resource_compare_FIMD_interface() : OpenContext[0x%08x] have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else if (pCtxt->dwOccupantFIMD == 0)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_request_FIMD_interface() : Resource was not occupied by Anyone\r\n")));
		return FALSE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_request_FIMD_interface() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_request_FIMD_window(DWORD dwWinNum, DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();

	if (pCtxt->dwOccupantFIMDWindow[dwWinNum] == 0)
	{
		pCtxt->dwOccupantFIMDWindow[dwWinNum] = dwOpenContext;
		VDE_MSG((_T("[VDE] SVE_resource_request_FIMD_window() : OpenContext[0x%08x] have resource Win[%d]\r\n"), dwOpenContext, dwWinNum));
		return TRUE;
	}
	else if (pCtxt->dwOccupantFIMDWindow[dwWinNum] == dwOpenContext)
	{
		//VDE_ERR((_T("[VDE:ERR] SVE_resource_request_FIMD_window() : OpenContext[0x%08x] already have resource Win[%d]\r\n"), dwOpenContext, dwWinNum));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_request_FIMD_window() : Resource Win[%d] was occupied by other OpenContext\r\n"), dwWinNum));
		return FALSE;
	}
}

BOOL SVE_resource_release_FIMD_window(DWORD dwWinNum, DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();

	if (pCtxt->dwOccupantFIMDWindow[dwWinNum] == dwOpenContext)
	{
		pCtxt->dwOccupantFIMDWindow[dwWinNum] = 0;
		VDE_MSG((_T("[VDE] SVE_resource_release_FIMD_window() : OpenContext[0x%08x] release resource Win[%d]\r\n"), dwOpenContext, dwWinNum));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_release_FIMD_window() : Resource Win[%d] was occupied by other OpenContext\r\n"), dwWinNum));
		return FALSE;
	}
}

BOOL SVE_resource_compare_FIMD_window(DWORD dwWinNum, DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();

	if (pCtxt->dwOccupantFIMDWindow[dwWinNum] == dwOpenContext)
	{
		VDE_MSG((_T("[VDE] SVE_resource_compare_FIMD_window() : OpenContext[0x%08x] have resource Win[%d]\r\n"), dwOpenContext, dwWinNum));
		return TRUE;
	}
	else if (pCtxt->dwOccupantFIMDWindow[dwWinNum] == 0)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_compare_FIMD_window() : Resource Win[%d] was not occupied by Anyone\r\n"), dwWinNum));
		return FALSE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_compare_FIMD_window() : Resource Win[%d] was occupied by other OpenContext\r\n"), dwWinNum));
		return FALSE;
	}
}

BOOL SVE_resource_request_Post(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();
	SVEnginePowerContext *pPMCtxt = SVE_get_power_context();

	if (pCtxt->dwOccupantPost == 0)
	{
		pCtxt->dwOccupantPost = dwOpenContext;
		SVE_hw_power_control(HWPWR_POST_ON);
		SVE_hw_clock_control(HWCLK_POST_ON);
		pPMCtxt->bPostParam = TRUE;
		VDE_MSG((_T("[VDE] SVE_resource_request_Post() : OpenContext[0x%08x] have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else if (pCtxt->dwOccupantPost == dwOpenContext)
	{
		//VDE_ERR((_T("[VDE:ERR] SVE_resource_request_Post() : OpenContext[0x%08x] already have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_request_Post() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_release_Post(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();
	SVEnginePowerContext *pPMCtxt = SVE_get_power_context();

	if (pCtxt->dwOccupantPost == dwOpenContext)
	{
		pCtxt->dwOccupantPost = 0;
		SVE_hw_power_control(HWPWR_POST_OFF);
		SVE_hw_clock_control(HWCLK_POST_OFF);
		pPMCtxt->bPostParam = FALSE;
		VDE_MSG((_T("[VDE] SVE_resource_release_Post() : OpenContext[0x%08x] release resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_release_Post() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_compare_Post(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();

	if (pCtxt->dwOccupantPost == dwOpenContext)
	{
		VDE_MSG((_T("[VDE] SVE_resource_compare_Post() : OpenContext[0x%08x] have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else if (pCtxt->dwOccupantPost == 0)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_compare_Post() : Resource was not occupied by Anyone\r\n")));
		return FALSE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_compare_Post() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_request_Rotator(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();
	SVEnginePowerContext *pPMCtxt = SVE_get_power_context();

	if (pCtxt->dwOccupantRotator == 0)
	{
		pCtxt->dwOccupantRotator = dwOpenContext;
		SVE_hw_power_control(HWPWR_ROTATOR_ON);
		SVE_hw_clock_control(HWCLK_ROTATOR_ON);
		pPMCtxt->bRotatorParam = TRUE;
		VDE_MSG((_T("[VDE] SVE_resource_request_Rotator() : OpenContext[0x%08x] have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else if (pCtxt->dwOccupantRotator == dwOpenContext)
	{
		//VDE_ERR((_T("[VDE:ERR] SVE_resource_request_Rotator() : OpenContext[0x%08x] already have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_request_Rotator() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_release_Rotator(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();
	SVEnginePowerContext *pPMCtxt = SVE_get_power_context();

	if (pCtxt->dwOccupantRotator == dwOpenContext)
	{
		pCtxt->dwOccupantRotator = 0;
		SVE_hw_power_control(HWPWR_ROTATOR_OFF);
		SVE_hw_clock_control(HWCLK_ROTATOR_OFF);
		pPMCtxt->bRotatorParam = FALSE;
		VDE_MSG((_T("[VDE] SVE_resource_release_Rotator() : OpenContext[0x%08x] release resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_release_Rotator() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_compare_Rotator(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();

	if (pCtxt->dwOccupantRotator == dwOpenContext)
	{
		VDE_MSG((_T("[VDE] SVE_resource_compare_Rotator() : OpenContext[0x%08x] have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else if (pCtxt->dwOccupantRotator == 0)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_compare_Rotator() : Resource was not occupied by Anyone\r\n")));
		return FALSE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_compare_Rotator() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_request_TVScaler_TVEncoder(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();
	SVEnginePowerContext *pPMCtxt = SVE_get_power_context();

	if (pCtxt->dwOccupantTVScalerTVEncoder == 0)
	{
		pCtxt->dwOccupantTVScalerTVEncoder = dwOpenContext;
		SVE_hw_power_control(HWPWR_TV_ON);
		SVE_hw_clock_control(HWCLK_TV_ON);
		pPMCtxt->bTVSCParam = TRUE;
		pPMCtxt->bTVEncParam = TRUE;
		VDE_MSG((_T("[VDE] SVE_resource_request_TVScaler_TVEncoder() : OpenContext[0x%08x] have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else if (pCtxt->dwOccupantTVScalerTVEncoder == dwOpenContext)
	{
		//VDE_ERR((_T("[VDE:ERR] SVE_resource_request_TVScaler_TVEncoder() : OpenContext[0x%08x] already have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_request_TVScaler_TVEncoder() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_release_TVScaler_TVEncoder(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();
	SVEnginePowerContext *pPMCtxt = SVE_get_power_context();

	if (pCtxt->dwOccupantTVScalerTVEncoder == dwOpenContext)
	{
		pCtxt->dwOccupantTVScalerTVEncoder = 0;
		SVE_hw_power_control(HWPWR_TV_OFF);
		SVE_hw_clock_control(HWCLK_TV_OFF);
		pPMCtxt->bTVSCParam = FALSE;
		pPMCtxt->bTVEncParam = FALSE;
		VDE_MSG((_T("[VDE] SVE_resource_release_TVScaler_TVEncoder() : OpenContext[0x%08x] release resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_release_TVScaler_TVEncoder() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

BOOL SVE_resource_compare_TVScaler_TVEncoder(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();

	if (pCtxt->dwOccupantTVScalerTVEncoder == dwOpenContext)
	{
		VDE_MSG((_T("[VDE] SVE_resource_compare_TVScaler_TVEncoder() : OpenContext[0x%08x] have resource\r\n"), dwOpenContext));
		return TRUE;
	}
	else if (pCtxt->dwOccupantTVScalerTVEncoder == 0)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_compare_TVScaler_TVEncoder() : Resource was not occupied by Anyone\r\n")));
		return FALSE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_resource_compare_TVScaler_TVEncoder() : Resource was occupied by other OpenContext\r\n")));
		return FALSE;
	}
}

