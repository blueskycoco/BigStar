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
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:	DMA.H

Abstract:		Definitions for the DMA controller found on the Samsung 2440 CPU.
  
Notes:			

Environment:	
-----------------------------------------------------------------------------*/


#include <windows.h>

//===================== Register Configuration Constants ======================


// DMA0, SDMA0
#define DMA_UART0_DMA0  (0)
#define DMA_UART0_DMA1  (1)
#define DMA_UART1_DMA0  (2)
#define DMA_UART1_DMA1  (3)
#define DMA_UART2_DMA0  (4)
#define DMA_UART2_DMA1  (5)
#define DMA_UART3_DMA0  (6)
#define DMA_UART3_DMA1  (7)
#define DMA_PCM0_TX     (8)
#define DMA_PCM0_RX     (9)
#define DMA_I2S0_TX     (10)
#define DMA_I2S0_RX     (11)
#define DMA_SPI0_TX     (12)
#define DMA_SPI0_RX     (13)
#define DMA_HSI_TX      (14)
#define DMA_HSI_RX      (15)

// DMA1, SDMA1
#define DMA_PCM1_TX     (0)
#define DMA_PCM1_RX     (1)
#define DMA_I2S1_TX     (2)
#define DMA_I2S1_RX     (3)
#define DMA_SPI1_TX     (4)
#define DMA_SPI1_RX     (5)
#define DMA_AC_PCMOUT   (6)
#define DMA_AC_PCMIN    (7)
#define DMA_AC_MICIN    (8)
#define DMA_PWM         (9)
#define DMA_IRDA        (10)
#define DMA_EXTERNAL    (11)
#define DMA_RESERVED0   (12)
#define DMA_RESERVED1   (13)
#define DMA_SECU_RX     (14)
#define DMA_SECU_TX     (15)

#define STOP_DMA_TRANSFER (1<<0)
#define ENABLE_DMA_CHANNEL (1<<0)

//----- Register definitions for DISRCn control register -----
//
// bits[30-0] = start address of source data to transfer

//----- Register definitions for DISRCCn control register -----
//
#define SOURCE_PERIPHERAL_BUS				0x00000002
#define FIXED_SOURCE_ADDRESS				0x00000001


//----- Register definitions for DIDSTn control register -----
//
// bits[30-0] = start address of destination for the transfer


//----- Register definitions for DIDSTCn control register -----
//
#define DESTINATION_PERIPHERAL_BUS			0x00000002
#define FIXED_DESTINATION_ADDRESS			0x00000001


//----- Register definitions for DCONn control register -----
//
#define HANDSHAKE_MODE						0x80000000
#define DREQ_DACK_SYNC						0x40000000
#define GENERATE_INTERRUPT					0x20000000
#define SELECT_BURST_TRANSFER				0x10000000
#define SELECT_WHOLE_SERVICE_MODE			0x08000000

#define DMA_TRIGGERED_BY_HARDWARE			0x00800000
#define NO_DMA_AUTO_RELOAD					0x00400000

// bits[21-20] = select transfer word size
//------------------------------------------------------------
#define TRANSFER_BYTE						0x00000000				// 8  bits
#define TRANSFER_HALF_WORD					0x00100000				// 16 bits
#define TRANSFER_WORD						0x00200000				// 32 bits
//
// bits[19-0] = used to specify the number of transfer operations in a DMA request



//----- Register definitions for DSTATn status register -----
#define DMA_TRANSFER_IN_PROGRESS			0x00100000
//
// bits[19-0] = the current transfer count


//----- Register definitions for DCSRCn configuration register -----
//
// bits[30-0] = current source address for DMA channel n


//----- Register definitions for DCDSTn configuration register -----
//
// bits[30-0] = current destination address for DMA channel n


//----- Register definitions for DMASKTRIGn configuration register -----
//#define STOP_DMA_TRANSFER					0x00000004
#define ENABLE_DMA_CHANNEL					0x00000001



//=============================================================================

