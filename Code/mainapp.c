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
#include "voltage.h"
#include "SFR_Macro.h"

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

void delay(void)   //
{
    unsigned char a,b;
    for(b=20;b>0;b--)
        for(a=1;a>0;a--);
} 

/*******************************************************************/
//UINT16 mtcnt,mprevtcnt;
void main (void) 
{
	u8 il=0;
//	UINT16 minus;
	u8 motorOutFlag = 0;
	u16 i=0;
//	u16 adc_val;

	//AppInit();
	BoardInit();
	
	// Test code
//	for(il=0;il<12;il++)
//	{
//		WDT_Reset();
//		delayms(10);
//		Turn_LED();
//	}
	
	//delay();
	
	//MyPrintf("Hello Wifi \r\n");
  
	//Set_Bat_TH();//���õ�ص��������ֵ
	while(1)
	{
		if(GetSysClock())//500uS
		{
			ResetSysClock();
			WDT_Reset();
			
			Voltage_Monitor();
			//Vol_Monitor_Idle();
			
			if(Check_Recieve_Valid())//���յ���Ч�����ݰ�
			{
				Rx_Package_Handle(RX_Packet);
			}
			
			if(CheckTimeOut_Wifi())//WIFI�źų�ʱ
			{
				SetMotor_Brake();//ɲ��
				Set_Motor_Status(MOTOR_MAIN,0);//��λ���״̬
				SetArm( ARM_STOP );
				Set_Motor_Status(MOTOR_ARM,0);//��λ���״̬
				SetClaw( CLAW_STOP );
			}	
			else 
			{
				//ǰ���źŽ��ճ�ʱ��ɲ��
				if(CheckTimeOut_Motor())
				{
					SetMotor_Brake();//ɲ��
					Set_Motor_Status(MOTOR_MAIN,0);//��λ���״̬
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
	
	//if(Get_Vol_Status() == NORMAL)
	{
		if( (uartBuf[2]!=0x80) || (uartBuf[1]!=0x80) )
		{
			Control_Motor(uartBuf[2],uartBuf[1]);
			
			Set_Motor_Status(MOTOR_MAIN,1);//���õ��״̬
			
			Set_TimeOut_Motor();//��λ��ʱ����
		}
		
		if(uartBuf[3] != 0x00)//0x00�����ź�
		{
			Control_ARM (uartBuf[3]);
			Control_Claw(uartBuf[4]);
		}
	}
}
