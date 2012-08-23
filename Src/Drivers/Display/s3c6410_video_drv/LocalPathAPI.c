#include <bsp.h>
#include "SVEngine.h"

BOOL SVE_LocalPath_API_Proc(
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
	SVEnginePowerContext *pPMCtxt;
	BOOL bRet = TRUE;

	//VDE_MSG((_T("[VDE] SVE_LocalPath_API_Proc(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\r\n"),
	//			hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	VDE_MSG((_T("[VDE] ++SVE_LocalPath_API_Proc()\n\r")));

	pCtxt = SVE_get_context();
	pPMCtxt = SVE_get_power_context();

	//-------------------------------------------------------------
	// Check OpenContext of Caller have the right to access to H/W Resource
	//-------------------------------------------------------------
	switch(dwCode)
	{
		case SVE_LOCALPATH_SET_WIN0_START:
		case SVE_LOCALPATH_SET_WIN0_STOP:
		{
			bRet = (SVE_resource_compare_FIMD_window(DISP_WIN0, hOpenContext)
					&& SVE_resource_compare_Post(hOpenContext));
			break;
		}
		case SVE_LOCALPATH_SET_WIN1_START:
		case SVE_LOCALPATH_SET_WIN1_STOP:
		case SVE_LOCALPATH_SET_WIN2_START:
		case SVE_LOCALPATH_SET_WIN2_STOP:
			bRet = TRUE;
			break;
	}

	if (!bRet)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_LocalPath_API_Proc(0x%08x) : No Right to Access to H/W Resource\r\n"), dwCode));
		goto CleanUp;
	}

	//---------------------------------------
	// Processing IOCTL for Local Path (FIMD+Post)
	//---------------------------------------
	switch(dwCode)
	{
		case SVE_LOCALPATH_SET_WIN0_START:
		{
			BOOL bRetry = TRUE;

			VDE_MSG((_T("[VDE] SVE_LocalPath_API_Proc() : SVE_LOCALPATH_SET_WIN0_START\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				Post_enable_interrupt();
				Post_processing_start();
				Disp_set_framebuffer(DISP_WIN0, IMAGE_FRAMEBUFFER_PA_START);	// Safe Frame Bufer Address for Local Path
				Disp_window_onfoff(DISP_WIN0, DISP_WINDOW_ON);

				pCtxt->bWindowEnable[DISP_WIN0] = TRUE;

				// Wait for Cmd context get free or Wait for Local Path enabled
				if (WAIT_TIMEOUT == SVE_wait_disp_cmd_done())
				{
					VDE_ERR((_T("[VDE:ERR] SVE_LocalPath_API_Proc() : SVE_LOCALPATH_SET_WIN0_START : SVE_wait_disp_cmd_done() TimeOut\n\r")));
					bRet = FALSE;
				}
			}

			// Backup for PM
			pPMCtxt->bLocalPathWin0Enable = TRUE;

			break;
		}

		case SVE_LOCALPATH_SET_WIN0_STOP:
		{
			BOOL bRetry = TRUE;

			VDE_MSG((_T("[VDE] SVE_LocalPath_API_Proc() : SVE_LOCALPATH_SET_WIN0_STOP\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				while(bRetry)
				{
					EnterCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);

					if (pCtxt->LocalPathCmdCtxt.bCmdSetWin0Enable == FALSE
						&& pCtxt->LocalPathCmdCtxt.bCmdSetWin0Disable == FALSE)
					{
						pCtxt->LocalPathCmdCtxt.bCmdSetWin0Disable = TRUE;
						bRetry = FALSE;
					}

					LeaveCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);

					// Wait for Cmd context get free or Wait for Local Path disabled
					//if (WAIT_TIMEOUT == SVE_wait_post_cmd_done()) // because of System Hang problem, move Post IST to Display IST
					if (WAIT_TIMEOUT == SVE_wait_disp_cmd_done())
					{
						VDE_ERR((_T("[VDE:ERR] SVE_LocalPath_API_Proc() : SVE_LOCALPATH_SET_WIN0_STOP : SVE_wait_post_cmd_done() TimeOut\n\r")));
						bRet = FALSE;
					}

					pCtxt->bWindowEnable[DISP_WIN0] = FALSE;
				}
			}

			// Backup for PM
			pPMCtxt->bLocalPathWin0Enable = FALSE;

			break;
		}

		case SVE_LOCALPATH_SET_WIN1_START:
			VDE_ERR((_T("[VDE:ERR] SVE_LocalPath_API_Proc() : SVE_LOCALPATH_SET_WIN1_START : Not Implemented Yet...\n\r")));
			bRet = FALSE;
			break;
		case SVE_LOCALPATH_SET_WIN1_STOP:
			VDE_ERR((_T("[VDE:ERR] SVE_LocalPath_API_Proc() : SVE_LOCALPATH_SET_WIN1_STOP : Not Implemented Yet...\n\r")));
			bRet = FALSE;
			break;
		case SVE_LOCALPATH_SET_WIN2_START:
			VDE_ERR((_T("[VDE:ERR] SVE_LocalPath_API_Proc() : SVE_LOCALPATH_SET_WIN2_START : Not Implemented Yet...\n\r")));
			bRet = FALSE;
			break;
		case SVE_LOCALPATH_SET_WIN2_STOP:
			VDE_ERR((_T("[VDE:ERR] SVE_LocalPath_API_Proc() : SVE_LOCALPATH_SET_WIN2_STOP : Not Implemented Yet...\n\r")));
			bRet = FALSE;
			break;
	}

CleanUp:

	VDE_MSG((_T("[VDE] --SVE_LocalPath_API_Proc()\n\r")));

	return bRet;
}


