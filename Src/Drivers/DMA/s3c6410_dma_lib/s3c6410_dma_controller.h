#ifndef __S3C6410_DMA_CONTROLLER_H__
#define __S3C6410_DMA_CONTROLLER_H__

#if __cplusplus
extern "C"
{
#endif

#define MAX_LLI_ENTRY		(32)

typedef enum
{
	DMA_UART0_TX = 0,	// DMAC0, 0
	DMA_UART0_RX,		// DMAC0, 1
	DMA_UART1_TX,		// DMAC0, 2
	DMA_UART1_RX,		// DMAC0, 3
	DMA_UART2_TX,		// DMAC0, 4
	DMA_UART2_RX,		// DMAC0, 5
	DMA_UART3_TX,		// DMAC0, 6
	DMA_UART3_RX,		// DMAC0, 7
	DMA_PCM0_TX,		// DMAC0, 8
	DMA_PCM0_RX,		// DMAC0, 9
	DMA_I2S0_TX,		// DMAC0, 10
	DMA_I2S0_RX,		// DMAC0, 11
	DMA_SPI0_TX,		// DMAC0, 12
	DMA_SPI0_RX,		// DMAC0, 13
	DMA_I2S_V40_TX,	// DMAC0, 14
	DMA_I2S_V40_RX,	// DMAC0, 15
	DMA_PCM1_TX,		// DMAC1, 0
	DMA_PCM1_RX,		// DMAC1, 1
	DMA_I2S1_TX,		// DMAC1, 2
	DMA_I2S1_RX,		// DMAC1, 3
	DMA_SPI1_TX,		// DMAC1, 4
	DMA_SPI1_RX,		// DMAC1, 5
	DMA_AC97_PCMOUT,	// DMAC1, 6
	DMA_AC97_PCMIN,	// DMAC1, 7
	DMA_AC97_MICIN,	// DMAC1, 8
	DMA_PWM,			// DMAC1, 9
	DMA_IRDA,			// DMAC1, 10
	DMA_EXTERNAL,		// DMAC1, 11
	DMA_GPS,			// DMAC1, 12
	SDMA_UART0_TX = 100,	// SDMAC0, 0
	SDMA_UART0_RX,		// SDMAC0, 1
	SDMA_UART1_TX,		// SDMAC0, 2
	SDMA_UART1_RX,		// SDMAC0, 3
	SDMA_UART2_TX,		// SDMAC0, 4
	SDMA_UART2_RX,		// SDMAC0, 5
	SDMA_UART3_TX,		// SDMAC0, 6
	SDMA_UART3_RX,		// SDMAC0, 7
	SDMA_PCM0_TX,		// SDMAC0, 8
	SDMA_PCM0_RX,		// SDMAC0, 9
	SDMA_I2S0_TX,		// SDMAC0, 0
	SDMA_I2S0_RX,		// SDMAC0, 1
	SDMA_SPI0_TX,		// SDMAC0, 2
	SDMA_SPI0_RX,		// SDMAC0, 3
	SDMA_I2S_V40_TX,	// SDMAC0, 4
	SDMA_I2S_V40_RX,	// SDMAC0, 5
	SDMA_PCM1_TX,		// SDMAC1, 0
	SDMA_PCM1_RX,		// SDMAC1, 1
	SDMA_I2S1_TX,		// SDMAC1, 2
	SDMA_I2S1_RX,		// SDMAC1, 3
	SDMA_SPI1_TX,		// SDMAC1, 4
	SDMA_SPI1_RX,		// SDMAC1, 5
	SDMA_AC97_PCMOUT,	// SDMAC1, 6
	SDMA_AC97_PCMIN,	// SDMAC1, 7
	SDMA_AC97_MICIN,	// SDMAC1, 8
	SDMA_PWM,			// SDMAC1, 9
	SDMA_IRDA,			// SDMAC1, 10
	SDMA_EXTERNAL,		// SDMAC1, 11
	SDMA_SECU_RX,			// SDMAC1, 14
	SDMA_SECU_TX,			// SDMAC1, 15
	DMA_SOURCE_MAX,
	DMA_MEM = 199
//	DMA_RESERVED0,		// DMAC1, 12
//	DMA_RESERVED1,		// DMAC1, 13
//	DMA_SECU_RX,		// DMAC1, 14
//	DMA_SECU_TX		// DMAC1, 15
} DMA_SOURCE;

typedef enum
{
	DMAC0_UART0_TX = 0,	// DMAC0, 0
	DMAC0_UART0_RX,		// DMAC0, 1
	DMAC0_UART1_TX,		// DMAC0, 2
	DMAC0_UART1_RX,		// DMAC0, 3
	DMAC0_UART2_TX,		// DMAC0, 4
	DMAC0_UART2_RX,		// DMAC0, 5
	DMAC0_UART3_TX,		// DMAC0, 6
	DMAC0_UART3_RX,		// DMAC0, 7
	DMAC0_PCM0_TX,		// DMAC0, 8
	DMAC0_PCM0_RX,		// DMAC0, 9
	DMAC0_I2S0_TX,			// DMAC0, 10
	DMAC0_I2S0_RX,			// DMAC0, 11
	DMAC0_SPI0_TX,			// DMAC0, 12
	DMAC0_SPI0_RX,			// DMAC0, 13
	DMAC0_I2S_V40_TX,		// DMAC0, 14
	DMAC0_I2S_V40_RX,		// DMAC0, 15
} DMAC0_SOURCE;

typedef enum
{
	DMAC1_PCM1_TX = 0,		// DMAC1, 0
	DMAC1_PCM1_RX,		// DMAC1, 1
	DMAC1_I2S1_TX,			// DMAC1, 2
	DMAC1_I2S1_RX,			// DMAC1, 3
	DMAC1_SPI1_TX,			// DMAC1, 4
	DMAC1_SPI1_RX,			// DMAC1, 5
	DMAC1_AC97_PCMOUT,	// DMAC1, 6
	DMAC1_AC97_PCMIN,		// DMAC1, 7
	DMAC1_AC97_MICIN,		// DMAC1, 8
	DMAC1_PWM,			// DMAC1, 9
	DMAC1_IRDA,			// DMAC1, 10
	DMAC1_EXTERNAL,		// DMAC1, 11
	DMAC1_GPS,				// DMAC1, 12
	DMAC1_RESERVED0,		// DMAC1, 12
	DMAC1_RESERVED1		// DMAC1, 13
} DMAC1_SOURCE;

typedef enum
{
	SDMAC0_UART0_TX = 0,	// SDMAC0, 0
	SDMAC0_UART0_RX,		// SDMAC0, 1
	SDMAC0_UART1_TX,		// SDMAC0, 2
	SDMAC0_UART1_RX,		// SDMAC0, 3
	SDMAC0_UART2_TX,		// SDMAC0, 4
	SDMAC0_UART2_RX,		// SDMAC0, 5
	SDMAC0_UART3_TX,		// SDMAC0, 6
	SDMAC0_UART3_RX,		// SDMAC0, 7
	SDMAC0_PCM0_TX,		// SDMAC0, 8
	SDMAC0_PCM0_RX,		// SDMAC0, 9
	SDMAC0_I2S0_TX,		// SDMAC0, 10
	SDMAC0_I2S0_RX,		// SDMAC0, 11
	SDMAC0_SPI0_TX,		// SDMAC0, 12
	SDMAC0_SPI0_RX,		// SDMAC0, 13
	SDMAC0_I2S_V40_TX,		// SDMAC0, 14
	SDMAC0_I2S_V40_RX,		// SDMAC0, 15
} SDMAC0_SOURCE;

typedef enum
{
	SDMAC1_PCM1_TX = 0,		// SDMAC1, 0
	SDMAC1_PCM1_RX,		// SDMAC1, 1
	SDMAC1_I2S1_TX,			// SDMAC1, 2
	SDMAC1_I2S1_RX,			// SDMAC1, 3
	SDMAC1_SPI1_TX,			// SDMAC1, 4
	SDMAC1_SPI1_RX,			// SDMAC1, 5
	SDMAC1_AC97_PCMOUT,	// SDMAC1, 6
	SDMAC1_AC97_PCMIN,		// SDMAC1, 7
	SDMAC1_AC97_MICIN,		// SDMAC1, 8
	SDMAC1_PWM,			// SDMAC1, 9
	SDMAC1_IRDA,			// SDMAC1, 10
	SDMAC1_EXTERNAL,		// SDMAC1, 11
	SDMAC1_RESERVED0,		// SDMAC1, 12
	SDMAC1_RESERVED1,		// SDMAC1, 13
	SDMAC1_SECU_RX,		// SDMAC1, 14
	SDMAC1_SECU_TX		// SDMAC1, 15
} SDMAC1_SOURCE;

typedef enum
{
	DMAC0 = 0,
	DMAC1,
	SDMAC0,
	SDMAC1
} DMAC;

typedef enum
{
	DMA_CH0 = 0,
	DMA_CH1,
	DMA_CH2,
	DMA_CH3,
	DMA_CH4,
	DMA_CH5,
	DMA_CH6,
	DMA_CH7
} DMAC_CH;

typedef enum
{
	AHB_M1,	// Memory
	AHB_M2		// Peripheral
} AHB_MASTER;

typedef enum
{
	MEM_TO_MEM = 0,
	MEM_TO_PERI,
	PERI_TO_MEM,
	PERI_TO_PERI
} FLOW_CONTROL;

typedef enum
{
	FIXED =0,
	INCREASE
} ADDRESS_UPDATE;

typedef enum
{
	BYTE_UNIT = 0,
	HWORD_UNIT,
	WORD_UNIT
} TRANSFER_UNIT;

typedef enum
{
	BURST_1 = 0,
	BURST_4,
	BURST_8,
	BURST_16,
	BURST_32,
	BURST_64,
	BURST_128,
	BURST_256
} BURST_SIZE;

typedef enum
{
	LLI_NEXT_ENTRY,
	LLI_FIRST_ENTRY,
	LLI_THIS_IS_END
} LLI_NEXT_ITEM;

typedef enum
{
	NO_INT_PEND = 0x0,
	TC_INT_PEND = 0x1,
	ERR_INT_PEND = 0x2,
	TC_AND_ERR_INT_PEND = 0x3
} DMA_INT_STATUS;

typedef struct
{
	BOOL bValid;
	DMA_SOURCE DMASrc;
	DMAC Controller;
	DMAC_CH Channel;
	DWORD dwIRQ;
	void *pCHReg;
	int LLICount;
	unsigned int LLIPhyAddr;
	unsigned int LLIVirAddr;
	AHB_MASTER LLIAHBM;
	AHB_MASTER SrcAHBM;
	AHB_MASTER DstAHBM;
	unsigned int SrcPeri;
	unsigned int DstPeri;
	ADDRESS_UPDATE SrcUpdate;
	ADDRESS_UPDATE DstUpdate;
	TRANSFER_UNIT SrcUnit;
	TRANSFER_UNIT DstUnit;
	BURST_SIZE SrcBurst;
	BURST_SIZE DstBurst;
	FLOW_CONTROL FlowCtrl;
} DMA_CH_CONTEXT;

typedef enum
{
	DMA_SUCCESS,
	DMA_ERROR_NULL_PARAMETER,
	DMA_ERROR_ILLEGAL_PARAMETER,
	DMA_ERROR_NOT_INITIALIZED,
	DMA_ERROR_NOT_IMPLEMENTED,
	DMA_ERROR_XXX
} DMA_ERROR;

DMA_ERROR DMA_initialize_register_address(void *pDMAC0Reg, void *pDMAC1Reg, void *pSysConReg);

BOOL DMA_request_channel(DMA_CH_CONTEXT *pCtxt, DMA_SOURCE DMASrc);
BOOL DMA_release_channel(DMA_CH_CONTEXT *pCtxt);

DMA_ERROR DMA_initialize_channel(DMA_CH_CONTEXT *pCtxt, BOOL bSync);
DMA_ERROR DMA_set_channel_source(DMA_CH_CONTEXT *pCtxt, unsigned int uiSrcAddr, TRANSFER_UNIT Unit, BURST_SIZE Burst, ADDRESS_UPDATE Update);
DMA_ERROR DMA_set_channel_destination(DMA_CH_CONTEXT *pCtxt, unsigned int uiDstAddr, TRANSFER_UNIT Unit, BURST_SIZE Burst, ADDRESS_UPDATE Update);
DMA_ERROR DMA_set_channel_transfer_size(DMA_CH_CONTEXT *pCtxt, unsigned int uiByteCount);

DMA_ERROR DMA_initialize_LLI(DMA_CH_CONTEXT *pCtxt, int iLLICount);
DMA_ERROR DMA_set_initial_LLI(DMA_CH_CONTEXT *pCtxt, int iIntialLLIEntryNumber);
DMA_ERROR DMA_set_LLI_entry(DMA_CH_CONTEXT *pCtxt, int iEntryNumber, LLI_NEXT_ITEM NextItem, unsigned int uiSrcAddr, unsigned int uiDstAddr, unsigned int uiByteCount);

DMA_ERROR DMA_channel_start(DMA_CH_CONTEXT *pCtxt);
DMA_ERROR DMA_channel_stop(DMA_CH_CONTEXT *pCtxt);

DMA_INT_STATUS DMA_get_interrupt_status(DMA_CH_CONTEXT *pCtxt);
void DMA_set_interrupt_mask(DMA_CH_CONTEXT *pCtxt);
void DMA_clear_interrupt_mask(DMA_CH_CONTEXT *pCtxt);
void DMA_clear_interrupt_pending(DMA_CH_CONTEXT *pCtxt);

static BOOL DMA_dmac0_enable(void);
static BOOL DMA_dmac0_disable(void);
static BOOL DMA_dmac1_enable(void);
static BOOL DMA_dmac1_disable(void);
static BOOL DMA_allocate_LLI_context(DMA_CH_CONTEXT *pCtxt);
static BOOL DMA_free_LLI_context(DMA_CH_CONTEXT *pCtxt);
static BOOL DMA_lock(void);
static BOOL DMA_unlock(void);

#if __cplusplus
}
#endif

#endif	// __S3C6410_DMA_CONTROLLER_H__
