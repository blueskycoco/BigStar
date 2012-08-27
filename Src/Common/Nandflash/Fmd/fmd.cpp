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
#include <bsp_cfg.h>
#include <fmd.h>
#include <s3c6410_nand.h>
#include <s3c6410_syscon.h>
#include <bsp_args.h>
#include <ethdbg.h>
#include "Cfnand.h"
#if MAGNETO
#include <image_cfg.h>
#endif

#define CHECK_SPAREECC  (1) // 1
#define DIRTYBIT_CHECK  (1)  // 1 if you use this check, you have to set CHECK_SPAREECC to 1
#define NAND_DEBUG      (0)
#define NAND_BASE       (0xB0200000)	// PA:0x70200000
#define IOPORT_BASE     (0xB2B08000)	// PA:0x7F008000
#define SYSCON_BASE     (0xB2A0F000)	// PA:0x7E00F000

#define RdPage512(x)        RdPage512R(x, (UINT32)(&(s6410NAND->NFDATA)))
#define RdPage512Unalign(x) RdPage512UnalignR(x, (UINT32)(&(s6410NAND->NFDATA)))
#define WrPage512(x)        WrPage512R(x, (UINT32)(&(s6410NAND->NFDATA)))
#define WrPage512Unalign(x) WrPage512UnalignR(x, (UINT32)(&(s6410NAND->NFDATA)))
#define RdPageInfo(x)       RdPageInfoR(x, (UINT32)(&(s6410NAND->NFDATA)))
#define WrPageInfo(x)       WrPageInfoR(x, (UINT32)(&(s6410NAND->NFDATA)))

extern "C"
{
	NANDDeviceInfo stDeviceInfo;

	void RdPage512R(unsigned char *bufPt, UINT32 nNFDATAAddr);
	void RdPage512UnalignR(unsigned char *bufPt, UINT32 nNFDATAAddr);
	void WrPage512R(unsigned char *bufPt, UINT32 nNFDATAAddr);
	void WrPage512UnalignR(unsigned char *bufPt, UINT32 nNFDATAAddr);
	void RdPageInfoR(PBYTE pBuff, UINT32 nNFDATAAddr);
	void WrPageInfoR(PBYTE pBuff, UINT32 nNFDATAAddr);
}

static volatile S3C6410_NAND_REG *s6410NAND = (S3C6410_NAND_REG *)NAND_BASE;
static volatile S3C6410_SYSCON_REG *s6410SYSCON = (S3C6410_SYSCON_REG *)SYSCON_BASE;

BOOL NEED_EXT_ADDR;
NANDDeviceInfo GetNandInfo(void) { return stDeviceInfo; }

#if MAGNETO
#define MAX_REGIONS 16
#define DEFAULT_COMPACTION_BLOCKS 4

BSP_ARGS *pBSPArgs;
static BOOL DefineLayout();
static FlashRegion g_pRegionTable[MAX_REGIONS];
static DWORD g_dwNumRegions;
static FlashInfo g_flashInfo;
static BYTE g_pFLSBuffer[NAND_LB_PAGE_SIZE];  // temporary sector size is adapted for large block NAND
#endif

#ifdef IROMBOOT

typedef struct
{
    UINT32	n8MECC0;        // 8MECC0
    UINT32	n8MECC1;        // 8MECC1
    UINT32	n8MECC2;        // 8MECC2
    UINT8	n8MECC3;        // 8MECC3
} MECC8;

#endif

BOOL ECC_CorrectData(SECTOR_ADDR sectoraddr, LPBYTE pData, UINT32 nDataLen, UINT32 nRetEcc, ECC_CORRECT_TYPE nType)
{
	DWORD  nErrStatus;
	DWORD  nErrDataNo;
	DWORD  nErrBitNo;
	UINT32 nErrDataMask;
	UINT32 nErrBitMask = 0x7;
   	UINT32 nErrByte;
	UINT32 nErrBit;
	BOOL bRet = TRUE;

//	RETAILMSG(1, (TEXT("#### FMD_DRIVER:::ECC_CorrectData 0x%x, %d, 0x%x, 0x%x\r\n"), sectoraddr, nDataLen, nRetEcc, nType));

	if (nType == ECC_CORRECT_MAIN)
	{
		nErrStatus   = 0;
		nErrDataNo   = 7;
		nErrBitNo    = 4;
		nErrDataMask = 0x7ff;
	}
	else if (nType == ECC_CORRECT_SPARE)
	{
		nErrStatus   = 2;
		nErrDataNo   = 21;
		nErrBitNo    = 18;
		nErrDataMask = 0xf;
	}
	else
	{
		return FALSE;
	}

	switch((nRetEcc>>nErrStatus) & 0x3)
	{
		case 0:	// No Error
			bRet = TRUE;
			break;
		case 1:	// 1-bit Error(Correctable)
			nErrByte = (nRetEcc>>nErrDataNo)&nErrDataMask;
			nErrBit  = (nRetEcc>>nErrBitNo)&nErrBitMask;
			if (nErrByte < nDataLen)
			{
				if ((nDataLen == SPARE_DATA_ECC_SIZE) && (IS_LB))
				{
        			RETAILMSG(1,(TEXT("%cECC correctable error(0x%x). Byte:%d, bit:%d\r\n"), 'S', sectoraddr, nErrByte, nErrBit));
					// the 1st byte through ECC engine is badblock byte for SectorInfo structure in large block NAND flash
					// the relationship between input through ECC engine and SectorInfo structure is like this
					//
					//   ReadByte  -  ECC input byte  -----------  SectorInfoLocation(if pSectorInfo = (LPBYTE)pData) (Little Endian)
					//     1byte   -  bBadBlock                 -  6th byte  (*pData[5])
					//     4byte   -  dwReserved1 & 0xff        -  1st byte  (*pData[0])
					//             -  (dwReserved1>>8) & 0xff   -  2nd byte  (*pData[1])
					//             -  (dwReserved1>>16) & 0xff  -  3rd byte  (*pData[2])
					//             -  (dwReserved1>>24) & 0xff  -  4th byte  (*pData[3])
					//     1byte   -  bOEMReserved              -  5th byte  (*pData[4])
					//
					if (nErrByte == 0)
						nErrByte = 5;  // pSectorInfo->bBadBlock
					else
						nErrByte -= 1;
				}
				else
				{
        			RETAILMSG(1,(TEXT("%cECC correctable error(0x%x). Byte:%d, bit:%d\r\n"), ((nDataLen==SECTOR_SIZE)?'M':'E'), sectoraddr, nErrByte, nErrBit));
				}
				(pData)[nErrByte] ^= (1<<(nErrBit));
				bRet = TRUE;
			}
			else
			{
				RETAILMSG(1,(TEXT(" Err:ErrByte(%d) is bigger than DataLen(%d)\r\n"), nErrByte, nDataLen));
				bRet = FALSE;
			}
            bRet = TRUE;
			break;
		case 2:	// Multiple Error
			if (nDataLen == SPARE_DATA_ECC_SIZE)
			{
    			RETAILMSG(0,(TEXT("SECC Uncorrectable Multiple error(0x%x)\r\n"), sectoraddr));
    		}
    		else if (nDataLen == SECTOR_SIZE)
    		{
    			RETAILMSG(1,(TEXT("MECC Uncorrectable Multiple error(0x%x)\r\n"), sectoraddr));
    		}
    		else
    		{
    			RETAILMSG(1,(TEXT("EECC Uncorrectable Multiple error(0x%x)\r\n"), sectoraddr));
    		}
			bRet = FALSE;
			break;
		case 3:	// ECC area Error
			if (nDataLen == SPARE_DATA_ECC_SIZE)
			{
    			RETAILMSG(1,(TEXT("SECC Uncorrectable Area error(0x%x)\r\n"), sectoraddr));
    		}
    		else if (nDataLen == SECTOR_SIZE)
    		{
    			RETAILMSG(1,(TEXT("MECC Uncorrectable Area error(0x%x)\r\n"), sectoraddr));
    		}
    		else
    		{
    			RETAILMSG(1,(TEXT("EECC Uncorrectable Area error(0x%x)\r\n"), sectoraddr));
    		}
            bRet = FALSE;
    		break;
		default:
			bRet = FALSE;
			break;
	}

	return bRet;
}

/*
	@func   DWORD | ReadFlashID | Reads the flash manufacturer and device codes.
	@rdesc  Manufacturer and device codes.
	@comm
	@xref
*/
static DWORD ReadFlashID(void)
{
	BYTE  Mfg, Dev;
	DWORD i;

	NF_nFCE_L();						// Deselect the flash chip.
	NF_CMD(CMD_READID);					// Send flash ID read command.

	NF_ADDR(0);

	for (i = 0; i < 10; i++)
	{
		Mfg	= NF_RDDATA_BYTE();

		if (Mfg == 0xEC || Mfg == 0x98)
			break;
	}
	

	Dev	= NF_RDDATA_BYTE();
	RETAILMSG(1, (TEXT(" ReadFlashID first chip cycle = 0x%x 0x%x\n\r"), Mfg,Dev));
#if 1  // read more infomation
	{
		BYTE  c3rd, c4th, c5th;

		c3rd = NF_RDDATA_BYTE();
		c4th = NF_RDDATA_BYTE();
		c5th = NF_RDDATA_BYTE();
		RETAILMSG(1, (TEXT(" ReadFlashID first chip 1st cycle = 0x%x\n\r"), Mfg));
		RETAILMSG(1, (TEXT(" ReadFlashID first chip 2nd cycle = 0x%x\n\r"), Dev));
		RETAILMSG(1, (TEXT(" ReadFlashID first chip 3rd cycle = 0x%x\n\r"), c3rd));
		RETAILMSG(1, (TEXT(" ReadFlashID first chip 4th cycle = 0x%x\n\r"), c4th));
		RETAILMSG(1, (TEXT(" ReadFlashID first chip 5th cycle = 0x%x\n\r"), c5th));
	}
#endif

	NF_nFCE_H();

	NF_nFCE1_L();						// Deselect the flash chip.
	NF_CMD(CMD_READID); 				// Send flash ID read command.

	NF_ADDR(0);

	for (i = 0; i < 10; i++)
	{
		Mfg = NF_RDDATA_BYTE();

		if (Mfg == 0xEC || Mfg == 0x98)
			break;
	}

	Dev = NF_RDDATA_BYTE();	
	RETAILMSG(1, (TEXT(" ReadFlashID second chip cycle = 0x%x 0x%x\n\r"), Mfg,Dev));
#if 1  // read more infomation
		{
			BYTE  c3rd, c4th, c5th;
	
			c3rd = NF_RDDATA_BYTE();
			c4th = NF_RDDATA_BYTE();
			c5th = NF_RDDATA_BYTE();
			RETAILMSG(1, (TEXT(" ReadFlashID second chip 1st cycle = 0x%x\n\r"), Mfg));
			RETAILMSG(1, (TEXT(" ReadFlashID second chip 2nd cycle = 0x%x\n\r"), Dev));
			RETAILMSG(1, (TEXT(" ReadFlashID second chip 3rd cycle = 0x%x\n\r"), c3rd));
			RETAILMSG(1, (TEXT(" ReadFlashID second chip 4th cycle = 0x%x\n\r"), c4th));
			RETAILMSG(1, (TEXT(" ReadFlashID second chip 5th cycle = 0x%x\n\r"), c5th));
		}
#endif
	
	NF_nFCE1_H();

	return ((DWORD)(Mfg<<8)+Dev);
}

