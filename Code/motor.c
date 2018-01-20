/***********************************************************************************
 Company 	: 	�����Ƽ�
 File Name 	:	motor.c
 Author 	:	WANG 
 Create Data 	: 	2017-05-12
 Last Modified 	:	2017-05-12
 Description 	:	�����������  
 Version 	: 	1.0
************************************************************************************/
#include "N76E003.h"
#include "Common.h"
#include "motor.h"
#include "PWM.h"
#include "SFR_Macro.h"
#include "delay.h"
#include "Function_define.h"
/**************************************************
*��������	SetMotor_L
*����  ��	���ơ���������ת���뷽��
					���� �� PWM1 + PWM4
*����  ��
			pwm��0-255	0->ֹͣ		255->����ٶ�
			direction�����Ƶ��������ת		0->��ת		1->��ת
*����  ��	��
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
*��������	SetMotor_R
*����  ��	���ơ��ҵ������ת���뷽��
					�ҵ�� �� PWM3 + PWM0
*����  ��
			pwm��0-255	0->ֹͣ		255->����ٶ�
			direction�����Ƶ��������ת		0->��ת		1->��ת
*����  ��	��
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
*��������	SetMotor_STOP
*����  ��	���ֹͣת������ɲ����
*����  ��	��
*����  ��	��
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
*��������	SetMotor_Brake
*����  ��	ɲ��
*����  ��	��
*����  ��	��
***************************************************/
void SetMotor_Brake(void)
{
		SetPWM(1 , 0xff);
		SetPWM(4 , 0xff);
		SetPWM(3 , 0xff);
		SetPWM(0 , 0xff);
}

/*
	С�����״̬
*/
static u8 Motor_Status;
void Set_Motor_Status(enum MOTOR_TYPE type , u8 status )
{
	switch(type)
	{
		case MOTOR_MAIN:
				if(status)
					Motor_Status |= MOTOR_STATUS_MAIN;
				else
					Motor_Status &= ~MOTOR_STATUS_MAIN;
			break;
		case MOTOR_ARM:
				if(status)
					Motor_Status |= MOTOR_STATUS_ARM;
				else
					Motor_Status &= ~MOTOR_STATUS_ARM;			
			break;
	}
}
u8 Check_Motor_Status(void)
{
	return Motor_Status;
}

/*
	����״̬��⣨���ֹͣ��
*/
u8 Check_Motor_Idle(void)
{
	if(Check_Motor_Status())
		return 0;//���ڵ����ת��æ״̬
	else
		return 1;//���ֹͣ������״̬
}

/* ��е�� */
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
	��ȡ��е��״̬
*/
ARM_DIR GetArmStatus(void)
{
	return arm_Dir_Tem;
}


/* ǰצ */
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
	����
	speed < 0 ��ת
  speed > 0 ��ת
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
	�ҵ��
	speed < 0 ��ת
  speed > 0 ��ת
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
	С��ǰ�����ҿ���
	speed : ǰ���ٶȿ�����
  dir   : ת���źſ�����
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
			if(speed == 0x80)//ԭ��ת��
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
			else if(speed > 0x80)//ǰ������ת��
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
			else if(speed < 0x80)//���˹���ת��
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
	
//		//Test ȫ����
//		if(carSpeed_L < 0)
//			carSpeed_L = -0x80;
//		else if(carSpeed_L > 0)
//			carSpeed_L = 0x80;
//		
//		if(carSpeed_R < 0)
//			carSpeed_R = -0x80;
//		else if(carSpeed_R > 0)
//			carSpeed_R = 0x80;
//		// End Test
		
	Control_Motor_L(carSpeed_L);
	Control_Motor_R(carSpeed_R);
}

/*
	���ճ�ʱ���
	��ʱ��ͣ��
*/
//#define TIMEOUT_MOTOR 700
#define TIMEOUT_MOTOR 500
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
	WiFi�źŽ��ճ�ʱ
*/
//#define TIMEOUT_WIFI 1000
#define TIMEOUT_WIFI 800
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
	��е�ۿ���
	������ arm -> WiFi�����ź�
*/
void Control_ARM(u8 arm)
{
	if( arm <= 0x40 )
	{
		SetArm( ARM_UP );
		Set_Motor_Status(MOTOR_ARM,1);//���õ��״̬
	}
	else if( arm >= 0xD0 )
	{
		SetArm( ARM_DOWN );
		Set_Motor_Status(MOTOR_ARM,1);//���õ��״̬
	}
	else
	{
		SetArm( ARM_STOP );
		Set_Motor_Status(MOTOR_ARM,0);//��λ���״̬
	}

}
/*
	ǰצ����
	
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
	ǰצ �źŽ��ճ�ʱ
*/
//#define TIMEOUT_CLAW 600
#define TIMEOUT_CLAW 400
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
