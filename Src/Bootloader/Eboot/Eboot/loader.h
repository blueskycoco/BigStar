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

#ifndef _LOADER_H_
#define _LOADER_H_

#include <blcommon.h>
#include <bootpart.h>

#include "nand.h"

#define USE_TRACE32_DOWNLOAD (0)

// Bootloader version.
//
#define EBOOT_VERSION_MAJOR     2
#define EBOOT_VERSION_MINOR     5


//------------------------------------------------------------------------------
//
//  Section Name:   Memory Configuration
//  Description.:   The constants defining memory layout below must match
//                  those defined in the .bib files.
//
//------------------------------------------------------------------------------
#if 0
#define CACHED_TO_UNCACHED_OFFSET   0x20000000

#define STEPLDR_RAM_IMAGE_BASE      0x00000000
#define STEPLDR_RAM_IMAGE_SIZE      0x0000C000 //only use 0x00021000

#define SUPERIPL_RAM_IMAGE_BASE      0x00000000
#define SUPERIPL_RAM_IMAGE_SIZE      0x00052000 //stepldr + ipl

#define UPLDR_RAM_IMAGE_BASE        0x00000000
#define UPLDR_RAM_IMAGE_SIZE        0x00200000 //only use 0x00200000

#define NAND_ROM_IMAGE_BASE         0x00000000
#define NAND_ROM_IMAGE_SIZE         0x02000000

#define EBOOT_RAM_IMAGE_BASE        0x80038000	// changed for magneto...DonGo
#define EBOOT_RAM_IMAGE_SIZE        0x00080000

#define DIONB0_RAM_IMAGE_BASE       0x00000000
#define DIONB0_SECTOR_SIZE          0x00040000

#define FLASHBIN_RAM_IMAGE_BASE     0x00000000
#define FLASHBIN_SECTOR_SIZE        0x00040000

#define EBOOT_STORE_OFFSET          0
#define EBOOT_STORE_ADDRESS         (EBOOT_RAM_IMAGE_BASE + EBOOT_STORE_OFFSET)
#define EBOOT_STORE_MAX_LENGTH      EBOOT_RAM_IMAGE_SIZE

// BinFS work area defined in boot.bib
#define BINFS_RAM_START             (0x800C0000 | CACHED_TO_UNCACHED_OFFSET)   // uncached, changed for magneto...DonGo

#define BINFS_RAM_LENGTH            0x42000

//
// Nk Memory reigions defined in config.bib...
//
#define ROM_RAMIMAGE_START          0x80000000
#define ROM_RAMIMAGE_SIZE           0x05000000


//#define FILE_CACHE_START            (0x8c200000 | CACHED_TO_UNCACHED_OFFSET)        // Start of file cache (temporary store
// Changed for magneto... DonGo
#define FILE_CACHE_START            (0x80200000 | CACHED_TO_UNCACHED_OFFSET)        // Start of file cache (temporary store
#define FILE_CACHE_LENGTH            (0x03A00000)        // Length of Flash.bin
                                                // for flash images).
#else
#define CACHED_TO_UNCACHED_OFFSET   0x20000000

#define STEPLDR_RAM_IMAGE_BASE      0x00000000
#define STEPLDR_RAM_IMAGE_SIZE      0x00002800

#define BLOCK0IMG_RAM_IMAGE_BASE    0x00000000
#define BLOCK0IMG_RAM_IMAGE_SIZE    (STEPLDR_RAM_IMAGE_SIZE+IMAGE_NBL2_SIZE) //stepldr + nbl2

#define IPL_RAM_IMAGE_BASE          0x00000000
#define IPL_RAM_IMAGE_SIZE          IMAGE_IPL_SIZE

#define SUPERIPL_RAM_IMAGE_BASE     0x00000000
#define SUPERIPL_RAM_IMAGE_SIZE     (BLOCK0IMG_RAM_IMAGE_SIZE+IPL_RAM_IMAGE_SIZE) //stepldr + nbl2 + ipl

#define TOC_SIZE                    0x00000200

#define UPLDR_RAM_IMAGE_BASE        0x00000000
#define UPLDR_RAM_IMAGE_SIZE        0x00400000 //uldr only use 0x00400000

#define NAND_ROM_IMAGE_BASE         0x00000000
#define NAND_ROM_IMAGE_SIZE         0x04000000

#define EBOOT_RAM_IMAGE_BASE        IMAGE_EBOOT_ADDR_VA
#define EBOOT_RAM_IMAGE_SIZE        IMAGE_EBOOT_SIZE

