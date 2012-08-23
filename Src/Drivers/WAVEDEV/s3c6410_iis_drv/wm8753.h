/*-----------------------------------------------------------------------------
 * Copyright (c) Wolfson Microelectronics plc.  All rights reserved.
 *
 * This software as well as any related documentation is furnished under 
 * license and may only be used or copied in accordance with the terms of the 
 * license. The information in this file is furnished for informational use 
 * only, is subject to change without notice, and should not be construed as 
 * a commitment by Wolfson Microelectronics plc. Wolfson Microelectronics plc
 * assumes no responsibility or liability for any errors or inaccuracies that
 * may appear in this document or any software that may be provided in
 * association with this document. 
 *
 * Except as permitted by such license, no part of this document may be 
 * reproduced, stored in a retrieval system, or transmitted in any form or by 
 * any means without the express written consent of Wolfson Microelectronics plc. 
 *
 * $Id: WM8753RegisterDefs.h 2361 2005-11-04 15:53:00Z ib $
 *
 * The registers on the Wolfson WM8753 device.
 *
 * Warning:
 *  This driver is specifically written for Wolfson Codecs. It is not a 
 *  general CODEC device driver.
 *
 *---------------------------------------------------------------------------*/
#ifndef __WM8753REGISTERDEFS_H__
#define __WM8753REGISTERDEFS_H__

typedef unsigned short WM_USHORT;   /* Do not use. Left in for backwards compatiblity only */

//typedef WM_USHORT                WM_REGTYPE;
typedef WM_USHORT                WM_REGVAL;

/* 
 * Include files
 */

/*
 * Register values.
 */
#define WM8753_DAC_CONTROL                          0x01
#define WM8753_ADC_CONTROL                          0x02
#define WM8753_VOICE_AUDIO_INTERFACE                0x03
#define WM8753_HIFI_AUDIO_INTERFACE                 0x04
#define WM8753_INTERFACE_CONTROL                    0x05
#define WM8753_SAMPLE_RATE_CTRL_1                   0x06
#define WM8753_SAMPLE_RATE_CTRL_2                   0x07
#define WM8753_LEFT_DAC_VOLUME                      0x08
#define WM8753_RIGHT_DAC_VOLUME                     0x09
#define WM8753_BASS_CONTROL                         0x0A
#define WM8753_TREBLE_CONTROL                       0x0B
#define WM8753_ALC_1                                0x0C
#define WM8753_ALC_2                                0x0D
#define WM8753_ALC_3                                0x0E
#define WM8753_NOISE_GATE                           0x0F
#define WM8753_LEFT_ADC_VOLUME                      0x10
#define WM8753_RIGHT_ADC_VOLUME                     0x11
#define WM8753_ADD_CONTROL_1                        0x12
#define WM8753_3D_CONTROL                           0x13
#define WM8753_PWR_MGMT_1                           0x14
#define WM8753_PWR_MGMT_2                           0x15
#define WM8753_PWR_MGMT_3                           0x16
#define WM8753_PWR_MGMT_4                           0x17
#define WM8753_ID_REGISTER                          0x18
#define WM8753_INTERUPT_POLARITY                    0x19
#define WM8753_INTERUPT_ENABLE                      0x1A
#define WM8753_GPIO_CONTROL_1                       0x1B
#define WM8753_GPIO_CONTROL_2                       0x1C
#define WM8753_RESET                                0x1F
#define WM8753_RECORD_MIX_1                         0x20
#define WM8753_RECORD_MIX_2                         0x21
#define WM8753_LEFT_OUT_MIX_1                       0x22
#define WM8753_LEFT_OUT_MIX_2                       0x23
#define WM8753_RIGHT_OUT_MIX_1                      0x24
#define WM8753_RIGHT_OUT_MIX_2                      0x25
#define WM8753_MONO_OUT_MIX_1                       0x26
#define WM8753_MONO_OUT_MIX_2                       0x27
#define WM8753_LOUT1_VOLUME                         0x28
#define WM8753_ROUT1_VOLUME                         0x29
#define WM8753_LOUT2_VOLUME                         0x2A
#define WM8753_ROUT2_VOLUME                         0x2B
#define WM8753_MONOOUT_VOLUME                       0x2C
#define WM8753_OUTPUT_CONTROL                       0x2D
#define WM8753_ADC_INPUT_MODE                       0x2E
#define WM8753_INPUT_CONTROL_1                      0x2F
#define WM8753_INPUT_CONTROL_2                      0x30
#define WM8753_LEFT_INPUT_VOLUME                    0x31
#define WM8753_RIGHT_INPUT_VOLUME                   0x32
#define WM8753_MIC_BIAS_COMP_CONTROL                0x33
#define WM8753_CLOCK_CONTROL                        0x34
#define WM8753_PLL1_CONTROL_1                       0x35
#define WM8753_PLL1_CONTROL_2                       0x36
#define WM8753_PLL1_CONTROL_3                       0x37
#define WM8753_PLL1_CONTROL_4                       0x38
#define WM8753_PLL2_CONTROL_1                       0x39
#define WM8753_PLL2_CONTROL_2                       0x3A
#define WM8753_PLL2_CONTROL_3                       0x3B
#define WM8753_PLL2_CONTROL_4                       0x3C
#define WM8753_BIAS_CONTROL                         0x3D
#define WM8753_ADD_CONTROL_2                        0x3F

#define WM8753_REGISTER_COUNT                       0x3C
#define WM8753_MAX_REGISTER                         WM8753_ADD_CONTROL_2

//#define WM8753_MAX_REGISTER_COUNT                     (WM8753_MAX_REGISTER + 1) //64

/* General volume defines */
#define WM8753_VOLUME_UPDATE        ((WM_REGVAL)(1U << 8))

/* DAC Control (0x01) */
#define WM8753_DACINV               ((WM_REGVAL)(1U << 6))   /* DACINV: DAC Phase Invert */
#define WM8753_DACMONOMIX_MASK      ((WM_REGVAL)(3U << 4))   /* DMONOMIX: [5:4] */
#define WM8753_DACMONOMIX_STEREO    ((WM_REGVAL)(0U << 4))   /* 00: stereo */
#define WM8753_DACMONOMIX_MONO_L    ((WM_REGVAL)(1U << 4))   /* 01: mono ((L+R)/2) into DACL, '0' into DACR */
#define WM8753_DACMONOMIX_MONO_R    ((WM_REGVAL)(2U << 4))   /* 10: mono ((L+R)/2) into DACR, '0' into DACL */
#define WM8753_DACMONOMIX_MONO_LR   ((WM_REGVAL)(3U << 4))   /* 11: mono ((L+R)/2) into DACL and DACR */
#define WM8753_DAC_SOFT_MUTE        ((WM_REGVAL)(1U << 3))   /* DACMU: Digital soft Mute */
#define WM8753_DEEMP_MASK           ((WM_REGVAL)(3U << 1))   /* DEEMPH: [2:1] */
#define WM8753_DEEMP_NONE           ((WM_REGVAL)(0U << 1))   /* 00: No De-emphasis */
#define WM8753_DEEMP_32KHZ          ((WM_REGVAL)(1U << 1))   /* 01: 32kHz sample rate */
#define WM8753_DEEMP_44_1KHZ        ((WM_REGVAL)(2U << 1))   /* 10: 44.1kHz sample rate */
#define WM8753_DEEMP_48KHZ          ((WM_REGVAL)(3U << 1))   /* 11: 48kHz sample rate */

/* ADC Control (0x02) */
#define WM8753_DATSEL_MASK          ((WM_REGVAL)(3U << 7))   /* DATSEL: [8:7] */
#define WM8753_DATSEL_L_R           ((WM_REGVAL)(0U << 7))   /* 00: left data = left ADC; right data = right ADC */
#define WM8753_DATSEL_L_L           ((WM_REGVAL)(1U << 7))   /* 01: left data = left ADC; right data = left ADC */
#define WM8753_DATSEL_R_R           ((WM_REGVAL)(2U << 7))   /* 10: left data = right ADC; right data = right ADC */
#define WM8753_DATSEL_R_L           ((WM_REGVAL)(3U << 7))   /* 11: left data = right ADC; right data = left ADC */
#define WM8753_ADCPOL_MASK          ((WM_REGVAL)(3U << 5))   /* ADCPOL: [6:5] */
#define WM8753_ADCPOL_NONE_INVERT   ((WM_REGVAL)(0U << 5))   /* 00: Polarity not inverted */
#define WM8753_ADCPOL_L_INVERT      ((WM_REGVAL)(1U << 5))   /* 01: Left polarity inverted */
#define WM8753_ADCPOL_R_INVERT      ((WM_REGVAL)(2U << 5))   /* 10: Right polarity inverted */
#define WM8753_ADCPOL_LR_INVERT     ((WM_REGVAL)(3U << 5))   /* 11: Left and Right polarity inverted */
#define WM8753_VXFILTER_HIFI        ((WM_REGVAL)(0U << 4))   /* VXFILT: ADC Filter - HiFi */
#define WM8753_VXFILTER_VOICE       ((WM_REGVAL)(1U << 4))   /* VXFILT: ADC Filter - Voice */
#define WM8753_HPMODE_MASK          ((WM_REGVAL)(3U << 2))   /* HPMODE: [3:2] */
#define WM8753_HPMODE_3_4HZ_48KHZ   ((WM_REGVAL)(0U << 2))   /* 00: 3.4Hz @ fs = 48kHz */
#define WM8753_HPMODE_82HZ_16KHZ    ((WM_REGVAL)(1U << 2))   /* 01: 82Hz @ fs = 16kHz (41Hz @ fs = 8kHz) */
#define WM8753_HPMODE_82HZ_8KHZ     ((WM_REGVAL)(2U << 2))   /* 10: 82Hz @ fs = 8kHz (164Hz @ fs = 16kHz) */
#define WM8753_HPMODE_170HZ_8KHZ    ((WM_REGVAL)(3U << 2))   /* 11: 170Hz @ fs = 8kHz (340Hz @ fs = 16kHz) */
#define WM8753_DC_OFFSET_CLEAR      ((WM_REGVAL)(0U << 1))   /* HPOR: Clear DC offset */
#define WM8753_DC_OFFSET_STORE      ((WM_REGVAL)(1U << 1))   /* HPOR: Store DC offset */
#define WM8753_HP_FILTER_DISABLE    ((WM_REGVAL)(1U << 0))   /* ADCHPD: ADC High Pass Filter (Digital) Disable */  
 
/*
 * Audio Interfaces - HIFI/VOICE interface control (0x03/0x04)
 */
