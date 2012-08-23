#include <windows.h>
#include <types.h>
#include <string.h>
#include <stdio.h>
#include <tchar.h>
#include <nkintr.h>
//#include <oalintr.h>
#include <pm.h>
#include "pmplatform.h"
#include <ceddk.h>
#include <S3c6410.h>
#include <bsp.h>
#include <DrvLib.h>
#include "smdk6410_camera.h"
#include "Module.h"
#include "drvlib.h"
/*
// ---------------------------------------------------------------------------
// Macros and Definitions
// ---------------------------------------------------------------------------
*/

// For Debug
#define CAM_MSG				0
#define CAM_INOUT			0
#define	CAM_ERR				1
#define CAM_DUMP			0
#define  MSG_ERROR 1
#define MSG_INFO 0
// Macros

// Definitions
#define	CAM_CLK_SOURCE			(S3C6410_HCLKx2)

#define CAM_CODEC_SACLER_START_BIT			(1<<15)
#define CAM_PVIEW_SACLER_START_BIT			(1<<15)

#define CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT		(1<<31)
#define CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT		(1<<30)
#define CAM_PVIEW_SCALER_CAPTURE_ENABLE_BIT		(1<<29)

#define CAM_OFFSET_STEP						(4)

DWORD dwTemVal = 0;

// ---------------------------------------------------------------------------
// Variables
// Pointer to camera driver instance which we will send back with callback functions
DWORD dwCameraDriverContext;    

// Signals the application that the video or still image frame is available
PFNCAMHANDLEFRAME pfnCameraHandleVideoFrame = NULL;
PFNCAMHANDLEFRAME pfnCameraHandleStillFrame = NULL;
PFNCAMHANDLEFRAME pfnCameraHandlePreviewFrame = NULL;

volatile S3C6410_GPIO_REG 	*s6410IOP = NULL;
volatile S3C6410_CAMIF_REG 	*s6410CAM = NULL;
volatile S3C6410_SYSCON_REG	*s6410PWR = NULL;
HANDLE		hPwrControl;

BUFFER_DESC	Video_Buffer;
BUFFER_DESC	Still_Buffer;
BUFFER_DESC	Preview_Buffer;

PHYSICAL_ADDRESS PhysPreviewAddr;
PHYSICAL_ADDRESS PhysCodecAddr;

PBYTE	pPreviewVirtAddr;
PBYTE	pCodecVirtAddr;

BYTE	PreviewOn=0;
BYTE	CodecOn=0;
BYTE	VideoOn=0;
BYTE	StillOn=0;

UINT32	PreviewFrameCnt=0;			// this is for skipping first 3 frames. Because first 3 frames are useless.
UINT32	CodecFrameCnt=0;			// this is for skipping first 3 frames. Because first 3 frames are useless.

INT32	gHorOffset1=0;
INT32	gVerOffset1=0;
INT32	gHorOffset2=0;
INT32	gVerOffset2=0;

UINT32	g_CamIrq_C = IRQ_CAMIF_C;	
UINT32	g_CamSysIntr_C = SYSINTR_UNDEFINED;
UINT32	g_CamIrq_P = IRQ_CAMIF_P;	
UINT32	g_CamSysIntr_P = SYSINTR_UNDEFINED;

HANDLE	CaptureThread;
HANDLE	CaptureEvent;
HANDLE	PreviewThread;
HANDLE	PreviewEvent;


/******************************************************************************
	Functions
******************************************************************************/


// ---------------------------------------------------------------------------
// Local Functions
// ---------------------------------------------------------------------------
static void CameraGpioInit();			// Initialize GPIO setting for Camera Interface
static void CameraFirstModuleEnable();
static void CameraSecondModuleEnable();
static void CameraInterfaceReset();		// Reset Camera Inteface IP
static void CameraSetClockDiv();		// Clock Div setting

static void CameraSetCodecRegister(UINT32 width, UINT32 height, int Format);	// set codec register
static void CameraSetPreviewRegister(UINT32 width, UINT32 height, int Format); 	// set preview register

static void CameraSetScaler(UINT32 width, UINT32 height, int path);

static void CalculateBurstSize(unsigned int hSize,unsigned int *mainBurstSize,unsigned int *remainedBurstSize);
static void CalculatePrescalerRatioShift(unsigned int SrcSize, unsigned int DstSize, unsigned int *ratio,unsigned int *shift);

static UINT32 CalculateBufferSize(UINT32 width, UINT32 height, int format);

static BOOL InitializeBuffer();
static BOOL DeinitializeBuffer();

static BOOL InterruptInitialize();

static DWORD WINAPI CameraCaptureThread(void);
static DWORD WINAPI CameraPreviewThread(void);

///////////////////////////////////////////////////////////////////////////////
static void Delay(UINT32 count)
{
	volatile int i, j = 0;
	volatile static int loop = TARGET_ARM_CLK/100000;  // mod by shin.0313
	
	for(;count > 0;count--)
		for(i=0;i < loop; i++) { j++; }
}

void DumpCIFRegs()
{
	RETAILMSG(CAM_DUMP,(TEXT("DumpCIFRegs s6410CAM	=%x\r\n"),s6410CAM));
	RETAILMSG(CAM_DUMP,(TEXT("CISRCFMT				=%x\r\n"),s6410CAM->CISRCFMT			));
	RETAILMSG(CAM_DUMP,(TEXT("CIWDOFST				=%x\r\n"),s6410CAM->CIWDOFST			));
	RETAILMSG(CAM_DUMP,(TEXT("CIGCTRL					=%x\r\n"),s6410CAM->CIGCTRL				));
	RETAILMSG(CAM_DUMP,(TEXT("CIFCTRL1				=%x\r\n"),s6410CAM->CIFCTRL1			));
	RETAILMSG(CAM_DUMP,(TEXT("CIFCTRL2				=%x\r\n"),s6410CAM->CIFCTRL2			));
	RETAILMSG(CAM_DUMP,(TEXT("CIDOWSFT2				=%x\r\n"),s6410CAM->CIDOWSFT2			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOYSA1				=%x\r\n"),s6410CAM->CICOYSA1			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOYSA2				=%x\r\n"),s6410CAM->CICOYSA2			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOYSA3				=%x\r\n"),s6410CAM->CICOYSA3			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOYSA4				=%x\r\n"),s6410CAM->CICOYSA4			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOCBSA1				=%x\r\n"),s6410CAM->CICOCBSA1			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOCBSA2				=%x\r\n"),s6410CAM->CICOCBSA2			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOCBSA3				=%x\r\n"),s6410CAM->CICOCBSA3			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOCBSA4				=%x\r\n"),s6410CAM->CICOCBSA4			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOCRSA1				=%x\r\n"),s6410CAM->CICOCRSA1			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOCRSA2				=%x\r\n"),s6410CAM->CICOCRSA2			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOCRSA3				=%x\r\n"),s6410CAM->CICOCRSA3			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOCRSA4				=%x\r\n"),s6410CAM->CICOCRSA4			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOTRGFMT			=%x\r\n"),s6410CAM->CICOTRGFMT		));
	RETAILMSG(CAM_DUMP,(TEXT("CICOCTRL				=%x\r\n"),s6410CAM->CICOCTRL			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOSCPRERATIO	=%x\r\n"),s6410CAM->CICOSCPRERATIO));
	RETAILMSG(CAM_DUMP,(TEXT("CICOSCPREDST		=%x\r\n"),s6410CAM->CICOSCPREDST	));
	RETAILMSG(CAM_DUMP,(TEXT("CICOSCCTRL			=%x\r\n"),s6410CAM->CICOSCCTRL		));
	RETAILMSG(CAM_DUMP,(TEXT("CICOTAREA				=%x\r\n"),s6410CAM->CICOTAREA			));
	RETAILMSG(CAM_DUMP,(TEXT("PAD4						=%x\r\n"),s6410CAM->PAD4					));
	RETAILMSG(CAM_DUMP,(TEXT("CICOSTATUS			=%x\r\n"),s6410CAM->CICOSTATUS		));
	RETAILMSG(CAM_DUMP,(TEXT("PAD5						=%x\r\n"),s6410CAM->PAD5					));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRYSA1				=%x\r\n"),s6410CAM->CIPRYSA1			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRYSA2				=%x\r\n"),s6410CAM->CIPRYSA2			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRYSA3				=%x\r\n"),s6410CAM->CIPRYSA3			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRYSA4				=%x\r\n"),s6410CAM->CIPRYSA4			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRCBSA1				=%x\r\n"),s6410CAM->CIPRCBSA1			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRCBSA2				=%x\r\n"),s6410CAM->CIPRCBSA2			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRCBSA3				=%x\r\n"),s6410CAM->CIPRCBSA3			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRCBSA4				=%x\r\n"),s6410CAM->CIPRCBSA4			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRCRSA1				=%x\r\n"),s6410CAM->CIPRCRSA1			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRCRSA2				=%x\r\n"),s6410CAM->CIPRCRSA2			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRCRSA3				=%x\r\n"),s6410CAM->CIPRCRSA3			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRCRSA4				=%x\r\n"),s6410CAM->CIPRCRSA4			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRTRGFMT			=%x\r\n"),s6410CAM->CIPRTRGFMT		));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRCTRL				=%x\r\n"),s6410CAM->CIPRCTRL			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRSCPRERATIO	=%x\r\n"),s6410CAM->CIPRSCPRERATIO));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRSCPREDST		=%x\r\n"),s6410CAM->CIPRSCPREDST	));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRSCCTRL			=%x\r\n"),s6410CAM->CIPRSCCTRL		));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRTAREA				=%x\r\n"),s6410CAM->CIPRTAREA			));
	RETAILMSG(CAM_DUMP,(TEXT("PAD6						=%x\r\n"),s6410CAM->PAD6					));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRSTATUS			=%x\r\n"),s6410CAM->CIPRSTATUS		));
	RETAILMSG(CAM_DUMP,(TEXT("PAD7						=%x\r\n"),s6410CAM->PAD7					));
	RETAILMSG(CAM_DUMP,(TEXT("CIIMGCPT				=%x\r\n"),s6410CAM->CIIMGCPT			));
	RETAILMSG(CAM_DUMP,(TEXT("CICPTSEQ				=%x\r\n"),s6410CAM->CICPTSEQ			));
	RETAILMSG(CAM_DUMP,(TEXT("PAD8						=%x\r\n"),s6410CAM->PAD8					));
	RETAILMSG(CAM_DUMP,(TEXT("PAD9						=%x\r\n"),s6410CAM->PAD9					));
	RETAILMSG(CAM_DUMP,(TEXT("CIIMGEFF				=%x\r\n"),s6410CAM->CIIMGEFF			));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOY0SA				=%x\r\n"),s6410CAM->MSCOY0SA			));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOCB0SA				=%x\r\n"),s6410CAM->MSCOCB0SA			));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOCR0SA				=%x\r\n"),s6410CAM->MSCOCR0SA			));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOY0END				=%x\r\n"),s6410CAM->MSCOY0END			));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOCB0END			=%x\r\n"),s6410CAM->MSCOCB0END		));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOCR0END			=%x\r\n"),s6410CAM->MSCOCR0END		));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOYOFF				=%x\r\n"),s6410CAM->MSCOYOFF			));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOCBOFF				=%x\r\n"),s6410CAM->MSCOCBOFF			));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOCROFF				=%x\r\n"),s6410CAM->MSCOCROFF			));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOWIDTH				=%x\r\n"),s6410CAM->MSCOWIDTH			));
	RETAILMSG(CAM_DUMP,(TEXT("MSCOCTRL				=%x\r\n"),s6410CAM->MSCOCTRL			));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRY0SA				=%x\r\n"),s6410CAM->MSPRY0SA			));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRCB0SA				=%x\r\n"),s6410CAM->MSPRCB0SA			));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRCR0SA				=%x\r\n"),s6410CAM->MSPRCR0SA			));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRY0END				=%x\r\n"),s6410CAM->MSPRY0END			));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRCB0END			=%x\r\n"),s6410CAM->MSPRCB0END		));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRCR0END			=%x\r\n"),s6410CAM->MSPRCR0END		));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRYOFF				=%x\r\n"),s6410CAM->MSPRYOFF			));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRCBOFF				=%x\r\n"),s6410CAM->MSPRCBOFF			));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRCROFF				=%x\r\n"),s6410CAM->MSPRCROFF			));
	RETAILMSG(CAM_DUMP,(TEXT("MSPRWIDTH				=%x\r\n"),s6410CAM->MSPRWIDTH			));
	RETAILMSG(CAM_DUMP,(TEXT("CIMSCTRL				=%x\r\n"),s6410CAM->CIMSCTRL			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOSCOSY				=%x\r\n"),s6410CAM->CICOSCOSY			));
	RETAILMSG(CAM_DUMP,(TEXT("CICOSCOSCB			=%x\r\n"),s6410CAM->CICOSCOSCB		));
	RETAILMSG(CAM_DUMP,(TEXT("CICOSCOSCR			=%x\r\n"),s6410CAM->CICOSCOSCR		));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRSCOSY				=%x\r\n"),s6410CAM->CIPRSCOSY			));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRSCOSCB			=%x\r\n"),s6410CAM->CIPRSCOSCB		));
	RETAILMSG(CAM_DUMP,(TEXT("CIPRSCOSCR			=%x\r\n"),s6410CAM->CIPRSCOSCR		));
}

