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
//  File:  image_cfg.h
//
//  Defines configuration parameters used to create the NK and Bootloader
//  program images.
//
#ifndef __IMAGE_CFG_H
#define __IMAGE_CFG_H

#if __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//  RESTRICTION
//
//  This file is a configuration file. It should ONLY contain simple #define
//  directives defining constants. This file is included by other files that
//  only support simple substitutions.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  NAMING CONVENTION
//
//  The IMAGE_ naming convention ...
//
//  IMAGE_<NAME>_<SECTION>_<MEMORY_DEVICE>_[OFFSET|SIZE|START|END]
//
//      <NAME>          - WINCE, BOOT, SHARE
//      <SECTION>       - section name: user defined
//      <MEMORY_DEVICE> - the memory device the block resides on
//      OFFSET          - number of bytes from memory device start address
//      SIZE            - maximum size of the block
//      START           - start address of block    (device address + offset)
//      END             - end address of block      (start address  + size - 1)
//
//------------------------------------------------------------------------------

// DRAM Base Address
#ifdef SMDK6410_X5D
#define DRAM_BASE_PA_START                  (0x60000000)
#else
#define DRAM_BASE_PA_START					(0x50000000)
#endif
#define DRAM_BASE_CA_START					(0x80000000)
#define DRAM_BASE_UA_START					(0xA0000000)

//------------------------------------------------------------------------------

// BSP ARGs Area
#define IMAGE_SHARE_ARGS_OFFSET				(0x00020000)
#define IMAGE_SHARE_ARGS_PA_START			(DRAM_BASE_PA_START+IMAGE_SHARE_ARGS_OFFSET)
#define IMAGE_SHARE_ARGS_CA_START			(DRAM_BASE_CA_START+IMAGE_SHARE_ARGS_OFFSET)
#define IMAGE_SHARE_ARGS_UA_START			(DRAM_BASE_UA_START+IMAGE_SHARE_ARGS_OFFSET)
#define IMAGE_SHARE_ARGS_SIZE				(0x00000800)

//------------------------------------------------------------------------------

// Display Frame Buffer
#ifdef SMDK6410_X5D
#define IMAGE_FRAMEBUFFER_OFFSET			(0x03000000)
#else
#define IMAGE_FRAMEBUFFER_OFFSET			(0x0E800000)
#endif
#define IMAGE_FRAMEBUFFER_PA_START			(DRAM_BASE_PA_START+IMAGE_FRAMEBUFFER_OFFSET)
#define IMAGE_FRAMEBUFFER_UA_START			(DRAM_BASE_UA_START+IMAGE_FRAMEBUFFER_OFFSET)
#define IMAGE_FRAMEBUFFER_SIZE				(0x00C00000)

#ifdef SMDK6410_X5D
#define IMAGE_EBOOT_FRAMEBUFFER_OFFSET		(0x03000000)
#else
#define IMAGE_EBOOT_FRAMEBUFFER_OFFSET		(0x0E800000)
#endif
#define IMAGE_EBOOT_FRAMEBUFFER_PA_START	(DRAM_BASE_PA_START+IMAGE_EBOOT_FRAMEBUFFER_OFFSET)
#define IMAGE_EBOOT_FRAMEBUFFER_UA_START	(DRAM_BASE_UA_START+IMAGE_EBOOT_FRAMEBUFFER_OFFSET)
#define IMAGE_EBOOT_FRAMEBUFFER_SIZE		(0x01000000)

// MFC Video Process Buffer
#ifdef SMDK6410_X5D
#define IMAGE_MFC_BUFFER_OFFSET             (0x03800000)
#else
#define IMAGE_MFC_BUFFER_OFFSET				(0x0F400000)
#endif
#define IMAGE_MFC_BUFFER_PA_START			(DRAM_BASE_PA_START+IMAGE_MFC_BUFFER_OFFSET)
#define IMAGE_MFC_BUFFER_UA_START			(DRAM_BASE_UA_START+IMAGE_MFC_BUFFER_OFFSET)
#define IMAGE_MFC_BUFFER_SIZE				(0x00C00000)

// CMM memory
#ifdef SMDK6410_X5D
#define IMAGE_CMM_BUFFER_OFFSET             (0x02800000)
#else
#define IMAGE_CMM_BUFFER_OFFSET             (0x0E000000)
#endif
#define IMAGE_CMM_BUFFER_PA_START           (DRAM_BASE_PA_START+IMAGE_CMM_BUFFER_OFFSET)
#define IMAGE_CMM_BUFFER_UA_START           (DRAM_BASE_UA_START+IMAGE_CMM_BUFFER_OFFSET)
#define IMAGE_CMM_BUFFER_SIZE               (0x00800000)
//CAM memory
#ifdef SMDK6410_X5D
#define IMAGE_CAM_BUFFER_OFFSET             (0x02100000)
#else
#define IMAGE_CAM_BUFFER_OFFSET             (0x0D600000)
#endif
#define IMAGE_CAM_BUFFER_PA_START			(DRAM_BASE_PA_START+IMAGE_CAM_BUFFER_OFFSET)

//------------------------------------------------------------------------------

#define IMAGE_BOOT_CODE_CA_START
//------------------------------------------------------------------------------

#define VATOPA_OFFSET						(DRAM_BASE_CA_START - DRAM_BASE_PA_START) //0x30000000 //VA:0x8000000 - PA:0x50000000)
#define NAND_FLASH_START_UA					(UINT32)OALPAtoVA(S3C6410_BASE_REG_PA_NFCON, FALSE)
#define NAND_FLASH_START_CA					(UINT32)OALPAtoVA(S3C6410_BASE_REG_PA_NFCON, TRUE)
#define NAND_FLASH_SIZE                     (0x10000000)

#define IMAGE_WINCE_OFFSET                  (0x00200000)
#define IMAGE_WINCE_CODE_PA                 (DRAM_BASE_PA_START+IMAGE_WINCE_OFFSET)
#define IMAGE_WINCE_CODE_SIZE               (0x00300000)

#define IMAGE_NBL2_OFFSET                   (0x02300000)
#define IMAGE_NBL2_ADDR_PA                  (DRAM_BASE_PA_START+IMAGE_NBL2_OFFSET)
#define IMAGE_NBL2_ADDR_VA                  (DRAM_BASE_CA_START+IMAGE_NBL2_OFFSET)
#define IMAGE_NBL2_SIZE                     (0x0001d800)

#define IMAGE_IPL_OFFSET                    (0x00138000)
#define IMAGE_IPL_ADDR_PA                   (DRAM_BASE_PA_START+IMAGE_IPL_OFFSET)
#define IMAGE_IPL_ADDR_VA                   (DRAM_BASE_CA_START+IMAGE_IPL_OFFSET)
#define IMAGE_IPL_SIZE                      (0x00040000)

#define IMAGE_EBOOT_OFFSET                  (0x00038000)
#define IMAGE_EBOOT_ADDR_PA                 (DRAM_BASE_PA_START+IMAGE_EBOOT_OFFSET)
#define IMAGE_EBOOT_ADDR_VA                 (DRAM_BASE_CA_START+IMAGE_EBOOT_OFFSET)
#define IMAGE_EBOOT_SIZE                    (0x00080000)

#if __cplusplus
}
#endif

#endif
