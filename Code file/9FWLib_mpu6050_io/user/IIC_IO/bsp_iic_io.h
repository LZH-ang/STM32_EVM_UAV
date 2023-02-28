#ifndef __bsp_iic_io_H
#define __bsp_iic_io_H

#include "stm32f10x.h"

#define MPU6050_GPIO_I2C_PORT	GPIOB					/* IICGPIO�˿� */
#define MPU6050_RCC_I2C_PORT 	RCC_APB2Periph_GPIOB	/* MPU6050GPIO�˿�ʱ�� */
#define MPU6050_I2C_SCL_PIN		GPIO_Pin_6				/* ���ӵ�SCLʱ���ߵ�GPIO */
#define MPU6050_I2C_SDA_PIN		GPIO_Pin_7				/* ���ӵ�SDA�����ߵ�GPIO */
#define MPU6050_I2C_INT_PORT	GPIOB 					/* INTGPIO�˿� */
#define MPU6050_I2C_INT_PIN   	GPIO_Pin_5				/* ���ӵ�INT��GPIO */

#if 0	/* �������룺 1 ѡ��GPIO�Ŀ⺯��ʵ��IO��д */
	#define MPU6050_I2C_SCL_1()  GPIO_SetBits(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SCL_PIN)		/* SCL = 1 */
	#define MPU6050_I2C_SCL_0()  GPIO_ResetBits(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SCL_PIN)		/* SCL = 0 */
	#define MPU6050_I2C_SDA_1()  GPIO_SetBits(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SDA_PIN)		/* SDA = 1 */
	#define MPU6050_I2C_SDA_0()  GPIO_ResetBits(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SDA_PIN)		/* SDA = 0 */
	#define MPU6050_I2C_SDA_READ()  GPIO_ReadInputDataBit(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SDA_PIN)	/* ��SDA����״̬ */
	#define MPU6050_I2C_SCL_READ()  GPIO_ReadInputDataBit(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SCL_PIN)	/* ��SCL����״̬ */
#else	/* �����֧ѡ��ֱ�ӼĴ�������ʵ��IO��д */
    /*��ע�⣺����д������IAR��߼����Ż�ʱ���ᱻ�����������Ż� */
	#define MPU6050_I2C_SCL_1()  MPU6050_GPIO_I2C_PORT->BSRR = MPU6050_I2C_SCL_PIN			/* SCL = 1 */
	#define MPU6050_I2C_SCL_0()  MPU6050_GPIO_I2C_PORT->BRR = MPU6050_I2C_SCL_PIN			/* SCL = 0 */
	#define MPU6050_I2C_SDA_1()  MPU6050_GPIO_I2C_PORT->BSRR = MPU6050_I2C_SDA_PIN			/* SDA = 1 */
	#define MPU6050_I2C_SDA_0()  MPU6050_GPIO_I2C_PORT->BRR = MPU6050_I2C_SDA_PIN			/* SDA = 0 */
	#define MPU6050_I2C_SDA_READ()  ((MPU6050_GPIO_I2C_PORT->IDR & MPU6050_I2C_SDA_PIN) != 0)	/* ��SDA����״̬ */
	#define MPU6050_I2C_SCL_READ()  ((MPU6050_GPIO_I2C_PORT->IDR & MPU6050_I2C_SCL_PIN) != 0)	/* ��SCL����״̬ */
#endif

void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(uint8_t ack);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
void i2c_GPIO_Config(void);

#endif /*__bsp_iic_io_H*/
