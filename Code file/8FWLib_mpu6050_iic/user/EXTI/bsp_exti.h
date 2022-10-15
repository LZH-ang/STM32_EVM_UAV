#ifndef __bsp_exti_H
#define __bsp_exti_H

#include "stm32f10x.h"

#define	MPU6050_GPIO_CLK		RCC_APB2Periph_GPIOB
#define	MPU6050_GPIO_PORT		GPIOB	   
#define	MPU6050_GPIO_PIN		GPIO_Pin_5

#define EXTI_CLK				RCC_APB2Periph_AFIO
#define MPU6050_LINE			EXTI_Line5			//中断线设置
#define MPU6050_IRQChannel		EXTI9_5_IRQn;		//中断通道设置
#define MPU6050_PortSource		GPIO_PortSourceGPIOB
#define MPU6050_PinSource		GPIO_PinSource5
#define MPU6050_IRQHandler		EXTI9_5_IRQHandler	//中断函数

void GPIO_INT_Init(void);

#endif /*__bsp_exti_H*/
