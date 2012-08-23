#include <windows.h>
#include <pehdr.h>
#include <romldr.h>

#include <bsp_cfg.h>
#include <image_cfg.h>
#include "utils.h"

// Constants.
//

#define NAND_SB_PAGE_SIZE_BYTES	(0x200)		// 1 Page = 0x200 (512 Bytes)
#define NAND_SB_BLOCK_SIZE_BYTES	(0x4000)		// 1 Block = 16 KB
#define NAND_SB_PAGES_PER_BLOCK	(NAND_SB_BLOCK_SIZE_BYTES / NAND_SB_PAGE_SIZE_BYTES)	// 32-pages

#define NAND_LB_PAGE_SIZE_BYTES	(0x800)		// 1 Page = 0x800 (2048 Bytes)
#define NAND_LB_BLOCK_SIZE_BYTES	(0x20000)	// 1 Block = 128 KB
#define NAND_LB_PAGES_PER_BLOCK	(NAND_LB_BLOCK_SIZE_BYTES / NAND_LB_PAGE_SIZE_BYTES)	// 64-pages

#define NAND_BYTES_PER_PAGE		((g_bLargeBlock==TRUE)?NAND_LB_PAGE_SIZE_BYTES:NAND_SB_PAGE_SIZE_BYTES)
#define NAND_PAGES_PER_BLOCK		((g_bLargeBlock==TRUE)?NAND_LB_PAGES_PER_BLOCK:NAND_SB_PAGES_PER_BLOCK)


	// NOTE: we assume that this Steppingstone loader occupies *part* the first (good) NAND flash block.  More
	// specifically, the loader takes up 4096 bytes (or 8 NAND pages) of the first block.  We'll start our image
	// copy on the very next page.
//#define NAND_COPY_PAGE_OFFSET   (0x1000/NAND_BYTES_PER_PAGE) //0x1000 means Stepping stone size 4K
#define NAND_COPY_PAGE_OFFSET   (STEPLDR_LENGTH/NAND_BYTES_PER_PAGE) //0x1000 means Stepping stone size 4K

#define LOAD_ADDRESS_PHYSICAL   IMAGE_NBL2_ADDR_PA 	// defined in "image_cfg.h" file.
#define LOAD_SIZE_BYTES		 	(0x0001E000)    //the length of NBL2
#define LOAD_SIZE_PAGES		 	(LOAD_SIZE_BYTES / NAND_BYTES_PER_PAGE)

// Globals variables.
ROMHDR * volatile const pTOC = (ROMHDR *)-1;

extern BOOL g_bLargeBlock;

// Function in "Nand.c"
void NAND_Init(void);
BOOL NAND_ReadPage(UINT32 block, UINT32 page, volatile BYTE *buffer);

typedef void (*PFN_IMAGE_LAUNCH)();

/*
	@func   BOOLEAN | SetupCopySection | Copies the IPL image's copy section data (initialized globals) to the correct fix-up location.  Once completed, the IPLs initialized globals are valid.
	@rdesc  TRUE == Success and FALSE == Failure.
	@comm
	@xref
*/
static BOOLEAN SetupCopySection(ROMHDR *const pTOC)
{
	// This code doesn't make use of global variables so there are no copy sections.  To reduce code size, this is a stub function...
	return(TRUE);
}


/*
	@func   void | main | C entrypoint function for the Steppingstone loader.
	@rdesc  None.
	@comm
	@xref
*/
void main(void)
{
	register nBlock;
	register nPage;
	register nBadBlocks;
	volatile unsigned char *pBuf;

	// Set up copy section (initialized globals).
	//
	// NOTE: after this call, globals become valid.
	//
	SetupCopySection(pTOC);

	// Enable the ICache.
	//System_EnableICache();		// I-Cache was already enabled in startup.s

	// Set up all GPIO ports.
	Port_Init();
	Led_Display(0xf);

	// UART Initialize
	Uart_Init();

	Uart_SendString("\r\n\r\nWM 6.0 Steploader for SMDK6410\r\n");

	// Initialize the NAND flash interface.
	Uart_SendString("NAND Initialize\n\r");
	NAND_Init();

	// Copy image from NAND flash to RAM.
	pBuf = (unsigned char *)LOAD_ADDRESS_PHYSICAL;

    // NOTE: we assume that this Steppingstone loader occupies *part* the first (good) NAND flash block.  More
    // specifically, the loader takes up 4096 bytes (or 8 NAND pages) of the first block.  We'll start our image
    // copy on the very next page.

	nBadBlocks = 0;

	for (nPage = NAND_COPY_PAGE_OFFSET ; nPage < (LOAD_SIZE_PAGES + NAND_COPY_PAGE_OFFSET) ; nPage++)
	{
		nBlock = ((nPage / NAND_PAGES_PER_BLOCK) + nBadBlocks);

		if (!NAND_ReadPage(nBlock, (nPage % NAND_PAGES_PER_BLOCK), pBuf))
		{
			if ((nPage % NAND_PAGES_PER_BLOCK) != 0)
			{
				Led_Display(0x9);	// real ECC Error.
				Uart_SendString("ECC Error.\r\n");

				while(1)
				{
					// Spin forever...
				}
			}

			// ECC error on a block boundary is (likely) a bad block - retry the page 0 read on the next block.
			nBadBlocks++;
			nPage--;

			continue;
		}

		pBuf += NAND_BYTES_PER_PAGE;
	}

	Led_Display(0x6);

	Uart_SendString("Launch NBL2 ...\n\r");

	((PFN_IMAGE_LAUNCH)(LOAD_ADDRESS_PHYSICAL))();
}

