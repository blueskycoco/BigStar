#include <bsp.h>
#include "SVEngine.h"

BOOL SVE_initialize_interrupt(void)
{
	SVEngineContext *pCtxt;
	DWORD dwIRQ;

	VDE_MSG((_T("[VDE] ++SVE_initialize_interrupt()\n\r")));

	pCtxt = SVE_get_context();

	// Interrupt for Display Controller (Vsync Interrupt)
	dwIRQ = IRQ_LCD1_FRAME;
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &dwIRQ, sizeof(DWORD), &(pCtxt->dwSysIntrDisp), sizeof(DWORD), NULL))
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : IOCTL_HAL_REQUEST_SYSINTR dwSysIntrDisp Failed\n\r")));
		pCtxt->dwSysIntrDisp = SYSINTR_UNDEFINED;
		goto CleanUp;
	}

	pCtxt->hInterruptDisp = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(NULL == pCtxt->hInterruptDisp)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : CreateEvent() hInterruptDisp Failed \n\r")));
		goto CleanUp;
	}

	if (!(InterruptInitialize(pCtxt->dwSysIntrDisp, pCtxt->hInterruptDisp, 0, 0)))
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : InterruptInitialize() hInterruptDisp Failed \n\r")));
		goto CleanUp;
	}

	// Interrupt for Post Processor
	dwIRQ = IRQ_POST;
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &dwIRQ, sizeof(DWORD), &(pCtxt->dwSysIntrPost), sizeof(DWORD), NULL))
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : IOCTL_HAL_REQUEST_SYSINTR dwSysIntrPost Failed\n\r")));
		pCtxt->dwSysIntrPost = SYSINTR_UNDEFINED;
		goto CleanUp;
	}

	pCtxt->hInterruptPost = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(NULL == pCtxt->hInterruptPost)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : CreateEvent() hInterruptPost Failed \n\r")));
		goto CleanUp;
	}

	if (!(InterruptInitialize(pCtxt->dwSysIntrPost, pCtxt->hInterruptPost, 0, 0)))
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : InterruptInitialize() hInterruptPost Failed \n\r")));
		goto CleanUp;
	}

	// Interrupt for TV Scaler
	dwIRQ = IRQ_TVSCALER;
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &dwIRQ, sizeof(DWORD), &(pCtxt->dwSysIntrTVSC), sizeof(DWORD), NULL))
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : IOCTL_HAL_REQUEST_SYSINTR dwSysIntrTVSC Failed\n\r")));
		pCtxt->dwSysIntrTVSC = SYSINTR_UNDEFINED;
		goto CleanUp;
	}

	pCtxt->hInterruptTVSC = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(NULL == pCtxt->hInterruptTVSC)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : CreateEvent() hInterruptTVSC Failed \n\r")));
		goto CleanUp;
	}

	if (!(InterruptInitialize(pCtxt->dwSysIntrTVSC, pCtxt->hInterruptTVSC, 0, 0)))
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : InterruptInitialize() hInterruptTVSC Failed \n\r")));
		goto CleanUp;
	}

	// Interrupt for Rotator
	dwIRQ = IRQ_ROTATOR;
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &dwIRQ, sizeof(DWORD), &(pCtxt->dwSysIntrRotator), sizeof(DWORD), NULL))
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : IOCTL_HAL_REQUEST_SYSINTR dwSysIntrRotator Failed\n\r")));
		pCtxt->dwSysIntrRotator = SYSINTR_UNDEFINED;
		goto CleanUp;
	}

	pCtxt->hInterruptRotator = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(NULL == pCtxt->hInterruptRotator)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : CreateEvent() hInterruptRotator Failed \n\r")));
		goto CleanUp;
	}

	if (!(InterruptInitialize(pCtxt->dwSysIntrRotator, pCtxt->hInterruptRotator, 0, 0)))
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_interrupt() : InterruptInitialize() hInterruptRotator Failed \n\r")));
		goto CleanUp;
	}

	VDE_MSG((_T("[VDE] --SVE_initialize_interrupt()\n\r")));

	return TRUE;

CleanUp:

	VDE_ERR((_T("[VDE:ERR] --SVE_initialize_interrupt() : Failed\n\r")));

	return FALSE;
}

