#include "Common.h"
//#include "stdlib.h"
#include "Timer.h"
#include "GPIO.h"
#include "UART.h"
#include "PWM.h"
#include "Audio.h"
#include "Board.h"
#include "LED.h"
#include "ADC.h"
/*
**********************************************************************
* 硬件抽象层
* 
* 
* 
*
***********************************************************************
*/

/***********************Local function Define***********************/
 
static void Timer3_IntHandler(void);
 
/***********************Global Variables Define***********************/
EUINT16 mTimerCnt[MAX_TIMER_NUM];
 
EUINT8 mTock;
EUINT8 mTIFlg;
EUINT8 mlstock;
EUINT8 mTick_1ms;
EUINT16 mTick_1s;
 
 
/*
板级的初始化
*/
void BoardInit(void)
{
	GPIO_Init();
	LED_Init();
	//Init_ADC();
	
	Timer3Init(Timer3_IntHandler);//1ms interrupt
	
	InitialUART0_Timer1(4800);
	PWM_Init();
	
	GlobalInterruptEnable();//使能中断全局GIE。
	StartTimer3();
}
 
///*********************************************************************
//Brief: MT_SetFoot_Left ,control the left foot motor 
//Return: void
//Called: 
// 
//**********************************************************************/
//void MT_SetFoot_Left(UINT8 dir,UINT8 spd)
//{

//	if(spd==0)
//	{//stop
//		
//	}
//}
 

 
/*********************************************************************
Brief: SysShutdown ,进入低功耗状态
Return: void
Called: mainapp.c
 
**********************************************************************/
void SysShutdown(void)
{

}

/*********************************************************************
Brief: printstr ,测试打印
Return: void
Called: mainapp.c
 
**********************************************************************/
void printstr(UINT8* pdin,UINT8 dlen)
{
 	UART0SendData(pdin,dlen);
}

 
/*********************************************************************
Brief: HW_Board_Poll ,硬件轮询函数入口
Return: void
Called: mainapp.c
 
**********************************************************************/
void HW_Board_Poll(void)
{
 
//////////////////////////Timer/////////////////////////////////		
	if(mTIFlg)
	{
		mTIFlg=0;
	}
	
	//1ms flag
	if(mTock==1 && mlstock==0)
	{
		mTick_1ms=1;
		mTick_1s++;
 
	}
	
	mlstock=mTock;
}

 
/*********************************************************************
Brief: GetIRTick ,IR 计数器 , 750us记一次
Return: void
Called: mainapp.c
 
**********************************************************************/
UINT16 GetIRTick(void)
{
	return GetTimer0Cnt();
}

 

/*********************************************************************
Brief: GetTick_1s ,每秒输出1个脉冲
Return: void
Called: unkown
 
**********************************************************************/
UINT8 GetTick_1s(void)
{
	if(mTick_1s>=1000) 
	{
		mTick_1s=0;
		return 1;
	}
	else
	{
		return 0;
	}
}

 
/*********************************************************************
Brief: GetTick_1ms ,每毫秒输出1个脉冲
Return: void
Called: unkown
 
**********************************************************************/
UINT8 GetTick_1ms(void)
{
	UINT8 ret;
	
	ret=mTick_1ms;
	mTick_1ms=0;
	return ret;
}

 
/*********************************************************************
Brief: GetTickCount ,得到计数器的tick 用于产生伪随机数等场景
Return: void
Called: unkown
 
**********************************************************************/
UINT16 GetTickCount(void)
{
	return GetTimer3Count();
}

 
/*********************************************************************
Brief: SetTimer ,设置定时器  设定对应 id 定时器的周期
Return: void
Called: unkown
 
**********************************************************************/
UINT8 SetTimer(UINT8 id,UINT16 ms)
{
	if(id>=MAX_TIMER_NUM) return 0xFF;
	
	mTimerCnt[id]=ms;
	
	return id;
}
 
/*********************************************************************
Brief: GetTimer 查询定时器的计数状态
return: 0 ,停止; 否则当前的计数值
Called: unkown
 
**********************************************************************/
UINT16 GetTimer(UINT8 id)
{
	return mTimerCnt[id];
}
 
 
/*********************************************************************
Brief: GetIOLevel IR 4个接收引脚电平
return: 0 ,停止; 否则当前的计数值
Called: IRComm.c
 
**********************************************************************/
UINT8 GetIOLevel(UINT8 i)
{
	UINT8 ret;
	
	switch(i)
	{
		case 0://front
			ret=GetPort1Val(7);
		break;
		case 1://back
			ret=GetPort3Val(0);
		break;
		case 2://left
			ret=GetPort0Val(5);
		break;
		case 3://right
			ret=GetPort0Val(7);
		break;
	}
	
	return ret;
}
 
/*********************************************************************
Brief: SetAttackPin IR   IR 攻击输出 ,有问题,需要改
return: 
Called: IRComm.c
 
**********************************************************************/
void  SetAttackPin(UINT8 val)
{
	SetPort2Val(0,val); //
} 
////////////////////////////////////Interrupt handler////////////////////////////////////

void RegGPIOIntHandler(UINT8 flg,FN_INT_HANDLER fn)
{
	switch(flg)
	{
		case EXTI_INT0:
			SetEX0IntHandler(fn);
		break;
		case EXTI_INT1:
			SetEX1IntHandler(fn);
		break;
		case EXTI_Px5:
			SetGPx5IntHandler(fn);
		break;
		case EXTI_Px6:
			SetGPx6IntHandler(fn);
		break;
	
	}

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
timer3 interrupt handler   1mS interrpt
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
		
		//超时复位接收Buf
		if(CheckTimeOut())
		{
			ResetIndexRx();
			//MyPrintf("Reset Index\r\n");
		}		
	}
	

	//修改：让电流阈值随电池电压的降低而降低
	if(Get_ADC() > 200)
	{
		SetPWM_Stop();
		//SetPWM_Start();
	}
	else
	{
		SetPWM_Start();
	}
	
}


 