/*
	@func   PVOID | FMD_Init | Initializes the NAND flash controller.
	@rdesc  Pointer to S3C6410 NAND controller registers.
	@comm
	@xref
*/
PVOID FMD_Init(LPCTSTR lpActiveReg, PPCI_REG_INFO pRegIn, PPCI_REG_INFO pRegOut)
{
	// Caller should have specified NAND controller address.
	//
	BOOL  bNandExt = FALSE;
    UINT8 nMID, nDID, nLargeCycle;
	int   nCnt;
	BOOL  bLastMode = SetKMode(TRUE);
	volatile DWORD rdid;
	UINT32 nTotPages;

	RETAILMSG(1,(TEXT("[FMD] ++FMD_Init()\r\n")));

#if MAGNETO
	pBSPArgs = ((BSP_ARGS *) IMAGE_SHARE_ARGS_UA_START);
#endif

	if (pRegIn && pRegIn->MemBase.Num && pRegIn->MemBase.Reg[0])
		s6410NAND = (S3C6410_NAND_REG *)(pRegIn->MemBase.Reg[0]);
	else
		s6410NAND = (S3C6410_NAND_REG *)NAND_BASE;


	// Configure SMC Chip Select Mux for NAND
	//
	s6410SYSCON = (S3C6410_SYSCON_REG *)SYSCON_BASE;
	s6410SYSCON->MEM_SYS_CFG = (s6410SYSCON->MEM_SYS_CFG & ~(0x1000)); // 8-bit data width
	s6410SYSCON->MEM_SYS_CFG = (s6410SYSCON->MEM_SYS_CFG & ~(0x3F)) | (0x00); // Xm0CSn[2] = NFCON CS0


	// Set up initial flash controller configuration.
	//
	s6410NAND->NFCONF = (NAND_TACLS  <<  12) | (NAND_TWRPH0 <<  8) | (NAND_TWRPH1 <<  4);
	s6410NAND->NFCONT = (0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(0x3<<1)|(1<<0);
	s6410NAND->NFSTAT = (1<<4);

	// Get manufacturer and device codes.
	rdid = ReadFlashID();
    RETAILMSG(1, (TEXT("[FMD:INF] FMD_Init() : Read ID = 0x%x\r\n"), rdid));


	nMID = (UINT8)(rdid >> 8);
	nDID = (UINT8)(rdid & 0xff);

    RETAILMSG(1, (TEXT("[FMD:INF] FMD_Init() : Read ID = 0x%08x\r\n"), rdid));
	for (nCnt = 0; astNandSpec[nCnt].nMID != 0; nCnt++)
	{
        if (nDID == astNandSpec[nCnt].nDID && nMID == astNandSpec[nCnt].nMID)
		{
			bNandExt = TRUE;
			break;
		}
	}

	if (!bNandExt)
	{
        RETAILMSG(1, (TEXT("[FMD:ERR] FMD_Init() : Unknown ID = 0x%08x\r\n"), rdid));
		SetKMode (bLastMode);
		return NULL;
	}

	NUM_OF_BLOCKS = astNandSpec[nCnt].nNumOfBlks;
	PAGES_PER_BLOCK = astNandSpec[nCnt].nPgsPerBlk;
	SECTORS_PER_PAGE = astNandSpec[nCnt].nSctsPerPg;
    nLargeCycle = astNandSpec[nCnt].nLargeCycle;

	nTotPages = NUM_OF_BLOCKS * PAGES_PER_BLOCK;
	if (((nTotPages-1)>>16) != 0x0)
		NEED_EXT_ADDR = TRUE;
	else
		NEED_EXT_ADDR = FALSE;

    RETAILMSG(1, (TEXT("NFCONF            = 0x%X\r\n"), s6410NAND->NFCONF));
    RETAILMSG(1, (TEXT("NUM_OF_BLOCKS     = %d\r\n"), NUM_OF_BLOCKS));
    RETAILMSG(1, (TEXT("PAGES_PER_BLOCK   = %d\r\n"), PAGES_PER_BLOCK));
    RETAILMSG(1, (TEXT("SECTORS_PER_PAGE  = %d\r\n"), SECTORS_PER_PAGE));
    RETAILMSG(1, (TEXT("Addr Cycle        = %d\r\n"), NEED_EXT_ADDR ? 5 : 4));

#if MAGNETO
	DefineLayout();
#endif

	SetKMode (bLastMode);

    RETAILMSG(1, (TEXT("[FMD] --FMD_Init()\r\n")));

	return((PVOID)s6410NAND);
}


/*
	@func   BOOL | FMD_ReadSector | Reads the specified sector(s) from NAND flash.
	@rdesc  TRUE = Success, FALSE = Failure.
	@comm
	@xref
*/
BOOL FMD_ReadSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors)
{
	BOOL bRet = TRUE;

#if (NAND_DEBUG)
	RETAILMSG(1, (TEXT("FMD::FMD_ReadSector 0x%x \r\n"), startSectorAddr));
#endif

	if ( IS_LB )
		bRet = FMD_LB_ReadSector(startSectorAddr, pSectorBuff, pSectorInfoBuff, dwNumSectors, USE_NFCE);
	else
		bRet = FMD_SB_ReadSector(startSectorAddr, pSectorBuff, pSectorInfoBuff, dwNumSectors, USE_NFCE);

#if (NAND_DEBUG)
	RETAILMSG(1, (TEXT("FMD::FMD_ReadSector -- \r\n")));
#endif

	return bRet;
}


BOOL FMD_WriteSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff,
                        DWORD dwNumSectors)
{
	BOOL bRet = TRUE;

#if (NAND_DEBUG)
	RETAILMSG(1, (TEXT("FMD::FMD_WriteSector 0x%x \r\n"), startSectorAddr));
#endif

	if ( IS_LB )
		bRet = FMD_LB_WriteSector(startSectorAddr, pSectorBuff, pSectorInfoBuff, dwNumSectors, USE_NFCE);
	else
		bRet = FMD_SB_WriteSector(startSectorAddr, pSectorBuff, pSectorInfoBuff, dwNumSectors, USE_NFCE);

	return bRet;
}

/*
	@func   BOOL | FMD_EraseBlock | Erases the specified flash block.
	@rdesc  TRUE = Success, FALSE = Failure.
	@comm
	@xref
*/
BOOL FMD_EraseBlock(BLOCK_ID blockID)
{
	BOOL bRet = TRUE;

#if (NAND_DEBUG)
	RETAILMSG(1, (TEXT("FMD::FMD_EraseBlock 0x%x \r\n")));
#endif

	if ( IS_LB )
		bRet = FMD_LB_EraseBlock(blockID, USE_NFCE);
	else
		bRet = FMD_SB_EraseBlock(blockID, USE_NFCE);

    return bRet;
}

BOOL FMD_Deinit(PVOID hFMD)
{
	return(TRUE);
}


/*
	@func   BOOL | FMD_GetInfo | Provides information on the NAND flash.
	@rdesc  TRUE = Success, FALSE = Failure.
	@comm
	@xref
*/
BOOL FMD_GetInfo(PFlashInfo pFlashInfo)
{
	UINT32  nCnt;
	UINT32 nNandID;
	UINT8 nMID, nDID;

    if (!pFlashInfo)
        return(FALSE);

	BOOL bLastMode = SetKMode(TRUE); // for READFlashID();

    pFlashInfo->flashType = NAND;

	nNandID = ReadFlashID();

	nMID = nNandID >> 8;
	nDID = nNandID & 0xff;

	for (nCnt = 0; astNandSpec[nCnt].nMID != 0; nCnt++)
	{
		if (nDID == astNandSpec[nCnt].nDID)
		{
			break;
		}
	}

	pFlashInfo->dwNumBlocks = NUM_OF_BLOCKS;
	pFlashInfo->wSectorsPerBlock = PAGES_PER_BLOCK;
	pFlashInfo->wDataBytesPerSector = NAND_SECTOR_SIZE;
	pFlashInfo->dwBytesPerBlock = (pFlashInfo->wSectorsPerBlock * pFlashInfo->wDataBytesPerSector);

    RETAILMSG(1, (TEXT("NUMBLOCKS : %d(0x%x), SECTORSPERBLOCK = %d(0x%x), BYTESPERSECTOR = %d(0x%x) BYTESPERBLOCK = %d(0x%x)\r\n"),
        pFlashInfo->dwNumBlocks, pFlashInfo->dwNumBlocks, pFlashInfo->wSectorsPerBlock, pFlashInfo->wSectorsPerBlock, pFlashInfo->wDataBytesPerSector, pFlashInfo->wDataBytesPerSector,
        pFlashInfo->dwBytesPerBlock, pFlashInfo->dwBytesPerBlock));

	SetKMode(bLastMode);

	return TRUE;
}

#if MAGNETO
BOOL  FMD_GetInfoEx(PFlashInfoEx pFlashInfo, PDWORD pdwNumRegions)
{
    RETAILMSG(1, (L"FMD_GetInfoEx enter.\r\n"));


    if (!pdwNumRegions)
    {
        return FALSE;
    }

    if (!pFlashInfo)
    {
        // Return required buffer size to caller
        *pdwNumRegions = g_dwNumRegions;
        return TRUE;
    }

    if (*pdwNumRegions < g_dwNumRegions)
    {
        *pdwNumRegions = g_dwNumRegions;
        DEBUGMSG (1, (TEXT("FMD_GetInfoEx: Insufficient buffer for number of regions")));
        return FALSE;
    }

    memcpy (pFlashInfo->region, g_pRegionTable, g_dwNumRegions * sizeof(FlashRegion));

    for (DWORD iRegion = 0; iRegion < g_dwNumRegions; iRegion++) {
        RETAILMSG(1, (L"Type=%d, StartP=0x%x, NumP=0x%x, NumL=0x%x, Sec/Blk=0x%x, B/Blk=0x%x, Compact=%d.\r\n",
            g_pRegionTable[iRegion].regionType,
            g_pRegionTable[iRegion].dwStartPhysBlock,
            g_pRegionTable[iRegion].dwNumPhysBlocks,
            g_pRegionTable[iRegion].dwNumLogicalBlocks,
            g_pRegionTable[iRegion].dwSectorsPerBlock,
            g_pRegionTable[iRegion].dwBytesPerBlock,
            g_pRegionTable[iRegion].dwCompactBlocks));

    }

    *pdwNumRegions = g_dwNumRegions;

    pFlashInfo->cbSize					= sizeof(FlashInfoEx);
    pFlashInfo->flashType				= NAND;
    pFlashInfo->dwNumBlocks			= NUM_OF_BLOCKS;
    pFlashInfo->dwDataBytesPerSector	= NAND_SECTOR_SIZE;
    pFlashInfo->dwNumRegions			= g_dwNumRegions;

    return(TRUE);
}

BOOL FMD_GetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, PBYTE pOEMReserved)
{
	if ( IS_LB )
		FMD_LB_GetOEMReservedByte( physicalSectorAddr,  pOEMReserved, USE_NFCE);
	else
		FMD_SB_GetOEMReservedByte( physicalSectorAddr,  pOEMReserved, USE_NFCE);

	return TRUE;
}

//  FMD_SetOEMReservedByte
//
//  Sets the OEM reserved byte (for metadata) for the specified physical sector.
//
BOOL FMD_SetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, BYTE bOEMReserved)
{
    BOOL bRet = TRUE;

	if ( IS_LB )
		bRet = FMD_LB_SetOEMReservedByte(physicalSectorAddr, bOEMReserved, USE_NFCE);
	else
		bRet = FMD_SB_SetOEMReservedByte(physicalSectorAddr, bOEMReserved, USE_NFCE);

    return bRet;
}

