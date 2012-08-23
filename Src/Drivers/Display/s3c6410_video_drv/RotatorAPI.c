#include <bsp.h>
#include "SVEngine.h"

BOOL SVE_Rotator_API_Proc(
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

	//VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\r\n"),
	//			hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	VDE_MSG((_T("[VDE] ++SVE_Rotator_API_Proc()\n\r")));

	pCtxt = SVE_get_context();
	pPMCtxt = SVE_get_power_context();

	//-------------------------------------------------------------
	// Check OpenContext of Caller have the right to access to H/W Resource
	//-------------------------------------------------------------
	switch(dwCode)
	{
	case SVE_ROTATOR_SET_OPERATION_PARAM:
	case SVE_ROTATOR_SET_SOURCE_BUFFER:
	case SVE_ROTATOR_SET_DESTINATION_BUFFER:
	case SVE_ROTATOR_SET_OPERATION_START:
	case SVE_ROTATOR_SET_OPERATION_STOP:
	case SVE_ROTATOR_WAIT_OPERATION_DONE:
	case SVE_ROTATOR_GET_STATUS:
		bRet = SVE_resource_compare_Rotator(hOpenContext);
		break;
	}

	if (!bRet)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_Rotator_API_Proc(0x%08x) : No Right to Access to H/W Resource\r\n"), dwCode));
		goto CleanUp;
	}

	//--------------------------------
	// Processing IOCTL for Image Rotator
	//--------------------------------

	switch(dwCode)
	{
		case SVE_ROTATOR_SET_OPERATION_PARAM:
		{
			SVEARG_ROTATOR_PARAMETER *pArg;

			VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_PARAM\n\r")));

			pArg = (SVEARG_ROTATOR_PARAMETER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (pCtxt->bRotatorBusy)		// Check Rotator Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_PARAM : Image Rotator is Running\n\r")));
					bRet = FALSE;
					break;
				}

				Rotator_initialize(pArg->dwImgFormat, pArg->dwOpType, pArg->dwSrcWidth, pArg->dwSrcHeight);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tRotatorParam, pArg, sizeof(SVEARG_ROTATOR_PARAMETER));
			//pPMCtxt->bRotatorParam = TRUE; // Move to ResourceAPI.c

			break;
		}

		case SVE_ROTATOR_SET_SOURCE_BUFFER:
		{
			SVEARG_ROTATOR_BUFFER *pArg;

			VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_SOURCE_BUFFER\n\r")));

			pArg = (SVEARG_ROTATOR_BUFFER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (pCtxt->bRotatorBusy)		// Check Rotator Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_SOURCE_BUFFER : Image Rotator is Running\n\r")));
					bRet = FALSE;
					break;
				}

				Rotator_set_source_buffer(pArg->dwBufferRGBY, pArg->dwBufferCb, pArg->dwBufferCr);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tRotatorSrcBuffer, pArg, sizeof(SVEARG_ROTATOR_BUFFER));
			pPMCtxt->bRotatorSrcBuffer = TRUE;

			break;
		}

		case SVE_ROTATOR_SET_DESTINATION_BUFFER:
		{
			SVEARG_ROTATOR_BUFFER *pArg;

			VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_DESTINATION_BUFFER\n\r")));

			pArg = (SVEARG_ROTATOR_BUFFER *)pBufIn;

			if (pPMCtxt->bPowerOn)
			{
				if (pCtxt->bRotatorBusy)		// Check Rotator Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_DESTINATION_BUFFER : Image Rotator is Running\n\r")));
					bRet = FALSE;
					break;
				}

				Rotator_set_destination_buffer(pArg->dwBufferRGBY, pArg->dwBufferCb, pArg->dwBufferCr);
			}

			// Backup for PM
			memcpy(&pPMCtxt->tRotatorDstBuffer, pArg, sizeof(SVEARG_ROTATOR_BUFFER));
			pPMCtxt->bRotatorDstBuffer = TRUE;

			break;
		}

		case SVE_ROTATOR_SET_OPERATION_START:
		{
			VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_START\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				if (pCtxt->bRotatorBusy)		// Check Rotator Running
				{
					VDE_ERR((_T("[VDE:ERR] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_START : Image Rotator is Running\n\r")));
					bRet = FALSE;
					break;
				}

				// Image Rotator Status Flag
				pCtxt->bRotatorBusy = TRUE;

				Rotator_enable_interrupt();
				Rotator_start();
			}

			// Backup for PM
			pPMCtxt->bRotatorStart = TRUE;

			break;
		}

		case SVE_ROTATOR_SET_OPERATION_STOP:
		{
			VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_STOP\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				if (pCtxt->bRotatorBusy)		// Check Rotator Running
				{
					if (WAIT_TIMEOUT == SVE_wait_rotator_finish())
					{
						if (pCtxt->bRotatorBusy == FALSE)
						{
							// Time Out, But Rotator Finished...
							VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_STOP : SVE_wait_rotator_finish() TimeOut, But Rotator Finished\n\r")));
						}
						else
						{
							VDE_ERR((_T("[VDE:ERR] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_STOP : SVE_wait_rotator_finish() TimeOut\n\r")));
							bRet = FALSE;
						}
					}
				}
				else
				{
					VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_START : Image Rotator Already Stopped\n\r")));
				}
			}

			// Backup for PM
			pPMCtxt->bRotatorStart = FALSE;

			break;
		}

		case SVE_ROTATOR_WAIT_OPERATION_DONE:	// Actually Same with SVE_ROTATOR_SET_OPERATION_STOP API
		{
			VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_WAIT_OPERATION_DONE\n\r")));

			if (pPMCtxt->bPowerOn)
			{
				if (pCtxt->bRotatorBusy)		// Check Rotator Running
				{
					if (WAIT_TIMEOUT == SVE_wait_rotator_finish())
					{
						if (pCtxt->bRotatorBusy == FALSE)
						{
							// Time Out, But Rotator Finished...
							VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_STOP : SVE_wait_rotator_finish() TimeOut, But Rotator Finished\n\r")));
						}
						else
						{
							VDE_ERR((_T("[VDE:ERR] SVE_Rotator_API_Proc() : SVE_ROTATOR_SET_OPERATION_STOP : SVE_wait_rotator_finish() TimeOut\n\r")));
							bRet = FALSE;
						}
					}
				}
				else
				{
					VDE_MSG((_T("[VDE] SVE_Rotator_API_Proc() : SVE_ROTATOR_WAIT_OPERATION_DONE : Image Rotator Already Stopped\n\r")));
				}
			}

			// Backup for PM
			pPMCtxt->bRotatorStart = FALSE;

			break;
		}

		case SVE_ROTATOR_GET_STATUS:
			VDE_ERR((_T("[VDE:ERR] SVE_Rotator_API_Proc() : SVE_ROTATOR_GET_STATUS : Not Implemented Yet...\n\r")));
			bRet = FALSE;
			break;
	}

CleanUp:

	VDE_MSG((_T("[VDE] --SVE_Rotator_API_Proc()\n\r")));

	return bRet;
}


