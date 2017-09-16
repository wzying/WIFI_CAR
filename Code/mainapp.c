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
			
			Indicator_Light();//指示灯显示
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
//      SetMotor_Brake();//刹车
//			//SetMotor_STOP();//不刹车
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