//  FMD_PowerUp
//
//  Performs any necessary powerup procedures...
//
VOID FMD_PowerUp(VOID)
{
	// Set up initial flash controller configuration.
	s6410NAND->NFCONF = (NAND_TACLS  <<  12) | (NAND_TWRPH0 <<  8) | (NAND_TWRPH1 <<  4);
	s6410NAND->NFCONT = (0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(0x3<<1)|(1<<0);
	s6410NAND->NFSTAT = (1<<4);
    RETAILMSG(1,(TEXT("--[FMD] FMD_PowerUp\r\n")));
}

//  FMD_PowerDown
//
//  Performs any necessary powerdown procedures...
//
VOID FMD_PowerDown(VOID)
{
    RETAILMSG(1,(TEXT("--[FMD] FMD_PowerDown\r\n")));
}

//  FMD_OEMIoControl
//
//  Used for any OEM defined IOCTL operations
//
BOOL FMD_OEMIoControl(DWORD dwIoControlCode, PBYTE pInBuf, DWORD nInBufSize, PBYTE pOutBuf, DWORD nOutBufSize, PDWORD pBytesReturned)
{
#if	MAGNETO
    BSP_ARGS *pBSPArgs = ((BSP_ARGS *) IMAGE_SHARE_ARGS_UA_START);
#endif

    RETAILMSG(1,(TEXT("[FMD] FMD_OEMIoControl(0x%08x)\r\n"), dwIoControlCode));

	switch(dwIoControlCode)
	{

	    case IOCTL_FMD_GET_INTERFACE:
	    {
			RETAILMSG(1, (TEXT("[FMD] FMD_OEMIoControl() : IOCTL_FMD_GET_INTERFACE\r\n")));

	        if (!pOutBuf || nOutBufSize < sizeof(FMDInterface))
	        {
				RETAILMSG(1, (TEXT("[FMD:ERR] FMD_OEMIoControl() : Invalid Parameter\r\n")));
				return FALSE;
	        }

	        PFMDInterface pInterface = (PFMDInterface)pOutBuf;

	        pInterface->cbSize = sizeof(FMDInterface);
	        pInterface->pInit = FMD_Init;
	        pInterface->pDeInit = FMD_Deinit;
	        pInterface->pGetInfo = FMD_GetInfo;
	        pInterface->pGetInfoEx = FMD_GetInfoEx;
	        pInterface->pGetBlockStatus = FMD_GetBlockStatus;
	        pInterface->pSetBlockStatus = FMD_SetBlockStatus;
	        pInterface->pReadSector = FMD_ReadSector;
	        pInterface->pWriteSector = FMD_WriteSector;
	        pInterface->pEraseBlock = FMD_EraseBlock;
	        pInterface->pPowerUp = FMD_PowerUp;
	        pInterface->pPowerDown = FMD_PowerDown;
	        pInterface->pGetPhysSectorAddr = NULL;

	        break;
	    }
#if 0
		case IOCTL_FMD_LOCK_BLOCKS:
			// LOCK is not supported.!!!!
			BlockLockInfo * pLockInfo;
			pLockInfo = (BlockLockInfo *)pInBuf;
			RETAILMSG(1, (TEXT("IOCTL_FMD_LOCK_BLOCKS!!!!(0x%x,0x%x) \r\n"), pLockInfo->StartBlock, pLockInfo->NumBlocks));

			if ( IS_LB )		// Large Block
			{
				if ( READ_REGISTER_BYTE(pNFSBLK) >> 6 < (ULONG)(pLockInfo->StartBlock + pLockInfo->NumBlocks) )
					WRITE_REGISTER_USHORT(pNFSBLK, (pLockInfo->StartBlock + pLockInfo->NumBlocks)<<6);
			}
			else	// Small Block
			{
				if ( READ_REGISTER_BYTE(pNFSBLK) >> 5 < (ULONG)(pLockInfo->StartBlock + pLockInfo->NumBlocks)*8 )
				{
//					RETAILMSG(1, (TEXT("Write value (0x%x) \r\n"), ((ULONG)(pLockInfo->StartBlock + pLockInfo->NumBlocks)*8)<<5));
					WRITE_REGISTER_ULONG(pNFSBLK, ((ULONG)(pLockInfo->StartBlock + pLockInfo->NumBlocks)*8)<<5);
//					RETAILMSG(1, (TEXT("Read value  (0x%x) \r\n"), READ_REGISTER_ULONG(pNFSBLK)));
				}
			}
			pBSPArgs->nfsblk = pLockInfo->StartBlock + pLockInfo->NumBlocks;

			break;
		case IOCTL_FMD_UNLOCK_BLOCKS:
			RETAILMSG(1, (TEXT("IOCTL_FMD_UNLOCK_BLOCKS!!!!(0x%x,0x%x) \r\n"), pLockInfo->StartBlock, pLockInfo->NumBlocks));
			RETAILMSG(1, (TEXT("S3C6410 Does not support IOCTL_FMD_UNLOCK_BLOCKS !!!! \r\n")));
	        return(FALSE);
#endif
	    default:
	        RETAILMSG(1, (TEXT("FMD_OEMIoControl: unrecognized IOCTL (0x%x)\r\n"), dwIoControlCode));
	        return(FALSE);

    }

    return TRUE;
}
#endif // MAGNETO

static BOOL IsBlockBad(BLOCK_ID blockID)
{
	BOOL bRet = FALSE;

	if ( IS_LB )
		bRet = LB_IsBlockBad(blockID, USE_NFCE);
	else
		bRet = SB_IsBlockBad(blockID, USE_NFCE);

	return bRet;
}


/*
	@func   DWORD | FMD_GetBlockStatus | Returns the status of the specified block.
	@rdesc  Block status (see fmd.h).
	@comm
	@xref
*/
DWORD FMD_GetBlockStatus(BLOCK_ID blockID)
{
	DWORD dwResult = 0;

	if ( IS_LB )
		dwResult = FMD_LB_GetBlockStatus(blockID, USE_NFCE);
	else
		dwResult = FMD_SB_GetBlockStatus(blockID, USE_NFCE);

	return dwResult;
}


/*
	@func   BOOL | MarkBlockBad | Marks the specified block as bad.
	@rdesc  TRUE = Success, FALSE = Failure.
	@comm
	@xref
*/
static BOOL MarkBlockBad(BLOCK_ID blockID)
{
	BOOL bRet = TRUE;

	if ( IS_LB )
		bRet = LB_MarkBlockBad(blockID, USE_NFCE);
	else
		bRet = SB_MarkBlockBad(blockID, USE_NFCE);

    return bRet;
}

/*
	@func   BOOL | FMD_SetBlockStatus | Marks the block with the specified block status.
	@rdesc  TRUE = Success, FALSE = Failure.
	@comm
	@xref
*/
BOOL FMD_SetBlockStatus(BLOCK_ID blockID, DWORD dwStatus)
{
	BOOL bRet = TRUE;

	if ( IS_LB )
		bRet = FMD_LB_SetBlockStatus(blockID, dwStatus, USE_NFCE);
	else
		bRet = FMD_SB_SetBlockStatus(blockID, dwStatus, USE_NFCE);

    return bRet;
}

BOOL FMD_LB_ReadSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors,int mode)
{
	DWORD  i;
	volatile DWORD rddata;
	UINT32 nRetEcc = 0;
	UINT32 MECCBuf[4];
	UINT32 nSectorLoop;
	UINT32 nSpareAddr = NAND_SECTOR_SIZE;
	UINT32 nColAddr = 0;
	UINT32 nPageAddr = startSectorAddr;
#if CHECK_SPAREECC
	UINT32 SECCBuf;
	WORD   wReserved2copy1 = 0;
	WORD   wReserved2copy2 = 0;
#else
	UINT32 nTmpBuf;
#endif  // CHECK_SPAREECC

#if (NAND_DEBUG)
	RETAILMSG(1, (TEXT("FMD::FMD_LB_ReadSector 0x%x \r\n"), nPageAddr));
#endif

	if (!pSectorBuff && !pSectorInfoBuff)
		return(FALSE);

	if ( dwNumSectors > 1 )
	{
		RETAILMSG(1, (TEXT("######## FATAL ERROR => FMD::FMD_ReadSector->dwNumsectors is bigger than 1. \r\n")));
		return FALSE;
	}

	if (!pSectorBuff)
	{
		return NAND_LB_ReadSectorInfo(nPageAddr, pSectorInfoBuff, mode);
	}

	BOOL bLastMode = SetKMode(TRUE);
/*+add for next device support dillon 0824*/
	if(startSectorAddr>8192*64)
	{
		RETAILMSG(1, (TEXT("FMD::FMD_LB_ReadSector BlockID 0x%x \r\n"), startSectorAddr-8192*64));
		nPageAddr = startSectorAddr-8192*64;
		NF_nFCE1_L();
	}
	else
	{
		NF_nFCE_L();
	}
/*-add for next device support dillon 0824*/
	NF_CLEAR_RB();

	NF_CMD(CMD_READ);							// Send read command.

	NF_ADDR((nSpareAddr)&0xff);
	NF_ADDR((nSpareAddr>>8)&0xff);
	NF_ADDR((nPageAddr)&0xff);
	NF_ADDR((nPageAddr>>8)&0xff);
	if (NEED_EXT_ADDR)
		NF_ADDR((nPageAddr>>16)&0xff);

	NF_CMD(CMD_READ3);	// 2nd command

	NF_DETECT_RB();								// Wait for command to complete.

	if (pSectorInfoBuff)
	{
#if CHECK_SPAREECC
		NF_RSTECC();
		NF_MECC_UnLock();
#endif  // CHECK_SPAREECC
		pSectorInfoBuff->bBadBlock = NF_RDDATA_BYTE();      // bBadBlock (1)
		pSectorInfoBuff->dwReserved1 = NF_RDDATA_WORD();    // dwReserved1 (4)
		pSectorInfoBuff->bOEMReserved = NF_RDDATA_BYTE();   // bOEMReserved (1)
#if CHECK_SPAREECC
		NF_MECC_Lock();
#endif  // CHECK_SPAREECC
		pSectorInfoBuff->wReserved2 = NF_RDDATA_BYTE();     // bOEMReserved2 (2)
		pSectorInfoBuff->wReserved2 |= (NF_RDDATA_BYTE()<<8);

		// Read the copy1 of wReserved2
		wReserved2copy1 = NF_RDDATA_BYTE();                 // Copy1 of wReserved2 (2)
		wReserved2copy1 |= (NF_RDDATA_BYTE()<<8);

		// Read the copy2 of wReserved2
		wReserved2copy2 = NF_RDDATA_BYTE();                 // Copy2 of wReserved2 (2)
		wReserved2copy2 |= (NF_RDDATA_BYTE()<<8);

        // Determine wReserved2 data is correct or not.
        if (pSectorInfoBuff->wReserved2 != wReserved2copy1)
        {
            if ( wReserved2copy1 == wReserved2copy2 )
            {
                pSectorInfoBuff->wReserved2 = wReserved2copy1;
            }
            else if ( pSectorInfoBuff->wReserved2 == wReserved2copy2 )
            {
                // pSectorInfoBuff->wReserved2 data is correct. Do nothing.
            }
            else    // if every 3 values are different. Error.
            {
        		RETAILMSG(1, (TEXT("######## FATAL ERROR => FMD::FMD_ReadSector->wReserved2 data is not correct (0x%x)\r\n"), pSectorInfoBuff->wReserved2));
            }
        }
	}
	else
	{
		 for(i=0; i<sizeof(SectorInfo)/sizeof(DWORD); i++)
		 {
			rddata = (DWORD) NF_RDDATA_WORD();		// read and trash the data
		 }
         rddata = (DWORD) NF_RDDATA_WORD();		// read and trash the data. copy of wReserved2.

	}

#if CHECK_SPAREECC
	if (pSectorInfoBuff)
	{
		SECCBuf = NF_RDDATA_WORD();                     // ECC of first 6 bytes of Spare Area
		NF_WRMECCD0( ((SECCBuf&0xff00)<<8)|(SECCBuf&0xff) );
		NF_WRMECCD1( ((SECCBuf&0xff000000)>>8)|((SECCBuf&0xff0000)>>16) );
		nRetEcc = NF_ECC_ERR0;

		if (!ECC_CorrectData(nPageAddr, (LPBYTE)pSectorInfoBuff, SPARE_DATA_ECC_SIZE, nRetEcc, ECC_CORRECT_MAIN))
		{
            if ( pSectorInfoBuff->bBadBlock == 0xFF ) // Not Bad block
            {
            
			/*+add for next device support dillon 0824*/
				if(startSectorAddr>8192*64)
				{
                	NF_nFCE1_H();
				}else
					NF_nFCE_H();		
			/*-add for next device support dillon 0824*/
                SetKMode (bLastMode);
                return FALSE;
            }
        }
    }
    else
    {
        rddata = (DWORD) NF_RDDATA_WORD();  // read and trash the data
    }
#else
	nTmpBuf = NF_RDDATA_WORD();
#endif  // CHECK_SPAREECC

#if CHECK_SPAREECC
	NF_RSTECC();
	NF_MECC_UnLock();
#endif  // CHECK_SPAREECC
	for (nSectorLoop = 0; nSectorLoop < SECTORS_PER_PAGE; nSectorLoop++)
	{
		MECCBuf[nSectorLoop] = NF_RDDATA_WORD();    // 4 x ECC of MData (4)
	}
#if CHECK_SPAREECC
	NF_MECC_Lock();
#endif  // CHECK_SPAREECC

#if CHECK_SPAREECC
	SECCBuf = NF_RDDATA_WORD();                     // ECC of MData area (4)
	NF_WRMECCD0( ((SECCBuf&0xff00)<<8)|(SECCBuf&0xff) );
	NF_WRMECCD1( ((SECCBuf&0xff000000)>>8)|((SECCBuf&0xff0000)>>16) );

	nRetEcc = NF_ECC_ERR0;

	if (!ECC_CorrectData(nPageAddr, (LPBYTE)MECCBuf, SPARE_MECC_ECC_SIZE, nRetEcc, ECC_CORRECT_MAIN))
	{
	    if ( pSectorInfoBuff->bBadBlock == 0xFF ) // Not Bad block
	    {
			/*+add for next device support dillon 0824*/
				if(startSectorAddr>8192*64)
				{
                	NF_nFCE1_H();
				}else
					NF_nFCE_H();		
			/*-add for next device support dillon 0824*/
    		SetKMode (bLastMode);
    		return FALSE;
    	}
	}
#endif  // CHECK_SPAREECC

#if DIRTYBIT_CHECK
	if (pSectorInfoBuff)
	{
		UINT8 cDirtyBit[8];
		UINT32 nCnt;
		UINT32 nTmp;

		nTmp = NF_RDDATA_WORD();  // reserved (4)

		for (nCnt = 0; nCnt < 8; nCnt++)
		{
			cDirtyBit[nCnt] = NF_RDDATA_BYTE();     // 8 x DirtyBit (1)
		}

		if (!(cDirtyBit[(nPageAddr%64)/8] & (1<<(nPageAddr%8))))  // Check Dirty bit
		{
			pSectorInfoBuff->wReserved2 &= ~(1<<0);
		}
	}
#endif  // DIRTYBIT_CHECK

	for (nSectorLoop = 0; nSectorLoop < SECTORS_PER_PAGE; nSectorLoop++)
	{
		nColAddr = nSectorLoop * SECTOR_SIZE;

		NF_CMD(CMD_RDO);    // Send read command.
		NF_ADDR((nColAddr)&0xff);
		NF_ADDR((nColAddr>>8)&0xff);
		NF_CMD(CMD_RDO2);   // 2nd command

		NF_RSTECC();
		NF_MECC_UnLock();

		if( ((DWORD) (pSectorBuff+nSectorLoop*SECTOR_SIZE)) & 0x3)
		{
			for(i=0; i<SECTOR_SIZE/sizeof(DWORD); i++)
			{
				rddata = (DWORD) NF_RDDATA_WORD();
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+0] = (BYTE)(rddata & 0xff);
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+1] = (BYTE)(rddata>>8 & 0xff);
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+2] = (BYTE)(rddata>>16 & 0xff);
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+3] = (BYTE)(rddata>>24 & 0xff);
			}
		}
		else
		{
			RdPage512(pSectorBuff+nSectorLoop*SECTOR_SIZE);					// Read page/sector data.
		}

		NF_MECC_Lock();

		NF_WRMECCD0( ((MECCBuf[nSectorLoop]&0xff00)<<8)|(MECCBuf[nSectorLoop]&0xff) );
		NF_WRMECCD1( ((MECCBuf[nSectorLoop]&0xff000000)>>8)|((MECCBuf[nSectorLoop]&0xff0000)>>16) );

		nRetEcc = NF_ECC_ERR0;

		if (!ECC_CorrectData(nPageAddr, pSectorBuff+nSectorLoop*SECTOR_SIZE, SECTOR_SIZE, nRetEcc, ECC_CORRECT_MAIN))
		{
			/*+add for next device support dillon 0824*/
				if(startSectorAddr>8192*64)
				{
                	NF_nFCE1_H();
				}else
					NF_nFCE_H();		
			/*-add for next device support dillon 0824*/
       		SetKMode (bLastMode);
       		return FALSE;
		}
	}

	/*+add for next device support dillon 0824*/
		if(startSectorAddr>8192*64)
		{
			NF_nFCE1_H();
		}else
			NF_nFCE_H();		
	/*-add for next device support dillon 0824*/

	SetKMode (bLastMode);

	return(TRUE);
}

BOOL RAW_LB_ReadSector(UINT32 startSectorAddr, LPBYTE pSectorBuff, LPBYTE pSectorInfoBuff)
{
	UINT32 nPageAddr = startSectorAddr;
	DWORD  i;
	volatile DWORD rddata;
	UINT32 nRetEcc = 0;
	UINT32 nSectorLoop;
	UINT32 nColAddr = 0;

	if (!pSectorBuff && !pSectorInfoBuff)
		return(FALSE);

	BOOL bLastMode = SetKMode(TRUE);

	NF_nFCE_L();

	NF_CLEAR_RB();

	NF_CMD(CMD_READ);							// Send read command.

	NF_ADDR((nColAddr)&0xff);
	NF_ADDR((nColAddr>>8)&0xff);
	NF_ADDR((nPageAddr)&0xff);
	NF_ADDR((nPageAddr>>8)&0xff);
	if (NEED_EXT_ADDR)
		NF_ADDR((nPageAddr>>16)&0xff);

	NF_CMD(CMD_READ3);	// 2nd command
	NF_DETECT_RB();								// Wait for command to complete.

	for (nSectorLoop = 0; nSectorLoop < 4; nSectorLoop++)
	{
		if( ((DWORD) (pSectorBuff+nSectorLoop*SECTOR_SIZE)) & 0x3)
		{
			for(i=0; i<SECTOR_SIZE/sizeof(DWORD); i++)
			{
				rddata = (DWORD) NF_RDDATA_WORD();
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+0] = (BYTE)(rddata & 0xff);
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+1] = (BYTE)(rddata>>8 & 0xff);
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+2] = (BYTE)(rddata>>16 & 0xff);
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+3] = (BYTE)(rddata>>24 & 0xff);
			}
		}
		else
		{
			RdPage512(pSectorBuff+nSectorLoop*SECTOR_SIZE);					// Read page/sector data.
		}
	}

	for(i=0; i<64; i++)
	{
		*pSectorInfoBuff = NF_RDDATA_BYTE();		// read and trash the data
		pSectorInfoBuff+=1;
	}

	NF_nFCE_H();

	SetKMode (bLastMode);

	return(TRUE);
}

