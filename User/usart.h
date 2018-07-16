/**
  ******************************************************************************
  * @file    usart.h
  * $Author: wdluo $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   串口操作相关函数声明.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USART_H
#define _USART_H
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stm32f10x.h>

/* Exported Functions --------------------------------------------------------*/
void USART_RCC_Configuration(void);
void USART_GPIO_Configuration(void);
void USART_Configuration(void);
void USART1SendString(char *cmd,u16 len);
void USART1SendByte(uint8_t SendData);
void USART1SendNByte(uint8_t *cmd,u16 len);
void u1_printf(char* fmt,...);

#endif /*_usart_H*/

/*********************************END OF FILE**********************************/
