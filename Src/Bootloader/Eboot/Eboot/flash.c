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

#include <windows.h>
#include <bsp.h>
#include "loader.h"

#include <fmd.h>
#include <s3c6410.h>
#include "cfnand.h"

extern DWORD g_ImageType;
extern const PTOC   g_pTOC;
extern DWORD        g_dwTocEntry;
extern IPLmain(void);

static BOOL WriteFlashNK(UINT32 address, UINT32 size);
static BOOL WriteSector(SECTOR_ADDR sector, VOID *pBuffer, SectorInfo *pInfo);
static BOOL EraseBlock(BLOCK_ID block);

/*
    @func   BOOL | OEMIsFlashAddr | Tests whether the address provided resides in the Samsung's flash.
    @rdesc  TRUE = Specified address resides in flash, FALSE = Specified address doesn't reside in flash.
    @comm    
    @xref   
*/
BOOL OEMIsFlashAddr(DWORD dwAddr)
{
    //EdbgOutputDebugString("OEMIsFlashAddr: 0x%x, %d\r\n", dwPhysStart, bRc);
    return(TRUE);
}


/*
    @func   LPBYTE | OEMMapMemAddr | Remaps a specified address to a file cache location.  The file cache is used as a temporary store for flash images before they're written to flash.
    @rdesc  Corresponding address within a file cache area.
    @comm    
    @xref   
*/
LPBYTE OEMMapMemAddr(DWORD dwImageStart, DWORD dwAddr)
{
	if (g_ImageType & IMAGE_TYPE_DIO)
	{
		dwAddr = (FILE_CACHE_START + (dwAddr - NAND_ROM_IMAGE_BASE));
	    return (LPBYTE)dwAddr;
	}
    else
	if (g_ImageType & IMAGE_TYPE_DIONB0)
	{
		dwAddr = (FILE_CACHE_START + (dwAddr - NAND_ROM_IMAGE_BASE));
	    return (LPBYTE)dwAddr;
	}
    else
	if (g_ImageType & IMAGE_TYPE_STEPLDR)
	{
		dwAddr = (FILE_CACHE_START + (dwAddr - STEPLDR_RAM_IMAGE_BASE));
	    return (LPBYTE)dwAddr;
	}
    else
	if (g_ImageType & IMAGE_TYPE_FLASHBIN)
	{
		dwAddr = (FILE_CACHE_START + (dwAddr - FLASHBIN_RAM_IMAGE_BASE));
	    return (LPBYTE)dwAddr;
	}
    else
	if (g_ImageType & IMAGE_TYPE_UPLDR)
	{
		dwAddr = (FILE_CACHE_START + (dwAddr - UPLDR_RAM_IMAGE_BASE));
	    return (LPBYTE)dwAddr;
	}
    else
    if (g_ImageType & IMAGE_TYPE_LOADER)
    {
		dwAddr = (FILE_CACHE_START + (dwAddr - EBOOT_RAM_IMAGE_BASE));
	    return (LPBYTE)dwAddr;
	}
	else
    if (g_ImageType & IMAGE_TYPE_RAWBIN)
    {
        OALMSG(TRUE, (TEXT("OEMMapMemAddr 0x%x  0x%x\r\n"),dwAddr,(FILE_CACHE_START + dwAddr)));
		dwAddr = FILE_CACHE_START + dwAddr;
	    return (LPBYTE)dwAddr;
	}

    return (LPBYTE)dwAddr;
}


//------------------------------------------------------------------------------
//
//  Function:  BLFlashDownload
//
//  This function download image from flash memory to RAM.
//
UINT32 BLFlashDownload()
{
	UINT32 rc = BL_ERROR;

	IPLmain(); rc=BL_JUMP;
//	rc = ReadFlashNK();
//	rc = ReadFlashIPL();

	return rc;
}

//------------------------------------------------------------------------------

