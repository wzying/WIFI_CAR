/***********************************************************************************
 Company 	: 	布塔科技
 File Name 	:	motor.c
 Author 	:	WANG 
 Create Data 	: 	2017-05-12
 Last Modified 	:	2017-05-12
 Description 	:	电机驱动函数  
 Version 	: 	1.0
************************************************************************************/
#include "N76E003.h"
#include "Common.h"
#include "motor.h"
#include "PWM.h"
#include "SFR_Macro.h"
#include "delay.h"

/**************************************************
*函数名：	SetMotor_L
*功能  ：	控制“左电机”的转速与方向
					左电机 ： PWM1 + PWM4
*参数  ：
			pwm：0-255	0->停止		255->最大速度
			direction：控制电机的正反转		0->正转		1->反转
*返回  ：	无
***************************************************/
void SetMotor_L(u8 pwm , u8 direction)
{
	if(pwm == 0)//
	{
		SetPWM(1 , 0);
		SetPWM(4 , 0);
	}
	else
	{
		pwm += MOTOR_PWM_DIF;
		if(direction)//??
		{
			SetPWM(1 , 0);
			SetPWM(4 , pwm);	
		}
		else//??
		{
			SetPWM(1 , pwm);
			SetPWM(4 , 0);
		}
	}
}

/**************************************************
*函数名：	SetMotor_R
*功能  ：	控制“右电机”的转速与方向
					右电机 ： PWM3 + PWM0
*参数  ：
			pwm：0-255	0->停止		255->最大速度
			direction：控制电机的正反转		0->正转		1->反转
*返回  ：	无
***************************************************/
void SetMotor_R(u8 pwm , u8 direction)
{
	if(pwm == 0)
	{
		SetPWM(3 , 0);
		SetPWM(0 , 0);		
	}
	else
	{
		pwm += MOTOR_PWM_DIF;
		if(direction)//??
		{
			SetPWM(3 , pwm);
			SetPWM(0 , 0);
		}
		else//??
		{
			SetPWM(3 , 0);
			SetPWM(0 , pwm);
		}
	}
}

/**************************************************
*函数名：	SetMotor_STOP
*功能  ：	电机停止转动（不刹车）
*参数  ：	无
*返回  ：	无
***************************************************/
void SetMotor_STOP(void)
{
//	clr_PWMRUN;

//	P12=0;P11=0; P00=0;P01=0;
	
	SetPWM(1 , 0);
	SetPWM(4 , 0);
	SetPWM(3 , 0);
	SetPWM(0 , 0);
}

/**************************************************
*函数名：	SetMotor_Brake
*功能  ：	刹车
*参数  ：	无
*返回  ：	无
***************************************************/
void SetMotor_Brake(void)
{
		SetPWM(1 , 0xff);
		SetPWM(4 , 0xff);
		SetPWM(3 , 0xff);
		SetPWM(0 , 0xff);
}

/* 机械臂 */
static ARM_DIR arm_Dir_Tem;
void SetArm( ARM_DIR dir)
{
	arm_Dir_Tem = dir;
	switch(dir)
	{
	case ARM_UP:
		MOTOR_ARM_A = 1;
		MOTOR_ARM_B = 0;
		break;
	case ARM_DOWN:
		MOTOR_ARM_A = 0;
		MOTOR_ARM_B = 1;
		break;
	case ARM_STOP:
	default:
		MOTOR_ARM_A = 0;
		MOTOR_ARM_B = 0;
		break;
	}
}
/*
	获取机械臂状态
*/
ARM_DIR GetArmStatus(void)
{
	return arm_Dir_Tem;
}


/* 前爪 */
static CLAW_DIR claw_Dir_Tem;
void SetClaw( CLAW_DIR dir )
{
	claw_Dir_Tem = dir;
	switch(dir)
	{
	case CLAW_RELEASE:
		MOTOR_CLAW_A = 1;
		MOTOR_CLAW_B = 0;
		break;
	case CLAW_HOLD:
		MOTOR_CLAW_A = 0;
		MOTOR_CLAW_B = 1;
		break;
	case CLAW_STOP:	
	default:
		MOTOR_CLAW_A = 0;
		MOTOR_CLAW_B = 0;
		break;
	}	
}
CLAW_DIR GetClawStatus(void)
{
	return claw_Dir_Tem;
}


/*
	左电机
	speed < 0 反转
  speed > 0 正转
*/
void Control_Motor_L(s16 speed)
{
	if(speed >= 0)
	{
		SetMotor_L(speed,0);
	}
	else
	{
		SetMotor_L(-speed,1);
	}
}
/*
	右电机
	speed < 0 反转
  speed > 0 正转
*/
void Control_Motor_R(s16 speed)
{
	if(speed >= 0)
	{
		SetMotor_R(speed,0);
	}
	else
	{
		SetMotor_R(-speed,1);
	}
}


