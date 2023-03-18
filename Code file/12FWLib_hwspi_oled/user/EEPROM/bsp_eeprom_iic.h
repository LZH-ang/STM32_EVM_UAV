#ifndef __bsp_eeprom_iic_H
#define __bsp_eeprom_iic_H

#include "stm32f10x.h"
#include "bsp_usart.h"

#define I2CT_FLAG_TIMEOUT						((uint32_t)0x1000)
/*信息输出*/
#define EEPROM_DEBUG_ON         		0
#define EEPROM_INFO(fmt,arg...)     printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)    printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)    do{\
																	  if(EEPROM_DEBUG_ON)\
                                    printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                    }while(0)

/* STM32 I2C 快速模式 */
#define I2C_Speed              400000
/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2Cx_OWN_ADDRESS7      0X0A
/* AT24C01/02每页有8个字节 */
//#define I2C_PageSize           8
/* AT24C04/08A/16A每页有16个字节 */
#define I2C_PageSize          16
#define EEPROM_page(x)     		(0x000+x*I2C_PageSize)	//24C08:64page																	
#define EEP_Firstpage      		0x000
#define ROM										1024//24C08:1024byte 0x000-0x3ff
/* EEPROM Addresses defines */
#define EEPROM_ADDRESS        0xA0
#define EEPROM_Block0_ADDRESS 0xA0  /* E2 = 0 */
#define EEPROM_Block1_ADDRESS 0xA2 	/* E2 = 0 */
#define EEPROM_Block2_ADDRESS 0xA4 	/* E2 = 0 */
#define EEPROM_Block3_ADDRESS 0xA6 	/* E2 = 0 */
#define WriteProtect(a)	if (a)	\
												GPIO_SetBits(EEPROM_I2C_WP_PORT,EEPROM_I2C_WP_PIN);\
												else		\
												GPIO_ResetBits(EEPROM_I2C_WP_PORT,EEPROM_I2C_WP_PIN)

#define EEPROM_I2C 														I2C2
#define EEPROM_I2C_APBxClock                  RCC_APB1PeriphClockCmd
#define EEPROM_I2C_CLK                        RCC_APB1Periph_I2C2
#define EEPROM_I2C_GPIO_APBxClock             RCC_APB2PeriphClockCmd
#define EEPROM_I2C_GPIO_CLK                   RCC_APB2Periph_GPIOB
#define EEPROM_I2C_SCL_PORT                   GPIOB
#define EEPROM_I2C_SCL_PIN                    GPIO_Pin_10
#define EEPROM_I2C_SDA_PORT                   GPIOB
#define EEPROM_I2C_SDA_PIN                    GPIO_Pin_11
#define EEPROM_I2C_WP_PORT										GPIOB
#define EEPROM_I2C_WP_PIN            					GPIO_Pin_1

void EEPROM_I2C_Config(void);
uint32_t I2C_EEPROM_ByteW(uint16_t Waddr, uint8_t* pData);
uint32_t I2C_EEPROM_PageW(uint16_t Waddr, uint8_t* pData, uint8_t NumByte);
void I2C_EEPROM_BufferW(uint16_t Waddr, uint8_t* pBuffer, uint16_t NumByte);
uint32_t I2C_EEPROM_ByteR(uint16_t Raddr);
uint32_t I2C_EEPROM_BufferR(uint16_t Raddr, uint8_t* pBuffer, uint16_t NumByte);
uint32_t EEPROM_ACK_POLL(void);
uint8_t EEPROM_I2C_Test(void);

#endif /*__bsp_eeprom_iic_H*/