#define DIONB0_RAM_IMAGE_BASE       0x00000000
#define DIONB0_IMAGE_SIZE           0x03000000

#define FLASHBIN_RAM_IMAGE_BASE     0x00000000
#define FLASHBIN_IMAGE_SIZE         0x05000000

// BinFS work area defined in boot.bib
#define BINFS_RAM_START             (0x800C0000 | CACHED_TO_UNCACHED_OFFSET)
#define BINFS_RAM_LENGTH            0x42000

//
// Nk Memory reigions defined in config.bib...
//
#define ROM_RAMIMAGE_START          0x80000000
#define ROM_RAMIMAGE_SIZE           0x05000000

#define FILE_CACHE_START            (0x80200000 | CACHED_TO_UNCACHED_OFFSET)        // Start of file cache (temporary store for flash images).
#define FILE_CACHE_LENGTH           (0x05000000)
#endif

// Driver globals pointer (parameter sharing memory used by bootloader and OS).
//
#define pBSPArgs                    ((BSP_ARGS *) IMAGE_SHARE_ARGS_UA_START)


// Platform "BOOTME" root name.
//
#define PLATFORM_STRING         "SMDK6410"
extern NANDDeviceInfo stDeviceInfo;

#undef SECTOR_SIZE
//#ifndef SECTOR_SIZE
#define LB_SECTOR_SIZE	                2048
#define SB_SECTOR_SIZE	                512
#define SECTOR_SIZE						((IS_LB)?LB_SECTOR_SIZE:SB_SECTOR_SIZE )
//#endif

#ifndef BADBLOCKMARK
#define BADBLOCKMARK                0x00
#endif

#define SECTOR_TO_BLOCK(sector)			((sector) >> ((IS_LB)?6:8) )
#define BLOCK_TO_SECTOR(block)			((block)  << ((IS_LB)?6:8) )

// fs: sector number
// returns block aligned value
__inline DWORD SECTOR_TO_BLOCK_SIZE(DWORD sn) {
    return ( (sn / PAGES_PER_BLOCK) + ( (sn % PAGES_PER_BLOCK) ? 1 : 0) );
}

// fs: file size in bytes
// returns sector aligned value
__inline DWORD FILE_TO_SECTOR_SIZE(DWORD fs) {
    return ( (fs / SECTOR_SIZE) + ( (fs % SECTOR_SIZE) ? 1 : 0) );
}

// ns: number of sectors
// N.B: returns sector aligned value since we can't tell
__inline DWORD SECTOR_TO_FILE_SIZE(DWORD ns) {
    return ( ns * SECTOR_SIZE );
}


// fs: sector number
// returns block aligned value
__inline DWORD FILE_TO_BLOCK_SIZE(DWORD fb) {
    return ( (fb / (SECTOR_SIZE*PAGES_PER_BLOCK)) + ( (fb % (SECTOR_SIZE*PAGES_PER_BLOCK)) ? 1 : 0) );
}

//
// Boot Config Flags...
//

// BOOT_MODE_ flags indicate where we are booting from
#define BOOT_MODE_NAND          0x00000001
#define BOOT_MODE_NOR_AMD       0x00000002
#define BOOT_MODE_NOR_STRATA    0x00000004
#define BOOT_MODE_TEST          0x00000008
#define BOOT_MODE_MASK(dw)     (0x0000000F & (dw))

// BOOT_TYPE_ flags indicate whether we are booting directly off media, or downloading an image
#define BOOT_TYPE_DIRECT        0x00000010  // boot off media?
#define BOOT_TYPE_MULTISTAGE    0x00000020  // multi-stage bootloaders?
#define BOOT_TYPE_MASK(dw)     (0x000000F0 & (dw))

// TARGET_TYPE_ flags indicate where the image is to be written to.
#define TARGET_TYPE_RAMIMAGE    0x00000100  // Directly to SDRAM
#define TARGET_TYPE_CACHE       0x00000200  // FLASH_CACHE
#define TARGET_TYPE_NOR         0x00000400  // NOR Flash
#define TARGET_TYPE_NAND        0x00000800  // NAND Flash
//...
#define TARGET_TYPE_MASK(dw)   (0x00000F00 & (dw))

#define CONFIG_FLAGS_DHCP       0x00001000
#define CONFIG_FLAGS_DEBUGGER   0x00002000
#define CONFIG_FLAGS_MASK(dw)  (0x0000F000 & (dw))
#define CONFIG_FLAGS_KITL		0x00008000

#define CONFIG_BOOTDELAY_DEFAULT       5


