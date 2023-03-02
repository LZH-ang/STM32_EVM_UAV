#ifndef __bsp_oled_H
#define __bsp_oled_H

#include "stm32f10x.h"
#include <stdlib.h>
#include <string.h>
#include "bsp_systick.h"
//#include "delay.h"
//DRIVER IC: SH1106

//-----------------OLED端口定义----------------

#define OLEDGPIO_RCC RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA
#define OLED_GPIOA GPIOA
#define OLED_GPIOB GPIOB
#define OLED_SCK  GPIO_Pin_13		
#define OLED_MOSI GPIO_Pin_15
#define OLED_NSS  GPIO_Pin_12
#define OLED_RES  GPIO_Pin_8
#define OLED_DC   GPIO_Pin_11

#define OLED_SCL_Clr() GPIO_ResetBits(OLED_GPIOB,OLED_SCK)//SCL
#define OLED_SCL_Set() GPIO_SetBits(OLED_GPIOB,OLED_SCK)

#define OLED_SDA_Clr() GPIO_ResetBits(OLED_GPIOB,OLED_MOSI)//SDA
#define OLED_SDA_Set() GPIO_SetBits(OLED_GPIOB,OLED_MOSI)

#define OLED_RES_Clr() GPIO_ResetBits(OLED_GPIOA,OLED_RES)//RES
#define OLED_RES_Set() GPIO_SetBits(OLED_GPIOA,OLED_RES)

#define OLED_DC_Clr()  GPIO_ResetBits(OLED_GPIOA,OLED_DC)//DC
#define OLED_DC_Set()  GPIO_SetBits(OLED_GPIOA,OLED_DC)

#define OLED_CS_Clr()  GPIO_ResetBits(OLED_GPIOB,OLED_NSS)//CS
#define OLED_CS_Set()  GPIO_SetBits(OLED_GPIOB,OLED_NSS)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ColorTurn(uint8_t mode);
void OLED_DisplayTurn(uint8_t mode);
void OLED_WR_Byte(uint8_t dat,uint8_t mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_Init(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
void OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1,uint8_t mode);
void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode);
void OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);


#endif /*__bsp_oled_H*/

