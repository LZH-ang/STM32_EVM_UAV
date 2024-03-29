#include "stm32f10x.h"
#include <stdbool.h>
#include "bsp_motor.h"
#include "bsp_usart.h"
#include "bsp_ws2812.h"
#include "bsp_systick.h"
#include "bsp_key.h"
#include "bsp_eeprom_iic.h"
#include "bsp_oled.h"
#include "bsp_si24r1.h"
#include "bmp.h"

int main(void)
{
    SI24R1_InitTypeDef SI24R1_InitStruct;
    uint8_t oledt = 0;
    static uint8_t SI24R1_buff[32] = {0};
    static uint8_t EEPROM_t[1048] = {0};
    uint8_t keycode;
    bool OLEDonoff = 1;
    MOTOR_Init();
    USART_Config();
    KEY_Init();
    SysTick_Init();
    WS2812_Init();
    EEPROM_I2C_Config();

    printf("\n我是Kelee.你好STM32!\n");
    printf("\nHere is Kelee.Hello STM32!\n");
    oledt = I2C_EEPROM_ByteR(0x02f);
    if(oledt != 0xC3)
    {
        printf("\nEEPROM_t = 0x%x\n",oledt);
        I2C_EEPROM_BufferW(0x000, Github, 0x3ff);
    }
    EEPROM_ACK_POLL();
    I2C_EEPROM_BufferR(0x000, EEPROM_t, 0x3ff);

    OLED_Init();
    OLED_ColorTurn(0);
    OLED_DisplayTurn(0);
    OLED_ShowPicture(0,0,128,64,EEPROM_t,0);
    OLED_Refresh();

    SI24R1_Init();
    SI24R1_InitStruct.ADR_WIDTH = 0x03;			// 5位接收地址
    SI24R1_InitStruct.RX_ADDR_Px = RX_ADDR_P0;	// 接收通道0
    SI24R1_InitStruct.EN_RXADDR_Px = 0x01;		// 使能接收通道0
    SI24R1_InitStruct.RF_CHANNEL = 40;			// 射频频道选择2440MHz
    SI24R1_InitStruct.RX_PW_Px = RX_PW_P0;		// 选择设置接收通道0负载数据宽度
    SI24R1_InitStruct.RF_SET = 0x0f;			// 传输速率功率设置，2Mbps 7dbm
    SI24R1_InitStruct.RF_FEATURE = 0x01;		// 使能NOACK
    SI24R1_InitStruct.SI24R1_CONFIG = 0x0f;		// CRC使能，16位CRC校验，上电，接收模式
    SI24R1_Mode_NOACK(&SI24R1_InitStruct,RX_MODE);

    while(1)
    {
        keycode = KEY_FIFO_Get();
        if(keycode != KEY_NONE)
        {
            switch(keycode)
            {
            case KEY_1_DOWN:
                printf("\nKEY_1_DOWN\n");
                break;
            case KEY_1_UP:
                printf("\nKEY_1_UP\n");
                SI24R1_Shutdown();
#if SI24R1_debug
                printf("\nSI24R1 Shutdown\n");
#endif
                break;
            case KEY_1_LONG:
                printf("\nKEY_1_LONG\n");
                OLEDonoff = !OLEDonoff;
                if(OLEDonoff)
                    OLED_DisPlay_On();
                else
                    OLED_DisPlay_Off();
                break;
            case KEY_2_DOWN:
                printf("\nKEY_2_DOWN\n");
                break;
            case KEY_2_UP:
                printf("\nKEY_2_UP\n");
                SI24R1_buff[0] = 0x55;
                SI24R1_Mode_NOACK(&SI24R1_InitStruct,TX_MODE);
                if(SI24R1_TxPacket(SI24R1_buff) == 0x20)
#if SI24R1_debug
                    printf("\nSI24R1 TXsuccess\n");
#endif
#if SI24R1_debug
                else
                    printf("\nSI24R1 TXfail\n");
#endif
                SI24R1_Mode_NOACK(&SI24R1_InitStruct,RX_MODE);
                break;
            case KEY_2_LONG:
                printf("\nKEY_2_LONG\n");
                break;
            case KEY_3_DOWN:
                printf("\nKEY_3_DOWN\n");
                break;
            case KEY_3_UP:
                printf("\nKEY_3_UP\n");
                SI24R1_buff[0] = 0xAA;
                SI24R1_Mode_NOACK(&SI24R1_InitStruct,TX_MODE);
                if(SI24R1_TxPacket(SI24R1_buff) == 0x20)
#if SI24R1_debug
                    printf("\nSI24R1 TXsuccess\n");
#endif
#if SI24R1_debug
                else
                    printf("\nSI24R1 TXfail\n");
#endif
                SI24R1_Mode_NOACK(&SI24R1_InitStruct,RX_MODE);
                break;
            case KEY_3_LONG:
                printf("\nKEY_3_LONG\n");
                break;
            default:
                ;
            }
        }
        if(SI24R1_RxPacket(SI24R1_buff))
        {
#if SI24R1_debug
            printf("\nSI24R1 readdate:0x%X\n",SI24R1_buff[0]);
#endif
            switch(SI24R1_buff[0])
            {
            case 0x55:
                WS2812_BLUE;
                break;
            case 0xAA:
                WS2812_DARK;
                break;
            default:
                break;
            }
            SI24R1_buff[0] = 0;
        }
    }
}