//
// Bootloader configuration parameters.
//
typedef struct _BOOTCFG {

    ULONG       ImageIndex;

    ULONG       ConfigFlags;
    ULONG       BootDelay;

    EDBG_ADDR   EdbgAddr;
    ULONG       SubnetMask;
    ULONG	Device_ID[4];
} BOOT_CFG, *PBOOT_CFG;


//
// On disk structures for NAND bootloaders...
//
#if 0
//
// OEM Reserved (Nand) Blocks for TOC and various bootloaders
//

// NAND Boot (loads into SteppingStone) @ Block 0
// SuperIPL @ Block 0
#define STEPLDR_BLOCK                 0
#define STEPLDR_BLOCK_SIZE            2
#define STEPLDR_SECTOR                BLOCK_TO_SECTOR(STEPLDR_BLOCK)

// TOC @ Block 1
#define TOC_BLOCK                   2
#define TOC_BLOCK_SIZE              1
#define TOC_SECTOR                  BLOCK_TO_SECTOR(TOC_BLOCK)

// Dio.nb0 @ Block 2
#define DIONB0_BLOCK                 3
#define DIONB0_BLOCK_SIZE            SECTOR_TO_BLOCK(DIONB0_SECTOR_SIZE)
#define DIONB0_SECTOR                BLOCK_TO_SECTOR(DIONB0_BLOCK)

// flash.bin @ Block 2
#define FLASHBIN_BLOCK              3
#define FLASHBIN_BLOCK_SIZE         SECTOR_TO_BLOCK(FLASHBIN_SECTOR_SIZE)
#define FLASHBIN_SECTOR             BLOCK_TO_SECTOR(FLASHBIN_BLOCK)

// Eboot @ Block 5
#define EBOOT_BLOCK                 5
#define EBOOT_SECTOR_SIZE           FILE_TO_SECTOR_SIZE(EBOOT_RAM_IMAGE_SIZE)
#define EBOOT_BLOCK_SIZE            SECTOR_TO_BLOCK(EBOOT_SECTOR_SIZE)
#define EBOOT_SECTOR                BLOCK_TO_SECTOR(EBOOT_BLOCK)

// UPLDR @ Next Block
#define UPLDR_BLOCK                 (STEPLDR_BLOCK_SIZE + TOC_BLOCK_SIZE + EBOOT_BLOCK_SIZE)
#define UPLDR_SECTOR_SIZE           FILE_TO_SECTOR_SIZE(UPLDR_RAM_IMAGE_SIZE)
#define UPLDR_BLOCK_SIZE            SECTOR_TO_BLOCK(UPLDR_SECTOR_SIZE)
#define UPLDR_SECTOR                BLOCK_TO_SECTOR(UPLDR_BLOCK)

#define RESERVED_BOOT_BLOCKS        FLASHBIN_BLOCK

// Images start after OEM Reserved Blocks
#define IMAGE_START_BLOCK           RESERVED_BOOT_BLOCKS
#define IMAGE_START_SECTOR          BLOCK_TO_SECTOR(IMAGE_START_BLOCK)
#else
//
// OEM Reserved (Nand) Blocks for TOC and various bootloaders
//

//--------------------  Bootloader Images  --------------------
// NAND Boot (loads into SteppingStone) @ Block 0
// Stepldr @ Block 0
#define STEPLDR_BLOCK               (0)
#define STEPLDR_BLOCK_SIZE          FILE_TO_BLOCK_SIZE(STEPLDR_RAM_IMAGE_SIZE)

// Block0Img @ Block 0
#define BLOCK0IMG_BLOCK             (0)
#define BLOCK0IMG_BLOCK_SIZE        FILE_TO_BLOCK_SIZE(BLOCK0IMG_RAM_IMAGE_SIZE)

// IPL @ Block 1
#define IPL_BLOCK                   (BLOCK0IMG_BLOCK+BLOCK0IMG_BLOCK_SIZE)
#define IPL_BLOCK_SIZE              FILE_TO_BLOCK_SIZE(IPL_RAM_IMAGE_SIZE)
#define IPL_BLOCK_RESERVED          (1)

// SuperIPL @ Block 0
#define SUPERIPL_BLOCK              (0)
#define SUPERIPL_BLOCK_SIZE         (BLOCK0IMG_BLOCK_SIZE+IPL_BLOCK_SIZE)
#define SUPERIPL_BLOCK_RESERVED     (1)

