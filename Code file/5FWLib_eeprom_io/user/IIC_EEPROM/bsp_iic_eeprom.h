#ifndef __bsp_iic_eeprom_H
#define __bsp_iic_eeprom_H

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

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2Cx_OWN_ADDRESS7      0X0A
/* AT24C01/02每页有8个字节 */
//#define EEPROM_PageSize       8
/* AT24C04/08A/16A每页有16个字节 */
#define EEPROM_PageSize       16
#define EEPROM_page(x)     		(0x000+x*EEPROM_PageSize)	//24C08:64page																	
#define EEP_Firstpage      		0x000
#define ROM										1024//24C08:1024byte 0x000-0x3ff
/* EEPROM Addresses defines */
#define EEPROM_ADDRESS        0xA0
#define EEPROM_Block0_ADDRESS 0xA0  /* A2 = 0 */
#define EEPROM_Block1_ADDRESS 0xA2 	/* A2 = 0 */
#define EEPROM_Block2_ADDRESS 0xA4 	/* A2 = 0 */
#define EEPROM_Block3_ADDRESS 0xA6 	/* A2 = 0 */
#define EEPROM_I2C_WR	0x00			/* 写控制bit */
#define EEPROM_I2C_RD	0x01			/* 读控制bit */

uint8_t I2C_EEPROM_ByteR(uint16_t Raddr, uint8_t* pBuffer, uint16_t NumByte);
uint8_t I2C_EEPROM_ByteW(uint16_t Waddr, uint8_t* pBuffer, uint16_t NumByte);
uint8_t EEPROM_ACK_POLL(void);
void EEPROM_Erase(void);
uint8_t EEPROM_Test(void);

#endif /*__bsp_iic_eeprom_H*/
