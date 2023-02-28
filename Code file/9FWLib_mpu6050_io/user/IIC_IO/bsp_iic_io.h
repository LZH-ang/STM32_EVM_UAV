#ifndef __bsp_iic_io_H
#define __bsp_iic_io_H

#include "stm32f10x.h"

#define MPU6050_GPIO_I2C_PORT	GPIOB					/* IICGPIO端口 */
#define MPU6050_RCC_I2C_PORT 	RCC_APB2Periph_GPIOB	/* MPU6050GPIO端口时钟 */
#define MPU6050_I2C_SCL_PIN		GPIO_Pin_6				/* 连接到SCL时钟线的GPIO */
#define MPU6050_I2C_SDA_PIN		GPIO_Pin_7				/* 连接到SDA数据线的GPIO */
#define MPU6050_I2C_INT_PORT	GPIOB 					/* INTGPIO端口 */
#define MPU6050_I2C_INT_PIN   	GPIO_Pin_5				/* 连接到INT的GPIO */

#if 0	/* 条件编译： 1 选择GPIO的库函数实现IO读写 */
	#define MPU6050_I2C_SCL_1()  GPIO_SetBits(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SCL_PIN)		/* SCL = 1 */
	#define MPU6050_I2C_SCL_0()  GPIO_ResetBits(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SCL_PIN)		/* SCL = 0 */
	#define MPU6050_I2C_SDA_1()  GPIO_SetBits(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SDA_PIN)		/* SDA = 1 */
	#define MPU6050_I2C_SDA_0()  GPIO_ResetBits(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SDA_PIN)		/* SDA = 0 */
	#define MPU6050_I2C_SDA_READ()  GPIO_ReadInputDataBit(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SDA_PIN)	/* 读SDA口线状态 */
	#define MPU6050_I2C_SCL_READ()  GPIO_ReadInputDataBit(MPU6050_GPIO_PORT_I2C, MPU6050_I2C_SCL_PIN)	/* 读SCL口线状态 */
#else	/* 这个分支选择直接寄存器操作实现IO读写 */
    /*　注意：如下写法，在IAR最高级别优化时，会被编译器错误优化 */
	#define MPU6050_I2C_SCL_1()  MPU6050_GPIO_I2C_PORT->BSRR = MPU6050_I2C_SCL_PIN			/* SCL = 1 */
	#define MPU6050_I2C_SCL_0()  MPU6050_GPIO_I2C_PORT->BRR = MPU6050_I2C_SCL_PIN			/* SCL = 0 */
	#define MPU6050_I2C_SDA_1()  MPU6050_GPIO_I2C_PORT->BSRR = MPU6050_I2C_SDA_PIN			/* SDA = 1 */
	#define MPU6050_I2C_SDA_0()  MPU6050_GPIO_I2C_PORT->BRR = MPU6050_I2C_SDA_PIN			/* SDA = 0 */
	#define MPU6050_I2C_SDA_READ()  ((MPU6050_GPIO_I2C_PORT->IDR & MPU6050_I2C_SDA_PIN) != 0)	/* 读SDA口线状态 */
	#define MPU6050_I2C_SCL_READ()  ((MPU6050_GPIO_I2C_PORT->IDR & MPU6050_I2C_SCL_PIN) != 0)	/* 读SCL口线状态 */
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
