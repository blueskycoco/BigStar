#include <bsp.h>

//------------------------------------------------------------------------------
//
//  Function:  OALArgsQuery
//
//  This function is called from other OAL modules to return boot arguments.
//  Boot arguments are typically placed in fixed memory location and they are
//  filled by boot loader. In case that boot arguments can't be located
//  the function should return NULL. The OAL module then must use default
//  values.
//

void* OALArgsQuery(UINT32 type)
{
	VOID *pData = NULL;
	BSP_ARGS *pArgs=(BSP_ARGS*)IMAGE_SHARE_ARGS_UA_START;
 	static UCHAR deviceId[16] = "";
 	static UCHAR UUId[16] = "36410001";

	OALMSG(OAL_ARGS&&OAL_FUNC, (L"+OALArgsQuery(%d)\r\n", type));

	// Check if there is expected signature
	if (
		pArgs->header.signature  != OAL_ARGS_SIGNATURE ||
		pArgs->header.oalVersion != OAL_ARGS_VERSION   ||
		pArgs->header.bspVersion != BSP_ARGS_VERSION
	)
	{
		RETAILMSG(1, (L"Goto Clean up.\r\n"));
		goto cleanUp;
	}

	// Depending on required args
	switch (type) {
		case OAL_ARGS_QUERY_UPDATEMODE:
			pData = &pArgs->fUpdateMode;
            break;

		case OAL_ARGS_QUERY_KITL:
			pData = &pArgs->kitl;
		    break;

		case OAL_ARGS_QUERY_DEVID:
			if (deviceId[0] == '\0') {

				DWORD count, code;
				int j = 0;
				UCHAR d;
				// Copy prefix
				count = sizeof(BSP_DEVICE_PREFIX) - 1;	// BSP_DEVICE_PREFIX= "SMDK6410" defined in bsp_cfg.h
				if (count > sizeof(deviceId)/2) count = sizeof(deviceId)/2;
				memcpy(deviceId, BSP_DEVICE_PREFIX, count);

				// Create unique part of name from SoC ID

				code = 0x36410001;

				// Convert it to hexa number
				for (j = 28; j >= 0 && count < sizeof(deviceId) - 1; j -= 4) {
					d = (UCHAR)((code >> j) & 0xF);
					deviceId[count++] = d < 10 ? '0' + d : 'A' + d - 10;
				}

				// End string
				while (count < sizeof(deviceId)) deviceId[count++] = '\0';
			}
			pData = deviceId;
		    break;
		case OAL_ARGS_QUERY_UUID:
			pData = UUId;
            break;
	}

cleanUp:
	return pData;
}



//------------------------------------------------------------------------------