void Camera1Power(BOOL bOnOff);
void CameraSetClockDiv();
void CameraModuleReset();
//----------------------------------------------------------------------------
// Function Name	  : CameraInit
//
// Detail Description : Initialize Camera module
//
// Return Data Type   : int
//
// Programming Note   : <program limitation>
//----------------------------------------------------------------------------
int CameraInit(void *pData)
{
	//int i;
	//LARGE_INTEGER 	PerfCount,perfc1;
	
	RETAILMSG(CAM_INOUT,(TEXT("##CameraInit###\r\n")));
	// 0. Map to Virtual Address
	// Camera Virtual alloc
	s6410CAM = (S3C6410_CAMIF_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_CAMIF, sizeof(S3C6410_CAMIF_REG), FALSE);
	if (s6410CAM == NULL)
	{
		RETAILMSG(1,(TEXT("For s6410CAM: DrvLib_MapIoSpace failed!\r\n")));
		return FALSE;;
	}
	//RETAILMSG(CAM_INOUT,(TEXT("CameraInit s6410CAM=%x\r\n"),s6410CAM));

	// GPIO Virtual alloc
	s6410IOP = (S3C6410_GPIO_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_GPIO, sizeof(S3C6410_GPIO_REG), FALSE);
	if (s6410IOP == NULL)
	{
		RETAILMSG(1,(TEXT("For s6410IOP: DrvLib_MapIoSpace failed!\r\n")));
		return FALSE;;
	}
	//RETAILMSG(CAM_INOUT,(TEXT("CameraInit s6410IOP=%x\r\n"),s6410IOP));

	// PWM clock Virtual alloc
	s6410PWR = (S3C6410_SYSCON_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_SYSCON, sizeof(S3C6410_SYSCON_REG), FALSE);
	if (s6410PWR == NULL)
	{
		RETAILMSG(1,(TEXT("For s6410PWR: DrvLib_MapIoSpace failed!\r\n")));
		return FALSE;;
	}
	//RETAILMSG(CAM_INOUT,(TEXT("CameraInit s6410PWR=%x\r\n"),s6410PWR));


	hPwrControl = CreateFile( L"PWC0:", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if (INVALID_HANDLE_VALUE == hPwrControl )
	{
		RETAILMSG(CAM_ERR, (TEXT("[CAM] CameraInit() : PWC0 Open Device Failed\r\n")));
		return FALSE;
	}	
	//QueryPerformanceCounter(&PerfCount);
	//QueryPerformanceCounter(&perfc1);
	//RETAILMSG(1,(TEXT("[CAM] %d %d\r\n"),PerfCount.LowPart,perfc1.LowPart));
	/*
	for(i=0;i<(sizeof(S3C6410_SYSCON_REG))/4;i++)
		RETAILMSG(1,(TEXT("%08x:   %08x\r\n"),i,*((&s6410PWR->APLL_LOCK)+i)));
	for(i=0;i<(sizeof(S3C6410_SYSCON_REG))/4;i++)
	{
		if(i%16==0)
		{
			RETAILMSG(1,(TEXT("\r\n")));
			RETAILMSG(1,(TEXT("%x:   "),(&s6410PWR->APLL_LOCK)+i));
		}
		RETAILMSG(CAM_INOUT,(TEXT("%x "),*((&s6410PWR->APLL_LOCK)+i)));
	}
	RETAILMSG(1,(TEXT("\r\n")));
	*/
	
	// 0. Initialize module.
	ModuleInit();
	
	//CameraSetClockDiv();
	
    // 1. Camera IO setup	
	CameraGpioInit();
	RETAILMSG(CAM_INOUT,(TEXT("CameraGpioInit\r\n")));

	// 5. Allocation Buffer();
	if(!InitializeBuffer())
	{
		return FALSE;
	}

	// 6. Interrupt Initlaize();
	if(!InterruptInitialize())
	{
		return FALSE;
	}

    //RETAILMSG(CAM_INOUT,(TEXT("------------------CameraInit\r\n")));
    
    return TRUE;
}

void CameraDeinit()
{
	RETAILMSG(CAM_INOUT,(TEXT("CameraDeInit\r\n")));

	if(s6410IOP != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)s6410IOP);
		s6410IOP = NULL;
	}
	if(s6410CAM != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)s6410CAM);
		s6410CAM = NULL;
	}
	if(s6410PWR != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)s6410PWR);
		s6410PWR = NULL;
	}

	ModuleDeinit();
	DeinitializeBuffer();

	CloseHandle(CaptureThread);
	CloseHandle(CaptureEvent);
	CloseHandle(PreviewThread);
	CloseHandle(PreviewEvent);
	
	//RETAILMSG(CAM_INOUT,(TEXT("CameraDeInit\r\n")));
}

//----------------------------------------------------------------------------
// Function Name	  : CameraGpioInit()
//
// Detail Description : Initialize Camera related GPIO
//
// Return Data Type   : None
//
// Programming Note   : <program limitation>
//----------------------------------------------------------------------------