static UINT32 ReadFlashIPL()
{
    UINT32 rc = BL_ERROR;
    HANDLE hFMD = NULL;
    FlashInfo flashInfo;
    UINT32 offset;
    SectorInfo sectorInfo;
    SECTOR_ADDR sector;
    BLOCK_ID block;
    UINT8 *pImage;
    UINT32 *pInfo;


    // Check if there is a valid image
    OALMSG(OAL_INFO, (L"\r\nLoad IPL Image from flash memory\r\n"));

    // Open FMD to access NAND
    hFMD = FMD_Init(NULL, NULL, NULL);
    if (hFMD == NULL) {
        OALMSG(OAL_ERROR, (L"ERROR: BLFlashDownload: "
            L"FMD_Init call failed\r\n"
        ));
        goto cleanUp;
    }

    // Get flash info
    if (!FMD_GetInfo(&flashInfo)) {
        OALMSG(OAL_ERROR, (L"ERROR: BLFlashDownload: "
            L"FMD_GetInfo call failed\r\n"
        ));
        goto cleanUp;
    }

    // Start from NAND start
    block  = 0;
    sector = 8;
    offset = 0;

    // Set address where to place image
    pImage = (UINT8*)(IMAGE_IPL_ADDR_VA);

    // Read image to memory
    while (offset < IMAGE_IPL_SIZE && block < flashInfo.dwNumBlocks) {

        // Read sectors in block
        while ( offset < IMAGE_IPL_SIZE ) {
            // When block read fail, there isn't what we can do more
            if (!FMD_ReadSector(sector, pImage, &sectorInfo, 1)) {
                OALMSG(OAL_ERROR, (L"\r\nERROR: BLFlashDownload: "
                    L"Failed read sector %d from flash\r\n", sector
                ));
                goto cleanUp;
            }

            // Move to next sector
            sector++;
            pImage += flashInfo.wDataBytesPerSector;
            offset += flashInfo.wDataBytesPerSector;
//            OALMSG(OAL_INFO, (L"."));
        }

        // Move to next block
        block++;
    }

    OALMSG(OAL_INFO, (L"\r\n"));

    // Check if IPL is image and dump its content
    pInfo = (UINT32*)(IMAGE_IPL_ADDR_VA + ROM_SIGNATURE_OFFSET);
    if (*pInfo != ROM_SIGNATURE) {
        OALMSG(OAL_ERROR, (L"ERROR: BLFlashDownload: "
            L"IPL image doesn't have ROM signature at 0x%08x\r\n", pInfo
        ));
        goto cleanUp;
    }

    g_pTOC->id[g_dwTocEntry].dwJumpAddress = IMAGE_IPL_ADDR_VA;
    rc = BL_JUMP;

cleanUp:
    return rc;
}

//------------------------------------------------------------------------------

static UINT32 ReadFlashNK()
{
    UINT32 rc = BL_ERROR;
    HANDLE hFMD = NULL;
    FlashInfo flashInfo;
    ROMHDR *pTOC;
    UINT32 offset, size, toc;
    UINT32 blockSize, sectorSize, sectorsPerBlock;
    SectorInfo sectorInfo;
    SECTOR_ADDR sector;
    BLOCK_ID block;
    UINT8 *pImage;

    // Check if there is a valid image
    OALMSG(OAL_INFO, (L"\r\nLoad NK image from flash memory\r\n"));

    // Open FMD to access NAND
    hFMD = FMD_Init(NULL, NULL, NULL);
    if (hFMD == NULL) {
        OALMSG(OAL_ERROR, (L"ERROR: BLFlashDownload: "
            L"FMD_Init call failed\r\n"
        ));
        goto cleanUp;
    }

    // Get flash info
    if (!FMD_GetInfo(&flashInfo)) {
        OALMSG(OAL_ERROR, (L"ERROR: BLFlashDownload: "
            L"FMD_GetInfo call failed\r\n"
        ));
        goto cleanUp;
    }

    // Make block & sector size ready
    blockSize = flashInfo.dwBytesPerBlock;
    sectorSize = flashInfo.wDataBytesPerSector;
    sectorsPerBlock = flashInfo.wSectorsPerBlock;

    // Skip reserved blocks
    block = 0;
    while (block < flashInfo.dwNumBlocks) {
        if ((FMD_GetBlockStatus(block) & BLOCK_STATUS_BAD) != 0) {
            OALMSG(OAL_WARN, (L"WARN: EBOOT!FMD_GetBlockStatus: "
                L"Skip bad block %d\r\n", block
            ));
            block++;
            continue;
        }
        if ((FMD_GetBlockStatus(block) & BLOCK_STATUS_RESERVED) == 0) break;
        block++;
    }

    // Set address where to place image
    pImage = (UINT8*)IMAGE_WINCE_CODE_PA;
    size = IMAGE_WINCE_CODE_SIZE;

    // Read image to memory
    offset = 0;
    toc = 0;
    pTOC = NULL;
    while (offset < size && block < flashInfo.dwNumBlocks) {

        // Skip bad blocks
        if ((FMD_GetBlockStatus(block) & BLOCK_STATUS_BAD) != 0) {
            block++;
            sector += flashInfo.wSectorsPerBlock;
            continue;
        }

        // Read sectors in block
        sector = 0;
        while (sector < sectorsPerBlock && offset < size) {
            // When block read fail, there isn't what we can do more
		    
			RETAILMSG(1, (TEXT("0x%x 0x%x \r\n"), block * sectorsPerBlock + sector, pImage + offset));

            if (!FMD_ReadSector(
                block * sectorsPerBlock + sector, pImage + offset, 
                &sectorInfo, 1
            )) {
                OALMSG(OAL_ERROR, (L"\r\nERROR: BLFlashDownload: "
                    L"Failed read sector %d from flash\r\n", sector
                ));
                goto cleanUp;
            }
            // Move to next sector
            sector++;
            offset += sectorSize;
        }

        // Move to next block
        block++;
    }

	g_pTOC->id[g_dwTocEntry].dwJumpAddress = IMAGE_WINCE_CODE_PA;
    rc = BL_JUMP;

cleanUp:
    return rc;
}


