/********************************************************************
 *                 Lenovo System Event
 *
 * Copyright (c) Lenovo Mobile Limited.  All rights reserved.
 *
 * ���ƣ�SysEvent.h
 * 
 * ���ܣ�����ϵͳ��Ӧ�ó���佻�����¼�����Ϣ��ע�����ͺ���
 *
 * ���ߣ�������
 *
 * �汾��1.3
 *
 * ��ʷ��2006-06-28  �汾0.1������ by ������
 *		 2006-06-30  �汾0.2������ by ������
 *		 2006-07-03  �汾0.3������ by ������
 *		 2006-07-04  �汾0.4������ by ������
 *		 2006-07-05  �汾0.5������ by ������
 *		 2006-07-21  �汾0.6������ by ������
 *		 2006-08-01  �汾0.7������ by ������
 *		 2006-08-02  �汾0.8������ by ������
 *		 2006-08-07  �汾0.9������ by ������
 *		 2006-09-01  �汾1.0������ by ������
 *		 2006-10-03  �汾1.1������ by ������
 *		 2006-11-07  �汾1.2������ by ������
 *		 2006-11-23  �汾1.3������ by ������
 *
 ********************************************************************/
 
/**********************************************************************************************
   ע�����
  
   1�����ļ���ʹ��������ѭAPI����ԭ�򣬿�ʹ��API������²����������ֱ�ӽ�����
   2����Ϊʹ���߽϶࣬���ļ���ʱ�����뵽�汾����ϵͳ�У������ļ�����ʽ�����޸ģ������벻Ҫʹ��
   	  Notepad�����޸ģ����ʹ��UltraEdit�����ļ��޸ľ���ı༭����޸ģ�������ͻʱ��ʱЭ�̣�����
   	  ���Ǳ��˵Ĺ����ɹ���
 ***********************************************************************************************/

#pragma once

#ifndef _SYSEVENT_H
#define _SYSEVENT_H

#ifdef IAMCAMERADRIVER	//Camera Driver���ڳ����ж���
#include "LeDShowLib_h.h"
#endif

