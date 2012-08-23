/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.
Copyright (c) 2001  Microsoft Corporation

Module Name:	FMD_LB.H

Abstract:		FLASH Media Driver Interface Samsung S3C6410 CPU with NAND Flash
                controller.

Environment:	As noted, this media driver works on behalf of the FAL to directly
				access the underlying FLASH hardware.  Consquently, this module
				needs to be linked with FLASHFAL.LIB to produce the device driver
				named FLASHDRV.DLL.

-----------------------------------------------------------------------------*/
#ifndef _S3C6410_FMD_LB_
#define _S3C6410_FMD_LB_

///////////////////////////////////////////////
// Spare Area Layout (64 bytes) for 2KByte/Page
///////////////////////////////////////////////
// +-----+-----------+--------+--------+----------+----------+----------+----------+-----------+-----------+----------+
// | 1B  | 4B        | 1B     | 2B     | 4B       | 4B       | 4B       | 4B       | 4B        | 4B        | 8B       |
// | Bad | Rsv1      | OEMRsv | Rsv2   | Sec0 ECC | Sec1 ECC | Sec2 ECC | Sec3 ECC | Spare ECC | Reserved  | DirtyBit |
// +-----+-----------+--------+--------+----------+----------+----------+----------+-----------+-----------+----------+
// |<--- FMD Driver Reserved (8B) ---->|
//


#define LB_NAND_LOG_2_PAGES_PER_BLOCK	6			// Used to avoid multiplications

#ifdef __cplusplus
extern "C"  {
#endif
BOOL NAND_LB_ReadSectorInfo(SECTOR_ADDR sectorAddr, PSectorInfo pInfo, int mode);
BOOL NAND_LB_WriteSectorInfo(SECTOR_ADDR sectorAddr, PSectorInfo pInfo, int mode);
BOOL FMD_LB_ReadSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors, int mode);
BOOL RAW_LB_ReadSector(UINT32 startSectorAddr, LPBYTE pSectorBuff, LPBYTE pSectorInfoBuff);
BOOL LB_IsBlockBad(BLOCK_ID blockID, int mode);
DWORD FMD_LB_GetBlockStatus(BLOCK_ID blockID, int mode);
BOOL FMD_LB_EraseBlock(BLOCK_ID blockID, int mode);
BOOL FMD_LB_WriteSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors, int mode);
BOOL LB_MarkBlockBad(BLOCK_ID blockID, int mode);
BOOL FMD_LB_SetBlockStatus(BLOCK_ID blockID, DWORD dwStatus, int mode);
BOOL FMD_LB_GetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, PBYTE pOEMReserved, int mode);
BOOL FMD_LB_SetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, BYTE bOEMReserved, int mode);
void LB_NF_Reset(int mode);
#ifdef IROMBOOT
BOOL FMD_LB_WriteSector_Steploader(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors, int mode);
#endif
#ifdef __cplusplus
}
#endif

#endif _S3C6410_FMD_LB_
