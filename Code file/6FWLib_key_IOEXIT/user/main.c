#include "stm32f10x.h"   // �൱��51��Ƭ���е�  #include <reg51.h>
#include "bsp_motor.h"
#include "bsp_ws2812.h"
#include "bsp_usart.h"
#include "bsp_key.h"

int main(void)
{
	uint8_t led=0;
	// ���������ʱ��ϵͳ��ʱ���Ѿ������ó�72M��
	MOTOR_Init();
	WS2812_Init();
	USART_Config();
	KEY_Init();
	
	printf("\nK1:����WS2812\n");
	printf("\nK2:����MOTOR\n");
	printf("\nK3:���ƴ��ڷ�������\n");
	
	while(1)
	{
		if(KEY_IO(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_OFF)
		{
			led++;if(led > 6)led = 1;
			switch(led)
			{
				case 1:WS2812_RED;break;
				case 2:WS2812_GREEN;break;
				case 3:WS2812_BLUE;break;
				case 4:WS2812_YELLO;break;
				case 5:WS2812_PURPLE;break;
				case 6:WS2812_CYAN;break;
				default :WS2812_DARK;
			}
		}
	}
}
