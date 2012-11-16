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
//  File:  init.c
//
//  Samsung SMDK6410 board initialization code.
//
#include <bsp.h>

#define RETAILMSG(cond,printf_exp)	  ((cond)?(NKDbgPrintfW printf_exp),1:0)

#define NOT_FIXEDUP (DWORD)-1
DWORD dwOEMDrWatsonSize = 128 * 1024;//NOT_FIXEDUP;


UINT32 g_oalIoCtlClockSpeed;

void Init_BspArgs(void);
void Port_Init(void);


//------------------------------------------------------------------------------
//
//  Global:  g_oalRtcResetTime
//
//  RTC init time after a RTC reset has occured.
//
SYSTEMTIME g_oalRtcResetTime =
{
	2007, 	// wYear
	7,		// wMonth
	7,		// wDayofWeek
	1,		// wDay
	12,		// wHour
	0,		// wMinute
	0,		// wSecond
	0		// wMilliseconds
};

static void InitializeINFORMSFR(void);
static void InitializeGPIO(void);
static void InitializeCLKGating(void);
static void InitializeBlockPower(void);
static void InitializeCLKSource(void);
static void InitializeOTGCLK(void);

static void Delay(UINT32 count)
{
	volatile int i, j = 0;
	volatile static int loop = S3C6410_ACLK/100000;

	for(;count > 0;count--)
		for(i=0;i < loop; i++) { j++; }
}

extern BOOL (*pfnOEMSetMemoryAttributes) (LPVOID pVirtualAddr, LPVOID pPhysAddr, DWORD cbSize, DWORD dwAttributes);

//------------------------------------------------------------------------------
//
//  OEMSetMemoryAttributes
//
//  OEM function to change memory attributes that isn't supported by kernel.
//  Current implementaion only supports PAGE_WRITECOMBINE.
//
//  This function first try to use PAT, and then try MTRR if PAT isn't available.
//
//------------------------------------------------------------------------------
BOOL OEMSetMemoryAttributes (
    LPVOID pVirtAddr,       // Virtual address of region
    LPVOID pPhysAddrShifted,// PhysicalAddress >> 8 (to support up to 40 bit address)
    DWORD  cbSize,          // Size of the region
    DWORD  dwAttributes     // attributes to be set
    )
{
    if (PAGE_WRITECOMBINE != dwAttributes) {
        DEBUGMSG (1, (L"OEMSetMemoryAttributes: Only PAGE_WRITECOMBINE is supported\r\n"));
        return FALSE;
    }

    return NKVirtualSetAttributes (pVirtAddr, cbSize,
                                  0x4,                  // not cacheable, but bufferable
                                  0xC,                  // Mask of all cache related bits
                                  &dwAttributes);
}