void CameraGpioInit()		//	Initialize GPIO setting for Camera Interface
{
    s6410IOP->GPFPUD = (s6410IOP->GPFPUD & ~(0x3ffffff));         // CAM IO PullUpDown Disable setup except CAMRESET
    s6410IOP->GPFCON = (s6410IOP->GPFCON & ~(0x3ffffff)) | 0x2aaaaaa; 
	s6410IOP->GPKCON1 = (s6410IOP->GPKCON1 & ~(0x0F<<20))|(1<<20);
	s6410IOP->GPKPUD = (s6410IOP->GPKPUD & ~(0x3<<26)) | (0<<26);
	Sleep(32);
#if 0
    //32m camera reset
	GPIO_SetFunction(CAM_GPIO_PORT, CAM_3M_RST, eGPIO_OUTPUT);
	GPIO_SetPullUpDown(CAM_GPIO_PORT, CAM_3M_RST, eGPIO_PULL_DISABLE);
    //32m camera cs
	GPIO_SetFunction(CAM_GPIO_PORT, CAM_3M_CS, eGPIO_OUTPUT);
	GPIO_SetPullUpDown(CAM_GPIO_PORT, CAM_3M_CS, eGPIO_PULL_DISABLE);
    //32m camera power
	GPIO_SetFunction(CAM_GPIO_PORT, CAM_3M_PWR, eGPIO_OUTPUT);
	GPIO_SetPullUpDown(CAM_GPIO_PORT, CAM_3M_PWR, eGPIO_PULL_DISABLE);
	
    //32m camera
	GPIO_SetData(CAM_GPIO_PORT, CAM_3M_RST, eGPIO_LOW);
	GPIO_SetData(CAM_GPIO_PORT, CAM_3M_CS, eGPIO_LOW);
	GPIO_SetData(CAM_GPIO_PORT, CAM_3M_PWR, eGPIO_LOW);

	//100K camera reset low
	GPIO_SetFunction(CAM_100K_GPIO_PORT, CAM_100K_RST, eGPIO_OUTPUT);
	GPIO_SetPullUpDown(CAM_100K_GPIO_PORT, CAM_100K_RST, eGPIO_PULL_DISABLE);
	//100K camera Power
	GPIO_SetFunction(CAM_100K_GPIO_PORT, CAM_100K_PWR, eGPIO_OUTPUT);
	GPIO_SetPullUpDown(CAM_100K_GPIO_PORT, CAM_100K_PWR, eGPIO_PULL_DISABLE);
	//100K camera CS
	GPIO_SetFunction(CAM_GPIO_PORT, CAM_100K_CS, eGPIO_OUTPUT);
	GPIO_SetPullUpDown(CAM_GPIO_PORT, CAM_100K_CS, eGPIO_PULL_DISABLE);

	//100K camera
	GPIO_SetData(CAM_GPIO_PORT, CAM_100K_CS, eGPIO_LOW);
	GPIO_SetData(CAM_100K_GPIO_PORT, CAM_100K_RST, eGPIO_LOW);
	GPIO_SetData(CAM_100K_GPIO_PORT, CAM_100K_PWR, eGPIO_LOW);
#endif
}


//----------------------------------------------------------------------------
// Function Name	  : CameraInterfaceReset()
//
// Detail Description : Reset Camera interface
//
// Return Data Type   : None
//
// Programming Note   : 6410 does not support ITU656 interface.
//----------------------------------------------------------------------------
void CameraInterfaceReset()	// Reset Camera Inteface IP
{
	// This functin is used on power handler operation.
	// So, you should not use Kernel API functions as like as "Sleep()".

	MODULE_DESCRIPTOR value;
	
	ModuleGetFormat(value);

	RETAILMSG(CAM_INOUT,(TEXT("CameraInterfaceReset\r\n")));
#if 1
#if 0
	while((s6410CAM->CICOSTATUS & (1<<22)) == 1) // check global  path disable
	{
		RETAILMSG(MSG_ON,(TEXT("global  path 1\r\n")));	
	}
	while((s6410CAM->CICOSTATUS & (1<<28)) == 1) // check VSYNC low
	{
		RETAILMSG(MSG_ON,(TEXT("vsync 1\r\n")));	
	}
#else
	while((s6410CAM->CICOSTATUS & (1<<28)) ||(s6410CAM->CICOSTATUS & (1<<22))) 
	{					
		if ((s6410CAM->CICOSTATUS & (1<<22)) == 1)
			RETAILMSG(MSG_ERROR,(TEXT("  global disable unsatisfied @@@@@@@@@@@@@@\r\n")));
		if ((s6410CAM->CICOSTATUS & (1<<28)) == 1) 
			RETAILMSG(MSG_ERROR,(TEXT("  vsync low unsatisfied @@@@@@@@@@@@@@@@@@@\r\n")));
	}
#endif
#endif
	// Camera (FIMC2.0) I/F Reset
	s6410CAM->CISRCFMT |= (1<<31);	// 6410 board manual recommend   added by jjg 06.07.19  //ITU 601
	s6410CAM->CIGCTRL |= (1<<31);	// 6410 board manual recommend: SwRst 1 -> SwRst 0 for the first SFR setting
	s6410CAM->CIGCTRL &= ~(1<<31);

	//by shlee	//date: Sep. 29
	//if(value.ITUXXX == CAM_ITU656)
	//{
	//	s6410CAM->CISRCFMT &= ~(1<<31);	
	//}
}


void Camera1Power(BOOL bOnOff)
{
	RETAILMSG(CAM_INOUT,(TEXT("[CAM] Camera1Power %d\r\n"),bOnOff));
#if 0
	if(bOnOff)
	{
		//10k camera power
		GPIO_SetData(CAM_100K_GPIO_PORT, CAM_100K_PWR, eGPIO_LOW);
		Sleep(4);
		//10k camera set CS
		GPIO_SetData(CAM_GPIO_PORT, CAM_100K_CS, eGPIO_LOW);
		Sleep(4);
		
	    //32m camera power on
		GPIO_SetData(CAM_GPIO_PORT, CAM_3M_PWR, eGPIO_HIGH);
		Sleep(4);
	    //32m camera cs
		GPIO_SetData(CAM_GPIO_PORT, CAM_3M_CS, eGPIO_LOW);
		Sleep(8);
	    //32m camera release reset
		GPIO_SetData(CAM_GPIO_PORT, CAM_3M_RST, eGPIO_HIGH);
		Sleep(8);

	}else
	{
		GPIO_SetData(CAM_GPIO_PORT, CAM_3M_RST, eGPIO_LOW);
		GPIO_SetData(CAM_GPIO_PORT, CAM_3M_CS, eGPIO_HIGH);
		GPIO_SetData(CAM_GPIO_PORT, CAM_3M_PWR, eGPIO_LOW);
		
		GPIO_SetData(CAM_100K_GPIO_PORT, CAM_100K_PWR, eGPIO_LOW);
	}
#endif
	RETAILMSG(CAM_MSG,(TEXT("[CAM] CameraPower=%d\r\n"),bOnOff));
}

void CameraModuleReset()        // Reset Camera Module
{
    MODULE_DESCRIPTOR value;
    
    ModuleGetFormat(value);
    
    if(value.HighRst)
    {
        s6410CAM->CIGCTRL |= (1<<30);
        // Don't modify this delay time
        Delay(1000);            
        s6410CAM->CIGCTRL &= ~(1<<30);
        // Wait for Camera module initialization
        Delay(1000);        
    }
    else
    {
        //s6410CAM->CIGCTRL |= (1<<30);
        // Wait for Camera module initialization
        //Delay(1000);    
        s6410CAM->CIGCTRL &= ~(1<<30);
        // Wait for Camera module initialization
        Delay(1000);    
        
        s6410CAM->CIGCTRL |= (1<<30);
        // Don't modify this delay time
        Delay(1000);        
    }
}



void CameraClockOn(BOOL bOnOff)
{
	DWORD dwIPIndex = PWR_IP_CAMIF;
	DWORD dwBytes;
	static int isOn = 0;
	RETAILMSG(CAM_INOUT,(TEXT("[CAM] CameraClockOn %d\r\n"),bOnOff));
	// Camera clock
	if (!bOnOff)
	{
		if(isOn == 1)
		{
			isOn = 0;
			s6410PWR->HCLK_GATE &= ~(1<<10); // Camera clock disable
			s6410PWR->SCLK_GATE &= ~(1<<2); // Camera clock disable		
			if ( !DeviceIoControl(hPwrControl, IOCTL_PWRCON_SET_POWER_OFF, &dwIPIndex, sizeof(DWORD), NULL, 0, &dwBytes, NULL) )
			{
				RETAILMSG(CAM_ERR,(TEXT("[CAM:ERR] CameraClockOn(%d) : IOCTL_PWRCON_SET_POWER_OFF Failed\r\n")));
			}	
			
		}
	}
	else 
	{
		if(isOn == 0)
		{
			isOn = 1;
			
			if ( !DeviceIoControl(hPwrControl, IOCTL_PWRCON_SET_POWER_ON, &dwIPIndex, sizeof(DWORD), NULL, 0, &dwBytes, NULL) )
			{
				RETAILMSG(CAM_ERR,(TEXT("[CAM:ERR] CameraClockOn(%d) : IOCTL_PWRCON_SET_POWER_OFF Failed\r\n")));
			}		
				
			s6410PWR->HCLK_GATE |= (1<<10); // Camera clock enable
			s6410PWR->SCLK_GATE |= (1<<2); // Camera clock enable		
			Delay(1000);
			
		}
	}
	//RETAILMSG(CAM_INOUT,(TEXT("[CAM] CameraClockOn\r\n")));
}

//----------------------------------------------------------------------------
// Function Name	  : CameraCaptureSourceSet()
//
// Detail Description : Set camera register to receive capture
//
// Return Data Type   : None
//
// Programming Note   : <program limitation>
//----------------------------------------------------------------------------
void CameraCaptureSourceSet()		// Set source registers 
{
	UINT32 WinOfsEn=0;	
	RETAILMSG(CAM_INOUT,(TEXT("CameraCaptureSourceSet\r\n")));
	MODULE_DESCRIPTOR value;
	
	ModuleGetFormat(value);
	//RETAILMSG(CAM_MSG,(TEXT("value.InvVSYNC=0x%08x  value.InvHREF=%08x\r\n"),value.InvVSYNC,value.InvHREF));
	
	s6410CAM->CIGCTRL = ((value.HighRst ? 0 : 1)<<30)|(1<<29)|(0<<27)|(value.InvPCLK<<26)|
						(value.InvVSYNC<<25)|(value.InvHREF<<24)|(0<<22)|(1<<21)|(1<<20); // inverse PCLK
	s6410CAM->CIWDOFST = (1<<30)|(0xf<<27)|(0xf<<12); // clear overflow 
	s6410CAM->CIWDOFST = 0;	
	
	if((value.SourceHOffset > 0) || (value.SourceVOffset > 0))
	{
		WinOfsEn=1;
	}
	s6410CAM->CIWDOFST = (WinOfsEn<<31)|(value.SourceHOffset <<16)|(value.SourceVOffset);	
	s6410CAM->CIDOWSFT2 = (value.SourceHOffset <<16)|(value.SourceVOffset);	
	
	//TODO:: Set Offset for scaler
	s6410CAM->CISRCFMT = (value.ITUXXX<<31)|(value.UVOffset<<30)|(0<<29)|
							(value.SourceHSize<<16)|(value.Order422<<14)|(value.SourceVSize);      	     	
	
	RETAILMSG(CAM_MSG,(TEXT("s6410CAM->CISRCFMT=0x%08x  s6410CAM->CIGCTRL=%08x\r\n"),s6410CAM->CISRCFMT,s6410CAM->CIGCTRL));
	//RETAILMSG(CAM_INOUT,(TEXT("------------------CameraCaptureSourceSet\r\n")));
}

