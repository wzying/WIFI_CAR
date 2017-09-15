#ifndef _BOARD_H
#define _BOARD_H

#define MAX_TIMER_NUM  4
#define IR_TIMER 0
#define IR_OUT   1
#define MIC_TIMER 2
#define TM_POS   3
 

void BoardInit(void);
void SysShutdown(void);
UINT8 SetTimer(UINT8 id,UINT16 ms);
UINT16 GetTimer(UINT8 id);
void ClearPulseCount(void);
UINT16 GetPulseCount(void);

 

 
UINT8 GetIOLevel(UINT8 i);
void  SetAttackPin(UINT8 val);

void HW_Board_Poll(void);

UINT8 GetTick_1s(void);
UINT8 GetTick_1ms(void);
UINT16 GetTickCount(void);
UINT16 GetIRTick(void);
UINT16 GetRandom(void);

u8 GetSysClock(void);
void ResetSysClock(void);

void RegGPIOIntHandler(UINT8 flg,FN_INT_HANDLER fn);

void printstr(UINT8* pdin,UINT8 dlen);
#endif