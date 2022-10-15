#include "stm32f10x.h"
#include "bsp_motor.h"
#include "bsp_ws2812.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_systick.h"

int main(void)
{
    uint8_t led=0;
    uint8_t ws2812rgb=0;
    uint8_t keycode;
    // 来到这里的时候，系统的时钟已经被配置成72M。
    MOTOR_Init();
    WS2812_Init();
    USART_Config();
    KEY_Init();
    SysTick_Init();

    MOTOR_ON;
    Delay_5ms(2);
    MOTOR_OFF;

    printf("\nK1:控制WS2812\n");
    printf("\nK2:控制MOTOR\n");
    printf("\nK3:控制串口发送数据\n");
    KEY_SetParam(KID_K3, KEY_LONG_TIME, KEY_REPEAT_TIME);

    while(1)
    {
        keycode = KEY_FIFO_Get();
        if(keycode != KEY_NONE)
        {
            switch(keycode)
            {
            case KEY_1_DOWN:
                printf("\nKEY_1_DOWN\n");
                break;
            case KEY_1_UP:
                printf("\nKEY_1_UP\n");
                break;
            case KEY_1_LONG:
                printf("\nKEY_1_LONG\n");
                ws2812rgb=1;
                break;
            case KEY_2_DOWN:
                printf("\nKEY_2_DOWN\n");
                break;
            case KEY_2_UP:
                printf("\nKEY_2_UP\n");
                break;
            case KEY_2_LONG:
                printf("\nKEY_2_LONG\n");
                break;
            case KEY_3_DOWN:
                printf("\nKEY_3_DOWN\n");
                break;
            case KEY_3_UP:
                printf("\nKEY_3_UP\n");
                break;
            case KEY_3_LONG:
                printf("\nKEY_3_LONG\n");
                break;
            default:
                ;
            }
        }
        if(!ws2812rgb && keycode == KEY_1_UP)
        {
            led++;
            if(led > 6)led = 1;
            switch(led)
            {
            case 1:
                WS2812_RED;
                break;
            case 2:
                WS2812_GREEN;
                break;
            case 3:
                WS2812_BLUE;
                break;
            case 4:
                WS2812_YELLO;
                break;
            case 5:
                WS2812_PURPLE;
                break;
            case 6:
                WS2812_CYAN;
                break;
            default :
                WS2812_DARK;
            }
        }
        else if(ws2812rgb == 1)
        {
            WS2812RGB(100000,0x0f);
            ws2812rgb = 0;
        }
        else if(keycode == KEY_2_UP)
        {
            MOTOR_TOGGLE;
        }
        else if(keycode == KEY_3_UP)
        {
            printf("\n我是Kelee.你好STM32!\n");
            printf("\nHere is Kelee.Hello STM32!\n");
        }
    }
}