#define WM8753_ADCDOP_MASK          ((WM_REGVAL)(1U << 8))   /* ADCDOP: Voice only: [8] */
#define WM8753_ADCDOP_ADCDAT_VXDOUT ((WM_REGVAL)(1U << 8))   /* ADCDOP: Voice only: ADC data output to ADCDAT and VXDOUT */
#define WM8753_ADCDOP_ADCDAT        ((WM_REGVAL)(0U << 8))   /* ADCDOP: Voice only: ADC data output to ADCDAT or VXDOUT as selected by IFMODE[1:0] */
#define WM8753_ADCDOP_VXDOUT        ((WM_REGVAL)(0U << 8))   /* ADCDOP: Voice only: ADC data output to ADCDAT or VXDOUT as selected by IFMODE[1:0] */
#define WM8753_DACCLKINV            ((WM_REGVAL)(1U << 7))   /* BCLKINV/VXCLKINV: VXCLK invert */
#define WM8753_BITCLKINV            WM8753_DACCLKINV
#define WM8753_VXCLKINV             WM8753_DACCLKINV

#define WM8753_MASTER               ((WM_REGVAL)(1U << 6))   /* MS/PMS: WM8753 is the master */
#define WM8753_SLAVE                ((WM_REGVAL)(0U << 6))   /* MS/PMS: WM8753 is the slave */

#define WM8753_HIFI_LR_SWAP         ((WM_REGVAL)(1U << 5))   /* LRSWAP: HIFI only: swap left and right channels */
#define WM8753_VOICE_MONO           ((WM_REGVAL)(1U << 5))   /* MONO: Voice only: output only on left channel */
#define WM8753_VOICE_LEFT_ONLY      WM8753_VOICE_MONO
#define WM8753_VOICE_BOTH           ((WM_REGVAL)(0U << 5))   /* MONO: Voice only: output left and right channels */

#define WM8753_DSP_MODE_MASK        ((WM_REGVAL)(1U << 4))   /* LRP: [4] DSP Mode */
#define WM8753_LRC_INVERT           ((WM_REGVAL)(1U << 4))   /* LRP: right, left and I2S: invert LRC polarity */
#define WM8753_DSP_MODE_A           ((WM_REGVAL)(0U << 4))   /* LRP: DSP mode A: 2nd BITCLK after LRC */
#define WM8753_DSP_MODE_B           ((WM_REGVAL)(1U << 4))   /* LRP: DSP mode B: 1st BITCLK after LRC */

#define WM8753_WORD_LENGTH_MASK     ((WM_REGVAL)(3U << 2))   /* WL - [3:2] data word length */
#define WM8753_WORD_LENGTH_16BIT    ((WM_REGVAL)(0U << 2))   /* 00: 16-bit word length */
#define WM8753_WORD_LENGTH_20BIT    ((WM_REGVAL)(1U << 2))   /* 01: 20-bit word length */
#define WM8753_WORD_LENGTH_24BIT    (((WM_REGVAL)2U << 2))   /* 10: 24-bit word length */
#define WM8753_WORD_LENGTH_32BIT    (((WM_REGVAL)3U << 2))   /* 11: 32-bit word length (not right-justified) */

#define WM8753_DATA_FORMAT_MASK     ((WM_REGVAL)(3U << 0))   /* FORMAT - [1:0] - data format */
#define WM8753_DATA_FORMAT_RJUST    ((WM_REGVAL)(0U << 0))   /* 00: right-justified */
#define WM8753_DATA_FORMAT_LJUST    ((WM_REGVAL)(1U << 0))   /* 01: left-justified */
#define WM8753_DATA_FORMAT_I2S      ((WM_REGVAL)(2U << 0))   /* 10: I2S */
#define WM8753_DATA_FORMAT_DSP      ((WM_REGVAL)(3U << 0))   /* 11: DSP mode */

/* INTERFACE_CONTROL (0x05) */
#define WM8753_VXCLK_ENABLE         ((WM_REGVAL)(0U << 7))   /* VXCLKTRI: VXCLK pin is input/output */
#define WM8753_VXCLK_TRISTATE       ((WM_REGVAL)(1U << 7))   /* VXCLKTRI: VXCLK pin is tristated */
#define WM8753_BITCLK_ENABLE        ((WM_REGVAL)(0U << 6))   /* BCLKTRI: BCLK pin is input/output */
#define WM8753_BITCLK_TRISTATE      ((WM_REGVAL)(1U << 6))   /* BCLKTRI: BCLK pin is tristated */
#define WM8753_VXDOUT_OUTPUT        ((WM_REGVAL)(0U << 5))   /* VXDTRI: VXDOUT pin is output */
#define WM8753_VXDOUT_TRISTATE      ((WM_REGVAL)(1U << 5))   /* VXDTRI: VXDOUT pin is tristated */
#define WM8753_ADCDAT_OUTPUT        ((WM_REGVAL)(0U << 4))   /* ADCTRI: ADCDATA pin is output */
#define WM8753_ADCDAT_TRISTATE      ((WM_REGVAL)(1U << 4))   /* ADCTRI: ADCDATA pin is tristated */

#define WM8753_INTERFACE_MODE_MASK  ((WM_REGVAL)(3U << 2))   /* IFMODE - [3:2]: */
#define WM8753_MODE_HIFI_VOICE      ((WM_REGVAL)(0U << 2))   /* 00: HIFI DAC + Voice codec */
#define WM8753_MODE_VOICE_ON_HIFI   ((WM_REGVAL)(1U << 2))   /* 01: Voice codec on HIFI interface */
#define WM8753_MODE_HIFI            ((WM_REGVAL)(2U << 2))   /* 10: HIFI ADC and DAC, no voice */
#define WM8753_MODE_HIFI_VXFS       ((WM_REGVAL)(3U << 2))   /* 11: HIFI using VXFS for ADC, no voice */
#define WM8753_VXFS_INPUT           ((WM_REGVAL)(0U << 1))   /* VXFSOE: VXFS pin is input in master mode */
#define WM8753_VXFS_OUTPUT          ((WM_REGVAL)(1U << 1))   /* VXFSOE: VXFS pin is output in master mode */
#define WM8753_LRC_INPUT            ((WM_REGVAL)(0U << 0))   /* LRCOE: LRC pin is input in master mode */
#define WM8753_LRC_OUTPUT           ((WM_REGVAL)(1U << 0))   /* LRCOE: LRC pin is output in master mode */
 
/*
 * SAMPLE_RATE_CTRL_1 (0x06)
 */
#define WM8753_SAMPLERATE_MASK      ((WM_REGVAL)(0x1FU << 1))/* SR - [5:1] */
#define WM8753_SAMPLERATE_SHIFT     1

#define WM8753_SAMPLE_CLOCK_MASK    ((WM_REGVAL)(1U << 0))   /* USB: [0] */
#define WM8753_SAMPLE_USB           ((WM_REGVAL)(1U << 0))   /* USB: USB mode */
#define WM8753_SAMPLE_NORMAL        ((WM_REGVAL)(0U << 0))   /* USB: normal mode */   
#define WM8753_SAMPLE_FEQUENCY_MASK ((WM_REGVAL)(1U << 7))   /* PSR: [7] */
#define WM8753_SAMPLE_256FS         ((WM_REGVAL)(0U << 7))   /* PSR: 256FS */
#define WM8753_SAMPLE_384FS         ((WM_REGVAL)(1U << 7))   /* PSR: 384FS */
#define WM8753_ADC_SRMODE_MASK      ((WM_REGVAL)(1U << 8))   /* SRMODE: Sample rate for ADC */
#define WM8753_ADC_SRMODE_SR        ((WM_REGVAL)(0U << 8))   /* SRMODE: Sample rate from SR[4:0] */
#define WM8753_ADC_SRMODE_PSR       ((WM_REGVAL)(1U << 8))   /* SRMODE: Sample rate from PSR */

/*
 * SAMPLE_RATE_CTRL_2 (0x07)
 */
#define WM8753_VXCLK_RATE_MASK      ((WM_REGVAL)(7U << 6))  /* PBMODE - [8:6] */
#define WM8753_VXCLK_MCLK_DIV_1     ((WM_REGVAL)(0U << 6))
#define WM8753_VXCLK_MCLK_DIV_2     ((WM_REGVAL)(1U << 6))
#define WM8753_VXCLK_MCLK_DIV_4     ((WM_REGVAL)(2U << 6))
#define WM8753_VXCLK_MCLK_DIV_8     ((WM_REGVAL)(3U << 6))
#define WM8753_VXCLK_MCLK_DIV_16    ((WM_REGVAL)(4U << 6))

#define WM8753_BCLK_RATE_MASK       ((WM_REGVAL)(7U << 3))  /* BMODE - [5:3] */
#define WM8753_BCLK_MCLK_DIV_1      ((WM_REGVAL)(0U << 3))
#define WM8753_BCLK_MCLK_DIV_2      ((WM_REGVAL)(1U << 3))
#define WM8753_BCLK_MCLK_DIV_4      ((WM_REGVAL)(2U << 3))
#define WM8753_BCLK_MCLK_DIV_8      ((WM_REGVAL)(3U << 3))
#define WM8753_BCLK_MCLK_DIV_16     ((WM_REGVAL)(4U << 3))

#define WM8753_VXDAC_OVERSAMPLE_64  ((WM_REGVAL)(1U << 2))   /* VXDACOSR */
#define WM8753_VXDAC_OVERSAMPLE_128 ((WM_REGVAL)(0U << 2))   /* VXDACOSR */
#define WM8753_ADC_OVERSAMPLE_64    ((WM_REGVAL)(1U << 1))   /* ADCOSR */
#define WM8753_ADC_OVERSAMPLE_128   ((WM_REGVAL)(0U << 1))   /* ADCOSR */
#define WM8753_DAC_OVERSAMPLE_64    ((WM_REGVAL)(1U << 0))   /* DACOSR */
#define WM8753_DAC_OVERSAMPLE_128   ((WM_REGVAL)(0U << 0))   /* DACOSR */

/*
 * DAC_VOLUME (0x8/0x9)
 */
#define WM8753_DACVOL_MASK          0x0FF   /* 01 = -127dB, FF = 0dB, 0.5dB steps */
#define WM8753_DACVOL_0DB           0x0FF
#define WM8753_DACVOL_MUTE          0x000

/*
 * ADC_VOLUME (0x10/0x11)
 */
#define WM8753_ADCVOL_MASK          0x0FF   /* 01 = -97dB, FF = +30dB, 0.5dB steps */
#define WM8753_ADCVOL_0DB           0x0C3
#define WM8753_ADCVOL_MUTE          0x000


/*
 * DAC Volume - _db should be between 0 and -127, -128 is mute
 * ADC Volume - _db should be between 30 and -97, -98 is mute
 * E.g. WM8753_DACVOL( 0 ) for 0dBFS, WM8753_DACVOL( -12 ) for -12dbFS.
 */
#define WM8753_DACVOL(_dB)          (WM8753_DACVOL_0DB + (2*(_dB)))
#define WM8753_ADCVOL(_dB)          (WM8753_ADCVOL_0DB + (2*(_dB)))

/*
 * 0x0A - 0x0B
#define WM8753_BASS_CONTROL                         0x0A
#define WM8753_TREBLE_CONTROL                       0x0B
*/

/*
 * ALC and noise gate - 0xC - 0xF
 */

