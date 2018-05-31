
#include "stm32f10x.h"
#include "rc522.h"
#include "delay.h"
#include "spi_driver.h"

void RC522_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RC522_RESET_GPIO_CLK,ENABLE);
	GPIO_InitStruct.GPIO_Pin = RC522_RESET_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RC522_RESET_GPIO_PORT, &GPIO_InitStruct);
	SPI_Configuration(SPI1);
}

/*
/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{  
unsigned char i, ucAddr;

RC522_SCK_RESET();   //MF522_SCK = 0;
RC522_NSEL_RESET();  //MF522_NSS = 0;
ucAddr = ((Address<<1)&0x7E);
RC522_Delay(10);
for(i=8;i>0;i--)
{
//MF522_SI = ((ucAddr&0x80)==0x80);
if((ucAddr&0x80)==0x80)
{
RC522_MOSI_SET();
}
else
{
RC522_MOSI_RESET();
}
RC522_SCK_SET();  //MF522_SCK = 1;
ucAddr <<= 1;
RC522_Delay(10);
RC522_SCK_RESET();  //MF522_SCK = 0;
RC522_Delay(10);
}

for(i=8;i>0;i--)
{
//MF522_SI = ((value&0x80)==0x80);
if((value&0x80)==0x80)
{
RC522_MOSI_SET();
}
else
{
RC522_MOSI_RESET();
}
RC522_SCK_SET();  //MF522_SCK = 1;
value <<= 1;
RC522_Delay(10);
RC522_SCK_RESET();  //MF522_SCK = 0;
RC522_Delay(10);
//         MF522_SCK = 1;
//         value <<= 1;
//         MF522_SCK = 0;
}
RC522_NSEL_SET();  //MF522_NSS = 1;
RC522_SCK_SET();    //MF522_SCK = 1;
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
unsigned char i, ucAddr;
unsigned char ucResult=0;

RC522_SCK_RESET();   //MF522_SCK = 0;
RC522_NSEL_RESET();  //MF522_NSS = 0;
ucAddr = ((Address<<1)&0x7E)|0x80;
RC522_Delay(10);
for(i=8;i>0;i--)
{
//          MF522_SI = ((ucAddr&0x80)==0x80);
//          MF522_SCK = 1;
//          ucAddr <<= 1;
//          MF522_SCK = 0;

if((ucAddr&0x80)==0x80)
{
	RC522_MOSI_SET();
}
else
{
	RC522_MOSI_RESET();
}
RC522_SCK_SET();  //MF522_SCK = 1;
ucAddr <<= 1;
RC522_Delay(10);
RC522_SCK_RESET();  //MF522_SCK = 0;
RC522_Delay(10);
}
for(i=8;i>0;i--)
{
	RC522_SCK_SET();  //MF522_SCK = 1;
	ucResult <<= 1;
	RC522_Delay(10);
	//ucResult|=(bit)MF522_SO;

	// 			 	 if(RC522_MISO_STATUS==1)
	// 				 {
	// 					 ucResult|=0x01;
	// 				 }
	// 				 else
	// 				 {
	// 					 ucResult&=~0x01;
	// 				 }
	ucResult |=RC522_MISO_STATUS;

	RC522_SCK_RESET();  //MF522_SCK = 0;
	RC522_Delay(10);
}

RC522_NSEL_SET();   //MF522_NSS = 1;
RC522_SCK_SET();    //MF522_SCK = 1;
return ucResult;
}
*/
//#define MAXRLEN 18

