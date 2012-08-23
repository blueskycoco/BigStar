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
@echo off
@REM del %_FLATRELEASEDIR%\flash.dio.nb0
@REM rename %_FLATRELEASEDIR%\flash.dio.postproc flash.dio.nb0
@REM rename %_FLATRELEASEDIR%\FLASH.DIO flash.dio.nb0
if /I not "%MAKEIMG_BIN_FORMAT%" == "imageupdate" goto :not_flash_bin
del %_FLATRELEASEDIR%\flash.dio.nb0
rename %_FLATRELEASEDIR%\flash.dio.postproc flash.dio.nb0
:not_flash_bin

