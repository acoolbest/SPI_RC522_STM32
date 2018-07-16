#include "delay.h"
#include "stm32f10x_conf.h"
#include "buzzer.h"


//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数

void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟使能
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5                                                                       
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIMx在CCR2上的预装载寄存器
	TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIMx在ARR上的预装载寄存器
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}

#if 1

// 定义乐曲：《开锁铃声》
const tNote song_unlock[]=
{
  {M6,TT/12},{M6,TT/12},{M6,TT/12},
  {0,0},
};
#endif

#if 1

// 定义乐曲：《关锁铃声》
const tNote song_lock[]=
{
  {M5,TT/12},{M1,TT/12},{M5,TT/12},
  {0,0},
};
#endif

// 蜂鸣器停止发声
void buzzerQuiet(void)	
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_Cmd(TIM3, DISABLE);  //停止TIM3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 //根据设定参数初始化GPIOB.5
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);	//PB.5 输出低
}

/////////////////////////////////////////////////////////	 
//蜂鸣器发出指定频率的声音
//usFreq是发声频率，取值 (系统时钟/65536)+1 ～ 20000，单位：Hz
void buzzerSound(unsigned short usFreq)	 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	unsigned long  ulVal;
	if((usFreq<=8000000/65536UL)||(usFreq>20000))
	{
		buzzerQuiet();// 蜂鸣器静音
	}
	else
	{
		GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
		//设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //TIM_CH2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO	  
		ulVal=8000000/usFreq;
		//TIM3->ARR =ulVal ;	//设置自动重装载寄存器周期的值（音调） 
		TIM_SetAutoreload(TIM3,ulVal);//设置自动重装载寄存器周期的值（音调） 
		TIM_SetCompare2(TIM3,ulVal/2);//音量
		TIM_Cmd(TIM3, ENABLE);  //启动TIM3
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
		buzzerQuiet(); // 蜂鸣器静音
		delay_ms(10);// 10 ms 
	} 
}

void buzzer_init(void)
{
	TIM3_PWM_Init(14399,10);	 //分频。PWM频率=72000/14400/11（Khz）
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
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	TIM3_PWM_Init(14399,10);	 //分频。PWM频率=72000/14400/11（Khz）
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