/*
    @func   BOOL | OEMStartEraseFlash | Called at the start of image download, this routine begins the flash erase process.
    @rdesc  TRUE = Success, FALSE = Failure.
    @comm    
    @xref   
*/
BOOL OEMStartEraseFlash(DWORD dwStartAddr, DWORD dwLength)
{
    // Nothing to do (erase done in OEMWriteFlash)...
    //
//    OALMSG(OAL_INFO, (L"OEMStartEraseFlash: return TRUE\r\n"));
    return(TRUE);
}


/*
    @func   void | OEMContinueEraseFlash | Called frequenty during image download, this routine continues the flash erase process.
    @rdesc  N/A.
    @comm    
    @xref   
*/
void OEMContinueEraseFlash(void)
{
    // Nothing to do (erase done in OEMWriteFlash)...
    //
//    EdbgOutputDebugString("OEMContinueEraseFlash\r\n");
}


/*
    @func   BOOL | OEMFinishEraseFlash | Called following the image download, this routine completes the flash erase process.
    @rdesc  TRUE = Success, FALSE = Failure.
    @comm    
    @xref   
*/
BOOL OEMFinishEraseFlash(void)
{
    // Nothing to do (erase done in OEMWriteFlash)...
    //
//    EdbgOutputDebugString("OEMFinishEraseFlash\r\n");
    return(TRUE);
}