// TOC @ Block 3
#define TOC_BLOCK                   (SUPERIPL_BLOCK+SUPERIPL_BLOCK_SIZE+SUPERIPL_BLOCK_RESERVED)
#define TOC_BLOCK_SIZE              FILE_TO_BLOCK_SIZE(TOC_SIZE)
#define TOC_BLOCK_RESERVED          (1)
#define TOC_SECTOR                  BLOCK_TO_SECTOR(TOC_BLOCK)

// Eboot @ Block 5
#define EBOOT_BLOCK                 (TOC_BLOCK+TOC_BLOCK_SIZE+TOC_BLOCK_RESERVED)
#define EBOOT_BLOCK_SIZE            FILE_TO_BLOCK_SIZE(EBOOT_RAM_IMAGE_SIZE)
#define EBOOT_BLOCK_RESERVED        (1)
// Eboot @ Block 7
#define LOGO_BLOCK                 (EBOOT_BLOCK+EBOOT_BLOCK_SIZE+EBOOT_BLOCK_RESERVED)
#define LOGO_BLOCK_SIZE            FILE_TO_BLOCK_SIZE(0xaf000)
#define LOGO_BLOCK_RESERVED        (1)

#define RESERVED_BOOT_BLOCKS        (LOGO_BLOCK+LOGO_BLOCK_SIZE+LOGO_BLOCK_RESERVED)

#define RESERVED_BLOCK              16//(RESERVED_BOOT_BLOCKS)//11//

//--------------------  OS Images  --------------------
// flash.bin @ Block 10
#define FLASHBIN_BLOCK              (RESERVED_BLOCK)
#define FLASHBIN_BLOCK_SIZE         FILE_TO_BLOCK_SIZE(FLASHBIN_IMAGE_SIZE)

// Dio.nb0 @ Block 10
#define DIONB0_BLOCK                (RESERVED_BLOCK)
#define DIONB0_BLOCK_SIZE           FILE_TO_BLOCK_SIZE(DIONB0_IMAGE_SIZE)

// UPLDR @ Next Block
#define UPLDR_BLOCK                 (RESERVED_BLOCK)
#define UPLDR_BLOCK_SIZE            FILE_TO_BLOCK_SIZE(UPLDR_RAM_IMAGE_SIZE)

// Images start after OEM Reserved Blocks
#define IMAGE_START_BLOCK           (FLASHBIN_BLOCK)
#define IMAGE_START_SECTOR          BLOCK_TO_SECTOR(IMAGE_START_BLOCK)
#define IMAGE_BLOCK_SIZE            (FLASHBIN_BLOCK_SIZE)

#endif
//
// OEM Defined TOC...
//
#define MAX_SG_SECTORS              14
#define IMAGE_STRING_LEN            16  // chars
#define MAX_TOC_DESCRIPTORS         3   // per sector
#define TOC_SIGNATURE               0x434F544E  // (NAND TOC)

// Default image descriptor to load.
// We store Eboot as image 0, nk.nb0 as image 1
#define DEFAULT_IMAGE_DESCRIPTOR    1

// IMAGE_TYPE_ flags indicate whether the image is a Bootloader,
// RAM image, supports BinFS, Multiple XIP, ...
//
#define IMAGE_TYPE_LOADER           0x00000001  // eboot.bin
#define IMAGE_TYPE_RAMIMAGE         0x00000002  // nk.bin
#define IMAGE_TYPE_BINFS            0x00000004
#define IMAGE_TYPE_MXIP             0x00000008
#define IMAGE_TYPE_RAWBIN			0x00000040	// raw bin .nb0
#define IMAGE_TYPE_STEPLDR			0x00000080	// stepldr.bin
#define IMAGE_TYPE_UPLDR			0x00000100	// upld.nb0
#define IMAGE_TYPE_DIO  			0x00000200	// dio image
#define IMAGE_TYPE_DIONB0  			0x00000400	// dio.nb0 image
#define IMAGE_TYPE_FLASHBIN			0x00000800	// flash.bin image
#define IMAGE_TYPE_MASK(dw)        (0x00000FFF & (dw))



// SG_SECTOR: supports chained (scatter gather) sectors.
// This structure equates to a sector-based MXIP RegionInfo in blcommon.
//
typedef struct _SG_SECTOR {

    DWORD dwSector;     // Starting sector of the image segment
    DWORD dwLength;     // Image length of this segment, in contigious sectors.

} SG_SECTOR, *PSG_SECTOR;


