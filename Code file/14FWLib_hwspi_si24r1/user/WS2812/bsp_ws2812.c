#include "bsp_ws2812.h"

void WS2812_Init(void)
{
	GPIO_InitTypeDef GPIO_WS2812;
	
	RCC_APB2PeriphClockCmd(WS2812_Clock, ENABLE);
	GPIO_WS2812.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_WS2812.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_WS2812.GPIO_Pin = WS2812;
	GPIO_Init(WS2812GPIO, &GPIO_WS2812);
	ws2812_reset();
	WS2812_OFF;
}

void Send_ws2812Byte(uint8_t ws2812_Rlight,uint8_t ws2812_Glight,uint8_t ws2812_Blight)
{
//Green
	if((ws2812_Glight&0x80) == 0)Send_0();else Send_1();
	if((ws2812_Glight&0x40) == 0)Send_0();else Send_1();
	if((ws2812_Glight&0x20) == 0)Send_0();else Send_1();
	if((ws2812_Glight&0x10) == 0)Send_0();else Send_1();
	if((ws2812_Glight&0x08) == 0)Send_0();else Send_1();
	if((ws2812_Glight&0x04) == 0)Send_0();else Send_1();
	if((ws2812_Glight&0x02) == 0)Send_0();else Send_1();
	if((ws2812_Glight&0x01) == 0)Send_0();else Send_1();
//Red
	if((ws2812_Rlight&0x80) == 0)Send_0();else Send_1();
	if((ws2812_Rlight&0x40) == 0)Send_0();else Send_1();
	if((ws2812_Rlight&0x20) == 0)Send_0();else Send_1();
	if((ws2812_Rlight&0x10) == 0)Send_0();else Send_1();
	if((ws2812_Rlight&0x08) == 0)Send_0();else Send_1();
	if((ws2812_Rlight&0x04) == 0)Send_0();else Send_1();
	if((ws2812_Rlight&0x02) == 0)Send_0();else Send_1();
	if((ws2812_Rlight&0x01) == 0)Send_0();else Send_1();
//Blue
	if((ws2812_Blight&0x80) == 0)Send_0();else Send_1();
	if((ws2812_Blight&0x40) == 0)Send_0();else Send_1();
	if((ws2812_Blight&0x20) == 0)Send_0();else Send_1();
	if((ws2812_Blight&0x10) == 0)Send_0();else Send_1();
	if((ws2812_Blight&0x08) == 0)Send_0();else Send_1();
	if((ws2812_Blight&0x04) == 0)Send_0();else Send_1();
	if((ws2812_Blight&0x02) == 0)Send_0();else Send_1();
	if((ws2812_Blight&0x01) == 0)Send_0();else Send_1();
}

int16_t R=0x00,G=0x00,B=0xff;//0000ff
void WS2812RGB(uint32_t speed,uint8_t light)//speed>80 light:0x00~ff
{
	B=light;
	while(G<=light){Send_ws2812Byte(R,G,B);delay_us(speed);G++;}
	G=light;//00ffff
	while(B>=0){Send_ws2812Byte(R,G,B);delay_us(speed);B--;}
	B=0x00;//00ff00
	while(R<=light){Send_ws2812Byte(R,G,B);delay_us(speed);R++;}
	R=light;//ffff00
	while(G>=0){Send_ws2812Byte(R,G,B);delay_us(speed);G--;}
	G=0x00;//ff0000
	while(B<=light){Send_ws2812Byte(R,G,B);delay_us(speed);B++;}
	B=light;//ff00ff
	while(R>=0){Send_ws2812Byte(R,G,B);delay_us(speed);R--;}
	R=0x00;//0000ff
}
//使用软件延时方法
void Send_1(void)
{
	WS2812state(1);delay_850ns();
	WS2812state(0);delay_400ns;
}
void Send_0(void)
{
	WS2812state(1);delay_250ns;
	WS2812state(0);delay_1000ns();
}

void ws2812_reset(void)
{
	Send_ws2812Byte(0,0,0);
	Send_ws2812Byte(0,0,0);
	Send_ws2812Byte(0,0,0);
	Send_ws2812Byte(0,0,0);
	delay_us(100);
}

//#define delay_250ns	__NOP()
//#define delay_400ns	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP()
void delay_1000ns(void)
{
	uint8_t del_t=3;
	while(del_t--)
		__NOP();
}
void delay_850ns(void)
{
	uint8_t del_t=2;
	while(del_t--)
		__NOP();
}

void delay_us(uint32_t time)
{    
   uint8_t i=0;  
   while(time--)
   {
      i=5;
      while(i--);    
   }
}
