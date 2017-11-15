
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
#include "ADC.h"
#include "voltage.h"

#define RES_H		20
#define RES_L 	12
#define RES_RATIO 	RES_L/(RES_L+RES_H)
#define ADC_MAX			0xFF
#define VOL_REF			355			//参考电压3.55V

VOL_STATUS vol_Status = NORMAL;
void Set_Vol_Status(VOL_STATUS s)
{
	vol_Status = s;//欠压
}
VOL_STATUS Get_Vol_Status(void)
{
	return vol_Status;
}

/*
	平均值
*/
#define AVERAGE_N		5
u8 Get_Vol_Average(void)
{
	u8 i=0;
	u16 sum=0;
//	for(i=0;i<AVERAGE_N;i++)
//	{
//		sum += Get_ADC_BAT_8Bit();
//		delay10ms();
//	}
	
	sum = Get_ADC_BAT_8Bit();
	delay10ms();
	sum += Get_ADC_BAT_8Bit();
	delay10ms();
	sum += Get_ADC_BAT_8Bit();
	delay10ms();
	sum += Get_ADC_BAT_8Bit();
	
	return sum/4;
}

/*
	根据ADC值计算电压值
	返回： 电池电压*100
*/
u16 Get_Bat_Voltage(u8 adc)
{
	return (u16)(adc*VOL_REF/(ADC_MAX*RES_RATIO));
}

void Send_Voltage(u16 v)
{
	//v = v>>4;
	Send_Data_To_UART0(0x01);
	Send_Data_To_UART0(v);
}

/*
	电压监测
*/
#define 	UNDER_VOL_TH		0x80
#define   HYSTERESIS_TH		5
u8 vol_level[5] = {0x70,0x80,0x90,0xA0,0xB0};
u8 Check_Vol_Level(u8 v)
{
	u8 level;
	//
	if( v < vol_level[0] )
		level = 0;
  else 	if( v < vol_level[1] )
		level = 1;
	else 	if( v < vol_level[2] )
		level = 2;
	else 	if( v < vol_level[3] )
		level = 3;
	
	if( v > (vol_level[level]+HYSTERESIS_TH) )
	{
		if(level<3)
			level++;
	}
	return level;
}

static u8 vol_Flag = 0xAF;
void Voltage_Monitor(void)
{ 
	static i=0;
	u16 vol;
	
	if(i>=2000)
	{
		i=0;
		
//		vol = Get_ADC_BAT();
//		vol = vol>>4;		
		
		//vol = Get_ADC_BAT_8Bit();
		vol = Get_Vol_Average();
		
		//Send_Voltage(vol);
//		if( vol < 0x7A )//4.4V
//		{
//			vol_Flag = 0xA0; //电量过低		
//		}
//		else if( vol > (0x7A+15) )
//			vol_Flag = 0xAF;
		
		if(vol < 0x75)     //10%
			vol_Flag = 0xA1;
		else if(vol < 0x7E)//20%
		{
			if(vol_Flag!=0xA1)
				vol_Flag = 0xA2;
		}
		Send_Voltage(vol_Flag);
	}
	else
	{
		i++;
	}	
	
//	if(vol < UNDER_VOL_TH)
//		Set_Vol_Status(UNDER_VOL);
//	else if(vol > (UNDER_VOL_TH+3)) 
//		Set_Vol_Status(NORMAL);
}
