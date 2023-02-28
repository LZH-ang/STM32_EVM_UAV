#include "bsp_mpu6050_io.h"

static __IO uint32_t  I2CTimeout = I2CT_FLAG_TIMEOUT;
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);


/**
  * @brief   ��MPU6050�Ĵ�����ȡ����
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
            goto cmd_fail;	/* MPU6050������Ӧ�� */
        }
    }
	i2c_SendByte(reg_add);
	while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(2);
            goto cmd_fail;	/* MPU6050������Ӧ�� */
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
            goto cmd_fail;	/* MPU6050������Ӧ�� */
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
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    printf("\nMPU6050 READ ERROR\n");
    i2c_Stop();
	return 0;
}

/**
  * @brief   д���ݵ�MPU6050�Ĵ���
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
            goto cmd_fail;	/* MPU6050������Ӧ�� */
        }
    }
	i2c_SendByte(reg_add);
	while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(5);
            goto cmd_fail;	/* MPU6050������Ӧ�� */
        }
    }
	i2c_SendByte(reg_dat);
	while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(6);
            goto cmd_fail;	/* MPU6050������Ӧ�� */
        }
    }
	i2c_Stop();
	return 1;
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    printf("\nMPU6050 WRITE ERROR\n");
    i2c_Stop();
	return 0;
}

uint8_t MPU6050_ACK_POLL(void)
{
    uint8_t poll=0;
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2CTimeout--)		/* �������æ */
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
	MPU6050_WriteReg(MPU6050_FIFO_EN, 0x00);		//�ر�FIFO
	MPU6050_WriteReg(MPU6050_INT_ENABLE, 0x00);		//�ر��ж�
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//ʹ��������PLLʱ��
}

/**
  * @brief   ��ȡMPU6050��ID
  * @param   
  * @retval  
  */
uint8_t MPU6050ReadID(void)
{
	uint8_t Re = 0;
    MPU6050_ReadData(MPU6050_WHO_AM_I,&Re,1);    //��������ַ
	if(Re != 0x68)
	{
		printf("\nMPU6050 dectected error!\r\n��ⲻ��MPU6050ģ�飬����ģ���뿪����Ľ���");
		return 0;
	}
	else
	{
		printf("\nMPU6050 ID = 0x%2X\r\n",Re);
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
