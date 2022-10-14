#ifndef __bsp_motor_H
#define __bsp_motor_H

#include "stm32f10x.h"

#define ON  1
#define OFF 0
#define MOTORGPIO GPIOB
#define MOTOR GPIO_Pin_8
#define MOTOR_Clock RCC_APB2Periph_GPIOB

#define MOTOROFF GPIO_WriteBit(MOTORGPIO,MOTOR,Bit_RESET)
#define MOTORON	GPIO_WriteBit(MOTORGPIO,MOTOR,Bit_SET)

/* 使用标准的固件库控制IO*/
#define MOTORstate(a)	if (a)	\
					GPIO_SetBits(MOTORGPIO,MOTOR);\
					else		\
					GPIO_ResetBits(MOTORGPIO,MOTOR)

/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		 {p->BRR=i;}	 //输出低电平
#define digitalToggle(p,i) {p->ODR ^=i;} //输出反转状态
#define MOTOR_TOGGLE	 digitalToggle(MOTORGPIO,MOTOR)
#define MOTOR_ON		   digitalHi(MOTORGPIO,MOTOR)
#define MOTOR_OFF			 digitalLo(MOTORGPIO,MOTOR)

void MOTOR_Init(void);
void delay_ms(uint16_t time);

#endif /*__bsp_motor_H*/
