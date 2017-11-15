#ifndef _ADC_H_
#define _ADC_H_

void Init_ADC(void);
u16 Get_ADC(void);
u16 Get_ADC_BAT(void);
void Send_Voltage(u16 v);
u8 Get_ADC_BAT_8Bit(void);
#endif