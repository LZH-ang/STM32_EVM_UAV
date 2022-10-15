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

/* ʹ�ñ�׼�Ĺ̼������IO*/
#define MOTORstate(a)	if (a)	\
					GPIO_SetBits(MOTORGPIO,MOTOR);\
					else		\
					GPIO_ResetBits(MOTORGPIO,MOTOR)

/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		 {p->BRR=i;}	 //����͵�ƽ
#define digitalToggle(p,i) {p->ODR ^=i;} //�����ת״̬
#define MOTOR_TOGGLE	 digitalToggle(MOTORGPIO,MOTOR)
#define MOTOR_ON		   digitalHi(MOTORGPIO,MOTOR)
#define MOTOR_OFF			 digitalLo(MOTORGPIO,MOTOR)

void MOTOR_Init(void);
void delay_ms(uint16_t time);

#endif /*__bsp_motor_H*/
