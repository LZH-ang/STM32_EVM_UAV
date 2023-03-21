#include "bsp_oled.h"
#include "oledfont.h"


static __IO uint32_t  SPITimeout = SPIT_FLAG_TIMEOUT;
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

uint8_t OLED_GRAM[144][8];

//���Ժ���
uint8_t OLED_ColorTurn(uint8_t mode)
{
    if(mode==0)
    {
        OLED_WR_Byte(0xA6,OLED_CMD);//������ʾ
    }
    if(mode==1)
    {
        OLED_WR_Byte(0xA7,OLED_CMD);//��ɫ��ʾ
    }
	return 0;
}

//��Ļ��ת180��
uint8_t OLED_DisplayTurn(uint8_t mode)
{
    if(mode==0)
    {
        OLED_WR_Byte(0xC8,OLED_CMD);//������ʾ
        OLED_WR_Byte(0xA1,OLED_CMD);
    }
    if(mode==1)
    {
        OLED_WR_Byte(0xC0,OLED_CMD);//��ת��ʾ
        OLED_WR_Byte(0xA0,OLED_CMD);
    }
	return 0;
}

uint8_t OLED_WR_Byte(uint8_t dat,uint8_t mode)
{
    if(mode)
        OLED_DC_Set();
    else
        OLED_DC_Clr();
	OLED_CS_Clr();
	SPITimeout = SPIT_FLAG_TIMEOUT;
    /* �ȴ����ͻ�����Ϊ�գ�TXE�¼� */
    while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
    }
	SPI_I2S_ClearFlag(OLED_SPI, SPI_I2S_FLAG_TXE);
	SPI_I2S_SendData(OLED_SPI, dat);
	SPITimeout = SPIT_FLAG_TIMEOUT;
    /* �ȴ����ͻ�����Ϊ�գ�TXE�¼� */
    while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
    }
	SPI_I2S_ClearFlag(OLED_SPI, SPI_I2S_FLAG_TXE);
	/* �ȴ������꣬BSY�¼� */
    while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_BSY) == SET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(2);
    }
	OLED_CS_Set();
    OLED_DC_Set();
	return 0;
}


//����OLED��ʾ
uint8_t OLED_DisPlay_On(void)
{
    OLED_WR_Byte(0xAF,OLED_CMD);//������Ļ
    OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
    OLED_WR_Byte(0x14,OLED_CMD);//������ɱ�
	return 0;
}

//�ر�OLED��ʾ
uint8_t OLED_DisPlay_Off(void)
{
    OLED_WR_Byte(0xAE,OLED_CMD);//�ر���Ļ
    OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
    OLED_WR_Byte(0x10,OLED_CMD);//�رյ�ɱ�
	return 0;
}

//�����Դ浽OLED
uint8_t OLED_Refresh(void)
{
    uint8_t i,n;
    for(i=0; i<8; i++)
    {
        OLED_WR_Byte(0xb0+i,OLED_CMD); //��������ʼ��ַ
        OLED_WR_Byte(0x02,OLED_CMD);   //���õ�����ʼ��ַ
        OLED_WR_Byte(0x10,OLED_CMD);   //���ø�����ʼ��ַ
        for(n=0; n<128; n++)
            OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
    }
	return 0;
}

//��������
void OLED_Clear(void)
{
    memset(OLED_GRAM,0,144*8*sizeof(uint8_t));
    OLED_Refresh();//������ʾ
}

//OLED�ĳ�ʼ��
uint8_t OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI2_InitStructure;
	
    RCC_APB2PeriphClockCmd(OLEDGPIO_RCC, ENABLE);		//ʹ��GPIO�˿�ʱ��
	RCC_APB1PeriphClockCmd(OLEDSPI_RCC, ENABLE);		//ʹ��SPI2�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = OLED_RES|OLED_DC;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
    GPIO_Init(OLED_GPIOA, &GPIO_InitStructure);	  		//��ʼ��GPIOA
    GPIO_InitStructure.GPIO_Pin = OLED_SCK|OLED_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
    GPIO_Init(OLED_GPIOB, &GPIO_InitStructure);	  		//��ʼ��GPIOB
	GPIO_InitStructure.GPIO_Pin = OLED_NSS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
    GPIO_Init(OLED_GPIOB, &GPIO_InitStructure);	  		//��ʼ��GPIOB
	
	SPI2_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI2_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI2_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI2_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI2_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI2_InitStructure.SPI_NSS = SPI_NSS_Soft;
