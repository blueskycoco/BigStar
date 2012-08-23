/********************************************************************
 *                 Lenovo System Event
 *
 * Copyright (c) Lenovo Mobile Limited.  All rights reserved.
 *
 * 名称：SysEvent.h
 * 
 * 功能：定义系统和应用程序间交互的事件、消息、注册表、宏和函数
 *
 * 作者：张乃雷
 *
 * 版本：1.3
 *
 * 历史：2006-06-28  版本0.1。创建 by 张乃雷
 *		 2006-06-30  版本0.2。整理 by 张乃雷
 *		 2006-07-03  版本0.3。整理 by 张乃雷
 *		 2006-07-04  版本0.4。整理 by 张乃雷
 *		 2006-07-05  版本0.5。整理 by 张乃雷
 *		 2006-07-21  版本0.6。整理 by 张乃雷
 *		 2006-08-01  版本0.7。整理 by 张乃雷
 *		 2006-08-02  版本0.8。整理 by 张乃雷
 *		 2006-08-07  版本0.9。整理 by 张乃雷
 *		 2006-09-01  版本1.0。整理 by 张乃雷
 *		 2006-10-03  版本1.1。整理 by 张乃雷
 *		 2006-11-07  版本1.2。整理 by 张乃雷
 *		 2006-11-23  版本1.3。整理 by 张乃雷
 *
 ********************************************************************/
 
/**********************************************************************************************
   注意事项：
  
   1、本文件的使用者需遵循API优先原则，可使用API的情况下不允许和驱动直接交互；
   2、因为使用者较多，本文件暂时不纳入到版本控制系统中，采用文件共享方式进行修改，所以请不要使用
   	  Notepad进行修改，最好使用UltraEdit等有文件修改警告的编辑软件修改，发生冲突时及时协商，避免
   	  覆盖别人的工作成果。
 ***********************************************************************************************/

#pragma once

#ifndef _SYSEVENT_H
#define _SYSEVENT_H

#ifdef IAMCAMERADRIVER	//Camera Driver请在程序中定义
#include "LeDShowLib_h.h"
#endif