/* ALC Control 1 - 0xC */
#define WM8753_ALC_LEVEL_MASK       ((WM_REGVAL)(0xF << 0)) /* ALCL - [3:0] */
#define WM8753_ALC_LEVEL(_dB)       (((WM_REGVAL)(((_dB)+28.5)*2/3+0.5)) & WM8753_ALC_LEVEL_MASK)

#define WM8753_ALCVOL_MASK          ((WM_REGVAL)(7U << 4))  /* MAXGAIN - [6:4] */
#define WM8753_ALCVOL_30DB          ((WM_REGVAL)(7U << 4))  /* +30dB */
#define WM8753_ALCVOL_24DB          ((WM_REGVAL)(6U << 4))  /* +24dB */
#define WM8753_ALCVOL_18DB          ((WM_REGVAL)(5U << 4))  /* +18dB */
#define WM8753_ALCVOL_12DB          ((WM_REGVAL)(4U << 4))  /* +12dB */
#define WM8753_ALCVOL_6DB           ((WM_REGVAL)(3U << 4))  /* +6dB  */
#define WM8753_ALCVOL_0DB           ((WM_REGVAL)(2U << 4))  /*  0dB  */
#define WM8753_ALCVOL_M6DB          ((WM_REGVAL)(1U << 4))  /* -6dB  */
#define WM8753_ALCVOL_M12DB         ((WM_REGVAL)(0U << 4))  /* -12dB */
#define WM8753_ALCVOL(_dB)          ((WM_REGVAL)(((((int)(_dB)+12+3)/6) << 4 ) & WM8753_ALCVOL_MASK))

#define WM8753_ALCSEL_MASK          ((WM_REGVAL)(3U << 7))  /* ALCSEL - [8:7] */
#define WM8753_ALC_OFF              ((WM_REGVAL)(0U << 7))  /* ALC off (PGA from register) */
#define WM8753_ALC_RIGHT            ((WM_REGVAL)(1U << 7))  /* Right channel only */
#define WM8753_ALC_LEFT             ((WM_REGVAL)(2U << 7))  /* Left channel only */
#define WM8753_ALC_STEREO           ((WM_REGVAL)(3U << 7))  /* Stereo (PGA registers unused) */

/* ALC Control 2 - 0xD */
#define WM8753_HOLD_MASK            ((WM_REGVAL)(0xF << 0)) /* HLD - [3:0] */
#define WM8753_HOLD_0MS             ((WM_REGVAL)(0x0 << 0)) /* 0ms */
#define WM8753_HOLD_2_67MS          ((WM_REGVAL)(0x1 << 0)) /* 2.67ms */
#define WM8753_HOLD_5_33MS          ((WM_REGVAL)(0x2 << 0)) /* 5.33ms */
#define WM8753_HOLD_10_67MS         ((WM_REGVAL)(0x3 << 0)) /* 10.67ms */
#define WM8753_HOLD_21_33MS         ((WM_REGVAL)(0x4 << 0)) /* 21.33ms */
#define WM8753_HOLD_43MS            ((WM_REGVAL)(0x5 << 0)) /* 42.67ms */
#define WM8753_HOLD_85MS            ((WM_REGVAL)(0x6 << 0)) /* 85.33ms */
#define WM8753_HOLD_171MS           ((WM_REGVAL)(0x7 << 0)) /* 170.67ms */
#define WM8753_HOLD_341MS           ((WM_REGVAL)(0x8 << 0)) /* 341.33ms */
#define WM8753_HOLD_683MS           ((WM_REGVAL)(0x9 << 0)) /* 682.67ms */
#define WM8753_HOLD_1_365S          ((WM_REGVAL)(0xA << 0)) /* 1.365s */
#define WM8753_HOLD_2_731S          ((WM_REGVAL)(0xB << 0)) /* 2.731s */
#define WM8753_HOLD_5_461S          ((WM_REGVAL)(0xC << 0)) /* 5.461s */
#define WM8753_HOLD_10_923S         ((WM_REGVAL)(0xD << 0)) /* 10.932s */
#define WM8753_HOLD_21_845S         ((WM_REGVAL)(0xE << 0)) /* 21.845s */
#define WM8753_HOLD_43_691S         ((WM_REGVAL)(0xF << 0)) /* 43.691s */

#define WM8753_ALC_SAMPLERATE_MASK  ((WM_REGVAL)(0xF << 4)) /* ALCSR - [7:4] */
#define WM8753_ALCSR_8K             ((WM_REGVAL)(0x6 << 4)) /* 8kHz */
#define WM8753_ALCSR_11K            ((WM_REGVAL)(0x8 << 4)) /* 11.025kHz */
#define WM8753_ALCSR_12K            ((WM_REGVAL)(0x8 << 4)) /* 12kHz */
#define WM8753_ALCSR_16K            ((WM_REGVAL)(0xA << 4)) /* 16kHz */
#define WM8753_ALCSR_22K            ((WM_REGVAL)(0xA << 4)) /* 22.05kHz */
#define WM8753_ALCSR_32K            ((WM_REGVAL)(0xC << 4)) /* 32kHz */
#define WM8753_ALCSR_44K            ((WM_REGVAL)(0x0 << 4)) /* 44.1kHz */
#define WM8753_ALCSR_48K            ((WM_REGVAL)(0x0 << 4)) /* 48kHz */
#define WM8753_ALCSR_88K            ((WM_REGVAL)(0xE << 4)) /* 88.2kHz */
#define WM8753_ALCSR_96K            ((WM_REGVAL)(0xE << 4)) /* 96kHz */

#define WM8753_ALC_ZEROCROSS        ((WM_REGVAL)(1U << 8))  /* ALCZC */

/* ALC Control 3 - 0xE */
#define WM8753_ATTACK_MASK          ((WM_REGVAL)(0xF << 0)) /* ATK - [3:0] */
#define WM8753_ATTACK_6MS           ((WM_REGVAL)(0x0 << 0)) /* 6ms */
#define WM8753_ATTACK_12MS          ((WM_REGVAL)(0x1 << 0)) /* 12ms */
#define WM8753_ATTACK_24MS          ((WM_REGVAL)(0x2 << 0)) /* 24ms */
#define WM8753_ATTACK_48MS          ((WM_REGVAL)(0x3 << 0)) /* 48ms */
#define WM8753_ATTACK_96MS          ((WM_REGVAL)(0x4 << 0)) /* 96ms */
#define WM8753_ATTACK_192MS         ((WM_REGVAL)(0x5 << 0)) /* 192ms */
#define WM8753_ATTACK_384MS         ((WM_REGVAL)(0x6 << 0)) /* 384ms */
#define WM8753_ATTACK_768MS         ((WM_REGVAL)(0x7 << 0)) /* 768ms */
#define WM8753_ATTACK_1_54S         ((WM_REGVAL)(0x8 << 0)) /* 1536ms */
#define WM8753_ATTACK_3S            ((WM_REGVAL)(0x9 << 0)) /* 3.07s */
#define WM8753_ATTACK_6S            ((WM_REGVAL)(0xA << 0)) /* 6.14s */

#define WM8753_DECAY_MASK           ((WM_REGVAL)(0xF << 4)) /* DCY - [7:4] */
#define WM8753_DECAY_24MS           ((WM_REGVAL)(0x0 << 4)) /* 24ms */
#define WM8753_DECAY_48MS           ((WM_REGVAL)(0x1 << 4)) /* 48ms */
#define WM8753_DECAY_96MS           ((WM_REGVAL)(0x2 << 4)) /* 96ms */
#define WM8753_DECAY_192MS          ((WM_REGVAL)(0x3 << 4)) /* 192ms */
#define WM8753_DECAY_384MS          ((WM_REGVAL)(0x4 << 4)) /* 384ms */
#define WM8753_DECAY_768MS          ((WM_REGVAL)(0x5 << 4)) /* 768ms */
#define WM8753_DECAY_1_54S          ((WM_REGVAL)(0x6 << 4)) /* 1536ms */
#define WM8753_DECAY_3S             ((WM_REGVAL)(0x7 << 4)) /* 3.072s */
#define WM8753_DECAY_6S             ((WM_REGVAL)(0x8 << 4)) /* 6.144s */
#define WM8753_DECAY_12S            ((WM_REGVAL)(0x9 << 4)) /* 12.288s */
#define WM8753_DECAY_24S            ((WM_REGVAL)(0xA << 4)) /* 24.576s */

/* Noise Gate Control - 0xF */
#define WM8753_NOISE_GATE_ENABLE    ((WM_REGVAL)(1U << 0))  /* NGAT */

#define WM8753_NOISE_GATE_TYPE      ((WM_REGVAL)(1U << 1))  /* NGG */
#define WM8753_NOISE_GATE_CONSTANT  ((WM_REGVAL)(0U << 1))  /* PGA gain held constant */
#define WM8753_NOISE_GATE_MUTE      ((WM_REGVAL)(1U << 1))  /* mute ADC output */

#define WM8753_NG_THRESH_MASK       ((WM_REGVAL)(0x1F << 3))/* NGTH - [7:3] */
#define WM8753_NG_THRESH(_dB)       (((WM_REGVAL)(((_dB)+76.5)*2/3+0.5) << 3) & WM8753_NG_THRESH_MASK)

/*
 * Suggested ALC profiles - see application note WAN0140.
 * 
 * Voice: Stereo Max +30dB, target -12dB, 0ms hold, 192ms decay, 24ms attack
 * Music: Stereo Max +30dB, target -6dB, 341ms hold, 6s decay, 24ms attack
 * 
 * For both: Noise gate enabled, gain constant, threshold -76.5dB
 */
#define WM8753_ALC1_PROFILE_MASK    (WM8753_ALC_LEVEL_MASK | WM8753_ALCVOL_MASK)
#define WM8753_ALC2_PROFILE_MASK    (WM8753_HOLD_MASK | WM8753_ALC_ZEROCROSS)

/* Voice */
#define WM8753_ALCVOL_VOICE         WM8753_ALCVOL_30DB
#define WM8753_ALC_LEVEL_VOICE      WM8753_ALC_LEVEL(-12)
#define WM8753_ALCZC_VOICE          0
#define WM8753_HOLD_VOICE           WM8753_HOLD_0MS
#define WM8753_DECAY_VOICE          WM8753_DECAY_192MS
#define WM8753_ATTACK_VOICE         WM8753_ATTACK_24MS

#define WM8753_ALC1_VOICE           (WM8753_ALC_LEVEL_VOICE | WM8753_ALCVOL_VOICE)
#define WM8753_ALC2_VOICE           (WM8753_HOLD_VOICE | WM8753_ALCZC_VOICE)
#define WM8753_ALC3_VOICE           (WM8753_DECAY_VOICE | WM8753_ATTACK_VOICE)
#define WM8753_NOISE_GATE_VOICE     (WM8753_NOISE_GATE_ENABLE   |       \
                                     WM8753_NOISE_GATE_CONSTANT |       \
                                     WM8753_NG_THRESH(-76.5))

