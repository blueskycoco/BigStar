//------------------------------------------------------------------------------
//
//  Module: s3c6410_pm.c
//
//  Common Inteface & Glue logic of PM module
//
#include <bsp.h>
#include "s3c6410_pm.h"
#include "LTC3714.h"

/// This functions can be changed
void PMIC_Init()
{
    LTC3714_Init();
}
void PMIC_VoltageSet(UINT32 uPwr, UINT32 uVoltage, UINT32 uDelay)
{
    LTC3714_VoltageSet(uPwr, uVoltage,uDelay);
}