/*
    @func   BOOL | OEMWriteFlash | Writes data to flash (the source location is determined using OEMMapMemAddr).
    @rdesc  TRUE = Success, FALSE = Failure.
    @comm    
    @xref   
*/
BOOL OEMWriteFlash(DWORD dwStartAddr, DWORD dwLength)
{
	BOOL rc;

    EdbgOutputDebugString("OEMWriteFlash 0x%x 0x%x\r\n", dwStartAddr, dwLength);
    EdbgOutputDebugString("OEMWriteFlash 0x%x \r\n", g_ImageType);

	switch (g_ImageType) {
	case IMAGE_TYPE_DIONB0:
		rc = TRUE;
		break;
	case IMAGE_TYPE_FLASHBIN:
//		rc = WriteFlashNK(dwStartAddr, dwLength);
		rc = TRUE;
		break;
	case IMAGE_TYPE_STEPLDR:
	    rc = TRUE;
		break;	    
	case IMAGE_TYPE_LOADER:
	    rc = TRUE;
		break;	    

	default:
		rc = FALSE;
	}
	return rc;

//    return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WriteFlashNK(UINT32 address, UINT32 size)
{
    BOOL rc = FALSE;
    HANDLE hFMD;
    FlashInfo flashInfo;
    SectorInfo sectorInfo;
    BOOL ok = FALSE;
    BLOCK_ID block;
    //ROMHDR *pTOC;
    UINT32 *pInfo, count, sector;
    UINT32 blockSize, sectorSize, sectorsPerBlock;
    UINT8 *pData;


    OALMSG(OAL_INFO, (L"OEMWriteFlash: Writing NK image to flash\r\n"));

    // We need to know sector/block size
    if ((hFMD = FMD_Init(NULL, NULL, NULL)) == NULL) {
        OALMSG(OAL_ERROR, (L"ERROR: OEMWriteFlash: "
            L"FMD_Init call failed\r\n"
        ));
        goto cleanUp;
    }

    // Get flash info
    if (!FMD_GetInfo(&flashInfo)) {
        OALMSG(OAL_ERROR, (L"ERROR: EBOOT!OEMWriteFlash: "
            L"FMD_GetInfo call failed!\r\n"
        ));
        FMD_Deinit(hFMD);
        goto cleanUp;
    }

    // We don't need access to FMD library
    FMD_Deinit(hFMD);

    OALMSG(OAL_INFO, (L"OEMWriteFlash: "
        L"Flash has %d blocks, %d bytes/block, %d sectors/block\r\n",
        flashInfo.dwNumBlocks, flashInfo.dwBytesPerBlock,
        flashInfo.wSectorsPerBlock
    ));

    // Make block & sector size ready
    blockSize = flashInfo.dwBytesPerBlock;
    sectorSize = flashInfo.wDataBytesPerSector;
    sectorsPerBlock = flashInfo.wSectorsPerBlock;

    // Get data location
    pData = OEMMapMemAddr(address, address);

    // Verify that we get CE image.
    pInfo = (UINT32*)(pData + ROM_SIGNATURE_OFFSET);
    if (*pInfo++ != ROM_SIGNATURE) {
        OALMSG(OAL_ERROR, (L"ERROR: OEMWriteFlash: "
            L"Image Signature not found\r\n"
        ));
        goto cleanUp;
    }
    pInfo++;

    // Skip reserved blocks
    block = 0;
    while (block < flashInfo.dwNumBlocks) {
        if ((FMD_GetBlockStatus(block) & BLOCK_STATUS_BAD) != 0) {
            OALMSG(OAL_WARN, (L"WARN: EBOOT!OEMWriteFlash: "
                L"Skip bad block %d\r\n", block
            ));
            block++;
            continue;
        }
        if ((FMD_GetBlockStatus(block) & BLOCK_STATUS_RESERVED) == 0) break;
        block++;
    }

    OALMSG(OAL_INFO, (L"OEMWriteFlash: "
        L"NK image starts at block %d\r\n",  block
    ));

    // Write image
    count = 0;
    while (count < size && block < flashInfo.dwNumBlocks) {

        // If block is bad, we have to offset it
        if ((FMD_GetBlockStatus(block) & BLOCK_STATUS_BAD) != 0) {
            block++;
            OALMSG(OAL_WARN, (L"WARN: EBOOT!OEMWriteFlash: "
                L"Skip bad block %d\r\n", block
            ));
            continue;
        }

        // Erase block
        if (!EraseBlock(block)) {
            FMD_SetBlockStatus(block, BLOCK_STATUS_BAD);
            block++;
            OALMSG(OAL_WARN, (L"WARN: EBOOT!OEMWriteFlash: "
                L"Block %d erase failed, mark block as bad\r\n", block
            ));
            continue;
        }

        // Now write sectors
        sector = 0;
        while (sector < sectorsPerBlock && count < size) {

            // Prepare sector info
            memset(&sectorInfo, 0xFF, sizeof(sectorInfo));
            sectorInfo.dwReserved1 = 0;
            sectorInfo.wReserved2 = 0;

            // Write sector
            if (!(ok = WriteSector(
                block * sectorsPerBlock + sector, pData + count, &sectorInfo
            ))) break;

            // Move to next sector
            count += sectorSize;
            sector++;
        }

        // When sector write failed, mark block as bad and move back
        if (!ok) {
            OALMSG(OAL_WARN, (L"WARN: EBOOT!OEMWriteFlash: "
                L"Block %d sector %d write failed, mark block as bad\r\n", 
                block, sector
            ));
            // First move back
            count -= sector * flashInfo.wDataBytesPerSector;
            // Mark block as bad
            FMD_SetBlockStatus(block, BLOCK_STATUS_BAD);
        }

        // We are done with block
        block++;
    }

    // Erase rest of media
    while (block < flashInfo.dwNumBlocks) {

        // If block is bad, we have to offset it
        if ((FMD_GetBlockStatus(block) & BLOCK_STATUS_BAD) != 0) {
            block++;
            OALMSG(OAL_WARN, (L"WARN: EBOOT!OEMWriteFlash: "
                L"Skip bad block %d\r\n", block
            ));
            continue;
        }

        // When erase failed, mark block as bad and skip it
        if (!EraseBlock(block)) {
            FMD_SetBlockStatus(block, BLOCK_STATUS_BAD);
            block++;
            OALMSG(OAL_WARN, (L"WARN: EBOOT!OEMWriteFlash: "
                L"Block %d erase failed, mark block as bad\r\n", block
            ));
            continue;
        }

        // Move to next block
        block++;
    }

    // Close FMD driver
    FMD_Deinit(hFMD);
    hFMD = NULL;

    OALMSG(OAL_INFO, (L"OEMWriteFlash: NK written\r\n"));

    // Done
    rc = TRUE;

cleanUp:
    if (hFMD != NULL) FMD_Deinit(hFMD);
    return rc;
}
    
//------------------------------------------------------------------------------

BOOL WriteSector(SECTOR_ADDR sector, VOID *pBuffer, SectorInfo *pInfo)
{
    BOOL rc;
    UINT32 retry = 4;

    do {
        rc = FMD_WriteSector(sector, pBuffer, pInfo, 1);
    } while (!rc && --retry > 0);
    return rc;
}

//------------------------------------------------------------------------------

BOOL EraseBlock(BLOCK_ID block)
{
    BOOL rc;
    UINT32 retry = 4;

    // Erase block
    do {
        rc = FMD_EraseBlock(block);
    } while (!rc && --retry > 0);
    return rc;
}

//------------------------------------------------------------------------------
