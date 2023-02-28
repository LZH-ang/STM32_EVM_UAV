#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "bsp_motor.h"
#include "bsp_usart.h"
#include "bsp_ws2812.h"
#include "bsp_systick.h"
#include "bsp_mpu6050_io.h"
#include "bsp_iic_io.h"

#define TASK_ENABLE 0
extern uint16_t Task_Delay[NumOfTask];

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
	i2c_GPIO_Config();
	MPU6050_Init();
	
	printf("\n我是Kelee.你好STM32!\n");
	printf("\nHere is Kelee.Hello STM32!\n");

	//检测MPU6050
	if (MPU6050ReadID() == 1)
	{	
		while(1)
		{
			if(Task_Delay[0]==TASK_ENABLE)
			{
				LED?WS2812_GREEN:WS2812_DARK;
				Task_Delay[0]=200;
				LED = !LED;
			}
			
			if(Task_Delay[1]==TASK_ENABLE)
			{
				MPU6050ReadAcc(Acel); 
				printf("加速度：%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
				MPU6050ReadGyro(Gyro);
				printf("	陀螺仪%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
				MPU6050_ReturnTemp(&Temp);
				printf("	温度%8.2f\r\n",Temp);
				Task_Delay[1]=20;//更新一次数据，可根据自己的需求，提高采样频率，如100ms采样一次
			}
	//*************************************	下面是增加任务的格式************************************//
	//		if(Task_Delay[i]==0)
	//		{
	//			Task(i);
	//			Task_Delay[i]=;
	//		}
		}
	}
	else
	{
		printf("\r\n没有检测到MPU6050传感器！\r\n");
		WS2812_RED; 
		while(1);	
	}
}