/* Music */
#define WM8753_ALCVOL_MUSIC         WM8753_ALCVOL_30DB
#define WM8753_ALC_LEVEL_MUSIC      WM8753_ALC_LEVEL(-6)
#define WM8753_ALCZC_MUSIC          WM8753_ALC_ZEROCROSS
#define WM8753_HOLD_MUSIC           WM8753_HOLD_341MS
#define WM8753_DECAY_MUSIC          WM8753_DECAY_6S
#define WM8753_ATTACK_MUSIC         WM8753_ATTACK_24MS

#define WM8753_ALC1_MUSIC           (WM8753_ALC_LEVEL_MUSIC | WM8753_ALCVOL_MUSIC)
#define WM8753_ALC2_MUSIC           (WM8753_HOLD_MUSIC | WM8753_ALCZC_MUSIC)
#define WM8753_ALC3_MUSIC           (WM8753_DECAY_MUSIC | WM8753_ATTACK_MUSIC)
#define WM8753_NOISE_GATE_MUSIC     (WM8753_NOISE_GATE          |       \
                                     WM8753_NOISE_GATE_CONSTANT |       \
                                     WM8753_NG_THRESH(-76.5))

/*
 * ADC_VOLUME (0x10/0x11)
 * See below DAC_VOLUME (0x8/0x9)
 */
 
/*
 * 0x12 - 0x13
#define WM8753_ADD_CONTROL_1                        0x12
#define WM8753_3D_CONTROL                           0x13
 */
 
/*
 * Power management (registers 14h, 15h, 16h and 17h).
 */
#define WM8753_MAX_POWER_REGS                       4

/* Power managment 1 (register 14h) */
#define WM8753_PWR1_DIGENB                          ((WM_REGVAL)(1U<<0))
#define WM8753_PWR1_DACR                            ((WM_REGVAL)(1U<<2))
#define WM8753_PWR1_DACL                            ((WM_REGVAL)(1U<<3))
#define WM8753_PWR1_HIFIDAC                         (WM8753_PWR1_DACR | \
                                                     WM8753_PWR1_DACL)
#define WM8753_PWR1_VXDAC                           ((WM_REGVAL)(1U<<4))
#define WM8753_PWR1_DACS                            (WM8753_PWR1_HIFIDAC | \
                                                     WM8753_PWR1_VXDAC )
#define WM8753_PWR1_MICB                            ((WM_REGVAL)(1U<<5))
#define WM8753_PWR1_VREF                            ((WM_REGVAL)(1U<<6))
#define WM8753_PWR1_VMIDSEL_DISABLE                 ((WM_REGVAL)(0U<<7))
#define WM8753_PWR1_VMIDSEL_50KOHM                  ((WM_REGVAL)(1U<<7))
#define WM8753_PWR1_VMIDSEL_500KOHM                 ((WM_REGVAL)(2U<<7))
#define WM8753_PWR1_VMIDSEL_5KOHM                   ((WM_REGVAL)(3U<<7))
#define WM8753_PWR1_VMIDSEL_MASK                    ((WM_REGVAL)(3U<<7))

/* Power managment 2 (register 15h) */
#define WM8753_PWR2_LINEMIX                         ((WM_REGVAL)(1U<<0))
#define WM8753_PWR2_RXMIX                           ((WM_REGVAL)(1U<<1))
#define WM8753_PWR2_ADCR                            ((WM_REGVAL)(1U<<2))
#define WM8753_PWR2_ADCL                            ((WM_REGVAL)(1U<<3))
#define WM8753_PWR2_PGAR                            ((WM_REGVAL)(1U<<4))
#define WM8753_PWR2_PGAL                            ((WM_REGVAL)(1U<<5))
#define WM8753_PWR2_PGA                             (WM8753_PWR2_PGAR       | \
                                                     WM8753_PWR2_PGAL)
#define WM8753_PWR2_ADC                             (WM8753_PWR2_ADCR       | \
                                                     WM8753_PWR2_ADCL       | \
                                                     WM8753_PWR2_PGA)
#define WM8753_PWR2_ALCMIX                          ((WM_REGVAL)(1U<<6))
#define WM8753_PWR2_MIX                             (WM8753_PWR2_LINEMIX    | \
                                                     WM8753_PWR2_RXMIX      | \
                                                     WM8753_PWR2_ALCMIX)
#define WM8753_PWR2_MICAMP2EN                       ((WM_REGVAL)(1U<<7))
#define WM8753_PWR2_MICAMP1EN                       ((WM_REGVAL)(1U<<8))
#define WM8753_PWR2_MICAMPEN                        (WM8753_PWR2_MICAMP2EN  | \
                                                     WM8753_PWR2_MICAMP1EN)

/* Power managment 3 (register 16h) */
#define WM8753_PWR3_MONO2                           ((WM_REGVAL)(1U<<1))
#define WM8753_PWR3_MONO1                           ((WM_REGVAL)(1U<<2))
#define WM8753_PWR3_OUT4                            ((WM_REGVAL)(1U<<3))
#define WM8753_PWR3_OUT3                            ((WM_REGVAL)(1U<<4))
#define WM8753_PWR3_ROUT2                           ((WM_REGVAL)(1U<<5))
#define WM8753_PWR3_LOUT2                           ((WM_REGVAL)(1U<<6))
#define WM8753_PWR3_ROUT1                           ((WM_REGVAL)(1U<<7))
#define WM8753_PWR3_LOUT1                           ((WM_REGVAL)(1U<<8))
#define WM8753_PWR3_OUTPUT                          (WM8753_PWR3_MONO2      | \
                                                     WM8753_PWR3_MONO1      | \
                                                     WM8753_PWR3_OUT4       | \
                                                     WM8753_PWR3_OUT3       | \
                                                     WM8753_PWR3_ROUT2      | \
                                                     WM8753_PWR3_LOUT2      | \
                                                     WM8753_PWR3_ROUT1      | \
                                                     WM8753_PWR3_LOUT1)

/* Power managment 4 (register 17h) */
#define WM8753_PWR4_LEFTMIX                         ((WM_REGVAL)(1U<<0))
#define WM8753_PWR4_RIGHTMIX                        ((WM_REGVAL)(1U<<1))
#define WM8753_PWR4_MONOMIX                         ((WM_REGVAL)(1U<<2))
#define WM8753_PWR4_RECMIX                          ((WM_REGVAL)(1U<<3))
#define WM8753_PWR4_MIX                             (WM8753_PWR4_LEFTMIX    | \
                                                     WM8753_PWR4_RIGHTMIX   | \
                                                     WM8753_PWR4_MONOMIX    | \
                                                     WM8753_PWR4_RECMIX)
/*
 * 0x18-0x21
#define WM8753_ID_REGISTER                          0x18
#define WM8753_INTERUPT_POLARITY                    0x19
#define WM8753_INTERUPT_ENABLE                      0x1A
#define WM8753_GPIO_CONTROL_1                       0x1B
#define WM8753_GPIO_CONTROL_2                       0x1C
#define WM8753_RESET                                0x1F
#define WM8753_RECORD_MIX_1                         0x20
#define WM8753_RECORD_MIX_2                         0x21
 */

/*
 * Interrupt Polarity (register 19h)
 */
#define WM8753_INT_POL_TSD                            ((WM_REGVAL)(1U<<7))
#define WM8753_INT_POL_HPSW                            ((WM_REGVAL)(1U<<6))
#define WM8753_INT_POL_GPIO5                        ((WM_REGVAL)(1U<<5))
#define WM8753_INT_POL_GPIO4                        ((WM_REGVAL)(1U<<4))
#define WM8753_INT_POL_GPIO3                        ((WM_REGVAL)(1U<<3))
#define WM8753_INT_POL_MICDET                        ((WM_REGVAL)(1U<<1))
#define WM8753_INT_POL_MICSSDET                        ((WM_REGVAL)(1U<<0))

/*
 * Interrupt Masks (register 1Ah)
 */
#define WM8753_INT_EN_TSD                            ((WM_REGVAL)(1U<<7))
#define WM8753_INT_EN_HPSW                            ((WM_REGVAL)(1U<<6))
#define WM8753_INT_EN_GPIO5                            ((WM_REGVAL)(1U<<5))
#define WM8753_INT_EN_GPIO4                            ((WM_REGVAL)(1U<<4))
#define WM8753_INT_EN_GPIO3                            ((WM_REGVAL)(1U<<3))
#define WM8753_INT_EN_MICDET                        ((WM_REGVAL)(1U<<1))
#define WM8753_INT_EN_MICSSDET                        ((WM_REGVAL)(1U<<0))

/*
 * Interrupt Control [1] (register 1Bh)
 */
#define WM8753_INT_CON_DIS                            ((WM_REGVAL)(0x00U<<7))
#define WM8753_INT_CON_OPEN_DRAIN_ACT_LOW            ((WM_REGVAL)(0x01U<<7))
#define WM8753_INT_CON_ACT_HIGH                        ((WM_REGVAL)(0x02U<<7))
#define WM8753_INT_CON_ACT_LOW                        ((WM_REGVAL)(0x03U<<7))
#define WM8753_INT_CON_MASK                            ((WM_REGVAL)(0x03U<<7))
 
/*
 * GPIO Control 1 [GPIOs 4 & 5]
 */
#define WM8753_GPIO_CON_5_INPUT                        ((WM_REGVAL)(0x00U<<3))
#define WM8753_GPIO_CON_5_INT                        ((WM_REGVAL)(0x01U<<3))
#define WM8753_GPIO_CON_5_DRIVE_LOW                    ((WM_REGVAL)(0x02U<<3))
#define WM8753_GPIO_CON_5_DRIVE_HIGH                ((WM_REGVAL)(0x03U<<3))
#define WM8753_GPIO_CON_5_MASK                        ((WM_REGVAL)(0x03U<<3))

#define WM8753_GPIO_CON_4_INPUT                        ((WM_REGVAL)(0x00U<<0))
#define WM8753_GPIO_CON_4_PULL_DOWN_INPUT            ((WM_REGVAL)(0x02U<<0))
#define WM8753_GPIO_CON_4_PULL_UP_INPUT                ((WM_REGVAL)(0x03U<<0))
#define WM8753_GPIO_CON_4_DRIVE_LOW                    ((WM_REGVAL)(0x04U<<0))
#define WM8753_GPIO_CON_4_DRIVE_HIGH                ((WM_REGVAL)(0x05U<<0))
#define WM8753_GPIO_CON_4_SDOUT                        ((WM_REGVAL)(0x06U<<0))
#define WM8753_GPIO_CON_4_INT                        ((WM_REGVAL)(0x07U<<0))
#define WM8753_GPIO_CON_4_MASK                        ((WM_REGVAL)(0x07U<<0))

/*
 * GPIO Control 2 [GPIOs 1, 2 & 3]
 */
#define WM8753_GPIO_CON_3_INPUT                        ((WM_REGVAL)(0x00U<<6))
#define WM8753_GPIO_CON_3_DRIVE_LOW                    ((WM_REGVAL)(0x04U<<6))
#define WM8753_GPIO_CON_3_DRIVE_HIGH                ((WM_REGVAL)(0x05U<<6))
#define WM8753_GPIO_CON_3_SDOUT                        ((WM_REGVAL)(0x06U<<6))
#define WM8753_GPIO_CON_3_INT                        ((WM_REGVAL)(0x07U<<6))
#define WM8753_GPIO_CON_3_MASK                        ((WM_REGVAL)(0x07U<<6))

