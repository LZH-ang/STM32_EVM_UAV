#ifndef __core_delay_H
#define __core_delay_H

#include "stm32f10x.h"

/*
**********************************************************************
*         时间戳相关寄存器定义
**********************************************************************
*/
/*
 在Cortex-M里面有一个外设叫DWT(Data Watchpoint and Trace)，
 该外设有一个32位的寄存器叫CYCCNT，它是一个向上的计数器，
 记录的是内核时钟运行的个数，最长能记录的时间为：
 60s=2的32次方/72000000
 (假设内核频率为72M，内核跳一次的时间大概为1/72M=13.8ns)
 当CYCCNT溢出之后，会清0重新开始向上计数。
 使能CYCCNT计数的操作步骤：
 1、先使能DWT外设，这个由另外内核调试寄存器DEMCR的位24控制，写1使能
 2、使能CYCCNT寄存器之前，先清0
 3、使能CYCCNT寄存器，这个由DWT_CTRL(代码上宏定义为DWT_CR)的位0控制，写1使能
 (参考Cortex-M3 Technical Reference Manual)
*/

#define	DWT_CR		*(__IO uint32_t *)0xE0001000
#define	DWT_CYCCNT  *(__IO uint32_t *)0xE0001004
#define	DEM_CR      *(__IO uint32_t *)0xE000EDFC

#define	DEM_CR_TRCENA				(1 << 24)
#define	DWT_CR_CYCCNTENA			(1 <<  0)

/* 获取内核时钟频率 */
#define	GET_CPU_ClkFreq()		(SystemCoreClock)
#define SysClockFreq            (SystemCoreClock)
/* 为方便使用，在延时函数内部调用CPU_TS_TmrInit函数初始化时间戳寄存器，
   这样每次调用函数都会初始化一遍。
   把本宏值设置为0，然后在main函数刚运行时调用CPU_TS_TmrInit可避免每次都初始化 */  

#define CPU_TS_INIT_IN_DELAY_FUNCTION   0

void CPU_TS_TmrInit(void);
//最大延时值为60秒
void CPU_TS_Tmr_Delay_US(uint32_t us);
#define CPU_TS_Tmr_Delay_MS(ms)		CPU_TS_Tmr_Delay_US(ms*1000)
#define CPU_TS_Tmr_Delay_S(s)       CPU_TS_Tmr_Delay_MS(s*1000)

#endif /*__core_delay_H*/
