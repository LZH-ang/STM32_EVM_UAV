#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "bsp_motor.h"
#include "bsp_usart.h"
#include "bsp_ws2812.h"
#include "bsp_systick.h"
#include "bsp_mpu6050_iic.h"
#include "bsp_exti.h"

#define TASK_ENABLE 0
extern uint16_t Task_Delay[NumOfTask];
extern uint8_t MPU6050_Go;

int main(void)
{
    int16_t Acel[3];
    int16_t Gyro[3];
    float Temp;
    _Bool LED=0;

    MOTOR_Init();
    USART_Config();
    WS2812_Init();
    SysTick_Init();
    MPU6050_I2C_Config();
    MPU6050_Init();

    printf("\n����Kelee.���STM32!\n");
    printf("\nHere is Kelee.Hello STM32!\r\n\n");

    //���MPU6050
    if (MPU6050ReadID() == 1)
    {
        GPIO_INT_Init();
        while(1)			//����ѡ��ȫ�����ж�������
        {
            if(Task_Delay[0]==TASK_ENABLE)
            {
                LED?WS2812_GREEN:WS2812_DARK;
                Task_Delay[0]=200;
                LED = !LED;
            }
//			MPU6050_Go = 1;
            if((Task_Delay[1]==TASK_ENABLE) && MPU6050_Go)
            {
                EXTI->IMR &= ~MPU6050_LINE;		//��MPU6050����֮ǰ�Ƚ�ֹMPU6050�ж���Ӧ
                MPU6050ReadAcc(Acel);
                printf("���ٶȣ�%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
                MPU6050ReadGyro(Gyro);
                printf("	������%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
                MPU6050_ReturnTemp(&Temp);
                printf("	�¶�%8.2f\r\n",Temp);
                Task_Delay[1]=20;	//����һ�����ݣ��ɸ����Լ���������߲���Ƶ�ʣ���100ms����һ��
                MPU6050_Go = 0;
                EXTI->IMR |= MPU6050_LINE;		//�����ٴ�
            }
            //*************************************	��������������ĸ�ʽ************************************//
            //		if(Task_Delay[i]==0)
            //		{
            //			Task(i);
            //			Task_Delay[i]=;
            //		}
        }
    }
    else
    {
        printf("\r\nû�м�⵽MPU6050��������\r\n");
        WS2812_RED;
    }
}

