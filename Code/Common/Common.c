/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2017 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Jan/21/2017
//***********************************************************************************************************
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"

void delay_uart(void)   //4ms
{
    unsigned char a,b,c;
    for(c=12;c>0;c--)
        for(b=130;b>0;b--)
            for(a=19;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
}

//----------------------------------------------------------------------------------
//  enable the global interrupt switcher 
//----------------------------------------------------------------------------------
void GlobalInterruptEnable(void)
{
	EA=1;
}

void DisableInterrupts(void)
{
	EA=0;
}
////----------------------------------------------------------------------------------
//// UART0 baud rate initial setting 
////----------------------------------------------------------------------------------
void InitialUART0_Timer1(UINT32 u32Baudrate)    //T1M = 1, SMOD = 1
{
		P06_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
		P07_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
	
    SCON = 0x50;     	//UART0 Mode1,REN=1,TI=1
    TMOD |= 0x20;    	//Timer1 Mode1
    
    set_SMOD;        	//UART0 Double Rate Enable
    set_T1M;
    clr_BRCK;        	//Serial port 0 baud rate clock source = Timer1
 
#ifdef FOSC_160000
    TH1 = 256 - (1000000/u32Baudrate+1);               /*16 MHz */
#endif    	
#ifdef FOSC_166000
    TH1 = 256 - (1037500/u32Baudrate);         		     /*16.6 MHz */
#endif
    set_TR1;
		set_TI;						//For printf function must setting TI = 1
	
		set_ES;					//For interrupt enable
}

////---------------------------------------------------------------
//void InitialUART0_Timer3(UINT32 u32Baudrate) //use timer3 as Baudrate generator
//{
//		P06_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
//		P07_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit	
//	
//    SCON = 0x50;     //UART0 Mode1,REN=1,TI=1
//    set_SMOD;        //UART0 Double Rate Enable
//    T3CON &= 0xF8;   //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
//    set_BRCK;        //UART0 baud rate clock source = Timer3

//#ifdef FOSC_160000
//		RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);  		/*16 MHz */
//		RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);			/*16 MHz */
//#endif
//#ifdef FOSC_166000
//    RH3    = HIBYTE(65536 - (1037500/u32Baudrate)); 			/*16.6 MHz */
//    RL3    = LOBYTE(65536 - (1037500/u32Baudrate)); 			/*16.6 MHz */
//#endif
//    set_TR3;         //Trigger Timer3
//		set_TI;					 //For printf function must setting TI = 1
//	
//		set_ES;					//For interrupt enable
//}

UINT8 Receive_Data_From_UART0(void)
{
    UINT8 c;
    while (!RI);
    c = SBUF;
    RI = 0;
    return (c);
}

void Send_Data_To_UART0 (uint8_t c)
{
	u16 t=50000;
	
    TI = 0;
    SBUF = c;
    while(TI==0);
//	  {
//			if(t)
//				t--;
//			else
//				break;
//		}
		//delay_uart();
}

void SendDataUart(uint8_t *buf,uint8_t len)
{
	while(len--)
	{
		Send_Data_To_UART0(*buf);
		buf++;
	}
}

void MyPrintf(uint8_t *buf)
{
	while(*buf != NULL)
	{
		Send_Data_To_UART0(*buf);
		buf++;
	}
}


////----------------------------------------------------------------------------------
//// UART1 baud rate initial setting 
////----------------------------------------------------------------------------------
//void InitialUART1_Timer3(UINT32 u32Baudrate) //use timer3 as Baudrate generator
//{
//		P02_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
//		P16_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
//	
//	  SCON_1 = 0x50;   	//UART1 Mode1,REN_1=1,TI_1=1
//    T3CON = 0x08;   	//T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1), UART1 in MODE 1
//		clr_BRCK;
//	
//#ifdef FOSC_160000
//		RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);  		/*16 MHz */
//		RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);			/*16 MHz */
//#endif
//#ifdef FOSC_166000
//		RH3    = HIBYTE(65536 - (1037500/u32Baudrate));  			/*16.6 MHz */
//		RL3    = LOBYTE(65536 - (1037500/u32Baudrate));				/*16.6 MHz */
//#endif
//    set_TR3;         //Trigger Timer3
//}

//UINT8 Receive_Data_From_UART1(void)
//{
//    UINT8 c;
//    
//    while (!RI_1);
//    c = SBUF_1;
//    RI_1 = 0;
//    return (c);
//}

//void Send_Data_To_UART1 (UINT8 c)
//{
//    TI_1 = 0;
//    SBUF_1 = c;
//    while(TI_1==0);
//}



/*==========================================================================*/
#ifdef SW_Reset
void SW_Reset(void)
{
    TA = 0xAA;
    TA = 0x55;
    set_SWRST;
}
#endif
/*==========================================================================*/
