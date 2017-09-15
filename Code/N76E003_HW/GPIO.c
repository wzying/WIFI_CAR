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
		»úÐµ±Û
		P0.5 + P1.5
	*/
	P05_PushPull_Mode;
	P05=0;
	P15_PushPull_Mode;
	P15=0;	
	
	/*
		Ç°×¦
		P1.0 + P0.3
	*/
	P10_PushPull_Mode;
	P10=0;	
	P03_PushPull_Mode;
	P03=0;	
}

 
UINT8 GetPort0Val(UINT8 pin)
{
	UINT8 ret;
	
	switch(pin)
	{
		case 0:
			ret=P00;
		break;
		case 1:
			ret=P01;
		break;
		case 2:
			ret=P02;
		break;
		case 3:
			ret=P03;
		break;
		case 4:
			ret=P04;
		break;
		case 5:
			ret=P05;
		break;
		case 6:
			ret=P06;
		break;
		case 7:
			ret=P07;
		break;
	}
	
	return ret;
	
}
UINT8 GetPort1Val(UINT8 pin)
{
	UINT8 ret;
	
	switch(pin)
	{
		case 0:
			ret=P10;
		break;
		case 1:
			ret=P11;
		break;
		case 2:
			ret=P12;
		break;
		case 3:
			ret=P13;
		break;
		case 4:
			ret=P14;
		break;
		case 5:
			ret=P15;
		break;
		case 6:
			ret=P16;
		break;
		case 7:
			ret=P17;
		break;
	}
	
	return ret;
	
}
UINT8 GetPort3Val(UINT8 pin)
{
	UINT8 ret;
	
	ret=0;
	
	switch(pin)
	{
		case 0:
			ret=P30;
		break;
		 
	}
	
	return ret;
	
}


void  SetPort2Val(UINT8 pin,UINT8 val)
{
	switch(pin)
	{
		case 0:
			P20=val;
		break;
		 
	}
	

}



void SetGPx0IntHandler(FN_INT_HANDLER fn)
{
	mGPIOIntHandler4=fn;
}

void SetGPx5IntHandler(FN_INT_HANDLER fn)
{
	mGPIOIntHandler5=fn;
}

void SetGPx6IntHandler(FN_INT_HANDLER fn)
{
	mGPIOIntHandler6=fn;
}

void SetEX0IntHandler(FN_INT_HANDLER fn)
{
	mEX0IntHandler=fn;
}

void SetEX1IntHandler(FN_INT_HANDLER fn)
{
	mEX1IntHandler=fn;
}
/*
toggle the P07(IR output signal)
*/
//void ToggleIROut(void)
//{
//	P01=~P01;
//}

//void ToggleIROut_test(void)
//{
//	P00=~P00;
//}

//void SetIR_Pin(UINT8 sta)
//{
//	P01=sta;
//}

//void SetIR_Pin_test(UINT8 sta)
//{
//	P00=sta;
//}

//UINT8 GetTouch_Head(void)
//{
//	return P15;
//}

//UINT8 GetTouch_HeadRight(void)
//{
//	return P13;
//}

//UINT8 GetP05Val(void)
//{
//	return P05;
//}

//UINT8 GetP06Val(void)
//{
//	return P06;
//}

//UINT8 GetP02Val(void)
//{
//	return P02;
//}

//void SetP14(UINT8 sta)
//{
//	P14=sta;
//}
// 
//UINT8 GetP13Val(void)
//{
//	return P13;
//}

//UINT8 GetP15Val(void)
//{
//	return P15;
//}
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

/*
Extern 0 interrupt handler
*/
void EXT_INT0(void) interrupt 0
{
	if(mEX0IntHandler)
	{
		mEX0IntHandler();
	}
}
