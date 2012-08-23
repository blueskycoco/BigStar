#include <bsp.h>
#include "SVEngine.h"

BOOL SVE_TVScaler_API_Proc(
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

	//VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\r\n"),
	//			hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	VDE_MSG((_T("[VDE] ++SVE_TVScaler_API_Proc()\n\r")));

	pCtxt = SVE_get_context();
	pPMCtxt = SVE_get_power_context();

	//-------------------------------------------------------------
	// Check OpenContext of Caller have the right to access to H/W Resource
	//-------------------------------------------------------------
	switch(dwCode)
	{
	case SVE_TVSC_SET_PROCESSING_PARAM:
	case SVE_TVSC_SET_SOURCE_BUFFER:
	case SVE_TVSC_SET_NEXT_SOURCE_BUFFER:
	case SVE_TVSC_SET_DESTINATION_BUFFER:
	case SVE_TVSC_SET_NEXT_DESTINATION_BUFFER:
	case SVE_TVSC_SET_PROCESSING_START:
	case SVE_TVSC_SET_PROCESSING_STOP:
	case SVE_TVSC_WAIT_PROCESSING_DONE:
	case SVE_TVSC_GET_PROCESSING_STATUS:
		bRet = SVE_resource_compare_TVScaler_TVEncoder(hOpenContext);
		break;
	}

	if (!bRet)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_TVScaler_API_Proc(0x%08x) : No Right to Access to H/W Resource\r\n"), dwCode));
		goto CleanUp;
	}

	//--------------------------------
	// Processing IOCTL for TV Scaler
	//--------------------------------

	switch(dwCode)
	{
		case SVE_TVSC_SET_PROCESSING_PARAM:
		{
			SVEARG_TVSC_PARAMETER *pArg;

			VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_PROCESSING_PARAM\n\r")));

			pArg = (SVEARG_TVSC_PARAMETER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (TVSC_get_processing_state() == TVSC_BUSY)		// TV Scaler Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_PROCESSING_START : TV Scaler is Running\n\r")));
					bRet = FALSE;
					break;
				}

				TVSC_initialize(pArg->dwOpMode, pArg->dwScanMode,
							pArg->dwSrcType, pArg->dwSrcBaseWidth, pArg->dwSrcBaseHeight, pArg->dwSrcWidth, pArg->dwSrcHeight, pArg->dwSrcOffsetX, pArg->dwSrcOffsetY,
							pArg->dwDstType, pArg->dwDstBaseWidth, pArg->dwDstBaseHeight, pArg->dwDstWidth, pArg->dwDstHeight, pArg->dwDstOffsetX, pArg->dwDstOffsetY);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tTVSCParam, pArg, sizeof(SVEARG_TVSC_PARAMETER));
			//pPMCtxt->bTVSCParam = TRUE; // Move to ResourceAPI.c

			break;
		}

		case SVE_TVSC_SET_SOURCE_BUFFER:
		{
			SVEARG_TVSC_BUFFER *pArg;

			VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_SOURCE_BUFFER\n\r")));

			pArg = (SVEARG_TVSC_BUFFER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (TVSC_get_processing_state() == TVSC_BUSY)		// TV Scaler Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_SOURCE_BUFFER : TV Scaler is Running\n\r")));
					bRet = FALSE;
					break;
				}

				TVSC_set_source_buffer(pArg->dwBufferRGBY, pArg->dwBufferCb, pArg->dwBufferCr);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tTVSCSrcBuffer, pArg, sizeof(SVEARG_TVSC_BUFFER));
			pPMCtxt->bTVSCSrcBuffer = TRUE;

			break;
		}

		case SVE_TVSC_SET_NEXT_SOURCE_BUFFER:
		{
			SVEARG_TVSC_BUFFER *pArg;

			VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_NEXT_SOURCE_BUFFER\n\r")));

			pArg = (SVEARG_TVSC_BUFFER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (TVSC_get_processing_state() == TVSC_BUSY)		// TV Scaler Running
				{
					BOOL bRetry = TRUE;

					while(bRetry)
					{
						EnterCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);

						if (pCtxt->TVSCCmdCtxt.bCmdSetSrcBuffer == FALSE)
						{
							// We can change buffer address here...
							// But, after interrupt occurs, It can take effect..
							TVSC_set_next_source_buffer(pArg->dwBufferRGBY, pArg->dwBufferCb, pArg->dwBufferCr);

							pCtxt->TVSCCmdCtxt.bCmdSetSrcBuffer = TRUE;
							bRetry = FALSE;
						}

						LeaveCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);

						if (bRetry || pArg->bWaitForVSync)
						{
							if (WAIT_TIMEOUT == SVE_wait_tvsc_cmd_done())
							{
								VDE_ERR((_T("[VDE:ERR] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_NEXT_SOURCE_BUFFER : SVE_wait_tvsc_cmd_done() TimeOut\n\r")));
								bRetry = FALSE;
								bRet = FALSE;
							}
						}
					}
				}
				else
				{
					TVSC_set_next_source_buffer(pArg->dwBufferRGBY, pArg->dwBufferCb, pArg->dwBufferCr);
				}
			}

			// Backup for PM
			memcpy(&pPMCtxt->tTVSCSrcBuffer, pArg, sizeof(SVEARG_TVSC_BUFFER));
			pPMCtxt->bTVSCSrcBuffer = TRUE;

			break;
		}

		case SVE_TVSC_SET_DESTINATION_BUFFER:
		{
			SVEARG_TVSC_BUFFER *pArg;

			VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_DESTINATION_BUFFER\n\r")));

			pArg = (SVEARG_TVSC_BUFFER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (TVSC_get_processing_state() == TVSC_BUSY)		// TV Scaler Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_DESTINATION_BUFFER : TV Scaler is Running\n\r")));
					bRet = FALSE;
					break;
				}

				TVSC_set_destination_buffer(pArg->dwBufferRGBY, pArg->dwBufferCb, pArg->dwBufferCr);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tTVSCDstBuffer, pArg, sizeof(SVEARG_TVSC_BUFFER));
			pPMCtxt->bTVSCDstBuffer = TRUE;

			break;
		}

		case SVE_TVSC_SET_NEXT_DESTINATION_BUFFER:
		{
			SVEARG_TVSC_BUFFER *pArg;

			VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_NEXT_DESTINATION_BUFFER\n\r")));

			pArg = (SVEARG_TVSC_BUFFER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (TVSC_get_processing_state() == TVSC_BUSY)		// TV Scaler Running
				{
					BOOL bRetry = TRUE;

					while(bRetry)
					{
						EnterCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);

						if (pCtxt->TVSCCmdCtxt.bCmdSetDstBuffer == FALSE)
						{
							// We can change buffer address here...
							// But, after interrupt occurs, It can take effect..
							TVSC_set_next_destination_buffer(pArg->dwBufferRGBY, pArg->dwBufferCb, pArg->dwBufferCr);

							pCtxt->TVSCCmdCtxt.bCmdSetDstBuffer = TRUE;
							bRetry = FALSE;
						}

						LeaveCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);

						if (bRetry || pArg->bWaitForVSync)
						{
							if (WAIT_TIMEOUT == SVE_wait_tvsc_cmd_done())
							{
								VDE_ERR((_T("[VDE:ERR] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_NEXT_DESTINATION_BUFFER : SVE_wait_tvsc_cmd_done() TimeOut\n\r")));
								bRetry = FALSE;
								bRet = FALSE;
							}
						}
					}
				}
				else
				{
					TVSC_set_next_destination_buffer(pArg->dwBufferRGBY, pArg->dwBufferCb, pArg->dwBufferCr);
				}
			}

			// Backup for PM
			memcpy(&pPMCtxt->tTVSCDstBuffer, pArg, sizeof(SVEARG_TVSC_BUFFER));
			pPMCtxt->bTVSCDstBuffer = TRUE;

			break;
		}

		case SVE_TVSC_SET_PROCESSING_START:
		{
			VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_PROCESSING_START\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				if (TVSC_get_processing_state() == TVSC_BUSY)		// TV Scaler Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_PROCESSING_START : TV Scaler is Running\n\r")));
					bRet = FALSE;
					break;
				}

				TVSC_enable_interrupt();
				TVSC_processing_start();
			}

			// Backup for PM
			pPMCtxt->bTVSCStart = TRUE;

			break;
		}

		case SVE_TVSC_SET_PROCESSING_STOP:
		{
			VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_PROCESSING_STOP\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				TVSC_autoload_disable();

				if (TVSC_get_processing_state() == TVSC_BUSY)	// TV Scaler Running
				{
					// Wait for Current Frame Finished
					if (WAIT_TIMEOUT == SVE_wait_tvsc_cmd_done())
					{
						if (TVSC_get_processing_state() == TVSC_IDLE)
						{
							// Time Out, But TV Scaler Finished
							VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_PROCESSING_STOP : SVE_wait_tvsc_cmd_done() TimeOut, But TV Scaler Finished\n\r")));
						}
						else
						{
							VDE_INF((_T("[VDE:INF] SVE_TVScaler_API_Proc() : SVE_TVSC_SET_PROCESSING_STOP : SVE_wait_tvsc_cmd_done() TimeOut\n\r")));
							TVSC_processing_stop();
						}
					}
				}
			}

			// Backup for PM
			pPMCtxt->bTVSCStart = FALSE;

			break;
		}

		case SVE_TVSC_WAIT_PROCESSING_DONE:
		{
			VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_WAIT_PROCESSING_DONE\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				if (TVSC_get_processing_state() == TVSC_BUSY)		// TV Scaler Running
				{
					if (WAIT_TIMEOUT == SVE_wait_tvsc_cmd_done())
					{
						if (TVSC_get_processing_state() == TVSC_IDLE)
						{
							// Time Out, But TV Scaler Finished
							VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_WAIT_PROCESSING_DONE : SVE_wait_tvsc_cmd_done() TimeOut, But TV Scaler Finished\n\r")));
						}
						else
						{
							VDE_INF((_T("[VDE:INF] SVE_TVScaler_API_Proc() : SVE_TVSC_WAIT_PROCESSING_DONE : SVE_wait_tvsc_cmd_done() TimeOut\n\r")));
							bRet = FALSE;
						}
					}
				}
			}

			// Backup for PM
			pPMCtxt->bTVSCStart = FALSE;

			break;
		}

		case SVE_TVSC_GET_PROCESSING_STATUS:
		{
			DWORD *pArg;

			VDE_MSG((_T("[VDE] SVE_TVScaler_API_Proc() : SVE_TVSC_GET_PROCESSING_STATUS\n\r")));

			pArg = (DWORD *)pBufOut;

			if (pPMCtxt->bPowerOn)
			{
				*pArg = TVSC_get_processing_state();
			}
			else
			{
				*pArg = TVSC_IDLE;
			}

			*pdwActualOut = sizeof(DWORD);

			break;
		}
	}

CleanUp:

	VDE_MSG((_T("[VDE] --SVE_TVScaler_API_Proc()\n\r")));

	return bRet;
}


