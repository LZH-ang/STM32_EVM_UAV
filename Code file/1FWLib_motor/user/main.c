#include "stm32f10x.h"   // �൱��51��Ƭ���е�  #include <reg51.h>
#include "bsp_motor.h"

int main(void)
{
    // ���������ʱ��ϵͳ��ʱ���Ѿ������ó�72M��
    MOTOR_Init();
    while(1)
    {
        MOTORstate(ON);
        delay_ms(300);
        MOTORstate(OFF);
        delay_ms(300);
    }
}
