#ifndef __bsp_iic_io_H
#define __bsp_iic_io_H

#include "stm32f10x.h"

#define EEPROM_GPIO_PORT_I2C	GPIOB									/* IICGPIO端口 */
#define EEPROM_RCC_I2C_PORT 	RCC_APB2Periph_GPIOB	/* EEPROMGPIO端口时钟 */
#define EEPROM_I2C_SCL_PIN		GPIO_Pin_10				/* 连接到SCL时钟线的GPIO */
#define EEPROM_I2C_SDA_PIN		GPIO_Pin_11				/* 连接到SDA数据线的GPIO */
#define EEPROM_I2C_WP_PORT		GPIOB 								/* WPGPIO端口 */
#define EEPROM_I2C_WP_PIN     GPIO_Pin_1				/* 连接到WP写保护的GPIO */

#if 0	/* 条件编译： 1 选择GPIO的库函数实现IO读写 */
	#define EEPROM_I2C_SCL_1()  GPIO_SetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)		/* SCL = 1 */
	#define EEPROM_I2C_SCL_0()  GPIO_ResetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)	/* SCL = 0 */
	#define EEPROM_I2C_SDA_1()  GPIO_SetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)		/* SDA = 1 */
	#define EEPROM_I2C_SDA_0()  GPIO_ResetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)	/* SDA = 0 */
	#define EEPROM_I2C_SDA_READ()  GPIO_ReadInputDataBit(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)	/* 读SDA口线状态 */
	#define EEPROM_I2C_SCL_READ()  GPIO_ReadInputDataBit(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)	/* 读SCL口线状态 */
#else	/* 这个分支选择直接寄存器操作实现IO读写 */
    /*　注意：如下写法，在IAR最高级别优化时，会被编译器错误优化 */
	#define EEPROM_I2C_SCL_1()  EEPROM_GPIO_PORT_I2C->BSRR = EEPROM_I2C_SCL_PIN				/* SCL = 1 */
	#define EEPROM_I2C_SCL_0()  EEPROM_GPIO_PORT_I2C->BRR = EEPROM_I2C_SCL_PIN				/* SCL = 0 */
	#define EEPROM_I2C_SDA_1()  EEPROM_GPIO_PORT_I2C->BSRR = EEPROM_I2C_SDA_PIN				/* SDA = 1 */
	#define EEPROM_I2C_SDA_0()  EEPROM_GPIO_PORT_I2C->BRR = EEPROM_I2C_SDA_PIN				/* SDA = 0 */
	#define EEPROM_I2C_SDA_READ()  ((EEPROM_GPIO_PORT_I2C->IDR & EEPROM_I2C_SDA_PIN) != 0)	/* 读SDA口线状态 */
	#define EEPROM_I2C_SCL_READ()  ((EEPROM_GPIO_PORT_I2C->IDR & EEPROM_I2C_SCL_PIN) != 0)	/* 读SCL口线状态 */
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