//	SPI2_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI2_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI2_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI2_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(OLED_SPI, &SPI2_InitStructure);
    GPIO_SetBits(OLED_GPIOA,OLED_RES|OLED_DC);
    GPIO_SetBits(OLED_GPIOB,OLED_NSS);
	SPI_Cmd(OLED_SPI, ENABLE);

    OLED_RES_Clr();
    SysTick_Delay_ms(100);
    OLED_RES_Set();
    OLED_WR_Byte(0xAE,OLED_CMD); /* Display OFF/ON:(AEH - AFH) */
    OLED_WR_Byte(0xB0,OLED_CMD); /* Set Page Address:(B0H - B7H) */
    OLED_WR_Byte(0x02,OLED_CMD); /* Set Lower Column Address:(00H - 0FH) */
    OLED_WR_Byte(0x10,OLED_CMD); /* Set Higher Column Address:(10H - 1FH) */
    OLED_WR_Byte(0xD5,OLED_CMD); /* Divide Ratio/Oscillator Frequency Mode Set:(D5H) */
    OLED_WR_Byte(0x80,OLED_CMD); /* Divide Ratio/Oscillator Frequency Data Set:(00H - 3FH) */
    OLED_WR_Byte(0xA8,OLED_CMD); /* Multiplex Ration Mode Set:(A8H) */
    OLED_WR_Byte(0x3F,OLED_CMD); /* Multiplex Ration Data Set:(00H - 3FH) */
    OLED_WR_Byte(0xD3,OLED_CMD); /* Display Offset Mode Set:(D3H) */
    OLED_WR_Byte(0x00,OLED_CMD); /* Display Offset Data Set:(00H~3FH) */
    OLED_WR_Byte(0x40,OLED_CMD); /* Set Display Start Line:(40H - 7FH) */
    OLED_WR_Byte(0xAD,OLED_CMD); /* DC-DC Control Mode Set:(ADH) */
    OLED_WR_Byte(0x8B,OLED_CMD); /* DC-DC ON/OFF Mode Set:(8AH - 8BH)  */\
    OLED_WR_Byte(0xA1,OLED_CMD); /* Set Segment Re-map:(A0H - A1H) */
    OLED_WR_Byte(0xC8,OLED_CMD); /* Set Common Output Scan Direction:(C0H - C8H) */
    OLED_WR_Byte(0xDA,OLED_CMD); /* Common Pads Hardware Configuration Mode Set:(DAH) */
    OLED_WR_Byte(0x12,OLED_CMD); /* Sequential/Alternative Mode Set:(02H - 12H) */
    OLED_WR_Byte(0x81,OLED_CMD); /* The Contrast Control Mode Set:(81H) */
    OLED_WR_Byte(0xFF,OLED_CMD); /* Contrast Data Register Set:(00H - FFH) */
    OLED_WR_Byte(0xD9,OLED_CMD); /* Pre-charge Period Mode Set:(D9H) */
    OLED_WR_Byte(0x1F,OLED_CMD); /* Dis-charge/Pre-charge Period Data Set:(00H - FFH) */
    OLED_WR_Byte(0xDB,OLED_CMD); /* VCOM Deselect Level Mode Set:(DBH) */
    OLED_WR_Byte(0x40,OLED_CMD); /* VCOM Deselect Level Data Set:(00H - FFH) */
    OLED_WR_Byte(0x33,OLED_CMD); /* Set Pump voltage value: (30H~33H) */
    OLED_WR_Byte(0xA6,OLED_CMD); /* Set Normal/Reverse Display:(A6H -A7H) */
    OLED_Clear();
    SysTick_Delay_ms(100);
    OLED_WR_Byte(0xAF,OLED_CMD); /* Display OFF/ON:(AEH - AFH) */
    SysTick_Delay_ms(100);
	return 0;
}

//����
//x:0~127
//y:0~63
//t:1 ��� 0,���
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
    uint8_t i,m,n;
    i=y/8;
    m=y%8;
    n=1<<m;
    if(t) 
	{
        OLED_GRAM[x][i]|=n;
    }
    else
    {
        OLED_GRAM[x][i]=~OLED_GRAM[x][i];
        OLED_GRAM[x][i]|=n;
        OLED_GRAM[x][i]=~OLED_GRAM[x][i];
    }
}

//����
//x1,y1:�������
//x2,y2:��������
void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode)
{
    uint16_t t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //������������
    delta_y=y2-y1;
    uRow=x1;//�����������
    uCol=y1;
    if(delta_x>0)incx=1; //���õ�������
    else if (delta_x==0)incx=0;//��ֱ��
    else {
        incx=-1;
        delta_x=-delta_x;
    }
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//ˮƽ��
    else {
        incy=-1;
        delta_y=-delta_x;
    }
    if(delta_x>delta_y)distance=delta_x; //ѡȡ��������������
    else distance=delta_y;
    for(t=0; t<distance+1; t++)
    {
        OLED_DrawPoint(uRow,uCol,mode);//����
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}
//x,y:Բ������
//r:Բ�İ뾶
void OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r)
{
    int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);

        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);

        a++;
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//size1:ѡ������ 6x8/6x12/8x16/12x24
//mode:0,������ʾ;1,��ɫ��ʾ
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode)
{
    uint8_t i,m,temp,size2,chr1;
    uint8_t x0=x,y0=y;
    if(size1==8)size2=6;
    else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    chr1=chr-' ';  //����ƫ�ƺ��ֵ
    for(i=0; i<size2; i++)	//���������
    {
        if(size1==8)
        {
            temp=asc2_0806[chr1][i];   //����0806����
        }
        else if(size1==12)
        {
            temp=asc2_1206[chr1][i];   //����1206����
        }
        else if(size1==16)
        {
            temp=asc2_1608[chr1][i];   //����1608����
        }
        else if(size1==24)
        {
            temp=asc2_2412[chr1][i];   //����2412����
        }
        else return;
        for(m=0; m<8; m++)
        {
            if(temp&0x01)OLED_DrawPoint(x,y,!mode);
            else OLED_DrawPoint(x,y,mode);
            temp>>=1;
            y++;
        }
        x++;
        if((size1!=8)&&((x-x0)==size1/2))
        {
            x=x0;
            y0=y0+8;
        }
        y=y0;
    }
}


