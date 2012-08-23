/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.
Copyright (c) 2001  Microsoft Corporation

Module Name:	S3C6410.H

Abstract:		FLASH Media Driver Interface Samsung S3C6410 CPU with NAND Flash
                controller.

Environment:	As noted, this media driver works on behalf of the FAL to directly
				access the underlying FLASH hardware.  Consquently, this module
				needs to be linked with FLASHFAL.LIB to produce the device driver
				named FLASHDRV.DLL.

-----------------------------------------------------------------------------*/
#ifndef _S3C6410_FMD_SB_
#define _S3C6410_FMD_SB_

///////////////////////////////////////////////
// Spare Area Layout (16 bytes) for 512Byte/Page
///////////////////////////////////////////////
// +-----------+--------+-----+--------+----------+
// | 4B        | 1B     | 1B  | 2B     | 4B       |
// | Rsv1      | OEMRsv | Bad | Rsv2   | Main ECC |
// +-----------+--------+-----+--------+----------+
// |<--- FMD Driver Reserved (8B) ---->|
//


#define SB_NAND_LOG_2_PAGES_PER_BLOCK	5			// Used to avoid multiplications

#ifdef __cplusplus
extern "C"  {
#endif
void NAND_SB_ReadSectorInfo(SECTOR_ADDR sectorAddr, PSectorInfo pInfo, int mode);
BOOL NAND_SB_WriteSectorInfo(SECTOR_ADDR sectorAddr, PSectorInfo pInfo, int mode);
BOOL FMD_SB_ReadSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors, int mode);
BOOL RAW_SB_ReadSector(UINT32 startSectorAddr, LPBYTE pSectorBuff, LPBYTE pSectorInfoBuff);
BOOL SB_IsBlockBad(BLOCK_ID blockID, int mode);
DWORD FMD_SB_GetBlockStatus(BLOCK_ID blockID, int mode);
BOOL FMD_SB_EraseBlock(BLOCK_ID blockID, int mode);
BOOL FMD_SB_WriteSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors, int mode);
BOOL SB_MarkBlockBad(BLOCK_ID blockID, int mode);
BOOL FMD_SB_SetBlockStatus(BLOCK_ID blockID, DWORD dwStatus, int mode);
BOOL FMD_SB_GetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, PBYTE pOEMReserved, int mode);
BOOL FMD_SB_SetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, BYTE bOEMReserved, int mode);
void SB_NF_Reset(int mode);
#ifdef IROMBOOT
BOOL FMD_SB_WriteSector_Steploader(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors, int mode);
#endif
#ifdef __cplusplus
}
#endif

#endif _S3C6410_FMD_SB_

