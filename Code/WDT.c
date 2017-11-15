
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
#include "ADC.h"
#include "voltage.h"
#include "WDT.h"

//bit BIT_TMP;

void WDT_Init(void)
{
//----------------------------------------------------------------------------------------------
// WDT Init !!! ENABLE CONFIG WDT FIRST !!!
// Warning:
// Always check CONFIG WDT enable first, CONFIG not enable, SFR can't enable WDT reset
// Please call Enable_WDT_Reset_Config() function to enable CONFIG WDT reset
//----------------------------------------------------------------------------------------------

//	Enable_WDT_Reset_Config();

	  TA=0xAA;TA=0x55;WDCON=0x07;						//Setting WDT prescale 
		set_WDCLR;														//Clear WDT timer
		while((WDCON|~SET_BIT6)==0xFF);				//confirm WDT clear is ok before into power down mode
		//EA = 1;
		set_WDTR;															//WDT run
			
}
