#include "stm32f10x.h"
#include <stdbool.h>
#include "bsp_motor.h"
#include "bsp_usart.h"
#include "bsp_ws2812.h"
#include "bsp_systick.h"
#include "bsp_key.h"
#include "bsp_eeprom_iic.h"
#include "bsp_oled.h"
#include "bmp.h"

int main(void)
{
	uint8_t oledt = 0;
	static uint8_t EEPROM_t[1048] = {0};
	uint8_t keycode;
	bool KEY1 = 1;
    MOTOR_Init();
    USART_Config();
	KEY_Init();
	SysTick_Init();
    WS2812_Init();
	EEPROM_I2C_Config();

    printf("\nŒ“ «Kelee.ƒ„∫√STM32!\n");
    printf("\nHere is Kelee.Hello STM32!\n");
	oledt = I2C_EEPROM_ByteR(0x02f);
	if(oledt != 0xC3)
	{
		printf("\nEEPROM_t = 0x%x\n",oledt);
		I2C_EEPROM_BufferW(0x000, Github, 0x3ff);
	}	
	EEPROM_ACK_POLL();
	I2C_EEPROM_BufferR(0x000, EEPROM_t, 0x3ff);

	OLED_Init();
    OLED_ColorTurn(0);
    OLED_DisplayTurn(0);
	OLED_ShowPicture(0,0,128,64,EEPROM_t,0);
	OLED_Refresh();

    while(1)
    {
		keycode = KEY_FIFO_Get();
		if(keycode != KEY_NONE)
		{
			switch(keycode)
			{
				case KEY_1_DOWN:printf("\nKEY_1_DOWN\n");break;
				case KEY_1_UP:	printf("\nKEY_1_UP\n");
								KEY1 = !KEY1;
								if(KEY1)
									OLED_DisPlay_On();
								else
									OLED_DisPlay_Off();break;
				case KEY_1_LONG:printf("\nKEY_1_LONG\n");break;
				case KEY_2_DOWN:printf("\nKEY_2_DOWN\n");break;
				case KEY_2_UP:	printf("\nKEY_2_UP\n");break;
				case KEY_2_LONG:printf("\nKEY_2_LONG\n");break;
				case KEY_3_DOWN:printf("\nKEY_3_DOWN\n");break;
				case KEY_3_UP:	printf("\nKEY_3_UP\n");break;
				case KEY_3_LONG:printf("\nKEY_3_LONG\n");break;
				default:;
			}
		}	
	}
}


