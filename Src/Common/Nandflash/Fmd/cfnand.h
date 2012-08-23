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
#ifndef _S3C6410_DEVBOARD_
#define _S3C6410_DEVBOARD_

#include "FMD_LB.h"
#include "FMD_SB.h"
#include "nand.h"

#define BW_X08                              0
#define BW_X16                              1
#define BW_X32                              2

/*****************************************************************************/
/* S6410 Internal Data Structure Definition                                    */
/*****************************************************************************/
typedef struct
{
	UINT16          nMID;           /* Manufacturer ID               */
	UINT16          nDID;           /* Device ID                     */

	UINT16          nNumOfBlks;     /* Number of Blocks              */
	UINT16          nPgsPerBlk;     /* Number of Pages per block     */
	UINT16          nSctsPerPg;     /* Number of Sectors per page    */
	UINT16          nNumOfPlanes;   /* Number of Planes              */
	UINT16          nBlksInRsv;     /* The Number of Blocks
									   in Reservior for Bad Blocks   */
	UINT8           nBadPos;        /* BadBlock Information Poisition*/
	UINT8           nLsnPos;        /* LSN Position                  */
	UINT8           nECCPos;        /* ECC Policy : HW_ECC, SW_ECC   */
	UINT16          nBWidth;        /* Nand Organization X8 or X16   */

	UINT16          nTrTime;        /* Typical Read Op Time          */
	UINT16          nTwTime;        /* Typical Write Op Time         */
	UINT16          nTeTime;        /* Typical Erase Op Time         */
	UINT16          nTfTime;        /* Typical Transfer Op Time      */
    UINT8           nLargeCycle;    /* Large Cycle (LB : 5, SB : 4) */
} S6410Spec;

static S6410Spec     astNandSpec[] = {
	/*************************************************************************/
	/* nMID, nDID,                                                           */
	/*            nNumOfBlks                                                 */
	/*                  nPgsPerBlk                                           */
	/*                      nSctsPerPg                                       */
	/*                         nNumOfPlanes                                  */
	/*                            nBlksInRsv                                 */
	/*                                nBadPos                                */
	/*                                   nLsnPos                             */
	/*                                      nECCPos                          */
	/*                                         nBWidth                       */
	/*                                                nTrTime                */
	/*                                                    nTwTime            */
	/*                                                         nTeTime       */
	/*                                                                nTfTime*/
    /*                                                                    nLargeCycle */
	/*************************************************************************/
    /* 8Gbit DDP NAND Flash */
    { 0xEC, 0xD3, /*16384*/8192, 64, 4, 2,160, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 1},
    /* 4Gbit DDP NAND Flash */
    { 0xEC, 0xAC, 4096, 64, 4, 2, 80, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 1},
    { 0xEC, 0xDC, 4096, 64, 4, 2, 80, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 1},
    //{ 0xEC, 0xBC, 4096, 64, 4, 2, 80, 0, 2, 8, BW_X16, 50, 350, 2000, 50},
    //{ 0xEC, 0xCC, 4096, 64, 4, 2, 80, 0, 2, 8, BW_X16, 50, 350, 2000, 50},
    /* 2Gbit NAND Flash */
    { 0xEC, 0xAA, 2048, 64, 4, 1, 40, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 1},
    { 0xEC, 0xDA, 2048, 64, 4, 1, 40, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 1},
    //{ 0xEC, 0xBA, 2048, 64, 4, 1, 40, 0, 2, 8, BW_X16, 50, 350, 2000, 50},
    //{ 0xEC, 0xCA, 2048, 64, 4, 1, 40, 0, 2, 8, BW_X16, 50, 350, 2000, 50},
    /* 2Gbit DDP NAND Flash */
    { 0xEC, 0xDA, 2048, 64, 4, 2, 40, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 1},
    { 0xEC, 0xAA, 2048, 64, 4, 2, 40, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 1},
    //{ 0xEC, 0xBA, 2048, 64, 4, 2, 40, 0, 2, 8, BW_X16, 50, 350, 2000, 50},
    //{ 0xEC, 0xCA, 2048, 64, 4, 2, 40, 0, 2, 8, BW_X16, 50, 350, 2000, 50},
    /*1Gbit NAND Flash */
    { 0xEC, 0xA1, 1024, 64, 4, 1, 20, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 0},
    { 0xEC, 0xF1, 1024, 64, 4, 1, 20, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 0},
    //{ 0xEC, 0xB1, 1024, 64, 4, 1, 20, 0, 2, 8, BW_X16, 50, 350, 2000, 50},
    //{ 0xEC, 0xC1, 1024, 64, 4, 1, 20, 0, 2, 8, BW_X16, 50, 350, 2000, 50},
    /* 1Gbit NAND Flash */
    { 0xEC, 0x79, 8192, 32, 1, 4,120, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 0},
    { 0xEC, 0x78, 8192, 32, 1, 4,120, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 0},
    //{ 0xEC, 0x74, 8192, 32, 1, 4,120,11, 0, 6, BW_X16, 50, 350, 2000, 50},
    //{ 0xEC, 0x72, 8192, 32, 1, 4,120,11, 0, 6, BW_X16, 50, 350, 2000, 50},
    /* 512Mbit NAND Flash */
    { 0xEC, 0x76, 4096, 32, 1, 4, 70, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 1},
    { 0xEC, 0x36, 4096, 32, 1, 4, 70, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 1},

    /* 512Mbit XD Card */
    { 0x98, 0x76, 4096, 32, 1, 4, 70, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 1},

    /* 1Mbit XD Card */
    { 0x98, 0x79, 8192, 32, 1, 4, 70, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 1},

    //{ 0xEC, 0x56, 4096, 32, 1, 4, 70,11, 0, 6, BW_X16, 50, 350, 2000, 50},
    //{ 0xEC, 0x46, 4096, 32, 1, 4, 70,11, 0, 6, BW_X16, 50, 350, 2000, 50},
    /* 256Mbit NAND Flash */
    { 0xEC, 0x75, 2048, 32, 1, 1, 35, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 1},
    { 0xEC, 0x35, 2048, 32, 1, 1, 35, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 1},
    //{ 0xEC, 0x55, 2048, 32, 1, 1, 35,11, 0, 6, BW_X16, 50, 350, 2000, 50},
    //{ 0xEC, 0x45, 2048, 32, 1, 1, 35,11, 0, 6, BW_X16, 50, 350, 2000, 50},
    /* 128Mbit NAND Flash */
    { 0xEC, 0x73, 1024, 32, 1, 1, 20, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 0},
    { 0xEC, 0x33, 1024, 32, 1, 1, 20, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 0},
    //{ 0xEC, 0x53, 1024, 32, 1, 1, 20,11, 0, 6, BW_X16, 50, 350, 2000, 50},
    //{ 0xEC, 0x43, 1024, 32, 1, 1, 20,11, 0, 6, BW_X16, 50, 350, 2000, 50},
    { 0xEC, 0x33, 1024, 32, 1, 1, 20, 5, 0, 6, BW_X08, 50, 350, 2000, 50, 0},
    { 0x2E, 0x64,    8192,  64, 8, 1,  160, 0, 2, 8, BW_X08, 50, 350, 2000, 50, 1},
};

#define POS_BADBLOCK    ((IS_LB)?0x00:0x05)
#define POS_OEMRESERVED ((IS_LB)?0x05:0x04)

#endif _S3C6410_DEVBOARD_

