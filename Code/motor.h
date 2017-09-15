/***********************************************************************************
 Company 	: 	�����Ƽ�
 File Name 	:	motor.h
 Author 	:	WANG 
 Create Data 	: 	2017-05-12
 Last Modified 	:	2017-05-12
 Description 	:	�������ͷ�ļ�
 Version 	: 	1.0
************************************************************************************/

#ifndef _MOTOR_H_
#define	_MOTOR_H_

#define MOTOR_PWM_DIF  80 

/* 	���� 	IO��PWM1 + PC7	*/
#define	MOTOR_L	PC_ODR_bit.ODR7

/*	�ҵ��	IO��PWM0 + PD4	*/
#define MOTOR_R PD_ODR_bit.ODR4

/* 
	��е�� 
	P0.5 + P1.5
*/
#define MOTOR_ARM_A P05
#define MOTOR_ARM_B P15
typedef enum {
	ARM_STOP,
	ARM_UP,
	ARM_DOWN
}ARM_DIR;

/* 
	ǰצ 
	P1.0 + P0.3
*/
#define MOTOR_CLAW_A P10
#define MOTOR_CLAW_B P03
typedef enum {
	CLAW_STOP,
	CLAW_HOLD,
	CLAW_RELEASE
}CLAW_DIR;


void SetMotor_L(u8 pwm , u8 direction);
void SetMotor_R(u8 pwm , u8 direction);
void SetMotor_STOP(void);
void SetMotor_Brake(void);
/* ǰצ */
void SetClaw( CLAW_DIR dir );
/* ��е�� */
void SetArm( ARM_DIR dir);
void Control_Motor(u8 speed,u8 dir);
void Set_TimeOut_Motor(void);
u8 CheckTimeOut_Motor(void);

#endif