//------------------------------------------------------------------------------
//
//  Function:  OEMInit
//
//  This is Windows CE OAL initialization function. It is called from kernel
//  after basic initialization is made.
//
void OEMInit()
{
	UINT32 logMask_Backup;
	extern DWORD CEProcessorType;	// From nkarm.h in the private tree.
   	//volatile S3C6410_VIC_REG *pIntr = (S3C6410_VIC_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_VIC0, FALSE);

    //NKForceCleanBoot();
	OALMSG(OAL_FUNC, (L"[OAL] ++OEMInit()\r\n"));
	OALMSG(TRUE, (L"[OAL] ++OEMInit()\r\n"));

    OALMSG(OAL_FUNC, ((L"[OAL] APLLCLK : %d\n\r"), System_GetAPLLCLK()));
    OALMSG(OAL_FUNC, ((L"[OAL] ARMCLK  : %d\n\r"), System_GetARMCLK()));
    OALMSG(OAL_FUNC, ((L"[OAL] HCLK    : %d\n\r"), System_GetHCLK()));
    OALMSG(OAL_FUNC, ((L"[OAL] PCLK    : %d\n\r"), System_GetPCLK()));

	g_oalIoCtlClockSpeed = System_GetARMCLK();

    // Lie about the Processor Type (we do this so that the visual C tools work)
	CEProcessorType = PROCESSOR_STRONGARM;

	logMask_Backup = g_oalLogMask;
	OALLogSetZones( (1<<OAL_LOG_ERROR) | (1<<OAL_LOG_WARN) | (1<<OAL_LOG_FUNC) );

	RETAILMSG(TRUE, (TEXT("CEProcessorType = 0x%x\r\n"),CEProcessorType));
	RETAILMSG(TRUE, (TEXT("APLLCLK: %d, FCLK: %d, HCLK: %d, PCLK: %d\r\n"),
        System_GetAPLLCLK(),
        System_GetARMCLK(),
        System_GetHCLK(),
        System_GetPCLK()));

	// Set memory size for DrWatson kernel support
 	if (dwOEMDrWatsonSize != NOT_FIXEDUP)
 	{
        	dwNKDrWatsonSize = dwOEMDrWatsonSize;      // set size of reserved memory for Watson dump
 	}
	// Intialize optional kernel functions. (Processor Extended Feature)
	//
 	pOEMIsProcessorFeaturePresent = OALIsProcessorFeaturePresent;

	// Set OEMSetMemoryAttributes function
	pfnOEMSetMemoryAttributes = OEMSetMemoryAttributes;

	// Turn Off all Debug LED
	//
	OEMWriteDebugLED(0, 0x0);

	// Initialize INFORM SFR
	// This function should be called for CPU Identification.
	InitializeINFORMSFR();

	// Initialize Clock Source
	//
	InitializeCLKSource();

	// Initialize Clock Gating
	//
	InitializeCLKGating();

	// Initialize Block Power
	//
	InitializeBlockPower();
	// Initialize OTG PHY Clock
	//
	InitializeOTGCLK();

	// Initilize cache globals
	OALCacheGlobalsInit();

	OALLogSerial(
		L"DCache: %d sets, %d ways, %d line size, %d size\r\n",
		g_oalCacheInfo.L1DSetsPerWay, g_oalCacheInfo.L1DNumWays,
		g_oalCacheInfo.L1DLineSize, g_oalCacheInfo.L1DSize
	);
	OALLogSerial(
		L"ICache: %d sets, %d ways, %d line size, %d size\r\n",
		g_oalCacheInfo.L1ISetsPerWay, g_oalCacheInfo.L1INumWays,
		g_oalCacheInfo.L1ILineSize, g_oalCacheInfo.L1ISize
	);

	// Initialize Interrupts
	//
	if (!OALIntrInit())
	{
		OALMSG(OAL_ERROR, (L"[OAL:ERR] OEMInit() : failed to initialize interrupts\r\n"));
	}

	// Initialize system clock
	OALTimerInit(RESCHED_PERIOD, OEM_COUNT_1MS, 0);
	// Initialize GPIO
	//
	InitializeGPIO();

	// Initialize the KITL connection if required
	RETAILMSG(1, (TEXT("OALKitlStart() \n\r")));
	OALKitlStart();

	Init_BspArgs();
	OALMSG(OAL_FUNC, (L"-OEMInit\r\n"));

	OALLogSetZones(logMask_Backup);
}

//------------------------------------------------------------------------------
#define pBSPArgs					((BSP_ARGS *) IMAGE_SHARE_ARGS_UA_START)
void Init_BspArgs(void)
{
	// Initialize the BSP args structure.
	//
	if (
		pBSPArgs->header.signature  != OAL_ARGS_SIGNATURE ||
		pBSPArgs->header.oalVersion != OAL_ARGS_VERSION   ||
		pBSPArgs->header.bspVersion != BSP_ARGS_VERSION
	)
	{
		OALMSG(OAL_FUNC, (L"Init_BspArgs.\n"));
		memset(pBSPArgs, 0, sizeof(BSP_ARGS));
		pBSPArgs->header.signature	   = OAL_ARGS_SIGNATURE;
		pBSPArgs->header.oalVersion	  = OAL_ARGS_VERSION;
		pBSPArgs->header.bspVersion	  = BSP_ARGS_VERSION;
		pBSPArgs->fUpdateMode = FALSE;
		pBSPArgs->fUldrReboot = FALSE;
	}
	RETAILMSG(1, (TEXT("pBSPArgs->header.signature(%x)=%x\n"), OAL_ARGS_SIGNATURE, pBSPArgs->header.signature));
	RETAILMSG(1, (TEXT("pBSPArgs->header.oalVersion(%x)=%x\n"), OAL_ARGS_VERSION, pBSPArgs->header.oalVersion));
	RETAILMSG(1, (TEXT("pBSPArgs->header.bspVersion(%x)=%x\n"), BSP_ARGS_VERSION, pBSPArgs->header.bspVersion));
	RETAILMSG(1, (TEXT("pBSPArgs->fUpdateMode=%x\n"), pBSPArgs->fUpdateMode));
	RETAILMSG(1, (TEXT("pBSPArgs->fUldrReboot=%x\n"), pBSPArgs->fUldrReboot));
	RETAILMSG(1, (TEXT("pBSPArgs->kitl.flags=%x\n"), pBSPArgs->kitl.flags));

}