void SVE_deinitialize_interrupt(void)
{
	SVEngineContext *pCtxt;

	VDE_MSG((_T("[VDE] ++SVE_deinitialize_interrupt()\n\r")));

	pCtxt = SVE_get_context();

	if (pCtxt->dwSysIntrDisp != SYSINTR_UNDEFINED)
	{
		InterruptDisable(pCtxt->dwSysIntrDisp);
	}

	if (pCtxt->hInterruptDisp != NULL)
	{
		CloseHandle(pCtxt->hInterruptDisp);
		pCtxt->hInterruptDisp = NULL;
	}

	if (pCtxt->dwSysIntrDisp != SYSINTR_UNDEFINED)
	{
		KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &(pCtxt->dwSysIntrDisp), sizeof(DWORD), NULL, 0, NULL);
		pCtxt->dwSysIntrDisp = SYSINTR_UNDEFINED;
	}

	if (pCtxt->dwSysIntrPost != SYSINTR_UNDEFINED)
	{
		InterruptDisable(pCtxt->dwSysIntrPost);
	}

	if (pCtxt->hInterruptPost != NULL)
	{
		CloseHandle(pCtxt->hInterruptPost);
		pCtxt->hInterruptPost = NULL;
	}

	if (pCtxt->dwSysIntrPost != SYSINTR_UNDEFINED)
	{
		KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &(pCtxt->dwSysIntrPost), sizeof(DWORD), NULL, 0, NULL);
		pCtxt->dwSysIntrPost = SYSINTR_UNDEFINED;
	}

	if (pCtxt->dwSysIntrTVSC != SYSINTR_UNDEFINED)
	{
		InterruptDisable(pCtxt->dwSysIntrTVSC);
	}

	if (pCtxt->hInterruptTVSC != NULL)
	{
		CloseHandle(pCtxt->hInterruptTVSC);
		pCtxt->hInterruptTVSC = NULL;
	}

	if (pCtxt->dwSysIntrTVSC != SYSINTR_UNDEFINED)
	{
		KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &(pCtxt->dwSysIntrTVSC), sizeof(DWORD), NULL, 0, NULL);
		pCtxt->dwSysIntrTVSC = SYSINTR_UNDEFINED;
	}

	if (pCtxt->dwSysIntrRotator != SYSINTR_UNDEFINED)
	{
		InterruptDisable(pCtxt->dwSysIntrRotator);
	}

	if (pCtxt->hInterruptRotator != NULL)
	{
		CloseHandle(pCtxt->hInterruptRotator);
		pCtxt->hInterruptRotator = NULL;
	}

	if (pCtxt->dwSysIntrRotator != SYSINTR_UNDEFINED)
	{
		KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &(pCtxt->dwSysIntrRotator), sizeof(DWORD), NULL, 0, NULL);
		pCtxt->dwSysIntrRotator = SYSINTR_UNDEFINED;
	}

	VDE_MSG((_T("[VDE] --SVE_deinitialize_interrupt()\n\r")));
}

BOOL SVE_initialize_thread(void)
{
	SVEngineContext *pCtxt;

	VDE_MSG((_T("[VDE] ++SVE_initialize_thread()\n\r")));

	pCtxt = SVE_get_context();

	pCtxt->hDisplayIST = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SVE_DisplayIntrThread, NULL, 0, NULL);
	if (pCtxt->hDisplayIST == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_thread() : DisplayIST CreateThread() Fail\n\r")));
		goto CleanUp;
	}

	pCtxt->hPostIST = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SVE_PostIntrThread, NULL, 0, NULL);
	if (pCtxt->hPostIST == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_thread() : hPostIST CreateThread() Fail\n\r")));
		goto CleanUp;
	}

	pCtxt->hTVScalerIST = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SVE_TVScalerIntrThread, NULL, 0, NULL);
	if (pCtxt->hTVScalerIST == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_thread() : hTVScalerIST CreateThread() Fail\n\r")));
		goto CleanUp;
	}

	pCtxt->hRotatorIST = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SVE_RotatorIntrThread, NULL, 0, NULL);
	if (pCtxt->hRotatorIST == NULL)
	{
		VDE_ERR((_T("[VDE:ERR] SVE_initialize_thread() : hRotatorIST CreateThread() Fail\n\r")));
		goto CleanUp;
	}

	VDE_MSG((_T("[VDE] --SVE_initialize_thread()\n\r")));

	return TRUE;