BOOL NAND_LB_ReadSectorInfo(SECTOR_ADDR sectorAddr, PSectorInfo pInfo, int mode)
{
	BOOL   bRet = TRUE;
	UINT32 nSpareAddr = NAND_SECTOR_SIZE;
	UINT32 nPageAddr = sectorAddr;
#if CHECK_SPAREECC
	UINT32 SECCBuf;
	UINT32 nRetEcc = 0;
    WORD   wReserved2copy1 = 0;
    WORD   wReserved2copy2 = 0;
#endif  // CHECK_SPAREECC
	UINT32 nTmpBuf;
    if(pInfo == NULL)
    {
        RETAILMSG(1, (TEXT("NAND_LB_ReadSectorInfo : pInfo == NULL\r\n")));
        return FALSE;
    }
#ifdef IROMBOOT
    if((pInfo != NULL) && (sectorAddr < (STEPLDR_LENGTH/SECTOR_SIZE)))
    {
    	pInfo->bBadBlock = BADBLOCKMARK;
    	pInfo->dwReserved1  = 0xFFFFFFFF;
    	pInfo->bOEMReserved = ~(OEM_BLOCK_RESERVED | OEM_BLOCK_READONLY);;
    	pInfo->wReserved2 = 0xFFFF;
    	return bRet;
    }
#endif

	BOOL bLastMode = SetKMode(TRUE);
	/*+add for next device support dillon 0824*/
	if(sectorAddr>8192*64)
	{
		nPageAddr = sectorAddr-8192*64;
		NF_nFCE1_L();
	}
	else
	{
		NF_nFCE_L();
	}
	/*-add for next device support dillon 0824*/


	NF_CLEAR_RB();
	NF_CMD(CMD_READ);							// Send read confirm command.

	NF_ADDR((nSpareAddr)&0xff);
	NF_ADDR((nSpareAddr>>8)&0xff);
	NF_ADDR((nPageAddr)&0xff);
	NF_ADDR((nPageAddr>>8)&0xff);
	if (NEED_EXT_ADDR)
		NF_ADDR((nPageAddr>>16)&0xff);

	NF_CMD(CMD_READ3);
	NF_DETECT_RB();

#if CHECK_SPAREECC
	NF_RSTECC();
	NF_MECC_UnLock();
#endif  // CHECK_SPAREECC
	pInfo->bBadBlock = NF_RDDATA_BYTE();        // bBadBlock (1)
	pInfo->dwReserved1  = NF_RDDATA_WORD();     // dwReserved1 (4)
	pInfo->bOEMReserved = NF_RDDATA_BYTE();     // bOEMReserved (1)
#if CHECK_SPAREECC
	NF_MECC_Lock();
#endif  // CHECK_SPAREECC
	pInfo->wReserved2 = NF_RDDATA_BYTE();       // wReserved2 (2)
	pInfo->wReserved2 |= (NF_RDDATA_BYTE()<<8);

    // Read the copy1 of wReserved2 (2)
    wReserved2copy1 = NF_RDDATA_BYTE();
    wReserved2copy1 |= (NF_RDDATA_BYTE()<<8);

    // Read the copy2 of wReserved2 (2)
    wReserved2copy2 = NF_RDDATA_BYTE();
    wReserved2copy2 |= (NF_RDDATA_BYTE()<<8);

    // Determine wReserved2 data is correct or not.
    if (pInfo->wReserved2 != wReserved2copy1)
    {
        if ( wReserved2copy1 == wReserved2copy2 )
        {
            pInfo->wReserved2 = wReserved2copy1;
        }
        else if ( pInfo->wReserved2 == wReserved2copy2 )
        {
            // pInfo->wReserved2 data is correct. Do nothing.
        }
        else    // if every 3 values are different. Error.
        {
    		RETAILMSG(1, (TEXT("######## FATAL ERROR => FMD::FMD_ReadSector->wReserved2 data is not correct (0x%x)\r\n"), pInfo->wReserved2));
        }
    }

#if CHECK_SPAREECC
	SECCBuf = NF_RDDATA_WORD(); // ECC of first 6 bytes of Spare Area
	NF_WRMECCD0( ((SECCBuf&0xff00)<<8)|(SECCBuf&0xff) );
	NF_WRMECCD1( ((SECCBuf&0xff000000)>>8)|((SECCBuf&0xff0000)>>16) );
	nRetEcc = NF_ECC_ERR0;

	if (!ECC_CorrectData(nPageAddr, (LPBYTE)pInfo, SPARE_DATA_ECC_SIZE, nRetEcc, ECC_CORRECT_MAIN))
	{
	    if ( pInfo->bBadBlock == 0xFF ) // Not Bad block
	    {
			/*+add for next device support dillon 0824*/
			if(sectorAddr>8192*64)
			{
            	NF_nFCE1_H();
			}else
				NF_nFCE_H();		
			/*-add for next device support dillon 0824*/
    		SetKMode (bLastMode);
    		return FALSE;
	    }
	}
#else
	nTmpBuf = NF_RDDATA_WORD();
#endif  // CHECK_SPAREECC

	nTmpBuf = NF_RDDATA_WORD(); // Mecc[0]
	nTmpBuf = NF_RDDATA_WORD(); // Mecc[1]
	nTmpBuf = NF_RDDATA_WORD(); // Mecc[2]
	nTmpBuf = NF_RDDATA_WORD(); // Mecc[3]

	nTmpBuf = NF_RDDATA_WORD(); // ECC of MData area (4)

#if DIRTYBIT_CHECK
	{
		UINT8 cDirtyBit[8];
		UINT32 nCnt;
		UINT32 nTmp;

		nTmp = NF_RDDATA_WORD();  // reserved

		for (nCnt = 0; nCnt < 8; nCnt++)
		{
			cDirtyBit[nCnt] = NF_RDDATA_BYTE();
		}

		if (!(cDirtyBit[(nPageAddr%64)/8] & (1<<(nPageAddr%8))))  // Check Dirty bit
		{
			pInfo->wReserved2 &= ~(1<<0);
		}
	}
#endif  // DIRTYBIT_CHECK

	/*+add for next device support dillon 0824*/
	if(sectorAddr>8192*64)
	{
		NF_nFCE1_H();
	}else
		NF_nFCE_H();		
	/*-add for next device support dillon 0824*/

	SetKMode(bLastMode);

	return bRet;
}


BOOL FMD_SB_ReadSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors,int mode)
{
	UINT32 nPageAddr = startSectorAddr;
	ULONG  MECC;
	UINT32 nRetEcc = 0;
	DWORD  nSctCnt = 0;

	if (!pSectorBuff && !pSectorInfoBuff)
		return(FALSE);

	BOOL bLastMode = SetKMode(TRUE);

#if (NAND_DEBUG)
	RETAILMSG(1,(TEXT("#### FMD_DRIVER:::FMD_sbreadT \r\n")));
#endif

	while (dwNumSectors--)
	{
#ifdef IROMBOOT
        if ((pSectorInfoBuff != NULL) && (nPageAddr < (STEPLDR_LENGTH/SECTOR_SIZE)))
        {
            pSectorInfoBuff->bBadBlock = BADBLOCKMARK;
            pSectorInfoBuff->dwReserved1  = 0xFFFFFFFF;
            pSectorInfoBuff->bOEMReserved = ~(OEM_BLOCK_RESERVED | OEM_BLOCK_READONLY);;
            pSectorInfoBuff->wReserved2 = 0xFFFF;

            pSectorInfoBuff++;
            nPageAddr++;
            nSctCnt++;

            // skip steploader sectors
            //  because they doesn't have valid sector information.
            // sector information area of steploader is used for 8-bit ECC
            continue;
        }
#endif
		NF_RSTECC();
		NF_MECC_UnLock();
		NF_nFCE_L();

		if (!pSectorBuff)
		{
			NF_CLEAR_RB();
			NF_CMD(CMD_READ2);							// Send read confirm command.

			NF_ADDR(0);									// Ignored.
			NF_ADDR((nPageAddr)&0xff);			// Page address.
			NF_ADDR((nPageAddr>>8)&0xff);
			if (NEED_EXT_ADDR)
				NF_ADDR((nPageAddr>>16)&0xff);

			NF_SB_DETECT_RB();

			RdPageInfo((PBYTE)pSectorInfoBuff);	// Read page/sector information.

			pSectorInfoBuff++;
		}
		else
		{
			NF_CLEAR_RB();

			NF_CMD(CMD_READ);							// Send read command.

			NF_ADDR(0);									// Column = 0.
			NF_ADDR((nPageAddr)&0xff);			// Page address.
			NF_ADDR((nPageAddr>>8)&0xff);
			if (NEED_EXT_ADDR)
				NF_ADDR((nPageAddr>>16)&0xff);

			NF_SB_DETECT_RB();								// Wait for command to complete.

			if( ((DWORD) pSectorBuff) & 0x3)
			{
				RdPage512Unalign (pSectorBuff);
			}
			else
			{
				RdPage512(pSectorBuff);					// Read page/sector data.
			}

			NF_MECC_Lock();

			if (pSectorInfoBuff)
			{
				RdPageInfo((PBYTE)pSectorInfoBuff);		// Read page/sector information.
				pSectorInfoBuff ++;
			}
			else
			{
				BYTE TempInfo[8];
				RdPageInfo(TempInfo);				   	// Read page/sector information.
			}

			MECC  = NF_RDDATA_BYTE() << 0;
			MECC |= NF_RDDATA_BYTE() << 8;
			MECC |= NF_RDDATA_BYTE() << 16;
			MECC |= (NF_RDMECC0() &0xff000000);

			NF_WRMECCD0( ((MECC&0xff00)<<8)|(MECC&0xff) );
			NF_WRMECCD1( ((MECC&0xff000000)>>8)|((MECC&0xff0000)>>16) );

			nRetEcc = NF_ECC_ERR0;

//          if (!ECC_CorrectData(nPageAddr, pSectorBuff+nSctCnt*SECTOR_SIZE, SECTOR_SIZE, nRetEcc, ECC_CORRECT_MAIN))
            if (!ECC_CorrectData(nPageAddr, pSectorBuff, SECTOR_SIZE, nRetEcc, ECC_CORRECT_MAIN))
			{
				NF_nFCE_H();
				SetKMode (bLastMode);
				return FALSE;
			}

			pSectorBuff += NAND_SECTOR_SIZE;
		}

		NF_nFCE_H();
		nPageAddr++;
		nSctCnt++;
	}

	SetKMode (bLastMode);

	return(TRUE);
}

BOOL RAW_SB_ReadSector(UINT32 startSectorAddr, LPBYTE pSectorBuff, LPBYTE pSectorInfoBuff)
{
	UINT32 nPageAddr = startSectorAddr;
	DWORD  i;
	volatile DWORD rddata;
	UINT32 nRetEcc = 0;
	UINT32 nSectorLoop;
	UINT32 nColAddr = 0;

	if (!pSectorBuff && !pSectorInfoBuff)
		return(FALSE);

	BOOL bLastMode = SetKMode(TRUE);

	NF_nFCE_L();

	NF_CLEAR_RB();

	NF_CMD(CMD_READ);							// Send read command.

	NF_ADDR((nColAddr)&0xff);
	NF_ADDR((nPageAddr)&0xff);
	NF_ADDR((nPageAddr>>8)&0xff);
	if (NEED_EXT_ADDR)
		NF_ADDR((nPageAddr>>16)&0xff);

	NF_CMD(CMD_READ3);	// 2nd command
	NF_SB_DETECT_RB();								// Wait for command to complete.

	for (nSectorLoop = 0; nSectorLoop < 1; nSectorLoop++)
	{
		if( ((DWORD) (pSectorBuff+nSectorLoop*SECTOR_SIZE)) & 0x3)
		{
			for(i=0; i<SECTOR_SIZE/sizeof(DWORD); i++)
			{
				rddata = (DWORD) NF_RDDATA_WORD();
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+0] = (BYTE)(rddata & 0xff);
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+1] = (BYTE)(rddata>>8 & 0xff);
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+2] = (BYTE)(rddata>>16 & 0xff);
				(pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+3] = (BYTE)(rddata>>24 & 0xff);
			}
		}
		else
		{
			RdPage512(pSectorBuff+nSectorLoop*SECTOR_SIZE);					// Read page/sector data.
		}
	}

	for( i = 0; i < 16; i++)
	{
		*pSectorInfoBuff = NF_RDDATA_BYTE();		// read and trash the data
		pSectorInfoBuff+=1;
	}

	NF_nFCE_H();

	SetKMode (bLastMode);

	return(TRUE);
}

