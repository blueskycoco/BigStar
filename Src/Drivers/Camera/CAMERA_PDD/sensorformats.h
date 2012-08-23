//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//

#include <mmreg.h>

#ifndef __SENSORFORMATS_H
#define __SENSORFORMATS_H

#ifndef mmioFOURCC    
#define mmioFOURCC( ch0, ch1, ch2, ch3 )          \
     ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |  \
     ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif  

#define BITRATE(DX,DY,DBITCOUNT,FRAMERATE)    (DX * abs(DY) * DBITCOUNT * FRAMERATE)
#define SAMPLESIZE(DX,DY,DBITCOUNT) (DX * abs(DY) * DBITCOUNT / 8)


#define REFTIME_30FPS 333333
#define REFTIME_15FPS 666666
#define REFTIME_3FPS  3333333

//
// FourCC of the YUV formats
// For information about FourCC, go to:
//     http://www.webartz.com/fourcc/indexyuv.htm
//     http://www.fourcc.org
//

#define FOURCC_UYVY     mmioFOURCC('U', 'Y', 'V', 'Y')  // MSYUV: 1394 conferencing camera 4:4:4 mode 1 and 3
#define FOURCC_YUY2     mmioFOURCC('Y', 'U', 'Y', '2')
#define FOURCC_YV12     mmioFOURCC('Y', 'V', '1', '2')
#define FOURCC_IJPG 	mmioFOURCC('I', 'J', 'P', 'G')

#define MEDIASUBTYPE_RGB565 {0xe436eb7b, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
#define MEDIASUBTYPE_RGB555 {0xe436eb7c, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
#define MEDIASUBTYPE_RGB24  {0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
#define MEDIASUBTYPE_RGB32  {0xe436eb7e, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}

#define MEDIASUBTYPE_YV12   {0x32315659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define MEDIASUBTYPE_YUY2   {0x32595559, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

#define MEDIASUBTYPE_IJPG 	{0x47504A49, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

#define MAKE_STREAM_MODE_YV12(StreamModeName, DX, DY, DBITCOUNT, FRAMERATE) \
    CS_DATARANGE_VIDEO StreamModeName =  \
    { \
        {    \
            sizeof (CS_DATARANGE_VIDEO),     \
            0, \
            SAMPLESIZE(DX,DY,DBITCOUNT),     \
            0,                               \
            STATIC_CSDATAFORMAT_TYPE_VIDEO,   \
            MEDIASUBTYPE_YV12, \
            STATIC_CSDATAFORMAT_SPECIFIER_VIDEOINFO \
        }, \
 \
        TRUE,                   \
        TRUE,                   \
        CS_VIDEOSTREAM_CAPTURE, \
        0,                      \
 \
        { \
            STATIC_CSDATAFORMAT_SPECIFIER_VIDEOINFO,  \
            CS_AnalogVideo_None, \
            DX,DY,    \
            DX,DY,    \
            DX,DY,    \
            1,        \
            1,        \
            1,        \
            1,        \
            DX, DY,   \
            DX, DY,   \
            DX,       \
            DY,       \
            0,        \
            0,        \
            0,        \
            0,        \
            REFTIME_##FRAMERATE##FPS,                      \
            REFTIME_##FRAMERATE##FPS,                      \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE) / 8,        \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE),            \
        },  \
 \
        { \
            0,0,DX,DY,                            \
            0,0,DX,DY,                            \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE), \
            0L,                                 \
            REFTIME_##FRAMERATE##FPS,                      \
            sizeof (CS_BITMAPINFOHEADER),       \
            DX,                                 \
            DY,                                 \
            3,                        \
            DBITCOUNT,                \
            FOURCC_YV12 | BI_SRCPREROTATE,      \
            SAMPLESIZE(DX,DY,DBITCOUNT), \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0, 0, 0                   \
        } \
    }; 

