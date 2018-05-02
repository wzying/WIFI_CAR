#ifndef _VOLTAGE_H_
#define _VOLTAGE_H_

#define LM_I_MOTOR 180  //电机限流阈值

extern u16 g_I_motor;

typedef enum{
	UNDER_VOL,
	NORMAL,
}VOL_STATUS;

void Voltage_Monitor(void);
VOL_STATUS Get_Vol_Status(void);
void Set_Vol_Sampling(void);
void Reset_Vol_Sampling(void);
u8 Get_Vol_Sampling(void);
void Set_Bat_TH(void);
void Set_Motor_Idle(void);
void Set_Motor_Busy(void);
void Vol_Monitor_Idle(void);
void I_Limit(void);

#endif