/*
	小车前后左右控制
	speed : 前后速度控制量
  dir   : 转向信号控制量
*/
void Control_Motor(u8 speed,u8 dir)
{
	s16 carSpeed_L=0,carSpeed_R=0;
	//u8 speed_dif;
	u8 flag=0;
	
		carSpeed_L = speed - 0x80;
		carSpeed_R = speed - 0x80;

		if( dir != 0x80 )
		{
			if(speed == 0x80)//原地转向
			{
				if( dir > 0x80 )//
				{
					//carSpeed_L = 0;
					carSpeed_R = dir - 0x80;
					carSpeed_L = -carSpeed_R/2;
				}
				if( dir < 0x80 )
				{
					//carSpeed_R = 0;
					carSpeed_L = 0x80 - dir;
					carSpeed_R = -carSpeed_L/2;
				}
			}
			else if(speed > 0x80)//前进过程转向
			{
				if( dir > 0x80 )//
				{
					carSpeed_L = 0;
					carSpeed_R = dir - 0x80;
				}
				if( dir < 0x80 )
				{
					carSpeed_R = 0;
					carSpeed_L = 0x80 - dir;
				}				
			}
			else if(speed < 0x80)//后退过程转向
			{
				if( dir > 0x80 )//
				{
					carSpeed_L = 0;
					carSpeed_R = 0x80 - dir;
				}
				if( dir < 0x80 )
				{
					carSpeed_R = 0;
					carSpeed_L = dir - 0x80;
				}						
			}
		}	
	
	Control_Motor_L(carSpeed_L);
	Control_Motor_R(carSpeed_R);
}

/*
	接收超时检测
	超时则停车
*/
//#define TIMEOUT_MOTOR 700
#define TIMEOUT_MOTOR 300
u16 timeOut_Motor=0;
u8 CheckTimeOut_Motor(void)
{
	if(timeOut_Motor)
		timeOut_Motor--;
	else
		return 1;
	
	return 0;
}
void Set_TimeOut_Motor(void)
{
	timeOut_Motor = TIMEOUT_MOTOR;
}

/*
	WiFi信号接收超时
*/
//#define TIMEOUT_WIFI 1000
#define TIMEOUT_WIFI 600
u16 timeOut_Wifi=0;
u8 CheckTimeOut_Wifi(void)
{
	if(timeOut_Wifi)
		timeOut_Wifi--;
	else
		return 1;
	
	return 0;
}
void Set_TimeOut_Wifi(void)
{
	timeOut_Wifi = TIMEOUT_WIFI;
}


/*
	机械臂控制
	参数： arm -> WiFi控制信号
*/
void Control_ARM(u8 arm)
{
	if( arm <= 0x40 )
	{
		SetArm( ARM_UP );
		// Test code
		//delayms(1000);
	}
	else if( arm >= 0xD0 )
	{
		SetArm( ARM_DOWN );
	}
	else
	{
		SetArm( ARM_STOP );
	}

}
/*
	前爪控制
	
*/
static u8 claw_Turn_Flag = 0;
static u8 claw_EN = 0;
void Set_Claw_EN(void)
{
	claw_EN = 1;
}
void Reset_Claw_EN(void)
{
	claw_EN = 0;
}
u8 Check_Claw_EN(void)
{
	return claw_EN;
}
void Claw_Trun(void)
{
	if(claw_Turn_Flag)
		claw_Turn_Flag = 0;
	else
		claw_Turn_Flag = 1;
}
void Claw_Dir(void)
{
	Set_Claw_EN();
	Set_TimeOut_Claw();
	if(claw_Turn_Flag)
		SetClaw( CLAW_HOLD );
	else
		SetClaw( CLAW_RELEASE );
}

void Control_Claw(u8 claw)
{
	
	if( claw <= 0x40 )
	{
		//SetClaw( CLAW_HOLD );
		Claw_Dir();
	}
	else if( claw >= 0xC0 )
	{
		//SetClaw( CLAW_RELEASE );
		Claw_Dir();
	}
	else
	{
		SetClaw( CLAW_STOP );		
	}
}
/*
	前爪 信号接收超时
*/
//#define TIMEOUT_CLAW 600
#define TIMEOUT_CLAW 300
u16 timeOut_Claw=0;
u8 CheckTimeOut_Claw(void)
{
	if(timeOut_Claw)
		timeOut_Claw--;
	else
		return 1;
	
	return 0;
}
void Set_TimeOut_Claw(void)
{
	timeOut_Claw = TIMEOUT_CLAW;
}