#if __cplusplus
    extern "C"
    {
#endif
 
//--------------------------------------------//
//  General：
//--------------------------------------------//

//---Event---//

#define SYS_POWER_ON			L"SYS_POWER_ON"		//系统开机
 
//--------------------------------------------//
//  CPU：
//--------------------------------------------//

//---Function---//

//请求高性能工作模式
#define	IOCTL_REQUEST_HIGHFRE	CTL_CODE(FILE_DEVICE_HAL, 4015, METHOD_BUFFERED, FILE_ANY_ACCESS)

//取消高性能工作模式
#define	IOCTL_RELEASE_HIGHFRE	CTL_CODE(FILE_DEVICE_HAL, 4016, METHOD_BUFFERED, FILE_ANY_ACCESS)

//--------------------------------------------//
//  Backlight：
//--------------------------------------------//

//注意，Backlight的常规范围为10-100，但Camera应用可以将其调至120

//---Function---//

//设置Backlight亮度
#define	IOCTL_SET_BACKLIGHT_FUNC		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4000, METHOD_BUFFERED, FILE_ANY_ACCESS)

//获取Backlight亮度
#define	IOCTL_GET_BACKLIGHT_FUNC		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4001, METHOD_BUFFERED, FILE_ANY_ACCESS)

//--------------------------------------------//
//  USB：
//--------------------------------------------//

//---Macro---//

#define USB_WORK_MODE_NETCARD	1	//USB处于网卡工作模式
#define USB_WORK_MODE_MOVECARD	0	//USB处于移动存储模式

//---Function---//

//设置USB工作模式
#define	IOCTL_SET_USB_FUNC		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0202, METHOD_BUFFERED, FILE_ANY_ACCESS)

//获取USB工作模式
#define	IOCTL_GET_USB_FUNC		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0203, METHOD_BUFFERED, FILE_ANY_ACCESS)

//--------------------------------------------//
//  RIL：
//--------------------------------------------//

//---Reg---//

//STKMainName为STK 0级菜单名，RIL需在SIM卡加电时写入STK 0级菜单名，下电时设置为空。
//以下键值在HKEY_LOCAL_MACHINE下使用。
#define RIL_REGKEY_STK_MAINNAME			L"System\\State\\Phone"
#define RIL_REGVALUE_STRING_MAINNAME	L"STKMainName"

//--------------------------------------------//
//  Button：
//--------------------------------------------//

//---Event---//

#define BUTTON_SEND					L"BUTTON_Send"				//拨号键按下
#define BUTTON_END					L"BUTTON_End"				//挂断键按下

#define BUTTON1_HALFDOWN			L"BUTTON1_Halfdown"			//二级按键1半按
#define BUTTON1_HALFUP				L"BUTTON1_Halfup"			//二级按键1抬起
#define BUTTON2_HALFDOWN			L"BUTTON2_Halfdown"			//二级按键2半按
#define BUTTON2_HALFUP				L"BUTTON2_Halfup"			//二级按键2抬起

#define BUTTON_HOLD_HOLD			L"BUTTON_Hold_Hold"			//Hold键生效
#define BUTTON_HOLD_RELEASE			L"BUTTON_Hold_Release"		//Hold键释放

#define BUTTON_MICCLEAR_DOWN		L"BUTTON_MicClear_Down"		//清音键按下
#define BUTTON_MICCLEAR_HOLD_ON		L"BUTTON_MicClear_Hold_On"	//清音键长按

#define BUTTON_FN_HOLD_DOWN			L"BUTTON_Fn_Hold"			//功能键按下

#define BUTTON_MUSIC_PLAY_PAUSE_UP		L"BUTTON_Music_Play_Pause_Up"		//音乐键－播放/暂停抬起
#define BUTTON_MUSIC_PREVIOUS_UP		L"BUTTON_Music_Previous_Up"			//音乐键－上一首抬起
#define BUTTON_MUSIC_NEXT_UP			L"BUTTON_Music_Next_Up"				//音乐键－下一首抬起
#define BUTTON_MUSIC_PLAY_PAUSE_DOWN	L"BUTTON_Music_Play_Pause_Down"		//音乐键－播放/暂停按下
#define BUTTON_MUSIC_PREVIOUS_DOWN		L"BUTTON_Music_Previous_Down"		//音乐键－上一首按下
#define BUTTON_MUSIC_NEXT_DOWN			L"BUTTON_Music_Next_Down"			//音乐键－下一首按下

//---Reg---//
//以下键值用于定义音乐按键在设备锁定后是否仍可使用，相应的控制面板在打开时，需要监测此项的变化，以做出相应变化
#define REG_APP_LEMUSIC				L"Software\\Lenovo\\LeMusic"
#define REG_VALUE_LEMUSIC_KEYLOCK	L"bMusicKeyLockEnable"	//DWORD

//--------------------------------------------//
//  Earphone：
//--------------------------------------------//

//---Event---//

#define	EARPHONE_IN				L"EARPHONE_In"		//耳机插上
#define	EARPHONE_OUT			L"EARPHONE_Out"		//耳机拔出

//--------------------------------------------//
//  APFService：
//--------------------------------------------//

//---Event---//

#define	APF_HANDSET				L"APF_Handset"		//听筒模式
#define	APF_HANDSFREE			L"APF_HandsFree"	//免提模式
#define	APF_EARPHONE_IN			L"APF_Earphone_In"	//听筒模式
#define	APF_EARPHONE_OUT		L"APF_Earphone_Out"	//免提模式

//--------------------------------------------//
//  WAVDEV：
//--------------------------------------------//

//---Macro---//

#define MIC_CLEARMODE_DISABLE	0	//清音模式开启
#define MIC_CLEARMODE_ENABLE	1	//清音模式关闭

#define WAVDEV_EQ_ORIGIN		0	//EQ模式－原声
#define WAVDEV_EQ_POP			1	//EQ模式－流行音乐
#define WAVDEV_EQ_OPERA			2	//EQ模式－剧院
#define WAVDEV_EQ_BASS			3	//EQ模式－重低音
#define WAVDEV_EQ_MONO			4	//EQ模式－单声道

//---Event---//

#define	WAVDEV_HANDSET			L"WAVDEV_Handset"		//听筒模式
#define	WAVDEV_HANDSFREE		L"WAVDEV_HandsFree"		//免提模式

//---Function---//

#if __cplusplus
typedef struct MicWorkMode
{
	BOOL m_bPhoneMode;
	BOOL m_bRecMode;
	/*
	MicWorkMode()
	{
		m_bPhoneMode = FALSE;
		m_bRecMode = FALSE;
	}
	*/
} T_WAVE_MIC_WORK_MODE;
#endif


//设置清音工作模式
#define	IOCTL_SET_MIC_CLEARMODE		CTL_CODE(FILE_DEVICE_HAL, 0x0100, METHOD_BUFFERED, FILE_ANY_ACCESS)

//获取清音工作模式
#define	IOCTL_GET_MIC_CLEARMODE		CTL_CODE(FILE_DEVICE_HAL, 0x0200, METHOD_BUFFERED, FILE_ANY_ACCESS)

//设置MIC在电话和录音时的默认工作模式
#define IOCTL_SET_MIC_WORKMODE		CTL_CODE(FILE_DEVICE_HAL, 0x0101, METHOD_BUFFERED, FILE_ANY_ACCESS)

//设置EQ工作模式
#define	IOCTL_SET_WAVDEV_EQ		CTL_CODE(FILE_DEVICE_HAL, 0x0300, METHOD_BUFFERED, FILE_ANY_ACCESS)

//获取EQ工作模式
#define	IOCTL_GET_WAVDEV_EQ		CTL_CODE(FILE_DEVICE_HAL, 0x0400, METHOD_BUFFERED, FILE_ANY_ACCESS)

//允许或禁止MIC的清音模式
#define IOCTL_ENABLE_MIC_CLEARMODE		CTL_CODE(FILE_DEVICE_HAL, 0x0102, METHOD_BUFFERED, FILE_ANY_ACCESS)

//--------------------------------------------//
//  MSSIM：
//--------------------------------------------//


//--------------------------------------------//
//  Camera：
//--------------------------------------------//

//---Struct---//

#if __cplusplus
typedef struct CAMColorFormat
{
	TCHAR szYCbCr444[20];
	TCHAR szYCbCr422[20];
	TCHAR szYCbCr420[20];
	TCHAR szRGB444[20];
	TCHAR szRGB565[20];
	TCHAR szRGB666[20];
	TCHAR szRGB888[20];

	CAMColorFormat()
	{
		_tcscpy(szYCbCr444, L"YCbCr444");
		_tcscpy(szYCbCr422, L"YCbCr422");
		_tcscpy(szYCbCr420, L"YCbCr420");
		_tcscpy(szRGB444, L"RGB444");
		_tcscpy(szRGB565, L"RGB565");
		_tcscpy(szRGB666, L"RGB666");
		_tcscpy(szRGB888, L"RGB888");
	}

}CAMColorFormat;
#endif

//---Macro---//

/*
// Ref: DShow Camera driver define the macros
typedef enum {
    // VideoProcAmp
    ENUM_BRIGHTNESS = 0,
    ENUM_CONTRAST,
    ENUM_HUE,
    ENUM_SATURATION,
    ENUM_SHARPNESS,
    ENUM_GAMMA,
    ENUM_COLORENABLE,
    ENUM_WHITEBALANCE,
    ENUM_BACKLIGHT_COMPENSATION,
    ENUM_GAIN,

    // CameraControl
    ENUM_PAN, //0
    ENUM_TILT, //1
    ENUM_ROLL, //2
    ENUM_ZOOM, //3
    ENUM_IRIS, //4
    ENUM_EXPOSURE, //5
    ENUM_FOCUS, //6
    ENUM_FLASH, //7

    // Lenovo extend from here
    
    ENUM_EFFECT, //8
    ENUM_STILL_QUALITY, //9
    ENUM_GETDATADIRECTCONTROL, //10
    ENUM_STILLFRAMEREQUEST, //11
    ENUM_PREVIEWFRAMEREQUEST, //12
    ENUM_HW_FEATRUE //13
    
} ENUM_DEV_PROP;
*/

/******************* CAM_DSHOW_DIGITALZOOM_CTLCODE **********************/
#define CAM_DSHOW_DIGITALZOOM_CTLCODE	3		//数字变焦控制
#define CAM_DIGITALZOOM_MIN				1		//最小数字变焦范围 - 1倍
#define CAM_DIGITALZOOM_MAX				32		//最大数字变焦范围 - 32倍

/******************* CAM_DSHOW_FOCUS_CTLCODE ****************************/
#define CAM_DSHOW_FOCUS_CTLCODE	6		//对焦控制
#define CAM_FOCUS_DISABLE		0		//对焦控制关闭
#define CAM_FOCUS_ENABLE		1		//对焦控制开启
#define CAM_FOCUS_AUTO			2		//进入自动对焦模式
#define CAM_FOCUS_AUTO_START	3		//开始自动对焦的动作
#define CAM_FOCUS_AUTO_STOP		4		//结束自动对焦的动作
#define CAM_FOCUS_MICRO			5		//进入微距模式

/******************* CAM_DSHOW_FLASH_CTLCODE ****************************/
#define CAM_DSHOW_FLASH_CTLCODE	7		//闪光灯控制
#define CAM_FLASH_DISABLE		0		//闪光灯关闭
#define CAM_FLASH_ENABLE		1		//闪光灯亮
#define CAM_FLASH_ALWAYS_ON		2		//闪光灯常亮开启
#define CAM_FLASH_ALWAYS_OFF	3		//闪光灯常亮关闭

/******************* CAM_EXTEND_EFFECT_CTLCODE **************************/
#define	CAM_EXTEND_EFFECT_CTLCODE	8

#define CAM_EFFECT_MISC_ID			0x01	//杂项
#define CAM_CAPTURE_ROTATE_DISABLE	0		//关闭录像视频数据旋转
#define CAM_CAPTURE_ROTATE_ENABLE	1		//打开录像视频数据旋转

#define CAM_EFFECT_SCENE_ID			0x02		//设置场景模式
#define CAM_SCENE_COMMON			0x101		//场景模式－普通
#define CAM_SCENE_PORTRAIT			0x102		//场景模式－人像
#define CAM_SCENE_LANDSCAPE			0x103		//场景模式－风景
#define CAM_SCENE_MOVE				0x104		//场景模式－运动
#define CAM_SCENE_MICRO				0x105		//场景模式－微距
#define CAM_SCENE_INDOOR			0x106		//场景模式－室内
#define CAM_SCENE_OUTDOOR			0x107		//场景模式－室外
#define CAM_SCENE_BACKLIGHTING		0x108		//场景模式－逆光
#define CAM_SCENE_DAY				0x109		//场景模式－白天
#define CAM_SCENE_DAWN				0x10A		//场景模式－黎明
#define CAM_SCENE_FORENOON			0x10B		//场景模式－上午
#define CAM_SCENE_NOON				0x10C		//场景模式－中午
#define CAM_SCENE_AFTERNOON			0x10D		//场景模式－下午
#define CAM_SCENE_SUNDOWN			0x10E		//场景模式－日落
#define CAM_SCENE_DUSK				0x10F		//场景模式－黄昏
#define CAM_SCENE_NIGHT				0x110		//场景模式－夜景
#define CAM_SCENE_SNOW				0x111		//场景模式－雪景
#define CAM_SCENE_NEEDFIRE			0x112		//场景模式－篝火

#define CAM_EFFECT_WB_ID			0x03		//设置白平衡
#define CAM_WB_AUTO					0x201		//白平衡－自动
#define CAM_WB_TUNGSTENLAMP			0x202		//白平衡－钨光灯
#define CAM_WB_FLUORESCENTLAMP		0x203		//白平衡－荧光灯
#define CAM_WB_FLASHLAMP			0x204		//白平衡－闪光灯
#define CAM_WB_DAYLIGHT				0x205		//白平衡－日光
#define CAM_WB_FINEDAY				0x206		//白平衡－晴天
#define CAM_WB_CLOUDY				0x207		//白平衡－多云
#define CAM_WB_SHADOW				0x208		//白平衡－阴影
#define CAM_WB_NIGHT				0x209		//白平衡－夜晚
#define CAM_WB_STARLIGHT			0x20A		//白平衡－星光

#define CAM_EFFECT_COLOR_ID			0x04		//设置色彩模式
#define CAM_COLOR_SEPIA				0x301		//色彩模式－怀旧
#define CAM_COLOR_BLACK_WHITE		0x302		//色彩模式－黑白
#define CAM_COLOR_COLOR				0x303		//色彩模式－彩色
#define CAM_COLOR_VIVID				0x304		//色彩模式－鲜艳
#define CAM_COLOR_NEGATIVE			0x305		//色彩模式－负片
#define CAM_COLOR_WARM				0x306		//色彩模式－暖光
#define CAM_COLOR_COLD				0x307		//色彩模式－冷光
#define CAM_COLOR_FILM				0x308		//色彩模式－胶片
#define CAM_COLOR_RELIEVO			0x309		//色彩模式－浮雕
#define CAM_COLOR_WATERPOWDER		0x30A		//色彩模式－粉彩

#define CAM_EFFECT_ISO_ID			0x05		//设置ISO模式
#define CAM_ISO_AUTO				0x401		//ISO－自动
#define CAM_ISO_64					0x402		//ISO－64
#define CAM_ISO_100					0x403		//ISO－100
#define CAM_ISO_200					0x404		//ISO－200
#define CAM_ISO_400					0x405		//ISO－400
#define CAM_ISO_800					0x406		//ISO－800
#define CAM_ISO_1000				0x407		//ISO－1000
#define CAM_ISO_1600				0x408		//ISO－1600

#define CAM_EFFECT_PHOTOMETRY_ID	0x06		//设置测光方式
#define CAM_PHOTOMETRY_MULTIPLE		0x501		//测光方式－多重测光
#define CAM_PHOTOMETRY_CENTRE		0x502		//测光方式－偏重中央测光
#define CAM_PHOTOMETRY_POINT		0x503		//测光方式－定点测光
#define CAM_PHOTOMETRY_AFREGION		0x504		//测光方式－AF区测光

#define CAM_EFFECT_EXPOSURE_ID		0x07		//设置曝光方式
#define CAM_EXPOSURE_AUTO			0x601		//曝光方式－自动
#define CAM_EXPOSURE_PROGRAMAUTO	0x602		//曝光方式－程序自动
#define CAM_EXPOSURE_SHUTTERFIRST	0x603		//曝光方式－快门优先
#define CAM_EXPOSURE_APERTUREFIRST	0x604		//曝光方式－光圈优先
#define CAM_EXPOSURE_MANUAL			0x605		//曝光方式－手动曝光
#define CAM_EXPOSURE_SCENE			0x606		//曝光方式－情景曝光

#define CAM_EFFECT_LENS_ID			0x08		//设置镜头类型
#define CAM_LENS_GENERAL			0x702		//镜头类型－普通
#define CAM_LENS_WIDEANGLE			0x703		//镜头类型－广角
#define CAM_LENS_SOFTLIGHT			0x704		//镜头类型－柔光
#define CAM_LENS_MICRO				0x705		//镜头类型－微距

#define CAM_EFFECT_SHUTTER_ID		0x09		//设置快门速度范围
#define CAM_SHUTTER_MIN				1			//最高快门速度 - 1
#define CAM_SHUTTER_MAX				8000		//最低快门速度 - 8000

#define CAM_EFFECT_GAIN_GLOBE_ID	0x10		//设置全局增益范围
#define CAM_GAIN_GLOBE_MIN			1			//最低全局增益 - 1
#define CAM_GAIN_GLOBE_MAX			256			//最高全局增益 - 256

#define CAM_EFFECT_GAIN_RED_ID		0x11		//设置红色增益范围
#define CAM_GAIN_RED_MIN			1			//最低红色增益 - 1
#define CAM_GAIN_RED_MAX			256			//最高红色增益 - 256

#define CAM_EFFECT_GAIN_GREEN_ID	0x12		//设置绿色增益范围
#define CAM_GAIN_GREEN_MIN			1			//最低绿色增益 - 1
#define CAM_GAIN_GREEN_MAX			256			//最高绿色增益 - 256

#define CAM_EFFECT_GAIN_BLUE_ID		0x13		//设置蓝色增益范围
#define CAM_GAIN_BLUE_MIN			1			//最低蓝色增益 - 1
#define CAM_GAIN_BLUE_MAX			256			//最高蓝色增益 - 256

/******************* CAM_EXTEND_STILL_QUALITY_CTLCODE **********************************/
#define CAM_EXTEND_STILL_QUALITY_CTLCODE	9	//设置照片质量
#define CAM_STILL_QUALITY_MIN				0	//最低照片质量 - 0
#define CAM_STILL_QUALITY_MAX				100	//最高照片质量 - 100

/******************* CAM_EXTEND_GET_DATA_DIRECT_CTLCODE ********************************/
#define CAM_EXTEND_GET_DATA_DIRECT_CTLCODE	10
#define CAM_DSHOW_PREVIEWFRAMEDATA_DISABLE	0	//关闭DirectShow Preview PIN帧数据
#define CAM_DSHOW_PREVIEWFRAMEDATA_ENABLE	1	//输出DirectShow Preview PIN帧数据
#define CAM_GET_STILLFRAMEDATA_DISABLE		2	//关闭直接获取Still PIN帧数据
#define CAM_GET_STILLFRAMEDATA_ENABLE		3	//打开直接获取Still PIN帧数据
#define CAM_STILL_CONTINUOUS_DISABLE				4	//关闭Still PIN帧数据连续输出模式
#define CAM_STILL_CONTINUOUS_ENABLE				5	//打开Still PIN帧数据连续输出模式

/******************* CAM_EXTEND_GET_PREVIEWFRAMEDATA_CTLCODE ***************************/
#define CAM_EXTEND_GET_PREVIEWFRAMEDATA_CTLCODE		12	//获取Preview PIN帧数据

/******************* CAM_EXTEND_GET_STILLFRAMEDATA_CTLCODE *****************************/
#define CAM_EXTEND_GET_STILLFRAMEDATA_CTLCODE		11	//获取Still PIN帧数据

/******************* CAM_EXTEND_HW_FEATURE_CTLCODE **********************/
#define CAM_EXTEND_HW_FEATURE_CTLCODE		13
#define CAM_OIS_DISABLE		0	//关闭光学防抖动
#define CAM_OIS_ENABLE		1	//打开光学防抖动

/******************* CAM_EXTEND_WORK_MODE_CTLCODE **********************/
#define CAM_EXTEND_WORK_MODE_CTLCODE		14
#define CAM_WORK_MODE_FOREGROUND		0	//相机在前台,正常模式
#define CAM_WORK_MODE_BACKGROUND		1	//相机在后台,省电模式


//---Reg---//

//InColor和OutColor中请填入struct CAMColorFormat所定义的值，InColor代表Camera Driver所获得的数据的颜色格式，在DShow的
//Pin Color Format与数据源颜色格式不一致的情况下根据InColor的设定值用LeDShowLib中的ILeImageProcess接口进行颜色数据转换；
//OutColor代表Camera Driver的GetFrameData接口输出时的颜色格式，必要时请使用LeDShowLib中的ILeImageProcess接口进行颜色数据转换。
//以下键值在HKEY_LOCAL_MACHINE下使用。
#define CAM1_REGKEY_COLOR_FORMAT		L"Software\\Microsoft\\DirectX\\DirectShow\\Color Format\\CAM1"
#define CAM2_REGKEY_COLOR_FORMAT		L"Software\\Microsoft\\DirectX\\DirectShow\\Color Format\\CAM2"
#define CAM_REGVALUE_STRING_INCOLOR		L"InColor"
#define CAM_REGVALUE_STRING_OUTCOLOR	L"OutColor"

//当绕过DirectShow保存Jpeg文件时，请使用以下键值。
//以下键值在HKEY_LOCAL_MACHINE下使用。
#define CAM1_FILE_PATH	L"Software\\Lenovo\\Camera\\CAM1\\JPGFILE"
#define CAM2_FILE_PATH	L"Software\\Lenovo\\Camera\\CAM2\\JPGFILE"
#define CAM_FILE_NAME	L"JpgFileName"

//---Event---//

#define CAM1_PRESTILL					L"CAM1_PreStill"					//摄像头1预备拍照
#define CAM2_PRESTILL					L"CAM2_PreStill"					//摄像头2预备拍照
#define CAM1_STILL_CONTINUOUS_SUCCESS	L"CAM1_StillContinuous_Success"		//摄像头1连拍成功
#define CAM1_STILL_CONTINUOUS_FAIL		L"CAM1_StillContinuous_Fail"		//摄像头1连拍失败
#define CAM2_STILL_CONTINUOUS_SUCCESS	L"CAM2_StillContinuous_Success"		//摄像头2连拍成功
#define CAM2_STILL_CONTINUOUS_FAIL		L"CAM2_StillContinuous_Fail"		//摄像头2连拍失败
#define CAM1_AUTOFOCUS_START			L"CAM1_AutoFocus_Start"				//摄像头1自动对焦开始
#define CAM1_AUTOFOCUS_SUCCESS			L"CAM1_AutoFocus_Success"			//摄像头1自动对焦成功
#define CAM1_AUTOFOCUS_FAIL				L"CAM1_AutoFocus_Fail"				//摄像头1自动对焦失败
#define CAM2_AUTOFOCUS_START			L"CAM2_AutoFocus_Start"				//摄像头2自动对焦开始
#define CAM2_AUTOFOCUS_SUCCESS			L"CAM2_AutoFocus_Success"			//摄像头2自动对焦成功
#define CAM2_AUTOFOCUS_FAIL				L"CAM2_AutoFocus_Fail"				//摄像头2自动对焦失败
#define CAM1_OPTICALZOOM_START			L"CAM1_OpticalZoom_Start"			//摄像头1光学变焦开始
#define CAM1_OPTICALZOOM_END			L"CAM1_OpticalZoom_End"				//摄像头1光学变焦完成
#define CAM2_OPTICALZOOM_START			L"CAM2_OpticalZoom_Start"			//摄像头2光学变焦开始
#define CAM2_OPTICALZOOM_END			L"CAM2_OpticalZoom_End"				//摄像头2光学变焦完成

#if __cplusplus
	}
#endif

#endif//endif _SYSEVENT_H