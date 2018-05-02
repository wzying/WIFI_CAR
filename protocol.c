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

/*
	protocol.c
	����Э��
	���ڲ����ʣ� 4800
	UARTһ�����ݰ����� 11 Byte ������ֻʹ���� 0 - 4 ��5���ֽڣ���6���ֽ��������
	Byte[0] �� 0x66 ������ͷ
	Byte[1] �� ת���źţ������ź��м�ֵ0x80 ����Χ0x00 - 0xFF
	Byte[2] �� ǰ���ٶȣ������ź��м�ֵ0x80������0x80��ǰ��С��0x80�˺�
	Byte[3] �� ��е��  �������ź��м�ֵ0x80 
	Byte[4] �� צ��    �������ź��м�ֵ0x80 
*/


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

/*
	���յ���Ч�����ݰ�
*/
u8 valid_R = 0;
u8 Check_Recieve_Valid(void)
{
	u8 res;
	res = valid_R;
	valid_R = 0;//��λ
	return res;
}
void Set_Recieve_valid(void)
{
	valid_R = 1;
}

//���յ������ݰ�
uint8_t RX_Packet[LEN_PACKET_MAX];
static uint8_t index_Rx = 0;
#define TIME_OUT_RX_MAX 10
uint16_t timeOut_Rx = 0;//���ճ�ʱ
void RecievePacket(uint8_t d)
{
	timeOut_Rx = TIME_OUT_RX_MAX;//���ճ�ʱ����ֵ
	if(index_Rx < LEN_PACKET_MAX)
	{
		RX_Packet[index_Rx] = d;

		if(index_Rx == (DATA_LEN-1))
		{
			if(RX_Packet[0] == 0x66)//��ͷ0x66
			{
				Set_Recieve_valid();//���յ���Ч�����ݰ�
			}
		}
		index_Rx++;
	}
}  

/*
	�������ݽ���
	RX_Handle
*/
void Rx_Package_Handle(uint8_t *uartBuf)
{
	Set_TimeOut_Wifi();//WIFI��ʱ��λ
	SetWifiStatus();//����WIFIΪ����״̬
	
	if(uartBuf[7] == 0x01)//����ң��
	{
		if( (uartBuf[1]!=0x80) || (uartBuf[2]!=0x80) || (uartBuf[3]!=0x80) )
		{
			Control_Motor_Key(uartBuf[1],uartBuf[2],uartBuf[3]);
			
			Set_TimeOut_Motor();//��λ��ʱ����
		}
		
		{
			Control_ARM (uartBuf[4]);
			Control_Claw(uartBuf[5]);
		}
	}
	//if(uartBuf[7] == 0x02)
	else //�����ǿ���
	{
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
}

void ResetIndexRx(void)
{
	index_Rx = 0;
}
//��ⳬʱ
uint8_t CheckTimeOut(void)
{
	if(timeOut_Rx)
		timeOut_Rx--;
	else
		return 1;	//timeOut_Rx==0,����1
	
	return 0;	
}
