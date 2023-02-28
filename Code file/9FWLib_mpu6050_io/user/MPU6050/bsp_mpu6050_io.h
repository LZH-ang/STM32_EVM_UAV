#ifndef __bsp_mpu6050_io_H
#define __bsp_mpu6050_io_H

#include "stm32f10x.h"
#include "bsp_iic_io.h"
#include "bsp_usart.h"
#include "bsp_systick.h"

#define I2CT_FLAG_TIMEOUT				((uint32_t)0x10000)
/*信息输出*/
#define MPU6050_DEBUG_ON         		0
#define MPU6050_INFO(fmt,arg...)     printf("<<-MPU6050-INFO->> "fmt"\n",##arg)
#define MPU6050_ERROR(fmt,arg...)    printf("<<-MPU6050-ERROR->> "fmt"\n",##arg)
#define MPU6050_DEBUG(fmt,arg...)    do{\
										if(MPU6050_DEBUG_ON)\
										printf("<<-MPU6050-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                    }while(0)

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2Cx_OWN_ADDRESS7		0X0A
#define MPU6050_ADDRESS_AD0_LOW     0x68 // address pin low (GND), default for InvenSense evaluation board
#define MPU6050_ADDRESS_AD0_HIGH    0x69 // address pin high (VCC)
#define MPU6050_DEFAULT_ADDRESS     (MPU6050_ADDRESS_AD0_LOW<<1)

#define MPU6050_GYRO_OUT        MPU6050_GYRO_XOUT_H		//MPU6050陀螺仪数据寄存器地址
#define MPU6050_ACC_OUT         MPU6050_ACCEL_XOUT_H	//MPU6050加速度数据寄存器地址
/* ========== MPU6050 REDISTER MAP START ========== */
#define		MPU6050_SELF_TEST_X		0x0D
#define 	MPU6050_SELF_TEST_Y 	0x0E
#define 	MPU6050_SELF_TEST_Z 	0x0F
#define 	MPU6050_SELF_TEST_A 	0x10
#define 	MPU6050_SMPLRT_DIV  	0x19	/* SMPLRT_DIV[7:0] */
#define 	MPU6050_CONFIG  		0x1A	/* - - EXT_SYNC_SET[2:0] DLPF_CFG[2:0] */
#define		MPU6050_GYRO_CONFI		0x1B	/* - - - FS_SEL[1:0] - - - */
#define		MPU6050_ACCEL_CONFI		0x1C	/* XA_ST YA_ST ZA_ST AFS_SEL[1:0] - - - */
#define		MPU6050_MOT_THR  		0x1F
#define		MPU6050_FIFO_EN 		0x23	/* TEMP XG YG ZG ACCEL SLV2 SLV1 SLV0 */
#define		MPU6050_I2C_MST_CTRL 	0x24
#define		MPU6050_I2C_SLV0_ADDR	0x25
#define		MPU6050_I2C_SLV0_REG 	0x26
#define		MPU6050_I2C_SLV0_CTRL   0x27
#define		MPU6050_I2C_SLV1_ADDR	0x28
#define		MPU6050_I2C_SLV1_REG 	0x29
#define		MPU6050_I2C_SLV1_CTRL   0x2A
#define		MPU6050_I2C_SLV2_ADDR	0x2B
#define		MPU6050_I2C_SLV2_REG 	0x2C
#define		MPU6050_I2C_SLV2_CTRL   0x2D
#define		MPU6050_I2C_SLV3_ADDR	0x2E
#define		MPU6050_I2C_SLV3_REG 	0x2F
#define		MPU6050_I2C_SLV3_CTRL   0x30
#define		MPU6050_I2C_SLV4_ADDR	0x31
#define		MPU6050_I2C_SLV4_REG 	0x32
#define		MPU6050_I2C_SLV4_DO 	0x33
#define		MPU6050_I2C_SLV4_CTRL   0x34
#define		MPU6050_I2C_SLV4_DI 	0x35
#define		MPU6050_I2C_MST_STATUS	0x36
#define		MPU6050_INT_PIN_CFG  	0x37
#define		MPU6050_INT_ENABLE 		0x38
#define		MPU6050_INT_STATUS  	0x3A
#define		MPU6050_ACCEL_XOUT_H 	0x3B	/* ACCEL_XOUT[15:8] */
#define		MPU6050_ACCEL_XOUT_L 	0x3C	/* ACCEL_XOUT[7:0]  */
#define		MPU6050_ACCEL_YOUT_H 	0x3D	/* ACCEL_YOUT[15:8] */
#define		MPU6050_ACCEL_YOUT_L 	0x3E	/* ACCEL_YOUT[7:0]  */
#define		MPU6050_ACCEL_ZOUT_H 	0x3F	/* ACCEL_ZOUT[15:8] */
#define		MPU6050_ACCEL_ZOUT_L 	0x40	/* ACCEL_ZOUT[7:0]  */
#define		MPU6050_TEMP_OUT_H      0x41	/* TEMP_OUT[15:8] */
#define		MPU6050_TEMP_OUT_L		0x42	/* TEMP_OUT[7:0]  */
#define		MPU6050_GYRO_XOUT_H 	0x43  /* GYRO_XOUT[15:8] */
#define		MPU6050_GYRO_XOUT_L		0x44	/* GYRO_XOUT[7:0]  */
#define		MPU6050_GYRO_YOUT_H 	0x45  /* GYRO_YOUT[15:8] */
#define		MPU6050_GYRO_YOUT_L		0x46	/* GYRO_YOUT[7:0]  */
#define		MPU6050_GYRO_ZOUT_H 	0x47  /* GYRO_ZOUT[15:8] */
#define		MPU6050_GYRO_ZOUT_L		0x48	/* GYRO_ZOUT[7:0]  */
#define		MPU6050_EXT_SENS_DATA_00	0x49	/* EXT_SENS_DATA_00[7:0] */
#define		MPU6050_EXT_SENS_DATA_01	0x4A	/* EXT_SENS_DATA_01[7:0] */
#define		MPU6050_EXT_SENS_DATA_02	0x4B	/* EXT_SENS_DATA_02[7:0] */
#define		MPU6050_EXT_SENS_DATA_03	0x4C	/* EXT_SENS_DATA_03[7:0] */
#define		MPU6050_EXT_SENS_DATA_04	0x4D	/* EXT_SENS_DATA_04[7:0] */
#define		MPU6050_EXT_SENS_DATA_05	0x4E	/* EXT_SENS_DATA_05[7:0] */
#define		MPU6050_EXT_SENS_DATA_06	0x4F	/* EXT_SENS_DATA_06[7:0] */
#define		MPU6050_EXT_SENS_DATA_07	0x50	/* EXT_SENS_DATA_07[7:0] */
#define		MPU6050_EXT_SENS_DATA_08	0x51	/* EXT_SENS_DATA_07[7:0] */
#define		MPU6050_EXT_SENS_DATA_09	0x52	/* EXT_SENS_DATA_09[7:0] */
#define		MPU6050_EXT_SENS_DATA_10	0x53	/* EXT_SENS_DATA_10[7:0] */
#define		MPU6050_EXT_SENS_DATA_11	0x54	/* EXT_SENS_DATA_11[7:0] */
#define		MPU6050_EXT_SENS_DATA_12	0x55	/* EXT_SENS_DATA_12[7:0] */
#define		MPU6050_EXT_SENS_DATA_13	0x56	/* EXT_SENS_DATA_13[7:0] */
#define		MPU6050_EXT_SENS_DATA_14	0x57	/* EXT_SENS_DATA_14[7:0] */
#define		MPU6050_EXT_SENS_DATA_15	0x58	/* EXT_SENS_DATA_15[7:0] */
#define		MPU6050_EXT_SENS_DATA_16	0x59	/* EXT_SENS_DATA_16[7:0] */
#define		MPU6050_EXT_SENS_DATA_17	0x5A	/* EXT_SENS_DATA_17[7:0] */
#define		MPU6050_EXT_SENS_DATA_18	0x5B	/* EXT_SENS_DATA_18[7:0] */
#define		MPU6050_EXT_SENS_DATA_19	0x5C	/* EXT_SENS_DATA_19[7:0] */
#define		MPU6050_EXT_SENS_DATA_20	0x5D	/* EXT_SENS_DATA_20[7:0] */
#define		MPU6050_EXT_SENS_DATA_21	0x5E	/* EXT_SENS_DATA_21[7:0] */
#define		MPU6050_EXT_SENS_DATA_22	0x5F	/* EXT_SENS_DATA_22[7:0] */
#define		MPU6050_EXT_SENS_DATA_23	0x60	/* EXT_SENS_DATA_23[7:0] */
#define		MPU6050_I2C_SLV0_DO			0x63
#define		MPU6050_I2C_SLV1_DO			0x64
#define		MPU6050_I2C_SLV2_DO			0x65
#define		MPU6050_I2C_SLV3_DO			0x66
#define		MPU6050_I2C_MST_DELAY_CTRL	0x67
#define		MPU6050_SIGNAL_PATH_RESET	0x68	/* - - - - - GYRO_RESET ACCEL_RESET TEMP_RESET */
#define		MPU6050_MOT_DETECT_CTRL		0x69
#define		MPU6050_USER_CTRL			0x6A
#define		MPU6050_PWR_MGMT_1 			0x6B	/* DEVICE_RESET SLEEP CYCLE - TEMP_DIS CLKSEL[2:0] */
#define		MPU6050_PWR_MGMT_2			0X6c	/**/
#define		MPU6050_FIFO_COUNTH 		0X72
#define		MPU6050_FIFO_COUNTl			0X73
#define		MPU6050_FIFO_R_W  		 	0X74
#define		MPU6050_WHO_AM_I 			0X75	/* - WHO_AM_I[6:1] - */
/* ========== MPU6050 REDISTER MAP END ========== */

uint8_t MPU6050_ReadData(uint8_t reg_add,uint8_t*Read,uint8_t num);
uint8_t MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat);
uint8_t MPU6050_ACK_POLL(void);
void MPU6050_Init(void);
uint8_t MPU6050ReadID(void);
void MPU6050ReadAcc(int16_t *accData);
void MPU6050ReadGyro(int16_t *gyroData);
void MPU6050ReadTemp(int16_t *tempData);
void MPU6050_ReturnTemp(float *Temperature);


#endif /*__bsp_mpu6050_io_H*/
