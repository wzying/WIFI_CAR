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
#include "protocol.h"

EUINT8 mdlen;
EUINT8 mTxIdx;
EUINT8 mRxLen;
EUINT8 mRxBuf[20];
EUINT8 mTxBuf[20];

/*
	UART·¢ËÍ
*/
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
	UARTÖÐ¶Ï
*/
void SerialPort0_ISR(void) interrupt 4
{
    if (RI==1) 
    {                                       /* if reception occur */
      clr_RI;                             /* clear reception flag for next reception */
			RecievePacket(SBUF);
    }
    if(TI==1)
    {
      clr_TI;                             /* if emission occur */
    }
}
