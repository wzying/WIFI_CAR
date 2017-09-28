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
#include "PWM.h"
#include "motor.h"

//bit BIT_TMP;
bit BIT_TMP;
//****SFRS		91H ****
#define set_SFRPAGE  BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS|=SET_BIT0;EA=BIT_TMP
#define clr_SFRPAGE  BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS&=~SET_BIT0;EA=BIT_TMP

/*
	PWM初始化
	左电机 ： PWM1 + PWM4
	右电机 ： PWM3 + PWM0
*/
void PWM_Init(void)
{
 
	P12_PushPull_Mode;//PWM0
	P11_PushPull_Mode;//PWM1
	
	//P10_PushPull_Mode;//PWM2
	P00_PushPull_Mode;//PWM3
	
	P01_PushPull_Mode;//PWM4
	//P03_PushPull_Mode;//PWM5
	
	P12=0;P11=0;/*P10=0;*/P00=0;P01=0;/*P03=0;*/
	
//	PWM0_P12_OUTPUT_DISABLE;
//	PWM1_P11_OUTPUT_DISABLE;
//	
//	PWM2_P10_OUTPUT_DISABLE;
//	PWM3_P00_OUTPUT_DISABLE;
//	
//	PWM4_P01_OUTPUT_DISABLE;
//	PWM5_P03_OUTPUT_DISABLE;

	PWM_IMDEPENDENT_MODE;
	PWM_CLOCK_FSYS;// selecte sysclk=16MHz as the input clock
	PWM_CLOCK_DIV_4;// pwm clock=16/16=1MHz
	PWMPH = 0x00;
	PWMPL = (0x7E + MOTOR_PWM_DIF);//
	/**********************************************************************
	PWM frequency = Fpwm/((PWMPH,PWMPL) + 1) <Fpwm = Fsys/PWM_CLOCK_DIV> 
						= (16MHz/8)/(0x063 + 1)
						= 20KHz  
	***********************************************************************/

	//-------- PWM start run--------------
	set_LOAD;
	set_PWMRUN;
	
	PWM0_P12_OUTPUT_ENABLE;
	PWM1_P11_OUTPUT_ENABLE;
	
	//PWM2_P10_OUTPUT_ENABLE;
	PWM3_P00_OUTPUT_ENABLE;
	
	PWM4_P01_OUTPUT_ENABLE;
	//PWM5_P03_OUTPUT_ENABLE;
}

void SetPWM_Start(void)
{
	//恢复PWM输出
	PWM0_P12_OUTPUT_ENABLE;
	PWM1_P11_OUTPUT_ENABLE;
	
	//PWM2_P10_OUTPUT_ENABLE;
	PWM3_P00_OUTPUT_ENABLE;
	
	PWM4_P01_OUTPUT_ENABLE;
	//PWM5_P03_OUTPUT_ENABLE;
	
	set_PWMRUN;
	
	//恢复机械臂
	SetArm(GetArmStatus());
	//恢复前爪
	SetClaw(GetClawStatus());
}

void SetPWM_Stop(void)
{
	clr_PWMRUN;

	PWM0_P12_OUTPUT_DISABLE;
	PWM1_P11_OUTPUT_DISABLE;
	
	//PWM2_P10_OUTPUT_DISABLE;
	PWM3_P00_OUTPUT_DISABLE;
	
	PWM4_P01_OUTPUT_DISABLE;
	//PWM5_P03_OUTPUT_DISABLE;

	P12=0;P11=0; P00=0;P01=0;	
	
	//短时间关闭机械臂
	MOTOR_ARM_A = 0;
	MOTOR_ARM_B = 0;	
	
	//短时间关闭前爪
	MOTOR_CLAW_A = 0;
	MOTOR_CLAW_B = 0;	
	
//	SetPWM(1 , 0); //占空比需要保留，不能清除。等电流降低后重新恢复输出
//	SetPWM(4 , 0);
//	SetPWM(3 , 0);
//	SetPWM(0 , 0);
}

void SetPWM(UINT8 chn,UINT8 val)
{
	switch(chn)
	{
		case 0:
			PWM0H = 0x00;					
  		PWM0L = val;
			set_LOAD;
		break;
		case 1:
			PWM1H = 0x00;					
  		PWM1L = val;
			set_LOAD;
		break;
		case 2:
			PWM2H = 0x00;					
  		PWM2L = val;
			set_LOAD;
		break;
		case 3:
			PWM3H = 0x00;					
  		PWM3L = val;
			set_LOAD;
		break;
		case 4:
			set_SFRPAGE;
			PWM4H = 0x00;					
  		PWM4L = val;
			clr_SFRPAGE;
			set_LOAD;
		break;
		case 5:
			set_SFRPAGE;
			PWM5H = 0x00;					
  		PWM5L = val;
			clr_SFRPAGE;
			set_LOAD;
		break;
	}
}
 
void PWMEnable(UINT8 chn)
{
	if(chn==0)
	{
		PWM0_P12_OUTPUT_ENABLE;
	}
	else if(chn==1)
	{
		PWM1_P11_OUTPUT_ENABLE;
	}
}

void PWMDisable(UINT8 chn)
{
	if(chn==0)
	{
		PWM0_P12_OUTPUT_DISABLE;
		P12=1;
	}
	else if(chn==1)
	{
		PWM1_P11_OUTPUT_DISABLE;
		P11=1;
	}
}
