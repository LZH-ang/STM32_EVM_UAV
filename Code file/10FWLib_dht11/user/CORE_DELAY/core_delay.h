#ifndef __core_delay_H
#define __core_delay_H

#include "stm32f10x.h"

/*
**********************************************************************
*         ʱ�����ؼĴ�������
**********************************************************************
*/
/*
 ��Cortex-M������һ�������DWT(Data Watchpoint and Trace)��
 ��������һ��32λ�ļĴ�����CYCCNT������һ�����ϵļ�������
 ��¼�����ں�ʱ�����еĸ�������ܼ�¼��ʱ��Ϊ��
 60s=2��32�η�/72000000
 (�����ں�Ƶ��Ϊ72M���ں���һ�ε�ʱ����Ϊ1/72M=13.8ns)
 ��CYCCNT���֮�󣬻���0���¿�ʼ���ϼ�����
 ʹ��CYCCNT�����Ĳ������裺
 1����ʹ��DWT���裬����������ں˵��ԼĴ���DEMCR��λ24���ƣ�д1ʹ��
 2��ʹ��CYCCNT�Ĵ���֮ǰ������0
 3��ʹ��CYCCNT�Ĵ����������DWT_CTRL(�����Ϻ궨��ΪDWT_CR)��λ0���ƣ�д1ʹ��
 (�ο�Cortex-M3 Technical Reference Manual)
*/

#define	DWT_CR		*(__IO uint32_t *)0xE0001000
#define	DWT_CYCCNT  *(__IO uint32_t *)0xE0001004
#define	DEM_CR      *(__IO uint32_t *)0xE000EDFC

#define	DEM_CR_TRCENA				(1 << 24)
#define	DWT_CR_CYCCNTENA			(1 <<  0)

/* ��ȡ�ں�ʱ��Ƶ�� */
#define	GET_CPU_ClkFreq()		(SystemCoreClock)
#define SysClockFreq            (SystemCoreClock)
/* Ϊ����ʹ�ã�����ʱ�����ڲ�����CPU_TS_TmrInit������ʼ��ʱ����Ĵ�����
   ����ÿ�ε��ú��������ʼ��һ�顣
   �ѱ���ֵ����Ϊ0��Ȼ����main����������ʱ����CPU_TS_TmrInit�ɱ���ÿ�ζ���ʼ�� */  

#define CPU_TS_INIT_IN_DELAY_FUNCTION   0

void CPU_TS_TmrInit(void);
//�����ʱֵΪ60��
void CPU_TS_Tmr_Delay_US(uint32_t us);
#define CPU_TS_Tmr_Delay_MS(ms)		CPU_TS_Tmr_Delay_US(ms*1000)
#define CPU_TS_Tmr_Delay_S(s)       CPU_TS_Tmr_Delay_MS(s*1000)

#endif /*__core_delay_H*/
