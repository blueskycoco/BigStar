**** Linking the Pictures application to an OEM Camera application ****
 

This document is intended for OEMs who chose not to use the Microsoft Camera application. It describes how to provide links from the Microsoft Pictures application to the OEM’s Camera application.
 

It is assumed that the reader is familiar with Browser Helper Object (BHO) technology. For more information:
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/mobilesdk5/html/mogrfTheBrowserHelperObjectBHO.asp 

 

In general, the process involves two steps:

1) Hide all existing integration points between the Pictures app and the Microsoft Camera app (by setting a registry key) 
2) Add in new integration points between the Pictures app and the OEM Camera app 
 

Setting the registry key DisableCamera=1 will remove the Camera icon in Picture browse view, remove the Camera label on the left softkey, remove the Camera menu item in the right softkey on PocketPC, and remove the two Camera-related option dialogs. See the topic camera registry settings for details on this registry key.

To create a link to an OEM’s Camera application, the OEM can use menu extensibility to add a menu item to the right softkey. This needs to be done separately for the Picture browse view and for the Picture Picker. See the topic Menu Extensibility for Camera, Pictures and Videos for details.

This works well on Smartphone.

On PocketPC, however, there is a complication. The Picture Picker on PocketPC does not have a softkey menu. Therefore, the OEM needs to use the helper object extensibility feature in the Pictures app to add in a Camera icon to the Picture Picker’s browse view.
 

The following section provides more detail on implementing this last step.

 
**** Using Helper Objects to add a Camera icon to browse view **** 

This requires the OEM writing a helper object extension (BHO) for the Pictures app. A sample BHO to serve this purpose is provided as part of the samples in the AK (04/public/shellw/oak/samples/OEMCamera/BHO). The BHO communicates with the Pictures app through inserticon.lib, which is provided as sample code here: 04/public/shellw/oak/samples/OEMCamera/InsertIcon 

CameraIcon.dll replaces the frame window and file explorer window's proc. The DLL passes all messages on to the original window proc.


Following are the steps to integrate the OEM BHO, OEM Camera app and CameraIcon.dll.

1.      OEM needs to implement a picture BHO module

2.      In the function CIBHOObj::SetSite (IUnknown * pUnkSite), the OEM must replace the Frame window’s process. See line 143 in camerabho.cpp

3.      In the function CIBHOObj::SetSite (IUnknown * pUnkSite), the OEM must call the InsertCameraIconToBrowser at the end of this function. See line 148 in camerabho.cpp

4.      In the new window process for the frame window, it should call PressCameraIcon to check if the camera icon is been pressed, if so, launch the OEM’s camera application. See the lines from 216 to 244 in camerabho.cpp.

5.      Detect if the Pictures app is in normal browse mode or in picture picker mode: Call the function GetPicturesMode to get the current picture mode. See the line 223 in camerabho.cpp

6.      In Picture Picker Mode, return the file captured by the OEM camera app to the calling function: Call ReturnFile2Caller to return the full file path to the calling function. See the line 234 in camerabho.cpp

7.      Register the BHO in the registry. For example, see camerabho.reg.

 



