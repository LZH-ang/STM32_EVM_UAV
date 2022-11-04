#include "core_delay.h"

void CPU_TS_TmrInit(void)
{
#if CPU_TS_INIT_IN_DELAY_FUNCTION
#else  
    DEM_CR |= (uint32_t)DEM_CR_TRCENA;	/* ʹ��DWT���� */                
    DWT_CYCCNT = (uint32_t)0u;		/* DWT CYCCNT�Ĵ���������0 */
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;/* ʹ��Cortex-M DWT CYCCNT�Ĵ��� */
#endif
}

uint32_t CPU_TS_TmrRd(void)
{        
	return ((uint32_t)DWT_CYCCNT);
}

/**
  * @brief  ����CPU���ڲ�����ʵ�־�ȷ��ʱ��32λ������
  * @param  us : �ӳٳ��ȣ���λ1 us
  * @retval ��
  * @note   ʹ�ñ�����ǰ�����ȵ���CPU_TS_TmrInit����ʹ�ܼ�������
            ��ʹ�ܺ�CPU_TS_INIT_IN_DELAY_FUNCTION
            �����ʱֵΪ8�룬��8*1000*1000
  */
void CPU_TS_Tmr_Delay_US(uint32_t us)
{
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;

#if	CPU_TS_INIT_IN_DELAY_FUNCTION	/* �ں����ڲ���ʼ��ʱ����Ĵ��� */   
	CPU_TS_TmrInit();	/* ��ʼ��ʱ��������� */
#endif
	ticks = us * (GET_CPU_ClkFreq()/1000000);	/* ��Ҫ�Ľ����� */      
	tcnt = 0;
	told = (uint32_t)CPU_TS_TmrRd();	/* �ս���ʱ�ļ�����ֵ */
	while(1)
	{
		tnow = (uint32_t)CPU_TS_TmrRd();  
		if(tnow != told)
		{   
			if(tnow > told)	/* 32λ�������ǵ��������� */ 
			{
				tcnt += tnow - told;  
			}
			else	/* ����װ�� */
			{
				tcnt += UINT32_MAX - told + tnow; 
			} 
			told = tnow;
			/*ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
			if(tcnt >= ticks)break;
		}  
	}
}


