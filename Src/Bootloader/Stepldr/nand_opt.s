
		INCLUDE	kxarm.h
		INCLUDE	s3c6410.inc

		TEXTAREA

;/////////////////////////////////////////////////////
;//
;//	void _Read_512Byte(unsigned char *pBuf, UINT32 nNFDATAAddr)
;//
;//	Read 512 bytes (1 Sector) word-alined buffer
;//	Buffer (r0) must be word-aligned
;//
;/////////////////////////////////////////////////////

		LEAF_ENTRY  _Read_512Byte

		stmfd	sp!, {r2 - r11}

		mov		r2, #512			; 512 byte count
1
		ldr		r4, [r1]    		; Load 1st word
		ldr		r5, [r1]			; Load 2nd word
		ldr		r6, [r1]			; Load 3rd word
		ldr		r7, [r1]			; Load 4th word
		ldr		r8, [r1]			; Load 5th word
		ldr		r9, [r1]			; Load 6th word
		ldr		r10,[r1]			; Load 7th word
		ldr		r11,[r1]			; Load 8th word
		stmia	r0!,  {r4 - r11}	; Store 8 words (32 byte)

		subs		r2, r2, #32
		bne		%B1

		ldmfd	sp!,  {r2 - r11}

	IF Interworking :LOR: Thumbing
		bx		lr				; Return with Thumb mode
	ELSE
		mov		pc, lr			; Return
	ENDIF

		ENTRY_END

		END

