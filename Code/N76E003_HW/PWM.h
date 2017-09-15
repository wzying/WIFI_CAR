#ifndef _PWM_H
#define _PWM_H
void PWM_Init(void);
void SetPWM(UINT8 chn,UINT8 val);
void PWMDisable(UINT8 chn);
void PWMEnable(UINT8 chn);
#endif