#include "stm32f10x.h"   // 相当于51单片机中的  #include <reg51.h>
#include "bsp_motor.h"
#include "bsp_usart.h"
#include "bsp_ws2812.h"
#include "bsp_iic_eeprom.h"
#include "bsp_iic_io.h"

static void Show_Message(void);

int main(void)
{
    char ch=0;
    MOTOR_Init();
    USART_Config();
    WS2812_Init();
    i2c_GPIO_Config();

    printf("\n我是Kelee.你好STM32!\n");
    printf("\nHere is Kelee.Hello STM32!\n");

    delay_ms(300);
    MOTORstate(ON);
    delay_ms(300);
    MOTORstate(OFF);

    EEPROM_Erase();
    if(EEPROM_Test() == 1)
    {
        WS2812_GREEN;
    }
    else
    {
        WS2812_RED;
    }

    Show_Message();
    while(1)
    {
        ch=getchar();
        printf("\nReceive：%c\n",ch);
        switch(ch)
        {
        case '1':
            WS2812_RED;
            break;
        case '2':
            WS2812_GREEN;
            break;
        case '3':
            WS2812_BLUE;
            break;
        case '4':
            WS2812_YELLO;
            break;
        case '5':
            WS2812_PURPLE;
            break;
        case '6':
            WS2812_CYAN;
            break;
        case '7':
            WS2812_WHITE;
            break;
        case '8':
            WS2812RGB(100000,0x0f);
            break;
        case '9':
            WS2812_DARK;
            break;
        default :
            Show_Message();
            break;
        }
    }
}

static void Show_Message(void)
{
    printf("\nPLZ send:\n");
    printf("  USART：%d 8-N-1 \n",DEBUG_USART_BAUDRATE);
    printf("   CMD   ------    RGB   \n");
    printf("    1    ------    RED   \n");
    printf("    2    ------   GREEN  \n");
    printf("    3    ------    BLUE  \n");
    printf("    4    ------   YELLOW \n");
    printf("    5    ------   PURPLE \n");
    printf("    6    ------    CYAN  \n");
    printf("    7    ------   WHITE  \n");
    printf("    8    ------   RGBON  \n");
    printf("    9    ------   RGBOFF \n");
}