BOOL FMD_LB_WriteSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors, int mode)
{
	DWORD  i;
	BOOL   bRet = TRUE;
	volatile UINT32 wrdata;
	UINT32 MECCBuf[4];
	UINT32 nSectorLoop;
	UINT32 nSpareAddr = NAND_SECTOR_SIZE;
	UINT32 nColAddr = 0;
	UINT32 nPageAddr = startSectorAddr;
#if CHECK_SPAREECC
	UINT32 SECCBuf[2];
#endif  // CHECK_SPAREECC

#if (NAND_DEBUG)
	RETAILMSG(1, (TEXT("FMD::FMD_LB_WriteSector 0x%x \r\n"), nPageAddr));
#endif

	//  Sanity check
	if (!pSectorBuff && !pSectorInfoBuff)
		return FALSE;

	if ( dwNumSectors > 1 )
	{
		RETAILMSG(1, (TEXT("######## FATAL ERROR => FMD::FMD_WriteSector->dwNumsectors is bigger than 1. \r\n")));
		return FALSE;
	}

	if (!pSectorBuff)
	{
		NAND_LB_WriteSectorInfo(nPageAddr, pSectorInfoBuff, mode);
		return TRUE;
	}

    BOOL bLastMode = SetKMode(TRUE);

	//  Enable Chip
	/*+add for next device support dillon 0824*/
	if(startSectorAddr>8192*64)
	{
		RETAILMSG(1, (TEXT("FMD::FMD_LB_WriteSector BlockID 0x%x \r\n"), startSectorAddr-8192*64));
		nPageAddr = startSectorAddr-8192*64;
		NF_nFCE1_L();
	}
	else
	{
		NF_nFCE_L();
	}
	/*-add for next device support dillon 0824*/

	//  Issue command
	NF_CMD(CMD_WRITE);

	//  Setup address
	NF_ADDR((nColAddr)&0xff);
	NF_ADDR((nColAddr>>8)&0xff);
	NF_ADDR((nPageAddr)&0xff);
	NF_ADDR((nPageAddr>>8)&0xff);
    if (NEED_EXT_ADDR)
        NF_ADDR((nPageAddr>>16)&0xff);

	for (nSectorLoop = 0; nSectorLoop < SECTORS_PER_PAGE; nSectorLoop++)
	{
		//  Initialize ECC register
		NF_RSTECC();
		NF_MECC_UnLock();

		//  Special case to handle un-aligned buffer pointer.
		//
		if( ((DWORD) (pSectorBuff+nSectorLoop*SECTOR_SIZE)) & 0x3)
		{
			//  Write the data
			for(i=0; i<SECTOR_SIZE/sizeof(DWORD); i++)
			{
				wrdata = (pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+0];
				wrdata |= (pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+1]<<8;
				wrdata |= (pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+2]<<16;
				wrdata |= (pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+3]<<24;
				NF_WRDATA_WORD(wrdata);
			}
		}
		else
		{
			WrPage512(pSectorBuff+nSectorLoop*SECTOR_SIZE);
		}

		//  Read out the ECC value generated by HW
		NF_MECC_Lock();

		MECCBuf[nSectorLoop] = NF_RDMECC0();
	}

	NF_CMD(CMD_RDI);
	NF_ADDR((nSpareAddr)&0xff);
	NF_ADDR((nSpareAddr>>8)&0xff);

	if(pSectorInfoBuff)
	{
#if CHECK_SPAREECC
		NF_RSTECC();
		NF_MECC_UnLock();
#endif  // CHECK_SPAREECC
		NF_WRDATA_BYTE(pSectorInfoBuff->bBadBlock);     // bBadBlock (1)
		NF_WRDATA_WORD(pSectorInfoBuff->dwReserved1);   // dwReserved1 (4)
		NF_WRDATA_BYTE(pSectorInfoBuff->bOEMReserved);  // bOEMReserved (1)
#if CHECK_SPAREECC
		NF_MECC_Lock();
		SECCBuf[0] = NF_RDMECC0();
#endif  // CHECK_SPAREECC
		NF_WRDATA_BYTE(pSectorInfoBuff->wReserved2&0xff);   // wReserved2 (2)
		NF_WRDATA_BYTE((pSectorInfoBuff->wReserved2>>8)&0xff);

        // Make another copy1 to avoid bit error of wReserved2 (2).
		NF_WRDATA_BYTE(pSectorInfoBuff->wReserved2&0xff);
		NF_WRDATA_BYTE((pSectorInfoBuff->wReserved2>>8)&0xff);

        // Make another copy2 to avoid bit error of wReserved2 (2).
		NF_WRDATA_BYTE(pSectorInfoBuff->wReserved2&0xff);
		NF_WRDATA_BYTE((pSectorInfoBuff->wReserved2>>8)&0xff);
	}
	else
	{
		// Make sure we advance the Flash's write pointer (even though we aren't writing the SectorInfo data)
		for(i=0; i<sizeof(SectorInfo)/sizeof(DWORD); i++)
		{
			NF_WRDATA_WORD(ALL_FF);
		}
		// copy1,2 of wReserved2 region.
		NF_WRDATA_WORD(ALL_FF);
	}

#if CHECK_SPAREECC
	if(pSectorInfoBuff)
	{
		NF_WRDATA_WORD(SECCBuf[0]); // ECC of first 6 bytes of Spare Area
	}
	else
	{
		// ECC of first 6 bytes of Spare Area
		NF_WRDATA_WORD(ALL_FF);
	}
#endif  // CHECK_SPAREECC

    // MECC data also included in SECC area.
#if CHECK_SPAREECC
	NF_RSTECC();
	NF_MECC_UnLock();
#endif  // CHECK_SPAREECC
	NF_WRDATA_WORD(MECCBuf[0]);
	NF_WRDATA_WORD(MECCBuf[1]);
	NF_WRDATA_WORD(MECCBuf[2]);
	NF_WRDATA_WORD(MECCBuf[3]);
#if CHECK_SPAREECC
	NF_MECC_Lock();
	SECCBuf[1] = NF_RDMECC0();
#endif  // CHECK_SPAREECC

#if CHECK_SPAREECC
	NF_WRDATA_WORD(SECCBuf[1]); // ECC of MData area (4)
#else
	NF_WRDATA_WORD(ALL_FF);
#endif  // CHECK_SPAREECC

#if DIRTYBIT_CHECK
	if (pSectorInfoBuff)
	{
		if ((pSectorInfoBuff->wReserved2 & 0x1) != 0x1)  // Set the dirty bit to 0
		{
			UINT32 nCnt;

			NF_WRDATA_WORD(ALL_FF);  // reserved

			for (nCnt = 0; nCnt < 8; nCnt++)
			{
				if (((nPageAddr%PAGES_PER_BLOCK)/8) == nCnt)
				{
					NF_WRDATA_BYTE(0xff & ~(1<<(nPageAddr%8)));
				}
				else
				{
					NF_WRDATA_BYTE(0xff);
				}
			}
		}
	}
#endif  // DIRTYBIT_CHECK

	//  Finish up the write operation
	NF_CMD(CMD_WRITE2);

	//  Wait for RB
	NF_WAITRB();

	if ( NF_RDSTAT & STATUS_ILLACC )
	{
		RETAILMSG(1, (TEXT("FMD_WriteSector() ######## Error Programming page (Illigar Access) %d!\n"), nPageAddr));
		s6410NAND->NFSTAT =  STATUS_ILLACC;	// Write 1 to clear.
		bRet = FALSE;
	}
	else
	{
		//  Check the status
		NF_CMD(CMD_STATUS);

		if(NF_RDDATA_BYTE() & STATUS_ERROR)
		{
			RETAILMSG(1, (TEXT("FMD_WriteSector() ######## Error Programming page %d!\n"), nPageAddr));
			bRet = FALSE;
		}
	}

	//  Disable the chip
	/*+add for next device support dillon 0824*/
	if(startSectorAddr>8192*64)
	{
		NF_nFCE1_H();
	}else
		NF_nFCE_H();		
	/*-add for next device support dillon 0824*/

	SetKMode(bLastMode);

	return bRet;
}

BOOL NAND_LB_WriteSectorInfo(SECTOR_ADDR sectorAddr, PSectorInfo pInfo, int mode)
{
	BOOL   bRet = TRUE;
	UINT32 nSpareAddr = NAND_SECTOR_SIZE;
	UINT32 nPageAddr = sectorAddr;
#if CHECK_SPAREECC
	UINT32 SECCBuf;
#endif  // CHECK_SPAREECC

	BOOL bLastMode = SetKMode(TRUE);

    if (nPageAddr == 0x4c0)
    	RETAILMSG(1, (TEXT("\n\nFMD::NAND_LB_WriteSectorInfo 0x%x %x\r\n\n\n"), nPageAddr,nSpareAddr));

	//  Chip enable
	/*+add for next device support dillon 0824*/
	if(sectorAddr>8192*64)
	{
		nPageAddr = sectorAddr-8192*64;
		NF_nFCE1_L();
	}
	else
	{
		NF_nFCE_L();
	}
	/*-add for next device support dillon 0824*/
	NF_CLEAR_RB();

	//  Write the command
	//  First, let's point to the spare area
	NF_CMD(CMD_WRITE);

	//  Write the address
	NF_ADDR((nSpareAddr)&0xff);
	NF_ADDR((nSpareAddr>>8)&0xff);
	NF_ADDR((nPageAddr)&0xff);
	NF_ADDR((nPageAddr>>8)&0xff);
	if (NEED_EXT_ADDR)
		NF_ADDR((nPageAddr>>16)&0xff);

#if CHECK_SPAREECC
	NF_RSTECC();
	NF_MECC_UnLock();
#endif  // CHECK_SPAREECC
	//  Now let's write the SectorInfo data
	//
	NF_WRDATA_BYTE(pInfo->bBadBlock);       // bBadBlock (1)
	NF_WRDATA_WORD(pInfo->dwReserved1);     // dwReserved1 (4)
	NF_WRDATA_BYTE(pInfo->bOEMReserved);    // bOEMReserved (1)
#if CHECK_SPAREECC
	NF_MECC_Lock();
	SECCBuf = NF_RDMECC0();
#endif  // CHECK_SPAREECC
	NF_WRDATA_BYTE(pInfo->wReserved2&0xff);     // wReserved2 (2)
	NF_WRDATA_BYTE((pInfo->wReserved2>>8)&0xff);

    // Make another copy1 to avoid bit error of wReserved2 (2).
	NF_WRDATA_BYTE(pInfo->wReserved2&0xff);
	NF_WRDATA_BYTE((pInfo->wReserved2>>8)&0xff);

    // Make another copy2 to avoid bit error of wReserved2 (2).
	NF_WRDATA_BYTE(pInfo->wReserved2&0xff);
	NF_WRDATA_BYTE((pInfo->wReserved2>>8)&0xff);

#if CHECK_SPAREECC
	NF_WRDATA_WORD(SECCBuf);    // ECC of first 6 bytes of Spare Area
#endif  // CHECK_SPAREECC


	NF_WRDATA_WORD(ALL_FF); // Mecc[0]
	NF_WRDATA_WORD(ALL_FF); // Mecc[1]
	NF_WRDATA_WORD(ALL_FF); // Mecc[2]
	NF_WRDATA_WORD(ALL_FF); // Mecc[3]

#if CHECK_SPAREECC
	NF_WRDATA_WORD(ALL_FF); // ECC of MData area (4)
#else
	NF_WRDATA_WORD(ALL_FF);
#endif  // CHECK_SPAREECC

#if DIRTYBIT_CHECK
	if ((pInfo->wReserved2 & 0x1) != 0x1)
	{
		UINT32 nCnt;

		NF_WRDATA_WORD(ALL_FF);

		for (nCnt = 0; nCnt < 8; nCnt++)
		{
			if (((nPageAddr%PAGES_PER_BLOCK)/8) == nCnt)
			{
				NF_WRDATA_BYTE(0xff & ~(1<<(nPageAddr%8)));
			}
			else
			{
				NF_WRDATA_BYTE(0xff);
			}
		}
	}
#endif  // DIRTYBIT_CHECK

	//  Issue the write complete command
	NF_CMD(CMD_WRITE2);

	//  Check ready bit
	NF_WAITRB();

	if ( NF_RDSTAT & STATUS_ILLACC )
	{
		RETAILMSG(1, (TEXT("NAND_LB_WriteSectorInfo() ######## Error Programming page (Illigar Access) %d!\n"), nPageAddr));
		s6410NAND->NFSTAT = STATUS_ILLACC;	// Write 1 to clear.
		bRet = FALSE;
	}
	else
	{
		//  Check the status of program
		NF_CMD(CMD_STATUS);

		if( NF_RDDATA_BYTE() & STATUS_ERROR)
		{
			RETAILMSG(1, (TEXT("NAND_LB_WriteSectorInfo() ######## Error Programming page %d!\n"), nPageAddr));
			bRet = FALSE;
		}
	}

	/*+add for next device support dillon 0824*/
	if(sectorAddr>8192*64)
	{
		NF_nFCE1_H();
	}else
		NF_nFCE_H();		
	/*-add for next device support dillon 0824*/

	SetKMode(bLastMode);

	return bRet;
}


