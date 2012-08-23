#include <bsp.h>
#include <DrvLib.h>
#include "SVEngine.h"

static SVEngineContext SVECtxt;
static SVEnginePowerContext SVEPMCtxt;

BOOL SVE_initialize_video_engine(void)
{
	SVEngineContext *pCtxt;

	VDE_MSG((_T("[VDE] ++SVE_initialize_video_engine()\r\n")));

	pCtxt = SVE_get_context();

	// Clear Context
	SVE_initialize_context();
	SVE_initialize_power_context();

	// map device SFR address
	if (SVE_map_device_address() == FALSE)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_video_engine() : SVE_map_device_address() Failed\r\n")));
		goto CleanUp;
	}

	// Intialize interrupt
	if (SVE_initialize_interrupt() == FALSE)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_video_engine() : SVE_intialize_interrupt() Failed\r\n")));
		goto CleanUp;
	}

	// Create Interrupt Thread
	if (SVE_initialize_thread() == FALSE)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_video_engine() : SVE_initialize_thread() Failed\r\n")));
		goto CleanUp;
	}

	// Open Power Control Driver
	if (SVE_initialize_power_control() == FALSE)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_video_engine() : SVE_initialize_power_control() Failed\r\n")));
		goto CleanUp;
	}

	// Initialize SFR Address of Sub Module
	LDI_initialize_register_address((void *)pCtxt->pSPIReg, (void *)pCtxt->pDispConReg, (void *)pCtxt->pGPIOReg);
	Disp_initialize_register_address((void *)pCtxt->pDispConReg, (void *)pCtxt->pMSMIFReg, (void *)pCtxt->pGPIOReg);
	Post_initialize_register_address((void *)pCtxt->pPostReg);
	Rotator_initialize_register_address((void *)pCtxt->pRotatorReg);
	TVSC_initialize_register_address((void *)pCtxt->pTVSCReg);
	TVEnc_initialize_register_address((void *)pCtxt->pTVEncReg, (void *)pCtxt->pGPIOReg);

	// Enable All Block Power
	//SVE_hw_power_control(HWPWR_ALL_ON);
	SVE_hw_power_control(HWPWR_DISPLAY_ON);
	SVE_hw_power_control(HWPWR_2D_ON);
	//SVE_hw_clock_control(HWCLK_ALL_ON);
	SVE_hw_clock_control(HWCLK_DISPLAY_ON);
	SVE_hw_clock_control(HWCLK_2D_ON);
	// TODO: manage each block power an clock

	VDE_MSG((_T("[VDE] --SVE_initialize_video_engine()\r\n")));

	return TRUE;

CleanUp:

	SVE_deinitialize_video_engine();

	VDE_ERR((_T("[VDE:ERR] --SVE_initialize_video_engine() : Failed\r\n")));


	return FALSE;
}


BOOL SVE_deinitialize_video_engine(void)
{
        //SVEngineContext *pCtxt;

	VDE_MSG((_T("[VDE] ++SVE_deinitialize_video_engine()\r\n")));

	//pCtxt = SVE_get_context();

	SVE_deinitialize_context();
	SVE_deinitialize_power_context();
	SVE_unmap_device_address();
	SVE_deinitialize_interrupt();
	SVE_deinitialize_thread();
	SVE_deinitialize_power_control();

	VDE_MSG((_T("[VDE] --SVE_deinitialize_video_engine()\r\n")));

	return TRUE;
}

SVEngineContext* SVE_get_context(void)
{
	return &SVECtxt;
}

SVEnginePowerContext* SVE_get_power_context(void)
{
	return &SVEPMCtxt;
}

