#ifndef _SMDK6410CAMERA_H
#define _SMDK6410CAMERA_H

#undef RETAILMSG
#define RETAILMSG(cond,printf_exp)   ((cond)?(NKDbgPrintfW printf_exp),1:0)	//((void)0)

#define CAM_GPIO_PORT 		eGPIO_K
#define CAM_3M_CS	 		eGPIO_4
#define CAM_3M_RST	 		eGPIO_13
#define CAM_3M_PWR	 		eGPIO_14

#define CAM_100K_CS	 		eGPIO_5
#define CAM_100K_GPIO_PORT 	eGPIO_F
#define CAM_100K_RST	 	eGPIO_3
#define CAM_100K_PWR	 	eGPIO_13

#define	VIDEO_CAPTURE_BUFFER 0x1
#define	STILL_CAPTURE_BUFFER 0x2
#define	PREVIEW_CAPTURE_BUFFER 0x3

#define OUTPUT_CODEC_YCBCR422	0x1
#define OUTPUT_CODEC_YCBCR420	0x2
#define OUTPUT_CODEC_RGB16		0x3
#define OUTPUT_CODEC_RGB24		0x4
#define OUTPUT_CODEC_JPEG		0x5

#define PREVIEW_PATH		0x1
#define CODEC_PATH			0x2

#define MAX_HW_FRAMES 	4
//by shlee
//#define CAPTURE_BUFFER_SIZE		6291456//10077696//		//  ( 2592*1944*2)
#define CAPTURE_BUFFER_SIZE		10077696//15116544//10077696//mf 3145728 // mf 2880000 //2457600//2880000		//  ( 1280*960*2 )

//OV3640 jpg output feature: width is fixed to 1024, but height is variable.
//but 6410 CAMIF assumes the following JPG_OUT_WIDTH*JPG_OUT_HEIGHT(must >actural width*height from sensor) YUV422 data received
//works fine. 
#define JPG_OUT_WIDTH		1024
#define JPG_OUT_HEIGHT		1024// 假设的sensor JPG output 时的max height,  
#define JPG_OUT_SIZE		JPG_OUT_WIDTH*JPG_OUT_HEIGHT*3

//kk 
#define PREVIEW_BUFFER_SIZE		 2880000//2457600		//  ( 640*480*2  )  * MAX_HW_FRAMES
//#define PREVIEW_BUFFER_SIZE		614400		//  ( 320*240*2  )  * MAX_HW_FRAMES

typedef struct
{
        DWORD VirtAddr;         
        int   size;             // size of the buffer
        int   BufferID;         // a buffer ID used to identify this buffer to the driver
        union{
        	DWORD *pY;
        	DWORD *pRGB;
        };
        DWORD *pCb;
        DWORD *pCr;
//        DWORD *pBuf;            // Address of the DMA buffer returned from a call to malloc().
} CAMERA_DMA_BUFFER_INFO, *P_CAMERA_DMA_BUFFER_INFO;

// structures
typedef struct {
	UINT32	Width;
	UINT32	Height;
	int		Format;
	int 	Size;
	int		FrameSize;
	UINT32	RealImageWidth;		//only for still pin when jpg format
	UINT32	RealImageHeight;	//only for still pin when jpg format
} BUFFER_DESC;

typedef enum
{
	SENSOR_TYPE_OV3640 = 1,
	SENSOR_TYPE_NOON30 = 2
}SENSOR_TYPE;



typedef void (*PFNCAMHANDLEFRAME)( DWORD dwContext );

int 	CameraInit(void *pData);
void 	CameraDeinit();
int 	CameraPrepareBuffer(P_CAMERA_DMA_BUFFER_INFO pBufInfo, int BufferType);
int 	CameraSetFormat(UINT32 width, UINT32 height, int Format, int BufferType);
int 	CameraDeallocateBuffer(int BufferType);
int		CameraGetCurrentFrameNum(int BufferType);
int 	CameraCaptureControl(int Format, BOOL on); 
int 	CameraSetRegisters(int Format); 
void	CameraClockOn(BOOL bOnOff);	// Clock on/off
void	CameraSleep(DWORD dwDeviceIndex);
void	CameraResume(DWORD dwDeviceIndex);
void    CameraPreviewRecovery();
void    CameraCaptureMode();
BOOL	CameraBrightness(int value);
BOOL	CameraWhitebalance(int value);
BOOL	CameraSaturation(int value);
BOOL	CameraContrast(int value);
BOOL	CameraZoom(int value);
BOOL    CameraExposure(int value);
BOOL    CameraFocus(int value);
BOOL    CameraFlash(int value);
BOOL    CameraScene(int value);
void    CameraCaptureSourceSet();
void    CameraModuleSetImageSize(UINT16 uiWidth, UINT16 uiHeight);
#endif
