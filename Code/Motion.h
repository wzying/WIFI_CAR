#ifndef _MOTION_H
#define _MOTION_H

#define MT_STA_IDLE 0

 

#define CL_STA_IDLE    0
#define CL_STA_STEP0_0 1
#define CL_STA_STEP0_1 2
#define CL_STA_STEP0_2 3
 

void Motion_Init(void);
void Motion_Sta(void);
 
UINT8 MotorActDone(void);
#endif