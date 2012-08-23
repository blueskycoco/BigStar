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
// -----------------------------------------------------------------------------
//
//      THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//      ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//      THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//      PARTICULAR PURPOSE.
//  
// -----------------------------------------------------------------------------
#include <windows.h>
#include <bsp.h>
#include <kitl_cfg.h>

#define KITL_DBON 1

OAL_KITL_DEVICE g_kitlDevice;

#ifdef	KITL_SERIAL
const OAL_KITL_SERIAL_DRIVER *GetKitlSerialDriver (void);
#endif
#ifdef	KITL_USBSERIAL
const OAL_KITL_SERIAL_DRIVER *GetKitlUSBSerialDriver (void);
#endif
volatile S3C6410_GPIO_REG *g_pGPIOReg;

#ifdef KITL_ETHERNET
static OAL_KITL_ETH_DRIVER g_kitlEthCS8900A = OAL_ETHDRV_CS8900A;
#endif

BOOL USBSerKitl_POLL = FALSE;

#ifdef	KITL_SERIAL
BOOL InitKitlSerialArgs (OAL_KITL_ARGS *pKitlArgs)
{
	DWORD dwIoBase = UART_Kitl;

	// Initialize flags
	pKitlArgs->flags = OAL_KITL_FLAGS_ENABLED | OAL_KITL_FLAGS_POLL;

	pKitlArgs->devLoc.LogicalLoc	= dwIoBase;
	pKitlArgs->devLoc.Pin			= OAL_INTR_IRQ_UNDEFINED;
	pKitlArgs->baudRate			= CBR_115200;
	pKitlArgs->dataBits			= DATABITS_8;
	pKitlArgs->parity				= PARITY_NONE;
	pKitlArgs->stopBits			= STOPBITS_10;

	g_kitlDevice.name			= L"6410Serial";
	g_kitlDevice.type				= OAL_KITL_TYPE_SERIAL;
	g_kitlDevice.pDriver			= (VOID*) GetKitlSerialDriver ();

	return TRUE;
}
#endif

#ifdef	KITL_USBSERIAL
BOOL InitKitlUSBSerialArgs (OAL_KITL_ARGS *pKitlArgs)
{
	DWORD dwIoBase = S3C6410_BASE_REG_PA_USBOTG_LINK;

	// Initialize flags
	pKitlArgs->flags = OAL_KITL_FLAGS_ENABLED;
#ifdef USBSER_KITL_POLL
	pKitlArgs->flags |= OAL_KITL_FLAGS_POLL;
	USBSerKitl_POLL = TRUE;
#endif

	pKitlArgs->devLoc.LogicalLoc	= dwIoBase;
	pKitlArgs->devLoc.Pin			= IRQ_OTG; //IRQ_USBD;

	g_kitlDevice.name			= L"6410USBSerial";
	g_kitlDevice.type				= OAL_KITL_TYPE_SERIAL;
	g_kitlDevice.pDriver			= (VOID*) GetKitlUSBSerialDriver ();

	return TRUE;
}
#endif

