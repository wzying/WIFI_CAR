#ifndef _GPIO_H
#define _GPIO_H

#define P_SW_BAT	P15
#define GET_BAT_STATUS (P_SW_BAT == 1)

void GPIO_Init(void);
void SetGPx0IntHandler(FN_INT_HANDLER fn);
void SetGPx5IntHandler(FN_INT_HANDLER fn);
void SetGPx6IntHandler(FN_INT_HANDLER fn);
void SetEX0IntHandler(FN_INT_HANDLER fn);
void SetEX1IntHandler(FN_INT_HANDLER fn);
//void ToggleIROut(void);
//void ToggleIROut_test(void);
//void SetIR_Pin_test(UINT8 sta);
//void SetIR_Pin(UINT8 sta);
//void SetP14(UINT8 sta);
//UINT8 GetP05Val(void);
//UINT8 GetP06Val(void);
//UINT8 GetP02Val(void);
//UINT8 GetP15Val(void);
//UINT8 GetP13Val(void);

UINT8 GetPort0Val(UINT8 pin);
UINT8 GetPort1Val(UINT8 pin);
UINT8 GetPort3Val(UINT8 pin);

void  SetPort2Val(UINT8 pin,UINT8 val);
void flipIO(void);
#endif