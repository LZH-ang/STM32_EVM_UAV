#include "bsp_systick.h"

volatile uint32_t TimingDelay=0;
volatile uint16_t KeyFIFOTime=0;

void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
//	if (SysTick_Config(SystemFrequency / 1000000))	// ST3.0.0��汾
	if(SysTick_Config(SystemCoreClock / 200))	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		printf("SysTick_Config_ERROR");
		while (1);
	}
}

void Delay_5ms(volatile uint32_t nTime)
{ 
	TimingDelay = nTime;	
//	// ʹ�ܵδ�ʱ��  
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
		// ����������ֵ��С��0��ʱ��CRTL�Ĵ�����λ16����1
		// ����1ʱ����ȡ��λ����0
		while( !((SysTick->CTRL)&(1<<16)) );
	}
//	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
	SysTick_Config(SystemCoreClock / 100);
}
