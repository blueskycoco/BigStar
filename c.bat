@REM
@REM Copyright (c)Samsung Elec. co, LTD.  All rights reserved.
@REM
@echo off
REM ** AUDIO wave driver build script **

rem build -cfs
REM copy /Y %_TARGETPLATROOT%\target\%_TGTCPU%\%WINCEDEBUG%\*.* %_FLATRELEASEDIR%
echo ----------------------------------------------
echo %_TARGETPLATROOT%\target\%_TGTCPU%\%WINCEDEBUG%
echo ----------------------------------------------
copy /Y %_TARGETPLATROOT%\target\%_TGTCPU%\%WINCEDEBUG%\*.* %_FLATRELEASEDIR%

copy /Y %_TARGETPLATROOT%\files\*.* %_FLATRELEASEDIR%


