#ifndef __SX50X0_H_
#define __SX50X0_H_

/*********************宏定义************************/

#ifndef false    
#define false 0    
#endif    
    
#ifndef true    
#define true 1    
#endif  

#define SX50X0_PIN_SDA                P10 
#define SX50X0_PIN_BUSY               P00 
//#define PA_PIN_EN											P1_2


///*********************全局变量************************/ 
//extern UINT8 gSx50x0DataBuff;//数据缓冲
//extern UINT8 gSx50x0SendBusy;//发送数据忙标志,0-空闲;1-忙;
///*********************全局函数************************/ 
void Sx50x0_IO_Init(void);
void Sx50x0SendStateMachine(void);
void Sx50x0SendData(UINT8 *pdin);
void Sx50x_DriveTickCount(void);
#endif

