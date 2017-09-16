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
	
	AppInit();
	BoardInit();
	
	MyPrintf("Hello WiFI Car \r\n");
	
	while(1)
	{
		if(GetSysClock())//500uS
		{
			ResetSysClock();
			
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
			
			Indicator_Light();//ָʾ����ʾ
		}
//			if(GetTick_1ms())
//			{//1ms event

//			}
//		 HW_Board_Poll();
//			AppTask();
		
//    delay();
//    
//    /* ???? */
//    if (timeOutSonix > 3000)
//    {
//      SetMotor_Brake();//ɲ��
//			//SetMotor_STOP();//��ɲ��
//      //SetArm( ARM_STOP );
//      //SetClaw( CLAW_STOP );
//      speed = 0;
//    }
//    else
//      timeOutSonix++;
//                
//    switch( dataPacket1 )
//    {
//      /* ????? */
//      case 0x70:
//        SetArm( ARM_DOWN );
//        break;
//      /* ????? */
//      case 0x71:
//				SetArm( ARM_UP );
//        break;
//      /* ???? */
//      case 0x05:
//        SetClaw( CLAW_RELEASE );
//        break;
//      /* ???? */
//      case 0x09:
//        SetClaw( CLAW_HOLD );
//        break;		
//      default :
//        SetArm( ARM_STOP );
//        SetClaw( CLAW_STOP );				
//        break;
//    } 

//    /* ???? */
//    motorOutFlag = 0;
//    switch( dataPacket0 )
//    {
//      /* ?? */
//      case 0x72:
//        SetMotor_L( 0 , 1 );
//        SetMotor_R( 180 , 0 );
//        break;
//      /* ?? */
//      case 0x73:
//        SetMotor_L( 180 , 0 );
//        SetMotor_R( 0 , 1 );
//        break;
//      /* ?? */
//      case 0x74:
//        SetMotor_L( speed , 0 );
//        SetMotor_R( speed , 0 );
//        break;
//      /* ?? */
//      case 0x75:
//        SetMotor_L( speed , 1 );
//        SetMotor_R( speed , 1 );
//        break;
//      /* ?? */
//      default:
//        motorOutFlag = 1;//?????
//        break;
//    }

//    dataPacket0 = 0x00;//???????,???????
//                          
//    //????,????????,????
//    if( motorOutFlag == 0 )
//    {
//      timeOutSonix = 0;
//    }		
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