int CameraPrepareBuffer(P_CAMERA_DMA_BUFFER_INFO pBufInfo, int BufferType)		// allocate DMA buffer
{
	int i,size;
    int sizeY,sizeC;
    
	RETAILMSG(CAM_INOUT,(TEXT("CameraPrepareBuffer\r\n")));
        
    if(BufferType == VIDEO_CAPTURE_BUFFER)
    {
    	size = CalculateBufferSize(Video_Buffer.Width, Video_Buffer.Height, Video_Buffer.Format); 
		Video_Buffer.FrameSize = size;
    	Video_Buffer.Size = size * MAX_HW_FRAMES;
		RETAILMSG(CAM_MSG,(TEXT("Video_Buffer.Width=%d Video_Buffer.Height=%d Video_Buffer.Size=%d  Video_Buffer.Format=%d\r\n"),Video_Buffer.Width, Video_Buffer.Height, Video_Buffer.Size, Video_Buffer.Format));

		if(Video_Buffer.Size > CAPTURE_BUFFER_SIZE)
		{
			RETAILMSG(CAM_ERR,(TEXT("Video size is larger than buffer size\r\n")));
			return FALSE;
		}

		for(i=0;i<MAX_HW_FRAMES;i++)
		{
		    pBufInfo[i].VirtAddr = (DWORD)pCodecVirtAddr + size*i;
		    pBufInfo[i].size = size;
		    pBufInfo[i].pY = (DWORD*)((DWORD)(PhysCodecAddr.LowPart) + size*i);
		}

		if(OUTPUT_CODEC_YCBCR420 == Video_Buffer.Format)
		{
			sizeY = Video_Buffer.Width*Video_Buffer.Height;
			sizeC = Video_Buffer.Width*Video_Buffer.Height/4;
			for(i=0;i<MAX_HW_FRAMES;i++)
			{
			    pBufInfo[i].pCb = (DWORD*)((DWORD)pBufInfo[i].pY + sizeY);
			    pBufInfo[i].pCr = (DWORD*)((DWORD)pBufInfo[i].pCb + sizeC);
			}			
		}
    }
    else if(BufferType == STILL_CAPTURE_BUFFER)
    {
    	size = CalculateBufferSize(Still_Buffer.Width, Still_Buffer.Height, Still_Buffer.Format);
    	Still_Buffer.FrameSize = size;
		Still_Buffer.Size = size;
		RETAILMSG(1,(TEXT("Still_Buffer.Width=%d Still_Buffer.Height=%d Still_Buffer.Size=%d  Still_Buffer.Format=%d\r\n"),Still_Buffer.Width, Still_Buffer.Height, Still_Buffer.Size, Still_Buffer.Format));
     

		if(Still_Buffer.Size > CAPTURE_BUFFER_SIZE)
		{
			RETAILMSG(CAM_ERR,(TEXT("Still size is larger than buffer size\r\n")));
			return FALSE;
		}
		
	    pBufInfo[0].VirtAddr = (DWORD)pCodecVirtAddr;
	    pBufInfo[0].size = size;
	    pBufInfo[0].pY = (DWORD*)(PhysCodecAddr.LowPart);

		
		if(OUTPUT_CODEC_YCBCR420 == Still_Buffer.Format)
		{
			sizeY = Still_Buffer.Width*Still_Buffer.Height;
			sizeC = Still_Buffer.Width*Still_Buffer.Height/4;
		    pBufInfo[0].pCb = (DWORD*)((DWORD)pBufInfo[0].pY + sizeY);
		    pBufInfo[0].pCr = (DWORD*)((DWORD)pBufInfo[0].pCb + sizeC);
		}	    	
    }
    else if(BufferType == PREVIEW_CAPTURE_BUFFER)
    {
		size = CalculateBufferSize(Preview_Buffer.Width, Preview_Buffer.Height, Preview_Buffer.Format);
		Preview_Buffer.FrameSize = size;
    	Preview_Buffer.Size = size * MAX_HW_FRAMES;
		RETAILMSG(CAM_MSG,(TEXT("Preview_Buffer.Width=%d Preview_Buffer.Height=%d Preview_Buffer.Size=%d Preview_Buffer.Format=%d\r\n"),Preview_Buffer.Width, Preview_Buffer.Height, Preview_Buffer.Size, Preview_Buffer.Format));   	

		if(Preview_Buffer.Size > PREVIEW_BUFFER_SIZE)
		{
			RETAILMSG(CAM_ERR,(TEXT("Preview size is larger than buffer size\r\n")));
			return FALSE;
		}

		for(i=0;i<MAX_HW_FRAMES;i++)
		{
		    pBufInfo[i].VirtAddr = (DWORD)pPreviewVirtAddr + size*i;
		    pBufInfo[i].size = size;
		    pBufInfo[i].pY = (DWORD*)((DWORD)(PhysPreviewAddr.LowPart) + size*i);
		}

		if(OUTPUT_CODEC_YCBCR420 == Preview_Buffer.Format)
		{
			sizeY = Preview_Buffer.Width*Preview_Buffer.Height;
			sizeC = Preview_Buffer.Width*Preview_Buffer.Height/4;
			for(i=0;i<MAX_HW_FRAMES;i++)
			{
			    pBufInfo[i].pCb = (DWORD*)((DWORD)pBufInfo[i].pY + sizeY);
			    pBufInfo[i].pCr = (DWORD*)((DWORD)pBufInfo[i].pCb + sizeC);
			}			
		}		
    }
    else 
    {
    	return FALSE;
    }
    
	//RETAILMSG(CAM_INOUT,(TEXT("------------------CameraPrepareBuffer\r\n")));
	return TRUE;
}


int 	CameraDeallocateBuffer(int BufferType)
{
	RETAILMSG(CAM_INOUT,(TEXT("CameraDeallocateBuffer\r\n")));

	//RETAILMSG(CAM_INOUT,(TEXT("------------------CameraDeallocateBuffer\r\n")));
	return TRUE;
}

int 	CameraSetFormat(UINT32 width, UINT32 height, int format, int BufferType)
{
	RETAILMSG(CAM_INOUT,(TEXT("CameraSetFormat WIDTH %d HEIGHT %d Format %d\r\n"), width, height, format));

    if(BufferType == VIDEO_CAPTURE_BUFFER)
    {
    	Video_Buffer.Width = width;
    	Video_Buffer.Height = height;
    	Video_Buffer.Format = format;
   		RETAILMSG(CAM_MSG,(TEXT("CaptureSetFormat: Video Capture Buffer\r\n")));	
    }
    else if(BufferType == STILL_CAPTURE_BUFFER)
    {
	    if(format == OUTPUT_CODEC_JPEG)
	    {
	    	Still_Buffer.Width = JPG_OUT_WIDTH;
	    	Still_Buffer.Height = JPG_OUT_HEIGHT;
	    	Still_Buffer.Format = format;
	    	Still_Buffer.RealImageWidth = width;
	    	Still_Buffer.RealImageHeight = height;
	   		RETAILMSG(MSG_INFO,(TEXT("CameraSetFormat: Still Capture Buffer Jpg\r\n")));	
	    }else
	    {
	    	Still_Buffer.Width = width;
	    	Still_Buffer.Height = height;
	    	Still_Buffer.Format = format;
	    	Still_Buffer.RealImageWidth = width;
	    	Still_Buffer.RealImageHeight = height;
	   		RETAILMSG(MSG_INFO,(TEXT("CameraSetFormat: Still Capture Buffer\r\n")));
	    }
    }
    else if(BufferType == PREVIEW_CAPTURE_BUFFER)
    {
    	Preview_Buffer.Width = width;
    	Preview_Buffer.Height = height;
    	Preview_Buffer.Format = format;
   		RETAILMSG(CAM_MSG,(TEXT("CaptureSetFormat: Preview Capture Buffer\r\n")));	
    }
    else 
    {
    	return FALSE;
    }	
	//RETAILMSG(CAM_INOUT,(TEXT("------------------CameraSetFormat\r\n")));
	return TRUE;
}