#ifdef KITL_ETHERNET
BOOL InitKitlEtherArgs (OAL_KITL_ARGS *pKitlArgs)
{
	// init flags
	pKitlArgs->flags = OAL_KITL_FLAGS_ENABLED | OAL_KITL_FLAGS_VMINI;
#ifdef CS8900A_KITL_POLLMODE
	pKitlArgs->flags |= OAL_KITL_FLAGS_POLL;
#endif //CS8900A_KITL_POLLMODE
#ifdef CS8900A_KITL_DHCP
	pKitlArgs->flags |= OAL_KITL_FLAGS_DHCP;
#endif //CS8900A_KITL_DHCP

	pKitlArgs->devLoc.IfcType			= Internal;
	pKitlArgs->devLoc.BusNumber		= 0;
	pKitlArgs->devLoc.LogicalLoc		= BSP_BASE_REG_PA_CS8900A_IOBASE;  // base address
	pKitlArgs->devLoc.Pin				= 0;

	OALKitlStringToMAC(CS8900A_MAC,pKitlArgs->mac);

#ifndef CS8900A_KITL_DHCP
	pKitlArgs->ipAddress				= OALKitlStringToIP(CS8900A_IP_ADDRESS);
	pKitlArgs->ipMask				= OALKitlStringToIP(CS8900A_IP_MASK);
	pKitlArgs->ipRoute				= OALKitlStringToIP(CS8900A_IP_ROUTER);
#endif CS8900A_KITL_DHCP

	g_kitlDevice.ifcType				= Internal;
	g_kitlDevice.type					= OAL_KITL_TYPE_ETH;
	g_kitlDevice.pDriver				= (void *)&g_kitlEthCS8900A;

	//configure nCS3 for cs8900a
	InitSROMC_CS8900();

	//setting EINT10 as IRQ_LAN
	if (!(pKitlArgs->flags & OAL_KITL_FLAGS_POLL))
	{
		g_pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
		g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<20)) | (0x2<<20);
		g_pGPIOReg->GPGPUD = (g_pGPIOReg->GPGPUD & ~(0x3<<20)); // pull-up/down disable
		g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<20)) | (0x1<<20); // High Level trigger
	}
	return TRUE;
}
#endif
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
BOOL OALKitlStart()
{
	OAL_KITL_ARGS	kitlArgs;
#ifdef KITL_ETHERNET
	OAL_KITL_ARGS	*pArgs;
#endif
	BOOL			bRet = FALSE;
	UCHAR			*szDeviceId = NULL;
	UCHAR			buffer[OAL_KITL_ID_SIZE]="\0";

	memset (&kitlArgs, 0, sizeof (kitlArgs));

#ifdef KITL_ETHERNET
	pArgs = (OAL_KITL_ARGS *)OALArgsQuery(OAL_ARGS_QUERY_KITL);
#endif
	//szDeviceId = (CHAR*)OALArgsQuery(OAL_ARGS_QUERY_DEVID);   

	// common parts
	kitlArgs.devLoc.IfcType = g_kitlDevice.ifcType = InterfaceTypeUndefined;
	g_kitlDevice.name	= g_oalIoCtlPlatformType;

	KITLOutputDebugString ("+OALKitlStart : ");

#ifdef KITL_SERIAL
	KITLOutputDebugString ("SERIAL\n");
	RETAILMSG(1,(TEXT("KITL_SERIAL!!\r\n")));
	bRet = InitKitlSerialArgs (&kitlArgs);
	strcpy(buffer,"6410SerKitl");
	szDeviceId = buffer;
	g_kitlDevice.id = kitlArgs.devLoc.LogicalLoc;
#endif //KITL_SERIAL

#ifdef KITL_USBSERIAL
	KITLOutputDebugString ("USB SERIAL\n");
	RETAILMSG(1,(TEXT("SERIAL!!\r\n")));
	bRet = InitKitlUSBSerialArgs (&kitlArgs);
	strcpy(buffer,"6410USBSerKitl");
	szDeviceId = buffer;
	g_kitlDevice.id = kitlArgs.devLoc.LogicalLoc;
#endif //KITL_USBSERIAL

#ifdef KITL_ETHERNET
	KITLOutputDebugString ("ETHERNET\n");
	RETAILMSG(1,(TEXT("KITL_ETHETNET!!\r\n")));
	if (pArgs->devLoc.LogicalLoc == 0)
	{	
		KITLOutputDebugString ("pArgs = NULL\n");
		bRet = InitKitlEtherArgs (&kitlArgs);
		OALKitlCreateName(BSP_DEVICE_PREFIX, kitlArgs.mac, buffer);
		szDeviceId = buffer;
		g_kitlDevice.id = kitlArgs.devLoc.LogicalLoc;
	}
	else
	{
		KITLOutputDebugString ("Kitl args bring from argument setting of RAM\n");
		g_kitlDevice.ifcType			= Internal;
		g_kitlDevice.id    				= BSP_BASE_REG_PA_CS8900A_IOBASE;  // base address
		g_kitlDevice.type               = OAL_KITL_TYPE_ETH;
		g_kitlDevice.pDriver            = (void *)&g_kitlEthCS8900A;

		memcpy(&kitlArgs, pArgs, sizeof (kitlArgs));
		OALKitlCreateName(BSP_DEVICE_PREFIX, kitlArgs.mac, buffer);
		szDeviceId = buffer;

		bRet = TRUE;
	}

	RETAILMSG(KITL_DBON, (L"DeviceId................. %hs\r\n", szDeviceId));
	RETAILMSG(KITL_DBON, (L"kitlArgs.flags............. 0x%x\r\n", kitlArgs.flags));
	RETAILMSG(KITL_DBON, (L"kitlArgs.devLoc.IfcType.... %d\r\n",   kitlArgs.devLoc.IfcType));
	RETAILMSG(KITL_DBON, (L"kitlArgs.devLoc.LogicalLoc. 0x%x\r\n", kitlArgs.devLoc.LogicalLoc));
	RETAILMSG(KITL_DBON, (L"kitlArgs.devLoc.PhysicalLoc 0x%x\r\n", kitlArgs.devLoc.PhysicalLoc));
	RETAILMSG(KITL_DBON, (L"kitlArgs.devLoc.Pin........ %d\r\n",   kitlArgs.devLoc.Pin));
	RETAILMSG(KITL_DBON, (L"kitlArgs.ip4address........ %s\r\n",   OALKitlIPtoString(kitlArgs.ipAddress)));

#endif //KITL_ETHERNET

	if (bRet == FALSE)
	{
		KITLOutputDebugString ("NONE\n");
		RETAILMSG(1, (TEXT("KITL Argument Setting INIT fail.\n")));
		return FALSE;
	}

	if ((kitlArgs.flags & OAL_KITL_FLAGS_ENABLED) == 0)
	{
		RETAILMSG(1, (TEXT("KITL is disabled.\r\n")));
	}
	else
	{
		RETAILMSG(1, (TEXT("KITL is enabeld.\r\n")));
	}

	KITLOutputDebugString ("Call OalKitlInit().\n");
	bRet = OALKitlInit (szDeviceId, &kitlArgs, &g_kitlDevice);
	
	return bRet;
}

