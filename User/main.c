/**
  ******************************************************************************
  * @file    main.c
  * $Author: wdluo $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   主函数.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "spi_driver.h"
#include "RC522.h"
#include "delay.h"
#include "string.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  串口打印输出W25X32操作相关数据
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t i;
	uint8_t Card_Type1[2];
	uint8_t Card_ID[4];
	uint8_t Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};    //{0x11,0x11,0x11,0x11,0x11,0x11};   //密码
	uint8_t Card_Data[16];
	uint8_t status;
	SystemInit();//系统时钟初始化
	USART_Configuration();//串口1初始化
	printf("\x0c\0");printf("\x0c\0");//超级终端清屏
	printf("\033[1;40;32m");//设置超级终端背景为黑色，字符为绿色
	printf("\r\n*******************************************************************************");
	printf("\r\n************************ Copyright 2009-2012, ViewTool ************************");
	printf("\r\n************************* http://www.viewtool.com/bbs *************************");
	printf("\r\n***************************** All Rights Reserved *****************************");
	printf("\r\n*******************************************************************************");
	printf("\r\n");
	delay_init();
	RC522_IO_Init();
	PcdReset();
	PcdAntennaOff();
	delay_ms(2);
	PcdAntennaOn();
	while(1)
	{
		delay_ms(10);
		if(MI_OK==PcdRequest(0x52, Card_Type1))
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];
			printf("Card Type(0x%04X):",cardType);
			switch(cardType){
			case 0x4400:
					printf("Mifare UltraLight\n\r");
					break;
			case 0x0400:
					printf("Mifare One(S50)\n\r");
					break;
			case 0x0200:
					printf("Mifare One(S70)\n\r");
					break;
			case 0x0800:
					printf("Mifare Pro(X)\n\r");
					break;
			case 0x4403:
					printf("Mifare DESFire\n\r");
					break;
			default:
					printf("Unknown Card\n\r");
					continue;
			}
			//delay_ms(10);
			status = PcdAnticoll(Card_ID);//防冲撞
			if(status != MI_OK){
					printf("Anticoll Error\n\r");
					continue;
			}else{
					printf("Serial Number:%02X%02X%02X%02X\n\r",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			}
			status = PcdSelect(Card_ID);  //选卡
			if(status != MI_OK){
					printf("Select Card Error\n\r");
					continue;
			}
			status = PcdAuthState(PICC_AUTHENT1A,5,Card_KEY,Card_ID);
			if(status != MI_OK){
					printf("Auth State Error\n\r");
					continue;
			}
			memset(Card_ID,1,4);
			memset(Card_Data,1,16);
			Card_Data[0]=0xaa;
			status = PcdWrite(5,Card_Data);                   //写入0XAA,0X01,0X01……
			if(status != MI_OK){
					printf("Card Write Error\n\r");
					continue;
			}
			memset(Card_Data,0,16);
			delay_us(8);
			
			status = PcdRead(5,Card_Data);                    //再一次把它读取出来16字节的卡片数据
			if(status != MI_OK){
					printf("Card Read Error\n\r");
					continue;
			}else{
				for(i=0;i<16;i++){
					printf("%02X ",Card_Data[i]);
				}
				printf("\n\r");
			}
			
			memset(Card_Data,2,16);
			Card_Data[0]=0xbb;
			delay_us(8);
			status = PcdWrite(5,Card_Data);                   //写入0Xbb,0X02,0X02……
			if(status != MI_OK){
					printf("Card Write Error\n\r");
					continue;
			}
			delay_us(8);
			
			status = PcdRead(5,Card_Data);                    //再一次把它读取出来16字节的卡片数据
			if(status != MI_OK){
					printf("Card Read Error\n\r");
					continue;
			}else{
				for(i=0;i<16;i++){
					printf("%02X ",Card_Data[i]);
				}
				printf("\n\r");
			}
			memset(Card_Data,0,16);
			PcdHalt();
		}
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  报告在检查参数发生错误时的源文件名和错误行数
  * @param  file 源文件名
  * @param  line 错误所在行数 
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* 用户可以增加自己的代码用于报告错误的文件名和所在行数,
       例如：printf("错误参数值: 文件名 %s 在 %d行\r\n", file, line) */

    /* 无限循环 */
    while (1)
    {
    }
}
#endif

/*********************************END OF FILE**********************************/
