#include "bsp_motor.h"

void MOTOR_Init(void)
{
	GPIO_InitTypeDef GPIO_MOTOR;
	
	RCC_APB2PeriphClockCmd(MOTOR_Clock, ENABLE);//�������ʱ����"|"	
	GPIO_MOTOR.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_MOTOR.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_MOTOR.GPIO_Pin = MOTOR;
	GPIO_Init(MOTORGPIO, &GPIO_MOTOR);
	MOTOR_OFF;
}

void delay_ms(uint16_t time)
{    
   uint16_t i=0;  
   while(time--)
   {
      i=12000;  //�Լ�����
      while(i--);    
   }
}
