#include "stm32f10x.h"   // �൱��51��Ƭ���е�  #include <reg51.h>
#include "bsp_motor.h"
#include "bsp_ws2812.h"

int main(void)
{

    // ���������ʱ��ϵͳ��ʱ���Ѿ������ó�72M��
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
