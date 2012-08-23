@REM
@REM Copyright (c)Samsung Elec. co, LTD.  All rights reserved.
@REM


echo copy /Y %_FLATRELEASEDIR%\langproc\*.reg, *.bib, *.sku.xml to  %_FLATRELEASEDIR%
copy /Y %_FLATRELEASEDIR%\langproc\*.reg %_FLATRELEASEDIR%
copy /Y %_FLATRELEASEDIR%\langproc\*.bib %_FLATRELEASEDIR%
copy /Y %_FLATRELEASEDIR%\langproc\*.sku.xml %_FLATRELEASEDIR%

echo copy /Y %_FLATRELEASEDIR%\packages\*.cab.pkg to  %_FLATRELEASEDIR% without oemdrivers package...
copy /y %_FLATRELEASEDIR%\packages\oemdrivers.cab.pkg %_FLATRELEASEDIR%\packages\oemdrivers.xxx
copy /y %_FLATRELEASEDIR%\packages\oemxipkernel.cab.pkg %_FLATRELEASEDIR%\packages\oemxipkernel.xxx
copy /y %_FLATRELEASEDIR%\packages\oemmisc.cab.pkg %_FLATRELEASEDIR%\packages\oemmisc.xxx

del %_FLATRELEASEDIR%\packages\oemdrivers.cab.pkg
del %_FLATRELEASEDIR%\packages\oemxipkernel.cab.pkg
del %_FLATRELEASEDIR%\packages\oemmisc.cab.pkg

copy /Y %_FLATRELEASEDIR%\packages\*.cab.pkg %_FLATRELEASEDIR%\prebuilt

copy /y %_FLATRELEASEDIR%\packages\oemdrivers.xxx %_FLATRELEASEDIR%\packages\oemdrivers.cab.pkg 
copy /y %_FLATRELEASEDIR%\packages\oemxipkernel.xxx %_FLATRELEASEDIR%\packages\oemxipkernel.cab.pkg 
copy /y %_FLATRELEASEDIR%\packages\oemmisc.xxx %_FLATRELEASEDIR%\packages\oemmisc.cab.pkg 

del %_FLATRELEASEDIR%\packages\oemdrivers.xxx
del %_FLATRELEASEDIR%\packages\oemxipkernel.xxx
del %_FLATRELEASEDIR%\packages\oemmisc.xxx