static void InitializeGPIO()
{
	volatile S3C6410_GPIO_REG *pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);

	OALMSG(TRUE, (L"[OAL] ++InitializeGPIO()\r\n"));

	// setup GPNDAT[9] as Input port to check Hive-Clean option
	
	pGPIOReg->GPADAT |= (1<<2);
	pGPIOReg->GPACON = (pGPIOReg->GPACON & ~(0x3<<8)) | (1<<8);
	pGPIOReg->GPAPUD = (pGPIOReg->GPAPUD & ~(0x3<<4)) | (0<<4);
	pGPIOReg->GPLCON1 = pGPIOReg->GPLCON1&(~(0xf<<20))|(0<<20);
	if ((pGPIOReg->GPLDAT & (1<<13)))  //耳机插入
	{ 

		pGPIOReg->GPADAT  |= (1<<2);	//关闭喇叭输出
		//RETAILMSG(1,(TEXT("Headphone has been inserted in!!\r\n")));
	}
	else								//耳机拔出
	{
		pGPIOReg->GPADAT  &= ~(1<<2);
		//RETAILMSG(1,(TEXT("Headphone has been pulled out!\r\n")));
	}
	pGPIOReg->GPNDAT |= (1<<9);
	pGPIOReg->GPNCON = (pGPIOReg->GPNCON & ~(0x3<<18)) | (0<<18);
	pGPIOReg->GPNPUD = (pGPIOReg->GPNPUD & ~(0x3<<18)) | (0<<18);
	pGPIOReg->GPLDAT &= ~(1<<3);
	pGPIOReg->GPLCON0 = (pGPIOReg->GPLCON0 & ~(0x3<<12)) | (1<<12);
	pGPIOReg->GPLPUD = (pGPIOReg->GPLPUD & ~(0x3<<6)) | (0<<6);
	Delay(1200);
	pGPIOReg->GPLDAT |= (1<<3);
	pGPIOReg->GPLDAT |= (1<<5);
	pGPIOReg->GPLCON0 = (pGPIOReg->GPLCON0 & ~(0x3<<20)) | (1<<20);
	pGPIOReg->GPLPUD = (pGPIOReg->GPLPUD & ~(0x3<<10)) | (0<<10);

	pGPIOReg->GPLDAT |= (1<<6);
	pGPIOReg->GPLCON0 = (pGPIOReg->GPLCON0 & ~(0x3<<24)) | (1<<24);
	pGPIOReg->GPLPUD = (pGPIOReg->GPLPUD & ~(0x3<<12)) | (0<<12);
	//Delay(100);
	//pGPIOReg->GPLDAT &= ~(1<<5);
	//pGPIOReg->GPLDAT &= ~(1<<6);
	Delay(800);
	pGPIOReg->GPLDAT |= (1<<5);
	pGPIOReg->GPLDAT |= (1<<6);
	// TODO: What port need initialization???
	OALMSG(TRUE, (L"[OAL] ++3G Power On Queue\r\n"));


	pGPIOReg->GPKCON1 = (pGPIOReg->GPKCON1 & ~(0x0F<<16))|(1<<16);
	pGPIOReg->GPKPUD = (pGPIOReg->GPKPUD & ~(0x3<<24)) | (0<<24);
	pGPIOReg->GPKDAT |= (1<<12);
	Delay(100);

	//3G module reset

	OALMSG(TRUE, (L"[OAL] GPKCON1:0x%x\r\n"),pGPIOReg->GPKCON1);
	OALMSG(TRUE, (L"[OAL] GPKDAT:0x%x\r\n"),pGPIOReg->GPKDAT);
	
	OALMSG(TRUE, (L"[OAL] --3G Power On Queue\r\n"));
	
	pGPIOReg->GPCDAT |= (1<<7);//power off 
	pGPIOReg->GPCCON = (pGPIOReg->GPCCON & ~(0x3<<28)) | (1<<28);


	
	pGPIOReg->GPCDAT |= (1<<6);//power off 
	pGPIOReg->GPCCON = (pGPIOReg->GPCCON & ~(0x3<<24)) | (1<<24);
	pGPIOReg->GPCPUD = (pGPIOReg->GPCPUD & ~(0x3<<12)) | (0<<12);

	pGPIOReg->GPCDAT &= ~(1<<5);//make sure 3g module power off
	pGPIOReg->GPCCON = (pGPIOReg->GPCCON & ~(0x3<<20)) | (1<<20);
	pGPIOReg->GPCPUD = (pGPIOReg->GPCPUD & ~(0x3<<10)) | (0<<10);
	pGPIOReg->GPCCONSLP= (pGPIOReg->GPCCONSLP& ~(0x3<<10)) | (1<<10);
	//Delay(10000);
	Delay(10);
	pGPIOReg->GPCDAT |= (1<<5);//vbat is on
	//Delay(15000);
	Delay(10);
	pGPIOReg->GPCDAT &= ~(1<<6);//power on 3g module
	//Delay(5000);
	Delay(400);
	pGPIOReg->GPCDAT |= (1<<6);//???
	//
	Delay(10);
	pGPIOReg->GPKCON1 = (pGPIOReg->GPKCON1 & ~(0x3<<20)) | (0<<20);
	pGPIOReg->GPKDAT |= (1<<14);
	pGPIOReg->GPKCON1 = (pGPIOReg->GPKCON1 & ~(0x3<<24)) | (1<<24);
	pGPIOReg->GPLDAT |= (1<<0);
	pGPIOReg->GPLCON0 = (pGPIOReg->GPLCON0 & ~(0x3<<0)) | (1<<0);
	pGPIOReg->GPLDAT |= (1<<1);
	pGPIOReg->GPLCON0 = (pGPIOReg->GPLCON0 & ~(0x3<<4)) | (1<<4);

	//camera flash led off
	pGPIOReg->GPLCON0 = (pGPIOReg->GPLCON0 & ~(0xf<<0)) | (0x1<<0);
	//s6410IOP->GPLPUD &= ~(0x3<<0);
	pGPIOReg->GPLDAT |= (1<<0);    //Flash light 0
	pGPIOReg->GPLCON0 = (pGPIOReg->GPLCON0 & ~(0xf<<4)) | (0x1<<4);
	//s6410IOP->GPLPUD &= ~(0x3<<2);
	pGPIOReg->GPLDAT |= (1<<1);    //Flash light 1
	pGPIOReg->GPKCON1 = (pGPIOReg->GPKCON1 & ~(0x0F<<20))|(1<<20);
	pGPIOReg->GPKPUD = (pGPIOReg->GPKPUD & ~(0x3<<26)) | (0<<26);
	pGPIOReg->GPKDAT |= (1<<13);
	OALMSG(TRUE, (L"[OAL] --InitializeGPIO()\r\n"));
}

