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
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "string.h"
#include "UART.h"

EUINT8 mdlen;
EUINT8 mTxIdx;
EUINT8 mRxLen;
EUINT8 mRxBuf[20];
EUINT8 mTxBuf[20];
/*
init uart0 with the pin mapped in p06,p07
and the baudrate is drived by timer1
*/
void TestUartInit(void)
{
		UINT32 u32Baudrate;
	
		mRxLen=0;
		u32Baudrate=9600;
	
		P06_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
		P07_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
	
    SCON = 0x50;     	//UART0 Mode1,REN=1,TI=1
    TMOD |= 0x20;    	//Timer1 Mode1
    
    set_SMOD;        	//UART0 Double Rate Enable
    set_T1M;
    clr_BRCK;        	//Serial port 0 baud rate clock source = Timer1
 
    TH1 = 256 - (1000000/u32Baudrate+1);               /*16 MHz */
 
    set_TR1;
		set_TI;						//For printf function must setting TI = 1
	
		set_ES;					//For interrupt enable
}



void UART0SendData(UINT8* pdin,UINT8 dlen)
{
	if(dlen<=20)
	{
		mdlen=dlen;
	}
	else
	{
		mdlen=20;
	}
	memcpy(mTxBuf,pdin,mdlen);
	
	SBUF = *mTxBuf;
	mTxIdx=0;
  
}

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
				//SendDataUart(RX_Packet,DATA_LEN);
				//Rx_Package_Handle(RX_Packet);
				
				//ResetIndexRx();//成功接收完一个包，复位下次接收
			}
		}
		index_Rx++;
	}
}  
//
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


void SerialPort0_ISR(void) interrupt 4
{
    if (RI==1) 
    {                                       /* if reception occur */
      clr_RI;                             /* clear reception flag for next reception */
		  //Send_Data_To_UART0(SBUF);
			RecievePacket(SBUF);
    }
    if(TI==1)
    {
      clr_TI;                             /* if emission occur */
    }
}
