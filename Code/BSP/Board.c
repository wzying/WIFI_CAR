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
�弶�ĳ�ʼ��
*/
void BoardInit(void)
{
	WDT_Init();//���Ź�
	
	GPIO_Init();
	LED_Init();
	//Init_ADC();

	Timer3Init(Timer3_IntHandler);//1ms interrupt

	InitialUART0_Timer1(19200);//��ʼ�����ڣ�������4800
	PWM_Init();
	
	GlobalInterruptEnable();//ʹ���ж�ȫ��GIE��
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
	Timer3 �жϻص�����
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
		
		//��ʱ��λ����Buf ����һ�������ս������ȴ���һ����
		if(CheckTimeOut())
		{
			ResetIndexRx();
		}		
	}
	
	//������ ����
  //��������ת ����	
	I_Limit();
}

