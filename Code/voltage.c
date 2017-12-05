
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

u16 g_I_motor;	//电机电流

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
	获取电机电流的比例系数
			0 - LM_I_MOTOR
	》》	0 - 10
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
		bat_full = 0x79;//5.1V
		bat_10   = 0x5A;//3.9V
		bat_20   = 0x60;//4.1V		
	}
	else//AA版6V
	{
		bat_full = 0x93;//6.1V
		bat_10   = 0x67;//4.4V
		bat_20   = 0x72;//4.8V		
	}	
}
/*
	根据电机电流设置阈值
  电池：5节充电电池 6V
    or  5节干电池   7.5V
*/
void Set_Bat_TH_R(u8 r)
{
	u8 d;
	d = r*5;
	
	bat_full = 0x96-d;//6.2V
	bat_20   = 0x87-d;//5.6V
	bat_10   = 0x7A-d;//5.2V		
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
		
		Set_Vol_Sampling();//正在采样中
		v_Gap = Get_ADC_BandGap();
		vol_avr = Get_Vol_Average();
		Reset_Vol_Sampling();
		
		/* v_R=1 对应实际电压 0.041V */
		if(vol_avr < 0xC5)//输入过高限制，以免溢出 . v_Gap 恒定= 0x5A
			v_R = (vol_avr*100)/v_Gap;
		else
			v_R = 0xFF;
		
		//Send_Data_To_UART0(v_R);
		//Send_Data_To_UART0(vol_avr);
		//Send_Data_To_UART0( (u8)(g_I_motor>>8) );
		//Send_Data_To_UART0( (u8)g_I_motor );
		//Send_Data_To_UART0(Get_I_Motor_R());
		
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
		
		Send_Voltage(vol_Flag);
	}
	else
	{
		i++;
	}	
}

/*
	电量检测 -- 检测停止状态的电压
*/
void Vol_Monitor_Idle(void)
{
	static u16 i=0;
	
	if(i>=2500)
	{
		i=0;
		
		//if(Check_Motor_Idle())//空闲状态才开启电量检测
		{
			Set_Vol_Sampling();//正在采样中
			v_Gap = Get_ADC_BandGap();
			vol_avr = Get_Vol_Average();
			Reset_Vol_Sampling();
			
			if(vol_avr < 0xC5)//输入过高限制，以免溢出 . v_Gap 恒定= 0x5A
				v_R = (vol_avr*100)/v_Gap;
			else
				v_R = 0xFF;
			
			//Send_Data_To_UART0(v_R);
			//Send_Data_To_UART0(vol_avr);
			//Send_Data_To_UART0( (u8)(g_I_motor>>8) );
			//Send_Data_To_UART0( (u8)g_I_motor );
			
			
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
	}
	else
	{
		i++;
	}		
}