BOOL SVE_map_device_address(void)
{
	SVEngineContext *pCtxt;

	VDE_MSG((_T("[VDE] ++SVE_map_device_address()\r\n")));

	pCtxt = SVE_get_context();

	pCtxt->pDispConReg = (S3C6410_DISPLAY_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_DISPLAY, sizeof(S3C6410_DISPLAY_REG), FALSE);
	if (pCtxt->pDispConReg == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_map_device_address() : pDispConReg DrvLib_MapIoSpace() Failed\r\n")));
		goto CleanUp;
	}

	pCtxt->pPostReg = (S3C6410_POST_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_POST, sizeof(S3C6410_POST_REG), FALSE);
	if (pCtxt->pPostReg == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_map_device_address() : pPostReg DrvLib_MapIoSpace() Failed\r\n")));
		goto CleanUp;
	}

	pCtxt->pRotatorReg = (S3C6410_ROTATOR_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_ROTATOR, sizeof(S3C6410_ROTATOR_REG), FALSE);
	if (pCtxt->pRotatorReg == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_map_device_address() : pRotatorReg DrvLib_MapIoSpace() Failed\r\n")));
		goto CleanUp;
	}

	pCtxt->pTVSCReg = (S3C6410_TVSC_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_TVSC, sizeof(S3C6410_TVSC_REG), FALSE);
	if (pCtxt->pTVSCReg == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_map_device_address() : pTVSCReg DrvLib_MapIoSpace() Failed\r\n")));
		goto CleanUp;
	}

	pCtxt->pTVEncReg = (S3C6410_TVENC_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_TVENC, sizeof(S3C6410_TVENC_REG), FALSE);
	if (pCtxt->pTVEncReg == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_map_device_address() : pTVEncReg DrvLib_MapIoSpace() Failed\r\n")));
		goto CleanUp;
	}

	pCtxt->pMSMIFReg = (S3C6410_MSMIF_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_MSMIF_SFR, sizeof(S3C6410_MSMIF_REG), FALSE);
	if (pCtxt->pMSMIFReg == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_map_device_address() : pMSMIFReg DrvLib_MapIoSpace() Failed\r\n")));
		goto CleanUp;
	}

	pCtxt->pSysConReg = (S3C6410_SYSCON_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_SYSCON, sizeof(S3C6410_SYSCON_REG), FALSE);
	if (pCtxt->pSysConReg == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_map_device_address() : pSysConReg DrvLib_MapIoSpace() Failed\r\n")));
		goto CleanUp;
	}

	pCtxt->pGPIOReg = (S3C6410_GPIO_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_GPIO, sizeof(S3C6410_GPIO_REG), FALSE);
	if (pCtxt->pGPIOReg == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_map_device_address() : pGPIOReg DrvLib_MapIoSpace() Failed\r\n")));
		goto CleanUp;
	}

	pCtxt->pSPIReg = (S3C6410_SPI_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_SPI0, sizeof(S3C6410_SPI_REG), FALSE);
	if (pCtxt->pSPIReg == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_map_device_address() : pSPIReg DrvLib_MapIoSpace() Failed\r\n")));
		goto CleanUp;
	}

	VDE_MSG((_T("[VDE] --SVE_map_device_address()\r\n")));

	return TRUE;

CleanUp:

	VDE_ERR((_T("[VDE:ERR] --SVE_map_device_address() : Failed\r\n")));

	return FALSE;
}

void SVE_unmap_device_address(void)
{
	SVEngineContext *pCtxt;

	VDE_MSG((_T("[VDE] ++SVE_unmap_device_address()\r\n")));

	pCtxt = SVE_get_context();

	if (pCtxt->pDispConReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)pCtxt->pDispConReg);
		pCtxt->pDispConReg = NULL;
	}

	if (pCtxt->pPostReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)pCtxt->pPostReg);
		pCtxt->pPostReg = NULL;
	}

	if (pCtxt->pRotatorReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)pCtxt->pRotatorReg);
		pCtxt->pRotatorReg = NULL;
	}

	if (pCtxt->pTVSCReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)pCtxt->pTVSCReg);
		pCtxt->pTVSCReg = NULL;
	}

	if (pCtxt->pTVEncReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)pCtxt->pTVEncReg);
		pCtxt->pTVEncReg = NULL;
	}

	if (pCtxt->pMSMIFReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)pCtxt->pMSMIFReg);
		pCtxt->pMSMIFReg = NULL;
	}

	if (pCtxt->pSysConReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)pCtxt->pSysConReg);
		pCtxt->pSysConReg = NULL;
	}

	if (pCtxt->pGPIOReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)pCtxt->pGPIOReg);
		pCtxt->pGPIOReg = NULL;
	}

	if (pCtxt->pSPIReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)pCtxt->pSPIReg);
		pCtxt->pSPIReg = NULL;
	}

	VDE_MSG((_T("[VDE] --SVE_unmap_device_address()\r\n")));
}

