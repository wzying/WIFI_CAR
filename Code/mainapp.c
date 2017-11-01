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

/***********************Local function Define***********************/
static void AppInit(void);
static void AppTask(void);
static void ClearSensorStatus(void);
static void WakeupAction(void);
/***********************Local Variables Define***********************/

EUINT8 midletick;//1s per tick
 
u8	uartData = 0;	//????
//u8	uartBuf[DATA_LEN];
u8	num = 0;

u8 dataPacket0;
u8 dataPacket1;
u8 speed = 0x00;
u16 timeOutSonix = 0;//?????????

/* ???? */
void delay(void)   //?? 0us
{
    unsigned char a,b;
    for(b=20;b>0;b--)
        for(a=1;a>0;a--);
} 

/*******************************************************************/
//UINT16 mtcnt,mprevtcnt;
void main (void) 
{
//	UINT16 minus;
	u8 motorOutFlag = 0;
	u16 i=0;
//	u16 adc_val;
	
	AppInit();
	BoardInit();
	
	MyPrintf("Hello WiFI Car \r\n");

	while(1)
	{
		if(GetSysClock())//500uS
		{
			ResetSysClock();
			
//			if(i>=1000)
//			{
//				i=0;
//				adc_val = Get_ADC_BAT();
////				if(adc_val > 200)
////				{
////					SetPWM_Stop();
////					continue;
////				}
////				else
////				{
////					SetPWM_Start();
////				}
//				//MyPrintf("ADC = ");
//				Send_Data_To_UART0(adc_val/1000 + '0');
//				Send_Data_To_UART0(adc_val%1000/100 + '0');
//				Send_Data_To_UART0(adc_val%100/10 + '0');
//				Send_Data_To_UART0(adc_val%10 + '0');
//				MyPrintf("\r\n");
//			}
//			else
//			{
//				i++;
//			}
			
			if(Check_Recieve_Valid())//接收到有效的数据包
			{
				Rx_Package_Handle(RX_Packet);
			}
			
			if(CheckTimeOut_Wifi())//WIFI信号超时
			{
				SetMotor_Brake();//刹车
				SetArm( ARM_STOP );
				SetClaw( CLAW_STOP );
			}	
			else 
			{
				//前后信号接收超时，刹车
				if(CheckTimeOut_Motor())
				{
					SetMotor_Brake();//刹车
				}
			}
			
			if(CheckTimeOut_Claw())//检测前爪停转
			{
				if(Check_Claw_EN())
				{
					Reset_Claw_EN();	
					Claw_Trun();//前爪反转
				}				
			}
				
			Indicator_Light();//指示灯显示
		}
	}
}

/*
应用层的一些初始化
*/
static void AppInit(void)
{
	
}

/*
应用层任务
*/
static void AppTask(void)
{
	 
}

/*
	接收数据解析
	RX_Handle
*/
void Rx_Package_Handle(uint8_t *uartBuf)
{
	Set_TimeOut_Wifi();//WIFI超时复位
	SetWifiStatus();//设置WIFI为连接状态
	
	if( (uartBuf[2]!=0x80) || (uartBuf[1]!=0x80) )
	{
		Control_Motor(uartBuf[2],uartBuf[1]);
		
		Set_TimeOut_Motor();//复位超时计数
	}
	
	if(uartBuf[3] != 0x00)//0x00空闲信号
	{
		Control_ARM (uartBuf[3]);
		Control_Claw(uartBuf[4]);
	}
}