BOOL FMD_SB_WriteSector(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors,int mode)
{
	BYTE   Status;
	UINT32 nPageAddr = (UINT32)startSectorAddr;
	ULONG  MECC;

	if (!pSectorBuff && !pSectorInfoBuff)
		return(FALSE);

#if (NAND_DEBUG)
	RETAILMSG(1,(TEXT("#### FMD_DRIVER:::FMD_sbwrite \r\n")));
#endif

	BOOL bLastMode = SetKMode(TRUE);

	NF_nFCE_L();						// Select the flash chip.

	while (dwNumSectors--)
	{
		if (!pSectorBuff)	// Only spare area
		{
			// If we are asked just to write the SectorInfo, we will do that separately
			NF_CMD(CMD_READ2);					 		// Send read command.

			NF_CMD(CMD_WRITE);							// Send write command.
			NF_ADDR(0);									// Column = 0.
			NF_ADDR((nPageAddr)&0xff);			// Page address.
			NF_ADDR((nPageAddr>>8)&0xff);
			if (NEED_EXT_ADDR)
				NF_ADDR((nPageAddr>>16)&0xff);

			// Write the SectorInfo data to the media.
			// Spare area[7:0]
			WrPageInfo((PBYTE)pSectorInfoBuff);

			NF_CLEAR_RB();
			NF_CMD(CMD_WRITE2);				// Send write confirm command.
			NF_SB_DETECT_RB();

			NF_CMD(CMD_STATUS);
			Status = NF_RDDATA_BYTE();					// Read command status.

			if (Status & STATUS_ERROR)
			{
				NF_nFCE_H();							// Deselect the flash chip.
				SetKMode (bLastMode);
				return(FALSE);
			}

			pSectorInfoBuff++;
		}
		else 		// Main area+Spare area.
		{
			NF_CMD(CMD_READ);					 		// Send read command.

			NF_CMD(CMD_WRITE);							// Send write command.

			NF_ADDR(0);									// Column = 0.
			NF_ADDR((nPageAddr)&0xff);			// Page address.
			NF_ADDR((nPageAddr>>8)&0xff);
			if (NEED_EXT_ADDR)
				NF_ADDR((nPageAddr>>16)&0xff);

			//  Special case to handle un-aligned buffer pointer.
			NF_RSTECC();
			NF_MECC_UnLock();

			if( ((DWORD) pSectorBuff) & 0x3)
			{
				WrPage512Unalign (pSectorBuff);
			}
			else
			{
				WrPage512(pSectorBuff);					// Write page/sector data.
			}

			NF_MECC_Lock();

			// Write the SectorInfo data to the media.
			// Spare area[7:0]
			if(pSectorInfoBuff)
			{
				WrPageInfo((PBYTE)pSectorInfoBuff);
				pSectorInfoBuff++;
			}
			else	// Make sure we advance the Flash's write pointer (even though we aren't writing the SectorInfo data)
			{
				BYTE TempInfo[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
				WrPageInfo(TempInfo);
			}

			// Write the SectorInfo data to the media.
			// Spare area[11:8]
			// Get the ECC data from status register.
			MECC = NF_RDMECC0();
			// Now, Write the ECC data to Spare area[11:8]
			NF_WRDATA_BYTE((UCHAR)((MECC	  ) & 0xff));		// Spare area offset 8
			NF_WRDATA_BYTE((UCHAR)((MECC >>  8) & 0xff));	// Spare area offset 9
			NF_WRDATA_BYTE((UCHAR)((MECC >> 16) & 0xff));	// Spare area offset 10
			NF_WRDATA_BYTE((UCHAR)((MECC >> 24) & 0xff));	// Spare area offset 11

			NF_CLEAR_RB();
			NF_CMD(CMD_WRITE2);							// Send write confirm command.
			NF_SB_DETECT_RB();

			do
			{
				NF_CMD(CMD_STATUS);
				Status = NF_RDDATA_BYTE();					// Read command status.
			}while(!(Status & STATUS_READY));

			if (Status & STATUS_ERROR)
			{
				NF_nFCE_H();							// Deselect the flash chip.
				SetKMode (bLastMode);
				return(FALSE);
			}

			pSectorBuff += NAND_SECTOR_SIZE;
		}

		++nPageAddr;
	}

	NF_nFCE_H();										// Deselect the flash chip.

	SetKMode (bLastMode);

	return(TRUE);
}


BOOL FMD_LB_EraseBlock(BLOCK_ID blockID, int mode)
{
	BOOL  bRet = TRUE;
	DWORD dwPageID = blockID << LB_NAND_LOG_2_PAGES_PER_BLOCK;
	BOOL  bLastMode = SetKMode(TRUE);

#if (NAND_DEBUG)
	RETAILMSG(1, (TEXT("FMD_LB_EraseBlock 0x%x \r\n"), blockID));
#endif

	//  Enable the chip
	if(blockID>8192)
	{
		dwPageID = (blockID-8192)<< LB_NAND_LOG_2_PAGES_PER_BLOCK;
		RETAILMSG(1, (TEXT("FMD_LB_EraseBlock 0x%x \r\n"), blockID));
		NF_nFCE1_L();
	}
	else
	{
		NF_nFCE_L();
	}

	NF_CLEAR_RB();

    //  Issue command
	NF_CMD(CMD_ERASE);

    //  Set up address
	NF_ADDR((dwPageID) & 0xff);
	NF_ADDR((dwPageID >> 8) & 0xff);
	if (NEED_EXT_ADDR)
		NF_ADDR((dwPageID >> 16) & 0xff);

	//  Complete erase operation
	NF_CMD(CMD_ERASE2);

	//  Wait for ready bit
	NF_DETECT_RB();	 // Wait tR(max 12us)

	if ( NF_RDSTAT & STATUS_ILLACC )
	{
		RETAILMSG(1, (TEXT("LB######## Error Erasing block (Illigar Access) %d!\n"), blockID));
		s6410NAND->NFSTAT =  STATUS_ILLACC;	// Write 1 to clear.
		bRet = FALSE;
	}
	else
	{
		//  Check the status
		NF_CMD(CMD_STATUS);

		if( NF_RDDATA_BYTE() & STATUS_ERROR)
		{
			RETAILMSG(1, (TEXT("LB######## Error Erasing block %d!\n"), blockID));
			bRet = FALSE;
		}
	}

	/*+add for next device support dillon 0824*/
	if(blockID>8192)
	{
		NF_nFCE1_H();
	}else
		NF_nFCE_H();		
	/*-add for next device support dillon 0824*/

	SetKMode(bLastMode);

	return bRet;
}

BOOL FMD_SB_EraseBlock(BLOCK_ID blockID, int mode)
{
	BOOL   bRet = TRUE;
	DWORD  dwPageID = blockID << SB_NAND_LOG_2_PAGES_PER_BLOCK;


	BOOL bLastMode = SetKMode(TRUE);

	//  Enable the chip
	NF_nFCE_L();						// Select the flash chip.

	//  Issue command
	NF_CMD(CMD_ERASE);

	//  Set up address
	NF_ADDR((dwPageID) & 0xff);
	NF_ADDR((dwPageID >> 8) & 0xff);
	if (NEED_EXT_ADDR)
		NF_ADDR((dwPageID >> 16) & 0xff);

	NF_CLEAR_RB();
	//  Complete erase operation
	NF_CMD(CMD_ERASE2);

	//  Wait for ready bit
	NF_SB_DETECT_RB();	 // Wait tR(max 12us)

	if ( NF_RDSTAT & STATUS_ILLACC )
	{
		RETAILMSG(1, (TEXT("SB######## Error Erasing block (Illigar Access) %d!\n"), blockID));
		s6410NAND->NFSTAT =  STATUS_ILLACC;	// Write 1 to clear.
		bRet = FALSE;
	}
	else
	{
		//  Check the status
		NF_CMD(CMD_STATUS);

		if( NF_RDDATA_BYTE() & STATUS_ERROR)
		{
			RETAILMSG(1, (TEXT("SB######## Error Erasing block %d!\n"), blockID));
			bRet = FALSE;
		}
	}

	NF_nFCE_H();						// Select the flash chip.

	SetKMode(bLastMode);

	return bRet;
}

DWORD FMD_LB_GetBlockStatus(BLOCK_ID blockID, int mode)
{
	SECTOR_ADDR nPageAddr = blockID << LB_NAND_LOG_2_PAGES_PER_BLOCK;
	SectorInfo SI;
	DWORD dwResult = 0;

	BOOL bLastMode = SetKMode(TRUE);

	if(!FMD_LB_ReadSector(nPageAddr, NULL, &SI, 1, mode))
	{
	    SetKMode(bLastMode);
		return BLOCK_STATUS_BAD;
	}

	if(!(SI.bOEMReserved & OEM_BLOCK_READONLY))
	{
		dwResult |= BLOCK_STATUS_READONLY;
	}

#if	MAGNETO
	if( nPageAddr < pBSPArgs->nfsblk )
	{
		dwResult |= BLOCK_STATUS_READONLY;
	}
#endif

	if (!(SI.bOEMReserved & OEM_BLOCK_RESERVED))
	{
		dwResult |= BLOCK_STATUS_RESERVED;
	}


	if(SI.bBadBlock != 0xFF)
	{
		dwResult |= BLOCK_STATUS_BAD;
	}

	SetKMode(bLastMode);

	return dwResult;
}

DWORD FMD_SB_GetBlockStatus(BLOCK_ID blockID, int mode)
{
	SECTOR_ADDR nPageAddr = blockID << SB_NAND_LOG_2_PAGES_PER_BLOCK;
	SectorInfo SI;
	DWORD dwResult = 0;

	BOOL bLastMode = SetKMode(TRUE);

	if(!FMD_SB_ReadSector(nPageAddr, NULL, &SI, 1, mode))
	{
	    SetKMode(bLastMode);
		return BLOCK_STATUS_BAD;
	}

	if(!(SI.bOEMReserved & OEM_BLOCK_READONLY))
	{
		dwResult |= BLOCK_STATUS_READONLY;
	}

#if	MAGNETO
	if( blockID < pBSPArgs->nfsblk )
	{
//		RETAILMSG(1,(TEXT("pBSPArgs->nfsblk=%d\r\n"),pBSPArgs->nfsblk));
		dwResult |= BLOCK_STATUS_READONLY;
	}
#endif

	if (!(SI.bOEMReserved & OEM_BLOCK_RESERVED))
	{
		dwResult |= BLOCK_STATUS_RESERVED;
	}

	if(SI.bBadBlock != 0xFF)
	{
		dwResult |= BLOCK_STATUS_BAD;
	}

	SetKMode(bLastMode);

	return dwResult;
}

BOOL FMD_LB_SetBlockStatus(BLOCK_ID blockID, DWORD dwStatus, int mode)
{
	BYTE bStatus = 0;

	if(dwStatus & BLOCK_STATUS_BAD)
	{
		if(!LB_MarkBlockBad (blockID, mode))
		{
			return FALSE;
		}
	}

	// We don't currently support setting a block to read-only, so fail if request is
	// for read-only and block is not currently read-only.
	if(dwStatus & BLOCK_STATUS_READONLY)
	{
		if(!(FMD_LB_GetBlockStatus(blockID, mode) & BLOCK_STATUS_READONLY))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL FMD_SB_SetBlockStatus(BLOCK_ID blockID, DWORD dwStatus, int mode)
{
	SECTOR_ADDR nPageAddr = blockID << SB_NAND_LOG_2_PAGES_PER_BLOCK;
	BYTE bStatus = 0;

	if(dwStatus & BLOCK_STATUS_BAD)
	{
		if(!SB_MarkBlockBad (blockID, mode))
		{
			return FALSE;
		}
	}

	// We don't currently support setting a block to read-only, so fail if request is
	// for read-only and block is not currently read-only.
	if(dwStatus & BLOCK_STATUS_READONLY)
	{
		if(!(FMD_SB_GetBlockStatus(blockID, mode) & BLOCK_STATUS_READONLY))
		{
			return FALSE;
		}
	}

	return TRUE;
}



#if MAGNETO
BOOL FMD_LB_GetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, PBYTE pOEMReserved, int mode)
{
	UINT32 nSpareAddr = NAND_SECTOR_SIZE;
	UINT32 nPageAddr = physicalSectorAddr;

	RETAILMSG(1, (TEXT("FMD_GetOEMReservedByte 0x%x \n"), nPageAddr));

    BOOL bLastMode = SetKMode(TRUE);

    //  Enable chip select
	/*+add for next device support dillon 0824*/
	if(physicalSectorAddr>8192*64)
	{
		nPageAddr = physicalSectorAddr-8192*64;
		NF_nFCE1_L();
	}
	else
	{
		NF_nFCE_L();
	}
	/*-add for next device support dillon 0824*/
	NF_CLEAR_RB();

    //  Issue command
    NF_CMD(CMD_READ);

    //  Set up address
    NF_ADDR((nSpareAddr+POS_OEMRESERVED)&0xff);
    NF_ADDR(((nSpareAddr+POS_OEMRESERVED)>>8)&0xff);
    NF_ADDR((nPageAddr)&0xff);
    NF_ADDR((nPageAddr>>8)&0xff);
    if (NEED_EXT_ADDR)
        NF_ADDR((nPageAddr>>16)&0xff);

	NF_CMD(CMD_READ3);

    //  Wait for the ready bit
	NF_DETECT_RB();	 // Wait tR(max 12us)

    //  Read the data
    *pOEMReserved = (BYTE) NF_RDDATA_BYTE();		// read and discard

    //  Disable chip select
    
	/*+add for next device support dillon 0824*/
	if(physicalSectorAddr>8192*64)
	{
		NF_nFCE1_H();
	}else
		NF_nFCE_H();		
	/*-add for next device support dillon 0824*/
    

    SetKMode(bLastMode);

	return TRUE;
}


BOOL FMD_SB_GetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, PBYTE pOEMReserved, int mode)
{
    BOOL bLastMode = SetKMode(TRUE);

    RETAILMSG(1, (TEXT("FMD_SB_GetOEMReservedByte, %x\r\n"), physicalSectorAddr));

    //  Enable chip select
    NF_nFCE_L();
	NF_CLEAR_RB();

    //  Issue command
    NF_CMD(CMD_READ2);

    //  Set up address
    NF_ADDR(POS_OEMRESERVED);
    NF_ADDR((physicalSectorAddr)&0xff);
    NF_ADDR((physicalSectorAddr>>8)&0xff);
    if (NEED_EXT_ADDR)
        NF_ADDR((physicalSectorAddr>>16)&0xff);

    //  Wait for the ready bit
	NF_SB_DETECT_RB();	 // Wait tR(max 12us)

    //  Read the data
    *pOEMReserved = (BYTE) NF_RDDATA_BYTE();

    //  Disable chip select
    NF_nFCE_H();

    SetKMode(bLastMode);

	return TRUE;
}



//  FMD_SetOEMReservedByte
//
//  Sets the OEM reserved byte (for metadata) for the specified physical sector.
//
BOOL FMD_LB_SetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, BYTE bOEMReserved, int mode)
{
    BOOL   bRet = TRUE;
	UINT32 nSpareAddr = NAND_SECTOR_SIZE;
	UINT32 nPageAddr = physicalSectorAddr;

	RETAILMSG(1, (TEXT("FMD_SetOEMReservedByte 0x%x \n"), nPageAddr));

    BOOL bLastMode = SetKMode(TRUE);

    //  Enable chip select
	/*+add for next device support dillon 0824*/
	if(physicalSectorAddr>8192*64)
	{
		nPageAddr = physicalSectorAddr-8192*64;
		NF_nFCE1_L();
	}
	else
	{
		NF_nFCE_L();
	}
	/*-add for next device support dillon 0824*/
	NF_CLEAR_RB();

    //  Issue command
    NF_CMD(CMD_WRITE);

    //  Set up address
    NF_ADDR((nSpareAddr+POS_OEMRESERVED)&0xff);
    NF_ADDR(((nSpareAddr+POS_OEMRESERVED)>>8)&0xff);
    NF_ADDR((nPageAddr)&0xff);
    NF_ADDR((nPageAddr>>8)&0xff);
    if (NEED_EXT_ADDR)
        NF_ADDR((nPageAddr>>16)&0xff);

    //  Write the data
	bOEMReserved = NF_RDDATA_BYTE() ;

    //  Complete the write
    NF_CMD(CMD_WRITE2);

    //  Wait for the ready bit
	NF_DETECT_RB();	 // Wait tR(max 12us)

	if ( NF_RDSTAT & STATUS_ILLACC )
	{
		RETAILMSG(1, (TEXT("FMD_LB_SetOEMReservedByte() ######## Error Programming page (Illigar Access) %d!\n"), nPageAddr));
		s6410NAND->NFSTAT =  STATUS_ILLACC;	// Write 1 to clear.
       	bRet = FALSE;
	}
	else
	{
		//  Check the status of program
		NF_CMD(CMD_STATUS);

		if( NF_RDDATA_BYTE() & STATUS_ERROR) {
			RETAILMSG(1, (TEXT("FMD_LB_SetOEMReservedByte() ######## Error Programming page %d!\n"), nPageAddr));
			bRet = FALSE;
		}
	}

    //  Disable chip select
	/*+add for next device support dillon 0824*/
	if(physicalSectorAddr>8192*64)
	{
		NF_nFCE1_H();
	}else
		NF_nFCE_H();		
	/*-add for next device support dillon 0824*/

    SetKMode(bLastMode);

    return bRet;
}