// IMAGE_DESCRIPTOR: describes the image to load.
// The image can be anything: bootloaders, RAMIMAGE, MXIP, ...
// Note: Our NAND uses H/W ECC, so no checksum needed.
//
typedef struct _IMAGE_DESCRIPTOR {

    // File version info
    DWORD dwVersion;                    // e.g: build number
    DWORD dwSignature;                  // e.g: "EBOT", "CFSH", etc
    UCHAR ucString[IMAGE_STRING_LEN];   // e.g: "PocketPC_2002"

    DWORD dwImageType;      // IMAGE_TYPE_ flags
    DWORD dwTtlSectors;     // TTL image size in sectors.
                            // We store size in sectors instead of bytes
                            // to simplify sector reads in Nboot.

    DWORD dwLoadAddress;    // Virtual address to load image (ImageStart)
    DWORD dwJumpAddress;    // Virtual address to jump (StartAddress/LaunchAddr)

    // This array equates to a sector-based MXIP MultiBINInfo in blcommon.
    // Unused entries are zeroed.
    // You could chain image descriptors if needed.
    SG_SECTOR sgList[MAX_SG_SECTORS];

    // BinFS support to load nk region only
	//struct
	//{
		ULONG dwStoreOffset;    // byte offset - not needed - remove!
		//ULONG RunAddress;     // nk dwRegionStart address
		//ULONG Length;         // nk dwRegionLength in bytes
		//ULONG LaunchAddress;  // nk dwLaunchAddr
	//} NKRegion;

} IMAGE_DESCRIPTOR, *PIMAGE_DESCRIPTOR;

//
// IMAGE_DESCRIPTOR signatures we know about
//
#define IMAGE_EBOOT_SIG             0x45424F54          // "EBOT", eboot.nb0
#define IMAGE_RAM_SIG               0x43465348          // "CFSH", nk.nb0
#define IMAGE_BINFS_SIG             (IMAGE_RAM_SIG + 1)

__inline BOOL VALID_IMAGE_DESCRIPTOR(PIMAGE_DESCRIPTOR pid) {
    return ( (pid) &&
      ((IMAGE_EBOOT_SIG == (pid)->dwSignature) ||
       (IMAGE_RAM_SIG == (pid)->dwSignature) ||
       (IMAGE_BINFS_SIG == (pid)->dwSignature)));
}

//  This is for MXIP chain.bin, which needs to be loaded into the SDRAM
//  during the start up.
typedef struct _CHAININFO {

    DWORD   dwLoadAddress;          // Load address in SDRAM
    DWORD   dwFlashAddress;         // Start location on the NAND
    DWORD   dwLength;               // The length of the image
} CHAININFO, *PCHAININFO;

//
// TOC: Table Of Contents, OEM on disk structure.
// sizeof(TOC) = SECTOR_SIZE.
// Consider the TOC_BLOCK to contain an array of PAGES_PER_BLOCK
// TOC entries, since the entire block is reserved.
//
typedef struct _TOC {
    DWORD               dwSignature;
    // How to boot the images in this TOC.
    // This could be moved into the image descriptor if desired,
    // but I prefer to conserve space.
    BOOT_CFG            BootCfg;

    // Array of Image Descriptors.
    IMAGE_DESCRIPTOR    id[MAX_TOC_DESCRIPTORS];

    CHAININFO           chainInfo;
} TOC, *PTOC;           // 512 bytes


__inline BOOL VALID_TOC(PTOC ptoc) {
    return ((ptoc) &&  (TOC_SIGNATURE == (ptoc)->dwSignature));
}


// Loader function prototypes.
//
BOOL InitEthDevice(PBOOT_CFG pBootCfg);
int OEMReadDebugByte(void);
BOOL OEMVerifyMemory(DWORD dwStartAddr, DWORD dwLength);
void Launch(DWORD dwLaunchAddr);
PVOID GetKernelExtPointer(DWORD dwRegionStart, DWORD dwRegionLength);
void OEMWriteDebugLED(WORD wIndex, DWORD dwPattern);

BOOL WriteOSImageToBootMedia(DWORD dwImageStart, DWORD dwImageLength, DWORD dwLaunchAddr);
BOOL ReadOSImageFromBootMedia(void);
BOOL WriteRawImageToBootMedia(DWORD dwImageStart, DWORD dwImageLength, DWORD dwLaunchAddr);

BOOL    TOC_Init(DWORD dwEntry, DWORD dwImageType, DWORD dwImageStart, DWORD dwImageLength, DWORD dwLaunchAddr);
BOOL    TOC_Read(void);
BOOL    TOC_Write(void);
void    TOC_Print(void);

#ifdef IROMBOOT
void ReserveLoaderArea();
#endif

#endif  // _LOADER_H_.
