#include "bsp_si24r1.h"

static __IO uint32_t  SPITimeout = SPIT_FLAG_TIMEOUT;
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

uint8_t static TX_ADDRESS[TX_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x01};  // ����һ����̬���͵�ַ

static uint8_t SPI_RW(uint8_t byte)
{
    uint8_t bit_ctr;
    for(bit_ctr=0; bit_ctr<8; bit_ctr++)
        {
            if(byte & 0x80)
                SI24R1_MOSI_Set();	//MOSI = 1
            else
                SI24R1_MOSI_Clr();	//MOSI = 0
            byte <<= 1;
            SI24R1_SCK_Set();	//SCK = 1
            byte |= GPIO_ReadInputDataBit(SI24R1_GPIOA, SI24R1_MISO);	//byte |= MISO
            SI24R1_SCK_Clr();	//SCK = 0
        }
    return(byte);
}

/********************************************************
�������ܣ�SI24R1���ų�ʼ��
��ڲ�������
����  ֵ����
*********************************************************/
void SI24R1_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(SI24R1_GPIO_RCC, ENABLE);		//ʹ��GPIO�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = SI24R1_MOSI|SI24R1_SCK|SI24R1_CSN|SI24R1_CE;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//�ٶ�10MHz
    GPIO_Init(SI24R1_GPIOA, &GPIO_InitStructure);			//��ʼ��GPIOA
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
    GPIO_InitStructure.GPIO_Pin = SI24R1_MISO;
    GPIO_Init(SI24R1_GPIOA, &GPIO_InitStructure);	  		//��ʼ��GPIOA
    GPIO_InitStructure.GPIO_Pin = SI24R1_IRQ;
    GPIO_Init(SI24R1_GPIOB, &GPIO_InitStructure);	  		//��ʼ��GPIOB
    SI24R1_SCK_Clr();	//SPIʱ�������� SCK = 0
    SI24R1_CSN_Set();	//CSN = 1
    SI24R1_CE_Clr();	//CE = 0
    SI24R1_MOSI_Clr();	//MOSI = 0
}

/********************************************************
�������ܣ�д�Ĵ�����ֵ�����ֽڣ�
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��WRITE_REG��reg��
		 value:�Ĵ�����ֵ
����  ֵ��״̬�Ĵ�����ֵ
*********************************************************/
uint8_t SI24R1_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    SI24R1_CSN_Clr();	//CSN = 0
    status = SPI_RW(reg);
    SPI_RW(value);
    SI24R1_CSN_Set();	//CSN = 1
    return(status);
}

/********************************************************
�������ܣ�д�Ĵ�����ֵ�����ֽڣ�
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��WRITE_REG��reg��
		 pBuf:д�����׵�ַ
		 bytes:д�����ֽ���
����  ֵ��״̬�Ĵ�����ֵ
*********************************************************/
uint8_t SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status,byte_ctr;
    SI24R1_CSN_Clr();	//CSN = 0
    status = SPI_RW(reg);
    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)
        SPI_RW(*pBuf++);
    SI24R1_CSN_Set();	//CSN = 1
    return(status);
}

/********************************************************
�������ܣ���ȡ�Ĵ�����ֵ�����ֽڣ�
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��READ_REG��reg��
����  ֵ���Ĵ���ֵ
*********************************************************/
uint8_t SI24R1_Read_Reg(uint8_t reg)
{
    uint8_t value;
    SI24R1_CSN_Clr();	//CSN = 0
    SPI_RW(reg);
    value = SPI_RW(0);
    SI24R1_CSN_Set();	//CSN = 1
    return(value);
}

/********************************************************
�������ܣ���ȡ�Ĵ�����ֵ�����ֽڣ�
��ڲ�����reg:�Ĵ���ӳ���ַ��READ_REG��reg��
					pBuf:���ջ��������׵�ַ
					bytes:��ȡ�ֽ���
����  ֵ��״̬�Ĵ�����ֵ
*********************************************************/
uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status,byte_ctr;
    SI24R1_CSN_Clr();	//CSN = 0
    status = SPI_RW(reg);
    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)
        pBuf[byte_ctr] = SPI_RW(0);	//��ȡ���ݣ����ֽ���ǰ
    SI24R1_CSN_Set();	//CSN = 1
    return(status);
}

