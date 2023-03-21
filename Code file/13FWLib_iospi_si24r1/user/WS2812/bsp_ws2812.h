#ifndef __bsp_ws2812_H
#define __bsp_ws2812_H

#include "stm32f10x.h"

#define WS2812GPIO GPIOB
#define WS2812 GPIO_Pin_9
#define WS2812_Clock RCC_APB2Periph_GPIOB

/* 使用标准的固件库控制IO*/
#define WS2812OFF GPIO_WriteBit(WS2812GPIO,WS2812,Bit_RESET)
#define WS2812ON	GPIO_WriteBit(WS2812GPIO,WS2812,Bit_SET)
#define WS2812state(a)	if (a)	\
					GPIO_SetBits(WS2812GPIO,WS2812);\
					else		\
					GPIO_ResetBits(WS2812GPIO,WS2812)

/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		 {p->BRR=i;}	 //输出低电平
#define digitalToggle(p,i) {p->ODR ^=i;} //输出反转状态
#define WS2812_TOGGLE	 		digitalToggle(WS2812GPIO,WS2812)
#define WS2812_ON		   		digitalHi(WS2812GPIO,WS2812)
#define WS2812_OFF			 	digitalLo(WS2812GPIO,WS2812)

void WS2812_Init(void);
void Send_1(void);
void Send_0(void);
void Send_ws2812Byte(uint8_t ws2812_Rlight,uint8_t ws2812_Glight,uint8_t ws2812_Blight);
void WS2812RGB(uint32_t speed,uint8_t light);//speed>80 light:0x00~ff
void ws2812_reset(void);
void delay_us(uint32_t time);
void delay_1000ns(void);
void delay_850ns(void);
#define delay_400ns	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP()
#define delay_250ns	__NOP()

//Reset Code >80us
#define WS2812_DARK 	Send_ws2812Byte(0,0,0)
#define WS2812_WHITE 	Send_ws2812Byte(63,63,63)
#define WS2812_RED 		Send_ws2812Byte(63,0,0)
#define WS2812_GREEN 	Send_ws2812Byte(0,63,0)
#define WS2812_BLUE 	Send_ws2812Byte(0,0,63)
#define WS2812_YELLO 	Send_ws2812Byte(63,63,0)
#define WS2812_PURPLE Send_ws2812Byte(63,0,63)
#define WS2812_CYAN 	Send_ws2812Byte(0,63,63)

#endif /*__bsp_ws2812_H*/
