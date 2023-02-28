#include "bsp_iic_io.h"

static void i2c_Delay(void)
{
	uint8_t i;
	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	for (i = 0; i < 7; i++);
}

void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	MPU6050_I2C_SDA_1();
	MPU6050_I2C_SCL_1();
	i2c_Delay();
	MPU6050_I2C_SDA_0();
	i2c_Delay();
	MPU6050_I2C_SCL_0();
	i2c_Delay();
}

void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	MPU6050_I2C_SDA_0();
	MPU6050_I2C_SCL_1();
	i2c_Delay();
	MPU6050_I2C_SDA_1();
}

void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;
	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			MPU6050_I2C_SDA_1();
		}
		else
		{
			MPU6050_I2C_SDA_0();
		}
		i2c_Delay();
		MPU6050_I2C_SCL_1();
		i2c_Delay();	
		MPU6050_I2C_SCL_0();
		if (i == 7)
		{
			MPU6050_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}

uint8_t i2c_ReadByte(uint8_t ack)
{
	uint8_t i;
	uint8_t value;
	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		MPU6050_I2C_SCL_1();
		i2c_Delay();
		if (MPU6050_I2C_SDA_READ())
		{
			value++;
		}
		MPU6050_I2C_SCL_0();
		i2c_Delay();
	}
	if(ack==0)
		i2c_NAck();
	else
		i2c_Ack();
	return value;
}

uint8_t i2c_WaitAck(void)
{
	uint8_t re;
	MPU6050_I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	MPU6050_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (MPU6050_I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;//NOACK
	}
	else
	{
		re = 0;//ACK
	}
	MPU6050_I2C_SCL_0();
	i2c_Delay();
	return re;
}

void i2c_Ack(void)
{
	MPU6050_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	MPU6050_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	MPU6050_I2C_SCL_0();
	i2c_Delay();
	MPU6050_I2C_SDA_1();	/* CPU释放SDA总线 */
}

void i2c_NAck(void)
{
	MPU6050_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	MPU6050_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	MPU6050_I2C_SCL_0();
	i2c_Delay();	
}

void i2c_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(MPU6050_RCC_I2C_PORT, ENABLE);	/* 打开GPIO时钟 */
	GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_SCL_PIN | MPU6050_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */
	GPIO_Init(MPU6050_GPIO_I2C_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_INT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */
	GPIO_Init(MPU6050_I2C_INT_PORT, &GPIO_InitStructure);
}
