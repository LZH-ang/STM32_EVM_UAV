#include "stm32f10x.h"   // �൱��51��Ƭ���е�  #include <reg51.h>
#include "bsp_motor.h"
#include "bsp_usart.h"
#include "bsp_ws2812.h"
#include "bsp_eeprom_iic.h"

#define DOUBLE_ADDR		0x008
#define LONGINT_ADDR    0x040
#define DIGITAL_ADDR    0x060
#define LETTERLOW_ADDR  0x070
#define LETTERUP_ADDR   0x090
#define CHINESE_ADDR	0x0b0

int main(void)
{
    uint8_t num = 1;
    uint8_t cal_flag = 0x000;
    uint8_t k;
    char digital_char[] = "0123456789";
    char letter_Lchar[] = "abcdefghijklmnopqrstuvwxyz";
    char letter_Uchar[] = "ABCDEFGHIGKLMNOPQRSTUVWXYZ";
    char Chinese_char[] = "STM32�ù�";
    long double double_buffer[7] = {0};	//64bit
    int int_bufffer[7] = {0};						//32bit

    // ���������ʱ��ϵͳ��ʱ���Ѿ������ó�72M��
    MOTOR_Init();
    USART_Config();
    EEPROM_I2C_Config();

    MOTORstate(ON);
    delay_ms(200);
    MOTORstate(OFF);

    printf("\n����Kelee.���STM32!\n");
    printf("Here is Kelee.Hello STM32!\n");

#if 0
    /*��ȡ�ַ�*/
    printf("\r\ndigital_char=%d '%s'\r\n",sizeof(digital_char),digital_char);
    I2C_EEPROM_BufferW(DIGITAL_ADDR, (void*)digital_char, sizeof(digital_char));
    I2C_EEPROM_BufferR(DIGITAL_ADDR, (void*)digital_char, sizeof(digital_char));
    printf("READ_EEPROM:%s\r\n",digital_char);
    printf("letter_Lchar=%d '%s'\r\n",sizeof(letter_Lchar),letter_Lchar);
    I2C_EEPROM_BufferW(LETTERLOW_ADDR, (void*)letter_Lchar, sizeof(letter_Lchar));
    I2C_EEPROM_BufferR(LETTERLOW_ADDR, (void*)letter_Lchar, sizeof(letter_Lchar));
    printf("READ_EEPROM:%s\r\n",letter_Lchar);
    printf("letter_Uchar=%d '%s'\r\n",sizeof(letter_Uchar),letter_Uchar);
    I2C_EEPROM_BufferW(LETTERUP_ADDR, (void*)letter_Uchar, sizeof(letter_Uchar));
    I2C_EEPROM_BufferR(LETTERUP_ADDR, (void*)letter_Uchar, sizeof(letter_Uchar));
    printf("READ_EEPROM:%s\r\n",letter_Uchar);
    printf("Chinese_char=%d '%s'\r\n",sizeof(Chinese_char),Chinese_char);
    I2C_EEPROM_BufferW(CHINESE_ADDR, (void*)Chinese_char, sizeof(Chinese_char));
    I2C_EEPROM_BufferR(CHINESE_ADDR, (void*)Chinese_char, sizeof(Chinese_char));
    printf("READ_EEPROM:%s\r\n",Chinese_char);

    WriteProtect(0);
    printf("\nEEPROM WRITE:%d\n",num);
    if(I2C_EEPROM_ByteW(0x100, &num))
        printf("EEPROM WRITE SUSSECC\n");
    else printf("EEPROM WRITE FAIL\n");
    EEPROM_ACK_POLL();
    if(I2C_EEPROM_ByteR(0x100) == num)
        printf("EEPROM READ SUSSECC value:%d\n",I2C_EEPROM_ByteR(0x100));
    else printf("EEPROM READ FAIL\n");

    if(EEPROM_I2C_Test())printf("TEST SUCCESS!\n");
    else printf("TEST FAIL!\n");

#else
    /*��ȡ���ݱ�־λ*/
    cal_flag = I2C_EEPROM_ByteR(cal_flag);
    if( cal_flag != 0x0CD )	/*����־����0xcd����ʾ֮ǰ����д������*/
    {
        printf("\r\nû�м�⵽���ݱ�־��FLASHû�д洢���ݣ���������С��д��ʵ��\r\n");
        cal_flag =0x0CD;
        /*д���־��0��ַ*/
        I2C_EEPROM_ByteW(0x000, &cal_flag);
        /*����Ҫд�������*/
        for( k=0; k<7; k++ )
        {
            double_buffer[k] = k+0.1;
            int_bufffer[k] = k*500+1;
        }
        EEPROM_ACK_POLL();
        /*д��С�����ݵ���ַ*/
        I2C_EEPROM_BufferW(DOUBLE_ADDR, (void*)double_buffer, sizeof(double_buffer));
        /*д���������ݵ���ַ*/
        I2C_EEPROM_BufferW(LONGINT_ADDR, (void*)int_bufffer, sizeof(int_bufffer));

        printf("��оƬд�����ݣ�\n");
        /*��ӡ������*/
        for( k=0; k<7; k++ )
        {
            printf("С��tx = %LF\r\n",double_buffer[k]);
            printf("����tx = %d\r\n",int_bufffer[k]);
        }
        printf("\r\n�븴λ�����壬�Զ�ȡ���ݽ��м���\r\n");
    }
    else
    {
        printf("\r\n��⵽���ݱ�־\r\n");
        EEPROM_ACK_POLL();
        /*��ȡС������*/
        I2C_EEPROM_BufferR(DOUBLE_ADDR, (void*)double_buffer, sizeof(double_buffer));
        /*��ȡ��������*/
        I2C_EEPROM_BufferR(LONGINT_ADDR, (void*)int_bufffer, sizeof(int_bufffer));
        printf("\r\n��оƬ�������ݣ�\r\n");
        for( k=0; k<7; k++ )
        {
            printf("С�� rx = %LF \r\n",double_buffer[k]);
            printf("���� rx = %d \r\n",int_bufffer[k]);
        }
    }
#endif

    while(1)
    {

    }
}