BOOL FMD_SB_SetOEMReservedByte(SECTOR_ADDR physicalSectorAddr, BYTE bOEMReserved, int mode)
{
    BOOL bRet = TRUE;

    BOOL bLastMode = SetKMode(TRUE);

    RETAILMSG(1, (TEXT("FMD_SB_SetOEMReservedByte, %x\r\n"), physicalSectorAddr));
    //  Enable chip select
    NF_nFCE_L();
	NF_CLEAR_RB();

    //  Issue command
    NF_CMD(CMD_READ2);
    NF_CMD(CMD_WRITE);

    //  Set up address
    NF_ADDR(POS_OEMRESERVED);
    NF_ADDR((physicalSectorAddr)&0xff);
    NF_ADDR((physicalSectorAddr>>8)&0xff);
    if (NEED_EXT_ADDR)
        NF_ADDR((physicalSectorAddr>>16)&0xff);

    //  Write the data
    bOEMReserved = NF_RDDATA_BYTE();

    //  Complete the write
    NF_CMD(CMD_WRITE2);

    //  Wait for the ready bit
	NF_SB_DETECT_RB();	 // Wait tR(max 12us)

	if ( NF_RDSTAT & STATUS_ILLACC )
	{
		RETAILMSG(1, (TEXT("FMD_SB_SetOEMReservedByte() ######## Error Programming page (Illigar Access) %d!\n"), physicalSectorAddr));
		s6410NAND->NFSTAT =  STATUS_ILLACC;	// Write 1 to clear.
       	bRet = FALSE;
	}
	else
	{
		//  Check the status of program
		NF_CMD(CMD_STATUS);

		if( NF_RDDATA_BYTE() & STATUS_ERROR) {
			RETAILMSG(1, (TEXT("FMD_SB_SetOEMReservedByte() ######## Error Programming page %d!\n"), physicalSectorAddr));
			bRet = FALSE;
		}
	}

    //  Disable chip select
    NF_nFCE_H();

    SetKMode(bLastMode);

    return bRet;
}
#endif // MAGNETO


BOOL LB_MarkBlockBad(BLOCK_ID blockID, int mode)
{
	DWORD  dwStartPage = blockID << LB_NAND_LOG_2_PAGES_PER_BLOCK;
	BOOL   bRet = TRUE;

	BOOL bLastMode = SetKMode(TRUE);

	//  Enable chip
	if(blockID>8192)
	{
		dwStartPage = (blockID-8192)<< LB_NAND_LOG_2_PAGES_PER_BLOCK;
		NF_nFCE1_L();
	}
	else
	{
		NF_nFCE_L();
	}
	NF_CLEAR_RB();

	//  Issue command
	//  We are dealing with spare area
	NF_CMD(CMD_WRITE);

	//  Set up address
	NF_ADDR((NAND_SECTOR_SIZE+POS_BADBLOCK)&0xff);
	NF_ADDR(((NAND_SECTOR_SIZE+POS_BADBLOCK)>>8)&0xff);
	NF_ADDR((dwStartPage) & 0xff);
	NF_ADDR((dwStartPage >> 8) & 0xff);
	if (NEED_EXT_ADDR)
		NF_ADDR((dwStartPage >> 16) & 0xff);

	NF_WRDATA_BYTE(BADBLOCKMARK);

	//  Copmlete the write
	NF_CMD(CMD_WRITE2);

	//  Wait for RB
	NF_DETECT_RB();	 // Wait tR(max 12us)

	if ( NF_RDSTAT & STATUS_ILLACC )
	{
        RETAILMSG(1, (TEXT("LB_MarkBlockBad() ######## Error Programming page (Illegal Access) %d!\n")));
		s6410NAND->NFSTAT =  STATUS_ILLACC;	// Write 1 to clear.
		bRet = FALSE;
	}
	else
	{
		//  Check the status of program
		NF_CMD(CMD_STATUS);

		if( NF_RDDATA_BYTE() & STATUS_ERROR)
		{
			RETAILMSG(1, (TEXT("LB_MarkBlockBad() ######## Error Programming page %d!\n")));
			bRet = FALSE;
		}
	}

	//  Disable chip select
	/*+add for next device support dillon 0824*/
	if(blockID>8192)
	{
		NF_nFCE1_H();
	}else
		NF_nFCE_H();		
	/*-add for next device support dillon 0824*/

    SetKMode(bLastMode);

	return bRet;
}

BOOL SB_MarkBlockBad(BLOCK_ID blockID, int mode)
{
	DWORD  dwStartPage = blockID << SB_NAND_LOG_2_PAGES_PER_BLOCK;
	BOOL   bRet = TRUE;

	BOOL bLastMode = SetKMode(TRUE);

	//  Enable chip
	NF_nFCE_L();
	NF_CLEAR_RB();

	//  Issue command
	//  We are dealing with spare area
	NF_CMD(CMD_READ2);
	NF_CMD(CMD_WRITE);

	//  Set up address
	NF_ADDR(POS_BADBLOCK);
	NF_ADDR((dwStartPage) & 0xff);
	NF_ADDR((dwStartPage >> 8) & 0xff);
	if (NEED_EXT_ADDR)
		NF_ADDR((dwStartPage >> 16) & 0xff);

	NF_WRDATA_BYTE(BADBLOCKMARK);

	//  Copmlete the write
	NF_CMD(CMD_WRITE2);

	//  Wait for RB
	NF_SB_DETECT_RB();	 // Wait tR(max 12us)

	if ( NF_RDSTAT & STATUS_ILLACC )
	{
		RETAILMSG(1, (TEXT("LB_MarkBlockBad() ######## Error Programming page (Illigar Access) %d!\n")));
		s6410NAND->NFSTAT =  STATUS_ILLACC;	// Write 1 to clear.
		bRet = FALSE;
	}
	else
	{
		//  Check the status of program
		NF_CMD(CMD_STATUS);

		if( NF_RDDATA_BYTE() & STATUS_ERROR)
		{
			RETAILMSG(1, (TEXT("LB_MarkBlockBad() ######## Error Programming page %d!\n")));
			bRet = FALSE;
		}
	}

    //  Disable chip select
    NF_nFCE_H();

    SetKMode(bLastMode);

    return bRet;
}

BOOL LB_IsBlockBad(BLOCK_ID blockID, int mode)
{
	DWORD  dwPageID = blockID << LB_NAND_LOG_2_PAGES_PER_BLOCK;
	BOOL   bRet = FALSE;
	BYTE   wFlag;

#ifdef IROMBOOT
    if (blockID == 0)
    {
        RETAILMSG(1, (TEXT("\n\n\nFMDLB: IsBlockBad - Page #: 0x%x  this block is for IROM BOOT !!!\r\n\n\n\n"), dwPageID));
        return TRUE;
    }
#endif

    BOOL bLastMode = SetKMode(TRUE);

	//  Enable the chip
	if(blockID>8192)
	{
		dwPageID = (blockID-8192)<< LB_NAND_LOG_2_PAGES_PER_BLOCK;
		NF_nFCE1_L();
	}
	else
	{
		NF_nFCE_L();
	}
	NF_CLEAR_RB();

	//  Issue the command
	NF_CMD(CMD_READ);

	//  Set up address
	NF_ADDR((NAND_SECTOR_SIZE+POS_BADBLOCK)&0xff);
	NF_ADDR(((NAND_SECTOR_SIZE+POS_BADBLOCK)>>8)&0xff);
	NF_ADDR((dwPageID) & 0xff);
	NF_ADDR((dwPageID >> 8) & 0xff);
    if (NEED_EXT_ADDR)
        NF_ADDR((dwPageID >> 16) & 0xff);

	NF_CMD(CMD_READ3);

	//  Wait for Ready bit
	NF_DETECT_RB();	 // Wait tR(max 12us)

	//  Now get the byte we want
	wFlag = (BYTE)(NF_RDDATA_BYTE()&0xff);

	if(wFlag != 0xff)
	{
		RETAILMSG(1, (TEXT("FMDLB: IsBlockBad - Page #: 0x%x \r\n"), dwPageID));
		bRet = TRUE;
	}

	//  Disable the chip
	/*+add for next device support dillon 0824*/
	if(blockID>8192)
	{
		NF_nFCE1_H();
	}else
		NF_nFCE_H();		
	/*-add for next device support dillon 0824*/

    SetKMode(bLastMode);

	return bRet;
}

BOOL SB_IsBlockBad(BLOCK_ID blockID, int mode)
{
	DWORD  dwPageID = blockID << SB_NAND_LOG_2_PAGES_PER_BLOCK;
	BOOL   bRet = FALSE;
	BYTE   wFlag;

    BOOL bLastMode = SetKMode(TRUE);

	//  Enable the chip
	NF_nFCE_L();
	NF_CLEAR_RB();
	//  Issue the command
	NF_CMD(CMD_READ2);

	//  Set up address
	NF_ADDR(POS_BADBLOCK);
	NF_ADDR((dwPageID) & 0xff);
	NF_ADDR((dwPageID >> 8) & 0xff);
    if (NEED_EXT_ADDR)
        NF_ADDR((dwPageID >> 16) & 0xff);

	//  Wait for Ready bit
	NF_SB_DETECT_RB();	 // Wait tR(max 12us)

	//  Now get the byte we want
	wFlag = (BYTE) NF_RDDATA_BYTE();

    if(wFlag != 0xff) {
		RETAILMSG(1, (TEXT("FMDSB: IsBlockBad - Page #: 0x%x \r\n"), dwPageID));
		bRet = TRUE;
	}

	//  Disable the chip
	NF_nFCE_H();

    SetKMode(bLastMode);

    return bRet;
}

#if MAGNETO
static BOOL DefineLayout()
{
    PFlashRegion pRegion = NULL;
    DWORD dwBlock = 0;

    if (!FMD_GetInfo (&g_flashInfo))
        return FALSE;

    // Find the MBR to determine if there is a flash layout sector
    g_dwNumRegions = 0;


    // Find the first usuable block
    while (dwBlock < g_flashInfo.dwNumBlocks) {
        if (!(FMD_GetBlockStatus(dwBlock) & (BLOCK_STATUS_BAD | BLOCK_STATUS_RESERVED))) {
            break;
        }
        dwBlock++;
    }

	RETAILMSG(1, (TEXT("DefineLayout: dwBlock = 0x%x \r\n"), dwBlock));

	// Find the first usuable sector
    DWORD dwSector = dwBlock * g_flashInfo.wSectorsPerBlock;
	RETAILMSG(1, (TEXT("DefineLayout: dwSector = 0x%x \r\n"), dwSector));
    if (!FMD_ReadSector (dwSector, g_pFLSBuffer, NULL, 1)) {
        return FALSE;
    }

    // compare the signatures
    if (IS_VALID_BOOTSEC(g_pFLSBuffer))
    {
        if (!FMD_ReadSector (dwSector+1, g_pFLSBuffer, NULL, 1)) {
            return FALSE;
        }
        if (IS_VALID_FLS(g_pFLSBuffer))
        {
            PFlashLayoutSector pFLS = (PFlashLayoutSector)(g_pFLSBuffer);

            // Cache the flash layout sector information
            g_dwNumRegions = pFLS->cbRegionEntries / sizeof(FlashRegion);
			RETAILMSG(1, (TEXT("DefineLayout: g_dwNumRegions = 0x%x \r\n"), g_dwNumRegions));

            // FlashRegion table starts after the ReservedEntry table.
            if (g_dwNumRegions)
            {
                pRegion = (PFlashRegion)((LPBYTE)pFLS + sizeof(FlashLayoutSector) + pFLS->cbReservedEntries);
			RETAILMSG(1, (TEXT("DefineLayout: sizeof(FlashLayoutSector) = %x cdReservedEntries = %x  \r\n"),
				sizeof(FlashLayoutSector),pFLS->cbReservedEntries));
            }
        }
    }

    if (!g_dwNumRegions)
    {
        g_dwNumRegions = 1;
    }

    if (g_dwNumRegions > MAX_REGIONS)
        return FALSE;


    if (pRegion)
    {
        memcpy (g_pRegionTable, pRegion, g_dwNumRegions * sizeof(FlashRegion));
    }
    else

    {
        g_pRegionTable[0].dwStartPhysBlock = 0;
        g_pRegionTable[0].dwNumPhysBlocks = g_flashInfo.dwNumBlocks;
        g_pRegionTable[0].dwNumLogicalBlocks = FIELD_NOT_IN_USE;
        g_pRegionTable[0].dwBytesPerBlock = g_flashInfo.dwBytesPerBlock;
        g_pRegionTable[0].regionType = FILESYS;
        g_pRegionTable[0].dwSectorsPerBlock = g_flashInfo.wSectorsPerBlock;
        g_pRegionTable[0].dwCompactBlocks = DEFAULT_COMPACTION_BLOCKS;
    }

	RETAILMSG(1, (TEXT("DefineLayout: g_pRegionTable[0].dwNumPhysBlocks = 0x%x\r\ndwBytesperBlock = %x \r\ndwSectorsPerBlock = %x\r\n")
						,g_pRegionTable[0].dwNumPhysBlocks,g_pRegionTable[0].dwBytesPerBlock
						,g_pRegionTable[0].dwSectorsPerBlock));
	RETAILMSG(1, (TEXT("DefineLayout: g_flashInfo.dwNumPhysBlocks = 0x%x\r\ndwBytesperBlock = %x \r\ndwSectorsPerBlock = %x\r\n")
						,g_flashInfo.dwNumBlocks,g_flashInfo.dwBytesPerBlock
						,g_flashInfo.wSectorsPerBlock));

    return TRUE;
}

