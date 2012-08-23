// All rights reserved ADENEO EMBEDDED 2010
// GpioTest.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{
	DEVMODE devmode={0};
	devmode.dmSize=sizeof(DEVMODE);
	//devmode.dmDisplayOrientation=DMDO_90;
	devmode.dmFields=DM_DISPLAYORIENTATION;
	ChangeDisplaySettingsEx(NULL,&devmode,NULL,CDS_TEST,NULL);
	if(devmode.dmDisplayOrientation==DMDO_90)
	{		
		RETAILMSG(1,(TEXT("Rotate Screen %d\r\n"),devmode.dmDisplayOrientation));
		devmode.dmSize=sizeof(DEVMODE);
		devmode.dmDisplayOrientation=DMDO_180;
		devmode.dmFields=DM_DISPLAYORIENTATION;
		ChangeDisplaySettingsEx(NULL,&devmode,NULL,0,NULL);
		
	}else{
		RETAILMSG(1,(TEXT("Rotate Screen %d\r\n"),devmode.dmDisplayOrientation));
		devmode.dmSize=sizeof(DEVMODE);
		devmode.dmDisplayOrientation=DMDO_90;
		devmode.dmFields=DM_DISPLAYORIENTATION;
		ChangeDisplaySettingsEx(NULL,&devmode,NULL,0,NULL);
		}
    return 0;
}

