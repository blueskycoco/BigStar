#ifndef _MODULE_H
#define _MODULE_H

typedef enum
{
	OV5642_ARRAY_INVALID,
	OV5642_ARRAY_QXGA,
	OV5642_ARRAY_VGA,
	OV5642_ARRAY_XGA,
	OV5642_ARRAY_SXGA,
	OV5642_ARRAY_QSXGA
}OV5642_ARRAY;

typedef enum
{
	STREAM_PREVIEW	    = 0x01,
	STREAM_VIDEOCAPTURE = 0x02,
	STREAM_STILLIMAGE = 0x04,
	STREAM_IDLE	= 0xF8
}STREAM_MODE;

typedef struct _MODULE_DESCRIPTOR
{
	BYTE ITUXXX;			// if ITU-601 8bit, set 1. if ITU-656 8bit, set 0
	BYTE UVOffset;			// Cb, Cr value offset. 1: +128 , 0: 0
	BYTE Order422; 			// 0x00:YCbYCr, 0x01:YCrYCb, 0x10:CbYCrY, 0x11:CrYCbY
	BYTE Codec;				// 422: 1   , 420: 0
	BYTE HighRst;			// Reset is    Low->High: 0   High->Low: 1 
	BYTE InvPCLK;			// 1: inverse the polarity of PCLK    0 : normal
	BYTE InvVSYNC;			// 1: inverse the polarity of VSYNC   0 : normal
	BYTE InvHREF;  			// 1: inverse the polarity of HREF	  0 : normal
	UINT32 SourceHSize; 	// Horizontal size
	UINT32 SourceVSize;		// Vertical size
	UINT32 SourceHOffset; 	// Horizontal size
	UINT32 SourceVOffset;	// Vertical size	
	UINT32 Clock;			// clock
	OV5642_ARRAY Ov5642ArrayMode;
	STREAM_MODE	 Ov5642StreamMode;
	UINT32 CurZoom;
} MODULE_DESCRIPTOR;


#define CAM_ITU601						(1)
#define CAM_ITU656						(0)

#define CAM_ORDER_YCBYCR				(0)
#define CAM_ORDER_YCRYCB				(1)
#define CAM_ORDER_CBYCRY				(2)
#define CAM_ORDER_CRYCBY				(3)

#define	CAM_UVOFFSET_0					(0)
#define	CAM_UVOFFSET_128				(1)

#define CAM_CODEC_422					(1)
#define CAM_CODEC_420					(0)

typedef enum
{
	QCIF, CIF/*352x288*/, 
	QQVGA, QVGA, VGA, SVGA/*800x600*/, SXGA/*1280x1024*/, UXGA/*1600x1200*/, QXGA/*2048x1536*/,QSXGA,/*2592*1944*/
	WVGA/*854x480*/, HD720/*1280x720*/, HD1080/*1920x1080*/,
	SUB_SAMPLING2/*800x600*/, SUB_SAMPLING4/*400x300*/
} IMG_SIZE;


int 		ModuleInit();
int 		Module1Init();
void 		ModuleDeinit();
void 		ModuleGetFormat(MODULE_DESCRIPTOR &outModuleDesc);
BOOL        Module1ArmGo();
BOOL        Module1PreviewEnable(int Format);
BOOL        Module1CaptureMode();
BOOL        Module1SetImageSize(UINT16 nWidth, UINT16 nHeight);

#endif
