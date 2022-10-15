#include "bsp_systick.h"

volatile uint32_t TimingDelay=0;
volatile uint16_t KeyFIFOTime=0;

void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
//	if (SysTick_Config(SystemFrequency / 1000000))	// ST3.0.0库版本
	if(SysTick_Config(SystemCoreClock / 200))	// ST3.5.0库版本
	{ 
		/* Capture error */ 
		printf("SysTick_Config_ERROR");
		while (1);
	}
}

void Delay_5ms(volatile uint32_t nTime)
{ 
	TimingDelay = nTime;	
//	// 使能滴答定时器  
//	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
	while(TimingDelay != 0);
//	SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}

void TimingFIFO_KeyScan(void)			/*KEY_FIFO_SCAN*/
{
	if (KeyFIFOTime < KEY_FIFO_TIME)
	{ 
		KeyFIFOTime++;
	}
	else
	{
		KeyFIFOTime = 0;
		KEY_Scan();
	}
}

void SysTick_Delay_us(volatile uint32_t us)
{
	uint32_t i;
	SysTick_Config(SystemCoreClock/1000000);
//	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
	for(i=0;i<us;i++)
	{
		while( !((SysTick->CTRL)&(1<<16)) );
	}
//	SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
	SysTick_Config(SystemCoreClock / 100);
}

void SysTick_Delay_ms(volatile uint32_t ms)
{
	uint32_t i;	
	SysTick_Config(SystemCoreClock/1000);
//	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
	for(i=0;i<ms;i++)
	{
		// 当计数器的值减小到0的时候，CRTL寄存器的位16会置1
		// 当置1时，读取该位会清0
		while( !((SysTick->CTRL)&(1<<16)) );
	}
//	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
	SysTick_Config(SystemCoreClock / 100);
}
