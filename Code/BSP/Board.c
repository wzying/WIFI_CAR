#include "Common.h"
#include "Timer.h"
#include "GPIO.h"
#include "UART.h"
#include "PWM.h"
#include "Audio.h"
#include "Board.h"
#include "LED.h"
#include "ADC.h"
#include "voltage.h"
#include "protocol.h"


/***********************Local function Define***********************/
 
static void Timer3_IntHandler(void);

/*
板级的初始化
*/
void BoardInit(void)
{
	WDT_Init();//看门狗
	
	GPIO_Init();
	LED_Init();
	//Init_ADC();

	Timer3Init(Timer3_IntHandler);//1ms interrupt

	InitialUART0_Timer1(19200);//初始化串口，波特率4800
	PWM_Init();
	
	GlobalInterruptEnable();//使能中断全局GIE。
	StartTimer3();
}

u8 sysClockFlag = 0;
void SetSysClock(void)
{
	sysClockFlag = 1;
}
void ResetSysClock(void)
{
	sysClockFlag = 0;
}
u8 GetSysClock(void)
{
	return sysClockFlag;
}

/*
	timer3 interrupt handler
	Timer3 中断回调函数
*/
static void Timer3_IntHandler(void)
{
	static u8 i=0;
	
	if(i<10)
	{
		i++;
	}
	else
	{
		i = 0;
		
		SetSysClock();
		
		//超时复位接收Buf ，即一个包接收结束，等待下一个包
		if(CheckTimeOut())
		{
			ResetIndexRx();
		}		
	}
	
	//马达电流 限流
  //过流、堵转 保护	
	I_Limit();
}

