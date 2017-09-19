
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
	//P0.4 AIN5
	P04_Input_Mode;
	
	Enable_ADC_AIN5;
}

u16 Get_ADC(void)
{
	u16 adc;
	
	clr_ADCF;
	set_ADCS;									// ADC start trig signal
//	MyPrintf("W ");
	while(ADCF == 0);
//	MyPrintf("L\r\n");
//	printf ("\n Value = 0x%bx",ADCRH);
//	printf ("\n Value = 0x%bx",ADCRL);	
	
	adc = ADCRH*16 + ADCRL;
	
	return adc;
}

