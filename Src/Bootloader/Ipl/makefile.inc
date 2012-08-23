!if 0
Copyright (c) Microsoft Corporation.  All rights reserved.
!endif
!if 0
Use of this source code is subject to the terms of the Microsoft end-user
license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
If you did not accept the terms of the EULA, you are not authorized to use
this source code. For a copy of the EULA, please see the LICENSE.RTF on your
install media.
!endif
!if 0


Module Name:

    makefile.inc

!endif

ipl_romimage:
!if "$(NOLINK)" == ""
    romimage $(ROMIMAGE_FLAGS) ipl.bib

!if 0
Create the "super" IPL - this is the IPL with the Steppingstone loader prepended to it.
!endif
    copy /b $(_PLATFORMROOT)\$(_TGTPLAT)\target\$(_TGTCPU)\$(WINCEDEBUG)\block0img.nb0 + $(_PLATFORMROOT)\$(_TGTPLAT)\target\$(_TGTCPU)\$(WINCEDEBUG)\ipl.nb0 $(_PLATFORMROOT)\$(_TGTPLAT)\target\$(_TGTCPU)\$(WINCEDEBUG)\superipl.nb0
    
!if "$(WINCEREL)"=="1"
    copy $(_PLATFORMROOT)\$(_TGTPLAT)\target\$(_TGTCPU)\$(WINCEDEBUG)\ipl.* $(_FLATRELEASEDIR)
    copy $(_PLATFORMROOT)\$(_TGTPLAT)\target\$(_TGTCPU)\$(WINCEDEBUG)\superipl.* $(_FLATRELEASEDIR)
!endif
!endif
