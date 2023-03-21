#include "bsp_eeprom_iic.h"

static __IO uint32_t  I2CTimeout = I2CT_FLAG_TIMEOUT;
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

//超时造成的异常问题可以通过重新初始化retry?

void EEPROM_I2C_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /*I2C_GPIO_Config*/
    /* 使能与 I2C 有关的时钟 */
    EEPROM_I2C_APBxClock(EEPROM_I2C_CLK, ENABLE);
    EEPROM_I2C_GPIO_APBxClock(EEPROM_I2C_GPIO_CLK, ENABLE);

    /*Write protect*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_WP_PIN;
    GPIO_Init(EEPROM_I2C_WP_PORT, &GPIO_InitStructure);

    /* I2C_SCL、I2C_SDA*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
    GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SCL_PIN;
    GPIO_Init(EEPROM_I2C_SCL_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SDA_PIN;
    GPIO_Init(EEPROM_I2C_SDA_PORT, &GPIO_InitStructure);

    /*I2C_Mode_Config*/
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
    I2C_Init(EEPROM_I2C, &I2C_InitStructure);
    /* 使能 I2C */
    I2C_Cmd(EEPROM_I2C, ENABLE);
    /*Write protect*/
    WriteProtect(0);
}

uint32_t I2C_EEPROM_ByteR(uint16_t Raddr)
{
    uint8_t data=0;
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(0);
    }
    /*START*/
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)//EV5
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(1);
    }
    /*DEVICE ADDRESS*/
    I2C_SendData(EEPROM_I2C, (EEPROM_ADDRESS|(Raddr>>7))&0xA6);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)//EV6
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(2);
    }
    /*WORD ADDRESS*/
    I2C_SendData(EEPROM_I2C, (uint8_t)Raddr);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)//EV8
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
    }
    /*START*/
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)//EV5
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(4);
    }
    /*ADEVICE DDRESS*/
    I2C_SendData(EEPROM_I2C, ((EEPROM_ADDRESS|(Raddr>>7))&0xA6)|0x01);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR)//EV6
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(5);
    }
    /*RECEIVERDATA*/
    I2C_AcknowledgeConfig(EEPROM_I2C, DISABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR)//EV7
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(6);
    }
    data = I2C_ReceiveData(EEPROM_I2C);
    /*STOP*/
    I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
    I2C_AcknowledgeConfig(EEPROM_I2C, ENABLE);
    return data;
}

uint32_t I2C_EEPROM_ByteW(uint16_t Waddr, uint8_t* pData)
{
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(7);
    }
    /*START*/
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)//EV5
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(8);
    }
    /*DEVICE ADDRESS*/
    I2C_SendData(EEPROM_I2C, (EEPROM_ADDRESS|(Waddr>>7))&0xA6);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)//EV6
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
    }
    /*WORD ADDRESS*/
    I2C_SendData(EEPROM_I2C, (uint8_t)Waddr);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)//EV8
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
    }
    /*SENDDATA*/
    I2C_SendData(EEPROM_I2C, *pData);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)//EV8_2
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(11);
    }
    /*STOP*/
    I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
    return 1;
}

uint32_t I2C_EEPROM_PageW(uint16_t Waddr, uint8_t* pData, uint8_t NumByte)
{
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(12);
    }
    /*START*/
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)//EV5
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(13);
    }
    /*DEVICE ADDRESS*/
    I2C_SendData(EEPROM_I2C, (EEPROM_ADDRESS|(Waddr>>7))&0xA6);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)//EV6
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(14);
    }
    /*WORD ADDRESS*/
    I2C_SendData(EEPROM_I2C, (uint8_t)Waddr);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)//EV8
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(15);
    }
    /*SENDDATA*/
    /* While there is data to be written */
    while(NumByte--)
    {
        /* Send the current byte */
        I2C_SendData(EEPROM_I2C, *pData);
        /* Point to the next byte to be written */
        pData++;
        I2CTimeout = I2CT_FLAG_TIMEOUT;
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(16);
        }
    }
    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(17);
    }
    /*STOP*/
    I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
    return 1;
}

uint32_t I2C_EEPROM_BufferR(uint16_t Raddr, uint8_t* pBuffer, uint16_t NumByte)
{
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    //*((u8 *)0x4001080c) |=0x80;
    while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(18);
    }
    /* Send START condition */
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    //*((u8 *)0x4001080c) &=~0x80;
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(19);
    }
    /* Send EEPROM address for write */
    I2C_SendData(EEPROM_I2C, (EEPROM_ADDRESS|(Raddr>>7))&0xA6);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(20);
    }
    /* Clear EV6 by setting again the PE bit */
//	I2C_Cmd(EEPROM_I2C, ENABLE);
    /* Send the EEPROM's internal address to write to */
    I2C_SendData(EEPROM_I2C, (uint8_t)Raddr);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(21);
    }
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(22);
    }
    /* Send EEPROM address for read */
    I2C_SendData(EEPROM_I2C, ((EEPROM_ADDRESS|(Raddr>>7))&0xA6)|0x01);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(23);
    }
    /* While there is data to be read */
    while(NumByte)
    {
        if(NumByte == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(EEPROM_I2C, DISABLE);
            /* Send STOP Condition */
            I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
        }
        /* Test on EV7 and clear it */
        I2CTimeout = I2CT_FLAG_TIMEOUT;
        while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)
        {
            if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(24);
        }
        /* Read a byte from the EEPROM */
        *pBuffer = I2C_ReceiveData(EEPROM_I2C);
        /* Point to the next location where the byte read will be saved */
        pBuffer++;
        /* Decrement the read bytes counter */
        NumByte--;
    }
    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(EEPROM_I2C, ENABLE);
    return 1;
}

