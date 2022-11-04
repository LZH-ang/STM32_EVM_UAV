#ifndef __bsp_dht11_H
#define __bsp_dht11_H

#include "stm32f10x.h"
#include "bsp_usart.h"

#define DHT11T_FLAG_TIMEOUT			((uint32_t)0x1000)
/*信息输出*/
#define DHT11_DEBUG_ON         		0
#define DHT11_INFO(fmt,arg...)		printf("<<-DHT11-INFO->> "fmt"\n",##arg)
#define DHT11_ERROR(fmt,arg...)		printf("<<-DHT11-ERROR->> "fmt"\n",##arg)
#define DHT11_DEBUG(fmt,arg...)		do{\
										if(DHT11_DEBUG_ON)\
										printf("<<-DHT11-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
										}while(0)

/************************** DHT11 数据类型定义********************************/
typedef struct
{
    uint8_t  humi_int;		/*湿度的整数部分*/
    uint8_t  humi_deci;	 	/*湿度的小数部分*/
    uint8_t  temp_int;	 	/*温度的整数部分*/
    uint8_t  temp_deci;	 	/*温度的小数部分*/
    uint8_t  check_sum;	 	/*校验和*/
} DHT11_Data_TypeDef;

/************************** DHT11 连接引脚定义********************************/
#define	DHT11_Dout_DATA_APBxClock_FUN			RCC_APB2PeriphClockCmd
#define	DHT11_Dout_GPIO_CLK               		RCC_APB2Periph_GPIOA
#define	DHT11_Dout_GPIO_PORT              		GPIOA
#define	DHT11_Dout_GPIO_PIN              		GPIO_Pin_12

#define	DHT11_Dout_0		GPIO_ResetBits(DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN)
#define	DHT11_Dout_1		GPIO_SetBits( DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN)
#define	DHT11_Dout_IN()		GPIO_ReadInputDataBit(DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN)

#define DHT11_DELAY_US(us)	CPU_TS_Tmr_Delay_US(us)
#define DHT11_DELAY_MS(ms)	CPU_TS_Tmr_Delay_MS(ms)

/************************** DHT11 函数声明 ********************************/
void DHT11_Init(void);
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data);

#endif /*__bsp_dht11_H*/