//------------------------------------------------------------------------------
//
//  Function:  OALGetTickCount
//
//  This function is Called by some KITL libraries to obtain relative time
//  since device boot. It is mostly used to implement timeout in network
//  protocol.
//

UINT32 OALGetTickCount()
{
	static ULONG count = 0;

	count++;
	return count/100;
}

#if 0
// Define a dummy SetKMode function to satisfy the NAND FMD.
//
DWORD SetKMode (DWORD fMode)
{
    return(1);
}
#endif
//------------------------------------------------------------------------------

#ifdef KITL_ETHERNET
// For SMDK6410
#define CS8900_Tacs	(0x0)	// 0clk
#define CS8900_Tcos	(0x4)	// 4clk
#define CS8900_Tacc	(0xd)	// 14clk
#define CS8900_Tcoh	(0x1)	// 1clk
#define CS8900_Tah	(0x4)	// 4clk
#define CS8900_Tacp	(0x6)	// 6clk
#define CS8900_PMC	(0x0)	// normal(1data)

static void InitSROMC_CS8900(void)
{
	volatile S3C6410_SROMCON_REG *s6410SROM = (S3C6410_SROMCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SROMCON, FALSE);

	s6410SROM->SROM_BW = (s6410SROM->SROM_BW & ~(0xF<<4)) | 
					(1<<7)| // nWBE/nBE(for UB/LB) control for Memory Bank1(0=Not using UB/LB, 1=Using UB/LB)
					(1<<6)| // Wait enable control for Memory Bank1 (0=WAIT disable, 1=WAIT enable)
					(1<<4); // Data bus width control for Memory Bank1 (0=8-bit, 1=16-bit)

	s6410SROM->SROM_BC1 = ((CS8900_Tacs<<28)+(CS8900_Tcos<<24)+(CS8900_Tacc<<16)+(CS8900_Tcoh<<12)\
					+(CS8900_Tah<<8)+(CS8900_Tacp<<4)+(CS8900_PMC));
}
#endif

