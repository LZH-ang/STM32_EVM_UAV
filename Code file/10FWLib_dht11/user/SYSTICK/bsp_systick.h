#ifndef __bsp_systick_H
#define __bsp_systick_H

#include "stm32f10x.h"

void SysTick_Init(void);
void Delay_5ms(volatile uint32_t nTime);

void SysTick_Delay_us(volatile uint32_t us);
void SysTick_Delay_ms(volatile uint32_t ms);

#endif /*__bsp_systick_H*/
