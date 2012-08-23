#include <bsp.h>
#include "SVEngine.h"

BOOL SVE_TVEncoder_API_Proc(
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
	SVEnginePowerContext *pPMCtxt;
	BOOL bRet = TRUE;

	//VDE_MSG((_T("[VDE] SVE_TVEncoder_API_Proc(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\r\n"),
	//			hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	VDE_MSG((_T("[VDE] ++SVE_TVEncoder_API_Proc()\n\r")));

	//pCtxt = SVE_get_context();
	pPMCtxt = SVE_get_power_context();

	//-------------------------------------------------------------
	// Check OpenContext of Caller have the right to access to H/W Resource
	//-------------------------------------------------------------
	switch(dwCode)
	{
	case SVE_TVENC_SET_INTERFACE_PARAM:
	case SVE_TVENC_SET_ENCODER_ON:
	case SVE_TVENC_SET_ENCODER_OFF:
	case SVE_TVENC_GET_INTERFACE_STATUS:
		bRet = SVE_resource_compare_TVScaler_TVEncoder(hOpenContext);
		break;
	}

	if (!bRet)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_TVEncoder_API_Proc(0x%08x) : No Right to Access to H/W Resource\r\n"), dwCode));
		goto CleanUp;
	}

	//--------------------------------
	// Processing IOCTL for TV Encoder
	//--------------------------------

	switch(dwCode)
	{
		case SVE_TVENC_SET_INTERFACE_PARAM:
		{
			SVEARG_TVENC_PARAMETER *pArg;

			VDE_MSG((_T("[VDE] SVE_TVEncoder_API_Proc() : SVE_TVENC_SET_INTERFACE_PARAM\n\r")));

			pArg = (SVEARG_TVENC_PARAMETER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (TVEnc_get_output_state() == TVENC_ENCODER_ON)		// TV Encoder Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_TVEncoder_API_Proc() : SVE_TVENC_SET_INTERFACE_PARAM : TV Encoder is Running\n\r")));
					bRet = FALSE;
					break;
				}

				TVEnc_initialize(pArg->dwOutputType, pArg->dwOutputStandard, pArg->dwMVisionPattern, pArg->dwSrcWidth, pArg->dwSrcHeight);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tTVEncParam, pArg, sizeof(SVEARG_TVENC_PARAMETER));
			//pPMCtxt->bTVEncParam = TRUE; // Move to ResourceAPI.c

			break;
		}

		case SVE_TVENC_SET_ENCODER_ON:	// Start TV Scaler & Enable TV Encoder
		{
			VDE_MSG((_T("[VDE] SVE_TVEncoder_API_Proc() : SVE_TVENC_SET_ENCODER_ON\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				//------------------
				// Enable TV Encoder
				//------------------
				if (TVEnc_get_output_state() == TVENC_ENCODER_ON)		// TV Encoder Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_TVEncoder_API_Proc() : SVE_TVENC_SET_ENCODER_ON : TV Encoder is Running\n\r")));
					//bRet = FALSE;	// Treat this as Non-Error
				}
				else
				{
					//TVEnc_disable_interrupt();
					TVEnc_output_onoff(TVENC_ENCODER_ON);
				}

				//--------------
				// Start TV Scaler
				//--------------
				if (TVSC_get_processing_state() == TVSC_BUSY)		// TV Scaler Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_TVEncoder_API_Proc() : SVE_TVENC_SET_ENCODER_ON : TV Scaler is Running\n\r")));
					bRet = FALSE;
					break;
				}

				TVSC_enable_interrupt();
				TVSC_processing_start();
			}

			// Backup for PM
			pPMCtxt->bTVEncEnable = TRUE;

			break;
		}

		case SVE_TVENC_SET_ENCODER_OFF:	// Stop TV Scaler & Disable TV Encoder
		{
			VDE_MSG((_T("[VDE] SVE_TVEncoder_API_Proc() : SVE_TVENC_SET_ENCODER_OFF\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				//----------------
				// Stop TV Scaler
				//----------------
				TVSC_autoload_disable();
				if (TVSC_get_processing_state() == TVSC_BUSY)	// TV Scaler Running
				{
					// Wait for Current Frame Finished
					if (WAIT_TIMEOUT == SVE_wait_tvsc_cmd_done())
					{
						if (TVSC_get_processing_state() == TVSC_IDLE)
						{
							// Time Out, But TV Scaler Finished
							VDE_MSG((_T("[VDE] SVE_TVEncoder_API_Proc() : SVE_TVENC_SET_ENCODER_OFF : SVE_wait_tvsc_cmd_done() TimeOut, But TV Scaler Finished\n\r")));
						}
						else
						{
							VDE_INF((_T("[VDE:INF] SVE_TVEncoder_API_Proc() : SVE_TVENC_SET_ENCODER_OFF : SVE_wait_tvsc_cmd_done() TimeOut\n\r")));
							TVSC_processing_stop();
						}
					}
				}

				//------------------
				// Disable TV Encoder
				//------------------
				if (TVEnc_get_output_state() == TVENC_ENCODER_OFF)		// TV Encoder Not Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_TVEncoder_API_Proc() : SVE_TVENC_SET_ENCODER_OFF : TV Encoder is Not Running\n\r")));
					//bRet = FALSE;	// Treat this as Non-Error
				}
				else
				{
					TVEnc_output_onoff(TVENC_ENCODER_OFF);
				}
			}

			// Backup for PM
			pPMCtxt->bTVEncEnable = FALSE;

			break;
		}

		case SVE_TVENC_GET_INTERFACE_STATUS:
			VDE_ERR((_T("[VDE:ERR] SVE_TVEncoder_API_Proc() : SVE_TVENC_GET_INTERFACE_STATUS : Not Implemented Yet...\n\r")));
			bRet = FALSE;
			break;
	}

CleanUp:

	VDE_MSG((_T("[VDE] --SVE_TVEncoder_API_Proc()\n\r")));

	return bRet;
}


