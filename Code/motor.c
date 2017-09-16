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
	if(pwm!=0)
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
	if(pwm!=0)
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

/**************************************************
*��������	SetMotor_STOP
*����  ��	���ֹͣת������ɲ����
*����  ��	��
*����  ��	��
***************************************************/
void SetMotor_STOP(void)
{
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

/* ��е�� */
void SetArm( ARM_DIR dir)
{
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

/* ǰצ */
void SetClaw( CLAW_DIR dir )
{
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
			
//			if( dir > 0x80 )//��ת���ҵ������
//			{
//				speed_dif = dir-0x80;
//				if(carSpeed_R>=0)
//					carSpeed_R = carSpeed_R-speed_dif;
//				else
//					carSpeed_R = carSpeed_R+speed_dif;
//			}
//			else//��ת����������
//			{
//				speed_dif = 0x80 - dir;
//				if(carSpeed_L>=0)
//					carSpeed_L = carSpeed_L-speed_dif;	
//				else
//					carSpeed_L = carSpeed_L+speed_dif;
//			}
		}	
	
	Control_Motor_L(carSpeed_L);
	Control_Motor_R(carSpeed_R);
}

/*
	���ճ�ʱ���
	��ʱ��ͣ��
*/
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
#define TIMEOUT_WIFI 1000
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
	ǰצ����
	
*/
void Control_Claw(u8 claw)
{
	if( claw <= 0x40 )
		SetClaw( CLAW_HOLD );
	else if( claw >= 0xC0 )
		SetClaw( CLAW_RELEASE );
	else
		SetClaw( CLAW_STOP );		
}
