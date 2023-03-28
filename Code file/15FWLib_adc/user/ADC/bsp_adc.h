
#ifndef __bsp_adc_H
#define __bsp_adc_H

#include "stm32f10x.h"

#define 	ADC_NVIC_ENABLE				0
#define 	ADC_DMA_ENABLE				1

// ADC 编号选择
// 可以是 ADC1/2，如果使用ADC3，中断相关的要改成ADC3的
#define		ADC_APBxClock_FUN        	RCC_APB2PeriphClockCmd
#define		ADC_x                    	ADC1
#define		ADC_CLK                  	RCC_APB2Periph_ADC1
// ADC GPIO宏定义
// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
#define		ADC_GPIO_APBxClock_FUN    	RCC_APB2PeriphClockCmd
#define		ADC_GPIO_CLK             	RCC_APB2Periph_GPIOA
#define		ADC_PORT                   	GPIOA
#define		ADC_PIN                   	GPIO_Pin_2
// ADC 通道宏定义
#define		ADC_CHANNEL               	ADC_Channel_2
#define    	ADC_DMA_CHANNEL           	DMA1_Channel2
#define		ADC_NBROFCHANNEL			1
#define		ADC_SAMPLETIME				ADC_SampleTime_28Cycles5
#define 	ADC_RCC						RCC_PCLK2_Div6
// ADC 中断相关宏定义
#define		ADC_IRQ                    	ADC1_2_IRQn
#define		ADC_IRQHandler             	ADC1_2_IRQHandler

void ADCx_Init(void);

#endif /*__bsp_adc_H*/
