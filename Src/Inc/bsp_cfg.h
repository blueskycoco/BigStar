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
//------------------------------------------------------------------------------
//
//  File:  bsp_cfg.h
//
//  This file contains system constant specific for SMDK6410 board.
//
#ifndef __BSP_CFG_H
#define __BSP_CFG_H

//------------------------------------------------------------------------------
//
//  Define:  BSP_DEVICE_PREFIX
//
//  Prefix used to generate device name for bootload/KITL
//
#define BSP_DEVICE_PREFIX       "SMDK6410"        // Device name prefix


//------------------------------------------------------------------------------
// Define: SYNCMODE
//
// SYNCMODE used to set cpu operation mode to syncronous mode or asyncronous mode
//------------------------------------------------------------------------------
#define SYNCMODE        (TRUE)

//------------------------------------------------------------------------------
// Define : PRESET_CLOCK
//
// Use Predefined CLOCK SETTING.
//------------------------------------------------------------------------------
#define PRESET_CLOCK    (TRUE)

//------------------------------------------------------------------------------
// CPU Revision (S3C6410 has EVT0, EVT1)
//------------------------------------------------------------------------------
#define EVT0            (36410100)
#define EVT1            (36410101)
#define CPU_REVISION    (EVT1)

//------------------------------------------------------------------------------
// Predefined System Clock setting selection
// Here are samples for clock setting that already tested.
// For S3C410, ARM 533Mhz, SystemBus 133Mhz is recommended.
// This values is only used on Driver written in C
//------------------------------------------------------------------------------
#define FIN             12000000

#if PRESET_CLOCK
#define CLK_25MHz        25000000
#define CLK_50MHz        50000000
#define CLK_33_25MHz     33250000
#define CLK_66_5MHz      66500000
#define CLK_96MHz        96000000
#define CLK_100MHz       100000000
#define CLK_133MHz       133000000
#define CLK_1333MHz      133333333
#define CLK_133_2MHz     133200000
#define CLK_150MHz       150000000
#define CLK_200MHz       200000000
#define CLK_233MHz       233000000
#define CLK_266MHz       266000000
#define CLK_266_4MHz     266400000
#define CLK_300MHz       300000000
#define CLK_400MHz       400000000
#define CLK_450MHz       450000000
#define CLK_532MHz       532000000
#define CLK_600MHz       600000000
#define CLK_634MHz       634000000
#define CLK_1332MHz      1332000000
#define CLK_666MHz       666000000
#define CLK_667MHz       667000000
#define CLK_798MHz       798000000
#define CLK_800MHz       800000000
#define CLK_900MHz       900000000

// Change This Definition to choose BSP Clock !!! (and "s3c6410.inc")
//#define TARGET_ARM_CLK    CLK_66_5MHz      //< Sync 66.5:66.5:66.5 HCLKx2=266
//#define TARGET_ARM_CLK    CLK_133MHz      //< Sync 133:133:66.5 HCLKx2=266
//#define TARGET_ARM_CLK    CLK_266MHz      //< Sync 266:133:66.5 HCLKx2=266
//#define TARGET_ARM_CLK    CLK_400MHz      //< Async 400:100:50 HCLKx2=200
//#define TARGET_ARM_CLK    CLK_450MHz      //< Sync 450:150:37.5 HCLKx2=300
//mf #define TARGET_ARM_CLK    CLK_532MHz      //< Sync 532:133:66.5 HCLKx2=266, Async is same
//#define TARGET_ARM_CLK    CLK_600MHz        //< Sync 600:150:75 HCLKx2=300
//#define TARGET_ARM_CLK    CLK_634MHz      //< Async 634:133:66.5 HCLKx2=266
//#define TARGET_ARM_CLK    CLK_666MHz      //< Sync 666:133.4:66.6 HCLKx2=266.4, Async 666:133:66.5 HCLKx2=266
//#define TARGET_ARM_CLK    CLK_798MHz      //< Sync 798:133:66.5  HCLKx2=266
#define TARGET_ARM_CLK    CLK_800MHz      //< Sync 800:133.33:66.6 HCLKx2=266.66, ASync 800:133:66.5  HCLKx2=266
//#ifdef BOOTLOADER
//#define TARGET_ARM_CLK    CLK_532MHz
//#else
//#define TARGET_ARM_CLK    CLK_900MHz      //< Sync 900:150:75, HCLKx2=300
//#endif
/// MPLL Setting
#if (TARGET_ARM_CLK == CLK_400MHz)
#define MPLL_CLK            (CLK_200MHz)
#else   // 532, 634, 666, 800, 900
#define MPLL_CLK            (CLK_266MHz)
#endif
#define MPLL_DIV            2
#define S3C6410_DoutMPLL    (MPLL_CLK/MPLL_DIV)     // 100 Mhz or 133Mhz