void CameraSetCodecRegister(UINT32 width, UINT32 height, int Format)		// set codec register
{	
	//MODULE_DESCRIPTOR value;
	BYTE	Out422_Co, Cpt_CoDMA_RGBFMT,Rotate,scalebypass;
	UINT32 MainBurstSizeY, RemainedBurstSizeY, MainBurstSizeC, RemainedBurstSizeC;	
	
	RETAILMSG(CAM_INOUT,(TEXT("SetCodecRegister  Width=%d  Height=%d  Format=%d\r\n"),width,height,Format));
	//ModuleGetFormat(value);
	scalebypass = 0;
	
	switch(Format)
	{
	case OUTPUT_CODEC_YCBCR422:
		Rotate=3;
		Out422_Co = 2;		// we don't support Non-interleave 4:2:2
		Cpt_CoDMA_RGBFMT = 0;
		CalculateBurstSize(width*2, &MainBurstSizeY, &RemainedBurstSizeY);
		MainBurstSizeY /= 2;
		RemainedBurstSizeY /= 2;
		MainBurstSizeC = MainBurstSizeY/2;
		RemainedBurstSizeC = RemainedBurstSizeY/2;				
		break;
	case OUTPUT_CODEC_RGB16:
		Rotate=3;
		Out422_Co = 3;
		Cpt_CoDMA_RGBFMT = 0;	
		CalculateBurstSize(width*2, &MainBurstSizeY, &RemainedBurstSizeY);		
		MainBurstSizeC = 0;
		RemainedBurstSizeC = 0;
		break;
	case OUTPUT_CODEC_RGB24:
		Rotate=3;
		Out422_Co = 3;
		Cpt_CoDMA_RGBFMT = 2;		
		CalculateBurstSize(width*4, &MainBurstSizeY, &RemainedBurstSizeY);	
		MainBurstSizeC = 0;
		RemainedBurstSizeC = 0;			
		break;
	case OUTPUT_CODEC_JPEG:	//we set the same value as yuv422
		scalebypass = 1;
		Rotate=0;
		Out422_Co = 2;
		Cpt_CoDMA_RGBFMT = 0;
		CalculateBurstSize(width*2, &MainBurstSizeY, &RemainedBurstSizeY);
		MainBurstSizeY /= 2;
		RemainedBurstSizeY /= 2;
		MainBurstSizeC = MainBurstSizeY/2;
		RemainedBurstSizeC = RemainedBurstSizeY/2;				
		break;
	case OUTPUT_CODEC_YCBCR420:
	default:
		Rotate=3; // mf 3;
		Out422_Co = 0;
		Cpt_CoDMA_RGBFMT = 0;
		CalculateBurstSize(width, &MainBurstSizeY, &RemainedBurstSizeY);
		CalculateBurstSize(width/2, &MainBurstSizeC, &RemainedBurstSizeC);			
		break;
	}		
	if(Format == OUTPUT_CODEC_YCBCR420)
		s6410CAM->CICOTRGFMT= (Out422_Co<<29)|(width<<16)|(0<<13)|(Rotate<<14)|(height);
	else
		s6410CAM->CICOTRGFMT= (Out422_Co<<29)|(width<<16)|(Rotate<<14)|(height);
	s6410CAM->CICOCTRL=(MainBurstSizeY<<19)|(RemainedBurstSizeY<<14)|(MainBurstSizeC<<9)|(RemainedBurstSizeC<<4)|(0);
	s6410CAM->CICOSCCTRL=(scalebypass<<31)|(1<<28)|(1<<27)|(0<<26)|(0<<25)|(3<<13)|(Cpt_CoDMA_RGBFMT<<11)|(0<<10);	

	if(Format != OUTPUT_CODEC_JPEG)
		CameraSetScaler(width,height,CODEC_PATH);

	s6410CAM->CICOTAREA=width*height;
	
	//RETAILMSG(CAM_INOUT,(TEXT("------------------SetCodecRegister\r\n")));
}

void CameraSetPreviewRegister(UINT32 width, UINT32 height, int Format) 	// set preview register
{
	//MODULE_DESCRIPTOR value;
	BYTE   Out422_Pr, Cpt_PrDMA_RGBFMT,Rotate,Flip;
	UINT32 MainBurstSizeY, RemainedBurstSizeY, MainBurstSizeC, RemainedBurstSizeC;		
		
	RETAILMSG(CAM_INOUT,(TEXT("SetPreviewRegister Format=%d\r\n"),Format));
	//ModuleGetFormat(value);
	
	switch(Format)
	{
	case OUTPUT_CODEC_YCBCR422:
		Rotate=3;
		Flip = 0;
		Out422_Pr = 2;		// we don't support Non-interleave 4:2:2
		Cpt_PrDMA_RGBFMT = 0;
		CalculateBurstSize(width*2, &MainBurstSizeY, &RemainedBurstSizeY);
		CalculateBurstSize(width/2, &MainBurstSizeC, &RemainedBurstSizeC);		
		break;
	case OUTPUT_CODEC_RGB16:
		Rotate=3;
		Flip = 0;
		Out422_Pr = 3;
		Cpt_PrDMA_RGBFMT = 0;	
		CalculateBurstSize(width*2, &MainBurstSizeY, &RemainedBurstSizeY);		
		MainBurstSizeC = 0;
		RemainedBurstSizeC = 0;
		break;
	case OUTPUT_CODEC_RGB24:
		Rotate=3;
		Flip = 0;
		Out422_Pr = 3;
		Cpt_PrDMA_RGBFMT = 2;		
		CalculateBurstSize(width*4, &MainBurstSizeY, &RemainedBurstSizeY);	
		MainBurstSizeC = 0;
		RemainedBurstSizeC = 0;			
		break;
	case OUTPUT_CODEC_YCBCR420:
	default:
		Rotate = 3;	//3;//   //180
		Flip = 0;	//1
		Out422_Pr = 0;
		Cpt_PrDMA_RGBFMT = 0;
		CalculateBurstSize(width, &MainBurstSizeY, &RemainedBurstSizeY);
		CalculateBurstSize(width/2, &MainBurstSizeC, &RemainedBurstSizeC);			
		break;
	}	
	
	s6410CAM->CIPRTRGFMT= (Out422_Pr<<29)|(width<<16)|(Rotate<<14)|(Flip<<13)|(height);
	s6410CAM->CIPRCTRL=(MainBurstSizeY<<19)|(RemainedBurstSizeY<<14)|(MainBurstSizeC<<9)|(RemainedBurstSizeC<<4);
	s6410CAM->CIPRSCCTRL=(1<<28)|(1<<27)|(0<<26)|(0<<25)|(3<<13)|(Cpt_PrDMA_RGBFMT<<11)|(0<<10);	
	
	CameraSetScaler(width,height,PREVIEW_PATH);
    
	s6410CAM->CIPRTAREA= width*height;	
	
	s6410CAM->CICPTSEQ = 0xFFFFFFFF;
	s6410CAM->CIMSCTRL = 0;	
	s6410CAM->CIPRSCOSY = 0;
	s6410CAM->CIPRSCOSCB = 0;
	s6410CAM->CIPRSCOSCR = 0;

	//RETAILMSG(CAM_INOUT,(TEXT("------------------SetPreviewRegister\r\n")));
}

/********************************************************
 CalculateBurstSize - Calculate the busrt lengths
 
 Description:	
 - dstHSize: the number of the byte of H Size.
 
********************************************************/
void CalculateBurstSize(unsigned int hSize,unsigned int *mainBurstSize,unsigned int *remainedBurstSize)
{
	unsigned int tmp;	
	//RETAILMSG(CAM_INOUT,(TEXT("++++++++++++++++++CalculateBurstSize\r\n")));
	tmp=(hSize/4)%16;
	switch(tmp) {
		case 0:
			*mainBurstSize=16;
			*remainedBurstSize=16;
			break;
		case 4:
			*mainBurstSize=16;
			*remainedBurstSize=4;
			break;
		case 8:
			*mainBurstSize=16;
			*remainedBurstSize=8;
			break;
		default: 
			tmp=(hSize/4)%8;
			switch(tmp) {
				case 0:
					*mainBurstSize=8;
					*remainedBurstSize=8;
					break;
				case 4:
					*mainBurstSize=8;
					*remainedBurstSize=4;
				default:
					*mainBurstSize=4;
					tmp=(hSize/4)%4;
					*remainedBurstSize= (tmp) ? tmp: 4;
					break;
			}
			break;
	}		    
	//RETAILMSG(CAM_INOUT,(TEXT("------------------CalculateBurstSize\r\n")));
}

/********************************************************
 CalculatePrescalerRatioShift - none
 
 Description:	
 - none
 
*/
void CalculatePrescalerRatioShift(unsigned int SrcSize, unsigned int DstSize, unsigned int *ratio,unsigned int *shift)
{
	RETAILMSG(CAM_INOUT,(TEXT("CalculatePrescalerRatioShift\r\n")));
	if(SrcSize>=64*DstSize) {
		RETAILMSG(CAM_INOUT,(TEXT("CalculatePrescalerRatioShift size erro!!!!!!!!!!!!\r\n")));
		//while(1);
	}
	else if(SrcSize>=32*DstSize) {
		*ratio=32;
		*shift=5;
	}
	else if(SrcSize>=16*DstSize) {
		*ratio=16;
		*shift=4;
	}
	else if(SrcSize>=8*DstSize) {
		*ratio=8;
		*shift=3;
	}
	else if(SrcSize>=4*DstSize) {
		*ratio=4;
		*shift=2;
	}
	else if(SrcSize>=2*DstSize) {
		*ratio=2;
		*shift=1;
	}
	else {
		*ratio=1;
		*shift=0;
	}    	
}

UINT32 CalculateBufferSize(UINT32 width, UINT32 height, int format)
{
	UINT32 size=0;
	switch(format)
	{
	case OUTPUT_CODEC_YCBCR422:
		size = width*height + width*height/2*2;
		break;
	case OUTPUT_CODEC_YCBCR420:
		size = width*height + width*height/4*2;
		break;
	case OUTPUT_CODEC_JPEG:
		size = width*height*3;
		break;
	case OUTPUT_CODEC_RGB24:
		size = width*height*4;
		break;
	case OUTPUT_CODEC_RGB16:
		size = width*height*2;
		break;						
	}

	RETAILMSG(CAM_INOUT,(TEXT("CalculateBufferSize size=%d\r\n"),size));
	return size;
}