#define WM8753_GPIO_CON_2_DRIVE_LOW                    ((WM_REGVAL)(0x00U<<3))
#define WM8753_GPIO_CON_2_DRIVE_HIGH                ((WM_REGVAL)(0x01U<<3))
#define WM8753_GPIO_CON_2_SDOUT                        ((WM_REGVAL)(0x02U<<3))
#define WM8753_GPIO_CON_2_INT                        ((WM_REGVAL)(0x03U<<3))
#define WM8753_GPIO_CON_2_ADC_CLK                    ((WM_REGVAL)(0x04U<<3))
#define WM8753_GPIO_CON_2_HIFI_DAC_CLK                ((WM_REGVAL)(0x05U<<3))
#define WM8753_GPIO_CON_2_ADC_CLK_DIV_2                ((WM_REGVAL)(0x06U<<3))
#define WM8753_GPIO_CON_2_DAC_CLK_DIV_2                ((WM_REGVAL)(0x07U<<3))
#define WM8753_GPIO_CON_2_MASK                        ((WM_REGVAL)(0x07U<<3))

#define WM8753_GPIO_CON_1_DRIVE_LOW                    ((WM_REGVAL)(0x00U<<0))
#define WM8753_GPIO_CON_1_DRIVE_HIGH                ((WM_REGVAL)(0x01U<<0))
#define WM8753_GPIO_CON_1_SDOUT                        ((WM_REGVAL)(0x02U<<0))
#define WM8753_GPIO_CON_1_INT                        ((WM_REGVAL)(0x03U<<0))
#define WM8753_GPIO_CON_1_MASK                        ((WM_REGVAL)(0x07U<<0))

/*
 * Mixer volumes (0x20/0x21/0x22/0x23/0x24/0x25/0x26/0x27)
 */
#define WM8753_BASE_MIXVOL_MASK     0x07
#define WM8753_BASE_MIXVOL_6DB      0x00   /*  +6db: 000 */
#define WM8753_BASE_MIXVOL_3DB      0x01   /*  +3dB: 001 */
#define WM8753_BASE_MIXVOL_0DB      0x02   /*   0dB: 010 */
#define WM8753_BASE_MIXVOL_M3DB     0x03   /*  -3dB: 011 */
#define WM8753_BASE_MIXVOL_M6DB     0x04   /*  -6dB: 100 */
#define WM8753_BASE_MIXVOL_M9DB     0x05   /*  -9dB: 101 */
#define WM8753_BASE_MIXVOL_M12DB    0x06   /* -12dB: 110 */
#define WM8753_BASE_MIXVOL_M15DB    0x07   /* -15dB: 111 */

#define WM8753_MIXVOL_SHIFT         4   /* LM2LOVOL/RM2ROVOL/MM2MOVOL [6:4] */
#define WM8753_MIXVOL_MASK          ((WM_REGVAL)(WM8753_BASE_MIXVOL_MASK << WM8753_MIXVOL_SHIFT))
#define WM8753_MIXVOL_6DB           ((WM_REGVAL)(WM8753_BASE_MIXVOL_6DB << WM8753_MIXVOL_SHIFT))
#define WM8753_MIXVOL_3DB           ((WM_REGVAL)(WM8753_BASE_MIXVOL_3DB << WM8753_MIXVOL_SHIFT))
#define WM8753_MIXVOL_0DB           ((WM_REGVAL)(WM8753_BASE_MIXVOL_0DB << WM8753_MIXVOL_SHIFT))
#define WM8753_MIXVOL_M3DB          ((WM_REGVAL)(WM8753_BASE_MIXVOL_M3DB << WM8753_MIXVOL_SHIFT))
#define WM8753_MIXVOL_M6DB          ((WM_REGVAL)(WM8753_BASE_MIXVOL_M6DB << WM8753_MIXVOL_SHIFT))
#define WM8753_MIXVOL_M9DB          ((WM_REGVAL)(WM8753_BASE_MIXVOL_M9DB << WM8753_MIXVOL_SHIFT))
#define WM8753_MIXVOL_M12DB         ((WM_REGVAL)(WM8753_BASE_MIXVOL_M12DB << WM8753_MIXVOL_SHIFT))
#define WM8753_MIXVOL_M15DB         ((WM_REGVAL)(WM8753_BASE_MIXVOL_M15DB  << WM8753_MIXVOL_SHIFT))

#define WM8753_ST_MIXVOL_SHIFT      4   /* ST2LOVOL/ST2ROVOL/ST2MOVOL [6:4] */
#define WM8753_ST_MIXVOL_MASK       ((WM_REGVAL)(WM8753_BASE_MIXVOL_MASK << WM8753_ST_MIXVOL_SHIFT))
#define WM8753_ST_MIXVOL_6DB        ((WM_REGVAL)(WM8753_BASE_MIXVOL_6DB << WM8753_ST_MIXVOL_SHIFT))
#define WM8753_ST_MIXVOL_3DB        ((WM_REGVAL)(WM8753_BASE_MIXVOL_3DB << WM8753_ST_MIXVOL_SHIFT))
#define WM8753_ST_MIXVOL_0DB        ((WM_REGVAL)(WM8753_BASE_MIXVOL_0DB << WM8753_ST_MIXVOL_SHIFT))
#define WM8753_ST_MIXVOL_M3DB       ((WM_REGVAL)(WM8753_BASE_MIXVOL_M3DB << WM8753_ST_MIXVOL_SHIFT))
#define WM8753_ST_MIXVOL_M6DB       ((WM_REGVAL)(WM8753_BASE_MIXVOL_M6DB << WM8753_ST_MIXVOL_SHIFT))
#define WM8753_ST_MIXVOL_M9DB       ((WM_REGVAL)(WM8753_BASE_MIXVOL_M9DB << WM8753_ST_MIXVOL_SHIFT))
#define WM8753_ST_MIXVOL_M12DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_M12DB << WM8753_ST_MIXVOL_SHIFT))
#define WM8753_ST_MIXVOL_M15DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_M15DB  << WM8753_ST_MIXVOL_SHIFT))

#define WM8753_VX_MIXVOL_SHIFT      0   /* VXD2LOVOL/VXD2ROVOL/VXD2MOVOL [2:0 */
#define WM8753_VX_MIXVOL_MASK       ((WM_REGVAL)(WM8753_BASE_MIXVOL_MASK << WM8753_VX_MIXVOL_SHIFT))
#define WM8753_VX_MIXVOL_6DB        ((WM_REGVAL)(WM8753_BASE_MIXVOL_6DB << WM8753_VX_MIXVOL_SHIFT))
#define WM8753_VX_MIXVOL_3DB        ((WM_REGVAL)(WM8753_BASE_MIXVOL_3DB << WM8753_VX_MIXVOL_SHIFT))
#define WM8753_VX_MIXVOL_0DB        ((WM_REGVAL)(WM8753_BASE_MIXVOL_0DB << WM8753_VX_MIXVOL_SHIFT))
#define WM8753_VX_MIXVOL_M3DB       ((WM_REGVAL)(WM8753_BASE_MIXVOL_M3DB << WM8753_VX_MIXVOL_SHIFT))
#define WM8753_VX_MIXVOL_M6DB       ((WM_REGVAL)(WM8753_BASE_MIXVOL_M6DB << WM8753_VX_MIXVOL_SHIFT))
#define WM8753_VX_MIXVOL_M9DB       ((WM_REGVAL)(WM8753_BASE_MIXVOL_M9DB << WM8753_VX_MIXVOL_SHIFT))
#define WM8753_VX_MIXVOL_M12DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_M12DB << WM8753_VX_MIXVOL_SHIFT))
#define WM8753_VX_MIXVOL_M15DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_M15DB  << WM8753_VX_MIXVOL_SHIFT))

#define WM8753_LREC_MIXVOL_SHIFT    0   /* LRECVOL[2:0] */
#define WM8753_LREC_MIXVOL_MASK     ((WM_REGVAL)(WM8753_BASE_MIXVOL_MASK << WM8753_LREC_MIXVOL_SHIFT))
#define WM8753_LREC_MIXVOL_6DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_6DB << WM8753_LREC_MIXVOL_SHIFT))
#define WM8753_LREC_MIXVOL_3DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_3DB << WM8753_LREC_MIXVOL_SHIFT))
#define WM8753_LREC_MIXVOL_0DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_0DB << WM8753_LREC_MIXVOL_SHIFT))
#define WM8753_LREC_MIXVOL_M3DB     ((WM_REGVAL)(WM8753_BASE_MIXVOL_M3DB << WM8753_LREC_MIXVOL_SHIFT))
#define WM8753_LREC_MIXVOL_M6DB     ((WM_REGVAL)(WM8753_BASE_MIXVOL_M6DB << WM8753_LREC_MIXVOL_SHIFT))
#define WM8753_LREC_MIXVOL_M9DB     ((WM_REGVAL)(WM8753_BASE_MIXVOL_M9DB << WM8753_LREC_MIXVOL_SHIFT))
#define WM8753_LREC_MIXVOL_M12DB    ((WM_REGVAL)(WM8753_BASE_MIXVOL_M12DB << WM8753_LREC_MIXVOL_SHIFT))
#define WM8753_LREC_MIXVOL_M15DB    ((WM_REGVAL)(WM8753_BASE_MIXVOL_M15DB  << WM8753_LREC_MIXVOL_SHIFT))

#define WM8753_RREC_MIXVOL_SHIFT    4   /* RRECVOL[2:0] */
#define WM8753_RREC_MIXVOL_MASK     ((WM_REGVAL)(WM8753_BASE_MIXVOL_MASK << WM8753_RREC_MIXVOL_SHIFT))
#define WM8753_RREC_MIXVOL_6DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_6DB << WM8753_RREC_MIXVOL_SHIFT))
#define WM8753_RREC_MIXVOL_3DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_3DB << WM8753_RREC_MIXVOL_SHIFT))
#define WM8753_RREC_MIXVOL_0DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_0DB << WM8753_RREC_MIXVOL_SHIFT))
#define WM8753_RREC_MIXVOL_M3DB     ((WM_REGVAL)(WM8753_BASE_MIXVOL_M3DB << WM8753_RREC_MIXVOL_SHIFT))
#define WM8753_RREC_MIXVOL_M6DB     ((WM_REGVAL)(WM8753_BASE_MIXVOL_M6DB << WM8753_RREC_MIXVOL_SHIFT))
#define WM8753_RREC_MIXVOL_M9DB     ((WM_REGVAL)(WM8753_BASE_MIXVOL_M9DB << WM8753_RREC_MIXVOL_SHIFT))
#define WM8753_RREC_MIXVOL_M12DB    ((WM_REGVAL)(WM8753_BASE_MIXVOL_M12DB << WM8753_RREC_MIXVOL_SHIFT))
#define WM8753_RREC_MIXVOL_M15DB    ((WM_REGVAL)(WM8753_BASE_MIXVOL_M15DB  << WM8753_RREC_MIXVOL_SHIFT))

