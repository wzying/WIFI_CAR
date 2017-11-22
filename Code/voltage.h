#ifndef _VOLTAGE_H_
#define _VOLTAGE_H_

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

#endif
