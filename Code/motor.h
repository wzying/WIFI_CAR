/***********************************************************************************
 Company 	: 	布塔科技
 File Name 	:	motor.h
 Author 	:	WANG 
 Create Data 	: 	2017-05-12
 Last Modified 	:	2017-05-12
 Description 	:	电机控制头文件
 Version 	: 	1.0
************************************************************************************/

#ifndef _MOTOR_H_
#define	_MOTOR_H_

#define MOTOR_PWM_DIF  80 

/* 	左电机 	IO：PWM1 + PC7	*/
#define	MOTOR_L	PC_ODR_bit.ODR7

/*	右电机	IO：PWM0 + PD4	*/
#define MOTOR_R PD_ODR_bit.ODR4

/* 
	机械臂 
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
	前爪 
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
/* 前爪 */
void SetClaw( CLAW_DIR dir );
/* 机械臂 */
void SetArm( ARM_DIR dir);
void Control_Motor(u8 speed,u8 dir);
void Set_TimeOut_Motor(void);
u8 CheckTimeOut_Motor(void);

#endif