#ifdef IROMBOOT
BOOL FMD_WriteSector_Stepldr(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors)
{
    BOOL bRet = TRUE;

	RETAILMSG(0, (TEXT("FMD::FMD_WriteSector_Stepldr 0x%x \r\n"), startSectorAddr));

	if(IS_LB)
		bRet = FMD_LB_WriteSector_Steploader(startSectorAddr, pSectorBuff, pSectorInfoBuff, dwNumSectors, USE_NFCE);
	else
		bRet = FMD_SB_WriteSector_Steploader(startSectorAddr, pSectorBuff, pSectorInfoBuff, dwNumSectors, USE_NFCE);

	return bRet;
}

BOOL FMD_LB_WriteSector_Steploader(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors, int mode)
{
    DWORD i;
    UINT16 nSectorLoop;
    volatile DWORD	wrdata;
    int NewSpareAddr = 0x0;
    int NewDataAddr = 0;
    int NewSectorAddr = startSectorAddr;
    MECC8 t8MECC[4];

	RETAILMSG(0, (TEXT("FMD::FMD_LB_WriteSector_Steploader 0x%x \r\n"), startSectorAddr));

    //Check Parameters.
    if (!pSectorBuff && !pSectorInfoBuff)
    {
        return(FALSE);
    }
    if ( dwNumSectors > 1 )
    {
        RETAILMSG(1, (TEXT("######## FATAL ERROR => FMD::FMD_WriteSector->dwNumsectors is bigger than 1. \r\n")));
        return FALSE;
    }

    BOOL bLastMode = SetKMode(TRUE);	// dummy function.

    s6410NAND->NFCONF = (s6410NAND->NFCONF & ~(1<<30)) | (1<<23) | (NAND_TACLS <<12) | (NAND_TWRPH0 <<8) | (NAND_TWRPH1 <<4);
    s6410NAND->NFCONT |= (1<<18)|(1<<13)|(1<<12)|(1<<11)|(1<<10)|(1<<9);
    s6410NAND->NFSTAT |= ((1<<6)|(1<<5)|(1<<4));

    //  Enable Chip
    NF_nFCE_L();	// Force nFCE to low

    //  Issue command
    NF_CMD(CMD_WRITE);	//0x80

    //  Setup address to write Main data
    NF_ADDR((NewDataAddr)&0xff);	// 2bytes for column address
    NF_ADDR((NewDataAddr>>8)&0xff);
    NF_ADDR((NewSectorAddr)&0xff);	// 3bytes for row address
    NF_ADDR((NewSectorAddr>>8)&0xff);
    NF_ADDR((NewSectorAddr>>16)&0xff);

    // initialize variable.
    for(i = 0; i < 4; i++) {
        t8MECC[i].n8MECC0 = 0x0;
        t8MECC[i].n8MECC1 = 0x0;
        t8MECC[i].n8MECC2 = 0x0;
        t8MECC[i].n8MECC3 = 0x0;
    }

    // Write each Sector in the Page. (4 Sector per Page, Loop 4 times.)
    for (nSectorLoop = 0; nSectorLoop < SECTORS_PER_PAGE; nSectorLoop++)
    {
        //  Initialize ECC register
        NF_MECC_UnLock();
        NF_RSTECC();

        // Special case to handle un-aligned buffer pointer.
        if( ((DWORD) (pSectorBuff+nSectorLoop*SECTOR_SIZE)) & 0x3)
        {
            //  Write the data
            for(i=0; i<SECTOR_SIZE/sizeof(DWORD); i++)
            {
                wrdata = (pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+0];
                wrdata |= (pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+1]<<8;
                wrdata |= (pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+2]<<16;
                wrdata |= (pSectorBuff+nSectorLoop*SECTOR_SIZE)[i*4+3]<<24;
                NF_WRDATA_WORD(wrdata);
            }
        }
        else
        {
            WrPage512(pSectorBuff+nSectorLoop*SECTOR_SIZE);
        }

        NF_MECC_Lock();

        while(!(s6410NAND->NFSTAT&(1<<7))) ;
        s6410NAND->NFSTAT|=(1<<7);

        //  Read out the ECC value generated by HW
        t8MECC[nSectorLoop].n8MECC0 = NF_8MECC0();
        t8MECC[nSectorLoop].n8MECC1 = NF_8MECC1();
        t8MECC[nSectorLoop].n8MECC2 = NF_8MECC2();
        t8MECC[nSectorLoop].n8MECC3 = (NF_8MECC3() & 0xff);

		/*
          //Debug Code :: Print Write Data
        RETAILMSG(1, (TEXT("\r\n===================WRITE DATA=====================")));
        for(i=0;i<512;i++)
        {
            if(i%16 == 0)
                RETAILMSG(1, (TEXT("\r\n")));
            RETAILMSG(1, (TEXT(" 0x%x "),*(pSectorBuff+(nSectorLoop*SECTOR_SIZE) + i)));
        }
        RETAILMSG(1, (TEXT("\r\n===================================================\r\n")));

        RETAILMSG(1, (TEXT("FMD_LB_WriteSector_Steploader() : Sdata : 0x%x, 0x%x, 0x%x, 0x%x\r\n")
                , t8MECC[nSectorLoop].n8MECC0
                , t8MECC[nSectorLoop].n8MECC1
                , t8MECC[nSectorLoop].n8MECC2
                , t8MECC[nSectorLoop].n8MECC3));
        */
    }

    for(nSectorLoop = 0; nSectorLoop < 4; nSectorLoop++)
    {
        NF_WRDATA_WORD(t8MECC[nSectorLoop].n8MECC0); // 4 byte n8MECC0
        NF_WRDATA_WORD(t8MECC[nSectorLoop].n8MECC1); // 4 byte n8MECC1
        NF_WRDATA_WORD(t8MECC[nSectorLoop].n8MECC2); // 4 byte n8MECC2
        NF_WRDATA_BYTE((t8MECC[nSectorLoop].n8MECC3) & 0xff); // 1 byte n8MECC3
    }

    s6410NAND->NFSTAT |=  (1<<4); //NF_CLEAR_RB

    //  Finish up the write operation
    NF_CMD(CMD_WRITE2);	// 0x10

    //  Wait for RB.
    NF_WAITRB();

    NF_CMD(CMD_STATUS);   // Read status command

    for(i=0;i<3;i++);  //twhr=60ns

    if(NF_RDDATA_BYTE() & STATUS_ERROR)
    {
        RETAILMSG(1, (TEXT("FMD_WriteSector() ######## Error Programming page %d!\n"), startSectorAddr));
        //  Disable the chip
        NF_nFCE_H();
        s6410NAND->NFCONF = (0<<23) | (NAND_TACLS <<12) | (NAND_TWRPH0 <<8) | (NAND_TWRPH1 <<4);
        SetKMode(bLastMode);	// Dummy function.
        return FALSE;
    }
    else
    {
        NF_nFCE_H();
        s6410NAND->NFCONF = (0<<23) | (NAND_TACLS <<12) | (NAND_TWRPH0 <<8) | (NAND_TWRPH1 <<4);
        SetKMode(bLastMode);	// Dummy function.
        return TRUE;
    }
}


BOOL FMD_SB_WriteSector_Steploader(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors, int mode)
{
    DWORD i;
    volatile DWORD	wrdata;
    int NewSpareAddr = 0x0;
    int NewDataAddr = 0;
    int NewSectorAddr = startSectorAddr;
    MECC8 t8MECC;

	ULONG blockPage = (((NewSectorAddr / PAGES_PER_BLOCK) * PAGES_PER_BLOCK) | (NewSectorAddr % PAGES_PER_BLOCK));

	RETAILMSG(0, (TEXT("FMD::FMD_SB_WriteSector_Steploader 0x%x \r\n"), startSectorAddr));

    //Check Parameters.
    if ((pSectorBuff == NULL) || (pSectorInfoBuff == NULL))
    {
        return(FALSE);
    }
    if ( dwNumSectors > 1 )
    {
        RETAILMSG(1, (TEXT("######## FATAL ERROR => FMD::FMD_WriteSector->dwNumsectors is bigger than 1. \r\n")));
        return FALSE;
    }

    BOOL bLastMode = SetKMode(TRUE);	// dummy function.

    s6410NAND->NFCONF = (s6410NAND->NFCONF & ~(1<<30)) | (1<<23) | (NAND_TACLS <<12) | (NAND_TWRPH0 <<8) | (NAND_TWRPH1 <<4);
    s6410NAND->NFCONT |= (1<<18)|(1<<13)|(1<<12)|(1<<11)|(1<<10)|(1<<9);
    s6410NAND->NFSTAT |= ((1<<6)|(1<<5)|(1<<4));

    //  Enable Chip
    NF_nFCE_L();// Force nFCE to low

//sun
	NF_CMD(CMD_READ);					 		// Send read command.

    //  Issue command
    NF_CMD(CMD_WRITE);	//0x80

    //  Setup address to write Main data
    NF_ADDR(0x0);	// 1bytes for column address
//sun
    NF_ADDR((blockPage)&0xff);	// 3bytes for row address
    NF_ADDR((blockPage>>8)&0xff);
    NF_ADDR((blockPage>>16)&0xff);
    //NF_ADDR((NewSectorAddr)&0xff);	// 3bytes for row address
    //NF_ADDR((NewSectorAddr>>8)&0xff);
    //NF_ADDR((NewSectorAddr>>16)&0xff);

    // initialize variable.

    t8MECC.n8MECC0 = 0x0;
    t8MECC.n8MECC1 = 0x0;
    t8MECC.n8MECC2 = 0x0;
    t8MECC.n8MECC3 = 0x0;


    // Write each Sector in the Page.
    //  Initialize ECC register
    NF_MECC_UnLock();
    NF_RSTECC();

    // Special case to handle un-aligned buffer pointer.
    if( ((DWORD) pSectorBuff) & 0x3)
    {
        //  Write the data
        for(i=0; i<SECTOR_SIZE/sizeof(DWORD); i++)
        {
            wrdata = (pSectorBuff)[i*4+0];
            wrdata |= (pSectorBuff)[i*4+1]<<8;
            wrdata |= (pSectorBuff)[i*4+2]<<16;
            wrdata |= (pSectorBuff)[i*4+3]<<24;
            NF_WRDATA_WORD(wrdata);
        }
    }
    else
    {
        WrPage512(pSectorBuff);
    }

    NF_MECC_Lock();

    while(!(s6410NAND->NFSTAT&(1<<7))) ;
    s6410NAND->NFSTAT|=(1<<7);

    //  Read out the ECC value generated by HW
    t8MECC.n8MECC0 = NF_8MECC0();
    t8MECC.n8MECC1 = NF_8MECC1();
    t8MECC.n8MECC2 = NF_8MECC2();
    t8MECC.n8MECC3 = (NF_8MECC3() & 0xff);

        //Debug Code :: Print Write Data
        /*
        RETAILMSG(, (TEXT("\r\n===================WRITE DATA=====================")));
        for(i=0;i<512;i++)
        {
            if(i%16 == 0)
                RETAILMSG(1, (TEXT("\r\n")));
            RETAILMSG(1, (TEXT(" 0x%x "),*(pSectorBuff + i)));
        }
        RETAILMSG(1, (TEXT("\r\n===================================================\r\n")));

        RETAILMSG(1, (TEXT("FMD_SB_WriteSector_Steploader() : Sdata : 0x%x, 0x%x, 0x%x, 0x%x\r\n")
                , t8MECC.n8MECC0
                , t8MECC.n8MECC1
                , t8MECC.n8MECC2
                , t8MECC.n8MECC3));
		*/
    NF_WRDATA_WORD(t8MECC.n8MECC0); // 4 byte n8MECC0
    NF_WRDATA_WORD(t8MECC.n8MECC1); // 4 byte n8MECC1
    NF_WRDATA_WORD(t8MECC.n8MECC2); // 4 byte n8MECC2
    NF_WRDATA_BYTE((t8MECC.n8MECC3) & 0xff); // 1 byte n8MECC3

    s6410NAND->NFSTAT |=  (1<<4); //NF_CLEAR_RB

    //  Finish up the write operation
    NF_CMD(CMD_WRITE2);	// 0x10

    //  Wait for RB.
    NF_SB_DETECT_RB();	 // Wait tR(max 12us)

    NF_CMD(CMD_STATUS);   // Read status command

    for(i=0;i<3;i++);  //twhr=60ns

    if(NF_RDDATA_BYTE() & STATUS_ERROR)
    {
        RETAILMSG(1, (TEXT("FMD_WriteSector() ######## Error Programming page %d!\n"), startSectorAddr));
        //  Disable the chip
        NF_nFCE_H();
        s6410NAND->NFCONF = (0<<23) | (NAND_TACLS <<12) | (NAND_TWRPH0 <<8) | (NAND_TWRPH1 <<4);
        SetKMode(bLastMode);	// Dummy function.
        return FALSE;
    }
    else
    {
        NF_nFCE_H();
        s6410NAND->NFCONF = (0<<23) | (NAND_TACLS <<12) | (NAND_TWRPH0 <<8) | (NAND_TWRPH1 <<4);
        SetKMode(bLastMode);	// Dummy function.
        return TRUE;
    }
}
#endif  // !IROMBOOT

#endif // MAGNETO
