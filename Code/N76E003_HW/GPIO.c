/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2017 Buta Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Website: http://www.ptahtoy.com
//  E-Mail : linshigao@ptahtoy.com
//  Date   : Jun/24/2017
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: BabyMonkey(Finger Toy) 
//***********************************************************************************************************
#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "GPIO.h"

FN_INT_HANDLER mGPIOIntHandler4=0; // Px4 channel interrupt
FN_INT_HANDLER mGPIOIntHandler5=0; // Px5 channel interrupt
FN_INT_HANDLER mGPIOIntHandler6=0; // Px6 channel interrupt
FN_INT_HANDLER mEX0IntHandler=0;
FN_INT_HANDLER mEX1IntHandler=0;
/*
GPIO initialization
*/
void GPIO_Init(void)
{
	/*
		机械臂
		P1.4 + P1.3
	*/
	P14_PushPull_Mode;
	P14=0;
	P13_PushPull_Mode;
	P13=0;	
	
	/*
		前爪
		P1.0 + P0.3
	*/
	P10_PushPull_Mode;
	P10=0;	
	P03_PushPull_Mode;
	P03=0;	
	
	/*
		电池组检测
	  P1.5
	*/
	P15_Quasi_Mode;
}

/*
Pin Interrupt handler 
*/
void PinInterrupt_ISR (void) interrupt 7
{
	UINT8 flg;
	
	flg=PIF;
	if((flg &0x10) == 0x10)
	{//P04 interrupt
		if(mGPIOIntHandler4)
		{
			mGPIOIntHandler4();
		}
	}
	else if((flg &0x20) == 0x20)
	{//P05 interrupt
		if(mGPIOIntHandler5)
		{
			mGPIOIntHandler5();
		}
	}
	else if((flg &0x40) == 0x40)
	{//P06 interrupt
		if(mGPIOIntHandler6)
		{
			mGPIOIntHandler6();
		}
	}
	
	PIF = 0;                             //clear interrupt flag
	 
}

