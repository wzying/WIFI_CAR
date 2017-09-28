/*
	WIFI״ָ̬ʾ��
	LEDָʾ��
*/
#include "N76E003.h"
#include "Common.h"
#include "Function_define.h"
#include "LED.h"
/*
	LED IO��ʼ��
	WIFI״ָ̬ʾ��
	P1.7
	�ߵ�ƽ�������͵�ƽϨ��
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
		return 1;//��ʱ��WIFI�Ͽ�
	
	return 0;//δ��ʱ����ʾWIFI��������״̬
}

void SetWifiStatus(void)
{
	wifi_connect_timeout = WIFI_CONNECT_TIMEOUT;
}

/*
	LED��˸
*/
#define LED_FLASH_RATE	1000	//LED��˸����
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
	LEDָʾ��
*/
void Indicator_Light(void)
{
	//���WIFI����״̬
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