#define WM8753_MREC_MIXVOL_SHIFT    0   /* MRECVOL[2:0] */
#define WM8753_MREC_MIXVOL_MASK     ((WM_REGVAL)(WM8753_BASE_MIXVOL_MASK << WM8753_MREC_MIXVOL_SHIFT))
#define WM8753_MREC_MIXVOL_6DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_6DB << WM8753_MREC_MIXVOL_SHIFT))
#define WM8753_MREC_MIXVOL_3DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_3DB << WM8753_MREC_MIXVOL_SHIFT))
#define WM8753_MREC_MIXVOL_0DB      ((WM_REGVAL)(WM8753_BASE_MIXVOL_0DB << WM8753_MREC_MIXVOL_SHIFT))
#define WM8753_MREC_MIXVOL_M3DB     ((WM_REGVAL)(WM8753_BASE_MIXVOL_M3DB << WM8753_MREC_MIXVOL_SHIFT))
#define WM8753_MREC_MIXVOL_M6DB     ((WM_REGVAL)(WM8753_BASE_MIXVOL_M6DB << WM8753_MREC_MIXVOL_SHIFT))
#define WM8753_MREC_MIXVOL_M9DB     ((WM_REGVAL)(WM8753_BASE_MIXVOL_M9DB << WM8753_MREC_MIXVOL_SHIFT))
#define WM8753_MREC_MIXVOL_M12DB    ((WM_REGVAL)(WM8753_BASE_MIXVOL_M12DB << WM8753_MREC_MIXVOL_SHIFT))
#define WM8753_MREC_MIXVOL_M15DB    ((WM_REGVAL)(WM8753_BASE_MIXVOL_M15DB  << WM8753_MREC_MIXVOL_SHIFT))

/*
 * Record mixer routing (0x20/0x21)
 */
#define WM8753_REC_RMIX             ((WM_REGVAL)(1U << 7))   /* RSEL */
#define WM8753_REC_LMIX             ((WM_REGVAL)(1U << 3))   /* LSEL */
#define WM8753_REC_MMIX             ((WM_REGVAL)(1U << 3))   /* MSEL */

/*
 * Output mixer routing (0x22/0x23/0x24/0x25/0x26/0x27)
 */
#define WM8753_DAC2OUT              ((WM_REGVAL)(1U << 8))   /* LD2LO/RD2RO/LD2MO/RD2MO */
#define WM8753_VXDAC2OUT            WM8753_DAC2OUT           /* VXD2LO/VXD2RO */
#define WM8753_MIX2OUT              ((WM_REGVAL)(1U << 7))   /* LM2LO/RM2RO/MM2MO */
#define WM8753_ST2OUT               ((WM_REGVAL)(1U << 7))   /* ST2LO/ST2RO/ST2MO */
#define WM8753_VXD2MO               ((WM_REGVAL)(1U << 3))   /* R39 (0x27) */

/*
 * OUT volumes (0x28/0x29/0x2A/0x2B/0x2C)
 * 
 * 111_1111 = +6dB
 * ... (1.0dB steps)
 * 011_0000 = -73dB
 * 010_1111 to 000_0000 = Analogue MUTE
 * 
 * For WM8753_OUTVOL - _db should be between 6 and -73, < -73 is mute
 */
#define WM8753_OUTVOL_ZEROCROSS     ((WM_REGVAL)(1U << 7))   /* ZC */
#define WM8753_OUTVOL_MASK          0x07F       /* LOUT1VOL/ROUT1VOL/LOUT2VOL/ROUT2VOL/MONO2VOL */
#define WM8753_OUTVOL_MAX           0x7F
#define WM8753_OUTVOL_MIN           0x30
#define WM8753_OUTVOL_RANGE         (WM8753_OUTVOL_MAX - WM8753_OUTVOL_MIN)
#define WM8753_OUTVOL_0DB           0x79
#define WM8753_OUTVOL(_dB)          (WM8753_OUTVOL_0DB + (_dB))
#define WM8753_OUTVOL_MUTE          0x00

/*
 * OUTPUT_CONTROL (0x2D)
 */
#define WM8753_MONO2SEL_MASK        ((WM_REGVAL)(3U << 7))   /* MONO2SW - [8:7] */
#define WM8753_MONO2SEL_MONO1INV    ((WM_REGVAL)(0U << 7))   /* 00: MONO1 inverted */
#define WM8753_MONO2SEL_LMIX_2      ((WM_REGVAL)(1U << 7))   /* 01: LMIX/2 */
#define WM8753_MONO2SEL_RMIX_2      ((WM_REGVAL)(2U << 7))   /* 10: RMIX/2 */
#define WM8753_MONO2SEL_MIXERS_2    ((WM_REGVAL)(3U << 7))   /* 11: (LMIX+RMIX)/2 */

#define WM8753_HPSWITCHENABLE       ((WM_REGVAL)(1U << 6))   /* HPSWEN: Enable headphone switch */
#define WM8753_HPSWITCH_HEADPHONE   ((WM_REGVAL)(0U << 5))   /* HPSWPOL: GPIO4 high = headphone */     
#define WM8753_HPSWITCH_SPEAKER     ((WM_REGVAL)(1U << 5))   /* HPSWPOL: GPIO4 high = speaker */
#define WM8753_HPSWITCH_MASK        ((WM_REGVAL)(3U << 5))   /* Mask for WM8753_HPSWITCHENABLE |  */
                                                             /* WM8753_HPSWITCH_SPEAKER | WM8753_HPSWITCH_HEADPHONE  */


#define WM8753_THERMALSHUTDOWN_EN   ((WM_REGVAL)(1U << 4))   /* TSDEN: Thermal shutdown enable */

#define WM8753_ROUT2INV             ((WM_REGVAL)(1U << 2))   /* ROUT2INV */

#define WM8753_VREFOUT_500          ((WM_REGVAL)(0U << 3))   /* VROI: 500 Ohm */
#define WM8753_VREFOUT_90K          ((WM_REGVAL)(1U << 3))   /* VROI: 90 kOhm */

#define WM8753_OUT3SEL_MASK         ((WM_REGVAL)(3U << 0))   /* OUT3SW - [1:0] */
#define WM8753_OUT3SEL_VREF         ((WM_REGVAL)(0U << 0))   /* 00: VREF */
#define WM8753_OUT3SEL_ROUT2        ((WM_REGVAL)(1U << 0))   /* 01: ROUT2 */
#define WM8753_OUT3SEL_MIXERS_2     ((WM_REGVAL)(2U << 0))   /* 10: (LMIX+RMIX)/2 */

/* 
 * ADC input mode ( 0x2E )
 */
#define WM8753_MONOMIX_MASK         ((WM_REGVAL)(3U << 4))   /* MONOMIX - [5:4] */
#define WM8753_MONOMIX_STEREO       ((WM_REGVAL)(0U << 4))   /* 00: Stereo */
#define WM8753_MONOMIX_ANALOG_LEFT  ((WM_REGVAL)(1U << 4))   /* 01: Analogue Mono Mix (using left ADC) */
#define WM8753_MONOMIX_ANALOG_RIGHT ((WM_REGVAL)(2U << 4))   /* 10: Analogue Mono Mix (using right ADC) */
#define WM8753_MONOMIX_DIGITAL      ((WM_REGVAL)(3U << 4))   /* 11: Digital Mono Mix */
#define WM8753_RIGHTADCSEL_MASK     ((WM_REGVAL)(3U << 2))   /* RADCSEL - [3:2] */
#define WM8753_RIGHTADCSEL_PGA      ((WM_REGVAL)(0U << 2))   /* 00: PGA */
#define WM8753_RIGHTADCSEL_LINE2    ((WM_REGVAL)(1U << 2))   /* 01: LINE2 or RXP-RXN */
#define WM8753_RIGHTADCSEL_RXP_RXN  ((WM_REGVAL)(1U << 2))   /* 01: LINE2 or RXP-RXN */
#define WM8753_RIGHTADCSEL_LRM      ((WM_REGVAL)(2U << 2))   /* 10: Left + Right + Mono output Mix */
#define WM8753_LEFTADCSEL_MASK      ((WM_REGVAL)(3U << 0))   /* LADCSEL - [1:0] */
#define WM8753_LEFTADCSEL_PGA       ((WM_REGVAL)(0U << 0))   /* 00: PGA */
#define WM8753_LEFTADCSEL_LINE1     ((WM_REGVAL)(1U << 0))   /* 01: LINE1 or RXP-RXN */
#define WM8753_LEFTADCSEL_RXP_RXN   ((WM_REGVAL)(1U << 0))   /* 01: LINE1 or RXP-RXN */
#define WM8753_LEFTADCSEL_LINE1DC   ((WM_REGVAL)(2U << 0))   /* 10: LINE1 DC measurement */

/* 
 * Input Control 1 ( 0x2F ) 
 */
#define WM8753_MICBOOST_MASK        3U
#define WM8753_MICBOOST_12DB        0U
#define WM8753_MICBOOST_18DB        1U
#define WM8753_MICBOOST_24DB        2U
#define WM8753_MICBOOST_30DB        3U
#define WM8753_MICBOOST_MAX         WM8753_MICBOOST_30DB
#define WM8753_MICBOOST_MIN         WM8753_MICBOOST_12DB
#define WM8753_MICBOOST_RANGE       (WM8753_MICBOOST_MAX - WM8753_MICBOOST_MIN)

#define WM8753_MIC2BOOST_SHIFT      7
#define WM8753_MIC2BOOST_MASK       ((WM_REGVAL)(WM8753_MICBOOST_MASK << WM8753_MIC2BOOST_SHIFT))  /* MIC2BOOST - [8:7] */
#define WM8753_MIC2BOOST_12DB       ((WM_REGVAL)(WM8753_MICBOOST_12DB << WM8753_MIC2BOOST_SHIFT))  /* 00: +12dB */
#define WM8753_MIC2BOOST_18DB       ((WM_REGVAL)(WM8753_MICBOOST_18DB << WM8753_MIC2BOOST_SHIFT))  /* 01: +18dB */
#define WM8753_MIC2BOOST_24DB       ((WM_REGVAL)(WM8753_MICBOOST_24DB << WM8753_MIC2BOOST_SHIFT))  /* 10: +24dB */
#define WM8753_MIC2BOOST_30DB       ((WM_REGVAL)(WM8753_MICBOOST_30DB << WM8753_MIC2BOOST_SHIFT))  /* 11: +30dB */

