//------------------------------------------------------------------------------
//
//  Module: LTC3714.h
//
//  Interface of Low Control library of LTC3714 PM chip
//
#ifndef _LTC3714__H_
#define _LTC3714__H_

/// Prototypes
void LTC3714_Init();
void LTC3714_VoltageSet(UINT32 uPwr, UINT32 uVoltage, UINT32 uDelay);

#endif _LTC3714__H_
