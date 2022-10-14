#include "bsp_iic_eeprom.h"
#include "bsp_iic_io.h"

static __IO uint32_t  I2CTimeout = I2CT_FLAG_TIMEOUT;
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

uint8_t I2C_EEPROM_ByteR(uint16_t Raddr, uint8_t* pBuffer, uint16_t NumByte)
{
    /* START */
    i2c_Start();
    /* Send EEPROM address for write */
    i2c_SendByte((EEPROM_ADDRESS|(Raddr>>7))&0xA6);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(1);
            goto cmd_fail;	/* EEPROM器件无应答 */
        }
    }
    /* Send the EEPROM's internal address to write to */
    i2c_SendByte((uint8_t)Raddr);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(i2c_WaitAck())
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(2);
            goto cmd_fail;	/* EEPROM器件无应答 */
        }
    }
    /* STRAT */
    i2c_Start();
    /* Send EEPROM address for read */
    i2c_SendByte(((EEPROM_ADDRESS|(Raddr>>7))&0xA6)|EEPROM_I2C_RD);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(i2c_WaitAck() != 0)
    {
        I2CTimeout--;
        if(I2CTimeout == 0)
        {
            I2C_TIMEOUT_UserCallback(3);
            goto cmd_fail;	/* EEPROM器件无应答 */
        }
    }
    /* While there is data to be read */
    while(NumByte)
    {
        /* Read a byte from the EEPROM */
        *pBuffer = i2c_ReadByte();
        /* Point to the next location where the byte read will be saved */
        pBuffer++;
        if(NumByte == 1)
        {
            /* NOACK */
            i2c_NAck();
            /* STOP */
            i2c_Stop();
        }
        else
        {
            i2c_Ack();
        }
        /* Decrement the read bytes counter */
        NumByte--;
    }
    return 1;
cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    printf("\nEEPROM READ ERROR\n");
    i2c_Stop();
    return 0;
}

uint8_t I2C_EEPROM_ByteW(uint16_t Waddr, uint8_t* pBuffer, uint16_t NumByte)
{
    uint16_t i;
    uint16_t usAddr;
    /*
    	写串行EEPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。
    	对于24xx08，page size = 16
    	简单的处理方法为：按字节写操作模式，没写1个字节，都发送地址
    	为了提高连续写的效率: 本函数采用page wirte操作。
    */
    usAddr = Waddr;
    for (i = 0; i < NumByte; i++)
    {
        /* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */
        if ((i == 0) || ((usAddr & (EEPROM_PageSize - 1)) == 0))
        {
            /*　发停止信号，启动内部写操作　*/
            i2c_Stop();
            /* 通过检查器件应答的方式，判断内部写操作是否完成 */
            I2CTimeout = I2CT_FLAG_TIMEOUT;
            do
            {
                /* 发起I2C总线启动信号 */
                i2c_Start();
                /* 发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
                i2c_SendByte((EEPROM_ADDRESS|(usAddr>>7))&0xA6);	/* 此处是写指令 */
                /* 判断器件是否正确应答 */
                I2CTimeout--;
                if(I2CTimeout == 0)
                {
                    I2C_TIMEOUT_UserCallback(4);
                    goto cmd_fail;	/* EEPROM器件写超时 */
                }
            }
            while(i2c_WaitAck());
            /* 发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要参考datasheet*/
            i2c_SendByte((uint8_t)usAddr);
            /* 等待ACK */
            I2CTimeout = I2CT_FLAG_TIMEOUT;
            while(i2c_WaitAck())
            {
                I2CTimeout--;
                if(I2CTimeout == 0)
                {
                    I2C_TIMEOUT_UserCallback(5);
                    goto cmd_fail;	/* EEPROM器件无应答 */
                }
            }
        }
        /* 开始写入数据 */
        i2c_SendByte(pBuffer[i]);
        /* 发送ACK */
        I2CTimeout = I2CT_FLAG_TIMEOUT;
        while(i2c_WaitAck())
        {
            I2CTimeout--;
            if(I2CTimeout == 0)
            {
                I2C_TIMEOUT_UserCallback(6);
                goto cmd_fail;	/* EEPROM器件无应答 */
            }
        }
        usAddr++;	/* 地址增1 */
    }
    /* 命令执行成功，发送I2C总线停止信号 */
    i2c_Stop();
    return 1;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    printf("\nEEPROM WRITE ERROR\n");
    i2c_Stop();
    return 0;
}

uint8_t EEPROM_ACK_POLL(void)
{
    uint8_t poll=0;
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2CTimeout--)		/* 检测总线忙 */
    {
        if((EEPROM_I2C_SDA_READ() || EEPROM_I2C_SCL_READ()) == 0)
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

void EEPROM_Erase(void)
{
    uint16_t i;
    static uint8_t buf[ROM];

    if(EEPROM_ACK_POLL() == 0)
    {
        /* 没有检测到EEPROM */
        printf("\n总线忙!\r\n");
        return;
    }
    else printf("\n检测到串行EEPROM!\n");
    /* 填充缓冲区 */
    for (i = 0; i < ROM; i++)
    {
        buf[i] = 0xff;
    }
    /* 写EEPROM, 起始地址 = 0，数据长度为 256 */
    if (I2C_EEPROM_ByteW(EEP_Firstpage, buf, ROM) == 0)
    {
        printf("\n擦除eeprom出错！\r\n");
        return;
    }
    else
    {
        printf("\n擦除eeprom成功！\r\n");
    }
}

/*
 * eeprom AT24C08 读写测试
 * 正常返回1，异常返回0
 */
uint8_t EEPROM_Test(void)
{
    uint16_t i;
    static uint8_t write_buf[ROM];
    static uint8_t read_buf[ROM];

    /*-----------------------------------------------------------------------------------*/
    if(EEPROM_ACK_POLL() == 0)
    {
        /* 没有检测到EEPROM */
        printf("\n总线忙!\r\n");
        return 0;
    }
    else printf("\n检测到串行EEPROM!\n");
    /*------------------------------------------------------------------------------------*/
    /* 填充测试缓冲区 */
    for(i = 0; i < ROM; i++)
    {
        write_buf[i] = (i>>0);
    }
    /*------------------------------------------------------------------------------------*/
    if (I2C_EEPROM_ByteW(EEP_Firstpage, write_buf, ROM) == 0)
    {
        printf("\n写eeprom出错！\r\n");
        return 0;
    }
    else
    {
        printf("\n写eeprom成功！\r\n");
    }
    /*写完之后延时或者一直询问再去读，不然会出错*/
    EEPROM_ACK_POLL();
    /*-----------------------------------------------------------------------------------*/
    if (I2C_EEPROM_ByteR(EEP_Firstpage, read_buf, ROM) == 0)
    {
        printf("\n读eeprom出错！\r\n");
        return 0;
    }
    else
    {
        printf("\n读eeprom成功，数据如下：\r\n");
    }
    /*-----------------------------------------------------------------------------------*/
    for (i = 0; i < ROM; i++)
    {
        if(read_buf[i] != write_buf[i])
        {
            printf("\n0x%03X ", read_buf[i]);
            printf("错误:EEPROM读出与写入的数据不一致");
            return 0;
        }
        printf(" %03X", read_buf[i]);
        if ((i & 15) == 15)
        {
            printf("\r\n");
        }
    }
    printf("\neeprom读写测试成功\r\n");
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
    EEPROM_ERROR("\nI2C等待超时!errorCode = %d",errorCode);
    return 0;
}