#if (TARGET_ARM_CLK == CLK_666MHz && SYNCMODE) || (TARGET_ARM_CLK == CLK_450MHz) || (TARGET_ARM_CLK == CLK_266MHz)
#define APLL_CLK            (TARGET_ARM_CLK*2)
#elif (TARGET_ARM_CLK == CLK_133MHz)
#define APLL_CLK            (TARGET_ARM_CLK*4)
#elif (TARGET_ARM_CLK == CLK_66_5MHz)
#define APLL_CLK            (TARGET_ARM_CLK*8)
#else
#define APLL_CLK            (TARGET_ARM_CLK)
#endif

#if (TARGET_ARM_CLK == CLK_450MHz) || ((TARGET_ARM_CLK == CLK_666MHz) && SYNCMODE) || (TARGET_ARM_CLK == CLK_266MHz)
#define APLL_DIV            2
#elif (TARGET_ARM_CLK == CLK_133MHz)
#define APLL_DIV            4
#elif (TARGET_ARM_CLK == CLK_66_5MHz)
#define APLL_DIV            8
#else
#define APLL_DIV            1
#endif
#define HCLK_DIV            2
#define PCLK_DIV            4


/// APLL and A:H:P CLK configuration
#if (SYNCMODE)
    #if (TARGET_ARM_CLK == CLK_666MHz) && (CPU_REVISION == EVT1)
        #define HCLKx2_DIV          5    // sync
    #elif (TARGET_ARM_CLK == CLK_532MHz) || (TARGET_ARM_CLK == CLK_600MHz) || (TARGET_ARM_CLK == CLK_266MHz) || (TARGET_ARM_CLK == CLK_133MHz)
        #define HCLKx2_DIV          2    // sync
    #elif (TARGET_ARM_CLK == CLK_798MHz) || (TARGET_ARM_CLK == CLK_900MHz) || (TARGET_ARM_CLK == CLK_450MHz) || (TARGET_ARM_CLK == CLK_800MHz)
        #define HCLKx2_DIV          3    // sync
    #elif (TARGET_ARM_CLK == CLK_66_5MHz)
        #define HCLKx2_DIV          4    // sync
    #endif
#else   // 400Mhz, 532Mhz, 634Mhz, 666Mhz
#define HCLKx2_DIV          1    // Async
#endif

#define S3C6410_ACLK        (APLL_CLK/APLL_DIV)

#if (SYNCMODE)
    #define S3C6410_HCLKx2      (APLL_CLK/HCLKx2_DIV)
#else
    #define S3C6410_HCLKx2      (MPLL_CLK/HCLKx2_DIV)
#endif
#define S3C6410_HCLK        (S3C6410_HCLKx2/HCLK_DIV)
#define S3C6410_PCLK        (S3C6410_HCLKx2/PCLK_DIV)

#else   // PRESET_CLOCK = FALSE
#define APLLVALUE    (((S3C6410_SYSCON_REG*)(S3C6410_BASE_REG_PA_SYSCON))->APLL_CON)
#define MPLLVALUE    (((S3C6410_SYSCON_REG*)(S3C6410_BASE_REG_PA_SYSCON))->MPLL_CON)
#define CLKDIV      (((S3C6410_SYSCON_REG*)(S3C6410_BASE_REG_PA_SYSCON))->CLKD_IV0)

