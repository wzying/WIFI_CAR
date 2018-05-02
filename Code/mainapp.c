/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2017 Buta Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Website: http://www.ptahtoy.com
//  E-Mail : wzy@ptahtoy.com
//  Date   : Jun/24/2017
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: 
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
#include "protocol.h"

/*******************************************************************/
void main (void) 
{
	u8 il=0;

	u8 motorOutFlag = 0;
	u16 i=0;

	BoardInit();
	
	while(1)
	{
		if(GetSysClock())//500uS进来一次
		{
			ResetSysClock();
			WDT_Reset();//喂狗
			
			Voltage_Monitor();
			
			if(Check_Recieve_Valid())//接收到有效的数据包
			{
				Rx_Package_Handle(RX_Packet);
			}
			
			if(CheckTimeOut_Wifi())//WIFI信号超时，长时间没有收到有效的wifi信号
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

