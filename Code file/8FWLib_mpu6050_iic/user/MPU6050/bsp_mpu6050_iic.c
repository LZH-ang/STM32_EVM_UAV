#include "bsp_mpu6050_iic.h"

static __IO uint32_t  I2CTimeout = I2CT_FLAG_TIMEOUT;
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

void MPU6050_I2C_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /*I2C_GPIO_Config*/
    /* 使能与 I2C 有关的时钟 */
    MPU6050_I2C_APBxClock(MPU6050_I2C_CLK, ENABLE);
    MPU6050_I2C_GPIO_APBxClock(MPU6050_I2C_GPIO_CLK, ENABLE);

//    /*MPUINT PIN*/
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	// 上拉输入
//    GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_INT_PIN;
//    GPIO_Init(MPU6050_I2C_INT_PORT, &GPIO_InitStructure);

    /* I2C_SCL、I2C_SDA*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	// 开漏输出
    GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_SCL_PIN;
    GPIO_Init(MPU6050_I2C_SCL_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_SDA_PIN;
    GPIO_Init(MPU6050_I2C_SDA_PORT, &GPIO_InitStructure);

    /* I2C 配置 */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    /* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 =I2Cx_OWN_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
    /* I2C的寻址模式 */
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    /* 通信速率 */
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
    /* I2C 初始化 */
    I2C_Init(MPU6050_I2C, &I2C_InitStructure);
    /* 使能 I2C */
    I2C_Cmd(MPU6050_I2C, ENABLE);
}

/**
  * @brief   从I2C设备里面读取一块数据
  * @param
  *		@arg pBuffer:存放从slave读取的数据的缓冲区指针
  *		@arg WriteAddr:接收数据的从设备的地址
  *		@arg NumByteToWrite:要从从设备读取的字节数
  * @retval  正常返回1，异常返回0
  */
uint8_t MPU6050_I2C_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_BUSY))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(0);
    }
    I2C_GenerateSTART(MPU6050_I2C, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(1);
    }
    /* Send slave address for write */
    I2C_Send7bitAddress(MPU6050_I2C, MPU6050_DEFAULT_ADDRESS, I2C_Direction_Transmitter);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 也行
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(2);
    }
    /* Send the slave's internal address to write to */
    I2C_SendData(MPU6050_I2C, ReadAddr);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
    }
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(MPU6050_I2C, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(4);
    }
    /* Send slave address for read */
    I2C_Send7bitAddress(MPU6050_I2C, MPU6050_DEFAULT_ADDRESS, I2C_Direction_Receiver);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(5);
    }
    /* While there is data to be read */
    while(NumByteToRead)
    {
        if(NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(MPU6050_I2C, DISABLE);
            /* Send STOP Condition */
            I2C_GenerateSTOP(MPU6050_I2C, ENABLE);
        }
        /* Test on EV7 and clear it */
        if(I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            /* Read a byte from the slave */
            *pBuffer = I2C_ReceiveData(MPU6050_I2C);
            /* Point to the next location where the byte read will be saved */
            pBuffer++;
            /* Decrement the read bytes counter */
            NumByteToRead--;
        }
    }
    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(MPU6050_I2C, ENABLE);
    return 1; //正常，返回1
}

/**
  * @brief   写一个字节到I2C设备中
  * @param
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:写地址
  * @retval  正常返回1，异常返回0
  */
uint8_t MPU6050_I2C_ByteWrite(uint8_t pBuffer, uint8_t WriteAddr)
{
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_BUSY))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(6);
    }
    /* Send STRAT condition */
    I2C_GenerateSTART(MPU6050_I2C, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(7);
    }
    /* Send slave address for write */
    I2C_Send7bitAddress(MPU6050_I2C, MPU6050_DEFAULT_ADDRESS, I2C_Direction_Transmitter);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(8);
    }
    /* Send the slave's internal address to write to */
    I2C_SendData(MPU6050_I2C, WriteAddr);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
    }
    /* Send the byte to be written */
    I2C_SendData(MPU6050_I2C, pBuffer);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
    }
    /* Send STOP condition */
    I2C_GenerateSTOP(MPU6050_I2C, ENABLE);
    return 1; //正常返回1
}

/**
  * @brief  Wait for slave Standby state
  * @param  无
  * @retval 无
  */
void MPU6050_I2C_WaitStandbyState(void)
{
    uint16_t SR1_Tmp = 0;
    do
    {
        /* Send START condition */
        I2C_GenerateSTART(MPU6050_I2C, ENABLE);
        /* Read I2C1 SR1 register */
        SR1_Tmp = I2C_ReadRegister(MPU6050_I2C, I2C_Register_SR1);
        /* Send slave address for write */
        I2C_Send7bitAddress(MPU6050_I2C, MPU6050_DEFAULT_ADDRESS, I2C_Direction_Transmitter);
    } while(!(I2C_ReadRegister(MPU6050_I2C, I2C_Register_SR1) & 0x0002));
    /* Clear AF flag */
    I2C_ClearFlag(MPU6050_I2C, I2C_FLAG_AF);
    /* STOP condition */
    I2C_GenerateSTOP(MPU6050_I2C, ENABLE);
}

/**
  * @brief   从MPU6050寄存器读取数据
  * @param
  * @retval
  */
void MPU6050_ReadData(uint8_t reg_add,uint8_t* Read,uint8_t num)
{
    MPU6050_I2C_BufferRead(Read,reg_add,num);
}

/**
  * @brief   写数据到MPU6050寄存器
  * @param
  * @retval
  */
void MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
    MPU6050_I2C_ByteWrite(reg_dat,reg_add);
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
	MPU6050_WriteReg(MPU6050_FIFO_EN, 0xf8);		//开启FIFO
	MPU6050_WriteReg(MPU6050_INT_PIN_CFG, 0x80);	//配置中断(低电平,触发产生脉冲)
	MPU6050_WriteReg(MPU6050_INT_ENABLE, 0x10);		//开启FIFO中断
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//使用陀螺仪PLL时钟
}

/**
  * @brief   读取MPU6050的ID
  * @param
  * @retval  正常返回1，异常返回0
  */
uint8_t MPU6050ReadID(void)
{
    uint8_t Re = 0;
    MPU6050_ReadData(MPU6050_WHO_AM_I,&Re,1);    //读器件地址
    if(Re != 0x68)
    {
        MPU6050_ERROR("MPU6050 dectected error!\n检测不到MPU6050模块，请检查模块与开发板的接线\r\n");
        return 0;
    }
    else
    {
        MPU6050_INFO("MPU6050 ID = 0x%2X",Re);
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