#if __cplusplus
    extern "C"
    {
#endif
 
//--------------------------------------------//
//  General��
//--------------------------------------------//

//---Event---//

#define SYS_POWER_ON			L"SYS_POWER_ON"		//ϵͳ����
 
//--------------------------------------------//
//  CPU��
//--------------------------------------------//

//---Function---//

//��������ܹ���ģʽ
#define	IOCTL_REQUEST_HIGHFRE	CTL_CODE(FILE_DEVICE_HAL, 4015, METHOD_BUFFERED, FILE_ANY_ACCESS)

//ȡ�������ܹ���ģʽ
#define	IOCTL_RELEASE_HIGHFRE	CTL_CODE(FILE_DEVICE_HAL, 4016, METHOD_BUFFERED, FILE_ANY_ACCESS)

//--------------------------------------------//
//  Backlight��
//--------------------------------------------//

//ע�⣬Backlight�ĳ��淶ΧΪ10-100����CameraӦ�ÿ��Խ������120

//---Function---//

//����Backlight����
#define	IOCTL_SET_BACKLIGHT_FUNC		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4000, METHOD_BUFFERED, FILE_ANY_ACCESS)

//��ȡBacklight����
#define	IOCTL_GET_BACKLIGHT_FUNC		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4001, METHOD_BUFFERED, FILE_ANY_ACCESS)

//--------------------------------------------//
//  USB��
//--------------------------------------------//

//---Macro---//

#define USB_WORK_MODE_NETCARD	1	//USB������������ģʽ
#define USB_WORK_MODE_MOVECARD	0	//USB�����ƶ��洢ģʽ

//---Function---//

//����USB����ģʽ
#define	IOCTL_SET_USB_FUNC		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0202, METHOD_BUFFERED, FILE_ANY_ACCESS)

//��ȡUSB����ģʽ
#define	IOCTL_GET_USB_FUNC		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0203, METHOD_BUFFERED, FILE_ANY_ACCESS)

//--------------------------------------------//
//  RIL��
//--------------------------------------------//

//---Reg---//

//STKMainNameΪSTK 0���˵�����RIL����SIM���ӵ�ʱд��STK 0���˵������µ�ʱ����Ϊ�ա�
//���¼�ֵ��HKEY_LOCAL_MACHINE��ʹ�á�
#define RIL_REGKEY_STK_MAINNAME			L"System\\State\\Phone"
#define RIL_REGVALUE_STRING_MAINNAME	L"STKMainName"

//--------------------------------------------//
//  Button��
//--------------------------------------------//

//---Event---//

#define BUTTON_SEND					L"BUTTON_Send"				//���ż�����
#define BUTTON_END					L"BUTTON_End"				//�Ҷϼ�����

#define BUTTON1_HALFDOWN			L"BUTTON1_Halfdown"			//��������1�밴
#define BUTTON1_HALFUP				L"BUTTON1_Halfup"			//��������1̧��
#define BUTTON2_HALFDOWN			L"BUTTON2_Halfdown"			//��������2�밴
#define BUTTON2_HALFUP				L"BUTTON2_Halfup"			//��������2̧��

#define BUTTON_HOLD_HOLD			L"BUTTON_Hold_Hold"			//Hold����Ч
#define BUTTON_HOLD_RELEASE			L"BUTTON_Hold_Release"		//Hold���ͷ�

#define BUTTON_MICCLEAR_DOWN		L"BUTTON_MicClear_Down"		//����������
#define BUTTON_MICCLEAR_HOLD_ON		L"BUTTON_MicClear_Hold_On"	//����������

#define BUTTON_FN_HOLD_DOWN			L"BUTTON_Fn_Hold"			//���ܼ�����

#define BUTTON_MUSIC_PLAY_PAUSE_UP		L"BUTTON_Music_Play_Pause_Up"		//���ּ�������/��̧ͣ��
#define BUTTON_MUSIC_PREVIOUS_UP		L"BUTTON_Music_Previous_Up"			//���ּ�����һ��̧��
#define BUTTON_MUSIC_NEXT_UP			L"BUTTON_Music_Next_Up"				//���ּ�����һ��̧��
#define BUTTON_MUSIC_PLAY_PAUSE_DOWN	L"BUTTON_Music_Play_Pause_Down"		//���ּ�������/��ͣ����
#define BUTTON_MUSIC_PREVIOUS_DOWN		L"BUTTON_Music_Previous_Down"		//���ּ�����һ�װ���
#define BUTTON_MUSIC_NEXT_DOWN			L"BUTTON_Music_Next_Down"			//���ּ�����һ�װ���

//---Reg---//
//���¼�ֵ���ڶ������ְ������豸�������Ƿ��Կ�ʹ�ã���Ӧ�Ŀ�������ڴ�ʱ����Ҫ������ı仯����������Ӧ�仯
#define REG_APP_LEMUSIC				L"Software\\Lenovo\\LeMusic"
#define REG_VALUE_LEMUSIC_KEYLOCK	L"bMusicKeyLockEnable"	//DWORD

//--------------------------------------------//
//  Earphone��
//--------------------------------------------//

//---Event---//

#define	EARPHONE_IN				L"EARPHONE_In"		//��������
#define	EARPHONE_OUT			L"EARPHONE_Out"		//�����γ�

//--------------------------------------------//
//  APFService��
//--------------------------------------------//

//---Event---//

#define	APF_HANDSET				L"APF_Handset"		//��Ͳģʽ
#define	APF_HANDSFREE			L"APF_HandsFree"	//����ģʽ
#define	APF_EARPHONE_IN			L"APF_Earphone_In"	//��Ͳģʽ
#define	APF_EARPHONE_OUT		L"APF_Earphone_Out"	//����ģʽ

//--------------------------------------------//
//  WAVDEV��
//--------------------------------------------//

//---Macro---//

#define MIC_CLEARMODE_DISABLE	0	//����ģʽ����
#define MIC_CLEARMODE_ENABLE	1	//����ģʽ�ر�

#define WAVDEV_EQ_ORIGIN		0	//EQģʽ��ԭ��
#define WAVDEV_EQ_POP			1	//EQģʽ����������
#define WAVDEV_EQ_OPERA			2	//EQģʽ����Ժ
#define WAVDEV_EQ_BASS			3	//EQģʽ���ص���
#define WAVDEV_EQ_MONO			4	//EQģʽ��������

//---Event---//

#define	WAVDEV_HANDSET			L"WAVDEV_Handset"		//��Ͳģʽ
#define	WAVDEV_HANDSFREE		L"WAVDEV_HandsFree"		//����ģʽ

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


//������������ģʽ
#define	IOCTL_SET_MIC_CLEARMODE		CTL_CODE(FILE_DEVICE_HAL, 0x0100, METHOD_BUFFERED, FILE_ANY_ACCESS)

//��ȡ��������ģʽ
#define	IOCTL_GET_MIC_CLEARMODE		CTL_CODE(FILE_DEVICE_HAL, 0x0200, METHOD_BUFFERED, FILE_ANY_ACCESS)

//����MIC�ڵ绰��¼��ʱ��Ĭ�Ϲ���ģʽ
#define IOCTL_SET_MIC_WORKMODE		CTL_CODE(FILE_DEVICE_HAL, 0x0101, METHOD_BUFFERED, FILE_ANY_ACCESS)

//����EQ����ģʽ
#define	IOCTL_SET_WAVDEV_EQ		CTL_CODE(FILE_DEVICE_HAL, 0x0300, METHOD_BUFFERED, FILE_ANY_ACCESS)

//��ȡEQ����ģʽ
#define	IOCTL_GET_WAVDEV_EQ		CTL_CODE(FILE_DEVICE_HAL, 0x0400, METHOD_BUFFERED, FILE_ANY_ACCESS)

//������ֹMIC������ģʽ
#define IOCTL_ENABLE_MIC_CLEARMODE		CTL_CODE(FILE_DEVICE_HAL, 0x0102, METHOD_BUFFERED, FILE_ANY_ACCESS)

//--------------------------------------------//
//  MSSIM��
//--------------------------------------------//


//--------------------------------------------//
//  Camera��
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
#define CAM_DSHOW_DIGITALZOOM_CTLCODE	3		//���ֱ佹����
#define CAM_DIGITALZOOM_MIN				1		//��С���ֱ佹��Χ - 1��
#define CAM_DIGITALZOOM_MAX				32		//������ֱ佹��Χ - 32��

/******************* CAM_DSHOW_FOCUS_CTLCODE ****************************/
#define CAM_DSHOW_FOCUS_CTLCODE	6		//�Խ�����
#define CAM_FOCUS_DISABLE		0		//�Խ����ƹر�
#define CAM_FOCUS_ENABLE		1		//�Խ����ƿ���
#define CAM_FOCUS_AUTO			2		//�����Զ��Խ�ģʽ
#define CAM_FOCUS_AUTO_START	3		//��ʼ�Զ��Խ��Ķ���
#define CAM_FOCUS_AUTO_STOP		4		//�����Զ��Խ��Ķ���
#define CAM_FOCUS_MICRO			5		//����΢��ģʽ

/******************* CAM_DSHOW_FLASH_CTLCODE ****************************/
#define CAM_DSHOW_FLASH_CTLCODE	7		//����ƿ���
#define CAM_FLASH_DISABLE		0		//����ƹر�
#define CAM_FLASH_ENABLE		1		//�������
#define CAM_FLASH_ALWAYS_ON		2		//����Ƴ�������
#define CAM_FLASH_ALWAYS_OFF	3		//����Ƴ����ر�

/******************* CAM_EXTEND_EFFECT_CTLCODE **************************/
#define	CAM_EXTEND_EFFECT_CTLCODE	8

#define CAM_EFFECT_MISC_ID			0x01	//����
#define CAM_CAPTURE_ROTATE_DISABLE	0		//�ر�¼����Ƶ������ת
#define CAM_CAPTURE_ROTATE_ENABLE	1		//��¼����Ƶ������ת

#define CAM_EFFECT_SCENE_ID			0x02		//���ó���ģʽ
#define CAM_SCENE_COMMON			0x101		//����ģʽ����ͨ
#define CAM_SCENE_PORTRAIT			0x102		//����ģʽ������
#define CAM_SCENE_LANDSCAPE			0x103		//����ģʽ���羰
#define CAM_SCENE_MOVE				0x104		//����ģʽ���˶�
#define CAM_SCENE_MICRO				0x105		//����ģʽ��΢��
#define CAM_SCENE_INDOOR			0x106		//����ģʽ������
#define CAM_SCENE_OUTDOOR			0x107		//����ģʽ������
#define CAM_SCENE_BACKLIGHTING		0x108		//����ģʽ�����
#define CAM_SCENE_DAY				0x109		//����ģʽ������
#define CAM_SCENE_DAWN				0x10A		//����ģʽ������
#define CAM_SCENE_FORENOON			0x10B		//����ģʽ������
#define CAM_SCENE_NOON				0x10C		//����ģʽ������
#define CAM_SCENE_AFTERNOON			0x10D		//����ģʽ������
#define CAM_SCENE_SUNDOWN			0x10E		//����ģʽ������
#define CAM_SCENE_DUSK				0x10F		//����ģʽ���ƻ�
#define CAM_SCENE_NIGHT				0x110		//����ģʽ��ҹ��
#define CAM_SCENE_SNOW				0x111		//����ģʽ��ѩ��
#define CAM_SCENE_NEEDFIRE			0x112		//����ģʽ������

#define CAM_EFFECT_WB_ID			0x03		//���ð�ƽ��
#define CAM_WB_AUTO					0x201		//��ƽ�⣭�Զ�
#define CAM_WB_TUNGSTENLAMP			0x202		//��ƽ�⣭�ٹ��
#define CAM_WB_FLUORESCENTLAMP		0x203		//��ƽ�⣭ӫ���
#define CAM_WB_FLASHLAMP			0x204		//��ƽ�⣭�����
#define CAM_WB_DAYLIGHT				0x205		//��ƽ�⣭�չ�
#define CAM_WB_FINEDAY				0x206		//��ƽ�⣭����
#define CAM_WB_CLOUDY				0x207		//��ƽ�⣭����
#define CAM_WB_SHADOW				0x208		//��ƽ�⣭��Ӱ
#define CAM_WB_NIGHT				0x209		//��ƽ�⣭ҹ��
#define CAM_WB_STARLIGHT			0x20A		//��ƽ�⣭�ǹ�

#define CAM_EFFECT_COLOR_ID			0x04		//����ɫ��ģʽ
#define CAM_COLOR_SEPIA				0x301		//ɫ��ģʽ������
#define CAM_COLOR_BLACK_WHITE		0x302		//ɫ��ģʽ���ڰ�
#define CAM_COLOR_COLOR				0x303		//ɫ��ģʽ����ɫ
#define CAM_COLOR_VIVID				0x304		//ɫ��ģʽ������
#define CAM_COLOR_NEGATIVE			0x305		//ɫ��ģʽ����Ƭ
#define CAM_COLOR_WARM				0x306		//ɫ��ģʽ��ů��
#define CAM_COLOR_COLD				0x307		//ɫ��ģʽ�����
#define CAM_COLOR_FILM				0x308		//ɫ��ģʽ����Ƭ
#define CAM_COLOR_RELIEVO			0x309		//ɫ��ģʽ������
#define CAM_COLOR_WATERPOWDER		0x30A		//ɫ��ģʽ���۲�

#define CAM_EFFECT_ISO_ID			0x05		//����ISOģʽ
#define CAM_ISO_AUTO				0x401		//ISO���Զ�
#define CAM_ISO_64					0x402		//ISO��64
#define CAM_ISO_100					0x403		//ISO��100
#define CAM_ISO_200					0x404		//ISO��200
#define CAM_ISO_400					0x405		//ISO��400
#define CAM_ISO_800					0x406		//ISO��800
#define CAM_ISO_1000				0x407		//ISO��1000
#define CAM_ISO_1600				0x408		//ISO��1600

#define CAM_EFFECT_PHOTOMETRY_ID	0x06		//���ò�ⷽʽ
#define CAM_PHOTOMETRY_MULTIPLE		0x501		//��ⷽʽ�����ز��
#define CAM_PHOTOMETRY_CENTRE		0x502		//��ⷽʽ��ƫ��������
#define CAM_PHOTOMETRY_POINT		0x503		//��ⷽʽ��������
#define CAM_PHOTOMETRY_AFREGION		0x504		//��ⷽʽ��AF�����

#define CAM_EFFECT_EXPOSURE_ID		0x07		//�����عⷽʽ
#define CAM_EXPOSURE_AUTO			0x601		//�عⷽʽ���Զ�
#define CAM_EXPOSURE_PROGRAMAUTO	0x602		//�عⷽʽ�������Զ�
#define CAM_EXPOSURE_SHUTTERFIRST	0x603		//�عⷽʽ����������
#define CAM_EXPOSURE_APERTUREFIRST	0x604		//�عⷽʽ����Ȧ����
#define CAM_EXPOSURE_MANUAL			0x605		//�عⷽʽ���ֶ��ع�
#define CAM_EXPOSURE_SCENE			0x606		//�عⷽʽ���龰�ع�

#define CAM_EFFECT_LENS_ID			0x08		//���þ�ͷ����
#define CAM_LENS_GENERAL			0x702		//��ͷ���ͣ���ͨ
#define CAM_LENS_WIDEANGLE			0x703		//��ͷ���ͣ����
#define CAM_LENS_SOFTLIGHT			0x704		//��ͷ���ͣ����
#define CAM_LENS_MICRO				0x705		//��ͷ���ͣ�΢��

#define CAM_EFFECT_SHUTTER_ID		0x09		//���ÿ����ٶȷ�Χ
#define CAM_SHUTTER_MIN				1			//��߿����ٶ� - 1
#define CAM_SHUTTER_MAX				8000		//��Ϳ����ٶ� - 8000

#define CAM_EFFECT_GAIN_GLOBE_ID	0x10		//����ȫ�����淶Χ
#define CAM_GAIN_GLOBE_MIN			1			//���ȫ������ - 1
#define CAM_GAIN_GLOBE_MAX			256			//���ȫ������ - 256

#define CAM_EFFECT_GAIN_RED_ID		0x11		//���ú�ɫ���淶Χ
#define CAM_GAIN_RED_MIN			1			//��ͺ�ɫ���� - 1
#define CAM_GAIN_RED_MAX			256			//��ߺ�ɫ���� - 256

#define CAM_EFFECT_GAIN_GREEN_ID	0x12		//������ɫ���淶Χ
#define CAM_GAIN_GREEN_MIN			1			//�����ɫ���� - 1
#define CAM_GAIN_GREEN_MAX			256			//�����ɫ���� - 256

#define CAM_EFFECT_GAIN_BLUE_ID		0x13		//������ɫ���淶Χ
#define CAM_GAIN_BLUE_MIN			1			//�����ɫ���� - 1
#define CAM_GAIN_BLUE_MAX			256			//�����ɫ���� - 256

/******************* CAM_EXTEND_STILL_QUALITY_CTLCODE **********************************/
#define CAM_EXTEND_STILL_QUALITY_CTLCODE	9	//������Ƭ����
#define CAM_STILL_QUALITY_MIN				0	//�����Ƭ���� - 0
#define CAM_STILL_QUALITY_MAX				100	//�����Ƭ���� - 100

/******************* CAM_EXTEND_GET_DATA_DIRECT_CTLCODE ********************************/
#define CAM_EXTEND_GET_DATA_DIRECT_CTLCODE	10
#define CAM_DSHOW_PREVIEWFRAMEDATA_DISABLE	0	//�ر�DirectShow Preview PIN֡����
#define CAM_DSHOW_PREVIEWFRAMEDATA_ENABLE	1	//���DirectShow Preview PIN֡����
#define CAM_GET_STILLFRAMEDATA_DISABLE		2	//�ر�ֱ�ӻ�ȡStill PIN֡����
#define CAM_GET_STILLFRAMEDATA_ENABLE		3	//��ֱ�ӻ�ȡStill PIN֡����
#define CAM_STILL_CONTINUOUS_DISABLE				4	//�ر�Still PIN֡�����������ģʽ
#define CAM_STILL_CONTINUOUS_ENABLE				5	//��Still PIN֡�����������ģʽ

/******************* CAM_EXTEND_GET_PREVIEWFRAMEDATA_CTLCODE ***************************/
#define CAM_EXTEND_GET_PREVIEWFRAMEDATA_CTLCODE		12	//��ȡPreview PIN֡����

/******************* CAM_EXTEND_GET_STILLFRAMEDATA_CTLCODE *****************************/
#define CAM_EXTEND_GET_STILLFRAMEDATA_CTLCODE		11	//��ȡStill PIN֡����

/******************* CAM_EXTEND_HW_FEATURE_CTLCODE **********************/
#define CAM_EXTEND_HW_FEATURE_CTLCODE		13
#define CAM_OIS_DISABLE		0	//�رչ�ѧ������
#define CAM_OIS_ENABLE		1	//�򿪹�ѧ������

/******************* CAM_EXTEND_WORK_MODE_CTLCODE **********************/
#define CAM_EXTEND_WORK_MODE_CTLCODE		14
#define CAM_WORK_MODE_FOREGROUND		0	//�����ǰ̨,����ģʽ
#define CAM_WORK_MODE_BACKGROUND		1	//����ں�̨,ʡ��ģʽ


//---Reg---//

//InColor��OutColor��������struct CAMColorFormat�������ֵ��InColor����Camera Driver����õ����ݵ���ɫ��ʽ����DShow��
//Pin Color Format������Դ��ɫ��ʽ��һ�µ�����¸���InColor���趨ֵ��LeDShowLib�е�ILeImageProcess�ӿڽ�����ɫ����ת����
//OutColor����Camera Driver��GetFrameData�ӿ����ʱ����ɫ��ʽ����Ҫʱ��ʹ��LeDShowLib�е�ILeImageProcess�ӿڽ�����ɫ����ת����
//���¼�ֵ��HKEY_LOCAL_MACHINE��ʹ�á�
#define CAM1_REGKEY_COLOR_FORMAT		L"Software\\Microsoft\\DirectX\\DirectShow\\Color Format\\CAM1"
#define CAM2_REGKEY_COLOR_FORMAT		L"Software\\Microsoft\\DirectX\\DirectShow\\Color Format\\CAM2"
#define CAM_REGVALUE_STRING_INCOLOR		L"InColor"
#define CAM_REGVALUE_STRING_OUTCOLOR	L"OutColor"

//���ƹ�DirectShow����Jpeg�ļ�ʱ����ʹ�����¼�ֵ��
//���¼�ֵ��HKEY_LOCAL_MACHINE��ʹ�á�
#define CAM1_FILE_PATH	L"Software\\Lenovo\\Camera\\CAM1\\JPGFILE"
#define CAM2_FILE_PATH	L"Software\\Lenovo\\Camera\\CAM2\\JPGFILE"
#define CAM_FILE_NAME	L"JpgFileName"

//---Event---//

#define CAM1_PRESTILL					L"CAM1_PreStill"					//����ͷ1Ԥ������
#define CAM2_PRESTILL					L"CAM2_PreStill"					//����ͷ2Ԥ������
#define CAM1_STILL_CONTINUOUS_SUCCESS	L"CAM1_StillContinuous_Success"		//����ͷ1���ĳɹ�
#define CAM1_STILL_CONTINUOUS_FAIL		L"CAM1_StillContinuous_Fail"		//����ͷ1����ʧ��
#define CAM2_STILL_CONTINUOUS_SUCCESS	L"CAM2_StillContinuous_Success"		//����ͷ2���ĳɹ�
#define CAM2_STILL_CONTINUOUS_FAIL		L"CAM2_StillContinuous_Fail"		//����ͷ2����ʧ��
#define CAM1_AUTOFOCUS_START			L"CAM1_AutoFocus_Start"				//����ͷ1�Զ��Խ���ʼ
#define CAM1_AUTOFOCUS_SUCCESS			L"CAM1_AutoFocus_Success"			//����ͷ1�Զ��Խ��ɹ�
#define CAM1_AUTOFOCUS_FAIL				L"CAM1_AutoFocus_Fail"				//����ͷ1�Զ��Խ�ʧ��
#define CAM2_AUTOFOCUS_START			L"CAM2_AutoFocus_Start"				//����ͷ2�Զ��Խ���ʼ
#define CAM2_AUTOFOCUS_SUCCESS			L"CAM2_AutoFocus_Success"			//����ͷ2�Զ��Խ��ɹ�
#define CAM2_AUTOFOCUS_FAIL				L"CAM2_AutoFocus_Fail"				//����ͷ2�Զ��Խ�ʧ��
#define CAM1_OPTICALZOOM_START			L"CAM1_OpticalZoom_Start"			//����ͷ1��ѧ�佹��ʼ
#define CAM1_OPTICALZOOM_END			L"CAM1_OpticalZoom_End"				//����ͷ1��ѧ�佹���
#define CAM2_OPTICALZOOM_START			L"CAM2_OpticalZoom_Start"			//����ͷ2��ѧ�佹��ʼ
#define CAM2_OPTICALZOOM_END			L"CAM2_OpticalZoom_End"				//����ͷ2��ѧ�佹���

#if __cplusplus
	}
#endif

#endif//endif _SYSEVENT_H