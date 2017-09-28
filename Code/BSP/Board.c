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
* Ӳ�������
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
�弶�ĳ�ʼ��
*/
void BoardInit(void)
{
	GPIO_Init();
	LED_Init();
	//Init_ADC();
	
	Timer3Init(Timer3_IntHandler);//1ms interrupt
	
	InitialUART0_Timer1(4800);
	PWM_Init();
	
	GlobalInterruptEnable();//ʹ���ж�ȫ��GIE��
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
Brief: SysShutdown ,����͹���״̬
Return: void
Called: mainapp.c
 
**********************************************************************/
void SysShutdown(void)
{

}

/*********************************************************************
Brief: printstr ,���Դ�ӡ
Return: void
Called: mainapp.c
 
**********************************************************************/
void printstr(UINT8* pdin,UINT8 dlen)
{
 	UART0SendData(pdin,dlen);
}

 
/*********************************************************************
Brief: HW_Board_Poll ,Ӳ����ѯ�������
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
Brief: GetIRTick ,IR ������ , 750us��һ��
Return: void
Called: mainapp.c
 
**********************************************************************/
UINT16 GetIRTick(void)
{
	return GetTimer0Cnt();
}

 

/*********************************************************************
Brief: GetTick_1s ,ÿ�����1������
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
Brief: GetTick_1ms ,ÿ�������1������
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
Brief: GetTickCount ,�õ���������tick ���ڲ���α������ȳ���
Return: void
Called: unkown
 
**********************************************************************/
UINT16 GetTickCount(void)
{
	return GetTimer3Count();
}

 
/*********************************************************************
Brief: SetTimer ,���ö�ʱ��  �趨��Ӧ id ��ʱ��������
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
Brief: GetTimer ��ѯ��ʱ���ļ���״̬
return: 0 ,ֹͣ; ����ǰ�ļ���ֵ
Called: unkown
 
**********************************************************************/
UINT16 GetTimer(UINT8 id)
{
	return mTimerCnt[id];
}
 
 
/*********************************************************************
Brief: GetIOLevel IR 4���������ŵ�ƽ
return: 0 ,ֹͣ; ����ǰ�ļ���ֵ
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
Brief: SetAttackPin IR   IR ������� ,������,��Ҫ��
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
		
		//��ʱ��λ����Buf
		if(CheckTimeOut())
		{
			ResetIndexRx();
			//MyPrintf("Reset Index\r\n");
		}		
	}
	

	//�޸ģ��õ�����ֵ���ص�ѹ�Ľ��Ͷ�����
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


 