void SVE_initialize_context(void)
{
	SVEngineContext *pCtxt;
	int iWinNum;

	VDE_MSG((_T("[VDE] ++SVE_initialize_context()\r\n")));

	pCtxt = SVE_get_context();

	// Clear Context
	memset(pCtxt, 0x0, sizeof(SVEngineContext));

	pCtxt->dwSignature = SVE_DRIVER_SIGNITURE;
	pCtxt->dwLastOpenContext = SVE_DRIVER_SIGNITURE;

	pCtxt->dwSysIntrDisp = SYSINTR_UNDEFINED;
	pCtxt->dwSysIntrPost = SYSINTR_UNDEFINED;
	pCtxt->dwSysIntrTVSC = SYSINTR_UNDEFINED;
	pCtxt->dwSysIntrRotator = SYSINTR_UNDEFINED;
	pCtxt->hPowerControl = INVALID_HANDLE_VALUE;

	// Critical Section for IOCTL
	InitializeCriticalSection(&pCtxt->csProc);

	// Display Controller Command Context
	for (iWinNum = DISP_WIN0; iWinNum < DISP_WIN_MAX; iWinNum ++)
	{
		InitializeCriticalSection(&pCtxt->DispCmdCtxt[iWinNum].csCmd);
		pCtxt->DispCmdCtxt[iWinNum].bCmdSetBuffer = FALSE;
		pCtxt->DispCmdCtxt[iWinNum].bCmdSetPosition = FALSE;
	}

	// Post Processor Command Context
	InitializeCriticalSection(&pCtxt->PostCmdCtxt.csCmd);
	pCtxt->PostCmdCtxt.bCmdSetSrcBuffer = FALSE;
	pCtxt->PostCmdCtxt.bCmdSetDstBuffer = FALSE;

	// Local Path Command Context
	InitializeCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
	pCtxt->LocalPathCmdCtxt.bCmdSetWin0Enable = FALSE;		// Depricated
	pCtxt->LocalPathCmdCtxt.bCmdSetWin0Disable = FALSE;
	pCtxt->LocalPathCmdCtxt.bCmdSetWin1Enable = FALSE;		// Depricated
	pCtxt->LocalPathCmdCtxt.bCmdSetWin1Disable = FALSE;
	pCtxt->LocalPathCmdCtxt.bCmdSetWin2Enable = FALSE;		// Depricated
	pCtxt->LocalPathCmdCtxt.bCmdSetWin2Disable = FALSE;

	// TV Scaler Command Context
	InitializeCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);
	pCtxt->TVSCCmdCtxt.bCmdSetSrcBuffer = FALSE;
	pCtxt->TVSCCmdCtxt.bCmdSetDstBuffer = FALSE;

	// Command Event
	pCtxt->hDispCmdDone = CreateEvent(NULL, TRUE, FALSE, NULL);	// Manual Reset, You should call ResetEvent() or use PulseEvent() Only
	pCtxt->hPostCmdDone = CreateEvent(NULL, TRUE, FALSE, NULL);	// Manual Reset, You should call ResetEvent() or use PulseEvent() Only
	pCtxt->hTVSCCmdDone = CreateEvent(NULL, TRUE, FALSE, NULL);	// Manual Reset, You should call ResetEvent() or use PulseEvent() Only

	// Operation Finish Event
	pCtxt->hRotatorFinish = CreateEvent(NULL, TRUE, FALSE, NULL);	// Manual Reset, You should call ResetEvent() or use PulseEvent() Only

	// Video engine HW configuration for Power Management
#if	0	// All values are FALSE or Zero
	BOOL bFIMDOutputEnable;
	BOOL bFIMDOutputTV;
	BOOL bFIMDWin0Enable;
	BOOL bFIMDWin1Enable;
	BOOL bFIMDWin2Enable;
	BOOL bFIMDWin3Enable;
	BOOL bFIMDWin4Enable;

	BOOL bLocalPathWin0PostEnable;
	BOOL bLocalPathWin1TVScalerEnable;
	BOOL bLocalPathWin1CamPreviewEnable;
	BOOL bLocalPathWin2TVScalerEnable;
	BOOL bLocalPathWin2CamCodecEnable;

	BOOL bPostEnable;
	BOOL bRotatorEnable;
	BOOL bTVSCEnable;
	BOOL bTVEncEnable;

	SVEARG_FIMD_OUTPUT_IF			tBK_FIMDOutputParam;
	SVEARG_FIMD_WIN_MODE			tBK_FIMDWinMode[DISP_WIN_MAX];
	SVEARG_FIMD_WIN_FRAMEBUFFER	tBK_FIMDWinFBuffer[DISP_WIN_MAX];
	SVEARG_FIMD_WIN_COLORKEY		tBK_FIMDColorKey[DISP_WIN_MAX];
	SVEARG_FIMD_WIN_ALPHA			tBK_FIMDAlpha[DISP_WIN_MAX];

	SVEARG_POST_PARAMETER			tBK_PostParam;
	SVEARG_POST_BUFFER				tBK_PostBuffer;

	SVEARG_ROTATOR_PARAMETER		tBK_RotatorParam;
	SVEARG_POST_BUFFER				tBK_RotatorBuffer;

	SVEARG_TVSC_PARAMETER			tBK_TVSCParam;
	SVEARG_TVSC_BUFFER				tBK_TVSCBuffer;

	SVEARG_TVENC_PARAMETER		tBK_TVEncParam;
