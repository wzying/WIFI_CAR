#ifndef _VOLTAGE_H_
#define _VOLTAGE_H_

typedef enum{
	UNDER_VOL,
	NORMAL,
}VOL_STATUS;

void Voltage_Monitor(void);
VOL_STATUS Get_Vol_Status(void);

#endif
