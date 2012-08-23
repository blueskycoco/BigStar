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
#ifndef _S3C6410_HSMMC_CH1_H_
#define _S3C6410_HSMMC_CH1_H_

#include "../s3c6410_hsmmc_ch1_lib/SDHC.h"

typedef class CSDHController : public CSDHCBase {
  public:
    // Constructor
    CSDHController() : CSDHCBase() {}

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

  protected:
    BOOL InitClkPwr();
    BOOL InitGPIO();
    BOOL InitHSMMC();

    BOOL InitCh();
} *PCSDHController;

#endif // _S3C6410_HSMMC_CH1_H_

