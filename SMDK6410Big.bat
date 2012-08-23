@REM
@REM Copyright (c) Microsoft Corporation.  All rights reserved.
@REM
@REM
@REM Use of 1this source code is subject to the terms of the Microsoft end-user
@REM license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
@REM If you did not accept the terms of the EULA, you are not authorized to use
@REM this source code. For a copy of the EULA, please see the LICENSE.RTF on your
@REM install media.
@REM

set WINCEREL=1

@REM support for SMDK6410X5D
@REM X5D MCP has 2Gb NAND + 512Mb M-DDR + 512Mb OneDRAM
@REM set SMDK6410_X5D=

set BSP_IROMBOOT=1

set IMGMULTIXIP=
set IMGNODEBUGGER=1
set IMGNOKITL=1
set IMGNOSIMTOOLKIT=1
set BSP_NOPCIBUS=1
set BSP_NOCS8900=1

set BSP_NOHSMMC_CH0=
set BSP_NOHSMMC_CH1=
set BSP_HSMMC_CH1_8BIT=1
set BSP_NOCFATAPI=1

set BSP_NOBACKLIGHT=
set BSP_NOBATTERY=
set BSP_NONLED=

@REM SERIAL/SIR Device Driver
set BSP_NOSERIAL=
set BSP_NOUART0=1
set BSP_NOUART1=
set BSP_NOUART2=
set BSP_NOUART3=
set BSP_NOIRDA2=1
set BSP_NOIRDA3=1

set BSP_NOUSB=
set BSP_NOUSBHCD=
set BSP_NOUSBFN=
set BSP_USBFN_SERIAL=
set BSP_USBFN_MSF=
set BSP_USBFN_RNDIS=1

set BSP_NOMFC=
set BSP_NOJPEG=

set BSP_NOPWRBTN=

@REM Support 2K Page Nand
set BSP_NANDPAGESIZE2K=1
if /i "%SMDK6410_X5D%"=="1" set BSP_NANDPAGESIZE2K=1

@REM AUDIO Device Driver
set BSP_NOAUDIO=
@REM AC97 I/F for AUDIO
set BSP_AUDIO_AC97=1
@REM IIS I/F for AUDIO
REM set BSP_AUDIO_AC97=

@REM CAMERA/I2C Device Driver
set BSP_NOCAMERA=
set IMGCAMERAOEM=1
set BSP_NOI2C=

@REM SPI Device Driver
set BSP_NOSPI=1

@REM D3DM Device Driver
set BSP_NOD3DM=
set PRJ_ENABLE_FSREGHIVE=1
set PRJ_ENABLE_REGFLUSH_THREAD=1

@REM OpenGLES Device Driver
set BSP_NOOES=1

@REM KEYBD Device Driver
set BSP_NOKEYBD=

set BSP_DEBUGPORT=SERIAL_UART0
@REM set BSP_DEBUGPORT=SERIAL_UART1

set IMGTESTCERTS=
set IMGPKGTESTCERTS=
set IMGUNSIGNEDTRUSTED=1

set IMGPAGINGPOOL=1

if /I not "%_TGTPROJ%" == "wpc" goto :not_ppc_proj
@REM =======================================
@REM setup for ASB, RAM Based File System
rem set IMGRAMFMD=1
set IMGRAMFMD=
@REM =======================================
set IMGNOSIR=
set IMGFAKERIL=
set DIFF_UPDATE=
set IMGSDKCERTS=
set IMGNORILTSP=1
set IMGNOTPCLOW=1
set IMGCAMERAOEM=1
set BSP_USEDVS=

:not_ppc_proj:


if /I not "%_TGTPROJ%" == "smartfon" goto :not_sf_proj
set SKUTYPE=PHONESKU
set BSP_NOTOUCH=
set IMGRAMFMD=
set IMGNOSIR=1
set IMGFAKERIL=1
set DIFF_UPDATE=
set IMGSDKCERTS=
set IMGTESTCERTS=
set IMGCAMERAOEM=1
set BSP_USEDVS=

:not_sf_proj:


if /I not "%_TGTPROJ%" == "uldr" goto :not_uldr
set BSP_NOCFATAPI=1
set BSP_NOTOUCH=1
set IMGNORILTSP=1
set IMGNOTPCLOW=1
set IMGNOTALLKMODE=
set BSP_NOSERIAL=1
set BSP_NOUSB=1
set BSP_NOUSBFN=1
set BSP_NOAUDIO=1
set BSP_NOBACKLIGHT=1
set BSP_NOBATTERY=1
set BSP_NONLED=1
set BSP_NOKEYBD=1
set BSP_NOPWRBTN=1
set BSP_NODISPLAY=1
set BSP_NOI2C=1
set BSP_NOCAMERA=1
set BSP_NOD3DM=1
set BSP_NOMFC=1
set BSP_NOJPEG=1
set IMGPERSISTENTSTORAGE=1
set BSP_NODISPLAY=1
set BSP_NOOES=1

:not_uldr


if /i "%IMGNOKITL%"=="" set IMGTESTCERTS=1
if /i "%IMGNOKITL%"=="" set IMGPKGTESTCERTS=1
if /i "%IMGNOKITL%"=="" set IMGSDKCERTS=1

set BSP_KITL=NONE
@REM set BSP_KITL=SERIAL_UART0
@REM set BSP_KITL=SERIAL_UART1
@REM set BSP_KITL=SERIAL_UART2
@REM set BSP_KITL=SERIAL_UART3
@REM set BSP_KITL=USBSERIAL

if /i not "%_TGTPROJ%"=="wpc" if /i not "%_TGTPROJ%"=="smartfon" goto :not_wpc_or_smartfon

    set SYSGEN_D3DM=1
    set SYSGEN_D3DMREF=1

    set IMGPERSISTENTSTORAGE=1
    set IMGCAMERAOEM=1

    set SAMPLES_DSHOWFILTERS=1
    set SAMPLES_MFC=1
    set SAMPLES_JPEG=1
    set SAMPLES_HYBRIDDIVX=1

	@REM In case of SMDK6410 X5D type, OpenGLES is disabled temporarily
	if /i "%SMDK6410_X5D%"=="1" set BSP_NOOES=1

    @REM for using GDI Performance Utility, DispPerf
    set DO_DISPPERF=

    REM default to PHONESKU for PocketPC and SmartPhone builds
    if "%SKUTYPE%"=="" set SKUTYPE=PHONESKU

    REM Make sure the project names are in lower case
    if /i "%_TGTPROJ%"=="wpc" set _TGTPROJ=wpc
    if /i "%_TGTPROJ%"=="smartfon" set _TGTPROJ=smartfon

:not_wpc_or_smartfon


