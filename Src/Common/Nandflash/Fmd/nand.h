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
#ifndef __NAND_H__
#define __NAND_H__

#include <fmd.h>

#define MAGNETO 1

typedef struct
{
	UINT16 nNumOfBlks;
	UINT16 nPagesPerBlk;
	UINT16 nSctsPerPage;
} NANDDeviceInfo;
#ifdef __cplusplus
extern "C"  {
#endif
NANDDeviceInfo GetNandInfo(void);
#ifdef IROMBOOT
BOOL FMD_WriteSector_Stepldr(SECTOR_ADDR startSectorAddr, LPBYTE pSectorBuff, PSectorInfo pSectorInfoBuff, DWORD dwNumSectors);
#endif
#ifdef __cplusplus
}
#endif

#define NAND_LB_PAGE_SIZE       (2048)  // Each Page has 512 Bytes
#define SPARE_DATA_ECC_SIZE     (6)     // bad block + reserved1 + OEMreserved
#define SPARE_MECC_ECC_SIZE     (16)    // MECC(4B) x 4

#define ALL_FF                  (0xffffffff)

#define NUM_OF_BLOCKS           (stDeviceInfo.nNumOfBlks)
#define PAGES_PER_BLOCK         (stDeviceInfo.nPagesPerBlk)
#define SECTORS_PER_PAGE        (stDeviceInfo.nSctsPerPage)

#undef SECTOR_SIZE
#define SECTOR_SIZE             (512)
#define NAND_SECTOR_SIZE        (SECTOR_SIZE*SECTORS_PER_PAGE)

#define IS_LB                   (SECTORS_PER_PAGE == 4 || SECTORS_PER_PAGE == 8)

#define USE_NFCE                0
#define USE_GPIO                0

#define CMD_READID              (0x90)  //  ReadID
#define CMD_READ                (0x00)  //  Read
#define CMD_READ2               (0x50)  //  Read2
#define CMD_READ3               (0x30)  //  Read3
#define CMD_RESET               (0xff)  //  Reset
#define CMD_ERASE               (0x60)  //  Erase phase 1
#define CMD_ERASE2              (0xd0)  //  Erase phase 2
#define CMD_WRITE               (0x80)  //  Write phase 1
#define CMD_WRITE2              (0x10)  //  Write phase 2
#define CMD_STATUS              (0x70)  //  STATUS
#define CMD_RDI                 (0x85)  //  Random Data Input
#define CMD_RDO                 (0x05)  //  Random Data Output
#define CMD_RDO2                (0xE0)  //  Random Data Output

#define BADBLOCKMARK            (0x00)

//  Status bit pattern
#define STATUS_READY            (0x40)  //  Ready
#define STATUS_ERROR            (0x01)  //  Error
#define	STATUS_ILLACC           (1<<5)  //	Illigar Access


#define NF_CMD(cmd)             {s6410NAND->NFCMD   =  (unsigned char)(cmd);}
#define NF_ADDR(addr)           {s6410NAND->NFADDR  =  (unsigned char)(addr);}

#define NF_nFCE_L()             {s6410NAND->NFCONT &= ~(1<<1);}
#define NF_nFCE_H()             {s6410NAND->NFCONT |=  (1<<1);}
#define NF_nFCE1_L()             {s6410NAND->NFCONT &= ~(1<<2);}
#define NF_nFCE1_H()             {s6410NAND->NFCONT |=  (1<<2);}
#define NF_RDESTST              (s6410NAND->NFECCERR1)

//-----------------------------------------

#define NF_RSTECC()             {s6410NAND->NFCONT |=  ((1<<5) | (1<<4));}

#define NF_MECC_UnLock()        {s6410NAND->NFCONT &= ~(1<<7);}
#define NF_MECC_Lock()          {s6410NAND->NFCONT |= (1<<7);}
#define NF_SECC_UnLock()        {s6410NAND->NFCONT &= ~(1<<6);}
#define NF_SECC_Lock()          {s6410NAND->NFCONT |= (1<<6);}

#define NF_CLEAR_RB()           {s6410NAND->NFSTAT = (1<<4);}	// Have write '1' to clear this bit.

#define NF_SB_DETECT_RB()       {while((s6410NAND->NFSTAT&0x11)!=0x11);} // RnB_Transdetect & RnB
#define NF_SB_WAITRB()          {while(!(s6410NAND->NFSTAT & (1<<0)));}

#define NF_DETECT_RB()          {NF_CMD(CMD_STATUS); \
                                 while(!(NF_RDDATA_BYTE() & STATUS_READY)); \
                                 NF_CMD(CMD_READ); \
                                }

#define NF_WAITRB()             {NF_CMD(CMD_STATUS); \
                                 while(!(NF_RDDATA_BYTE() & STATUS_READY)); \
                                }

#define NF_RDDATA_BYTE()        (s6410NAND->NFDATA)
#define NF_RDDATA_WORD()        (*(volatile UINT32 *)(&(s6410NAND->NFDATA)))

#define NF_WRDATA_BYTE(data)    {s6410NAND->NFDATA  =  (UINT8)(data);}
#define NF_WRDATA_WORD(data)    {*(volatile UINT32 *)(&(s6410NAND->NFDATA))  =  (UINT32)(data);}

#define NF_RDMECC0()            (s6410NAND->NFMECC0)
#define NF_RDMECC1()            (s6410NAND->NFMECC1)
#define NF_RDSECC()             (s6410NAND->NFSECC)

#define NF_RDMECCD0()           (s6410NAND->NFMECCD0)
#define NF_RDMECCD1()           (s6410NAND->NFMECCD1)
#define NF_RDSECCD()            (s6410NAND->NFSECCD)

#define NF_ECC_ERR0             (s6410NAND->NFECCERR0)
#define NF_ECC_ERR1             (s6410NAND->NFECCERR1)

#define NF_WRMECCD0(data)       {s6410NAND->NFMECCD0 = (data);}
#define NF_WRMECCD1(data)       {s6410NAND->NFMECCD1 = (data);}
#define NF_WRSECCD(data)        {s6410NAND->NFSECCD = (data);}

#define NF_8MECC0()             (s6410NAND->NF8MECC0)
#define NF_8MECC1()             (s6410NAND->NF8MECC1)
#define NF_8MECC2()             (s6410NAND->NF8MECC2)
#define NF_8MECC3()             (s6410NAND->NF8MECC3)

#define NF_RDSTAT               (s6410NAND->NFSTAT)

//-----------------------------------------------------------------------------

typedef enum
{
	ECC_CORRECT_MAIN = 0,	// correct Main ECC
	ECC_CORRECT_SPARE = 1	// correct Main ECC
} ECC_CORRECT_TYPE;

//-----------------------------------------------------------------------------

#endif	// __NAND_H_.