#define MAKE_STREAM_MODE_YUY2(StreamModeName, DX, DY, DBITCOUNT, FRAMERATE) \
    CS_DATARANGE_VIDEO StreamModeName =  \
    { \
        {    \
            sizeof (CS_DATARANGE_VIDEO),     \
            0, \
            SAMPLESIZE(DX,DY,DBITCOUNT),     \
            0,                               \
            STATIC_CSDATAFORMAT_TYPE_VIDEO,   \
            MEDIASUBTYPE_YUY2, \
            STATIC_CSDATAFORMAT_SPECIFIER_VIDEOINFO \
        }, \
 \
        TRUE,                   \
        TRUE,                   \
        CS_VIDEOSTREAM_CAPTURE, \
        0,                      \
 \
        { \
            STATIC_CSDATAFORMAT_SPECIFIER_VIDEOINFO,  \
            CS_AnalogVideo_None, \
            DX,DY,    \
            DX,DY,    \
            DX,DY,    \
            1,        \
            1,        \
            1,        \
            1,        \
            DX, DY,   \
            DX, DY,   \
            DX,       \
            DY,       \
            0,        \
            0,        \
            0,        \
            0,        \
            REFTIME_##FRAMERATE##FPS,                      \
            REFTIME_##FRAMERATE##FPS,                      \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE) / 8,        \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE),            \
        },  \
 \
        { \
            0,0,0,0,                            \
            0,0,0,0,                            \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE), \
            0L,                                 \
            REFTIME_##FRAMERATE##FPS,                      \
            sizeof (CS_BITMAPINFOHEADER),       \
            DX,                                 \
            DY,                                 \
            1,                        \
            DBITCOUNT,                \
            FOURCC_YUY2 | BI_SRCPREROTATE,      \
            SAMPLESIZE(DX,DY,DBITCOUNT), \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0, 0, 0                   \
        } \
    }; 

#define MAKE_STREAM_MODE_RGB565(StreamModeName, DX, DY, DBITCOUNT, FRAMERATE) \
    CS_DATARANGE_VIDEO StreamModeName =  \
    { \
        {    \
            sizeof (CS_DATARANGE_VIDEO),     \
            0, \
            SAMPLESIZE(DX,DY,DBITCOUNT),     \
            0,                               \
            STATIC_CSDATAFORMAT_TYPE_VIDEO,   \
            MEDIASUBTYPE_RGB565, \
            STATIC_CSDATAFORMAT_SPECIFIER_VIDEOINFO \
        }, \
 \
        TRUE,                   \
        TRUE,                   \
        CS_VIDEOSTREAM_CAPTURE, \
        0,                      \
 \
        { \
            STATIC_CSDATAFORMAT_SPECIFIER_VIDEOINFO,  \
            CS_AnalogVideo_None, \
            DX,DY,    \
            DX,DY,    \
            DX,DY,    \
            1,        \
            1,        \
            1,        \
            1,        \
            DX, DY,   \
            DX, DY,   \
            DX,       \
            DY,       \
            0,        \
            0,        \
            0,        \
            0,        \
            REFTIME_##FRAMERATE##FPS,                      \
            REFTIME_##FRAMERATE##FPS,                      \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE) / 4,        \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE),            \
        },  \
 \
        { \
            0,0,0,0,                            \
            0,0,0,0,                            \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE), \
            0L,                                 \
            REFTIME_##FRAMERATE##FPS,           \
            sizeof (CS_BITMAPINFOHEADER),       \
            DX,                                 \
            DY,                                 \
            1,                                  \
            DBITCOUNT,                          \
            CS_BI_BITFIELDS | BI_SRCPREROTATE,  \
            SAMPLESIZE(DX,DY,DBITCOUNT),        \
            0,                                  \
            0,                                  \
            0,                                  \
            0,                                  \
            0xf800, 0x07e0, 0x001f              \
        } \
    }; 




