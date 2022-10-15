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

/* ����ID, ��Ҫ����KEY_GetState()��������ڲ��� */
typedef enum
{
	KID_K1 = 0,
	KID_K2,
	KID_K3,
}KEY_ID_E;

/*
	�����˲�ʱ��50ms, ��λ5ms��
	ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼�
	��ʹ������·����Ӳ���˲������˲�����Ҳ���Ա�֤�ɿ��ؼ�⵽�����¼�
*/
#define KEY_FILTER_TIME   10
#define KEY_LONG_TIME     200			/* ��λ5ms�� ����1�룬��Ϊ�����¼� */
#define KEY_FIFO_TIME			1				/* 5ms */
#define KEY_REPEAT_TIME   40      /* ��λ5ms�� ����200ms����Ϊ�����¼� */

/*
	ÿ��������Ӧ1��ȫ�ֵĽṹ�������
*/
typedef struct
{
	/* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
	uint8_t (*IsKeyDownFunc)(void); /* �������µ��жϺ���,1��ʾ���� */

	uint8_t  Count;				/* �˲��������� */
	uint16_t LongCount;		/* ���������� */
	uint16_t LongTime;		/* �������³���ʱ��, 0��ʾ����ⳤ�� */
	uint8_t  State;				/* ������ǰ״̬�����»��ǵ��� */
	uint8_t  RepeatSpeed;	/* ������������ */
	uint8_t  RepeatCount;	/* �������������� */
}KEY_T;

/* ����Ӧ�ó���Ĺ��������������� */
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
	�����ֵ����, ���밴���´���ʱÿ�����İ��¡�����ͳ����¼�
	�Ƽ�ʹ��enum, ����#define��ԭ��
	(1) ����������ֵ,�������˳��ʹ���뿴���������
	(2) �������ɰ����Ǳ����ֵ�ظ���
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 ��ʾ�����¼� */

	KEY_1_DOWN,				/* 1������ */
	KEY_1_UP,					/* 1������ */
	KEY_1_LONG,				/* 1������ */

	KEY_2_DOWN,				/* 2������ */
	KEY_2_UP,					/* 2������ */
	KEY_2_LONG,				/* 2������ */

	KEY_3_DOWN,				/* 3������ */
	KEY_3_UP,					/* 3������ */
	KEY_3_LONG,				/* 3������ */
}KEY_ENUM;

/* ����FIFO�õ����� */
#define KEY_FIFO_SIZE	10
typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];		/* ��ֵ������ */
	uint8_t Read;									/* ��������ָ��1 */
	uint8_t Write;								/* ������дָ�� */
}KEY_FIFO_T;

uint8_t KEY_FIFO_Get(void);
uint8_t KEY_GetState(KEY_ID_E _ucKeyID);
void KEY_SetParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void KEY_FIFO_Clear(void);

#endif /*__bsp_key_H*/
