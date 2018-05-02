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
	串口协议
	串口波特率： 4800
	UART一个数据包发送 11 Byte 。这里只使用了 0 - 4 共5个字节，后6个字节无需关心
	Byte[0] ： 0x66 ，数据头
	Byte[1] ： 转向信号，空闲信号中间值0x80 ，范围0x00 - 0xFF
	Byte[2] ： 前进速度，空闲信号中间值0x80，大于0x80往前，小于0x80退后
	Byte[3] ： 机械臂  ，空闲信号中间值0x80 
	Byte[4] ： 爪子    ，空闲信号中间值0x80 
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
	接收到有效的数据包
*/
u8 valid_R = 0;
u8 Check_Recieve_Valid(void)
{
	u8 res;
	res = valid_R;
	valid_R = 0;//复位
	return res;
}
void Set_Recieve_valid(void)
{
	valid_R = 1;
}

//接收到的数据包
uint8_t RX_Packet[LEN_PACKET_MAX];
static uint8_t index_Rx = 0;
#define TIME_OUT_RX_MAX 10
uint16_t timeOut_Rx = 0;//接收超时
void RecievePacket(uint8_t d)
{
	timeOut_Rx = TIME_OUT_RX_MAX;//接收超时计数值
	if(index_Rx < LEN_PACKET_MAX)
	{
		RX_Packet[index_Rx] = d;

		if(index_Rx == (DATA_LEN-1))
		{
			if(RX_Packet[0] == 0x66)//包头0x66
			{
				Set_Recieve_valid();//接收到有效的数据包
			}
		}
		index_Rx++;
	}
}  

/*
	接收数据解析
	RX_Handle
*/
void Rx_Package_Handle(uint8_t *uartBuf)
{
	Set_TimeOut_Wifi();//WIFI超时复位
	SetWifiStatus();//设置WIFI为连接状态
	
	if(uartBuf[7] == 0x01)//按键遥控
	{
		if( (uartBuf[1]!=0x80) || (uartBuf[2]!=0x80) || (uartBuf[3]!=0x80) )
		{
			Control_Motor_Key(uartBuf[1],uartBuf[2],uartBuf[3]);
			
			Set_TimeOut_Motor();//复位超时计数
		}
		
		{
			Control_ARM (uartBuf[4]);
			Control_Claw(uartBuf[5]);
		}
	}
	//if(uartBuf[7] == 0x02)
	else //陀螺仪控制
	{
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
}

void ResetIndexRx(void)
{
	index_Rx = 0;
}
//检测超时
uint8_t CheckTimeOut(void)
{
	if(timeOut_Rx)
		timeOut_Rx--;
	else
		return 1;	//timeOut_Rx==0,返回1
	
	return 0;	
}
