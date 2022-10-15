#include "bsp_exti.h"

static void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(EXTI_CLK, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);			//优先级分组设置
	/***************内核中断配置******************/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = MPU6050_IRQChannel;
	NVIC_Init(&NVIC_InitStructure);
}

void GPIO_INT_Init(void)
{
	GPIO_InitTypeDef GPIO_MPU6050;
	EXTI_InitTypeDef EXTI_MPU6050;
	NVIC_Config();
	
	RCC_APB2PeriphClockCmd(MPU6050_GPIO_CLK, ENABLE);
	GPIO_MPU6050.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_MPU6050.GPIO_Pin = MPU6050_GPIO_PIN;
	GPIO_Init(MPU6050_GPIO_PORT, &GPIO_MPU6050);
	GPIO_EXTILineConfig(MPU6050_PortSource, MPU6050_PinSource);	//中断线配置
	/***************中断模式配置******************/
	EXTI_MPU6050.EXTI_Line = MPU6050_LINE;
	EXTI_MPU6050.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_MPU6050.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_MPU6050.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_MPU6050);
}