#define WM8753_MIC1BOOST_SHIFT      5
#define WM8753_MIC1BOOST_MASK       ((WM_REGVAL)(WM8753_MICBOOST_MASK << WM8753_MIC1BOOST_SHIFT))  /* MIC1BOOST - [6:5] */
#define WM8753_MIC1BOOST_12DB       ((WM_REGVAL)(WM8753_MICBOOST_12DB << WM8753_MIC1BOOST_SHIFT))  /* 00: +12dB */
#define WM8753_MIC1BOOST_18DB       ((WM_REGVAL)(WM8753_MICBOOST_18DB << WM8753_MIC1BOOST_SHIFT))  /* 01: +18dB */
#define WM8753_MIC1BOOST_24DB       ((WM_REGVAL)(WM8753_MICBOOST_24DB << WM8753_MIC1BOOST_SHIFT))  /* 10: +24dB */
#define WM8753_MIC1BOOST_30DB       ((WM_REGVAL)(WM8753_MICBOOST_30DB << WM8753_MIC1BOOST_SHIFT))  /* 11: +30dB */

#define WM8753_LINEMIXSEL_MASK      ((WM_REGVAL)(3U << 3))  /* LMSEL - [4:3] */
#define WM8753_LINEMIXSEL_1PLUS2    ((WM_REGVAL)(0U << 3))  /* 00: LINE1 + LINE2 */
#define WM8753_LINEMIXSEL_MIX       WM8753_LINEMIXSEL_1PLUS2
#define WM8753_LINEMIXSEL_1MINUS2   ((WM_REGVAL)(1U << 3))  /* 01: LINE1 - LINE2 */
#define WM8753_LINEMIXSEL_DIFF      WM8753_LINEMIXSEL_1MINUS2
#define WM8753_LINEMIXSEL_1ONLY     ((WM_REGVAL)(2U << 3))  /* 10: LINE1 (LINE2 disconnected) */
#define WM8753_LINEMIXSEL_2ONLY     ((WM_REGVAL)(3U << 3))  /* 11: LINE2 (LINE1 disconnected) */

#define WM8753_MONOMUXSEL_MASK      ((WM_REGVAL)(1U << 2))  /* MM: [2] */
#define WM8753_MONOMUXSEL_LINE      ((WM_REGVAL)(0U << 2))  /* MM: Line Mix Output */
#define WM8753_MONOMUXSEL_RX        ((WM_REGVAL)(1U << 2))  /* MM: Rx Mix Output (RXP +/- RXN) */

#define WM8753_RIGHTMUXSEL_MASK     ((WM_REGVAL)(1U << 1))  /* RM: [1] */
#define WM8753_RIGHTMUXSEL_LINE2    ((WM_REGVAL)(0U << 1))  /* RM: LINE 2 */
#define WM8753_RIGHTMUXSEL_RX       ((WM_REGVAL)(1U << 1))  /* RM: Rx Mix Output (RXP +/- RXN) */

#define WM8753_LEFTMUXSEL_MASK      ((WM_REGVAL)(1U << 0))  /* LM: [0] */
#define WM8753_LEFTMUXSEL_LINE1     ((WM_REGVAL)(0U << 0))  /* LM: LINE 1 */
#define WM8753_LEFTMUXSEL_RX        ((WM_REGVAL)(1U << 0))  /* LM: Rx Mix Output (RXP +/- RXN) */

/* 
 * Input Control 2 (0x30) 
 */
#define WM8753_RXMSEL_MASK          ((WM_REGVAL)(3U << 6))  /* RXMSEL - [7:6] */
#define WM8753_RXMSEL_RXP_MINUS_RXN ((WM_REGVAL)(0U << 6))  /* 00: RXP - RXN */
#define WM8753_RXMSEL_RXP_PLUS_RXN  ((WM_REGVAL)(1U << 6))  /* 01: RXP + RXN */
#define WM8753_RXMSEL_RXP           ((WM_REGVAL)(2U << 6))  /* 10: RXP (RXN disconnected) */
#define WM8753_RXMSEL_RXN           ((WM_REGVAL)(3U << 6))  /* 11: RXN (RXP disconnected) */
#define WM8753_MICMUX_MASK          ((WM_REGVAL)(3U << 4))  /* MICMUX - [5:4] */
#define WM8753_MICMUX_LEFT_PGA      ((WM_REGVAL)(0U << 4))  /* 00: Sidetone = Left PGA output */
#define WM8753_MICMUX_MIC1          ((WM_REGVAL)(1U << 4))  /* 01: Sidetone = Mic 1 preamp output */
#define WM8753_MICMUX_MIC2          ((WM_REGVAL)(2U << 4))  /* 10: Sidetone = Mic 2 preamp output */
#define WM8753_MICMUX_RIGHT_PGA     ((WM_REGVAL)(3U << 4))  /* 11: Sidetone = Right PGA output */
#define WM8753_LINEALC              ((WM_REGVAL)(1U << 3))  /* LINEALC: Line mix selected into ALC mix */
#define WM8753_MIC2ALC              ((WM_REGVAL)(1U << 2))  /* MIC2ALC: Mic 2 selected into ALC mix */
#define WM8753_MIC1ALC              ((WM_REGVAL)(1U << 1))  /* MIC1ALC: Mic 1 selected into ALC mix */
#define WM8753_RXALC                ((WM_REGVAL)(1U << 0))  /* RXALC: RX selected into ALC mix */

/*
 * IN volumes (0x31/0x32)
 * 
 * 11_1111 = +30dB
 * ... (0.75dB steps)
 * 00_0000 = -17.25dB
 * 
 */
#define WM8753_INVOL_MUTE           ((WM_REGVAL)(1U << 7))  /* LINMUTE/RINMUTE */
#define WM8753_INVOL_ZEROCROSS      ((WM_REGVAL)(1U << 6))  /* LZCEN/RZCEN */
#define WM8753_INVOL_MASK           0x03F                   /* LINVOL/RINVOL */
#define WM8753_INVOL_MAX            0x3F
#define WM8753_INVOL_MIN            0x00
#define WM8753_INVOL_RANGE          (WM8753_INVOL_MAX - WM8753_INVOL_MIN)
#define WM8753_INVOL_0DB            0x17
#define WM8753_INVOL(_dB)           (WM8753_INVOL_0DB + ( (4*(_dB)) / 3 ) )

/* 
 * Mic Bias comp control ( 0x33 ) 
 */
 
/*
 * CLOCK control (0x34)
 */
#define WM8753_VXCLK_DIV_MASK       ((WM_REGVAL)(7U << 6))   /* PCMDIV - [8:6] */
#define WM8753_VXCLK_DIV_1          ((WM_REGVAL)(0U << 6))   /* 000: divide by 1 */
#define WM8753_VXCLK_DIV_2          ((WM_REGVAL)(4U << 6))   /* 100: divide by 2 */
#define WM8753_VXCLK_DIV_3          ((WM_REGVAL)(2U << 6))   /* 010: divide by 3 */
#define WM8753_VXCLK_DIV_4          ((WM_REGVAL)(5U << 6))   /* 101: divide by 4 */
#define WM8753_VXCLK_DIV_5_5        ((WM_REGVAL)(3U << 6))   /* 011: divide by 5.5 */
#define WM8753_VXCLK_DIV_6          ((WM_REGVAL)(6U << 6))   /* 110: divide by 6 */
#define WM8753_VXCLK_DIV_8          ((WM_REGVAL)(7U << 6))   /* 111: divide by 8 */

#define WM8753_SWITCH_MCLK          ((WM_REGVAL)(0U << 5))   /* SLWCLK - timeout and headphone switch from MCLK */
#define WM8753_SWITCH_PCMCLK        ((WM_REGVAL)(1U << 5))   /* SLWCLK - timeout and headphone switch from PCMCLK */

#define WM8753_MCLKSEL_MASK         ((WM_REGVAL)(1U << 4))   /* MCLKSEL: [4] */
#define WM8753_MCLKSEL_PCMCLK       ((WM_REGVAL)(0U << 4))   /* MCLKSEL: Master clock for HIFI from MCLK pin */
#define WM8753_MCLKSEL_PLL1         ((WM_REGVAL)(1U << 4))   /* MCLKSEL: Master clock for HIFI from PLL1 (instead of MCLK pin) */
#define WM8753_PCMCLKSEL_MASK       ((WM_REGVAL)(1U << 3))   /* PCMCLKSEL: [3] */
#define WM8753_PCMCLKSEL_PCMCLK     ((WM_REGVAL)(0U << 3))   /* PCMCLKSEL: Master clock for Voice from MCLK pin */
#define WM8753_PCMCLKSEL_PLL2       ((WM_REGVAL)(1U << 3))   /* PCMCLKSEL: Master clock for Voice from PLL2 (instead of MCLK pin) */
#define WM8753_VXCLOCK_MASK         ((WM_REGVAL)(1U << 2))   /* CLKEQ: [2] */
#define WM8753_VXCLOCK_PLL2         ((WM_REGVAL)(0U << 2))   /* CLKEQ: Voice clock from PCMCLK or PLL2 */
#define WM8753_VXCLOCK_PCM          ((WM_REGVAL)(0U << 2))   /* CLKEQ: Voice clock from PCMCLK or PLL2 */
#define WM8753_VXCLOCK_HIFI         ((WM_REGVAL)(1U << 2))   /* CLKEQ: Voice clock same as HiFi clock */
#define WM8753_GP1CLK1_MASK         ((WM_REGVAL)(1U << 1))   /* GP1CLK1SEL: [1] */
#define WM8753_GP1CLK1_CLOCK        ((WM_REGVAL)(1U << 1))   /* GP1CLK1SEL: 0 = GP1, 1 = CLK1 */
#define WM8753_GP1CLK1_GPIO         ((WM_REGVAL)(0U << 1))   /* GP1CLK1SEL: 0 = GP1, 1 = CLK1 */
#define WM8753_GP2CLK2_MASK         ((WM_REGVAL)(1U << 0))   /* GP2CLK2SEL: [0] */
#define WM8753_GP2CLK2_CLOCK        ((WM_REGVAL)(1U << 0))   /* GP2CLK2SEL: 0 = GP1, 1 = CLK1 */
#define WM8753_GP2CLK2_GPIO         ((WM_REGVAL)(0U << 0))   /* GP2CLK2SEL: 0 = GP1, 1 = CLK1 */

/*
 * PLL CONTROL1 (PLL1:0x35/PLL2:0x39)
 */
#define WM8753_CLOCKOUT_MCLK        ((WM_REGVAL)(0U << 5))   /* CLK1SEL/CLK2SEL: CLKOUT from MCLK */
#define WM8753_CLOCKOUT_PLL         ((WM_REGVAL)(1U << 5))   /* CLK1SEL/CLK2SEL: CLKOUT from PLL */
#define WM8753_PLLCLOCK_DIV2        ((WM_REGVAL)(1U << 4))   /* CLK1DIV2/CLK2DIV2: CLKOUT divide by 2 */
#define WM8753_MCLK_DIV2            ((WM_REGVAL)(1U << 3))   /* MCLK1DIV2/MCLK2DIV2: MCLK divide by 2 */
#define WM8753_PLLOUT_DIV2          ((WM_REGVAL)(1U << 2))   /* PLL1DIV2/PLL2DIV2: PLL output divide by 2 */
#define WM8753_PLL_ACTIVE           ((WM_REGVAL)(1U << 1))   /* PLL1RB/PLL2RB: "reset bar" = PLL active */
#define WM8753_PLL_ENABLE           ((WM_REGVAL)(1U << 0))   /* PLL1EN/PLL2EN: PLL enabled */