static void InitializeINFORMSFR(void)
{
	volatile S3C6410_SYSCON_REG *pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);

	OALMSG(TRUE, (L"[OAL] ++InitializeINFORMSFR()\r\n"));

	pSysConReg->INFORM0 = 0x64100000;

	OALMSG(TRUE, (L"[OAL] --InitializeINFORMSFR()\r\n"));
}

static void InitializeCLKGating(void)
{
	volatile S3C6410_SYSCON_REG *pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);

	OALMSG(TRUE, (L"[OAL] InitializeCLKGating()\r\n"));

	// CAUTION !!!
	// HCLK_IROM, HCLK_MEM1, HCLK_MEM0, HCLK_MFC Should be Always On for power Mode
	// Because we can not expect when Warm Reset will be triggered..

	pSysConReg->HCLK_GATE = (0x0<<31)     // 3D
	                        |(1<<30)	// Reserved
							|(0<<29)	// USB Host
							|(0<<28)	// Security Sub-system
							|(0<<27)	// SDMA1
							|(0<<26)	// SDMA0, USB Host
							|(1<<25)	// Internal ROM			<--- Always On (for Power Mode)
							|(1<<24)	// DDR1					<--- Always On
							|(0<<23)	// Reserved
							|(1<<22)	// DMC1					<--- Always On (for Power Mode)
							|(1<<21)	// DMC0, SROM, OneNAND, NFCON, CFCON	<--- Always On (for Power Mode)
							|(0<<20)	// USB OTG
							|(0<<19)	// HSMMC2
							|(0<<18)	// HSMMC1
							|(0<<17)	// HSMMC0
							|(0<<16)	// MDP Interface
#ifdef ULDR
							|(1<<15)	// Direct Host Interface (MSM I/F)
