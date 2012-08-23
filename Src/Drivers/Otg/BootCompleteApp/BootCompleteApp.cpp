//
// BootCompleteApp.cpp
//

#include <windows.h>
#include <nkintr.h>
#include  <Wrlspwr.h>
BOOL GetWirelessState(RADIODEVTYPE radiotype, DWORD * radiostate)
{
    RDD * pDevice = ( RDD * )NULL;
    RDD * pTD;
    HRESULT hr = S_OK;
    
    GetWirelessDevices(&pDevice, 0);
    if (pDevice)
    {
        pTD = pDevice;
        
        // loop through the linked list of devices
        while (pTD)
        { 
            if(pTD->DeviceType == radiotype)
            {
                *radiostate =pTD->dwState;
                FreeDeviceList(pDevice);
                return TRUE;
            }	
            pTD = pTD->pNext;
        }
        // Free up the list of devices retrieved with          
        FreeDeviceList(pDevice);
    }
    
    return FALSE;
}
void SetWirelessState(RADIODEVTYPE radiotype, DWORD radiostate)
{	
    RDD * pTD;
    HRESULT hr = S_OK;
    RDD * pDevice = ( RDD * )NULL;
    
    GetWirelessDevices(&pDevice, 0);
    if (pDevice)
    {	
        pTD = pDevice;

        // loop through the linked list of devices
        while (pTD)
        { 
            if(pTD->DeviceType == radiotype)
            {
                if(radiostate == (pTD->dwState))
                {
                    FreeDeviceList(pDevice);
                    return;
                }

                hr = ChangeRadioState(pTD,radiostate,POWER_PRE_SAVE);
                FreeDeviceList(pDevice);
            }	
            pTD = pTD->pNext;
        }
        // Free up the list of devices retrieved with    
        FreeDeviceList(pDevice);
    }
}
int WINAPI WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR	 lpCmdLine,
					 int	   nCmdShow)
{

	HANDLE BootCompleteEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("OEM/BootCompleteEvent"));
	DWORD state=0;
	if (BootCompleteEvent == NULL)
	{
		RETAILMSG(1, (TEXT("[BootCompleteApp] ERROR: Notify OpenEvent() FAIL 0x%08X\r\n"), GetLastError()));
		CloseHandle(BootCompleteEvent);
		BootCompleteEvent = NULL;
	}
	else
	{
		RETAILMSG(1,(TEXT("[BootCompleteApp] Boot Complete Event Occured\r\n")));	
		ResetEvent(BootCompleteEvent);
		SetEvent(BootCompleteEvent);
		if(GetWirelessState(POWER_MANAGED,&state))
		{
			if(state==1)
				SetWirelessState(POWER_MANAGED,0);
			else
				RETAILMSG(1,(TEXT("WiFi is power off at boot up\r\n")));
		}else
			RETAILMSG(1,(TEXT("Get Wireless state failed\r\n")));
	}
	return 0;
}
