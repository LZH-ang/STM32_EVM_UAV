#ifndef __bsp_dht11_H
#define __bsp_dht11_H

#include "stm32f10x.h"
#include "bsp_usart.h"

#define DHT11T_FLAG_TIMEOUT			((uint32_t)0x1000)
/*��Ϣ���*/
#define DHT11_DEBUG_ON         		0
#define DHT11_INFO(fmt,arg...)		printf("<<-DHT11-INFO->> "fmt"\n",##arg)
#define DHT11_ERROR(fmt,arg...)		printf("<<-DHT11-ERROR->> "fmt"\n",##arg)
#define DHT11_DEBUG(fmt,arg...)		do{\
										if(DHT11_DEBUG_ON)\
										printf("<<-DHT11-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
										}while(0)

/************************** DHT11 �������Ͷ���********************************/
typedef struct
{
    uint8_t  humi_int;		/*ʪ�ȵ���������*/
    uint8_t  humi_deci;	 	/*ʪ�ȵ�С������*/
    uint8_t  temp_int;	 	/*�¶ȵ���������*/
    uint8_t  temp_deci;	 	/*�¶ȵ�С������*/
    uint8_t  check_sum;	 	/*У���*/
} DHT11_Data_TypeDef;

/************************** DHT11 �������Ŷ���********************************/
#define	DHT11_Dout_DATA_APBxClock_FUN			RCC_APB2PeriphClockCmd
#define	DHT11_Dout_GPIO_CLK               		RCC_APB2Periph_GPIOA
#define	DHT11_Dout_GPIO_PORT              		GPIOA
#define	DHT11_Dout_GPIO_PIN              		GPIO_Pin_12

#define	DHT11_Dout_0		GPIO_ResetBits(DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN)
#define	DHT11_Dout_1		GPIO_SetBits( DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN)
#define	DHT11_Dout_IN()		GPIO_ReadInputDataBit(DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN)

#define DHT11_DELAY_US(us)	CPU_TS_Tmr_Delay_US(us)
#define DHT11_DELAY_MS(ms)	CPU_TS_Tmr_Delay_MS(ms)

/************************** DHT11 �������� ********************************/
void DHT11_Init(void);
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data);

#endif /*__bsp_dht11_H*/