#else
							|(0<<15)	// Direct Host Interface (MSM I/F)
#endif
							|(0<<14)	// Indirect Host Interface
							|(0<<13)	// DMA1
							|(0<<12)	// DMA0
							|(0<<11)	// Jpeg
							|(0<<10)	// Cam Interface
							|(0<<9)		// TV Scaler
							|(0<<8)		// 2D
							|(0<<7)		// TV Encoder
							|(1<<6)		// Reserved
							|(0<<5)		// Post Processor
							|(0<<4)		// Rotator
							|(1<<3)		// Display Controller   <--- Always On
							|(0<<2)		// Trust Interrupt Controller
							|(1<<1)		// Interrupt Controller <--- Always On
							|(1<<0);	// MFC                  <--- Always On (for Power Mode)

	pSysConReg->PCLK_GATE = (0xF<<28)	// Reserved
	                        |(1<<27)    // IIC1
	                        |(0<<26)    // IIS2
	                        |(1<<25)    // Reserved
							|(0<<24)	// Security Key
							|(0<<23)	// CHIP ID
							|(0<<22)	// SPI1
							|(0<<21)	// SPI0
							|(0<<20)	// HSI Receiver
							|(0<<19)	// HSI Transmitter
							|(1<<18)	// GPIO					<--- Always On
							|(1<<17)	// IIC
							|(0<<16)	// IIS1
							|(0<<15)	// IIS0
							|(0<<14)	// AC97 Interface
							|(0<<13)	// TZPC
							|(1<<12)	// Touch Screen & ADC		<--- Always On
							|(0<<11)	// Keypad
							|(0<<10)	// IrDA
							|(0<<9)		// PCM1
							|(0<<8)		// PCM0
							|(1<<7)		// PWM Timer				<--- Always On
							|(1<<6)		// RTC					<--- Always On
                            #if   (CPU_REVISION == EVT0)
							|(0<<5)		// WatchDog Timer
                            #elif (CPU_REVISION == EVT1)
							|(1<<5)		// WatchDog Timer for SW_RST
							#endif
#if 		(DEBUG_PORT == DEBUG_UART0)				// Be Careful to Serial KITL Clock
							|(0<<4)		// UART3
							|(0<<3)		// UART2
							|(0<<2)		// UART1
							|(1<<1)		// UART0					<--- Always On
#elif	(DEBUG_PORT == DEBUG_UART1)				// Be Careful to Serial KITL Clock
							|(0<<4)		// UART3
							|(0<<3)		// UART2
							|(1<<2)		// UART1					<--- Always On
							|(0<<1)		// UART0
#elif	(DEBUG_PORT == DEBUG_UART2)				// Be Careful to Serial KITL Clock
							|(0<<4)		// UART3
							|(1<<3)		// UART2					<--- Always On
							|(0<<2)		// UART1
							|(0<<1)		// UART0
#elif	(DEBUG_PORT == DEBUG_UART3)				// Be Careful to Serial KITL Clock
							|(1<<4)		// UART3					<--- Always On
							|(0<<3)		// UART2
							|(0<<2)		// UART1
							|(0<<1)		// UART0
#endif
							|(0<<0);	// MFC

	pSysConReg->SCLK_GATE = (0x1<<31)	// Reserved
							|(0<<30)	// USB Host
							|(0<<29)	// MMC2 48
							|(0<<28)	// MMC1 48
							|(0<<27)	// MMC0 48
							|(0<<26)	// MMC2
							|(0<<25)	// MMC1
							|(0<<24)	// MMC0
							|(0<<23)	// SPI1 48
							|(0<<22)	// SPI0 48
							|(0<<21)	// SPI1
							|(0<<20)	// SPI0
							|(0<<19)	// DAC 27
							|(0<<18)	// TV Encoder 27
							|(0<<17)	// TV Scaler 27
							|(0<<16)	// TV Scaler
							|(0<<15)	// Display Controller 27
							|(1<<14)	// Display Controller			<--- Always On
							|(0<<13)	// Post Processor1 27
							|(0<<12)	// Post Processor0 27
							|(0<<11)	// Post Processor1
							|(0<<10)	// Post Processor0
							|(0<<9)		// Audio1
							|(0<<8)		// Audio0
							|(0<<7)		// Security Block
							|(0<<6)		// IrDA
							|(1<<5)		// UART0~3				<--- Always On
							|(1<<4)		// Reserved
							|(0<<3)		// MFC
							|(0<<2)		// Camera Interface
							|(0<<1)		// Jpeg
							|(1<<0);	// Reserved

	pSysConReg->MEM0_CLK_GATE = (0x3FFFFFF<<6)	// Reserved
	                            |(0<<5)         // CFCON
	                            |(0<<4)         // OneNAND1
	                            |(0<<3)         // OneNAND0
	                            |(1<<2)         // NFCON
	                            |(0<<1)         // SROM
	                            |(0<<0);		// Reserved
}


