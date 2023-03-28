#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue;

static void ADCx_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 打开 ADC IO端口时钟
    ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE);
    // 配置 ADC IO 引脚模式 必须为模拟输入
    GPIO_InitStructure.GPIO_Pin = ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    // 初始化 ADC IO
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);
}

#if ADC_NVIC_ENABLE
static void ADC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // 优先级分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    // 配置中断优先级
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
	// 打开DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// 复位DMA控制器
    DMA_DeInit(ADC_DMA_CHANNEL);
	// 配置 DMA 初始化结构体
    // 外设基址为：ADC 数据寄存器地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC_x->DR));
	// 存储器地址，实际上就是一个内部SRAM的变量
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
	// 数据源来自外设
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 缓冲区大小为1，缓冲区的大小应该等于存储器的大小
    DMA_InitStructure.DMA_BufferSize = 1;
	// 外设寄存器只有一个，地址不用递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 存储器地址固定,ENABLE：ADC多通道采集
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	// 外设数据大小为半字，即两个字节
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	// 存储器数据大小也为半字，跟外设数据大小相同
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	// 循环传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	// 禁止存储器到存储器模式，因为是从外设到存储器
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// 初始化DMA
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	// 使能 DMA 通道
    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
}
#endif

static void ADCx_Mode_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    // 打开ADC时钟
    ADC_APBxClock_FUN(ADC_CLK, ENABLE);
    // ADC 模式配置
    // 只使用一个ADC，属于独立模式
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    // 禁止扫描模式，多通道才要，单通道不需要
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    // 连续转换模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    // 不用外部触发转换，软件开启即可
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    // 转换结果右对齐
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    // 转换通道1个
    ADC_InitStructure.ADC_NbrOfChannel = ADC_NBROFCHANNEL;
    // 初始化ADC
    ADC_Init(ADC_x, &ADC_InitStructure);
    // 配置ADC时钟为PCLK2的分频
    RCC_ADCCLKConfig(ADC_RCC);
    // 配置 ADC 通道转换顺序和采样时间
    ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL, 1, ADC_SAMPLETIME);
	#if ADC_NVIC_ENABLE
    // ADC 转换结束产生中断，在中断服务程序中读取转换值
    ADC_ITConfig(ADC_x, ADC_IT_EOC, ENABLE);
	#endif
	#if ADC_DMA_ENABLE
	// 使能ADC DMA 请求
    ADC_DMACmd(ADC_x, ENABLE);
	#endif
    // 开启ADC ，并开始转换
    ADC_Cmd(ADC_x, ENABLE);
    // 初始化ADC 校准寄存器
    ADC_ResetCalibration(ADC_x);
    // 等待校准寄存器初始化完成
    while(ADC_GetResetCalibrationStatus(ADC_x));
    // ADC开始校准
    ADC_StartCalibration(ADC_x);
    // 等待校准完成
    while(ADC_GetCalibrationStatus(ADC_x));
    // 由于没有采用外部触发，所以使用软件触发ADC转换
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
