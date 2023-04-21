#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue;

static void ADCx_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // �� ADC IO�˿�ʱ��
    ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE);
    // ���� ADC IO ����ģʽ ����Ϊģ������
    GPIO_InitStructure.GPIO_Pin = ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    // ��ʼ�� ADC IO
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);
}

#if ADC_NVIC_ENABLE
static void ADC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // ���ȼ�����
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    // �����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
#endif

#if ADC_DMA_ENABLE
static void ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	// ��DMAʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// ��λDMA������
    DMA_DeInit(ADC_DMA_CHANNEL);
	// ���� DMA ��ʼ���ṹ��
    // �����ַΪ��ADC ���ݼĴ�����ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC_x->DR));
	// �洢����ַ��ʵ���Ͼ���һ���ڲ�SRAM�ı���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
	// ����Դ��������
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// ��������СΪ1���������Ĵ�СӦ�õ��ڴ洢���Ĵ�С
    DMA_InitStructure.DMA_BufferSize = 1;
	// ����Ĵ���ֻ��һ������ַ���õ���
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// �洢����ַ�̶�,ENABLE��ADC��ͨ���ɼ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	// �������ݴ�СΪ���֣��������ֽ�
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	// �洢�����ݴ�СҲΪ���֣����������ݴ�С��ͬ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	// ѭ������ģʽ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// ��ʼ��DMA
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	// ʹ�� DMA ͨ��
    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
}
#endif

static void ADCx_Mode_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    // ��ADCʱ��
    ADC_APBxClock_FUN(ADC_CLK, ENABLE);
    // ADC ģʽ����
    // ֻʹ��һ��ADC�����ڶ���ģʽ
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    // ��ֹɨ��ģʽ����ͨ����Ҫ����ͨ������Ҫ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    // ����ת��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    // �����ⲿ����ת����������������
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    // ת������Ҷ���
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    // ת��ͨ��1��
    ADC_InitStructure.ADC_NbrOfChannel = ADC_NBROFCHANNEL;
    // ��ʼ��ADC
    ADC_Init(ADC_x, &ADC_InitStructure);
    // ����ADCʱ��ΪPCLK2�ķ�Ƶ
    RCC_ADCCLKConfig(ADC_RCC);
    // ���� ADC ͨ��ת��˳��Ͳ���ʱ��
    ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL, 1, ADC_SAMPLETIME);
	#if ADC_NVIC_ENABLE
    // ADC ת�����������жϣ����жϷ�������ж�ȡת��ֵ
    ADC_ITConfig(ADC_x, ADC_IT_EOC, ENABLE);
	#endif
	#if ADC_DMA_ENABLE
	// ʹ��ADC DMA ����
    ADC_DMACmd(ADC_x, ENABLE);
	#endif
    // ����ADC ������ʼת��
    ADC_Cmd(ADC_x, ENABLE);
    // ��ʼ��ADC У׼�Ĵ���
    ADC_ResetCalibration(ADC_x);
    // �ȴ�У׼�Ĵ�����ʼ�����
    while(ADC_GetResetCalibrationStatus(ADC_x));
    // ADC��ʼУ׼
    ADC_StartCalibration(ADC_x);
    // �ȴ�У׼���
    while(ADC_GetCalibrationStatus(ADC_x));
    // ����û�в����ⲿ����������ʹ����������ADCת��
    ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}

void ADCx_Init(void)
{
    ADCx_GPIO_Config();
    ADCx_Mode_Config();
	#if ADC_NVIC_ENABLE
    ADC_NVIC_Config();
	#endif
	#if ADC_DMA_ENABLE
	ADC_DMA_Config();
	#endif
}