int		CameraGetCurrentFrameNum(int BufferType)
{
	int temp;
	//RETAILMSG(CAM_INOUT,(TEXT("++++++++++++++++++CameraGetCurrentFrameNum\r\n")));
	if(VIDEO_CAPTURE_BUFFER == BufferType)
	{
		temp = (s6410CAM->CICOSTATUS>>26)&3;
		temp = (temp + 2) % 4;		
	}
	else if(STILL_CAPTURE_BUFFER == BufferType)
	{
		temp = (s6410CAM->CICOSTATUS>>26)&3;
		temp = (temp + 2) % 4;		
	}
	else if(PREVIEW_CAPTURE_BUFFER == BufferType)
	{
		temp = (s6410CAM->CIPRSTATUS>>26)&3;
		temp = (temp + 2) % 4;		
	}
	//RETAILMSG(CAM_INOUT,(TEXT("------------------CameraGetCurrentFrameNum\r\n")));
	return temp;
}

int 	CameraCaptureControl(int Format, BOOL on)
{
	//RETAILMSG(CAM_INOUT,(TEXT("++++++++++++++++++CameraCaptureControl\r\n")));
	s6410CAM->CIIMGCPT &= ~(CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT);
	if(PREVIEW_CAPTURE_BUFFER == Format)
	{
		if(on)
		{
			RETAILMSG(CAM_MSG,(TEXT("Preview ON--------------------------------------------------\r\n")));
			PreviewOn = TRUE;			
			s6410CAM->CIPRSCCTRL |=(CAM_PVIEW_SACLER_START_BIT);
			s6410CAM->CIIMGCPT |= (CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT)|(CAM_PVIEW_SCALER_CAPTURE_ENABLE_BIT);
		}
		else
		{
			RETAILMSG(CAM_MSG,(TEXT("Preview OFF-------------------------------------------------\r\n")));
			PreviewFrameCnt = 0;
			PreviewOn = FALSE;				
			s6410CAM->CIPRSCCTRL &= ~(CAM_PVIEW_SACLER_START_BIT);
			s6410CAM->CIIMGCPT &= ~(CAM_PVIEW_SCALER_CAPTURE_ENABLE_BIT);
		}
	}
	else	// STILL, VIDEO
	{
		if(on)
		{
			RETAILMSG(CAM_MSG,(TEXT("Capture ON--------------------------------------------------\r\n")));
			CodecOn = TRUE;
			if(STILL_CAPTURE_BUFFER == Format)
			{
				StillOn = TRUE;
				VideoOn = FALSE;
			}
			else
			{
				StillOn = FALSE;
				VideoOn = TRUE;
			}			
			if(Still_Buffer.Format == OUTPUT_CODEC_JPEG && STILL_CAPTURE_BUFFER == Format)
			{
				//s6410CAM->CICOSCCTRL |=(CAM_CODEC_SACLER_START_BIT);
				s6410CAM->CIIMGCPT |=(CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT);//|(CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT);
			}else
			{
				s6410CAM->CICOSCCTRL |=(CAM_CODEC_SACLER_START_BIT);
				s6410CAM->CIIMGCPT |=(CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT)|(CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT);
			}
			
			s6410IOP->GPKDAT &= ~(1<<13);
			s6410IOP->GPLDAT &= ~(1<<0);    //Flash light 0
			s6410IOP->GPLDAT &= ~(1<<1);    //Flash light 1
		}
		else
		{
			RETAILMSG(CAM_MSG,(TEXT("Capture OFF-------------------------------------------------\r\n")));
			CodecOn = FALSE;			
			StillOn = FALSE;
			VideoOn = FALSE;
			CodecFrameCnt = 0;			
			s6410CAM->CICOSCCTRL &= ~(CAM_CODEC_SACLER_START_BIT);
			s6410CAM->CIIMGCPT &= ~((CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT));		
			
			s6410IOP->GPKDAT |= (1<<13);			
			s6410IOP->GPLDAT |= (1<<0);    //Flash light 0
			s6410IOP->GPLDAT |= (1<<1);    //Flash light 1
		}				
	}
	
	if(PreviewOn == FALSE && CodecOn == FALSE)	// All Off
	{
		s6410CAM->CIGCTRL &= ~(1<<20);
	}
	else		// On
	{
		s6410CAM->CIGCTRL |= (1<<20);
		s6410CAM->CIIMGCPT |= (CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT);
		
		//RETAILMSG(CAM_MSG,(TEXT("s6410CAM->CIPRSCCTRL=0x%x\r\n"),s6410CAM->CIPRSCCTRL));
		//RETAILMSG(CAM_MSG,(TEXT("s6410CAM->CIIMGCPT=0x%x\r\n"),s6410CAM->CIIMGCPT));
		//RETAILMSG(CAM_MSG,(TEXT("s6410CAM->CIPRSTATUS=0x%x\r\n"),s6410CAM->CIPRSTATUS));

		//DumpCIFRegs();
	}
	
	//RETAILMSG(CAM_INOUT,(TEXT("------------------CameraCaptureControl\r\n")));
	return TRUE;
}

int 	CameraSetRegisters(int format)
{
	int size;

    int sizeY,sizeC;
    
	RETAILMSG(CAM_INOUT,(TEXT("CameraSetRegisters\r\n")));

    if(format == VIDEO_CAPTURE_BUFFER)
    {
		size = Video_Buffer.FrameSize;
		s6410CAM->CICOYSA1 = (UINT32)(PhysCodecAddr.LowPart);
		s6410CAM->CICOYSA2 = s6410CAM->CICOYSA1 + size;
		s6410CAM->CICOYSA3 = s6410CAM->CICOYSA2 + size;
		s6410CAM->CICOYSA4 = s6410CAM->CICOYSA3 + size;
		
		if(OUTPUT_CODEC_YCBCR420 == Video_Buffer.Format)
		{
			sizeY = Video_Buffer.Width*Video_Buffer.Height;
			sizeC = Video_Buffer.Width*Video_Buffer.Height/4;			

			// In DSHOW  CB and Cr are changed.
			s6410CAM->CICOCRSA1=s6410CAM->CICOYSA1+sizeY;
			s6410CAM->CICOCRSA2=s6410CAM->CICOYSA2+sizeY;
			s6410CAM->CICOCRSA3=s6410CAM->CICOYSA3+sizeY;
			s6410CAM->CICOCRSA4=s6410CAM->CICOYSA4+sizeY;
		
			s6410CAM->CICOCBSA1=s6410CAM->CICOCRSA1+sizeC;
			s6410CAM->CICOCBSA2=s6410CAM->CICOCRSA2+sizeC;
			s6410CAM->CICOCBSA3=s6410CAM->CICOCRSA3+sizeC;
			s6410CAM->CICOCBSA4=s6410CAM->CICOCRSA4+sizeC;
		}
		CameraSetCodecRegister(Video_Buffer.Width, Video_Buffer.Height, Video_Buffer.Format);

    }
    else if(format == STILL_CAPTURE_BUFFER)
    {
  
		size = Still_Buffer.FrameSize;			
		s6410CAM->CICOYSA1 = (UINT32)(PhysCodecAddr.LowPart);
		if(OUTPUT_CODEC_JPEG == Still_Buffer.Format)
		{
			s6410CAM->CICOYSA2 = s6410CAM->CICOYSA1 + size;
			s6410CAM->CICOYSA3 = s6410CAM->CICOYSA1;
			s6410CAM->CICOYSA4 = s6410CAM->CICOYSA2;
		}else
		{
		s6410CAM->CICOYSA2 = s6410CAM->CICOYSA1;
		s6410CAM->CICOYSA3 = s6410CAM->CICOYSA1;
		s6410CAM->CICOYSA4 = s6410CAM->CICOYSA1;
		
		if(OUTPUT_CODEC_YCBCR420 == Still_Buffer.Format)
		{
			sizeY = Still_Buffer.Width*Still_Buffer.Height;
			sizeC = Still_Buffer.Width*Still_Buffer.Height/4;
			#if 1
			s6410CAM->CICOCBSA1=s6410CAM->CICOYSA1+sizeY;
            s6410CAM->CICOCBSA2=s6410CAM->CICOCBSA1;
            s6410CAM->CICOCBSA3=s6410CAM->CICOCBSA1;
            s6410CAM->CICOCBSA4=s6410CAM->CICOCBSA1;
        
            s6410CAM->CICOCRSA1=s6410CAM->CICOCBSA1+sizeC;
            s6410CAM->CICOCRSA2=s6410CAM->CICOCRSA1;
            s6410CAM->CICOCRSA3=s6410CAM->CICOCRSA1;
            s6410CAM->CICOCRSA4=s6410CAM->CICOCRSA1;
			#else
			// In DSHOW  CB and Cr are changed.
			s6410CAM->CICOCRSA1=s6410CAM->CICOYSA1+sizeY;
			s6410CAM->CICOCRSA2=s6410CAM->CICOCRSA1;
			s6410CAM->CICOCRSA3=s6410CAM->CICOCRSA1;
			s6410CAM->CICOCRSA4=s6410CAM->CICOCRSA1;
		
			s6410CAM->CICOCBSA1=s6410CAM->CICOCRSA1+sizeC;
			s6410CAM->CICOCBSA2=s6410CAM->CICOCBSA1;
			s6410CAM->CICOCBSA3=s6410CAM->CICOCBSA1;
			s6410CAM->CICOCBSA4=s6410CAM->CICOCBSA1;
			#endif
		}	 
			}			
		CameraSetCodecRegister(Still_Buffer.Width, Still_Buffer.Height, Still_Buffer.Format);
    }
    else if(format == PREVIEW_CAPTURE_BUFFER)
    {    
		size = Preview_Buffer.FrameSize;			
		s6410CAM->CIPRYSA1=(UINT32)(PhysPreviewAddr.LowPart);
		s6410CAM->CIPRYSA2=s6410CAM->CIPRYSA1+size;
		s6410CAM->CIPRYSA3=s6410CAM->CIPRYSA2+size;
		s6410CAM->CIPRYSA4=s6410CAM->CIPRYSA3+size;	   
		if(OUTPUT_CODEC_YCBCR420 == Preview_Buffer.Format)
		{
			sizeY = Preview_Buffer.Width*Preview_Buffer.Height;
			sizeC = Preview_Buffer.Width*Preview_Buffer.Height/4;
		
			// In DSHOW  CB and Cr are changed.
			s6410CAM->CIPRCRSA1=s6410CAM->CIPRYSA1+sizeY;
			s6410CAM->CIPRCRSA2=s6410CAM->CIPRYSA2+sizeY;
			s6410CAM->CIPRCRSA3=s6410CAM->CIPRYSA3+sizeY;
			s6410CAM->CIPRCRSA4=s6410CAM->CIPRYSA4+sizeY;
		
			s6410CAM->CIPRCBSA1=s6410CAM->CIPRCRSA1+sizeC;
			s6410CAM->CIPRCBSA2=s6410CAM->CIPRCRSA2+sizeC;
			s6410CAM->CIPRCBSA3=s6410CAM->CIPRCRSA3+sizeC;
			s6410CAM->CIPRCBSA4=s6410CAM->CIPRCRSA4+sizeC;
		}		 
		CameraSetPreviewRegister(Preview_Buffer.Width, Preview_Buffer.Height, Preview_Buffer.Format);
    }
    else 
    {		
    	return FALSE;
    }
    
	//RETAILMSG(CAM_INOUT,(TEXT("------------------CameraSetRegisters\r\n")));
	return TRUE;
}

