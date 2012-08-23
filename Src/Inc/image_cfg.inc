;//------------------------------------------------------------------------------
;//  NAMING CONVENTION
;//
;//  The IMAGE_ naming convention ...
;//
;//  IMAGE_<NAME>_<SECTION>_<MEMORY_DEVICE>_[OFFSET|SIZE|START|END]
;//
;//      <NAME>          - WINCE, BOOT, SHARE
;//      <SECTION>       - section name: user defined
;//      <MEMORY_DEVICE> - the memory device the block resides on
;//      OFFSET          - number of bytes from memory device start address
;//      SIZE            - maximum size of the block
;//      START           - start address of block    (device address + offset)
;//      END             - end address of block      (start address  + size - 1)
;//
;//------------------------------------------------------------------------------

;// DRAM Base Address
	[	SMDK6410_X5D
DRAM_BASE_PA_START		EQU	(0x60000000)
	|
DRAM_BASE_PA_START		EQU	(0x50000000)
	]
DRAM_BASE_CA_START		EQU	(0x80000000)
DRAM_BASE_UA_START		EQU	(0xA0000000)
	[	SMDK6410_X5D
DRAM_SIZE			EQU	(0x04000000)
	|
DRAM_SIZE			EQU	(0x08000000)
	]

;//------------------------------------------------------------------------------

;// Steploader Area
IMAGE_STEPLOADER_PA_START	EQU	(0x00000000)
IMAGE_STEPLOADER_SIZE		EQU	(0x00001000)

;//------------------------------------------------------------------------------

;// Eboot Area
IMAGE_EBOOT_OFFSET		EQU	(0x00038000)
IMAGE_EBOOT_PA_START		EQU	(DRAM_BASE_PA_START+IMAGE_EBOOT_OFFSET)
IMAGE_EBOOT_CA_START		EQU	(DRAM_BASE_CA_START+IMAGE_EBOOT_OFFSET)
IMAGE_EBOOT_UA_START		EQU	(DRAM_BASE_UA_START+IMAGE_EBOOT_OFFSET)
IMAGE_EBOOT_SIZE		EQU	(0x00080000)

EBOOT_BINFS_BUFFER_OFFSET	EQU	(0x000C0000)
EBOOT_BINFS_BUFFER_PA_START	EQU	(DRAM_BASE_PA_START+EBOOT_BINFS_BUFFER_OFFSET)
EBOOT_BINFS_BUFFER_CA_START	EQU	(DRAM_BASE_CA_START+EBOOT_BINFS_BUFFER_OFFSET)
EBOOT_BINFS_BUFFER_UA_START	EQU	(DRAM_BASE_UA_START+EBOOT_BINFS_BUFFER_OFFSET)
EBOOT_BINFS_BUFFER_SIZE		EQU	(0x00021000)

EBOOT_FILE_CACHE_OFFSET		EQU	(0x00200000)
EBOOT_FILE_CACHE_PA_START	EQU	(DRAM_BASE_PA_START+EBOOT_FILE_CACHE_OFFSET)
EBOOT_FILE_CACHE_CA_START	EQU	(DRAM_BASE_CA_START+EBOOT_FILE_CACHE_OFFSET)
EBOOT_FILE_CACHE_UA_START	EQU	(DRAM_BASE_UA_START+EBOOT_FILE_CACHE_OFFSET)
EBOOT_FILE_CACHE_SIZE		EQU	(0x01400000)

EBOOT_USB_BUFFER_OFFSET		EQU	(0x02000000)
EBOOT_USB_BUFFER_PA_START	EQU	(DRAM_BASE_PA_START+EBOOT_USB_BUFFER_OFFSET)
EBOOT_USB_BUFFER_CA_START	EQU	(DRAM_BASE_CA_START+EBOOT_USB_BUFFER_OFFSET)
EBOOT_USB_BUFFER_UA_START	EQU	(DRAM_BASE_UA_START+EBOOT_USB_BUFFER_OFFSET)
EBOOT_USB_BUFFER_SIZE		EQU	(0x01F00000)

;//------------------------------------------------------------------------------

;// NK Area
IMAGE_NK_OFFSET			EQU	(0x00200000)
IMAGE_NK_PA_START		EQU	(DRAM_BASE_PA_START+IMAGE_NK_OFFSET)
IMAGE_NK_CA_START		EQU	(DRAM_BASE_CA_START+IMAGE_NK_OFFSET)
IMAGE_NK_UA_START		EQU	(DRAM_BASE_UA_START+IMAGE_NK_OFFSET)
IMAGE_NK_SIZE			EQU	(0x01F00000)

;//------------------------------------------------------------------------------

