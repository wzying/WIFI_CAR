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
			
			if(Check_Recieve_Valid())//���յ���Ч�����ݰ�
			{
				Rx_Package_Handle(RX_Packet);
			}
			
			if(CheckTimeOut_Wifi())//WIFI�źų�ʱ
			{
				SetMotor_Brake();//ɲ��
				SetArm( ARM_STOP );
				SetClaw( CLAW_STOP );
			}	
			else 
			{
				//ǰ���źŽ��ճ�ʱ��ɲ��
				if(CheckTimeOut_Motor())
				{
					SetMotor_Brake();//ɲ��
				}
			}
			
			if(CheckTimeOut_Claw())//���ǰצͣת
			{
				if(Check_Claw_EN())
				{
					Reset_Claw_EN();	
					Claw_Trun();//ǰצ��ת
				}				
			}
				
			Indicator_Light();//ָʾ����ʾ
		}
	}
}

/*
Ӧ�ò��һЩ��ʼ��
*/
static void AppInit(void)
{
	
}

/*
Ӧ�ò�����
*/
static void AppTask(void)
{
	 
}

/*
	�������ݽ���
	RX_Handle
*/
void Rx_Package_Handle(uint8_t *uartBuf)
{
	Set_TimeOut_Wifi();//WIFI��ʱ��λ
	SetWifiStatus();//����WIFIΪ����״̬
	
	if( (uartBuf[2]!=0x80) || (uartBuf[1]!=0x80) )
	{
		Control_Motor(uartBuf[2],uartBuf[1]);
		
		Set_TimeOut_Motor();//��λ��ʱ����
	}
	
	if(uartBuf[3] != 0x00)//0x00�����ź�
	{
		Control_ARM (uartBuf[3]);
		Control_Claw(uartBuf[4]);
	}
}
