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
//  File:  uuid.c
//
//  This file implements the IOCTL_HAL_GET_UUID handler.
//
#include <windows.h>
#include <oal.h>
#include <bsp.h>


//------------------------------------------------------------------------------
//
//  External:  g_oalIoCtlVendorId
//
//  This is vendor/manufacturer code used to generate device UUID. It should
//  be defined in platform code.
//
extern const UINT8 g_oalIoCtlVendorId[6];

//------------------------------------------------------------------------------
//
//  Function:  OALIoCtlHalGetUUID
//
//  Implements the IOCTL_HAL_GET_UUID handler. This function fills in a
//  GUID structure.
//
BOOL OALIoCtlHalGetUUID (
    UINT32 code, VOID *pInpBuffer, UINT32 inpSize, VOID *pOutBuffer,
    UINT32 outSize, UINT32 *pOutSize
) {
 	BOOL rc = FALSE;
    UINT32 id0, id1;
	BSP_ARGS *pArgs=(BSP_ARGS*)IMAGE_SHARE_ARGS_UA_START;
	int iIndex;
	UINT8   szDottedD[16];
	BYTE tmp[20]={0x5b,0x44,0x45,0x4d,0x53,0x33,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x35,0x30,0x30,0x30,0x31};
	UCHAR bMask[64] = {0xA4, 0xAD, 0xBC, 0xE9, 0xC2, 0xBF, 0xBB, 0xE5,
					0xDC, 0xFF, 0xE5, 0x92, 0xFD, 0xF4, 0xEF, 0xAE,
					0xB6, 0xED, 0xB3, 0xD0, 0xE2, 0xDE, 0xA2, 0xC2,
					0xEB, 0xE0, 0xFE, 0xCD, 0x88, 0xA1, 0xB7, 0xE4,
					0xA4, 0xAD, 0xBC, 0xE9, 0xC2, 0xBF, 0xBB, 0xE5,
					0xDC, 0xFF, 0xE5, 0x92, 0xFD, 0xF4, 0xEF, 0xAE,
					0xB6, 0xED, 0xB3, 0xD0, 0xE2, 0xDE, 0xA2, 0xC2,
					0xEB, 0xE0, 0xFE, 0xCD, 0x88, 0xA1, 0xB7, 0xE4};
	UCHAR bMask2[64] = {0x12, 0xF7, 0x3B, 0x6F, 0x7C, 0x24, 0x6F, 0x28,
					0x32, 0x60, 0x6C, 0x28, 0x66, 0x6F, 0x45, 0x03,
					0x6D, 0x0D, 0x51, 0x2B, 0x61, 0x2B, 0x37, 0x00,
					0x60, 0x20, 0x04, 0x04, 0x1F, 0x5B, 0x67, 0x14,
					0x12, 0xF7, 0x3B, 0x6F, 0x7C, 0x24, 0x6F, 0x28,
					0x32, 0x60, 0x6C, 0x28, 0x66, 0x6F, 0x45, 0x03,
					0x6D, 0x0D, 0x51, 0x2B, 0x61, 0x2B, 0x37, 0x00,
					0x60, 0x20, 0x04, 0x04, 0x1F, 0x5B, 0x67, 0x14};

#if defined SMARTFON || defined WPC  // Smartphone or PocketPC build

    UCHAR uuid[sizeof(UUID)];

    // Check parameters
    if (pOutSize != NULL)
    {
        *pOutSize = sizeof(uuid);
    }

    if (pOutBuffer == NULL || outSize < sizeof(uuid)) {
        NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
        OALMSG(OAL_WARN, (L"WARN: OALIoCtlHalGetUUID: Buffer too small\r\n"));
        goto cleanUp;
    }
	//RETAILMSG(1,(TEXT("1\r\n")));
#if 0
    // Get die IDs
    id0 = 0x36410001;
    id1 = 0x36410100;

    // Store unique device ID.
    uuid[0] = (UINT8)( id0 >>  0 );
    uuid[1] = (UINT8)( id0 >>  8 );
    uuid[2] = (UINT8)( id0 >> 16 );
    uuid[3] = (UINT8)( id0 >> 24 );
    uuid[4] = (UINT8)( id1 >>  0 );
    uuid[5] = (UINT8)( id1 >>  8 );
    uuid[6] = (UINT8)( id1 >> 16 );
    uuid[7] = (UINT8)( id1 >> 24 );
    uuid[8] = uuid[7];

    // Lower 4 bits of byte 7 of the Device ID in upper nibble,
    // and 4 bit version in lower nibble.
    uuid[7] <<= 4;
    uuid[7] |= 1;

    // Upper 4 bits of byte 7 of the Device ID in upper nibble,
    // and 4 bit variant in lower nibble.
    uuid[8] &= 0xf0;
    uuid[8] |= 8;

    // Not used, set to 0.
    uuid[9]  = 0;

    // 48 bit HW MFG Product Line Number. We are going to use the MS
    // assigned ID for TI for this.
    uuid[10] = g_oalIoCtlVendorId[0];
    uuid[11] = g_oalIoCtlVendorId[1];
    uuid[12] = g_oalIoCtlVendorId[2];
    uuid[13] = g_oalIoCtlVendorId[3];
    uuid[14] = g_oalIoCtlVendorId[4];
    uuid[15] = g_oalIoCtlVendorId[5];
#else
	//pArgs->deviceId
	//pArgs->Device_ID= inet_addr(szDottedD);
	/*pArgs->Device_ID[0]=tmp[0]^bMask[0]^bMask2[0];
	pArgs->Device_ID[0]=(pArgs->Device_ID[0]<<8)|(tmp[1]^bMask[1]^bMask2[1]);
	pArgs->Device_ID[0]=(pArgs->Device_ID[0]<<8)|(tmp[2]^bMask[2]^bMask2[2]);
	pArgs->Device_ID[0]=(pArgs->Device_ID[0]<<8)|(tmp[3]^bMask[3]^bMask2[3]);
	pArgs->Device_ID[1]=tmp[4]^bMask[4]^bMask2[4];
	pArgs->Device_ID[1]=(pArgs->Device_ID[1]<<8)|(tmp[5]^bMask[5]^bMask2[5]);
	pArgs->Device_ID[1]=(pArgs->Device_ID[1]<<8)|(tmp[6]^bMask[6]^bMask2[6]);
	pArgs->Device_ID[1]=(pArgs->Device_ID[1]<<8)|(tmp[7]^bMask[7]^bMask2[7]);
	pArgs->Device_ID[2]=tmp[8]^bMask[8]^bMask2[8];
	pArgs->Device_ID[2]=(pArgs->Device_ID[2]<<8)|(tmp[9]^bMask[9]^bMask2[9]);
	pArgs->Device_ID[2]=(pArgs->Device_ID[2]<<8)|(tmp[10]^bMask[10]^bMask2[10]);
	pArgs->Device_ID[2]=(pArgs->Device_ID[2]<<8)|(tmp[11]^bMask[11]^bMask2[11]);
	pArgs->Device_ID[3]=tmp[12]^bMask[12]^bMask2[12];
	pArgs->Device_ID[3]=(pArgs->Device_ID[3]<<8)|(tmp[13]^bMask[13]^bMask2[13]);
	pArgs->Device_ID[3]=(pArgs->Device_ID[3]<<8)|(tmp[14]^bMask[14]^bMask2[14]);
	pArgs->Device_ID[3]=(pArgs->Device_ID[3]<<8)|(tmp[15]^bMask[15]^bMask2[15]);	*/
	/*pArgs->Device_ID[4]=tmp[16]^bMask[16]^bMask2[16];
	pArgs->Device_ID[4]=(pArgs->Device_ID[4]<<8)|(tmp[17]^bMask[17]^bMask2[17]);
	pArgs->Device_ID[4]=(pArgs->Device_ID[4]<<8)|(tmp[18]^bMask[18]^bMask2[18]);
	pArgs->Device_ID[4]=(pArgs->Device_ID[4]<<8)|(tmp[19]^bMask[19]^bMask2[19]);*/
	RETAILMSG(0,(TEXT(" %x %x %x %x \r\n"),pArgs->Device_ID[0],pArgs->Device_ID[1],pArgs->Device_ID[2],pArgs->Device_ID[3]));

	szDottedD[3]	=	pArgs->Device_ID[0]&0xff;	
	szDottedD[2]	=	(pArgs->Device_ID[0]>>8)&0xff;
	szDottedD[1]	=	(pArgs->Device_ID[0]>>16)&0xff;
	szDottedD[0]	=	(pArgs->Device_ID[0]>>24)&0xff;	
	szDottedD[7]	=	pArgs->Device_ID[1]&0xff;	
	szDottedD[6]	=	(pArgs->Device_ID[1]>>8)&0xff;
	szDottedD[5]	=	(pArgs->Device_ID[1]>>16)&0xff;
	szDottedD[4]	=	(pArgs->Device_ID[1]>>24)&0xff;
	szDottedD[11]	=	pArgs->Device_ID[2]&0xff;	
	szDottedD[10]	=	(pArgs->Device_ID[2]>>8)&0xff;
	szDottedD[9]	=	(pArgs->Device_ID[2]>>16)&0xff;
	szDottedD[8]	=	(pArgs->Device_ID[2]>>24)&0xff;
	szDottedD[15]	=	pArgs->Device_ID[3]&0xff;	
	szDottedD[14]	=	(pArgs->Device_ID[3]>>8)&0xff;
	szDottedD[13]	=	(pArgs->Device_ID[3]>>16)&0xff;
	szDottedD[12]	=	(pArgs->Device_ID[3]>>24)&0xff;
	/*szDottedD[19]	=	pArgs->Device_ID[4]&0xff;	
	szDottedD[18]	=	(pArgs->Device_ID[4]>>8)&0xff;
	szDottedD[17]	=	(pArgs->Device_ID[4]>>16)&0xff;
	szDottedD[16]	=	(pArgs->Device_ID[4]>>24)&0xff;*/
	for(iIndex=0;iIndex<16;iIndex++)
	{	
		//szDottedD[iIndex] ^= *(BYTE *)pInpBuffer[iIndex];	
		RETAILMSG(0,(TEXT(" %x "),szDottedD[iIndex]));
	}
	RETAILMSG(0,(TEXT("\r\n")));
	for (iIndex = 0; iIndex < 16; ++iIndex)
	   szDottedD[iIndex] ^= bMask[iIndex];
	for (iIndex = 0; iIndex < 16; ++iIndex)
	   szDottedD[iIndex] ^= bMask2[iIndex];
	for(iIndex=0;iIndex<16;iIndex++)
	{	
		//szDottedD[iIndex] ^= *(BYTE *)pInpBuffer[iIndex];	
		RETAILMSG(0,(TEXT(" %x<-> %x "),tmp[iIndex],szDottedD[iIndex]));
	}
#endif
    // Copy UUID to output buffer
    memcpy(pOutBuffer, szDottedD, sizeof(uuid));

#else  // Not Smartphone or PocketPC build

    GUID GuidPattern = { 0xd2a826f9, 0x86be, 0x483a, {
        0xba, 0x94, 0xde, 0x55, 0x4c, 0x55, 0xc, 0xe7
    }};

    VOID* pGuidPattern = &GuidPattern;

    // Check parameters
    if (pOutSize != NULL)
    {
        *pOutSize = sizeof(GuidPattern);
    }

    if (pOutBuffer == NULL || outSize < sizeof(GuidPattern)) {
        NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
        OALMSG(OAL_WARN, (L"WARN: OALIoCtlHalGetUUID: Buffer too small\r\n"));
        goto cleanUp;
    }

    // Get die IDs
    id0 = 0x36410001;
    id1 = 0x36410100;

    // Modify GUID with die IDs
    ((UINT32*)pGuidPattern)[0] ^= id0;
    ((UINT32*)pGuidPattern)[1] ^= id1;

    // Copy GUID pattern to output buffer
    memcpy(pOutBuffer, &GuidPattern, sizeof(GUID));

#endif // SP/PPC build check

    // Done
    rc = TRUE;

cleanUp:
    return rc;
}

//------------------------------------------------------------------------------