CleanUp:

	VDE_ERR((_T("[VDE:ERR] --SVE_initialize_thread() : Failed\n\r")));

	return FALSE;
}

void SVE_deinitialize_thread(void)
{
	// TODO: Delete IST
	// TODO: Delete IST
	// TODO: Delete IST
}

DWORD SVE_DisplayIntrThread(void)
{
	SVEngineContext *pCtxt;
	int iWinNum;

	VDE_MSG((_T("[VDE] ++SVE_DisplayIntrThread()\n\r")));

	pCtxt = SVE_get_context();

	CeSetThreadPriority(pCtxt->hDisplayIST, SVE_IST_PRIORITY);

	while(1)
	{
		WaitForSingleObject(pCtxt->hInterruptDisp, INFINITE);

#if 	0	// Local Path can start without Interrupt
		//---------------------------
		// Processing Local Path Command
		//---------------------------

		// Command : Local Path for Win0 Enable
		if (pCtxt->LocalPathCmdCtxt.bCmdSetWin0Enable)
		{
			Post_enable_interrupt();
			Post_processing_start();
			Disp_set_framebuffer(DISP_WIN0, IMAGE_FRAMEBUFFER_PA_START);	// Safe Frame Bufer Address for Local Path
			Disp_window_onfoff(DISP_WIN0, DISP_WINDOW_ON);

			//EnterCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
			pCtxt->LocalPathCmdCtxt.bCmdSetWin0Enable = FALSE;
			//LeaveCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
		}

		// Command : Local Path for Win1 Enable
		if (pCtxt->LocalPathCmdCtxt.bCmdSetWin1Enable)
		{
			// TODO: Camera Preview Path enable
			Disp_window_onfoff(DISP_WIN1, DISP_WINDOW_ON);

			//EnterCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
			pCtxt->LocalPathCmdCtxt.bCmdSetWin1Enable = FALSE;
			//LeaveCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
		}

		// Command : Local Path for Win2 Enable
		if (pCtxt->LocalPathCmdCtxt.bCmdSetWin2Enable)
		{
			// TODO: Camera Codec Path enable
			Disp_window_onfoff(DISP_WIN2, DISP_WINDOW_ON);

			//EnterCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
			pCtxt->LocalPathCmdCtxt.bCmdSetWin2Enable = FALSE;
			//LeaveCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
		}
#endif

		//---------------------------
		// Processing Local Path Command
		//---------------------------

		// Command : Local Path for Win0 Disable
		if (pCtxt->LocalPathCmdCtxt.bCmdSetWin0Disable)
		{
			Disp_envid_onoff(DISP_ENVID_DIRECT_OFF);
			Post_processing_stop();
			Disp_window_onfoff(DISP_WIN0, DISP_WINDOW_OFF);
			Disp_envid_onoff(DISP_ENVID_ON);

			//EnterCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
			pCtxt->LocalPathCmdCtxt.bCmdSetWin0Disable = FALSE;
			//LeaveCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
		}

		//-----------------------------------
		// Processing Display Controller Command
		//-----------------------------------
		for (iWinNum = DISP_WIN0; iWinNum < DISP_WIN4; iWinNum++)
		{
			// Command : Set Frame Buffer
			if (pCtxt->DispCmdCtxt[iWinNum].bCmdSetBuffer)
			{
				Disp_set_framebuffer(iWinNum, pCtxt->DispCmdCtxt[iWinNum].dwBuffer);

				//EnterCriticalSection(&pCtxt->DispCmdCtxt[iWinNum].csCmd);
				pCtxt->DispCmdCtxt[iWinNum].bCmdSetBuffer = FALSE;
				//LeaveCriticalSection(&pCtxt->DispCmdCtxt[iWinNum].csCmd);
			}

			// Command : Set Position
			if (pCtxt->DispCmdCtxt[iWinNum].bCmdSetPosition)
			{
				Disp_set_window_position(iWinNum, pCtxt->DispCmdCtxt[iWinNum].dwOffsetX, pCtxt->DispCmdCtxt[iWinNum].dwOffsetY);

				//EnterCriticalSection(&pCtxt->DispCmdCtxt[iWinNum].csCmd);
				pCtxt->DispCmdCtxt[iWinNum].bCmdSetPosition = FALSE;
				//LeaveCriticalSection(&pCtxt->DispCmdCtxt[iWinNum].csCmd);
			}
		}

		// Clear Interrupt Pending
		Disp_clear_interrupt_pending();

		InterruptDone(pCtxt->dwSysIntrDisp);

		PulseEvent(pCtxt->hDispCmdDone);

#if	0	// for debugging
		if ((pCtxt->pDispConReg->VIDCON1 & (0x3<<13)) == (0x2<<13))
		{
			VDE_ERR((_T("[VDE:ERR] SVE_DisplayIntrThread() : CMD Processing finished Too Late!!!\n\r")));
		}
#endif
	}

	VDE_MSG((_T("[VDE] --SVE_DisplayIntrThread()\n\r")));

	return 0;
}

