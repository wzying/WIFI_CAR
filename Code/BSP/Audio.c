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
#include "Common.h"
#include "SX50X0.h"
#include "Audio.h"
 

#define MAX_SONG_NUM	((UINT8)29)

//初始化声音
void Audio_Init(void)
{
	Sx50x0_IO_Init();
 
}

void AudioSta(void)
{
	Sx50x0SendStateMachine();
}

void Period_task_500us(void)
{		
	
	
	Sx50x_DriveTickCount();
}




//播放第idx段语音
//idx:0x01~0x1D
UINT8 PlayOneSong(UINT8 idx)
{
	if(idx>0x10)return 0xE1;//参数错误
	
	Sx50x0SendData(&idx);
	return 0x00;//执行成功
}
 #if(0)
//循环播放第idx段语音
//idx:0x01~0x10
void LoopPlayOneSong(UINT8 idx)
{
	UINT8 sLoopMark=0xF2;
	
	if(idx>0x10)return;
	
	Sx50x0SendData(&idx);
	Sx50x0SendData(&sLoopMark);
}
 
//设置音量
//size:0x00~0x0F
void SetAudioVol(UINT8 isize)
{
	if(isize>0x0F)return;
	isize+=0xE0;
	Sx50x0SendData(&isize);
	
}

//停止当前的播放
void StopPlay(void)
{
	UINT8 sStopMark=0xFE;
	
	Sx50x0SendData(&sStopMark);
}

#endif