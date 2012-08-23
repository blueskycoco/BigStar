//------------------------------------------------------------------------------
//
//  Module: s3c6410_pm.h
//
//  Common Definition of PM module
//
#ifndef _S3C6410_PM__H_
#define _S3C6410_PM__H_

#define SETVOLTAGE_ARM          (1)
#define SETVOLTAGE_INTERNAL     (2)
#define SETVOLTAGE_BOTH         (SETVOLTAGE_ARM|SETVOLTAGE_INTERNAL)

extern void PMIC_Init();
extern void PMIC_VoltageSet(UINT32 uPwr, UINT32 uVolate, UINT32 uDelay);

#endif _S3C6410_PM__H_
