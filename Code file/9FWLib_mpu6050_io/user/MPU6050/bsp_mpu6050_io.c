#include "bsp_mpu6050_io.h"

static __IO uint32_t  I2CTimeout = I2CT_FLAG_TIMEOUT;
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);


/**
  * @brief   从MPU6050寄存器读取数据
  * @param   
  * @retval  
  */
uint8_t MPU6050_ReadData(uint8_t reg_add,uint8_t*Read,uint8_t num)
{
	uint8_t i;
	i2c_Start();
	i2c_SendByte(MPU6050_DEFAULT_ADDRESS);
    while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(1);
            goto cmd_fail;	/* MPU6050器件无应答 */
        }
    }
	i2c_SendByte(reg_add);
	while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(2);
            goto cmd_fail;	/* MPU6050器件无应答 */
        }
    }
	i2c_Start();
	i2c_SendByte(MPU6050_DEFAULT_ADDRESS+1);
	while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(3);
            goto cmd_fail;	/* MPU6050器件无应答 */
        }
    }
	for(i=0;i<(num-1);i++)
	{
		*Read=i2c_ReadByte(1);
		Read++;
	}
	*Read=i2c_ReadByte(0);
	i2c_Stop();
	return 1;
cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    printf("\nMPU6050 READ ERROR\n");
    i2c_Stop();
	return 0;
}

/**
  * @brief   写数据到MPU6050寄存器
  * @param   
  * @retval  
  */
uint8_t MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
	i2c_Start();
	i2c_SendByte(MPU6050_DEFAULT_ADDRESS);
	while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(4);
            goto cmd_fail;	/* MPU6050器件无应答 */
        }
    }
	i2c_SendByte(reg_add);
	while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(5);
            goto cmd_fail;	/* MPU6050器件无应答 */
        }
    }
	i2c_SendByte(reg_dat);
	while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(6);
            goto cmd_fail;	/* MPU6050器件无应答 */
        }
    }
	i2c_Stop();
	return 1;
cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    printf("\nMPU6050 WRITE ERROR\n");
    i2c_Stop();
	return 0;
}

uint8_t MPU6050_ACK_POLL(void)
{
    uint8_t poll=0;
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2CTimeout--)		/* 检测总线忙 */
    {
        if((MPU6050_I2C_SDA_READ() || MPU6050_I2C_SCL_READ()) == 0)
        {
            I2CTimeout = I2CT_FLAG_TIMEOUT;
            poll++;
        }
        if(poll == 10)
        {
            I2C_TIMEOUT_UserCallback(0);
            return 0;
        }
    }
    return 1;
}

/**
  * @brief   初始化MPU6050芯片
  * @param   
  * @retval  
  */
void MPU6050_Init(void)
{
    //在复位之后要延时一段时间，若没有延时，则数据可能会出错
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x80);		//先复位
    SysTick_Delay_ms(10);
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00);		//解除休眠状态
	
    MPU6050_WriteReg(MPU6050_CONFIG, 0x04);			//DLPF设置(Fs=1Khz,带宽20hz)
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);	  	//陀螺仪采样率100Hz
    MPU6050_WriteReg(MPU6050_GYRO_CONFI, 0x18);     //陀螺仪自检及测量范围：0x18(不自检，2000deg/s)
	MPU6050_WriteReg(MPU6050_ACCEL_CONFI, 0x00);	//加速度计自检及测量范围：0x00(不自检，+-2g)
	MPU6050_WriteReg(MPU6050_FIFO_EN, 0x00);		//关闭FIFO
	MPU6050_WriteReg(MPU6050_INT_ENABLE, 0x00);		//关闭中断
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//使用陀螺仪PLL时钟
}

/**
  * @brief   读取MPU6050的ID
  * @param   
  * @retval  
  */
uint8_t MPU6050ReadID(void)
{
	uint8_t Re = 0;
    MPU6050_ReadData(MPU6050_WHO_AM_I,&Re,1);    //读器件地址
	if(Re != 0x68)
	{
		printf("\nMPU6050 dectected error!\r\n检测不到MPU6050模块，请检查模块与开发板的接线");
		return 0;
	}
	else
	{
		printf("\nMPU6050 ID = 0x%2X\r\n",Re);
		return 1;
	}	
}

/**
  * @brief   读取MPU6050的加速度数据
  * @param   
  * @retval  
  */
void MPU6050ReadAcc(int16_t *accData)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的角加速度数据
  * @param   
  * @retval  
  */
void MPU6050ReadGyro(int16_t *gyroData)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的原始温度数据
  * @param   
  * @retval  
  */
void MPU6050ReadTemp(int16_t *tempData)
{
	uint8_t buf[2];
    MPU6050_ReadData(MPU6050_TEMP_OUT_H,buf,2);     //读取温度值
    *tempData = (buf[0] << 8) | buf[1];
}


/**
  * @brief   读取MPU6050的温度数据，转化成摄氏度
  * @param   
  * @retval  
  */
void MPU6050_ReturnTemp(float *Temperature)
{
	int16_t temp3;
	uint8_t buf[2];
	MPU6050_ReadData(MPU6050_TEMP_OUT_H,buf,2);     //读取温度值
	temp3= (buf[0] << 8) | buf[1];	
	*Temperature=((double) temp3/340.0)+36.53;
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  errorCode：错误代码，可以用来定位是哪个环节出错.
  * @retval 返回0，表示IIC读取失败.
  */
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* Block communication and all processes */
    MPU6050_ERROR("\nI2C等待超时!errorCode = %d\n",errorCode);
    return 0;
}