//��ʾ�ַ���
//x,y:�������
//size1:�����С
//*chr:�ַ�����ʼ��ַ
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode)
{
    while((*chr>=' ')&&(*chr<='~'))//�ж��ǲ��ǷǷ��ַ�!
    {
        OLED_ShowChar(x,y,*chr,size1,mode);
        if(size1==8)x+=6;
        else x+=size1/2;
        chr++;
    }
}

//m^n
uint32_t OLED_Pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)
    {
        result*=m;
    }
    return result;
}

//��ʾ����
//x,y :�������
//num :Ҫ��ʾ������
//len :���ֵ�λ��
//size:�����С
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode)
{
    uint8_t t,temp,m=0;
    if(size1==8)m=2;
    for(t=0; t<len; t++)
    {
        temp=(num/OLED_Pow(10,len-t-1))%10;
        if(temp==0)
        {
            OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
        }
        else
        {
            OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
        }
    }
}

//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
//mode:0,������ʾ;1,��ɫ��ʾ
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1,uint8_t mode)
{
    uint8_t m,temp;
    uint8_t x0=x,y0=y;
    uint16_t i,size3=(size1/8+((size1%8)?1:0))*size1;  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    for(i=0; i<size3; i++)
    {
        if(size1==16)
        {
//			temp=Hzk1[num][i];   //����16*16����
        }
        else if(size1==24)
        {
//            temp=Hzk2[num][i];   //����24*24����
        }
        else if(size1==32)
        {
//            temp=Hzk3[num][i];   //����32*32����
        }
        else if(size1==64)
        {
//            temp=Hzk4[num][i];   //����64*64����
        }
/*
        else if(size1==??)
        {
            temp=??;   //����??����
        }		
*/
        else return;
        for(m=0; m<8; m++)
        {
            if(temp&0x01)OLED_DrawPoint(x,y,!mode);
            else OLED_DrawPoint(x,y,mode);
            temp>>=1;
            y++;
        }
        x++;
        if((x-x0)==size1)
        {
            x=x0;
            y0=y0+8;
        }
        y=y0;
    }
}

//num ��ʾ���ֵĸ���
//space ÿһ����ʾ�ļ��
//mode:0,������ʾ;1,��ɫ��ʾ
void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode)
{
    uint8_t i,n,t=0,m=0,r;
    while(1)
    {
        if(m==0)
        {
            OLED_ShowChinese(128,24,t,16,mode); //д��һ�����ֱ�����OLED_GRAM[][]������
            t++;
        }
        if(t==num)
        {
            for(r=0; r<16*space; r++)    //��ʾ���
            {
                for(i=1; i<144; i++)
                {
                    for(n=0; n<8; n++)
                    {
                        OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
                    }
                }
                OLED_Refresh();
            }
            t=0;
        }
        m++;
        if(m==16) {
            m=0;
        }
        for(i=1; i<144; i++) //ʵ������
        {
            for(n=0; n<8; n++)
            {
                OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
            }
        }
        OLED_Refresh();
    }
}

//x,y���������
//sizex,sizey,ͼƬ����
//BMP[]��Ҫд���ͼƬ����
//mode:0,������ʾ;1,��ɫ��ʾ
void OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode)
{
    uint16_t j=0;
    uint8_t i,n,temp,m;
    uint8_t x0=x,y0=y;
    sizey=sizey/8+((sizey%8)?1:0);
    for(n=0; n<sizey; n++)//ҳд��
    {
        for(i=0; i<sizex; i++)//��д��
        {
            temp=BMP[j];
            j++;
            for(m=0; m<8; m++)//һ��8����
            {
                if(temp&0x01)OLED_DrawPoint(x,y,!mode);
                else OLED_DrawPoint(x,y,mode);
                temp>>=1;
                y++;
            }
            x++;
            if((x-x0)==sizex)//ҳд�꣬��һҳ
            {
                x=x0;
                y0=y0+8;	
            }
			y=y0;   
        }
    }
}

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* �ȴ���ʱ��Ĵ���,���������Ϣ */
    OLED_ERROR("SPI �ȴ���ʱ!errorCode = %d",errorCode);
    return 0;
}
