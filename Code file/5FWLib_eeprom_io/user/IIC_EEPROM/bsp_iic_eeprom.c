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
            goto cmd_fail;	/* EEPROM������Ӧ�� */
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
            goto cmd_fail;	/* EEPROM������Ӧ�� */
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
            goto cmd_fail;	/* EEPROM������Ӧ�� */
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
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    printf("\nEEPROM READ ERROR\n");
    i2c_Stop();
    return 0;
}

uint8_t I2C_EEPROM_ByteW(uint16_t Waddr, uint8_t* pBuffer, uint16_t NumByte)
{
    uint16_t i;
    uint16_t usAddr;
    /*
    	д����EEPROM�������������������ȡ�ܶ��ֽڣ�ÿ��д����ֻ����ͬһ��page��
    	����24xx08��page size = 16
    	�򵥵Ĵ�����Ϊ�����ֽ�д����ģʽ��ûд1���ֽڣ������͵�ַ
    	Ϊ���������д��Ч��: ����������page wirte������
    */
    usAddr = Waddr;
    for (i = 0; i < NumByte; i++)
    {
        /* �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */
        if ((i == 0) || ((usAddr & (EEPROM_PageSize - 1)) == 0))
        {
            /*����ֹͣ�źţ������ڲ�д������*/
            i2c_Stop();
            /* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ���� */
            I2CTimeout = I2CT_FLAG_TIMEOUT;
            do
            {
                /* ����I2C���������ź� */
                i2c_Start();
                /* ��������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
                i2c_SendByte((EEPROM_ADDRESS|(usAddr>>7))&0xA6);	/* �˴���дָ�� */
                /* �ж������Ƿ���ȷӦ�� */
                I2CTimeout--;
                if(I2CTimeout == 0)
                {
                    I2C_TIMEOUT_UserCallback(4);
                    goto cmd_fail;	/* EEPROM����д��ʱ */
                }
            }
            while(i2c_WaitAck());
            /* �����ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ�ο�datasheet*/
            i2c_SendByte((uint8_t)usAddr);
            /* �ȴ�ACK */
            I2CTimeout = I2CT_FLAG_TIMEOUT;
            while(i2c_WaitAck())
            {
                I2CTimeout--;
                if(I2CTimeout == 0)
                {
                    I2C_TIMEOUT_UserCallback(5);
                    goto cmd_fail;	/* EEPROM������Ӧ�� */
                }
            }
        }
        /* ��ʼд������ */
        i2c_SendByte(pBuffer[i]);
        /* ����ACK */
        I2CTimeout = I2CT_FLAG_TIMEOUT;
        while(i2c_WaitAck())
        {
            I2CTimeout--;
            if(I2CTimeout == 0)
            {
                I2C_TIMEOUT_UserCallback(6);
                goto cmd_fail;	/* EEPROM������Ӧ�� */
            }
        }
        usAddr++;	/* ��ַ��1 */
    }
    /* ����ִ�гɹ�������I2C����ֹͣ�ź� */
    i2c_Stop();
    return 1;

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    printf("\nEEPROM WRITE ERROR\n");
    i2c_Stop();
    return 0;
}

uint8_t EEPROM_ACK_POLL(void)
{
    uint8_t poll=0;
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while(I2CTimeout--)		/* �������æ */
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
        /* û�м�⵽EEPROM */
        printf("\n����æ!\r\n");
        return;
    }
    else printf("\n��⵽����EEPROM!\n");
    /* ��仺���� */
    for (i = 0; i < ROM; i++)
    {
        buf[i] = 0xff;
    }
    /* дEEPROM, ��ʼ��ַ = 0�����ݳ���Ϊ 256 */
    if (I2C_EEPROM_ByteW(EEP_Firstpage, buf, ROM) == 0)
    {
        printf("\n����eeprom����\r\n");
        return;
    }
    else
    {
        printf("\n����eeprom�ɹ���\r\n");
    }
}

/*
 * eeprom AT24C08 ��д����
 * ��������1���쳣����0
 */
uint8_t EEPROM_Test(void)
{
    uint16_t i;
    static uint8_t write_buf[ROM];
    static uint8_t read_buf[ROM];

    /*-----------------------------------------------------------------------------------*/
    if(EEPROM_ACK_POLL() == 0)
    {
        /* û�м�⵽EEPROM */
        printf("\n����æ!\r\n");
        return 0;
    }
    else printf("\n��⵽����EEPROM!\n");
    /*------------------------------------------------------------------------------------*/
    /* �����Ի����� */
    for(i = 0; i < ROM; i++)
    {
        write_buf[i] = (i>>0);
    }
    /*------------------------------------------------------------------------------------*/
    if (I2C_EEPROM_ByteW(EEP_Firstpage, write_buf, ROM) == 0)
    {
        printf("\nдeeprom����\r\n");
        return 0;
    }
    else
    {
        printf("\nдeeprom�ɹ���\r\n");
    }
    /*д��֮����ʱ����һֱѯ����ȥ������Ȼ�����*/
    EEPROM_ACK_POLL();
    /*-----------------------------------------------------------------------------------*/
    if (I2C_EEPROM_ByteR(EEP_Firstpage, read_buf, ROM) == 0)
    {
        printf("\n��eeprom����\r\n");
        return 0;
    }
    else
    {
        printf("\n��eeprom�ɹ����������£�\r\n");
    }
    /*-----------------------------------------------------------------------------------*/
    for (i = 0; i < ROM; i++)
    {
        if(read_buf[i] != write_buf[i])
        {
            printf("\n0x%03X ", read_buf[i]);
            printf("����:EEPROM������д������ݲ�һ��");
            return 0;
        }
        printf(" %03X", read_buf[i]);
        if ((i & 15) == 15)
        {
            printf("\r\n");
        }
    }
    printf("\neeprom��д���Գɹ�\r\n");
    return 1;
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  errorCode��������룬����������λ���ĸ����ڳ���.
  * @retval ����0����ʾIIC��ȡʧ��.
  */
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* Block communication and all processes */
    EEPROM_ERROR("\nI2C�ȴ���ʱ!errorCode = %d",errorCode);
    return 0;
}
