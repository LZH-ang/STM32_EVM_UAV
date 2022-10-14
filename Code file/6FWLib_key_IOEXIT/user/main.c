#include "stm32f10x.h"   // 相当于51单片机中的  #include <reg51.h>
#include "bsp_motor.h"
#include "bsp_ws2812.h"
#include "bsp_usart.h"
#include "bsp_key.h"

int main(void)
{
	uint8_t led=0;
	// 来到这里的时候，系统的时钟已经被配置成72M。
	MOTOR_Init();
	WS2812_Init();
	USART_Config();
	KEY_Init();
	
	printf("\nK1:控制WS2812\n");
	printf("\nK2:控制MOTOR\n");
	printf("\nK3:控制串口发送数据\n");
	
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
