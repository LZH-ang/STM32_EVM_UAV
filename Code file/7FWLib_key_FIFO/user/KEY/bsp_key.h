#ifndef __bsp_key_H
#define __bsp_key_H

#include "stm32f10x.h"

#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOC
#define    KEY1_GPIO_PORT    GPIOC	   
#define    KEY1_GPIO_PIN		 GPIO_Pin_13

#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOA
#define    KEY2_GPIO_PORT    GPIOA	   
#define    KEY2_GPIO_PIN		 GPIO_Pin_0

#define    KEY3_GPIO_CLK     RCC_APB2Periph_GPIOA
#define    KEY3_GPIO_PORT    GPIOA	   
#define    KEY3_GPIO_PIN		 GPIO_Pin_1

#define    KEY_ON  1 
#define    KEY_OFF 0 

void KEY_Init(void);
void KEY_Scan(void);
uint8_t KEY_IO(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* 按键ID, 主要用于KEY_GetState()函数的入口参数 */
typedef enum
{
	KID_K1 = 0,
	KID_K2,
	KID_K3,
}KEY_ID_E;

/*
	按键滤波时间50ms, 单位5ms。
	只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
	即使按键电路不做硬件滤波，该滤波机制也可以保证可靠地检测到按键事件
*/
#define KEY_FILTER_TIME   10
#define KEY_LONG_TIME     200			/* 单位5ms， 持续1秒，认为长按事件 */
#define KEY_FIFO_TIME			1				/* 5ms */
#define KEY_REPEAT_TIME   40      /* 单位5ms， 持续200ms，认为连按事件 */

/*
	每个按键对应1个全局的结构体变量。
*/
typedef struct
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	uint8_t (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

	uint8_t  Count;				/* 滤波器计数器 */
	uint16_t LongCount;		/* 长按计数器 */
	uint16_t LongTime;		/* 按键按下持续时间, 0表示不检测长按 */
	uint8_t  State;				/* 按键当前状态（按下还是弹起） */
	uint8_t  RepeatSpeed;	/* 连续按键周期 */
	uint8_t  RepeatCount;	/* 连续按键计数器 */
}KEY_T;

/* 根据应用程序的功能重命名按键宏 */
#define KEY_DOWN_K1		KEY_1_DOWN
#define KEY_UP_K1			KEY_1_UP
#define KEY_LONG_K1		KEY_1_LONG

#define KEY_DOWN_K2		KEY_2_DOWN
#define KEY_UP_K2			KEY_2_UP
#define KEY_LONG_K2		KEY_2_LONG

#define KEY_DOWN_K3		KEY_3_DOWN
#define KEY_UP_K3			KEY_3_UP
#define KEY_LONG_K3		KEY_3_LONG

/*
	定义键值代码, 必须按如下次序定时每个键的按下、弹起和长按事件
	推荐使用enum, 不用#define，原因：
	(1) 便于新增键值,方便调整顺序，使代码看起来舒服点
	(2) 编译器可帮我们避免键值重复。
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_1_DOWN,				/* 1键按下 */
	KEY_1_UP,					/* 1键弹起 */
	KEY_1_LONG,				/* 1键长按 */

	KEY_2_DOWN,				/* 2键按下 */
	KEY_2_UP,					/* 2键弹起 */
	KEY_2_LONG,				/* 2键长按 */

	KEY_3_DOWN,				/* 3键按下 */
	KEY_3_UP,					/* 3键弹起 */
	KEY_3_LONG,				/* 3键长按 */
}KEY_ENUM;

/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE	10
typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];		/* 键值缓冲区 */
	uint8_t Read;									/* 缓冲区读指针1 */
	uint8_t Write;								/* 缓冲区写指针 */
}KEY_FIFO_T;

uint8_t KEY_FIFO_Get(void);
uint8_t KEY_GetState(KEY_ID_E _ucKeyID);
void KEY_SetParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void KEY_FIFO_Clear(void);

#endif /*__bsp_key_H*/
