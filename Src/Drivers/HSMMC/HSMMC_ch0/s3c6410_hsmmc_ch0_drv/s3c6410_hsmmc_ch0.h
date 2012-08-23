//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
#ifndef _S3C6410_HSMMC_CH0_H_
#define _S3C6410_HSMMC_CH0_H_

#include "../s3c6410_hsmmc_ch0_lib/SDHC.h"
#include <oal_intr.h>

// for card detect pin of HSMMC ch0 on SMDK6410.
#define SD_CD0_IRQ	IRQ_EINT13

typedef class CSDHController : public CSDHCBase {
  public:
    // Constructor
		// The new Constructor implementation is needed for card detect of HSMMC ch0 on SMDK6410.
#ifndef _SMDK6410_CH0_EXTCD_
    CSDHController() : CSDHCBase() {}
#else
    CSDHController();
#endif

    // Destructor
    virtual ~CSDHController() {}

    // Perform basic initialization including initializing the hardware
    // so that the capabilities register can be read.
    virtual BOOL Init(LPCTSTR pszActiveKey);

    virtual VOID PowerUp();

    virtual LPSDHC_DESTRUCTION_PROC GetDestructionProc() {
      return &DestroyHSMMCHCObject;
    }

    static VOID DestroyHSMMCHCObject(PCSDHCBase pSDHC);

#ifdef _SMDK6410_CH0_EXTCD_
		// Below functions and variables are newly implemented for card detect of HSMMC ch0 on SMDK6410.
		virtual SD_API_STATUS Start();
#endif

  protected:
    BOOL InitClkPwr();
    BOOL InitGPIO();
    BOOL InitHSMMC();

    BOOL InitCh();
#ifdef _SMDK6410_CH0_EXTCD_
    static DWORD SD_CardDetectThread(CSDHController *pController) {
      	return pController->CardDetectThread();
    }

    virtual DWORD CardDetectThread();

    virtual BOOL InitializeHardware();

		BOOL EnableCardDetectInterrupt();

    HANDLE		m_hevCardDetectEvent;
    HANDLE		m_htCardDetectThread;
    DWORD			m_dwSDDetectSysIntr;
    DWORD			m_dwSDDetectIrq;
#endif
} *PCSDHController;

#endif // _S3C6410_HSMMC_CH0_H_