static void InitializeBlockPower(void)
{
	volatile S3C6410_SYSCON_REG *pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);

	OALMSG(TRUE, (L"[OAL] InitializeBlockPower()\r\n"));

	pSysConReg->NORMAL_CFG = (1<<31)		// Reserved
							|(0<<30)		// IROM Block Off	(Internal 32KB Boot ROM)
							|(0x1FFF<<17)	// Reserved
							|(1<<16)		// DOMAIN_ETM On	(JTAG not connected when ETM off)
							|(1<<15)		// DOMAIN_S On	(SDMA0, SDMA1, Security System)
							|(1<<14)		// DOMAIN_F On	(LCD, Post, Rotator)
							|(0<<13)		// DOMAIN_P Off	(TV Scaler, TV Encoder, 2D)
							|(0<<12)		// DOMAIN_I Off	(Cam I/F, Jpeg)
							|(1<<11)		// Reserved
							|(0<<10)		// DOMAIN_G Off (3D)
							|(0<<9)			// DOMAIN_V Off	(MFC)
							|(0x100);		// Reserved
}

static void InitializeCLKSource(void)
{
	volatile S3C6410_SYSCON_REG *pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);

	OALMSG(TRUE, (L"[OAL] InitializeCLKSource()\r\n"));

	pSysConReg->CLK_SRC = (pSysConReg->CLK_SRC & ~(0xFFFFFFF0))
							|(0<<31)	// TV27_SEL	-> 27MHz
							|(0<<30)	// DAC27		-> 27MHz
							|(0<<28)	// SCALER_SEL	-> MOUT_EPLL
							|(1<<26)	// LCD_SEL	-> Dout_MPLL
							|(0<<24)	// IRDA_SEL	-> MOUT_EPLL
							|(0<<22)	// MMC2_SEL	-> MOUT_EPLL
							|(0<<20)	// MMC1_SEL	-> MOUT_EPLL
							|(0<<18)	// MMC0_SEL	-> MOUT_EPLL
							|(0<<16)	// SPI1_SEL	-> MOUT_EPLL
							|(0<<14)	// SPI0_SEL	-> MOUT_EPLL
							|(0<<13)	// UART_SEL	-> MOUT_EPLL
							|(0<<10)	// AUDIO1_SEL	-> MOUT_EPLL
							|(0<<7)		// AUDIO0_SEL	-> MOUT_EPLL
							|(0<<5)		// UHOST_SEL	-> 48MHz
							|(0<<4);	// MFCCLK_SEL	-> HCLKx2 (0:HCLKx2, 1:MoutEPLL)

	// TODO: What Clock need dividing???

	pSysConReg->CLK_DIV0 = (pSysConReg->CLK_DIV0 & ~(0xf<<28))
							| ((2-1)<<28);	// MFCCLK = SRC/2

	//pSysConReg->CLK_DIV1;
	//pSysConReg->CLK_DIV2;
}

//--------------------------------------------------------------------
//48MHz clock source for usb host1.1, IrDA, hsmmc, spi is shared with otg phy clock.
//So, initialization and reset of otg phy shoud be done on initial booting time.
//--------------------------------------------------------------------
static void InitializeOTGCLK(void)
{
	volatile S3C6410_SYSCON_REG *pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);
	volatile OTG_PHY_REG *pOtgPhyReg = (OTG_PHY_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_USBOTG_PHY, FALSE);

	pSysConReg->HCLK_GATE |= (1<<20);

	pSysConReg->OTHERS |= (1<<16);

	pOtgPhyReg->OPHYPWR = 0x0;  // OTG block, & Analog bock in PHY2.0 power up, normal operation
	pOtgPhyReg->OPHYCLK = 0x00; // Externel clock/oscillator, 48MHz reference clock for PLL
	pOtgPhyReg->ORSTCON = 0x1;
	Delay(100);
	pOtgPhyReg->ORSTCON = 0x0;
	Delay(100);	//10000

	pSysConReg->HCLK_GATE &= ~(1<<20);

}
