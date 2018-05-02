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
		if(GetSysClock())//500uS����һ��
		{
			ResetSysClock();
			WDT_Reset();//ι��
			
			Voltage_Monitor();
			
			if(Check_Recieve_Valid())//���յ���Ч�����ݰ�
			{
				Rx_Package_Handle(RX_Packet);
			}
			
			if(CheckTimeOut_Wifi())//WIFI�źų�ʱ����ʱ��û���յ���Ч��wifi�ź�
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