/********************************************************
�������ܣ�SI24R1����ģʽ��ʼ��
��ڲ�����SI24R1_InitTypeDef
		 Mode RX_MODE or TX_MODE
����  ֵ����
*********************************************************/
void SI24R1_Mode_NOACK(SI24R1_InitTypeDef* SI24R1_InitStruct,uint8_t Mode)
{
    //CE = 0
    SI24R1_CE_Clr();
    // ���õ�ַ���Ϊ5bytes
    SI24R1_Write_Reg(W_REGISTER + SETUP_AW, SI24R1_InitStruct->ADR_WIDTH);
    if(Mode == RX_MODE)
        {
            // ����ʹ�ý���ͨ��0�ͽ��յ�ַ
            SI24R1_Write_Buf(W_REGISTER + SI24R1_InitStruct->RX_ADDR_Px, TX_ADDRESS, TX_ADR_WIDTH);
            // ʹ�ܽ���ͨ��0
            SI24R1_Write_Reg(W_REGISTER + EN_RXADDR, SI24R1_InitStruct->EN_RXADDR_Px);
            // ���ý���ͨ��0�������ݿ��
            SI24R1_Write_Reg(W_REGISTER + SI24R1_InitStruct->RX_PW_Px, TX_PLOAD_WIDTH);
        }
    else
        {
            // д�뷢�͵�ַ
            SI24R1_Write_Buf(W_REGISTER + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
            // ʹ��NOACK
            SI24R1_Write_Reg(W_REGISTER + FEATURE, SI24R1_InitStruct->RF_FEATURE);
        }
    // ѡ����Ƶͨ��40
    SI24R1_Write_Reg(W_REGISTER + RF_CH, SI24R1_InitStruct->RF_CHANNEL);
    // ���ݴ�����2Mbps�����书��7dBm
    SI24R1_Write_Reg(W_REGISTER + RF_SETUP, SI24R1_InitStruct->RF_SET);
    if(Mode == RX_MODE)
        // CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
        SI24R1_Write_Reg(W_REGISTER + CONFIG, (SI24R1_InitStruct->SI24R1_CONFIG)|0x01);
    else
        // CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
        SI24R1_Write_Reg(W_REGISTER + CONFIG, (SI24R1_InitStruct->SI24R1_CONFIG)&0xfe);
    // ����CE���������豸
    SI24R1_CE_Set();
#if SI24R1_debug
    if(Mode == RX_MODE)
        printf("\nSI24R1 RXinitialze success\n");
    else
        printf("\nSI24R1 TXinitialze success\n");
#endif
}

/********************************************************
�������ܣ���ȡ��������
��ڲ�����rxbuf:�������ݴ���׵�ַ
����  ֵ��1:���յ�����
         0:û�н��յ�����
*********************************************************/
uint8_t SI24R1_RxPacket(uint8_t *rxbuf)
{
    uint8_t state;
    uint8_t pload_width;
    state = SI24R1_Read_Reg(STATUS);  			      	// ��ȡ״̬�Ĵ�����ֵ
    SI24R1_Write_Reg(W_REGISTER + STATUS,state);        // ���RX_DR�жϱ�־
    if(state & RX_DR)								    // ���յ�����
        {
            pload_width = SI24R1_Read_Reg(R_RX_PL_WID);		// ��ȡ�յ��������ֽ���
#if SI24R1_debug
            printf("\nSI24R1 RX:0x%X\n",state);
            printf("\nSI24R1 Readpload_width��%d\n",pload_width);
#endif
            SI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,pload_width);	// ��ȡ����
            SI24R1_Write_Reg(FLUSH_RX,0xff);				// ���RX FIFO�Ĵ���
            return 1;
        }
    return 0;                                         	// û�յ��κ�����
}

/********************************************************
�������ܣ�����һ�����ݰ�
��ڲ�����txbuf:Ҫ���͵�����
����  ֵ��0x10:�ﵽ����ط�����������ʧ��
         0x20:���ͳɹ�
         0xff:����ʧ��
*********************************************************/
uint8_t SI24R1_TxPacket(uint8_t *txbuf)
{
    uint8_t state;
    SI24R1_CE_Clr();										// CE���ͣ�ʹ��SI24R1����
    SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	// д���ݵ�TX FIFO,32���ֽ�
    SI24R1_CE_Set();										// CE�øߣ�ʹ�ܷ���
    SPITimeout = SPIT_FLAG_TIMEOUT;
    while(GPIO_ReadInputDataBit(SI24R1_GPIOB, SI24R1_IRQ))	// �ȴ��������
        {
            if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
        }
    state = SI24R1_Read_Reg(STATUS);  						// ��ȡ״̬�Ĵ�����ֵ
    SI24R1_Write_Reg(W_REGISTER + STATUS, state); 			// ���TX_DS��MAX_RT�жϱ�־
    if(state & MAX_RT)										// �ﵽ����ط�����
        {
            SI24R1_Write_Reg(FLUSH_TX,0xff);					// ���TX FIFO�Ĵ���
#if SI24R1_debug
            printf("\nSI24R1 TX:0x%X\n",MAX_RT);
#endif
            return MAX_RT;
        }
    if(state & TX_DS)										// �������
        {
#if SI24R1_debug
            printf("\nSI24R1 TX:0x%X\n",TX_DS);
#endif
            return TX_DS;
        }
#if SI24R1_debug
    printf("\nSI24R1 TX:0x%X\n",state);
#endif
    return state;											//����ʧ��
}

void SI24R1_Shutdown(void)
{
    SI24R1_Write_Reg(W_REGISTER + CONFIG, 0x0d);
}

void SI24R1_Standby(void)
{
    SI24R1_Write_Reg(W_REGISTER + CONFIG, 0x0f);
}

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* �ȴ���ʱ��Ĵ���,���������Ϣ */
    SI24R1_ERROR("SI24R1 �ȴ���ʱ!errorCode = %d",errorCode);
    return 0;
}
