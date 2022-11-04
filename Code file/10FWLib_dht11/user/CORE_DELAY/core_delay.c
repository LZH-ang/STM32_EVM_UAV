#include "core_delay.h"

void CPU_TS_TmrInit(void)
{
#if CPU_TS_INIT_IN_DELAY_FUNCTION
#else  
    DEM_CR |= (uint32_t)DEM_CR_TRCENA;	/* 使能DWT外设 */                
    DWT_CYCCNT = (uint32_t)0u;		/* DWT CYCCNT寄存器计数清0 */
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;/* 使能Cortex-M DWT CYCCNT寄存器 */
#endif
}

uint32_t CPU_TS_TmrRd(void)
{        
	return ((uint32_t)DWT_CYCCNT);
}

/**
  * @brief  采用CPU的内部计数实现精确延时，32位计数器
  * @param  us : 延迟长度，单位1 us
  * @retval 无
  * @note   使用本函数前必须先调用CPU_TS_TmrInit函数使能计数器，
            或使能宏CPU_TS_INIT_IN_DELAY_FUNCTION
            最大延时值为8秒，即8*1000*1000
  */
void CPU_TS_Tmr_Delay_US(uint32_t us)
{
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;

#if	CPU_TS_INIT_IN_DELAY_FUNCTION	/* 在函数内部初始化时间戳寄存器 */   
	CPU_TS_TmrInit();	/* 初始化时间戳并清零 */
#endif
	ticks = us * (GET_CPU_ClkFreq()/1000000);	/* 需要的节拍数 */      
	tcnt = 0;
	told = (uint32_t)CPU_TS_TmrRd();	/* 刚进入时的计数器值 */
	while(1)
	{
		tnow = (uint32_t)CPU_TS_TmrRd();  
		if(tnow != told)
		{   
			if(tnow > told)	/* 32位计数器是递增计数器 */ 
			{
				tcnt += tnow - told;  
			}
			else	/* 重新装载 */
			{
				tcnt += UINT32_MAX - told + tnow; 
			} 
			told = tnow;
			/*时间超过/等于要延迟的时间,则退出 */
			if(tcnt >= ticks)break;
		}  
	}
}


