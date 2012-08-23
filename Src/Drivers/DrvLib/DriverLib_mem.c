#include <windows.h>
#include <bsp_cfg.h>
#include <DrvLib.h>

#ifdef	REMOVE_BEFORE_RELEASE
#define DRVLIB_MSG(x)
#define DRVLIB_INF(x)
#define DRVLIB_ERR(x)	RETAILMSG(TRUE, x)
#else
//#define DRVLIB_MSG(x)	RETAILMSG(TRUE, x)
#define DRVLIB_MSG(x)
#define DRVLIB_INF(x)	RETAILMSG(TRUE, x)
//#define DRVLIB_INF(x)
#define DRVLIB_ERR(x)	RETAILMSG(TRUE, x)
//#define DRVLIB_ERR(x)
#endif

#if	(_WIN32_WCE >= 600)	// Only for WinCE 6.0 and later...

extern unsigned int g_oalAddressTable[][3];

void *DrvLib_MapIoSpace(UINT32 PhysicalAddress, UINT32 NumberOfBytes, BOOL CacheEnable)
{
	UINT32 MappedAddr = 0;
	int index = 0;

	DRVLIB_MSG((_T("[DRVLIB] ++DrvLib_MapIoSpace(0x%08x, 0x%08x, %d)\n\r"), PhysicalAddress, NumberOfBytes, CacheEnable));

	while(g_oalAddressTable[index][2])	// Find to end of table
	{
		if (	(PhysicalAddress >= g_oalAddressTable[index][1])
			&& (PhysicalAddress < ( g_oalAddressTable[index][1]+0x100000*g_oalAddressTable[index][2]) ))
		{
			// Matched Address Found

			MappedAddr = g_oalAddressTable[index][0] +(PhysicalAddress - g_oalAddressTable[index][1]);
			if (!CacheEnable)
			{
				MappedAddr += 0x20000000;	// Offset to Uncached Area
			}

			break;
		}

		index++;		// Find Next
	}

	if (MappedAddr)
	{
		DRVLIB_MSG((_T("[DRVLIB] --DrvLib_MapIoSpace() = 0x%08x\n\r"), MappedAddr));

		return (void *)MappedAddr;
	}
	else
	{
		DRVLIB_ERR((_T("[DRVLIB:ERR] --DrvLib_MapIoSpace() = NULL\n\r")));

		return NULL;
	}
}

void DrvLib_UnmapIoSpace(void *MappedAddress)
{
	return;
}

#else

#define PAGE_SIZE	UserKInfo[KINX_PAGESIZE]

void *DrvLib_MapIoSpace(UINT32 PhysicalAddress, UINT32 NumberOfBytes, BOOL CacheEnable)
{
	PVOID   pVirtualAddress;
	ULONGLONG SourcePhys;
	ULONG   SourceSize;
	BOOL    bSuccess;

	DRVLIB_MSG((_T("[DRVLIB] ++DrvLib_MapIoSpace(0x%08x, 0x%08x, %d)\n\r"), PhysicalAddress, NumberOfBytes, CacheEnable));

	SourcePhys = PhysicalAddress & ~(PAGE_SIZE - 1);
	SourceSize = NumberOfBytes + (NumberOfBytes & (PAGE_SIZE - 1));

	if (SourceSize < NumberOfBytes)
	{ // Prevent Integer overflow.
		DRVLIB_ERR((_T("[DRVLIB:ERR] DrvLib_MapIoSpace() : Invalid Parameter\n\r")));
		return NULL;
	}

	pVirtualAddress = VirtualAlloc(0, SourceSize, MEM_RESERVE, PAGE_NOACCESS);
	if (pVirtualAddress != NULL)
	{
		bSuccess = VirtualCopy(pVirtualAddress, (PVOID)(SourcePhys >> 8), SourceSize, PAGE_PHYSICAL | PAGE_READWRITE | (CacheEnable ? 0 : PAGE_NOCACHE));
		if (bSuccess)
		{
			(ULONG)pVirtualAddress += PhysicalAddress & (PAGE_SIZE - 1);
		}
		else
		{
			DRVLIB_ERR((_T("[DRVLIB:ERR] DrvLib_MapIoSpace() : VirtualCopy() Failed)\n\r")));
			VirtualFree(pVirtualAddress, 0, MEM_RELEASE);
			pVirtualAddress = NULL;
		}
	}
	else
	{
		DRVLIB_ERR((_T("[DRVLIB:ERR] DrvLib_MapIoSpace() : VirtualAlloc() Failed)\n\r")));
	}

	return pVirtualAddress;
}

void DrvLib_UnmapIoSpace(void *MappedAddress)
{
	VirtualFree((PVOID)((ULONG)MappedAddress & ~(ULONG)(PAGE_SIZE - 1)), 0, MEM_RELEASE);
}

#endif