BOOL InitializeBuffer()
{
    DMA_ADAPTER_OBJECT Adapter1, Adapter2;
    
	RETAILMSG(CAM_INOUT,(TEXT("InitializeBuffer\r\n")));
    memset(&Adapter1, 0, sizeof(DMA_ADAPTER_OBJECT));
    Adapter1.InterfaceType = Internal;
    Adapter1.ObjectSize = sizeof(DMA_ADAPTER_OBJECT);

    memset(&Adapter2, 0, sizeof(DMA_ADAPTER_OBJECT));
    Adapter2.InterfaceType = Internal;
    Adapter2.ObjectSize = sizeof(DMA_ADAPTER_OBJECT);

    pCodecVirtAddr  = (PBYTE)HalAllocateCommonBuffer(&Adapter1, CAPTURE_BUFFER_SIZE, &PhysCodecAddr, FALSE);
    //PhysCodecAddr.LowPart=IMAGE_CAM_BUFFER_PA_START;
	//pCodecVirtAddr = (PBYTE)DrvLib_MapIoSpace(IMAGE_CAM_BUFFER_PA_START, 0x00F00000, FALSE);
    if (pCodecVirtAddr  == NULL)
    {
        RETAILMSG(CAM_ERR, (TEXT("CameraPrepareBuffer() - Failed to allocate DMA buffer for CODEC Path.\r\n")));
        return FALSE;
    }

    pPreviewVirtAddr  = (PBYTE)HalAllocateCommonBuffer(&Adapter2, PREVIEW_BUFFER_SIZE, &PhysPreviewAddr, FALSE);
    if (pPreviewVirtAddr  == NULL)
    {
        RETAILMSG(CAM_ERR, (TEXT("CameraPrepareBuffer() - Failed to allocate DMA buffer for Preview Path.\r\n")));
        return FALSE;
    }  

	//RETAILMSG(CAM_INOUT,(TEXT("------------------InitializeBuffer\r\n")));
	return TRUE;
}

BOOL DeinitializeBuffer()
{
    DMA_ADAPTER_OBJECT Adapter1,Adapter2;
	RETAILMSG(CAM_INOUT,(TEXT("DeinitializeBuffer\r\n")));
    memset(&Adapter1, 0, sizeof(DMA_ADAPTER_OBJECT));
    Adapter1.InterfaceType = Internal;
    Adapter1.ObjectSize = sizeof(DMA_ADAPTER_OBJECT);

    memset(&Adapter2, 0, sizeof(DMA_ADAPTER_OBJECT));
    Adapter2.InterfaceType = Internal;
    Adapter2.ObjectSize = sizeof(DMA_ADAPTER_OBJECT);
    
    if(pCodecVirtAddr != NULL)
    {    
		HalFreeCommonBuffer(&Adapter1, 0, PhysCodecAddr, (PVOID)pCodecVirtAddr, FALSE);    
		pCodecVirtAddr = NULL;
	}

	if(pPreviewVirtAddr != NULL)
	{
		HalFreeCommonBuffer(&Adapter2, 0, PhysPreviewAddr, (PVOID)pPreviewVirtAddr, FALSE);    
		pPreviewVirtAddr = NULL;
	}
	//RETAILMSG(CAM_INOUT,(TEXT("-------------------DeinitializeBuffer\r\n")));
	return TRUE;
}

BOOL InterruptInitialize()
{
	DWORD   threadID;                         // thread ID
	BOOL	bSuccess;

	RETAILMSG(CAM_INOUT,(TEXT("InterruptInitialize\r\n")));

    if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &g_CamIrq_C, sizeof(UINT32), &g_CamSysIntr_C, sizeof(UINT32), NULL))
    {
        RETAILMSG(CAM_ERR, (TEXT("ERROR: Failed to request sysintr value for Camera interrupt.\r\n")));
        return FALSE;
    }

    CaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (!CaptureEvent)
    {
		RETAILMSG(CAM_ERR,(TEXT("Fail to create camera interrupt event\r\n")));
    	return FALSE;
    }

	bSuccess = InterruptInitialize(g_CamSysIntr_C, CaptureEvent, NULL, 0);
    if (!bSuccess) 
    {
        RETAILMSG(CAM_ERR,(TEXT("Fail to initialize camera interrupt event\r\n")));
        return FALSE;
    }    

    CaptureThread = CreateThread(NULL,
                                 0,
                                 (LPTHREAD_START_ROUTINE)CameraCaptureThread,
                                 0,
                                 0,
                                 &threadID);
    
    if (NULL == CaptureThread ) {
    	RETAILMSG(CAM_ERR,(TEXT("Create Camera Thread Fail\r\n")));
		return FALSE;
    }

	///////////////////////////////////////////////////////////////////////////////////////////////////

	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &g_CamIrq_P, sizeof(UINT32), &g_CamSysIntr_P, sizeof(UINT32), NULL))
    {
        RETAILMSG(CAM_ERR, (TEXT("ERROR: Failed to request sysintr value for Camera interrupt.\r\n")));
        return FALSE;
    }

    PreviewEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (!PreviewEvent)
    {
		RETAILMSG(CAM_ERR,(TEXT("Fail to create camera interrupt event\r\n")));
    	return FALSE;
    }

	bSuccess = InterruptInitialize(g_CamSysIntr_P, PreviewEvent, NULL, 0);
    if (!bSuccess) 
    {
        RETAILMSG(CAM_ERR,(TEXT("Fail to initialize camera interrupt event\r\n")));
        return FALSE;
    }    

    PreviewThread = CreateThread(NULL,
                                 0,
                                 (LPTHREAD_START_ROUTINE)CameraPreviewThread,
                                 0,
                                 0,
                                 &threadID);
    
    if (NULL == PreviewThread ) {
    	RETAILMSG(CAM_ERR,(TEXT("Create Camera Thread Fail\r\n")));
		return FALSE;
    }

	//RETAILMSG(CAM_INOUT,(TEXT("-------------------InterruptInitialize  %d  %d\r\n"),g_CamSysIntr_C,g_CamSysIntr_P));
	return TRUE;
}

DWORD WINAPI CameraCaptureThread(void)
{
	DWORD	dwCause;
	int i;
	
	while(TRUE)
	{
		dwCause = WaitForSingleObject(CaptureEvent, INFINITE);
		//RETAILMSG(CAM_ERR, (TEXT("[CAM_HW] Camera interrput c\r\n")));
		
		if (dwCause == WAIT_OBJECT_0) 
		{
			if(CodecFrameCnt >= 2)
			{
				CodecFrameCnt = 3;
				if(StillOn)
				{				
					s6410CAM->CICOSCCTRL &= ~(CAM_CODEC_SACLER_START_BIT);
					s6410CAM->CIIMGCPT &= ~((CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT));
					
					while((s6410CAM->CICOSTATUS & (1<<21)) == 1) // check Codec path disable
					{
						if(i++>2000)
						{
							RETAILMSG(MSG_ERROR, (TEXT("[CameraCaptureThread] unsafe CICOSTATUS %x ------\r\n"), s6410CAM->CICOSTATUS));
							break;
						}
						Sleep(2);
						
					}
					
#if 1
					if(Still_Buffer.Format == OUTPUT_CODEC_JPEG)
					{
						i = 0;
						while((s6410CAM->CICOSTATUS & (1<<22)) == 1) // check global  path disable
						{
							if(i++>2000)
							{
								RETAILMSG(MSG_ERROR, (TEXT("[CameraCaptureThread] unsafe CICOSTATUS %x ------\r\n"), s6410CAM->CICOSTATUS));
								break;
							}
							Sleep(2);
						}
					}
#endif

					pfnCameraHandleStillFrame(dwCameraDriverContext);
					
#if 0 				//yqf 0212, donot restart capture, those set/non-set don't effect still image quality, but for handleStillInterrupt safety???
					// Restart Capture
					RETAILMSG(MSG_ON, (TEXT("[CAM_HW] Camera interrput c Still_Buffer.Format = %d \r\n"),Still_Buffer.Format));
					if(Still_Buffer.Format == OUTPUT_CODEC_JPEG)
					{
						s6410CAM->CIIMGCPT |=(CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT);	
					}else
					{
						s6410CAM->CICOSCCTRL |=(CAM_CODEC_SACLER_START_BIT);
						s6410CAM->CIIMGCPT |=(CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT)|(CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT);	
					}
#endif
				}
				else if(VideoOn)
				{
					pfnCameraHandleVideoFrame(dwCameraDriverContext);
				}
			}
			else
			{
				CodecFrameCnt++;
			}
			s6410CAM->CIGCTRL |= (1<<19);			
			InterruptDone(g_CamSysIntr_C);	
		}
		else
		{
			RETAILMSG(CAM_ERR, (TEXT("[CAM_HW] InterruptThread : Exit %d, Cause %d\r\n"), GetLastError(), dwCause));
		}
	}

	return 0;
}