;// BSP ARGs Area
IMAGE_SHARE_ARGS_OFFSET				EQU	(0x00020000)
IMAGE_SHARE_ARGS_UPDATEMODE_OFFSET	EQU	(0x00000044)
IMAGE_SHARE_ARGS_ULDRREBOOT_OFFSET	EQU	(0x00000048)
IMAGE_SHARE_ARGS_PA_START			EQU	(DRAM_BASE_PA_START+IMAGE_SHARE_ARGS_OFFSET)
IMAGE_SHARE_ARGS_CA_START			EQU	(DRAM_BASE_CA_START+IMAGE_SHARE_ARGS_OFFSET)
IMAGE_SHARE_ARGS_UA_START			EQU	(DRAM_BASE_UA_START+IMAGE_SHARE_ARGS_OFFSET)
IMAGE_SHARE_ARGS_PA_UPDATEMODE		EQU	(IMAGE_SHARE_ARGS_PA_START+IMAGE_SHARE_ARGS_UPDATEMODE_OFFSET)
IMAGE_SHARE_ARGS_PA_ULDRREBOOT		EQU	(IMAGE_SHARE_ARGS_PA_START+IMAGE_SHARE_ARGS_ULDRREBOOT_OFFSET)
IMAGE_SHARE_ARGS_SIZE				EQU	(0x00000800)

;//------------------------------------------------------------------------------

;// Sleep Data Area
IMAGE_SLEEP_DATA_OFFSET		EQU	(0x00028000)
IMAGE_SLEEP_DATA_PA_START	EQU	(DRAM_BASE_PA_START+IMAGE_SLEEP_DATA_OFFSET)
IMAGE_SLEEP_DATA_CA_START	EQU	(DRAM_BASE_CA_START+IMAGE_SLEEP_DATA_OFFSET)
IMAGE_SLEEP_DATA_UA_START	EQU	(DRAM_BASE_UA_START+IMAGE_SLEEP_DATA_OFFSET)
IMAGE_SLEEP_DATA_SIZE		EQU	(0x00002000)

;//------------------------------------------------------------------------------

;// Display Frame Buffer
	[	SMDK6410_X5D
IMAGE_FRAMEBUFFER_OFFSET	EQU	(0x03000000)
	|
IMAGE_FRAMEBUFFER_OFFSET	EQU	(0x06800000)
	]
IMAGE_FRAMEBUFFER_PA_START	EQU	(DRAM_BASE_PA_START+IMAGE_FRAMEBUFFER_OFFSET)
IMAGE_FRAMEBUFFER_UA_START	EQU	(DRAM_BASE_UA_START+IMAGE_FRAMEBUFFER_OFFSET)
IMAGE_FRAMEBUFFER_SIZE		EQU	(0x00C00000)

;//------------------------------------------------------------------------------

;// MFC Video Process Buffer
	[	SMDK6410_X5D
IMAGE_MFC_BUFFER_OFFSET		EQU	(0x03800000)
	|
IMAGE_MFC_BUFFER_OFFSET		EQU	(0x07400000)
	]
IMAGE_MFC_BUFFER_PA_START	EQU	(DRAM_BASE_PA_START+IMAGE_MFC_BUFFER_OFFSET)
IMAGE_MFC_BUFFER_UA_START	EQU	(DRAM_BASE_UA_START+IMAGE_MFC_BUFFER_OFFSET)
IMAGE_MFC_BUFFER_SIZE		EQU	(0x00C00000)

;// CMM memory
	[	SMDK6410_X5D
IMAGE_CMM_BUFFER_OFFSET		EQU	(0x02D00000)
	|
IMAGE_CMM_BUFFER_OFFSET		EQU	(0x06500000)
	]
IMAGE_CMM_BUFFER_PA_START	EQU	(DRAM_BASE_PA_START+IMAGE_CMM_BUFFER_OFFSET)
IMAGE_CMM_BUFFER_UA_START	EQU	(DRAM_BASE_UA_START+IMAGE_CMM_BUFFER_OFFSET)
IMAGE_CMM_BUFFER_SIZE		EQU	(0x00300000)

;//------------------------------------------------------------------------------
;------------------------------------------------------------------------------
;	OEM Stack Layout
;------------------------------------------------------------------------------

TOP_OF_STACKS_PHYSICAL		EQU	(DRAM_BASE_PA_START+IMAGE_NK_OFFSET)	; Stack Top is in front of NK Image
TOP_OF_STACKS_VIRTUAL		EQU	(DRAM_BASE_CA_START+IMAGE_NK_OFFSET)