#endif  // PRESET_CLOCK


//------------------------------------------------------------------------------
// SMDK6410 EPLL Output Frequency
//------------------------------------------------------------------------------
//#define S3C6410_ECLK        (CLK_96MHz)        // 96 MHz         for USB Host, SD/HSMMC..
#define S3C6410_ECLK        (84666667)        // 84,666,667 Hz     for IIS Sampling Rate 44.1 KHz (384fs)
//#define S3C6410_ECLK        (92160000)        // 92,160,000 Hz     for IIS Sampling Rate 48 KHz (384fs)

//------------------------------------------------------------------------------
// System Tick Timer Definition
//------------------------------------------------------------------------------
// For Precision of System timer
// Use timer counter as large as possible. (32-bit Counter)
// Use timer divider as small as possible.
#define SYS_TIMER_PRESCALER     (2)    // PCLK / 2 (Do not use Prescaler as 1)
#define SYS_TIMER_DIVIDER       (1)
#define TICK_PER_SEC            (1000)
#define OEM_COUNT_1MS           (S3C6410_PCLK/SYS_TIMER_PRESCALER/SYS_TIMER_DIVIDER/TICK_PER_SEC)
#define RESCHED_PERIOD          (1)

#define PWM0_1_PRESCALER    ((int)(S3C6410_PCLK/1000000)-1)
#define PWM1_DIVIDER        1   //PWM1 for backlight(1Count:2us)


//------------------------------------------------------------------------------
// SMDK6410 Display Dimension
//------------------------------------------------------------------------------
#define LCD_MODULE_LTS222       (0)    // Portrait 2.2" QVGA RGB16
#define LCD_MODULE_LTV350       (1)    // Landscape 3.5" QVGA RGB16
#define LCD_MODULE_LTE480       (2)    // Landscape 4.8" WVGA RGB16
#define LCD_MODULE_EMUL48_D1    (3)    // Landscape 4.8" WVGA RGB16 as D1 (720x480)
#define LCD_MODULE_EMUL48_QV    (4)    // Landscape 4.8" WVGA RGB16 as QVGA (320x240)
#define LCD_MODULE_EMUL48_PQV   (5)    // Landscape 4.8" WVGA RGB16 as PQVGA (240x320)
#define LCD_MODULE_EMUL48_ML    (6)    // Landscape 4.8" WVGA RGB16 as 480x320
#define LCD_MODULE_EMUL48_MP    (7)    // Landscape 4.8" WVGA RGB16 as 320x480
#define LCD_MODULE_LTP700       (8)    // Landscape 7" WVGA RGB24
#define LCD_MODULE_LTM030DK     (9)     // Portrait 3.5" WVGA RGB16
#define LCD_MODULE_QC430     (10)     // Portrait 3.5" WVGA RGB16
#define LCD_MODULE_ZQ65RGB (11) //for 640*480
#define LCD_TYPE_PORTRAIT        (0)
#define LCD_TYPE_LANDSCAPE        (1)

#define SMDK6410_LCD_MODULE    (LCD_MODULE_ZQ65RGB)