DWORD WINAPI CameraPreviewThread(void)
{
	DWORD	dwCause;
	
	while(TRUE)
	{		
		dwCause = WaitForSingleObject(PreviewEvent, INFINITE);
		//RETAILMSG(CAM_ERR, (TEXT("[CAM_HW] Camera interrput p\r\n")));
		
		if (dwCause == WAIT_OBJECT_0) 
		{
			
			if(PreviewFrameCnt >= 2)
			{
				PreviewFrameCnt = 3;
				pfnCameraHandlePreviewFrame(dwCameraDriverContext);
			}
			else
			{
				PreviewFrameCnt ++;
			}
			s6410CAM->CIGCTRL |= (1<<18);
			InterruptDone(g_CamSysIntr_P);	
		}
		else
		{
			RETAILMSG(CAM_ERR, (TEXT("[CAM_HW] InterruptThread : Exit %d, Cause %d\r\n"), GetLastError(), dwCause));
		}
	}

	return 0;
}

void CameraSetClockDiv()
{
	MODULE_DESCRIPTOR value;
	int	div;
	
	ModuleGetFormat(value);
	div = (int)(CAM_CLK_SOURCE / (float)value.Clock + 0.5) - 1;
	RETAILMSG(CAM_MSG,(TEXT("[CAM]  CameraSetClockDiv = %d\r\n"),div));
	if(div>15)
		div = 15;
	
	s6410PWR->CLK_DIV0 = (s6410PWR->CLK_DIV0 & ~(0xf<<20))  | ((div & 0xf)<< 20); // CAMCLK is divided..	
}

void	CameraSleep(DWORD dwDeviceIndex)
{
	RETAILMSG(CAM_INOUT,(TEXT("[CAM] CameraSleep\r\n")));

	if(dwDeviceIndex==1)
	{
		Camera1Power(FALSE);	
		CameraClockOn(FALSE);
	}else
	{
		//CameraClockOn(FALSE);
	}
	//s6410IOP->GPFPUD = (s6410IOP->GPFPUD |(0x1555515)); 		// CAM IO PullUpDown Disable setup except CAMRESET
	//s6410IOP->GPFCON = (s6410IOP->GPFCON & ~(0x3ffff3f)) | 0x1555515; 
	//s6410IOP->GPFDAT = (s6410IOP->GPFDAT | 0x1ff7);				//& ~(0x1ff7)); 	//if output 0, the system will be crashed

    //RETAILMSG(CAM_INOUT,(TEXT("--------------- [CAM] CameraSleep GPFPUD=%X GPFDAT=%X\r\n"),s6410IOP->GPFPUD,s6410IOP->GPFDAT));
}

void 	CameraPreviewRecovery()
{
		Module1PreviewEnable(Preview_Buffer.Format);
}

void	CameraCaptureMode()
{
		Module1CaptureMode();
}

void	CameraResume(DWORD dwDeviceIndex)
{
	static int firsttimehere = 1;
	RETAILMSG(CAM_INOUT,(TEXT("[CAM] CameraResume  dwDeviceIndex=%d\r\n"),dwDeviceIndex));

	if(firsttimehere)
	{
		firsttimehere = 1;
		// 0. Initialize each module.
		Module1Init();

		CameraSetClockDiv();

		Camera1Power(TRUE);	

		CameraClockOn(TRUE);

		CameraGpioInit();    
		CameraModuleReset();

		// 4. Write Setting for Module using I2C
		Module1ArmGo();	
		//Sleep(2);
		//Sleep(2);
		//Module1PreviewEnable(Preview_Buffer.Format);	
	}
	// 5. Camera i/f reset
	CameraInterfaceReset();
	RETAILMSG(CAM_INOUT,(TEXT("CameraInterfaceReset\r\n")));

	// 6. Initialize I/F source register
	CameraCaptureSourceSet();


	//RETAILMSG(CAM_INOUT,(TEXT("--------------- [CAM] CameraResume\r\n")));
}


BOOL		CameraSaturation(int value)
{
	BOOL bRet = FALSE;
		bRet = TRUE;//Module1SaturationManual(value);

	return bRet;
}

BOOL		CameraContrast(int value)
{
	BOOL bRet = FALSE;
		bRet = TRUE;//Module1ContrastManual(value);

	return bRet;
}

BOOL		CameraWhitebalance(int value)
{
	BOOL bRet = FALSE;
		bRet = TRUE;//Module1WhiteBalanceManual(value);

	return bRet;
}

BOOL		CameraBrightness(int value)
{
	BOOL bRet = FALSE;
		bRet = TRUE;//Module1BrightnessManual(value);

	return bRet;
}

BOOL		CameraExposure (int value)
{
	BOOL bRet = FALSE;
		bRet = TRUE;//Module1AEManual (value);

	return bRet;
}

BOOL		CameraFocus(int value)
{
	BOOL bRet = FALSE;
		bRet = TRUE;//Module1FocusManual(value);

	return bRet;
}

BOOL		CameraFlash(int value)
{
	BOOL bRet = FALSE;
		bRet = TRUE;//Module1FlashManual(value);

	dwTemVal = value;
	return bRet;
}

BOOL		CameraScene(int value)
{
	BOOL bRet = FALSE;
		bRet = TRUE;//Module1SceneManual(value);
	dwTemVal = value;
	return bRet;
}

BOOL		CameraZoom(int value)
{
	BOOL bRet = FALSE;
		bRet = TRUE;//Module1ZoomManual(value);
	return bRet;
}

void CameraModuleSetImageSize(UINT16 uiWidth, UINT16 uiHeight)
{
		Module1SetImageSize(uiWidth, uiHeight);
}

void 	CameraSetScaler(UINT32 width, UINT32 height, int path)
{
	UINT32 H_Shift, V_Shift, PreHorRatio, PreVerRatio, MainHorRatio, MainVerRatio;
	UINT32 ScaleUp_H, ScaleUp_V, SrcWidth, SrcHeight, WinOfsEn;	
	MODULE_DESCRIPTOR moduleValue;
	
	RETAILMSG(CAM_INOUT,(TEXT("CameraSetScaler\r\n")));
	
	if(width == 0 || height == 0) return;
	
	ModuleGetFormat(moduleValue);
			
	SrcWidth=moduleValue.SourceHSize-moduleValue.SourceHOffset*2-gHorOffset1-gHorOffset2;
	SrcHeight=moduleValue.SourceVSize-moduleValue.SourceVOffset*2-gVerOffset1-gVerOffset2;	
	
	if((moduleValue.SourceHSize > SrcWidth) || (moduleValue.SourceVSize > SrcHeight))
	{
		WinOfsEn=1;
	}
	
	RETAILMSG(CAM_MSG,(TEXT("[CAM] width=%d   height=%d   SrcWidth=%d  SrcHeight=%d\r\n"),width,height,SrcWidth,SrcHeight));
	s6410CAM->CIWDOFST = (WinOfsEn<<31)|((moduleValue.SourceHOffset+gHorOffset1) <<16)|(moduleValue.SourceVOffset+gVerOffset1);		 
	s6410CAM->CIDOWSFT2 = ((moduleValue.SourceHOffset+gHorOffset2) <<16)|(moduleValue.SourceVOffset+gVerOffset2);			
	
	CalculatePrescalerRatioShift(SrcWidth, width, &PreHorRatio, &H_Shift);
	CalculatePrescalerRatioShift(SrcHeight, height, &PreVerRatio, &V_Shift);
	MainHorRatio=(SrcWidth<<8)/(width<<H_Shift);
	MainVerRatio=(SrcHeight<<8)/(height<<V_Shift);
		
	if(SrcWidth>width) ScaleUp_H=0; //down
	else ScaleUp_H=1;		//up

	if(SrcHeight>height) ScaleUp_V=0;
	else ScaleUp_V=1;
			
	switch(path)
	{
	case PREVIEW_PATH:
		s6410CAM->CIPRSCPRERATIO=((10-H_Shift-V_Shift)<<28)|(PreHorRatio<<16)|(PreVerRatio);		 
		s6410CAM->CIPRSCPREDST=((SrcWidth/PreHorRatio)<<16)|(SrcHeight/PreVerRatio); 
		s6410CAM->CIPRSCCTRL = (s6410CAM->CIPRSCCTRL & ~((0x1<<31)|(0x1<<30)|(0x1<<29)|(0x1ff<<16)|(0x1ff<<0)))
								|(0<<31)|(ScaleUp_H<<30)|(ScaleUp_V<<29)|(MainHorRatio<<16)|(MainVerRatio);
		break;
	case CODEC_PATH:
		s6410CAM->CICOSCPRERATIO=((10-H_Shift-V_Shift)<<28)|(PreHorRatio<<16)|(PreVerRatio);
		s6410CAM->CICOSCPREDST=((SrcWidth/PreHorRatio)<<16)|(SrcHeight/PreVerRatio); 
		s6410CAM->CICOSCCTRL=(s6410CAM->CICOSCCTRL & ~((0x1<<31)|(0x1<<30)|(0x1<<29)|(0x1ff<<16)|(0x1ff<<0)))
								|(0<<31)|(ScaleUp_H<<30)|(ScaleUp_V<<29)|(MainHorRatio<<16)|(MainVerRatio);	
		break;		
	}
	
	//RETAILMSG(CAM_INOUT,(TEXT("-------------------CameraSetScaler\r\n")));
}
