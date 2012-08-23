;
; Copyright (c) Microsoft Corporation.  All rights reserved.
;
;
; Use of this source code is subject to the terms of the Microsoft end-user
; license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
; If you did not accept the terms of the EULA, you are not authorized to use
; this source code. For a copy of the EULA, please see the LICENSE.RTF on your
; install media.
;
;******************************************************************************
;*
;* System On Chip(SOC)
;*
;* Copyright (c) 2002 Software Center, Samsung Electronics, Inc.
;* All rights reserved.
;*
;* This software is the confidential and proprietary information of Samsung 
;* Electronics, Inc("Confidential Information"). You Shall not disclose such 
;* Confidential Information and shall use it only in accordance with the terms 
;* of the license agreement you entered into Samsung.
;*
;******************************************************************************

    INCLUDE kxarm.h
	INCLUDE	s3c6410.inc
	INCLUDE	image_cfg.inc
	
PHY_RAM_START	EQU	DRAM_BASE_PA_START
VIR_RAM_START	EQU	DRAM_BASE_CA_START

	TEXTAREA

	LEAF_ENTRY Launch

	ldr	r2, = PhysicalStart
	ldr     r3, = (VIR_RAM_START - PHY_RAM_START)

	sub     r2, r2, r3
	
	[ {FALSE}
;	mrc     p15, 0, r1, c1, c0, 0
;	and    r1, r1, #0xC0000000
;	orr     r1, r1, #0x0070
	]

	mov		r3, #0							; Before turn off MMU, flush D cache
	mcr		p15, 0, r3, c7, c14, 0			; Clean & Invalidate Entire D Cache

    mov     r3, #0                			; Invalidate instruction cache
    mcr     p15, 0, r3, c7, c5, 0

	[ {TRUE}
	mov     r1, #0x0070             ; Disable MMU
	]
	mcr     p15, 0, r1, c1, c0, 0
	nop
	mov     pc, r2                  ; Jump to PStart
	nop


	; MMU & caches now disabled.

PhysicalStart

	mov     r2, #0
	mcr     p15, 0, r2, c8, c7, 0   ; Flush the TLB
	mov     pc, r0			; Jump to program we are launching.
        
    ENTRY_END
    
	;++
	;  Routine:
	;
	;      ShowLights
	;
	;  Description:
	;
	;      Set the Hexadecimal LED array to the values specified
	;
	;  Arguments:
	;
	;      r0 = word containing 8 nibble values to write to the Hexadecimal LED
	;      
	;
	;--

	LEAF_ENTRY ShowLights

	mov	pc, lr

    ENTRY_END


	LEAF_ENTRY CP15_Read_ID_Code
	mrc p15,0,r0,c0,c0,0
	mov pc, lr
    ENTRY_END
	
	LEAF_ENTRY CP15_Read_Cache_Type
	mrc p15,0,r0,c0,c0,1
	mov pc, lr
    ENTRY_END

	LEAF_ENTRY CP15_Read_TCM_Type
	mrc p15,0,r0,c0,c0,2
	mov pc, lr
    ENTRY_END

	LEAF_ENTRY CP15_Control_R1
	mrc p15,0,r0,c1,c0,0
	mov pc, lr
    ENTRY_END


	END

