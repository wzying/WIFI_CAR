
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

u16 g_I_motor;	//电机电流
u8 bat_full = 0;
u8 bat_10   = 0;
u8 bat_20   = 0;

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

/*
	平均值
*/
#define AVERAGE_N		2
u8 Get_Vol_Average(void)
{
	u8 adc;
//	u8 i=0;
//	u16 sum=0;
//	
//	delay_adc();
//	for(i=0;i<AVERAGE_N;i++)
//	{
//		sum += Get_ADC_BAT_8Bit();
//		delay_adc();
//	}
//	
//	return sum/AVERAGE_N;
	
	adc = Get_ADC_BAT_8Bit();
	return adc;
}

/*
	发送电池电量到APP
  格式：
  66 D1 D2 D3 D4 D5 CHECKSUM 99
	CHECKSUM= (D1^D2^D3^D4^D5)&0XFF

*/
u8 voltage_data[8] = {0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x99};
void Send_Voltage(u8 v)
{
//	Send_Data_To_UART0(0x01);
//	Send_Data_To_UART0(v);
	voltage_data[1] = v;
	voltage_data[6] = v;
	SendDataUart(voltage_data,8);
}

/*
	获取电机电流的比例系数
			  0 - LM_I_MOTOR
	返回：0 - 10
*/
u8 Get_I_Motor_R(void)
{
	u8 r;
	if(g_I_motor > LM_I_MOTOR)
	{
		r = 10;
	}
	else
	{
		r = (u8)((g_I_motor*10)/LM_I_MOTOR);
	}
	return r;
}

/*
	根据电机电流设置阈值
  电池：5节充电电池-6V  or  5节干电池-7.5V
	电流越大，阈值越小
*/
void Set_Bat_TH_R(u8 r)
{
	u8 d;
	d = r*5;
	
	bat_full = 0x96-d;//6.2V
	bat_20   = 0x87-d;//5.6V
	bat_10   = 0x7A-d;//5.2V		
}

/*
	电压监测
*/
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
		
		Set_Vol_Sampling();//正在采样中
		v_Gap = Get_ADC_BandGap();		//单片机内部标准电压，1.22V
		vol_avr = Get_Vol_Average();	//采样电压。
		//计算vol_avr 与 v_Gap 的比值，就能知道实际电压
		Reset_Vol_Sampling();//电压采样结束
		
		/* v_R=1 对应实际电压 0.041V */
		if(vol_avr < 0xC5)//输入过高限制，以免溢出 . v_Gap 恒定= 0x5A
			v_R = (vol_avr*100)/v_Gap;
		else
			v_R = 0xFF;
		
		Set_Bat_TH_R(Get_I_Motor_R());//设置电量阈值
		
		if(v_R < bat_10)     //10% 
			vol_Flag = 0xA1;
		else if(v_R < bat_20)//20%   
		{
			if(vol_Flag!=0xA1)
				vol_Flag = 0xA2;
		}
		else if(v_R > bat_full)//电量充足  
			vol_Flag = 0xAF;
		
		//发送电量状态到APP
		Send_Voltage(vol_Flag);
	}
	else
	{
		i++;
	}	
}

/*
	马达电流 限流
  过流、堵转 保护
*/
void I_Limit(void)
{
	//if(Get_ADC() > 200)
	if(!Get_Vol_Sampling())//如果没有在进行电压采样，才能进行电流采样
	{
		g_I_motor = Get_ADC();
		if( g_I_motor > LM_I_MOTOR )
		{
			SetPWM_Stop();
			//SetPWM_Start();
		}
		else
		{
			SetPWM_Start();
		}
	}	
}