/*
 * PLL CONTROL2/3/4 (PLL1: 0x36/7/8 & PLL2: 0x3A/B/C)
 */
#define WM8753_PLL_N_MASK           ((WM_REGVAL)(0xFU << 5)) /* PLL1N/PLL2N - [8:5] - in reg 2*/
#define WM8753_PLL_N(_n)            ((WM_REGVAL)((_n) << 5))
#define WM8753_PLL_K2_MASK          0x00F       /* PLL1K/PLL2K[21:18] - in reg 2 */
#define WM8753_PLL_K2(_k)           (((_k) & 0x3C0000) >> 18) /* reg 2 bits - [21:18] */
#define WM8753_PLL_K3(_k)           (((_k) & 0x03FE00) >> 9)  /* reg 3 bits - [17:9] */
#define WM8753_PLL_K4(_k)           (((_k) & 0x0001FF))       /* reg 4 bits - [8:0] */
#define WM8753_PLL_R2VAL(_n, _k)    (WM8753_PLL_N(_n) | WM8753_PLL_K2(_k))
#define WM8753_PLL_R3VAL(_n, _k)    (WM8753_PLL_K3(_k))
#define WM8753_PLL_R4VAL(_n, _k)    (WM8753_PLL_K4(_k))

/*
 * 0x3D
#define WM8753_BIAS_CONTROL                         0x3D
 */

/*
 * 0x3E is undefined
 */
 
/*
 * ADD_CONTROL_2 (0x3F)
 */
#define WM8753_OUT4SEL_MASK         ((WM_REGVAL)(3U << 7))   /* OUT4SW - [8:7] */
#define WM8753_OUT4SEL_VREF         ((WM_REGVAL)(0U << 7))   /* 00: VREF */
#define WM8753_OUT4SEL_RECMIX       ((WM_REGVAL)(1U << 7))   /* 01: Record Mixer */
#define WM8753_OUT4SEL_LOUT2        ((WM_REGVAL)(2U << 7))   /* 10: LOUT2 */

#define WM8753_THERMALSHUT_OUTPUTS  ((WM_REGVAL)(1U << 6))   /* TSDADEN: Thermal shutdown shuts down   */
                                                /* speaker and headphone outputs */

#define WM8753_FULL_OUTPUT_BIAS     ((WM_REGVAL)(0U << 1))   /* OPBIASX0P5: 1x Analogue output bias current */
#define WM8753_HALF_OUTPUT_BIAS     ((WM_REGVAL)(1U << 1))   /* OPBIASX0P5: 0.5x Analogue output bias current */
#define WM8753_FULL_DAC_MIXER_BIAS  ((WM_REGVAL)(0U << 0))   /* DMBIASX0P5: 1x DAC/Mixer bias current */
#define WM8753_HALF_DAC_MIXER_BIAS  ((WM_REGVAL)(1U << 0))   /* DMBIASX0P5: 0.5x DAC/Mixer bias current */




unsigned int WM8753_Codec_Init_Table[][2] =
{
#if 1
    { 0x1f, 0x000 }, // R31
    { 0x01, 0x000 }, // R01
    { 0x02, 0x000 }, // R02
    { 0x03, 0x00a }, // R03
    { 0x04, 0x002 }, // R04
//    { 0x05, 0x033 }, // R05
    { 0x05, 0x008 }, // R05
//    { 0x05, 0x00b }, // R05 Interface Control IFMODE=10:Hifi, VXFSOE=0, LRCOE=0
//    { 0x06, 0x038 }, // R06
    { 0x06, 0x022 }, // R06 44.1 on 384fs
    { 0x07, 0x0a7 }, // R07
    { 0x08, 0x1ff }, // R08
    { 0x09, 0x1ff }, // R09
    { 0x0a, 0x00f }, // R10
    { 0x0b, 0x00f }, // R11

    // for line input, ALC off
    { 0x0c, 0x07b }, // R12, ALC Control 1, ALCSEL 0:off,
    { 0x0d, 0x000 }, // R13
    { 0x0e, 0x032 }, // R14
    { 0x0f, 0x000 }, // R15
    { 0x10, 0x1c3 }, // R16
    { 0x11, 0x1c3 }, // R17
    { 0x12, 0x0c0 }, // R18
    { 0x13, 0x000 }, // R19
//    { 0x14, 0x0cc }, // R20 Power Management 1, VMIDSEL=50kOhm(01),VREF,DACL,DACR
    { 0x14, 0x0ec }, // R20 Power Management 1, VMIDSEL=50kOhm(01),VREF,MICB,DACL,DACR
//    { 0x15, 0x000 }, // R21 Power Management 2, All Off

#if 0
    { 0x15, 0x1ff },         // R21 Pwr_mgt2    all enable
    { 0x16, 0x1ff },         // R22 Pwr_mgt3
    { 0x17, 0x1ff },         // R23 Pwr_mgt4
#else
    { 0x15, 0x00d }, // R21 Power Management 2, ADCL, ADCR, LINEMIX
    { 0x16, 0x180 }, // R22 Power Management 3, LOUT1, ROUT1
    { 0x17, 0x003 }, // R23 Power Management 4, RIGHTMIX, LEFTMIX
#endif

    { 0x18, 0x000 }, // R24
    { 0x19, 0x000 }, // R25
    { 0x1a, 0x000 }, // R26
    { 0x1b, 0x000 }, // R27
    { 0x1c, 0x000 }, // R28
    { 0x20, 0x055 }, // R32
    { 0x21, 0x005 }, // R33
    { 0x22, 0x150 }, // R34
    { 0x23, 0x055 }, // R35
    { 0x24, 0x150 }, // R36
    { 0x25, 0x055 }, // R37
    { 0x26, 0x050 }, // R38
    { 0x27, 0x055 }, // R39
    { 0x28, 0x15f }, // R40
    { 0x29, 0x15f }, // R41
    { 0x2a, 0x15f }, // R42
    { 0x2b, 0x15f }, // R43
    { 0x2c, 0x079 }, // R44
    { 0x2d, 0x004 }, // R45
//    { 0x2e, 0x000 }, // R46
    { 0x2e, 0x005 }, // R46 ADC Input Mode, Stereo, LINE1, LINE2
    { 0x2f, 0x000 }, // R47 Input Control, LMSEL=LINE1+LINE2, MM=LineMixOutput, RM=LINE2, LM=LINE1
    { 0x30, 0x000 }, // R48
    { 0x31, 0x13f }, // R49 Left Input Volume
    { 0x32, 0x13f }, // R50 Right Input Volume
    { 0x33, 0x000 }, // R51 Mic Bias comp control
    { 0x34, 0x004 }, // R52
    { 0x35, 0x000 }, // R53
    { 0x36, 0x083 }, // R54
    { 0x37, 0x024 }, // R55
    { 0x38, 0x1ba }, // R56
    { 0x39, 0x000 }, // R57
    { 0x3a, 0x083 }, // R58
    { 0x3b, 0x024 }, // R59
    { 0x3c, 0x1ba }, // R60
    { 0x3d, 0x000 }, // R61
    { 0x3f, 0x000 }, // R62

#else
    { 0x1f, 0x000 }, // R31
    { 0x01, 0x000 }, // R01
    { 0x02, 0x000 }, // R02
    { 0x03, 0x00a }, // R03
    { 0x04, 0x002 }, // R04
    { 0x05, 0x033 }, // R05
//    { 0x06, 0x038 }, // R06
    { 0x06, 0x022 }, // R06 44.1 on 384fs
    { 0x07, 0x0a7 }, // R07
    { 0x08, 0x1ff }, // R08
    { 0x09, 0x1ff }, // R09
    { 0x0a, 0x00f }, // R10
    { 0x0b, 0x00f }, // R11
    { 0x0c, 0x07b }, // R12
    { 0x0d, 0x000 }, // R13
    { 0x0e, 0x032 }, // R14
    { 0x0f, 0x000 }, // R15
    { 0x10, 0x0c3 }, // R16
    { 0x11, 0x0c3 }, // R17
    { 0x12, 0x0c0 }, // R18
    { 0x13, 0x000 }, // R19
    { 0x14, 0x0cc }, // R20 Power Management 1, VMIDSEL=50kOhm(01),VREF,DACL,DACR
//    { 0x14, 0x0ec }, // R20 Power Management 1, VMIDSEL=50kOhm(01),VREF,MICB,DACL,DACR
//    { 0x15, 0x000 }, // R21 Power Management 2, All Off
    { 0x15, 0x00d }, // R21 Power Management 2, ADCL, ADCR, LINEMIX
    { 0x16, 0x180 }, // R22 Power Management 3, LOUT1, ROUT1
    { 0x17, 0x003 }, // R23 Power Management 4, RIGHTMIX, LEFTMIX
    { 0x18, 0x000 }, // R24
    { 0x19, 0x000 }, // R25
    { 0x1a, 0x000 }, // R26
    { 0x1b, 0x000 }, // R27
    { 0x1c, 0x000 }, // R28
    { 0x20, 0x055 }, // R32
    { 0x21, 0x005 }, // R33
    { 0x22, 0x150 }, // R34
    { 0x23, 0x055 }, // R35
    { 0x24, 0x150 }, // R36
    { 0x25, 0x055 }, // R37
    { 0x26, 0x050 }, // R38
    { 0x27, 0x055 }, // R39
    { 0x28, 0x15f }, // R40
    { 0x29, 0x15f }, // R41
    { 0x2a, 0x15f }, // R42
    { 0x2b, 0x15f }, // R43
    { 0x2c, 0x079 }, // R44
    { 0x2d, 0x004 }, // R45
//    { 0x2e, 0x000 }, // R46
    { 0x2e, 0x005 }, // R46 ADC Input Mode, Stereo, LINE1, LINE2
    { 0x2f, 0x000 }, // R47 Input Control, LMSEL=LINE1+LINE2, MM=LineMixOutput, RM=LINE2, LM=LINE1
    { 0x30, 0x000 }, // R48
    { 0x31, 0x137 }, // R49 Left Input Volume
    { 0x32, 0x137 }, // R50 Right Input Volume
    { 0x33, 0x000 }, // R51 Mic Bias comp control
    { 0x34, 0x004 }, // R52
    { 0x35, 0x000 }, // R53
    { 0x36, 0x083 }, // R54
    { 0x37, 0x024 }, // R55
    { 0x38, 0x1ba }, // R56
    { 0x39, 0x000 }, // R57
    { 0x3a, 0x083 }, // R58
    { 0x3b, 0x024 }, // R59
    { 0x3c, 0x1ba }, // R60
    { 0x3d, 0x000 }, // R61
    { 0x3f, 0x000 }, // R62
#endif
};


#endif   /* __WM8753REGISTERDEFS_H__ */
/*------------------------------ END OF FILE ---------------------------------*/
