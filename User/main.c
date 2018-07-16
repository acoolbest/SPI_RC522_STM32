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
#include "buzzer.h"
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

#define RS485_ADDR_0BIT  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
#define RS485_ADDR_1BIT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)
#define RS485_ADDR_2BIT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
#define RS485_ADDR_3BIT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)
#define RS485_ADDR_4BIT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)

void init_RS485_addr(u8 * rs485_addr)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	*rs485_addr = (RS485_ADDR_0BIT << 0) 
		| (RS485_ADDR_1BIT << 1) 
		| (RS485_ADDR_2BIT << 2)
		| (RS485_ADDR_3BIT << 3)
		| (RS485_ADDR_4BIT << 4);
	printf("RS485 addr is 0x%02x\n", *rs485_addr);
}

//relay on PB4
void relay_on(void)
{
  GPIO_SetBits(GPIOB,GPIO_Pin_4);
}
//relay off PB4
void relay_off(void)
{
  GPIO_ResetBits(GPIOB,GPIO_Pin_4);
}

void relay_init(void)
{
  GPIO_InitTypeDef 
  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

int main(void)
{
	uint8_t i;
	uint8_t Card_Type1[2];
	uint8_t Card_ID[4];
	uint8_t Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};    //{0x11,0x11,0x11,0x11,0x11,0x11};   //密码
	uint8_t Card_Data[16];
	uint8_t status;
	uint8_t addr;
	SystemInit();//系统时钟初始化
	USART_Configuration();//串口1初始化
	delay_init();
	RC522_IO_Init();
	PcdReset();
	PcdAntennaOff();
	delay_ms(2);
	PcdAntennaOn();

	relay_init();
	init_RS485_addr(&addr);
	TIM3_PWM_Init(14399,10);
	#if 0
	while(1)
	{
		delay_ms(10);
		if(MI_OK==PcdRequest(PICC_REQALL, Card_Type1))
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];
			//sprintf(print_buf, "Card Type(0x%04X):",cardType);
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
		}
	}
	#else
	char print_buf[256] = {"0123456789012345678901234567890123456789012345678901234567890123456789"};
	for(i=0;i<100;i++) 
	{
		//print_buf[i] = i+1;
	}
	uint16_t print_len = 0;
	while(1)
	{
		delay_ms(10);
		//printf("Card_Type(0x%04X)\n",0x0400);
		//u1_printf("%s\n","lalalalalalalalala222244444666");
		//USART1SendNByte((uint8_t *)print_buf, strlen(print_buf));
		//delay_ms(1000);
		//continue;
		if(MI_OK==PcdRequest(PICC_REQIDL, Card_Type1))
		//if(MI_OK==PcdRequest(PICC_REQALL, Card_Type1))	
		{
			buzzer_play(BUZZER_PLAY_UNLOCK);
			relay_on();
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];
			sprintf(print_buf, "Card Type(0x%04X):",cardType);
			//printf("Card Type(0x%04X):",cardType);
			switch(cardType){
			case 0x4400:
					printf("%s", strcat(print_buf, "Mifare UltraLight\n\r"));
					break;
			case 0x0400:
					//printf("%s", strcat(print_buf, "Mifare One(S50)\n\r"));
					print_len = sprintf(print_buf, 
						"Card Type(0x%04X):Mifare One(S50)\n\r",
						cardType);
					USART1SendString(print_buf,print_len);
					break;
			case 0x0200:
					printf("%s", strcat(print_buf, "Mifare One(S70)\n\r"));
					break;
			case 0x0800:
					printf("%s", strcat(print_buf, "Mifare Pro(X)\n\r"));
					break;
			case 0x4403:
					printf("%s", strcat(print_buf, "Mifare DESFire\n\r"));
					break;
			default:
					printf("%s", strcat(print_buf, "Unknown Card\n\r"));
					continue;
			}
			//delay_ms(10);
			status = PcdAnticoll(Card_ID);//防冲撞
			if(status != MI_OK){
					printf("Anticoll Error\n\r");
					continue;
			}else{
					print_len = sprintf(print_buf, 
						"Serial Number:%02X%02X%02X%02X\n\r",
						Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
					//printf("%s",print_buf);
					USART1SendString(print_buf,print_len);
			}

			status = PcdSelect(Card_ID);  //选卡
			if(status != MI_OK){
					printf("Select Card Error\n\r");
					continue;
			}
			printf("PcdSelect OK\n\r");
			#if 1
			status = PcdAuthState(PICC_AUTHENT1A,5,Card_KEY,Card_ID);
			if(status != MI_OK){
					printf("Auth State Error\n\r");
					continue;
			}
			USART1SendNByte((uint8_t *)print_buf, 100);
			printf("Auth State OK\n\r");
			memset(Card_ID,1,4);
			memset(Card_Data,1,16);
			Card_Data[0]=0xaa;
			status = PcdWrite(5,Card_Data);                   //写入0XAA,0X01,0X01……
			if(status != MI_OK){
					printf("Card Write Error\n\r");
					continue;
			}
			printf("Card Write aa OK                  \n\r");
			memset(Card_Data,0,16);
			delay_ms(8);
			
			status = PcdRead(5,Card_Data);                    //再一次把它读取出来16字节的卡片数据
			if(status != MI_OK){
					printf("Card Read Error\n\r");
					continue;
			}else{
				for(i=0;i<16;i++){
					printf("%02X \n",Card_Data[i]);
				}
				printf("aaaaa                      done                     \n\r");
			}
			printf("Card Read aa done\n\r");
			memset(Card_Data,2,16);
			Card_Data[0]=0xbb;
			delay_ms(8);
			status = PcdWrite(5,Card_Data);                   //写入0Xbb,0X02,0X02……
			if(status != MI_OK){
					printf("Card Write Error\n\r");
					continue;
			}
			printf("Card Write bb OK                      \n\r");
			delay_ms(8);
			
			status = PcdRead(5,Card_Data);                    //再一次把它读取出来16字节的卡片数据
			if(status != MI_OK){
					printf("Card Read Error\n\r");
					continue;
			}else{
				for(i=0;i<16;i++){
					printf("%02X \n",Card_Data[i]);
				}
				printf("bbbbb                        done                         \n\r");
			}
			printf("Card Read bb done\n\r");
			memset(Card_Data,0,16);
			#endif
			PcdHalt();
			buzzer_play(BUZZER_PLAY_LOCK);
			relay_off();
		}
	}
	#endif
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
