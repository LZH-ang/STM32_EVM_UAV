#ifndef __bsp_key_H
#define __bsp_key_H

#include "stm32f10x.h"

#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOC
#define    KEY1_GPIO_PORT    GPIOC
#define    KEY1_GPIO_PIN		 GPIO_Pin_13

#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOA
#define    KEY2_GPIO_PORT    GPIOA
#define    KEY2_GPIO_PIN		 GPIO_Pin_0

#define    KEY3_GPIO_CLK     RCC_APB2Periph_GPIOA
#define    KEY3_GPIO_PORT    GPIOA
#define    KEY3_GPIO_PIN		 GPIO_Pin_1

#define    KEY_ON  1
#define    KEY_OFF 0

#define EXTI_CLK					RCC_APB2Periph_AFIO

#define KEY2_LINE					EXTI_Line0						//�ж�������
#define KEY2_IRQChannel		EXTI0_IRQn;						//�ж�ͨ������
#define KEY2_PortSource		GPIO_PortSourceGPIOA
#define KEY2_PinSource		GPIO_PinSource0
#define KEY2_IRQHandler		EXTI0_IRQHandler			//�жϺ���

#define KEY3_LINE					EXTI_Line1						//�ж�������
#define KEY3_IRQChannel		EXTI1_IRQn;						//�ж�ͨ������
#define KEY3_PortSource		GPIO_PortSourceGPIOA
#define KEY3_PinSource		GPIO_PinSource1
#define KEY3_IRQHandler		EXTI1_IRQHandler			//�жϺ���

void KEY_Init(void);
uint8_t KEY_IO(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif /*__bsp_key_H*/
