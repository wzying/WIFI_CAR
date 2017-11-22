
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
#include "GPIO.h"

#define RES_H		20
#define RES_L 	12
#define RES_RATIO 	RES_L/(RES_L+RES_H)
#define ADC_MAX			0xFF
#define VOL_REF			355			//参考电压3.55V

//电压采样标志
u8 vol_sample=0;
void Set_Vol_Sampling(void)
{
	vol_sample = 1;//正在电压采样
}
void Reset_Vol_Sampling(void)
{
	vol_sample = 0;
}
u8 Get_Vol_Sampling(void)
{
	return vol_sample;
}


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
#define AVERAGE_N		1
u8 Get_Vol_Average(void)
{
	u8 i=0;
	u16 sum=0;
	
	delay_adc();
	for(i=0;i<AVERAGE_N;i++)
	{
		sum += Get_ADC_BAT_8Bit();
		delay_adc();
	}
	
	return sum/AVERAGE_N;
}

/*
	根据ADC值计算电压值
	返回： 电池电压*100
*/
u16 Get_Bat_Voltage(u8 adc)
{
	return (u16)(adc*VOL_REF/(ADC_MAX*RES_RATIO));
}

void Send_Voltage(u8 v)
{
	Send_Data_To_UART0(0x01);
	Send_Data_To_UART0(v);
}

/*
	电压监测
*/
#define 	UNDER_VOL_TH		0x80
#define   HYSTERESIS_TH		5
//默认是充电版4.8V 。AA版*5/4
u8 bat_full = 0x6D;
u8 bat_10   = 0x5E;
u8 bat_20   = 0x64;
/*
	设置阈值
*/
#define BAT_AA_R 5/4
void Set_Bat_TH(void)
{
//	if(GET_BAT_STATUS)//充电版4.8V
//	{
//		bat_full = 0x6D;//5.1V
//		bat_10   = 0x5E;//3.7V
//		bat_20   = 0x57;//4.0V	
//	}
//	else//AA版6V
//	{
//		bat_full = 0x83;//6.1V
//		bat_10   = 0x70;//5.1V
//		bat_20   = 0x7A;//5.6V	
//	}
	if(GET_BAT_STATUS)//充电版4.8V
	{
//		bat_full = 0x79;//5.1V
//		bat_10   = 0x5A;//3.9V
//		bat_20   = 0x60;//4.1V	
		bat_full = 0x74;//5.1V
		bat_10   = 0x56;//3.9V
		bat_20   = 0x5C;//4.1V			
	}
	else//AA版6V
	{
		bat_full = 0x93;//6.1V
		bat_10   = 0x72;//4.8V
		bat_20   = 0x7F;//5.3V	
	}	
}

static u8 vol_Flag = 0xAF;
static u8 vol_avr;
static	u16 v_Gap;
static	u8  v_R=0xFF;
void Voltage_Monitor(void)
{ 
	static u16 i=0;
	
	if(i>=2500)
	{
		i=0;
		
		Set_Vol_Sampling();
		v_Gap = Get_ADC_BandGap();
		vol_avr = Get_Vol_Average();
		Reset_Vol_Sampling();
		
		if(vol_avr < 0xC5)//输入过高限制，以免溢出 . v_Gap 恒定= 0x5A
			v_R = (vol_avr*100)/v_Gap;
		else
			v_R = 0xFF;
		
		Send_Data_To_UART0(v_R);
		//Send_Data_To_UART0(vol_avr);
		
		if(v_R < bat_10)     //10% 
			vol_Flag = 0xA1;
		else if(v_R < bat_20)//20%   
		{
			if(vol_Flag!=0xA1)
				vol_Flag = 0xA2;
		}
		else if(v_R > bat_full)//电量充足  
			vol_Flag = 0xAF;
		
		Send_Voltage(vol_Flag);
	}
	else
	{
		i++;
	}	
}
