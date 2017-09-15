#ifndef _TIMER_H
#define _TIMER_H
void Timer0Init(void);
UINT16 GetTimer0Cnt(void);
void Timer3Init(FN_INT_HANDLER handler);
void StartTimer3(void);
UINT16 GetTimer3Count(void);
#endif