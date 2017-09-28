/*
	WIFI状态指示灯
	LED指示灯
*/
#include "N76E003.h"
#include "Common.h"
#include "Function_define.h"
#include "LED.h"
/*
	LED IO初始化
	WIFI状态指示灯
	P1.7
	高电平点亮，低电平熄灭
*/
void LED_Init(void)
{
	P17_PushPull_Mode;
	LED1_STATUS=1;
	
	P30_PushPull_Mode;
	LED2_STATUS=1;	
}

void Set_LED_WIFI(void)
{
	LED1_STATUS=0;
	LED2_STATUS=0;
}

void Reset_LED_WIFI(void)
{
	LED1_STATUS=1;
	LED2_STATUS=1;
}

void Turn_LED(void)
{
	if(LED1_STATUS)
		Set_LED_WIFI();
	else
		Reset_LED_WIFI();
}

u16 wifi_connect_timeout = 0;
u8 CheckWifiStatus(void)
{
	if(wifi_connect_timeout)
		wifi_connect_timeout--;
	else
		return 1;//超时，WIFI断开
	
	return 0;//未超时，表示WIFI处于连接状态
}

void SetWifiStatus(void)
{
	wifi_connect_timeout = WIFI_CONNECT_TIMEOUT;
}

/*
	LED闪烁
*/
#define LED_FLASH_RATE	1000	//LED闪烁速率
u16 led_Flash_Rate = LED_FLASH_RATE;
void LED_Flash(void)
{
	if(led_Flash_Rate)
		led_Flash_Rate--;
	else
	{
		led_Flash_Rate = LED_FLASH_RATE;
		Turn_LED();
	}
}

/*
	LED指示灯
*/
void Indicator_Light(void)
{
	//检测WIFI连接状态
	if(CheckWifiStatus())
	{
		//Reset_LED_WIFI();
		LED_Flash();
	}
	else
	{
		Set_LED_WIFI();
	}	
}
