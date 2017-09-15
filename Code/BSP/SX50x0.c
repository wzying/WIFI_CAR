
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
#include "SX50x0.h"
 
/*********************全局变量************************/
EUINT8 gSx50x0DataBuff=0;//数据缓冲
EUINT8 gSx50x0SendBusy=0;//发送数据忙标志,0-空闲;1-忙;
/*********************函数声明************************/

/*********************函数实现************************/
void Sx50x0_IO_Init(void)
{
	P00_Input_Mode;//BUSY input
	P10_PushPull_Mode;//SDA output
}


#define STA_SX50_SEND_IDLE          (0)
#define STA_SX50_SEND_HEAD          (10)
#define STA_SX50_SEND_DATA_BIT_H    (20)
#define STA_SX50_SEND_DATA_BIT_L    (21)
#define STA_SX50_SEND_DATA_NEXT     (22)
#define STA_SX50_SEND_FINISHED      (30)
static UINT8 gSx50x0SendSta=STA_SX50_SEND_IDLE;
static UINT8 gSx50x0SendSMCnt=0;
static UINT8 gSx50x0SendBitNum=0;

//main loop 500us调用一次
void Sx50x0SendStateMachine(void)
{
  switch(gSx50x0SendSta)
  {
  case STA_SX50_SEND_IDLE:    
    break;
  case STA_SX50_SEND_HEAD:
    SX50X0_PIN_SDA=0;
    gSx50x0SendSMCnt=10;//5ms
    gSx50x0SendSta=STA_SX50_SEND_DATA_BIT_H;
    break;
  case STA_SX50_SEND_DATA_BIT_H:
    if(!gSx50x0SendSMCnt)
    {      
      if(gSx50x0DataBuff & 0x01)
        //逻辑1:高低电平3:1
      {
        gSx50x0SendSMCnt=3;
      }
      else
        //逻辑0:高低电平1:3
      {
        gSx50x0SendSMCnt=1;
      }
      SX50X0_PIN_SDA=1;
      gSx50x0SendSta=STA_SX50_SEND_DATA_BIT_L;
    }    
    break;
  case STA_SX50_SEND_DATA_BIT_L:
    if(!gSx50x0SendSMCnt)
    {      
      if(gSx50x0DataBuff & 0x01)
        //逻辑1:高低电平3:1
      {
        gSx50x0SendSMCnt=1;
      }
      else
        //逻辑0:高低电平1:3
      {
        gSx50x0SendSMCnt=3;
      }
      SX50X0_PIN_SDA=0;
      gSx50x0SendSta=STA_SX50_SEND_DATA_NEXT;
    } 
    break;
  case STA_SX50_SEND_DATA_NEXT:
    if(!gSx50x0SendSMCnt)
    {      
      if(gSx50x0SendBitNum<7)
      //未写完，继续写下一个
      {
        gSx50x0DataBuff>>=1;
        gSx50x0SendBitNum++;
        gSx50x0SendSta=STA_SX50_SEND_DATA_BIT_H;
      }
      else
      //写完一个Byte
      {
        gSx50x0SendSta=STA_SX50_SEND_IDLE;
        gSx50x0SendBitNum=0;
        SX50X0_PIN_SDA=1;
        gSx50x0SendBusy=0;//idle
		//		T3IE = 0;                						//关T3中断
      }
			 
			
    }  
    break;
  case STA_SX50_SEND_FINISHED:
    if(!gSx50x0SendSMCnt)
    {      
    }    
    break;
  default:
    break;
  }
	
 // if(gSx50x0SendSMCnt)gSx50x0SendSMCnt--;//500us/tick
}

void Sx50x_DriveTickCount(void)
{
 if(gSx50x0SendSMCnt)gSx50x0SendSMCnt--;//500us/tick
}

void Sx50x0SendData(UINT8 *pdin)
{
 //	T3IE = 1;                						//开T3中断
	gSx50x0DataBuff=*pdin;
  gSx50x0SendBitNum=0;
  SX50X0_PIN_SDA=1;
  gSx50x0SendSta=STA_SX50_SEND_HEAD;
}

