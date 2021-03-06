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

#define MOTOR_STATUS_MAIN	0x01
#define MOTOR_STATUS_ARM 	0x02
enum MOTOR_TYPE{
	MOTOR_MAIN,
	MOTOR_ARM,
	MOTOR_CLAW,
};

#define MOTOR_PWM_DIF  60 

/* 	左电机 	IO：PWM1 + PC7	*/
#define	MOTOR_L	PC_ODR_bit.ODR7

/*	右电机	IO：PWM0 + PD4	*/
#define MOTOR_R PD_ODR_bit.ODR4

/* 
	机械臂 
	P1.4 + P1.3
*/
#define MOTOR_ARM_A P14
#define MOTOR_ARM_B P13
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
void Control_Motor_Key(u8 speed,u8 dir_l,u8 dir_r);
void Set_TimeOut_Motor(void);
u8 CheckTimeOut_Motor(void);
void Control_Claw(u8 claw);
void Control_ARM(u8 arm);
void Set_TimeOut_Wifi(void);
u8 CheckTimeOut_Wifi(void);
ARM_DIR GetArmStatus(void);
CLAW_DIR GetClawStatus(void);
void Set_Claw_EN(void);
void Reset_Claw_EN(void);
u8 Check_Claw_EN(void);
void Claw_Trun(void);
u8 CheckTimeOut_Claw(void);
void Set_TimeOut_Claw(void);
u8 Check_Motor_Status(void);
u8 Check_Motor_Idle(void);

#endif
