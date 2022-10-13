#include "stm32f10x.h"   // 相当于51单片机中的  #include <reg51.h>
#include "bsp_motor.h"
#include "bsp_ws2812.h"

int main(void)
{

    // 来到这里的时候，系统的时钟已经被配置成72M。
    WS2812_Init();

    WS2812_CYAN;
    WS2812_GREEN;
    WS2812_YELLO;
    WS2812_RED;
    delay_us(200);

    while(1)
    {
        delay_us(20000);
        WS2812RGB(50000,0x1f);
    }
}
