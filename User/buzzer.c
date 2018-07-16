#include "delay.h"
#include "stm32f10x_conf.h"
#include "buzzer.h"


//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��

void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��ʹ��
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5                                                                       
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIMx��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM3, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
}

#if 1

// ����������������������
const tNote song_unlock[]=
{
  {M6,TT/12},{M6,TT/12},{M6,TT/12},
  {0,0},
};
#endif

#if 1

// ����������������������
const tNote song_lock[]=
{
  {M5,TT/12},{M1,TT/12},{M5,TT/12},
  {0,0},
};
#endif

// ������ֹͣ����
void buzzerQuiet(void)	
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_Cmd(TIM3, DISABLE);  //ֹͣTIM3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PB.5 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 //�����趨������ʼ��GPIOB.5
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);	//PB.5 �����
}

/////////////////////////////////////////////////////////	 
//����������ָ��Ƶ�ʵ�����
//usFreq�Ƿ���Ƶ�ʣ�ȡֵ (ϵͳʱ��/65536)+1 �� 20000����λ��Hz
void buzzerSound(unsigned short usFreq)	 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	unsigned long  ulVal;
	if((usFreq<=8000000/65536UL)||(usFreq>20000))
	{
		buzzerQuiet();// ����������
	}
	else
	{
		GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5    
		//���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //TIM_CH2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO	  
		ulVal=8000000/usFreq;
		//TIM3->ARR =ulVal ;	//�����Զ���װ�ؼĴ������ڵ�ֵ�������� 
		TIM_SetAutoreload(TIM3,ulVal);//�����Զ���װ�ؼĴ������ڵ�ֵ�������� 
		TIM_SetCompare2(TIM3,ulVal/2);//����
		TIM_Cmd(TIM3, ENABLE);  //����TIM3
	}  
}

void musicPlay(const tNote * MyScore)
{ 
	u8 i=0; 
	while(1) 
	{
		if (MyScore[i].mTime == 0) break; 
		buzzerSound(MyScore[i].mName); 
		delay_ms(MyScore[i].mTime);
		i++; 
		buzzerQuiet(); // ����������
		delay_ms(10);// 10 ms 
	} 
}

void buzzer_init(void)
{
	TIM3_PWM_Init(14399,10);	 //��Ƶ��PWMƵ��=72000/14400/11��Khz��
}

void buzzer_play(uint8_t type)
{
	switch(type)
	{
		case BUZZER_PLAY_UNLOCK:
			musicPlay(song_unlock);
			break;
		case BUZZER_PLAY_LOCK:
			musicPlay(song_lock);
			break;
		default:
			break;
	}
}

#if 0
int main(void) 
{ 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	TIM3_PWM_Init(14399,10);	 //��Ƶ��PWMƵ��=72000/14400/11��Khz��
	for (;;) 
	{
		musicPlay(MyScore3);
		delay_ms(3000);
		buzzer_play(BUZZER_PLAY_UNLOCK);
		delay_ms(1000);
		buzzer_play(BUZZER_PLAY_LOCK);
		delay_ms(3000);
	} 
}
#endif