#if (SMDK6410_LCD_MODULE == LCD_MODULE_LTS222)
#define LCD_WIDTH            240
#define LCD_HEIGHT            320
#define LCD_BPP                16
#define LCD_TYPE            LCD_TYPE_PORTRAIT
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_LTV350)
#define LCD_WIDTH            240//320
#define LCD_HEIGHT            320//240
#define LCD_BPP                16
#define LCD_TYPE            LCD_TYPE_LANDSCAPE
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_LTE480)
#define LCD_WIDTH            800
#define LCD_HEIGHT            480
#define LCD_BPP                16
#define LCD_TYPE            LCD_TYPE_LANDSCAPE
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_D1)
#define LCD_WIDTH            720
#define LCD_HEIGHT            480
#define LCD_BPP                16
#define LCD_TYPE            LCD_TYPE_LANDSCAPE
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_QV)
#define LCD_WIDTH            320
#define LCD_HEIGHT            240
#define LCD_BPP                16
#define LCD_TYPE            LCD_TYPE_LANDSCAPE
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_PQV)
#define LCD_WIDTH            240
#define LCD_HEIGHT            320
#define LCD_BPP                16
#define LCD_TYPE            LCD_TYPE_PORTRAIT
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_ML)
#define LCD_WIDTH            480
#define LCD_HEIGHT            320
#define LCD_BPP                16
#define LCD_TYPE            LCD_TYPE_LANDSCAPE
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_MP)
#define LCD_WIDTH            320
#define LCD_HEIGHT            480
#define LCD_BPP                16
#define LCD_TYPE            LCD_TYPE_PORTRAIT
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_LTP700)
#define LCD_WIDTH            800
#define LCD_HEIGHT            480
#define LCD_BPP                32    // rgb888 XRGB
#define LCD_TYPE            LCD_TYPE_LANDSCAPE
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_LTM030DK)
#define LCD_WIDTH            480
#define LCD_HEIGHT           800
#define LCD_BPP              16     // rgb888 XRGB
#define LCD_TYPE            LCD_TYPE_PORTRAIT
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_QC430)
#define LCD_WIDTH            240
#define LCD_HEIGHT           320
#define LCD_BPP              16     // rgb888 XRGB
#define LCD_TYPE            LCD_TYPE_PORTRAIT
#elif (SMDK6410_LCD_MODULE == LCD_MODULE_ZQ65RGB)
#define LCD_WIDTH            640
#define LCD_HEIGHT           480
#define LCD_BPP              16     // rgb888 XRGB
#define LCD_TYPE             LCD_TYPE_LANDSCAPE//LCD_TYPE_PORTRAIT//LCD_TYPE_LANDSCAPE PORTAIT is shuping , LANDSCAPE is hengping
#else
#error LCD_MODULE_UNDEFINED_ERROR
#endif

//------------------------------------------------------------------------------
// SMDK6410 Audio Sampling Rate
//------------------------------------------------------------------------------
#define AUDIO_44_1KHz        (44100)
#define AUDIO_48KHz            (48000)
#define AUDIO_SAMPLE_RATE    (AUDIO_44_1KHz)        // Keep sync with EPLL Fout

//------------------------------------------------------------------------------
// SMDK6410 UART Debug Port Baudrate
//------------------------------------------------------------------------------
#define DEBUG_UART0         (0)
#define DEBUG_UART1         (1)
#define DEBUG_UART2         (2)
#define DEBUG_UART3         (3)
#define DEBUG_BAUDRATE      (115200)

//------------------------------------------------------------------------------
// SMDK6410 NAND Flash Timing Parameter
// Need to optimize for each HClock value.
// Default vaule is 7.7.7. this value has maximum margin.
// so stable but performance cannot be max.
//------------------------------------------------------------------------------
#define NAND_TACLS         (7)
#define NAND_TWRPH0        (7)
#define NAND_TWRPH1        (7)

//------------------------------------------------------------------------------
// SMDK6410 Static SYSINTR Definition
//------------------------------------------------------------------------------
#define SYSINTR_OHCI     (SYSINTR_FIRMWARE+1)    // for USB Host

//------------------------------------------------------------------------------
// SMDK6410 BSP Debug Option
//------------------------------------------------------------------------------
//#define REMOVE_BEFORE_RELEASE        // Define this will remove debug message or somthing...

//------------------------------------------------------------------------------
// SteppingStone Size
//------------------------------------------------------------------------------
#define STEPLDR_LENGTH      0x2800 // 8KB

//------------------------------------------------------------------------------
// SMDK6410 Keypad Layout
//------------------------------------------------------------------------------
#define LAYOUT0            (0)        // 8*8 Keyppad board
#define LAYOUT1            (1)        // On-Board Key
#define LAYOUT2            (2)        // Qwerty Key board
#define MATRIX_LAYOUT    (LAYOUT1)

#endif

