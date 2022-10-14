#ifndef __bsp_iic_io_H
#define __bsp_iic_io_H

#include "stm32f10x.h"

#define EEPROM_GPIO_PORT_I2C	GPIOB									/* IICGPIO�˿� */
#define EEPROM_RCC_I2C_PORT 	RCC_APB2Periph_GPIOB	/* EEPROMGPIO�˿�ʱ�� */
#define EEPROM_I2C_SCL_PIN		GPIO_Pin_10				/* ���ӵ�SCLʱ���ߵ�GPIO */
#define EEPROM_I2C_SDA_PIN		GPIO_Pin_11				/* ���ӵ�SDA�����ߵ�GPIO */
#define EEPROM_I2C_WP_PORT		GPIOB 								/* WPGPIO�˿� */
#define EEPROM_I2C_WP_PIN     GPIO_Pin_1				/* ���ӵ�WPд������GPIO */

#if 0	/* �������룺 1 ѡ��GPIO�Ŀ⺯��ʵ��IO��д */
	#define EEPROM_I2C_SCL_1()  GPIO_SetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)		/* SCL = 1 */
	#define EEPROM_I2C_SCL_0()  GPIO_ResetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)	/* SCL = 0 */
	#define EEPROM_I2C_SDA_1()  GPIO_SetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)		/* SDA = 1 */
	#define EEPROM_I2C_SDA_0()  GPIO_ResetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)	/* SDA = 0 */
	#define EEPROM_I2C_SDA_READ()  GPIO_ReadInputDataBit(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)	/* ��SDA����״̬ */
	#define EEPROM_I2C_SCL_READ()  GPIO_ReadInputDataBit(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)	/* ��SCL����״̬ */
#else	/* �����֧ѡ��ֱ�ӼĴ�������ʵ��IO��д */
    /*��ע�⣺����д������IAR��߼����Ż�ʱ���ᱻ�����������Ż� */
	#define EEPROM_I2C_SCL_1()  EEPROM_GPIO_PORT_I2C->BSRR = EEPROM_I2C_SCL_PIN				/* SCL = 1 */
	#define EEPROM_I2C_SCL_0()  EEPROM_GPIO_PORT_I2C->BRR = EEPROM_I2C_SCL_PIN				/* SCL = 0 */
	#define EEPROM_I2C_SDA_1()  EEPROM_GPIO_PORT_I2C->BSRR = EEPROM_I2C_SDA_PIN				/* SDA = 1 */
	#define EEPROM_I2C_SDA_0()  EEPROM_GPIO_PORT_I2C->BRR = EEPROM_I2C_SDA_PIN				/* SDA = 0 */
	#define EEPROM_I2C_SDA_READ()  ((EEPROM_GPIO_PORT_I2C->IDR & EEPROM_I2C_SDA_PIN) != 0)	/* ��SDA����״̬ */
	#define EEPROM_I2C_SCL_READ()  ((EEPROM_GPIO_PORT_I2C->IDR & EEPROM_I2C_SCL_PIN) != 0)	/* ��SCL����״̬ */
#endif

#define WriteProtect(a)	if (a)	\
												GPIO_SetBits(EEPROM_I2C_WP_PORT,EEPROM_I2C_WP_PIN);\
												else		\
												GPIO_ResetBits(EEPROM_I2C_WP_PORT,EEPROM_I2C_WP_PIN)

void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
void EEPROM_Software_Reset(void);
void i2c_GPIO_Config(void);

#endif /*__bsp_iic_io_H*/
