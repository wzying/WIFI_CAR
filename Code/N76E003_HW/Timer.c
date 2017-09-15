
#include "N76E003.h"
#include "Common.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Timer.h"

FN_INT_HANDLER mTimer3IntHandler=0;

typedef union _TimerVal
{
	UINT8 mTimer0Cnt[2];
	UINT16  mVal;
}TimerVal;	

TimerVal mTimer0Cnt;
 
void Timer0Init(void)
{
	TIMER0_MODE1_ENABLE;
 
	clr_T0M; //fclk=fsys/12 
  TH0=0;TL0=0;
	set_TR0;                                    //Timer0 run
	 
}

UINT16 GetTimer0Cnt(void)
{

	clr_TR0;
	mTimer0Cnt.mTimer0Cnt[0]=TH0;
	mTimer0Cnt.mTimer0Cnt[1]=TL0;
	set_TR0;
	return mTimer0Cnt.mVal;
}

void Timer3Init(FN_INT_HANDLER handler)
{
	mTimer3IntHandler=handler;
	T3CON = 0x02;                           		//Timer3 Clock = Fsys/4
	RH3 = HIBYTE(TIMER_DIV4_VALUE_500us);                       //initial counter values 
	RL3 = LOBYTE(TIMER_DIV4_VALUE_500us);    
	
	set_ET3;                                    //enable Timer3 interrupt
 
}

void StartTimer3(void)
{
	set_TR3;//Timer3 run
}

UINT16 GetTimer3Count(void)
{
	UINT16 ret;
	
	ret=(RH3<<8) | RL3;
	
	return ret;
}

/************************************************************************************************************
*    TIMER 3 interrupt subroutine
************************************************************************************************************/
void Timer3_ISR (void) interrupt 16 
{
   clr_TF3;
	
	if(mTimer3IntHandler)
	{
		mTimer3IntHandler();
	}
}
