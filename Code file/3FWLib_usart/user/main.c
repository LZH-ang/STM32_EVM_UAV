#include "stm32f10x.h"   // 相当于51单片机中的  #include <reg51.h>
#include "bsp_motor.h"
#include "bsp_usart.h"
#include "bsp_ws2812.h"

static void Show_Message(void);

int main(void)
{
    // 来到这里的时候，系统的时钟已经被配置成72M。
    uint8_t array[] = {0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00};
    char ch=0;
    MOTOR_Init();
    USART_Config();
    WS2812_Init();

//	Usart_SendByte(DEBUG_USARTx, 0xcc);
//	Usart_SendHalfWord(DEBUG_USARTx, 0x8888);
//	Usart_SendArray(DEBUG_USARTx, array, 16);

//	Usart_SendString(DEBUG_USARTx," \nI'm Kelee.");
//	Usart_SendByte(DEBUG_USARTx, '\n');
//	Usart_SendString(DEBUG_USARTx,"Hello STM32!\n");

    printf("\n我是Kelee.你好STM32!\n");
    printf("\nHere is Kelee.Hello STM32!\n");

    delay_ms(300);
    MOTORstate(ON);
    delay_ms(300);
    MOTORstate(OFF);
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
