//====================================================================
// File Name : option.h
// Function  : S3C6410 
// Program   : Lee Jae Yong
// Date      : February 22, 2007
// Version   : 0.0
// History
//   0.0 : Programming start (February 22,2007)
//====================================================================

#ifndef __OPTION_H__
#define __OPTION_H__

#define FCLK           400000000  // 400MHz
#define HCLK           (FCLK/4)   // divisor 4
#define PCLK		(FCLK/8)		// divisor 8	// 66 MHz

// BUSWIDTH : 16,32
#define BUSWIDTH    (32)


#define _RAM_STARTADDRESS		(0x30000000)
#define _ISR_STARTADDRESS		(0x33ffff00)
#define _MMUTT_STARTADDRESS	(0x33ff8000)
#define _STACK_BASEADDRESS	(0x33ff8000)
#define HEAPEND					(0x33ff0000)
#define SB_NEED_EXT_ADDR				1
#define LB_NEED_EXT_ADDR				1

//If you use ADS1.x, please define ADS10
#define ADS10 TRUE

void Uart_Init(void);
void Uart_SendByte(int data);
void Uart_SendString(char *pt);
void Uart_SendDWORD(DWORD d, BOOL cr);
char *hex2char(unsigned int val);



// note: makefile,option.a should be changed

#endif /*__OPTION_H__*/