; Stack Size of Each Mode
FIQStackSize	EQU	256
IRQStackSize	EQU	256
AbortStackSize	EQU	256
UndefStackSize	EQU	256
SVCStackSize	EQU	1024
;UserStackSize	EQU	2048

; Stack Location of Each Mode (in Physical Address)
FIQStack_PA	EQU	(TOP_OF_STACKS_PHYSICAL	- 0x0)
IRQStack_PA	EQU	(FIQStack_PA		- FIQStackSize)
AbortStack_PA	EQU	(IRQStack_PA		- IRQStackSize)
UndefStack_PA	EQU	(AbortStack_PA		- AbortStackSize)
SVCStack_PA	EQU	(UndefStack_PA		- UndefStackSize)
UserStack_PA	EQU	(SVCStack_PA		- SVCStackSize)

; Stack Location of Each Mode (in Physical Address)
FIQStack_VA	EQU	(TOP_OF_STACKS_VIRTUAL	- 0x0)
IRQStack_VA	EQU	(FIQStack_VA		- FIQStackSize)
AbortStack_VA	EQU	(IRQStack_VA		- IRQStackSize)
UndefStack_VA	EQU	(AbortStack_VA		- AbortStackSize)
SVCStack_VA	EQU	(UndefStack_VA		- UndefStackSize)
UserStack_VA	EQU	(SVCStack_VA		- SVCStackSize)

;------------------------------------------------------------------------------
;	Sleep Data Layout
;------------------------------------------------------------------------------
SleepState_Data_Start		EQU	(0)
SleepState_WakeAddr		EQU	(SleepState_Data_Start)
SleepState_SYSCTL		EQU	(SleepState_WakeAddr	+ WORD_SIZE )
SleepState_MMUTTB0		EQU	(SleepState_SYSCTL	+ WORD_SIZE )
SleepState_MMUTTB1		EQU	(SleepState_MMUTTB0	+ WORD_SIZE )
SleepState_MMUTTBCTL		EQU	(SleepState_MMUTTB1	+ WORD_SIZE )
SleepState_MMUDOMAIN		EQU	(SleepState_MMUTTBCTL	+ WORD_SIZE )
SleepState_SVC_SP		EQU	(SleepState_MMUDOMAIN	+ WORD_SIZE )
SleepState_SVC_SPSR		EQU	(SleepState_SVC_SP	+ WORD_SIZE )
SleepState_FIQ_SPSR		EQU	(SleepState_SVC_SPSR	+ WORD_SIZE )
SleepState_FIQ_R8		EQU	(SleepState_FIQ_SPSR	+ WORD_SIZE )
SleepState_FIQ_R9		EQU	(SleepState_FIQ_R8	+ WORD_SIZE )
SleepState_FIQ_R10		EQU	(SleepState_FIQ_R9	+ WORD_SIZE )
SleepState_FIQ_R11		EQU	(SleepState_FIQ_R10	+ WORD_SIZE )
SleepState_FIQ_R12		EQU	(SleepState_FIQ_R11	+ WORD_SIZE )
SleepState_FIQ_SP		EQU	(SleepState_FIQ_R12	+ WORD_SIZE )
SleepState_FIQ_LR		EQU	(SleepState_FIQ_SP	+ WORD_SIZE )
SleepState_ABT_SPSR		EQU	(SleepState_FIQ_LR	+ WORD_SIZE )
SleepState_ABT_SP		EQU	(SleepState_ABT_SPSR	+ WORD_SIZE )
SleepState_ABT_LR		EQU	(SleepState_ABT_SP	+ WORD_SIZE )
SleepState_IRQ_SPSR		EQU	(SleepState_ABT_LR	+ WORD_SIZE )
SleepState_IRQ_SP		EQU	(SleepState_IRQ_SPSR	+ WORD_SIZE )
SleepState_IRQ_LR		EQU	(SleepState_IRQ_SP	+ WORD_SIZE )
SleepState_UND_SPSR		EQU	(SleepState_IRQ_LR	+ WORD_SIZE )
SleepState_UND_SP		EQU	(SleepState_UND_SPSR	+ WORD_SIZE )
SleepState_UND_LR		EQU	(SleepState_UND_SP	+ WORD_SIZE )
SleepState_SYS_SP		EQU	(SleepState_UND_LR	+ WORD_SIZE )
SleepState_SYS_LR		EQU	(SleepState_SYS_SP	+ WORD_SIZE )
SleepState_Data_End     	EQU	(SleepState_SYS_LR	+ WORD_SIZE )
SLEEPDATA_SIZE		    	EQU	((SleepState_Data_End - SleepState_Data_Start) / 4)

;//------------------------------------------------------------------------------

	END

