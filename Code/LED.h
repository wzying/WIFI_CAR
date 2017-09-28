/*

*/
#ifndef _LED_H_
#define _LED_H_

#define WIFI_CONNECT_TIMEOUT 3000	// 3000*0.5mS = 1.5S
#define LED1_STATUS P17
#define LED2_STATUS P30

void LED_Init(void);
void Set_LED_WIFI(void);
void Reset_LED_WIFI(void);
void SetWifiStatus(void);
u8 CheckWifiStatus(void);
void Indicator_Light(void);
void Turn_LED(void);

#endif

