#include "stm32f10x.h"   // 相当于51单片机中的  #include <reg51.h>
#include "bsp_motor.h"

int main(void)
{
    // 来到这里的时候，系统的时钟已经被配置成72M。
    MOTOR_Init();
    while(1)
    {
        MOTORstate(ON);
        delay_ms(300);
        MOTORstate(OFF);
        delay_ms(300);
    }
}
