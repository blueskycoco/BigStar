@REM
@REM Copyright (c) Microsoft Corporation.  All rights reserved.
@REM
@REM
@REM Use of this source code is subject to the terms of the Microsoft end-user
@REM license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
@REM If you did not accept the terms of the EULA, you are not authorized to use
@REM this source code. For a copy of the EULA, please see the LICENSE.RTF on your
@REM install media.
@REM
REM Platform specific sysgen settings

if "%SYSGEN_CDMASKU%" == "" set SYSGEN_GSMSKU=1
echo 1111111111111111111111111
set SYSGEN_MSFLASH=1
set SYSGEN_MSFLASH_RAMFMD=
set SYSGEN_SDBUS=1
set SYSGEN_SD_MEMORY=1
set SYSGEN_USB=1
set SYSGEN_USBFN=1
set SYSGEN_USBFN_SERIAL=1
set SYSGEN_USBFN_ETHERNET=1
set SYSGEN_USBFN_STORAGE=1
set SYSGEN_USB_HID=1
set SYSGEN_USB_HID_CLIENTS=1
set SYSGEN_USB_STORAGE=1
REM set SYSGEN_CAMERA_NULL=1
set SYSGEN_CONSOLE=1
set SYSGEN_ETHERNET=1
set SYSGEN_ETH_80211=1
set SYSGEN_BTH=1
set SYSGEN_BTH_CSR_ONLY=1
set SYSGEN_BTH_HID=1
set SYSGEN_BTH_HID_KEYBOARD=1
set SYSGEN_BTH_HID_MOUSE=1
set SYSGEN_BTH_UTILS=
set SYSGEN_NETUI=
set SYSGEN_STANDARDSHELL=
set SYSGEN_BTH_AG=1
set SYSGEN_NETUTILS=1
set SYSGEN_BTH_AUDIO=1
set SYSGEN_MINICOM=1
set SYSGEN_OBEX_SERVER=1
set SYSGEN_OBEX_CLIENT=1
set SYSGEN_OBEX_INBOX=1
set SYSGEN_OBEX_FILEBROWSER=1
echo 2222222222222222222222222222222