DWORD SVE_PostIntrThread(void)
{
	SVEngineContext *pCtxt;

	VDE_MSG((_T("[VDE] ++SVE_PostIntrThread()\n\r")));

	pCtxt = SVE_get_context();

	CeSetThreadPriority(pCtxt->hPostIST, SVE_IST_PRIORITY);

	while(1)
	{
		WaitForSingleObject(pCtxt->hInterruptPost, INFINITE);
#if 0 // because of System Hang problem, move to Display Interrupt Thread
		//---------------------------
		// Processing Local Path Command
		//---------------------------

		// Command : Local Path for Win0 Disable
		if (pCtxt->LocalPathCmdCtxt.bCmdSetWin0Disable)
		{
			Post_processing_stop();
			Disp_window_onfoff(DISP_WIN0, DISP_WINDOW_OFF);

			//EnterCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
			pCtxt->LocalPathCmdCtxt.bCmdSetWin0Disable = FALSE;
			//LeaveCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
		}
#endif
#if	0
		// Command : Local Path for Win1 Disable
		if (pCtxt->LocalPathCmdCtxt.bCmdSetWin1Disable)
		{
			// TODO: Camera Preview Path disable
			Disp_window_onfoff(DISP_WIN1, DISP_WINDOW_OFF);

			//EnterCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
			pCtxt->LocalPathCmdCtxt.bCmdSetWin1Disable = FALSE;
			//LeaveCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
		}

		// Command : Local Path for Win2 Disable
		if (pCtxt->LocalPathCmdCtxt.bCmdSetWin2Disable)
		{
			// TODO: Camera Codec Path disable
			Disp_window_onfoff(DISP_WIN2, DISP_WINDODISP_WINDOW_OFFW_ON);

			//EnterCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
			pCtxt->LocalPathCmdCtxt.bCmdSetWin2Disable = FALSE;
			//LeaveCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
		}
#endif

		//-----------------------------------
		// Processing Post Processor Command
		//-----------------------------------

		// Command : Set Src Buffer
		if (pCtxt->PostCmdCtxt.bCmdSetSrcBuffer)
		{
			// At this time we can not update current address from next address
			// Because current address is updated at the start of interrupt
			// So, in the IST we can change only current address to make affect

			//EnterCriticalSection(&pCtxt->PostCmdCtxt.csCmd);
			pCtxt->PostCmdCtxt.bCmdSetSrcBuffer = FALSE;
			//LeaveCriticalSection(&pCtxt->PostCmdCtxt.csCmd);
		}

		// Command : Set Dst Buffer
		if (pCtxt->PostCmdCtxt.bCmdSetDstBuffer)
		{
			// At this time we can not update current address from next address
			// Because current address is updated at the start of interrupt
			// So, in the IST we can change only current address to make affect

			//EnterCriticalSection(&pCtxt->PostCmdCtxt.csCmd);
			pCtxt->PostCmdCtxt.bCmdSetDstBuffer = FALSE;
			//LeaveCriticalSection(&pCtxt->PostCmdCtxt.csCmd);
		}

		// Clear Interrupt Pending
		Post_clear_interrupt_pending();

		InterruptDone(pCtxt->dwSysIntrPost);

		PulseEvent(pCtxt->hPostCmdDone);
	}

	VDE_MSG((_T("[VDE] --SVE_PostIntrThread()\n\r")));

	return 0;
}

