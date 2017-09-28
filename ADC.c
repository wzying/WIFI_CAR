
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "Audio.h"
#include "Board.h"
#include "PWM.h"
#include "motor.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "string.h"
#include "UART.h"
#include "LED.h"


void Init_ADC(void)
{
//	//P0.4 AIN5  检测电机电流
//	P04_Input_Mode;
//	Enable_ADC_AIN5;
//	
//	//P0.5 AIN4  检测电池电压
//	P05_Input_Mode;
//	Enable_ADC_AIN4;	
}

u16 Get_ADC(void)
{
	u16 adc;

	//P0.4 AIN5  检测电机电流
	Enable_ADC_AIN5;	
	
	clr_ADCF;
	set_ADCS;									// ADC start trig signal

	while(ADCF == 0);

	adc = ADCRH*16 + ADCRL;
	
	return adc;
}

//获取电池电压
u16 Get_ADC_BAT(void)
{
	u16 adc;	
	
	//P0.5 AIN4  检测电池电压
	Enable_ADC_AIN4;		

	clr_ADCF;
	set_ADCS;									// ADC start trig signal

	while(ADCF == 0);

	adc = ADCRH*16 + ADCRL;
	
	return adc;	
}

