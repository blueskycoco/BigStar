/**************************************************************************************
* 
*	Project Name : IIC Driver 
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is PDD layer for IIC Samsung driver. 
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : iic_pdd.h
*  
*	File Description : This file declare PDD functions for IIC driver.
*
*	Author : JeGeon.Jung
*	Dept. : AP Development Team
*	Created Date : 2007/06/12
*	Version : 0.1 
* 
*	History
*	- Created(JeGeon.Jung 2007/06/12)
*  
*	Todo
*
*
*	Note
*
**************************************************************************************/
#ifndef __IIC_PDD_H__
#define __IIC_PDD_H__

#include <iic.h>

#define DEFAULT_SLAVE_ADDRESS		0xc0
#define DEFAULT_INTERRUPT_ENABLE	1

//		declare functions
BOOL 		HW_Init 		(PHW_INIT_INFO pInitContext);
BOOL 		HW_Deinit 		(PHW_INIT_INFO pInitContext);
BOOL 		HW_OpenFirst 	(PHW_OPEN_INFO pOpenContext);
BOOL 		HW_CloseLast 	(PHW_OPEN_INFO pOpenContext);

BOOL 		HW_Open		 	(PHW_OPEN_INFO pOpenContext);
BOOL 		HW_Close		(PHW_OPEN_INFO pOpenContext);

BOOL		HW_PowerUp		(PHW_INIT_INFO pInitContext);
BOOL		HW_PowerDown	(PHW_INIT_INFO pInitContext);

VOID 		HW_SetRegister 	(PHW_OPEN_INFO pOpenContext);
VOID 		HW_SetClock		(PHW_OPEN_INFO pOpenContext);

BOOL		HW_Read		 	(PHW_OPEN_INFO pOpenContext, PIIC_IO_DESC pInData ,PIIC_IO_DESC pOutData);
BOOL		HW_Write		(PHW_OPEN_INFO pOpenContext, PIIC_IO_DESC pInData);

#endif //	__IIC_PDD_H__