DWORD SVE_RotatorIntrThread(void)
{
	SVEngineContext *pCtxt;
	BOOL bIntPending;

	VDE_MSG((_T("[VDE] ++SVE_RotatorIntrThread()\n\r")));

	pCtxt = SVE_get_context();

	CeSetThreadPriority(pCtxt->hRotatorIST, SVE_IST_PRIORITY);

	while(1)
	{
		WaitForSingleObject(pCtxt->hInterruptRotator, INFINITE);

		// Clear Interrupt Pending
		bIntPending = Rotator_clear_interrupt_pending();
		if (!bIntPending)
		{
			VDE_ERR((_T("[VDE:ERR] SVE_RotatorIntrThread() : Interrupt Occurs, But Pending Bit was not Set\n\r")));
		}

		// Image Rotator Operation Finished
		pCtxt->bRotatorBusy = FALSE;

		InterruptDone(pCtxt->dwSysIntrRotator);

		PulseEvent(pCtxt->hRotatorFinish);
	}

	VDE_MSG((_T("[VDE] --SVE_RotatorIntrThread()\n\r")));

	return 0;
}

DWORD SVE_TVScalerIntrThread(void)
{
	SVEngineContext *pCtxt;

	VDE_MSG((_T("[VDE] ++SVE_TVScalerIntrThread()\n\r")));

	pCtxt = SVE_get_context();

	CeSetThreadPriority(pCtxt->hTVScalerIST, SVE_IST_PRIORITY);

	while(1)
	{
		WaitForSingleObject(pCtxt->hInterruptTVSC, INFINITE);

#if	0	// TV Out Command
		//---------------------------
		// Processing Local Path Command
		//---------------------------

		// Command : Local Path for Win0 Disable
		if (pCtxt->LocalPathCmdCtxt.bCmdSetWin0Disable)
		{
			Post_processing_stop();
			Disp_window_onfoff(DISP_WIN0, DISP_WINDOW_OFF);

			//EnterCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
			pCtxt->LocalPathCmdCtxt.bCmdSetWin0Disable = FALSE;
			//LeaveCriticalSection(&pCtxt->LocalPathCmdCtxt.csCmd);
		}
#endif

		//-----------------------------------
		// Processing TV Scaler Command
		//-----------------------------------

		// Command : Set Src Buffer
		if (pCtxt->TVSCCmdCtxt.bCmdSetSrcBuffer)
		{
			// At this time we can not update current address from next address
			// Because current address is updated at the start of interrupt
			// So, in the IST we can change only current address to make affect

			//EnterCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);
			pCtxt->TVSCCmdCtxt.bCmdSetSrcBuffer = FALSE;
			//LeaveCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);
		}

		// Command : Set Dst Buffer
		if (pCtxt->TVSCCmdCtxt.bCmdSetDstBuffer)
		{
			// At this time we can not update current address from next address
			// Because current address is updated at the start of interrupt
			// So, in the IST we can change only current address to make affect

			//EnterCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);
			pCtxt->TVSCCmdCtxt.bCmdSetDstBuffer = FALSE;
			//LeaveCriticalSection(&pCtxt->TVSCCmdCtxt.csCmd);
		}

		// Clear Interrupt Pending
		TVSC_clear_interrupt_pending();

		InterruptDone(pCtxt->dwSysIntrTVSC);

		PulseEvent(pCtxt->hTVSCCmdDone);
	}

	VDE_MSG((_T("[VDE] --SVE_TVScalerIntrThread()\n\r")));

	return 0;
}

DWORD SVE_wait_disp_cmd_done(void)
{
	return WaitForSingleObject(SVE_get_context()->hDispCmdDone, SVE_DISP_CMD_TIMEOUT);
}

DWORD SVE_wait_post_cmd_done(void)
{
	return WaitForSingleObject(SVE_get_context()->hPostCmdDone, SVE_POST_CMD_TIMEOUT);
}

DWORD SVE_wait_tvsc_cmd_done(void)
{
	return WaitForSingleObject(SVE_get_context()->hTVSCCmdDone, SVE_TVSC_CMD_TIMEOUT);
}

DWORD SVE_wait_rotator_finish(void)
{
	return WaitForSingleObject(SVE_get_context()->hRotatorFinish, SVE_ROTATOR_FINISH_TIMEOUT);
}

