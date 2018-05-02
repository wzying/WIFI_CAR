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
//bit BIT_TMP;

void delay_adc(void)   // 5us
{
    unsigned char a,b;
    for(b=1;b>0;b--)
        for(a=10;a>0;a--);
}


void delayms(unsigned int t)   //?? 0us
{
    unsigned char a,b,c,d;
	  for(d=t;t>0;t--)
			for(c=17;c>0;c--)
        for(b=134;b>0;b--)
            for(a=2;a>0;a--);
}

void delay1ms(void)   //?? 0us
{
    unsigned char a,b,c;
    for(c=17;c>0;c--)
        for(b=134;b>0;b--)
            for(a=2;a>0;a--);
}

void delay10ms(void)   //?? 0us
{
    unsigned char a,b,c;
    for(c=193;c>0;c--)
        for(b=118;b>0;b--)
            for(a=2;a>0;a--);
}
