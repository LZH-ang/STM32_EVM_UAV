#include "bsp_mpu6050_iic.h"

static __IO uint32_t  I2CTimeout = I2CT_FLAG_TIMEOUT;
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

void MPU6050_I2C_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /*I2C_GPIO_Config*/
    /* ʹ���� I2C �йص�ʱ�� */
    MPU6050_I2C_APBxClock(MPU6050_I2C_CLK, ENABLE);
    MPU6050_I2C_GPIO_APBxClock(MPU6050_I2C_GPIO_CLK, ENABLE);

//    /*MPUINT PIN*/
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	// ��������
//    GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_INT_PIN;
//    GPIO_Init(MPU6050_I2C_INT_PORT, &GPIO_InitStructure);

    /* I2C_SCL��I2C_SDA*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	// ��©���
    GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_SCL_PIN;
    GPIO_Init(MPU6050_I2C_SCL_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_SDA_PIN;
    GPIO_Init(MPU6050_I2C_SDA_PORT, &GPIO_InitStructure);

    /* I2C ���� */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    /* �ߵ�ƽ�����ȶ����͵�ƽ���ݱ仯 SCL ʱ���ߵ�ռ�ձ� */
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 =I2Cx_OWN_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
    /* I2C��Ѱַģʽ */
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    /* ͨ������ */
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
    /* I2C ��ʼ�� */
    I2C_Init(MPU6050_I2C, &I2C_InitStructure);
    /* ʹ�� I2C */
    I2C_Cmd(MPU6050_I2C, ENABLE);
}

/**
  * @brief   ��I2C�豸�����ȡһ������
  * @param
  *		@arg pBuffer:��Ŵ�slave��ȡ�����ݵĻ�����ָ��
  *		@arg WriteAddr:�������ݵĴ��豸�ĵ�ַ
  *		@arg NumByteToWrite:Ҫ�Ӵ��豸��ȡ���ֽ���
  * @retval  ��������1���쳣����0
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
    while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED Ҳ��
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
    return 1; //����������1
}

/**
  * @brief   дһ���ֽڵ�I2C�豸��
  * @param
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:д��ַ
  * @retval  ��������1���쳣����0
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
    return 1; //��������1
}

/**
  * @brief  Wait for slave Standby state
  * @param  ��
  * @retval ��
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
  * @brief   ��MPU6050�Ĵ�����ȡ����
  * @param
  * @retval
  */
void MPU6050_ReadData(uint8_t reg_add,uint8_t* Read,uint8_t num)
{
    MPU6050_I2C_BufferRead(Read,reg_add,num);
}

/**
  * @brief   д���ݵ�MPU6050�Ĵ���
  * @param
  * @retval
  */
void MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
    MPU6050_I2C_ByteWrite(reg_dat,reg_add);
}

/**
  * @brief   ��ʼ��MPU6050оƬ
  * @param
  * @retval
  */
void MPU6050_Init(void)
{
    //�ڸ�λ֮��Ҫ��ʱһ��ʱ�䣬��û����ʱ�������ݿ��ܻ����
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x80);		//�ȸ�λ
    SysTick_Delay_ms(10);
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00);		//�������״̬
	
    MPU6050_WriteReg(MPU6050_CONFIG, 0x04);			//DLPF����(Fs=1Khz,����20hz)
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);	  	//�����ǲ�����100Hz
    MPU6050_WriteReg(MPU6050_GYRO_CONFI, 0x18);     //�������Լ켰������Χ��0x18(���Լ죬2000deg/s)
	MPU6050_WriteReg(MPU6050_ACCEL_CONFI, 0x00);	//���ٶȼ��Լ켰������Χ��0x00(���Լ죬+-2g)
	MPU6050_WriteReg(MPU6050_FIFO_EN, 0xf8);		//����FIFO
	MPU6050_WriteReg(MPU6050_INT_PIN_CFG, 0x80);	//�����ж�(�͵�ƽ,������������)
	MPU6050_WriteReg(MPU6050_INT_ENABLE, 0x10);		//����FIFO�ж�
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//ʹ��������PLLʱ��
}

/**
  * @brief   ��ȡMPU6050��ID
  * @param
  * @retval  ��������1���쳣����0
  */
uint8_t MPU6050ReadID(void)
{
    uint8_t Re = 0;
    MPU6050_ReadData(MPU6050_WHO_AM_I,&Re,1);    //��������ַ
    if(Re != 0x68)
    {
        MPU6050_ERROR("MPU6050 dectected error!\n��ⲻ��MPU6050ģ�飬����ģ���뿪����Ľ���\r\n");
        return 0;
    }
    else
    {
        MPU6050_INFO("MPU6050 ID = 0x%2X",Re);
        return 1;
    }
}

/**
  * @brief   ��ȡMPU6050�ļ��ٶ�����
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
  * @brief   ��ȡMPU6050�ĽǼ��ٶ�����
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
  * @brief   ��ȡMPU6050��ԭʼ�¶�����
  * @param
  * @retval
  */
void MPU6050ReadTemp(int16_t *tempData)
{
    uint8_t buf[2];
    MPU6050_ReadData(MPU6050_TEMP_OUT_H,buf,2);     //��ȡ�¶�ֵ
    *tempData = (buf[0] << 8) | buf[1];
}

/**
  * @brief   ��ȡMPU6050���¶����ݣ�ת�������϶�
  * @param
  * @retval
  */
void MPU6050_ReturnTemp(float *Temperature)
{
    int16_t temp3;
    uint8_t buf[2];
    MPU6050_ReadData(MPU6050_TEMP_OUT_H,buf,2);     //��ȡ�¶�ֵ
    temp3= (buf[0] << 8) | buf[1];
    *Temperature=((double) temp3/340.0)+36.53;
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  errorCode��������룬����������λ���ĸ����ڳ���.
  * @retval ����0����ʾIIC��ȡʧ��.
  */
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* Block communication and all processes */
    MPU6050_ERROR("\nI2C�ȴ���ʱ!errorCode = %d\n",errorCode);
    return 0;
}