/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(uint8_t req_code,uint8_t *pTagType)
{
	char status;  
	uint32_t  unLen;
	uint8_t ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);	//����ָʾMIFARECyptol��Ԫ��ͨ�Լ����п�������ͨ�ű����ܵ����
	WriteRawRC(BitFramingReg,0x07);	//���͵����һ���ֽڵ� ��λ
	SetBitMask(TxControlReg,0x03);	//TX1,TX2�ܽŵ�����źŴ��ݾ����͵��Ƶ�13.56�������ز��ź�

	ucComMF522Buf[0] = req_code;	//���� ��Ƭ������

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);	//Ѱ��
	//   UART_send_byte(status);
	if ((status == MI_OK) && (unLen == 0x10))	//Ѱ���ɹ����ؿ�����
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   status = MI_ERR;   }

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(uint8_t *pSnr)
{
	char status;
	uint8_t i,snr_check=0;
	uint32_t  unLen;
	uint8_t ucComMF522Buf[MAXRLEN]; 


	ClearBitMask(Status2Reg,0x08);		//��MFCryptol Onλ ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ
	WriteRawRC(BitFramingReg,0x00);		//����Ĵ��� ֹͣ�շ�
	ClearBitMask(CollReg,0x80);			//��ValuesAfterColl���н��յ�λ�ڳ�ͻ�����

	ucComMF522Buf[0] = PICC_ANTICOLL1;	//��Ƭ����ͻ����
	ucComMF522Buf[1] = 0x20;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);	//�뿨Ƭͨ��

	if (status == MI_OK)
	{
		for (i=0; i<4; i++)
		{   
			*(pSnr+i)  = ucComMF522Buf[i];	//����UID
			snr_check ^= ucComMF522Buf[i];
		}
		if (snr_check != ucComMF522Buf[i])
		{   status = MI_ERR;    }
	}

	SetBitMask(CollReg,0x80);
	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(uint8_t *pSnr)
{
	char status;
	uint8_t i;
	uint32_t  unLen;
	uint8_t ucComMF522Buf[MAXRLEN]; 

	ucComMF522Buf[0] = PICC_ANTICOLL1;
	ucComMF522Buf[1] = 0x70;
	ucComMF522Buf[6] = 0;
	for (i=0; i<4; i++)
	{
		ucComMF522Buf[i+2] = *(pSnr+i);
		ucComMF522Buf[6]  ^= *(pSnr+i);
	}
	CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);

	ClearBitMask(Status2Reg,0x08);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

	if ((status == MI_OK) && (unLen == 0x18))
	{   status = MI_OK;  }
	else
	{   status = MI_ERR;    }

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                   
char PcdAuthState(uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr)
{
	char status;
	uint32_t  unLen;
	uint8_t i,ucComMF522Buf[MAXRLEN]; 

	ucComMF522Buf[0] = auth_mode;
	ucComMF522Buf[1] = addr;
	for (i=0; i<6; i++)
	{    ucComMF522Buf[i+2] = *(pKey+i);   }
	for (i=0; i<6; i++)
	{    ucComMF522Buf[i+8] = *(pSnr+i);   }

	status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
	if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
	{   status = MI_ERR;   }

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          p [OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(uint8_t addr,uint8_t *pData)
{
	char status;
	uint32_t  unLen;
	uint8_t i,ucComMF522Buf[MAXRLEN]; 

	ucComMF522Buf[0] = PICC_READ;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if ((status == MI_OK) && (unLen == 0x90))
	{
		for (i=0; i<16; i++)
		{    *(pData+i) = ucComMF522Buf[i];   }
	}
	else
	{   status = MI_ERR;   }

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          p [IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdWrite(uint8_t addr,uint8_t *pData)
{
	char status;
	uint32_t  unLen;
	uint8_t i,ucComMF522Buf[MAXRLEN]; 

	ucComMF522Buf[0] = PICC_WRITE;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

	if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
	{   status = MI_ERR;   }

	if (status == MI_OK)
	{
		for (i=0; i<16; i++)
		{    ucComMF522Buf[i] = *(pData+i);   }
		CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
		if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
		{   status = MI_ERR;   }
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                
char PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *pValue)
{
	char status;
	uint32_t  unLen;
	uint8_t i,ucComMF522Buf[MAXRLEN]; 

	ucComMF522Buf[0] = dd_mode;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

	if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
	{   status = MI_ERR;   }

	if (status == MI_OK)
	{
		for (i=0; i<16; i++)
		{    ucComMF522Buf[i] = *(pValue+i);   }
		CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
		unLen = 0;
		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
		{    status = MI_OK;    }
	}

	if (status == MI_OK)
	{
		ucComMF522Buf[0] = PICC_TRANSFER;
		ucComMF522Buf[1] = addr;
		CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 

		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

		if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
		{   status = MI_ERR;   }
	}
	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(uint8_t sourceaddr, uint8_t goaladdr)
{
	char status;
	uint32_t  unLen;
	uint8_t ucComMF522Buf[MAXRLEN]; 

	ucComMF522Buf[0] = PICC_RESTORE;
	ucComMF522Buf[1] = sourceaddr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

	if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
	{   status = MI_ERR;   }

	if (status == MI_OK)
	{
		ucComMF522Buf[0] = 0;
		ucComMF522Buf[1] = 0;
		ucComMF522Buf[2] = 0;
		ucComMF522Buf[3] = 0;
		CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);

		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
		{    status = MI_OK;    }
	}

	if (status != MI_OK)
	{    return MI_ERR;   }

	ucComMF522Buf[0] = PICC_TRANSFER;
	ucComMF522Buf[1] = goaladdr;

	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

	if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
	{   status = MI_ERR;   }

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
	//char status;
	uint32_t  unLen;
	uint8_t ucComMF522Buf[MAXRLEN]; 

	ucComMF522Buf[0] = PICC_HALT;
	ucComMF522Buf[1] = 0;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	//status = 
	PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

	return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(uint8_t *pIndata,uint8_t len,uint8_t *pOutData)
{
	uint8_t i,n;
	ClearBitMask(DivIrqReg,0x04);
	WriteRawRC(CommandReg,PCD_IDLE);
	SetBitMask(FIFOLevelReg,0x80);
	for (i=0; i<len; i++)
	{   WriteRawRC(FIFODataReg, *(pIndata+i));   }
	WriteRawRC(CommandReg, PCD_CALCCRC);
	i = 0xFF;
	do 
	{
		n = ReadRawRC(DivIrqReg);
		i--;
	}
	while ((i!=0) && !(n&0x04));
	pOutData[0] = ReadRawRC(CRCResultRegL);
	pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
	RC522_RESET_SET();		//RST522_1;
	delay_us(10);			//_NOP();
	RC522_RESET_RESET();	//RST522_0;
	delay_ms(60);			//_NOP();
	RC522_RESET_SET();		//RST522_1;
	delay_us(500);			//_NOP();
	WriteRawRC(CommandReg,PCD_RESETPHASE);
	delay_ms(2);			//_NOP();

	WriteRawRC(ModeReg,0x3D);			//���巢�ͺͽ��ճ���ģʽ ��Mifare��ͨѶ��CRC��ʼֵ0x6363
	WriteRawRC(TReloadRegL,30);			//16λ��ʱ����λ        
	WriteRawRC(TReloadRegH,0);			//16λ��ʱ����λ
	WriteRawRC(TModeReg,0x8D);			//�����ڲ���ʱ��������
	WriteRawRC(TPrescalerReg,0x3E);		//���ö�ʱ����Ƶϵ��
	WriteRawRC(TxAutoReg,0x40);			//���Ʒ����ź�Ϊ100%ASK

	ClearBitMask(TestPinEnReg, 0x80);	//off MX and DTRQ out
	WriteRawRC(TxAutoReg,0x40);

	return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
uint8_t ReadRawRC(uint8_t Address)
{
	uint8_t ucAddr;
	uint8_t ucResult=0;
	ucAddr = ((Address<<1)&0x7E)|0x80;
	delay_ms(1);
	RC522_ENABLE;
	SPI_WriteNBytes(SPI1,&ucAddr,1);
	SPI_ReadNBytes(SPI1,&ucResult,1);
	RC522_DISABLE;
	return ucResult;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(uint8_t Address, uint8_t value)
{  
	uint8_t ucAddr;
	uint8_t write_buffer[2]={0};
	ucAddr = ((Address<<1)&0x7E);
	write_buffer[0] = ucAddr;
	write_buffer[1] = value;
	delay_ms(1);
	RC522_ENABLE;
	SPI_WriteNBytes(SPI1,write_buffer,2);
	RC522_DISABLE;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(uint8_t reg,uint8_t mask)  
{
	uint8_t tmp = 0x0;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(uint8_t reg,uint8_t mask)  
{
	uint8_t tmp = 0x0;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pIn [IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOut [OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
char PcdComMF522(uint8_t Command, 
		uint8_t *pInData, 
		uint8_t InLenByte,
		uint8_t *pOutData, 
		uint32_t  *pOutLenBit)
{
	char status = MI_ERR;
	uint8_t irqEn   = 0x00;
	uint8_t waitFor = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint32_t i;
	switch (Command)
	{
		case PCD_AUTHENT:		//Mifare��֤
			irqEn   = 0x12;		//��������ж�����ErrIEn  ��������ж�IdleIEn
			waitFor = 0x10;		//��֤Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ
			break;
		case PCD_TRANSCEIVE:	//���շ��� ���ͽ���
			irqEn   = 0x77;		//����TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
			waitFor = 0x30;		//Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ�� �����жϱ�־λ
			break;
		default:
			break;
	}

	WriteRawRC(ComIEnReg,irqEn|0x80);	//IRqInv��λ�ܽ�IRQ��Status1Reg��IRqλ��ֵ�෴
	ClearBitMask(ComIrqReg,0x80);		//Set1��λ����ʱ��CommIRqReg������λ����
	WriteRawRC(CommandReg,PCD_IDLE);	//д��������
	SetBitMask(FIFOLevelReg,0x80);		//��λFlushBuffer����ڲ�FIFO�Ķ���дָ���Լ�ErrReg��BufferOvfl��־λ�����

	for (i=0; i<InLenByte; i++)
	{   WriteRawRC(FIFODataReg, pInData[i]);    }	//д���ݽ�FIFOdata
	WriteRawRC(CommandReg, Command);				//д����


	if (Command == PCD_TRANSCEIVE)
	{    SetBitMask(BitFramingReg,0x80);  }			//StartSend��λ�������ݷ��� ��λ���շ�����ʹ��ʱ����Ч

	i = 800; //����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
	do	//��֤ ��Ѱ���ȴ�ʱ��	
	{
		n = ReadRawRC(ComIrqReg);					//��ѯ�¼��ж�
		i--;
	}
	while ((i!=0) && !(n&0x01) && !(n&waitFor));	//�˳�����i=0,��ʱ���жϣ���д��������
	ClearBitMask(BitFramingReg,0x80);				//��������StartSendλ

	if (i!=0)
	{    
		if(!(ReadRawRC(ErrorReg)&0x1B))		//�������־�Ĵ���BufferOfI CollErr ParityErr ProtocolErr
		{
			status = MI_OK;
			if (n & irqEn & 0x01)			//�Ƿ�����ʱ���ж�
			{   status = MI_NOTAGERR;   }
			if (Command == PCD_TRANSCEIVE)
			{
				n = ReadRawRC(FIFOLevelReg);				//��FIFO�б�����ֽ���
				lastBits = ReadRawRC(ControlReg) & 0x07;	//�����յ����ֽڵ���Чλ��
				if (lastBits)
				{   *pOutLenBit = (n-1)*8 + lastBits;   }	//N���ֽ�����ȥ1�����һ���ֽڣ�+���һλ��λ�� ��ȡ����������λ��
				else
				{   *pOutLenBit = n*8;   }					//�����յ����ֽ������ֽ���Ч
				if (n == 0)
				{   n = 1;    }
				if (n > MAXRLEN)
				{   n = MAXRLEN;   }
				for (i=0; i<n; i++)
				{   pOutData[i] = ReadRawRC(FIFODataReg);    }
			}
		}
		else
		{   status = MI_ERR;   }

	}


	SetBitMask(ControlReg,0x80);           // stop timer now
	WriteRawRC(CommandReg,PCD_IDLE); 
	return status;
}


/////////////////////////////////////////////////////////////////////
//��������  
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
	uint8_t i;
	i = ReadRawRC(TxControlReg);
	if (!(i & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}


/////////////////////////////////////////////////////////////////////
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

/*
 * ��������RC522_Config
 * ����  ������RC522�Ĺ�����ʽ
 * ����  ��Card_Type��������ʽ
 * ����  : ��
 * ����  ���ⲿ����
 */
void RC522_Config(uint8_t Card_Type)
{
	if (Card_Type == 'A')
	{
		ClearBitMask(Status2Reg,0x08);
		WriteRawRC(ModeReg,0x3D);//3F
		WriteRawRC(RxSelReg,0x86);//84
		WriteRawRC(RFCfgReg,0x7F);//4F
		WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		WriteRawRC(TReloadRegH,0);
		WriteRawRC(TModeReg,0x8D);
		WriteRawRC(TPrescalerReg,0x3E);
		//WriteRawRC(TxAutoReg,0x40);
		delay_ms(5);
		PcdAntennaOn();

	}	
}













