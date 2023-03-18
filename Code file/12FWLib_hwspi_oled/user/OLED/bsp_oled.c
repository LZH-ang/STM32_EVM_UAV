#include "bsp_oled.h"
#include "oledfont.h"


static __IO uint32_t  SPITimeout = SPIT_FLAG_TIMEOUT;
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

uint8_t OLED_GRAM[144][8];

//反显函数
uint8_t OLED_ColorTurn(uint8_t mode)
{
    if(mode==0)
    {
        OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
    }
    if(mode==1)
    {
        OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
    }
	return 0;
}

//屏幕旋转180度
uint8_t OLED_DisplayTurn(uint8_t mode)
{
    if(mode==0)
    {
        OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
        OLED_WR_Byte(0xA1,OLED_CMD);
    }
    if(mode==1)
    {
        OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
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
    /* 等待发送缓冲区为空，TXE事件 */
    while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
    }
	SPI_I2S_ClearFlag(OLED_SPI, SPI_I2S_FLAG_TXE);
	SPI_I2S_SendData(OLED_SPI, dat);
	SPITimeout = SPIT_FLAG_TIMEOUT;
    /* 等待发送缓冲区为空，TXE事件 */
    while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
    }
	SPI_I2S_ClearFlag(OLED_SPI, SPI_I2S_FLAG_TXE);
	/* 等待发送完，BSY事件 */
    while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_BSY) == SET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(2);
    }
	OLED_CS_Set();
    OLED_DC_Set();
	return 0;
}


//开启OLED显示
uint8_t OLED_DisPlay_On(void)
{
    OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
    OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
    OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	return 0;
}

//关闭OLED显示
uint8_t OLED_DisPlay_Off(void)
{
    OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
    OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
    OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	return 0;
}

//更新显存到OLED
uint8_t OLED_Refresh(void)
{
    uint8_t i,n;
    for(i=0; i<8; i++)
    {
        OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
        OLED_WR_Byte(0x02,OLED_CMD);   //设置低列起始地址
        OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
        for(n=0; n<128; n++)
            OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
    }
	return 0;
}

//清屏函数
void OLED_Clear(void)
{
    memset(OLED_GRAM,0,144*8*sizeof(uint8_t));
    OLED_Refresh();//更新显示
}

//OLED的初始化
uint8_t OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI2_InitStructure;
	
    RCC_APB2PeriphClockCmd(OLEDGPIO_RCC, ENABLE);		//使能GPIO端口时钟
	RCC_APB1PeriphClockCmd(OLEDSPI_RCC, ENABLE);		//使能SPI2端口时钟
    GPIO_InitStructure.GPIO_Pin = OLED_RES|OLED_DC;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_Init(OLED_GPIOA, &GPIO_InitStructure);	  		//初始化GPIOA
    GPIO_InitStructure.GPIO_Pin = OLED_SCK|OLED_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
    GPIO_Init(OLED_GPIOB, &GPIO_InitStructure);	  		//初始化GPIOB
	GPIO_InitStructure.GPIO_Pin = OLED_NSS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
    GPIO_Init(OLED_GPIOB, &GPIO_InitStructure);	  		//初始化GPIOB
	
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

//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
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

//画线
//x1,y1:起点坐标
//x2,y2:结束坐标
void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode)
{
    uint16_t t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //计算坐标增量
    delta_y=y2-y1;
    uRow=x1;//画线起点坐标
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向
    else if (delta_x==0)incx=0;//垂直线
    else {
        incx=-1;
        delta_x=-delta_x;
    }
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//水平线
    else {
        incy=-1;
        delta_y=-delta_x;
    }
    if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
    else distance=delta_y;
    for(t=0; t<distance+1; t++)
    {
        OLED_DrawPoint(uRow,uCol,mode);//画点
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
//x,y:圆心坐标
//r:圆的半径
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
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,正常显示;1,反色显示
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode)
{
    uint8_t i,m,temp,size2,chr1;
    uint8_t x0=x,y0=y;
    if(size1==8)size2=6;
    else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
    chr1=chr-' ';  //计算偏移后的值
    for(i=0; i<size2; i++)	//不能随意改
    {
        if(size1==8)
        {
            temp=asc2_0806[chr1][i];   //调用0806字体
        }
        else if(size1==12)
        {
            temp=asc2_1206[chr1][i];   //调用1206字体
        }
        else if(size1==16)
        {
            temp=asc2_1608[chr1][i];   //调用1608字体
        }
        else if(size1==24)
        {
            temp=asc2_2412[chr1][i];   //调用2412字体
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


//显示字符串
//x,y:起点坐标
//size1:字体大小
//*chr:字符串起始地址
//mode:0,反色显示;1,正常显示
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode)
{
    while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
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

//显示数字
//x,y :起点坐标
//num :要显示的数字
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
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

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//mode:0,正常显示;1,反色显示
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1,uint8_t mode)
{
    uint8_t m,temp;
    uint8_t x0=x,y0=y;
    uint16_t i,size3=(size1/8+((size1%8)?1:0))*size1;  //得到字体一个字符对应点阵集所占的字节数
    for(i=0; i<size3; i++)
    {
        if(size1==16)
        {
//			temp=Hzk1[num][i];   //调用16*16字体
        }
        else if(size1==24)
        {
//            temp=Hzk2[num][i];   //调用24*24字体
        }
        else if(size1==32)
        {
//            temp=Hzk3[num][i];   //调用32*32字体
        }
        else if(size1==64)
        {
//            temp=Hzk4[num][i];   //调用64*64字体
        }
/*
        else if(size1==??)
        {
            temp=??;   //调用??字体
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

//num 显示汉字的个数
//space 每一遍显示的间隔
//mode:0,正常显示;1,反色显示
void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode)
{
    uint8_t i,n,t=0,m=0,r;
    while(1)
    {
        if(m==0)
        {
            OLED_ShowChinese(128,24,t,16,mode); //写入一个汉字保存在OLED_GRAM[][]数组中
            t++;
        }
        if(t==num)
        {
            for(r=0; r<16*space; r++)    //显示间隔
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
        for(i=1; i<144; i++) //实现左移
        {
            for(n=0; n<8; n++)
            {
                OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
            }
        }
        OLED_Refresh();
    }
}

//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,正常显示;1,反色显示
void OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode)
{
    uint16_t j=0;
    uint8_t i,n,temp,m;
    uint8_t x0=x,y0=y;
    sizey=sizey/8+((sizey%8)?1:0);
    for(n=0; n<sizey; n++)//页写入
    {
        for(i=0; i<sizex; i++)//列写入
        {
            temp=BMP[j];
            j++;
            for(m=0; m<8; m++)//一列8个点
            {
                if(temp&0x01)OLED_DrawPoint(x,y,!mode);
                else OLED_DrawPoint(x,y,mode);
                temp>>=1;
                y++;
            }
            x++;
            if((x-x0)==sizex)//页写完，下一页
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
    /* 等待超时后的处理,输出错误信息 */
    OLED_ERROR("SPI 等待超时!errorCode = %d",errorCode);
    return 0;
}
