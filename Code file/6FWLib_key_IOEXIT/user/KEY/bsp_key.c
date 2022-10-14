#include "bsp_key.h"

static void NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(EXTI_CLK, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				//优先级分组设置
    /***************内核中断配置******************/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannel = KEY2_IRQChannel;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = KEY3_IRQChannel;
    NVIC_Init(&NVIC_InitStructure);
}

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_KEY;
    EXTI_InitTypeDef EXTI_KEY;
    NVIC_Config();

    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK|KEY3_GPIO_CLK, ENABLE);
    GPIO_KEY.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_KEY.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_KEY);

    GPIO_KEY.GPIO_Pin = KEY2_GPIO_PIN;
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_KEY);
    GPIO_EXTILineConfig(KEY2_PortSource, KEY2_PinSource);		//中断线配置
    /***************中断模式配置******************/
    EXTI_KEY.EXTI_Line = KEY2_LINE;
    EXTI_KEY.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_KEY.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_KEY.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_KEY);

    GPIO_KEY.GPIO_Pin = KEY3_GPIO_PIN;
    GPIO_Init(KEY3_GPIO_PORT, &GPIO_KEY);
    GPIO_EXTILineConfig(KEY3_PortSource, KEY3_PinSource);		//中断线配置
    /***************中断模式配置******************/
    EXTI_KEY.EXTI_Line = KEY3_LINE;
    EXTI_KEY.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_KEY.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_KEY.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_KEY);
}

uint8_t KEY_IO(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_OFF)
    {
        while(!GPIO_ReadInputDataBit(GPIOx,GPIO_Pin));
        return KEY_OFF;
    }
    else return KEY_ON;
}