void I2C_EEPROM_BufferW(uint16_t Waddr, uint8_t* pBuffer, uint16_t NumByte)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

    Addr = Waddr % I2C_PageSize;
    count = I2C_PageSize - Addr;
    NumOfPage =  NumByte / I2C_PageSize;
    NumOfSingle = NumByte % I2C_PageSize;

    /* If WriteAddr is I2C_PageSize aligned  */
    if(Addr == 0)
    {
        /* If NumByteToWrite < I2C_PageSize */
        if(NumOfPage == 0)
        {
            I2C_EEPROM_PageW(Waddr, pBuffer, NumOfSingle);
            EEPROM_ACK_POLL();
        }
        /* If NumByteToWrite > I2C_PageSize */
        else
        {
            while(NumOfPage--)
            {
                I2C_EEPROM_PageW(Waddr, pBuffer, I2C_PageSize);
                EEPROM_ACK_POLL();
                Waddr +=  I2C_PageSize;
                pBuffer += I2C_PageSize;
            }

            if(NumOfSingle!=0)
            {
                I2C_EEPROM_PageW(Waddr, pBuffer, NumOfSingle);
                EEPROM_ACK_POLL();
            }
        }
    }
    /* If WriteAddr is not I2C_PageSize aligned  */
    else
    {
        /* If NumByteToWrite < I2C_PageSize */
        if(NumOfPage== 0)
        {
            I2C_EEPROM_PageW(Waddr, pBuffer, NumOfSingle);
            EEPROM_ACK_POLL();
        }
        /* If NumByteToWrite > I2C_PageSize */
        else
        {
            NumByte -= count;
            NumOfPage =  NumByte / I2C_PageSize;
            NumOfSingle = NumByte % I2C_PageSize;
            if(count != 0)//写入没有aligned的数据
            {
                I2C_EEPROM_PageW(Waddr, pBuffer, count);
                EEPROM_ACK_POLL();
                Waddr += count;
                pBuffer += count;
            }
            while(NumOfPage--)
            {
                I2C_EEPROM_PageW(Waddr, pBuffer, I2C_PageSize);
                EEPROM_ACK_POLL();
                Waddr +=  I2C_PageSize;
                pBuffer += I2C_PageSize;
            }
            if(NumOfSingle != 0)
            {
                I2C_EEPROM_PageW(Waddr, pBuffer, NumOfSingle);
                EEPROM_ACK_POLL();
            }
        }
    }
}

uint32_t EEPROM_ACK_POLL(void)
{
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY))
    {
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(25);
    }
    do
    {
        /*START*/
        I2C_GenerateSTART(EEPROM_I2C, ENABLE);
        I2CTimeout = I2CT_FLAG_TIMEOUT;
        while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_SB) == RESET)//EV5
        {
            if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(26);
        }
        /*ADDRESS*/
        I2C_SendData(EEPROM_I2C, EEPROM_ADDRESS);
    }
    while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_ADDR) == RESET);//EV6
    /*STOP*/
    I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
    return 1;
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  errorCode：错误代码，可以用来定位是哪个环节出错.
  * @retval 返回0，表示IIC读取失败.
  */
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* Block communication and all processes */
    EEPROM_ERROR("I2C等待超时!errorCode = %d",errorCode);
    return 0;
}

/**
  * @brief  I2C(AT24C02)读写测试
  * @param  无
  * @retval 正常返回1，异常返回0
  */
uint8_t EEPROM_I2C_Test(void)
{
    uint16_t i;
    static uint8_t I2c_Buf_Write[ROM];
    static uint8_t I2c_Buf_Read[ROM];

    printf("写入的数据\n\r");
    for ( i=0; i<ROM; i++ ) //填充缓冲
    {
        I2c_Buf_Write[i] = i>>4;
        printf("0x%03X ", I2c_Buf_Write[i]);
        if(i%16 == 15)printf("\n\r");
    }
    //将I2c_Buf_Write中顺序递增的数据写入EERPOM中
    I2C_EEPROM_BufferW(EEP_Firstpage, I2c_Buf_Write, ROM);
    EEPROM_INFO("\n\r写成功\n\r");
    EEPROM_INFO("\n\r读出的数据\n\r");
    //将EEPROM读出数据顺序保持到I2c_Buf_Read中
    I2C_EEPROM_BufferR(EEP_Firstpage, I2c_Buf_Read, ROM);
    //将I2c_Buf_Read中的数据通过串口打印
    for (i=0; i<ROM; i++)
    {
        if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
        {
            EEPROM_ERROR("0x%03X ", I2c_Buf_Read[i]);
            EEPROM_ERROR("错误:I2C EEPROM写入与读出的数据不一致\n\r");
            return 0;
        }
        printf("0x%03X ", I2c_Buf_Read[i]);
        if(i%16 == 15)printf("\n\r");
    }
    EEPROM_INFO("I2C(AT24C02)读写测试成功\n\r");
    return 1;
}