#endif

	VDE_MSG((_T("[VDE] --SVE_initialize_context()\r\n")));
}


void SVE_deinitialize_context(void)
{
	SVEngineContext *pCtxt;
	int iWinNum;

	VDE_MSG((_T("[VDE] ++SVE_deinitialize_context()\r\n")));

	pCtxt = SVE_get_context();

	// Display Controller Command Context
	for (iWinNum = DISP_WIN0; iWinNum < DISP_WIN_MAX; iWinNum ++)
	{
		DeleteCriticalSection(&pCtxt->DispCmdCtxt[iWinNum].csCmd);
	}

	// Post Processor Command Context
	DeleteCriticalSection(&pCtxt->PostCmdCtxt.csCmd);

	// Local Path Command Context
	DeleteCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);

	// TV Scaler Command Context
	DeleteCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);

	// Critical Section for IOCTL
	DeleteCriticalSection(&pCtxt->csProc);

	if (pCtxt->hDispCmdDone != NULL)
	{
		CloseHandle(pCtxt->hDispCmdDone);
	}

	if (pCtxt->hPostCmdDone != NULL)
	{
		CloseHandle(pCtxt->hPostCmdDone);
	}

	if (pCtxt->hTVSCCmdDone != NULL)
	{
		CloseHandle(pCtxt->hTVSCCmdDone);
	}

	if (pCtxt->hRotatorFinish != NULL)
	{
		CloseHandle(pCtxt->hRotatorFinish);
	}

	VDE_MSG((_T("[VDE] --SVE_deinitialize_context()\r\n")));
}


DWORD SVE_get_api_function_code(DWORD dwCode)
{
	//#define CTL_CODE( DeviceType, Function, Method, Access ) ( ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) )
	return ((dwCode>>2)&0xFFF);
}

DWORD SVE_get_driver_signature(void)
{
	return (SVE_get_context()->dwSignature);
}

DWORD SVE_add_open_context(void)
{
	SVEngineContext *pCtxt = SVE_get_context();

	pCtxt->dwOpenCount++;
	pCtxt->dwLastOpenContext++;

	VDE_MSG((_T("[VDE] SVE_add_open_context() : OpenCount = %d, OpenContext = 0x%08x\r\n"), pCtxt->dwOpenCount, pCtxt->dwLastOpenContext));

	return (pCtxt->dwLastOpenContext);
}

BOOL SVE_remove_open_context(DWORD dwOpenContext)
{
	SVEngineContext *pCtxt = SVE_get_context();

	if (pCtxt->dwOpenCount > 0)
	{
		// Release H/W Resource
		if (pCtxt->dwOccupantFIMD == dwOpenContext) pCtxt->dwOccupantFIMD = 0;
		if (pCtxt->dwOccupantFIMDWindow[0] == dwOpenContext) pCtxt->dwOccupantFIMDWindow[0] = 0;
		if (pCtxt->dwOccupantFIMDWindow[1] == dwOpenContext) pCtxt->dwOccupantFIMDWindow[1] = 0;
		if (pCtxt->dwOccupantFIMDWindow[2] == dwOpenContext) pCtxt->dwOccupantFIMDWindow[2] = 0;
		if (pCtxt->dwOccupantFIMDWindow[3] == dwOpenContext) pCtxt->dwOccupantFIMDWindow[3] = 0;
		if (pCtxt->dwOccupantFIMDWindow[4] == dwOpenContext) pCtxt->dwOccupantFIMDWindow[4] = 0;
		if (pCtxt->dwOccupantPost == dwOpenContext) pCtxt->dwOccupantPost = 0;
		if (pCtxt->dwOccupantRotator == dwOpenContext) pCtxt->dwOccupantRotator = 0;
		if (pCtxt->dwOccupantTVScalerTVEncoder == dwOpenContext) pCtxt->dwOccupantTVScalerTVEncoder = 0;

		pCtxt->dwOpenCount--;

		VDE_MSG((_T("[VDE] SVE_remove_open_context(0x%08x) : OpenCount = %d\r\n"), pCtxt->dwLastOpenContext, pCtxt->dwOpenCount));

		return TRUE;
	}
	else
	{
		VDE_ERR((_T("[VDE:ERR] SVE_remove_open_context() : Current Open Count is 0 !!!\r\n")));

		return FALSE;
	}
}

DWORD SVE_get_current_open_count(void)
{
	return (SVE_get_context()->dwOpenCount);
}

