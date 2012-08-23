#include <bsp.h>
#include "SVEngine.h"

BOOL SVE_DispCon_API_Proc(
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

	//VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\r\n"),
	//			hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	VDE_MSG((_T("[VDE] ++SVE_DispCon_API_Proc()\n\r")));

	pCtxt = SVE_get_context();
	pPMCtxt = SVE_get_power_context();

	//-------------------------------------------------------------
	// Check OpenContext of Caller have the right to access to H/W Resource
	//-------------------------------------------------------------
	switch(dwCode)
	{
		case SVE_FIMD_SET_INTERFACE_PARAM:
		case SVE_FIMD_SET_OUTPUT_RGBIF:
		case SVE_FIMD_SET_OUTPUT_TV:
		case SVE_FIMD_SET_OUTPUT_ENABLE:
		case SVE_FIMD_SET_OUTPUT_DISABLE:
		case SVE_FIMD_GET_OUTPUT_STATUS:
		{
			bRet = SVE_resource_compare_FIMD_interface(hOpenContext);
			break;
		}
		case SVE_FIMD_SET_WINDOW_MODE:
		{
			SVEARG_FIMD_WIN_MODE *pArg = (SVEARG_FIMD_WIN_MODE *)pBufIn;
			bRet = SVE_resource_compare_FIMD_window(Disp_get_win_num_from_win_mode(pArg->dwWinMode), hOpenContext);
			break;
		}
		case SVE_FIMD_SET_WINDOW_POSITION:
		{
			SVEARG_FIMD_WIN_POS *pArg = (SVEARG_FIMD_WIN_POS *)pBufIn;
			bRet = SVE_resource_compare_FIMD_window(pArg->dwWinNum, hOpenContext);
			break;
		}
		case SVE_FIMD_SET_WINDOW_FRAMEBUFFER:
		{
			SVEARG_FIMD_WIN_FRAMEBUFFER *pArg = (SVEARG_FIMD_WIN_FRAMEBUFFER *)pBufIn;
			bRet = SVE_resource_compare_FIMD_window(pArg->dwWinNum, hOpenContext);
			break;
		}
		case SVE_FIMD_SET_WINDOW_ENABLE:
		case SVE_FIMD_SET_WINDOW_DISABLE:
		{
			DWORD dwWinNum = *((DWORD *)pBufIn);
			bRet = SVE_resource_compare_FIMD_window(dwWinNum, hOpenContext);
			break;
		}
#if	0	// TODO: Who should control ?? Owner of Win0? or Owner of Win1?
		case SVE_FIMD_SET_WINDOW_BLEND_COLORKEY:
		{
			SVEARG_FIMD_WIN_COLORKEY *pArg = (SVEARG_FIMD_WIN_COLORKEY *)pBufIn;
			bRet = SVE_resource_compare_FIMD_window(pArg->dwWinNum, hOpenContext);
			break;
		}
		case SVE_FIMD_SET_WINDOW_BLEND_ALPHA:
		{
			SVEARG_FIMD_WIN_ALPHA *pArg = (SVEARG_FIMD_WIN_ALPHA *)pBufIn;
			bRet = SVE_resource_compare_FIMD_window(pArg->dwWinNum, hOpenContext);
			break;
		}
#else
		case SVE_FIMD_SET_WINDOW_BLEND_COLORKEY:
		case SVE_FIMD_SET_WINDOW_BLEND_ALPHA:
#endif
		case SVE_FIMD_WAIT_FRAME_INTERRUPT:
		case SVE_FIMD_SET_WINDOW_COLORMAP:
		case SVE_FIMD_GET_WINDOW_STATUS:
		case SVE_FIMD_SET_WINDOW_BLEND_DISABLE:
			bRet = TRUE;
			break;
	}

	if (!bRet)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_DispCon_API_Proc(0x%08x) : No Right to Access to H/W Resource\r\n"), dwCode));
		goto CleanUp;
	}

	//-----------------------------------
	// Processing IOCTL for Display Controller
	//-----------------------------------
	switch(dwCode)
	{
		case SVE_FIMD_SET_INTERFACE_PARAM:
		{
			SVEARG_FIMD_OUTPUT_IF *pArg;

			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_INTERFACE_PARAM\n\r")));

			pArg = (SVEARG_FIMD_OUTPUT_IF *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				// Setup Output Device Information fot LCD
				Disp_set_output_device_information(&pArg->tRGBDevInfo);

				// Setup Output Device Information for TV
				Disp_set_output_TV_information(pArg->dwTVOutScreenWidth, pArg->dwTVOutScreenHeight);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tFIMDOutputParam, pArg, sizeof(SVEARG_FIMD_OUTPUT_IF));
			//pPMCtxt->bFIMDOutputParam = TRUE; // Move to ResourceAPI.c

			break;
		}

		case SVE_FIMD_SET_OUTPUT_RGBIF:
		{
			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_OUTPUT_RGBIF\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				// Initialize Display Controller for RGB I/F
				Disp_initialize_output_interface(DISP_VIDOUT_RGBIF);
			}

			// Backup for PM
			pPMCtxt->bFIMDOutputTV = FALSE;

			break;
		}

		case SVE_FIMD_SET_OUTPUT_TV:
		{
			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_OUTPUT_TV\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				// Initialize Display Controller for TV out
				Disp_initialize_output_interface(DISP_VIDOUT_TVENCODER);
			}

			// Backup for PM
			pPMCtxt->bFIMDOutputTV = TRUE;

			break;
		}

		case SVE_FIMD_SET_OUTPUT_ENABLE:
		{
			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_OUTPUT_ENABLE\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				// Enable Interrupt
				Disp_set_frame_interrupt(DISP_FRMINT_FRONTPORCH);
				Disp_enable_frame_interrupt();

				// Video Output Enable
				Disp_envid_onoff(DISP_ENVID_ON);
				pCtxt->bVideoEnable = TRUE;
			}

			// Backup for PM
			pPMCtxt->bFIMDOutputEnable = TRUE;

			break;
		}

		case SVE_FIMD_SET_OUTPUT_DISABLE:
		{
			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_OUTPUT_DISABLE\n\r")));
			Sleep(10); //Because of Bug for TVSC off. why?

			if (pPMCtxt->bPowerOn)
			{
				// Video Output Disable
				//Disp_envid_onoff(DISP_ENVID_DIRECT_OFF);
				Disp_envid_onoff(DISP_ENVID_OFF);
				pCtxt->bVideoEnable = FALSE;
			}

			// Backup for PM
			pPMCtxt->bFIMDOutputEnable = FALSE;

			break;
		}

		case SVE_FIMD_SET_WINDOW_MODE:
		{
			SVEARG_FIMD_WIN_MODE *pArg;

			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_MODE\n\r")));

			pArg = (SVEARG_FIMD_WIN_MODE *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				Disp_set_window_mode(pArg->dwWinMode, pArg->dwBPP, pArg->dwWidth, pArg->dwHeight, pArg->dwOffsetX, pArg->dwOffsetY);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tFIMDWinMode[Disp_get_win_num_from_win_mode(pArg->dwWinMode)], pArg, sizeof(SVEARG_FIMD_WIN_MODE));
			pPMCtxt->bFIMDWinMode[Disp_get_win_num_from_win_mode(pArg->dwWinMode)] = TRUE;

			break;
		}

		case SVE_FIMD_SET_WINDOW_POSITION:
		{
			SVEARG_FIMD_WIN_POS *pArg;

			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_POSITION\n\r")));

			pArg = (SVEARG_FIMD_WIN_POS *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (pCtxt->bVideoEnable && pCtxt->bWindowEnable[pArg->dwWinNum])	// Window is in use
				{
					BOOL bRetry = TRUE;

					while(bRetry)
					{
						EnterCriticalSection(&pCtxt->DispCmdCtxt[pArg->dwWinNum].csCmd);

						if (pCtxt->DispCmdCtxt[pArg->dwWinNum].bCmdSetPosition == FALSE)
						{
							pCtxt->DispCmdCtxt[pArg->dwWinNum].dwOffsetX = pArg->dwOffsetX;
							pCtxt->DispCmdCtxt[pArg->dwWinNum].dwOffsetY = pArg->dwOffsetY;
							pCtxt->DispCmdCtxt[pArg->dwWinNum].bCmdSetPosition = TRUE;
							bRetry = FALSE;
						}

						LeaveCriticalSection(&pCtxt->DispCmdCtxt[pArg->dwWinNum].csCmd);

						if (bRetry)
						{
							if (WAIT_TIMEOUT == SVE_wait_disp_cmd_done())
							{
								VDE_ERR((_T("[VDE:ERR] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_POSITION : SVE_wait_disp_cmd_done() TimeOut\n\r")));
								bRet = FALSE;
							}
						}
					}
				}
				else
				{
					Disp_set_window_position(pArg->dwWinNum, pArg->dwOffsetX, pArg->dwOffsetY);
				}
			}

			// Backup for PM
			pPMCtxt->tFIMDWinMode[pArg->dwWinNum].dwOffsetX = pArg->dwOffsetX;
			pPMCtxt->tFIMDWinMode[pArg->dwWinNum].dwOffsetY = pArg->dwOffsetY;
			pPMCtxt->bFIMDWinMode[pArg->dwWinNum] = TRUE;

			break;
		}

		case SVE_FIMD_SET_WINDOW_FRAMEBUFFER:
		{
			SVEARG_FIMD_WIN_FRAMEBUFFER *pArg;

			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_FRAMEBUFFER\n\r")));

			pArg = (SVEARG_FIMD_WIN_FRAMEBUFFER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (pCtxt->bVideoEnable && pCtxt->bWindowEnable[pArg->dwWinNum])	// Window is in use
				{
					BOOL bRetry = TRUE;

					while(bRetry)
					{
						EnterCriticalSection(&pCtxt->DispCmdCtxt[pArg->dwWinNum].csCmd);

						if (pCtxt->DispCmdCtxt[pArg->dwWinNum].bCmdSetBuffer == FALSE)
						{
							pCtxt->DispCmdCtxt[pArg->dwWinNum].dwBuffer = pArg->dwFrameBuffer;
							pCtxt->DispCmdCtxt[pArg->dwWinNum].bCmdSetBuffer = TRUE;
							bRetry = FALSE;
						}

						LeaveCriticalSection(&pCtxt->DispCmdCtxt[pArg->dwWinNum].csCmd);

						if (bRetry || pArg->bWaitForVSync)
						{
							if (WAIT_TIMEOUT == SVE_wait_disp_cmd_done())
							{
								VDE_ERR((_T("[VDE:ERR] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_FRAMEBUFFER : SVE_wait_disp_cmd_done() TimeOut\n\r")));
								bRet = FALSE;
							}
						}
					}
				}
				else
				{
					Disp_set_framebuffer(pArg->dwWinNum, pArg->dwFrameBuffer);
				}
			}

			// Backup for PM
			memcpy(&pPMCtxt->tFIMDWinFBuffer[pArg->dwWinNum], pArg, sizeof(SVEARG_FIMD_WIN_FRAMEBUFFER));
			pPMCtxt->bFIMDWinFBuffer[pArg->dwWinNum] = TRUE;

			break;
		}

		case SVE_FIMD_SET_WINDOW_ENABLE:
		{
			DWORD dwWinNum;

			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_ENABLE\n\r")));

			dwWinNum = *((DWORD *)pBufIn);

			if (pPMCtxt->bPowerOn)
			{
				Disp_window_onfoff(dwWinNum, DISP_WINDOW_ON);
				pCtxt->bWindowEnable[dwWinNum] = TRUE;
			}

			// Backup for PM
			pPMCtxt->bFIMDWinEnable[dwWinNum] = TRUE;

			break;
		}

		case SVE_FIMD_SET_WINDOW_DISABLE:
		{
			DWORD dwWinNum;

			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_DISABLE\n\r")));

            dwWinNum = *((DWORD *)pBufIn);

			if (pPMCtxt->bPowerOn)
			{
				
				Disp_window_onfoff(dwWinNum, DISP_WINDOW_OFF);
				pCtxt->bWindowEnable[dwWinNum] = FALSE;
			}

			// Backup for PM
			pPMCtxt->bFIMDWinEnable[dwWinNum] = FALSE;

			break;
		}

		case SVE_FIMD_SET_WINDOW_BLEND_COLORKEY:
		{
			SVEARG_FIMD_WIN_COLORKEY *pArg;

			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_BLEND_COLORKEY\n\r")));

			pArg = (SVEARG_FIMD_WIN_COLORKEY *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				Disp_set_color_key(pArg->dwWinNum, pArg->bOnOff, pArg->dwDirection, pArg->dwColorKey, pArg->dwCompareKey);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tFIMDColorKey[pArg->dwWinNum], pArg, sizeof(SVEARG_FIMD_WIN_COLORKEY));
			pPMCtxt->bFIMDColorKey[pArg->dwWinNum] = TRUE;

			break;
		}

		case SVE_FIMD_SET_WINDOW_BLEND_ALPHA:
		{
			SVEARG_FIMD_WIN_ALPHA *pArg;

			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_BLEND_ALPHA\n\r")));

			pArg = (SVEARG_FIMD_WIN_ALPHA *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				Disp_set_alpha_blending(pArg->dwWinNum, pArg->dwMethod, pArg->dwAlpha0, pArg->dwAlpha1);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tFIMDAlpha[pArg->dwWinNum], pArg, sizeof(SVEARG_FIMD_WIN_ALPHA));
			pPMCtxt->bFIMDAlpha[pArg->dwWinNum] = TRUE;

			break;
		}

		case SVE_FIMD_WAIT_FRAME_INTERRUPT:
		{
			if (pPMCtxt->bPowerOn && pCtxt->bVideoEnable)
			{
				if (WAIT_TIMEOUT == SVE_wait_disp_cmd_done())
				{
					VDE_ERR((_T("[VDE:ERR] SVE_DispCon_API_Proc() : SVE_FIMD_WAIT_FRAME_INTERRUPT : SVE_wait_disp_cmd_done() TimeOut\n\r")));
					bRet = FALSE;
				}
			}
			else
			{
				VDE_INF((_T("[VDE:INF] SVE_DispCon_API_Proc() : SVE_FIMD_WAIT_FRAME_INTERRUPT : Video Output is Disabled or Power Off Mode\n\r")));
			}
                        break;
		}

		case SVE_FIMD_GET_OUTPUT_STATUS:
		{
			SVEARG_FIMD_OUTPUT_STAT *pArg;

			VDE_MSG((_T("[VDE] SVE_DispCon_API_Proc() : SVE_FIMD_GET_OUTPUT_STATUS\n\r")));

			pArg = (SVEARG_FIMD_OUTPUT_STAT *)pBufOut;

			if (pPMCtxt->bPowerOn)
			{
				pArg->dwLineCnt = Disp_get_line_count();
				pArg->dwVerticalStatus = Disp_get_vertical_status();
				pArg->dwHorizontalStatus = Disp_get_horizontal_status();
				pArg->bENVID = Disp_get_envid_status();
			}
			else
			{
				// Return Pseudo Values in Power Off State
				pArg->dwLineCnt = 0;
				pArg->dwVerticalStatus = 0;
				pArg->dwHorizontalStatus = 0;
				pArg->bENVID = FALSE;
			}

			*pdwActualOut = sizeof(SVEARG_FIMD_OUTPUT_STAT);

			break;
		}

		case SVE_FIMD_SET_WINDOW_COLORMAP:
			VDE_ERR((_T("[VDE:ERR] SVE_DispCon_API_Proc() : SVE_FIMD_GET_OUTPUT_STATUS : Not Implemented Yet...\n\r")));
			bRet = FALSE;
			break;
		case SVE_FIMD_GET_WINDOW_STATUS:
			VDE_ERR((_T("[VDE:ERR] SVE_DispCon_API_Proc() : SVE_FIMD_GET_WINDOW_STATUS : Not Implemented Yet...\n\r")));
			bRet = FALSE;
			break;
		case SVE_FIMD_SET_WINDOW_BLEND_DISABLE:
			VDE_ERR((_T("[VDE:ERR] SVE_DispCon_API_Proc() : SVE_FIMD_SET_WINDOW_BLEND_DISABLE : Not Implemented Yet...\n\r")));
			bRet = FALSE;
			break;
		}

CleanUp:

	VDE_MSG((_T("[VDE] --SVE_DispCon_API_Proc()\n\r")));

	return bRet;
}


