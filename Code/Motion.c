/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2017 Buta Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Website: http://www.ptahtoy.com
//  E-Mail : linshigao@ptahtoy.com
//  Date   : Jun/24/2017
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: CoreXtalRobot 
//***********************************************************************************************************

#include "Common.h"
#include "Board.h"
#include "PWM.h"
#include "Motion.h"

 
EUINT8 mMotorSta;
EUINT8 mCoilSta;
 
void Motion_Init(void)
{
	mMotorSta=0;
 
}

//监控电机与电磁铁的运动时间
//call per 1ms
//限位器监测
void Motion_Sta(void)
{
 
	  
}