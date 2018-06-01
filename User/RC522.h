#include "stm32f10x.h"


#define RC522_RESET_PIN                 GPIO_Pin_0                /* PB0 */    //   RESET
#define RC522_RESET_GPIO_PORT           GPIOB                     /* GPIOB */
#define RC522_RESET_GPIO_CLK            RCC_APB2Periph_GPIOB
#define RC522_RESET_SET()               GPIO_SetBits(RC522_RESET_GPIO_PORT, RC522_RESET_PIN);      //1
#define RC522_RESET_RESET()             GPIO_ResetBits(RC522_RESET_GPIO_PORT, RC522_RESET_PIN);    //0

#define RC522_ENABLE                    GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define RC522_DISABLE                   GPIO_SetBits(GPIOA,GPIO_Pin_4)






/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

/////////////////////////////////////////////////////////////////////
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F                 0x3F

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
/////////////////////////////////////////////////////////////////////
#define MI_OK                     0
#define MI_NOTAGERR               1 //(-1)
#define MI_ERR                    2 //(-2)

#define MAXRLEN                   18

void RC522_IO_Init(void);



char PcdRequest(uint8_t req_code,uint8_t *pTagType);
char PcdAnticoll(uint8_t *pSnr);
char PcdSelect(uint8_t *pSnr);
char PcdAuthState(uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr);
char PcdRead(uint8_t addr,uint8_t *pData);
char PcdWrite(uint8_t addr,uint8_t *pData);
char PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *pValue);
char PcdBakValue(uint8_t sourceaddr, uint8_t goaladdr);
char PcdHalt(void);
void CalulateCRC(uint8_t *pIndata,uint8_t len,uint8_t *pOutData);
char PcdReset(void);
uint8_t ReadRawRC(uint8_t Address);
void WriteRawRC(uint8_t Address, uint8_t value);
void SetBitMask(uint8_t reg,uint8_t mask);
void ClearBitMask(uint8_t reg,uint8_t mask);
char PcdComMF522(uint8_t Command, 
					uint8_t *pInData, 
					uint8_t InLenByte,
					uint8_t *pOutData, 
					uint32_t  *pOutLenBit);
void PcdAntennaOn(void);
void PcdAntennaOff(void);
void RC522_Config(uint8_t Card_Type);

//д0��
/*
 * �K�׌���ҵ�һƪ�������������@�ӣ�
 * Sent bits: 26 (7 bits) //���� 0x26 / 0x52 ������
 * Received bits: 04 00
 * Sent bits: 93 20 //���nײ
 * Received bits: 01 23 45 67 00
 * Sent bits: 93 70 01 23 45 67 00 d0 6f //�x��
 * Received bits: 08 b6 dd ��SAK��
 * �������J�C��耣���Ȼ���T���_���ã����c���ˣ�
 * Sent bits: 50 00 57 cd //���ߣ�50 00 ���� PcdHalt()
 * Sent bits: 40 (7 bits) ������ָ�//�_���T��һ�lָ�Ҫ�O�� BitFramingReg ʹ ���� 7 ��λԪ�����Ҫ 7 ��
 * Received bits: a (4 bits)
 * Sent bits: 43 ������ָ�//�_���T�ڶ��lָ��
 * Received bits: 0a
 * Sent bits: a0 00 5f b1 //�����Č��^�K��һ�ν���
 * Received bits: 0a
 * Sent bits: 00 dc 44 20 b8 08 04 00 46 59 25 58 49 10 23 02 c0 10 //�����Č� block 0 �Y��
 * Received bits: 0a
 * ���c����Ҫ��
 * 1. �������M�� 3 Pass Authenticaiton 
 * 2. �l PcdHalt() 
 * 3. �l 0x40 in 7-bit 
 * 4. �l 0x43
 */
 
/*
 * ����˵��
 * ���Ȱ�������S50���Ĳ���˳����� Ѱ��->����ײ->ѡ�� ��
 * ��������Ҫ��֤���룬Ȼ�� ���ߡ�
 *
 * ���淢�͵�һ������ָ�����16���Ƶ�0x40������ֻ����7λ,
 * �ܶ����Ѿ��ǲ����������η���0x40�Լ����ֻ����7λ��ֻ��Ҫ�����������������Ϳ����ˡ�
 * Write_MFRC522()��WriteRawRC()
 * MFRC522_ToCard()��PcdComMF522()
 * ��һ�������÷���7��λ����Ҫ����BitFramingRegΪ0x07��
 * �ڶ�����дָ�������Ҫ˵�㲻�������������������ľ�Ҫ�úÿ��³������������õ����������ġ�
 *
 * Ȼ���͵ڶ���ָ��0x43��Ҫ��8λ������BitFramingRegΪ0x00�������������Ҳ���ԣ�Ĭ�ϵ�8λ����
 *
 * ����������buf[ ]={0xa0,0x00, 0x5f, 0xb1}��
 * ͬ����MFRC522_ToCard()����
 *���⴮ָ����Ҳû��ʲô��˼���͵���������ָ��ɣ�����������������Ը���˵�£���
 * ���﷢�͵�ָ���Ӧ��0x0a���ҵĺ�����0xa0�������Ǵ�С������ɣ���
 * ���û�յ�˵��û��Ū�ԣ�����һ��һ�����ԡ�
 * ͨ����������ָ������д��0�ˣ�
 * ��ֵ��ǰ�������˵������д��0������û�гɹ������������ǿ��Եģ��ҿ�ԭ����һ���ģ���
 * �һ��ǵ������õ�MFRC522_ToCard()����д��0��������18���ֽڣ����������Ŀ�0��16�ֽڣ�
 *
 * ���������и��ص㣬�Ȱ�Ҫ���ƵĿ��Ŀ�0�ֽڶ�����
 * ��CalulateCRC()���������2λУ��λ����16�ֽں������18�ֽ�һ��д�뵽��0��
 * �����������д��0�����ݣ���Ϊ��У�顣���ˣ���0�Ϳ��Գɹ�д��
 */

/*
 * https://blog.csdn.net/baidu_34570497/article/details/79689778
 * http://www.51hei.com/bbs/dpj-85337-1.html
 */









































