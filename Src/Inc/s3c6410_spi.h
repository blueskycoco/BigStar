//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//------------------------------------------------------------------------------
//
//  Header: s3c6410_spi.h
//
//  Defines the Serial Peripheral Interface (SPI) controller CPU register layout and
//  definitions.
//
#ifndef __S3C6410_SPI_H
#define __S3C6410_SPI_H

#if __cplusplus
	extern "C"
	{
#endif

#include <winioctl.h>
#include <ceddk.h>

typedef struct  
{
	UINT32      CH_CFG;        // 00
	UINT32      CLK_CFG;       // 04
	UINT32      MODE_CFG;      // 08
	UINT32      SLAVE_SEL;     // 0C
	UINT32      SPI_INT_EN;    // 10
	UINT32      SPI_STATUS;    // 14
	UINT32      SPI_TX_DATA;   // 18
	UINT32      SPI_RX_DATA;   // 1C
	UINT32      PACKET_COUNT;  // 20
	UINT32      PENDING_CLEAR; // 24
	UINT32      SWAP_CFG;      // 28
	UINT32      FB_CLK_SEL;    // 2C
} S3C6410_SPI_REG, *PS3C6410_SPI_REG; 



#define SPI_IOCTL_SET_CONFIG				CTL_CODE(FILE_DEVICE_SERIAL_PORT, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SPI_IOCTL_GET_CONFIG				CTL_CODE(FILE_DEVICE_SERIAL_PORT, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SPI_IOCTL_CLR_TXBUFF				CTL_CODE(FILE_DEVICE_SERIAL_PORT, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SPI_IOCTL_CLR_RXBUFF				CTL_CODE(FILE_DEVICE_SERIAL_PORT, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SPI_IOCTL_STOP       				CTL_CODE(FILE_DEVICE_SERIAL_PORT, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SPI_IOCTL_START       			CTL_CODE(FILE_DEVICE_SERIAL_PORT, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SPI_IOCTL_SET_CALLBACK       	CTL_CODE(FILE_DEVICE_SERIAL_PORT, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SPI_IOCTL_CLR_CALLBACK       	CTL_CODE(FILE_DEVICE_SERIAL_PORT, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SPI_IOCTL_IS_SLAVE_READY       		CTL_CODE(FILE_DEVICE_SERIAL_PORT, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SPI_IOCTL_SET_SPEED 				CTL_CODE(FILE_DEVICE_SERIAL_PORT, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define SPI_MASTER_MODE 					0
#define SPI_SLAVE_MODE						1		

#define SPI_WAIT_TIMEOUT					(0x10000000)
#define SPI_IS_TIMEOUT(count) 				((count & SPI_WAIT_TIMEOUT) ? TRUE : FALSE)

typedef enum {
	SPI_FORMAT_0,		
	SPI_FORMAT_1,		
	SPI_FORMAT_2,		
	SPI_FORMAT_3
} SPI_FORMAT;

typedef enum {
	SPI_BYTE_BUS = 0,
	SPI_HWORD_BUS,
	SPI_WORD_BUS,
	SPI_DWORD_BUS
} SPI_BUS_WIDTH;

typedef enum {
	SPI_BYTE_CHN,
	SPI_HWORD_CHN,
	SPI_WORD_CHN,
	SPI_DWORD_CHN
} SPI_CHN_WIDTH;

typedef enum {
	SPI_SWAP_DISABLE,
	SPI_BIT_SWAP=2,
	SPI_BYTE_SWAP=4,
	SPI_WORD_SWAP=8
} SPI_SWAP_SIZE;

typedef struct {
	PVOID             			VirtualAddress;
	UINT32			  			PhysicalAddress;
} SPI_DMA_BUFFER, *PSPI_DMA_BUFFER;


typedef struct {
	PVOID            	VirtualAddress;
	PVOID			PhysicalAddress;
} DMA_BUFFER, *PDMA_BUFFER;

typedef struct {
	DWORD						dwMode;
	
	BOOL						bUseRxFIFO;
	BOOL                      	bUseRxDMA;
	BOOL						bUseRxIntr;
	DWORD						dwRxTrigger;
	SPI_BUS_WIDTH				RxBusWidth;
	SPI_CHN_WIDTH				RxChnWidth;
	SPI_SWAP_SIZE				RxSwap;
	BOOL						bRxDMABurst;

	BOOL						bUseTxFIFO;
	BOOL                      	bUseTxDMA;
	BOOL						bUseTxIntr;
	DWORD						dwTxTrigger;
	SPI_BUS_WIDTH				TxBusWidth;
	SPI_CHN_WIDTH				TxChnWidth;
	SPI_SWAP_SIZE				TxSwap;
	BOOL						bTxDMABurst;

	DWORD						dwLLICount; // for DMA LLI setting
	BOOL						bLoopedLLI;	

	DWORD						dwLineStrength;

	DWORD						dwFBClkSel;
	
	DWORD						dwPrescaler;
	DWORD						dwTimeOutVal;
	SPI_FORMAT					Format;
} SPI_SET_CONFIG, *PSPI_SET_CONFIG;

typedef struct {
	DWORD						dwMode;
	
	BOOL						bUseRxFIFO;
	BOOL                      	bUseRxDMA;
	BOOL						bUseRxIntr;
	DWORD						dwRxTrigger;
	SPI_BUS_WIDTH				RxBusWidth;
	SPI_CHN_WIDTH				RxChnWidth;
	SPI_SWAP_SIZE				RxSwap;
	BOOL						bRxDMABurst;

	BOOL						bUseTxFIFO;
	BOOL                      	bUseTxDMA;
	BOOL						bUseTxIntr;
	DWORD						dwTxTrigger;
	SPI_BUS_WIDTH				TxBusWidth;
	SPI_CHN_WIDTH				TxChnWidth;
	SPI_SWAP_SIZE				TxSwap;
	BOOL						bTxDMABurst;
	
	DWORD						dwLLICount; // for DMA LLI setting
	BOOL						bLoopedLLI;	
	
	DWORD						dwPrescaler;
	DWORD						dwTimeOutVal;
	SPI_FORMAT					Format;
} SPI_GET_CONFIG, *PSPI_GET_CONFIG;

//------------------------------------------------------------------------------

#if __cplusplus
   	}
#endif

#endif	// __S3C6410_SPI_H