#define MAKE_STREAM_MODE_JPG(StreamModeName, DX, DY, DBITCOUNT, FRAMERATE) \
    CS_DATARANGE_VIDEO StreamModeName =  \
    { \
        {    \
            sizeof (CS_DATARANGE_VIDEO),     \
            0, \
            DX*DY,     \
            0,                               \
            STATIC_CSDATAFORMAT_TYPE_VIDEO,   \
            MEDIASUBTYPE_IJPG,   \
            STATIC_CSDATAFORMAT_SPECIFIER_VIDEOINFO \
        }, \
 \
        TRUE,                   \
        TRUE,                   \
        CS_VIDEOSTREAM_CAPTURE, \
        0,                      \
 \
        { \
            STATIC_CSDATAFORMAT_SPECIFIER_VIDEOINFO,  \
            CS_AnalogVideo_None, \
            DX,DY,    \
            DX,DY,    \
            DX,DY,    \
            1,        \
            1,        \
            1,        \
            1,        \
            DX, DY,   \
            DX, DY,   \
            DX,       \
            DY,       \
            0,        \
            0,        \
            0,        \
            0,        \
            REFTIME_##FRAMERATE##FPS,                      \
            REFTIME_##FRAMERATE##FPS,                      \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE) / 4,        \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE),            \
        },  \
 \
        { \
            0,0,0,0,                            \
            0,0,0,0,                            \
            BITRATE(DX,DY,DBITCOUNT,FRAMERATE), \
            0L,                                 \
            REFTIME_##FRAMERATE##FPS,           \
	            sizeof (CS_BITMAPINFOHEADER),       \
	            DX,                                 \
	            DY,                                 \
	            1,                                  \
	            DBITCOUNT,                          \
	            JPEG_DIB | BI_SRCPREROTATE,  \
	            SAMPLESIZE(DX,DY,DBITCOUNT),        \
	            0,                                  \
	            0,                                  \
	            0,                                  \
	            0,                                  \
	            0xf800, 0x07e0, 0x001f              \
        } \
    }; 





//kk 
MAKE_STREAM_MODE_RGB565(DCAM_StreamMode_0, 176, 144, 16, 15);
MAKE_STREAM_MODE_RGB565(DCAM_StreamMode_1, 176, 144, 16, 30);
MAKE_STREAM_MODE_RGB565(DCAM_StreamMode_2, 320, 240, 16, 15);
MAKE_STREAM_MODE_RGB565(DCAM_StreamMode_3, 320, 240, 16, 30);
MAKE_STREAM_MODE_RGB565(DCAM_StreamMode_4, 640, 480, 16, 15); //by kk imsi

MAKE_STREAM_MODE_YV12(DCAM_StreamMode_5, 160, -120, 12, 30);
MAKE_STREAM_MODE_YV12(DCAM_StreamMode_6, 176, -144, 12, 30);
MAKE_STREAM_MODE_YV12(DCAM_StreamMode_7, 320, -240, 12, 15);
MAKE_STREAM_MODE_YV12(DCAM_StreamMode_8, 352, -288, 12, 15);
MAKE_STREAM_MODE_YV12(DCAM_StreamMode_9, 640, -480, 12, 15);
MAKE_STREAM_MODE_YV12(DCAM_StreamMode_24, 800, -600, 12, 30);

MAKE_STREAM_MODE_YUY2(DCAM_StreamMode_10, 176, -144, 16, 15);
MAKE_STREAM_MODE_YUY2(DCAM_StreamMode_11, 320, -240, 16, 15);
MAKE_STREAM_MODE_YUY2(DCAM_StreamMode_12, 352, -288, 16, 15); 
MAKE_STREAM_MODE_YUY2(DCAM_StreamMode_13, 640, -480, 16, 15); 
MAKE_STREAM_MODE_YUY2(DCAM_StreamMode_14, 1280, -960, 16, 15); 
MAKE_STREAM_MODE_YUY2(DCAM_StreamMode_15, 1600, -1200, 16, 15); 
MAKE_STREAM_MODE_YUY2(DCAM_StreamMode_16, 2592, -1944, 16, 15); 

MAKE_STREAM_MODE_JPG(DCAM_StreamMode_17, 176, 144, 16, 15);
MAKE_STREAM_MODE_JPG(DCAM_StreamMode_18, 320, 240, 16, 15);
MAKE_STREAM_MODE_JPG(DCAM_StreamMode_19, 352, 288, 16, 15); 
MAKE_STREAM_MODE_JPG(DCAM_StreamMode_20, 640, 480, 16, 15); 
MAKE_STREAM_MODE_JPG(DCAM_StreamMode_21, 1280, 960, 16, 15); 
MAKE_STREAM_MODE_JPG(DCAM_StreamMode_22, 1600, 1200, 16, 15); 
MAKE_STREAM_MODE_JPG(DCAM_StreamMode_23, 2592, 1944, 16, 3); 
#endif //__SENSORFORMATS_H
