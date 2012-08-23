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
//  File:  deviceid.c
//
//  This file implements the IOCTL_HAL_GET_DEVICEID/IOCTL_HAL_GET_UUID handlers.
//
#include <windows.h>
#include <oal.h>
#include <s3c6410.h>

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
//  Function:  OALIoCtlHalGetDeviceId/OALIoCtlHalGetUUID
//
//  Implements the IOCTL_HAL_GET_DEVICEID and IOCTL_HAL_GET_UUID handlers.
//
//  A "universally unique" 128 bit identifier is required on all Windows
//  Mobile devices. At a minimum, the 64 bit Device Identifier described
//  below must be permanent (i.e., cannot be modified) and the UUID in
//  total cannot duplicate any other existing UUID. 
//
//  The UUID is composed of 128 bits in the following order: 
//  *   48 bit Manufacturer Code stored in the OAL. The 48 bit Manufacturer
//      Code is specific to the manufacturer and will be provided to each
//      manufacturer by Microsoft. 
//  *   16 bit Version/Variant Code stored in the Kernel. The 16 bit
//      Version/Variant Code describes the version and variant of the UUID.
//      The version is "1" and the variant is "8".
//  *   64 bit Device Identifier Code stored in the hardware (see
//      Realization in OEM documentation). The Device Identifier Code
//      must be stored in hardware and be un-alterable without destroying
//      the device. 
//
//  There are two ways how to call this IOCTL. First will pass buffer with
//  size 16 bytes (size of UUID structure). In such case 
//
BOOL OALIoCtlHalGetDeviceId(
    UINT32 code, VOID *pInpBuffer, UINT32 inpSize, VOID *pOutBuffer,
    UINT32 outSize, UINT32 *pOutSize
) {
    DEVICE_ID *pId = (DEVICE_ID *)pOutBuffer;
    UINT32 size;
    BOOL rc = FALSE;
	int i;
#if defined SMARTFON || defined WPC  // Smartphone or PocketPC build

    // First, handle the special case where we care called with a buffer size of 16 bytes
    if ( outSize == sizeof(UUID) )
    {
        rc = OALIoCtlHalGetUUID(IOCTL_HAL_GET_UUID, pInpBuffer, inpSize, pOutBuffer, outSize, pOutSize);
		//for(i=0;i<outSize;i++)
		//	RETAILMSG(1,(TEXT(" %x \r\n"),*((BYTE *)pOutBuffer+i)));
    }
    else // If buffer size is not 16 bytes, return a DEVICE_ID structure
    {
        UCHAR uuid[sizeof(UUID)]; 
    	//RETAILMSG(1,(TEXT(" Large than 16 \r\n")));
        // Compute required structure size
        size = sizeof(DEVICE_ID) + sizeof(uuid);

        // update size if pOutSize is specified
        if (pOutSize != NULL) 
        {
            *pOutSize = size;
        }

        // Check for invalid parameters        
        if (pOutBuffer == NULL || outSize < sizeof(DEVICE_ID))
        {
            NKSetLastError(ERROR_INVALID_PARAMETER);
            OALMSG(OAL_WARN, (
                L"WARN: OALIoCtlHalGetDeviceID: Invalid parameter\r\n"
            ));                
            goto cleanUp;            
        }

        // Set size to DEVICE_ID structure
        pId->dwSize = size;

        // If the size is too small, indicate the correct size 
        if (outSize < size) 
        {
            NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
            goto cleanUp;
        }

        // Get UUID
        if (OALIoCtlHalGetUUID(IOCTL_HAL_GET_UUID, NULL, 0, uuid, sizeof(uuid), NULL) == FALSE)
        {
            // IOCTL_HAL_GET_UUID handler will set last error, but this really should never happen.
            goto cleanUp;
        }

        // Populate DEVICE_ID structure members with fields from the UUID
        // The manufacturer ID portion of the UUID maps to the "PresetIDBytes" field
        // The HW-unique portion of the UUID maps to the "PlatformIDBytes" field

        // PresetIDBytes
        pId->dwPresetIDOffset = size - sizeof(g_oalIoCtlVendorId);
        pId->dwPresetIDBytes = sizeof(g_oalIoCtlVendorId);

        // PlatformIDBytes
        pId->dwPlatformIDOffset = sizeof(DEVICE_ID);
        pId->dwPlatformIDBytes  = sizeof(uuid) - sizeof(g_oalIoCtlVendorId);

        memcpy((UINT8*)pId + sizeof(DEVICE_ID), uuid, sizeof(uuid));

        // We are done
        rc = TRUE;        
    }
#else  // Not Smartphone or PocketPC build

    UINT32 length1, length2, offset;
    LPSTR pDeviceId = NULL;    

    // Get device unique id from arguments
    pDeviceId = OALArgsQuery(OAL_ARGS_QUERY_DEVID);
    if (pDeviceId == NULL) pDeviceId = "";
    
    // Compute required size (first is unicode, second multibyte!)
    length1 = (NKwcslen(g_oalIoCtlPlatformType) + 1) * sizeof(WCHAR);
    length2 = strlen(pDeviceId) + 1;
    size = sizeof(DEVICE_ID) + length1 + length2;

    // update size if pOutSize is specified
    if (pOutSize) *pOutSize = size;

    // Validate inputs (do it after we can return required size)
    if (pOutBuffer == NULL || outSize < sizeof(DEVICE_ID)) {
        NKSetLastError(ERROR_INVALID_PARAMETER);
        OALMSG(OAL_WARN, (
            L"WARN: OALIoCtlHalGetDeviceID: Invalid parameter\r\n"
        ));
        goto cleanUp;
    }

    // Set size to DEVICE_ID structure
    pId->dwSize = size;

    // If the size is too small, indicate the correct size 
    if (outSize < size) {
        NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
        goto cleanUp;
    }

    // Fill in the Device ID type
    offset = sizeof(DEVICE_ID);

    // Copy in PlatformType data
    pId->dwPresetIDOffset = offset;
    pId->dwPresetIDBytes = length1;
    memcpy((UINT8*)pId + offset, g_oalIoCtlPlatformType, length1);
    offset += length1;

    // Copy device id data
    pId->dwPlatformIDOffset = offset;
    pId->dwPlatformIDBytes  = length2;
    memcpy((UINT8*)pId + offset, pDeviceId, length2);

    // We are done
    rc = TRUE;

#endif // SP/PPC build check

cleanUp:

    return rc;
}

//------------------------------------------------------------------------------

