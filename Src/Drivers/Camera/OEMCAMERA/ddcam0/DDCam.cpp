#include "ddcam.h"


SHCAMERACAPTURE shcc;
int picture_number=0;


int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HRESULT         hResult;

	memset(&msg,0,sizeof(msg));
	// Set the SHCAMERACAPTURE structure.
	    ZeroMemory(&shcc, sizeof(shcc));
	    shcc.cbSize             = sizeof(shcc);
	    //shcc.hwndOwner          = hwndDlg;
	    //shcc.pszInitialDir      = TEXT("\\My Documents\\ÎÒµÄÍ¼Æ¬\\");
	    //shcc.pszDefaultFileName = TEXT("test.jpg");
	    shcc.pszTitle           = TEXT("Camera Demo");
	    shcc.VideoTypes         = CAMERACAPTURE_VIDEOTYPE_MESSAGING;
	    shcc.nResolutionWidth   = 0;
	    shcc.nResolutionHeight  = 0;
	    shcc.nVideoTimeLimit    = 0;
	    shcc.Mode               = CAMERACAPTURE_MODE_STILL;

	StringCchPrintf(shcc.szFile,MAX_PATH,TEXT("%s%s"),shcc.pszInitialDir,shcc.pszDefaultFileName);


	    // Display the Camera Capture dialog.   //Call OEMCameraCapture acordding to the registry setting
	    while(hResult = SHCameraCapture(&shcc),hResult == S_OK)
	    	{
			RETAILMSG(1, (TEXT("S_OK:Mode=%d nResolutionWidth=%d VideoTypes=0x%x\r\n"),shcc.Mode,shcc.nResolutionWidth,shcc.VideoTypes) );
			//StringCchCopy(pszFilename, MAX_PATH, shcc.szFile);
	    	}

		RETAILMSG(1, (TEXT("S_OK:Mode=%d nResolutionWidth=%d\r\n"),shcc.Mode,shcc.nResolutionWidth) );

	    // The next statements will execute only after the user takes
	    // a picture or video, or closes the Camera Capture dialog.
	    if (S_FALSE == hResult)
	    {
			RETAILMSG(1, (TEXT("SHCameraCapture false!!!!!\r\n") ) );
	    }else
	    {
	    	RETAILMSG(1, (TEXT("SHCameraCapture failed!!!!!\r\n") ) );
	    }

	    //return hResult;

	return msg.wParam;
}



