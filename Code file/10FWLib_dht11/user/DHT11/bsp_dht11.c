#include "bsp_dht11.h"
#include "core_delay.h"

static __IO uint32_t  DHT11Timeout = DHT11T_FLAG_TIMEOUT*10;
static uint32_t DHT11_TIMEOUT_UserCallback(uint8_t errorCode);

static void DHT11_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	/*定义一个GPIO_InitTypeDef类型的结构体*/

    DHT11_Dout_DATA_APBxClock_FUN(DHT11_Dout_GPIO_CLK, ENABLE);	/*开启DHT11_Dout_GPIO_PORT的外设时钟*/
    GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;	/*选择要控制的DHT11_Dout_GPIO_PORT引脚*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	/*设置引脚模式为通用开漏输出*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/*设置引脚速率为50MHz*/
    GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);	/*调用库函数,初始化DHT11_Dout_GPIO_PORT*/
}

void DHT11_Init(void)
{
    DHT11_GPIO_Config();
    DHT11_Dout_1;	/*拉高GPIOA12*/
	DHT11_INFO("DHT11 Init SUCCESS");
}

static uint8_t DHT11_ReadByte ( void )
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)	/*每bit以50us低电平标置开始,轮询直到从机发出的50us低电平结束*/    
	{	 	
		DHT11Timeout = DHT11T_FLAG_TIMEOUT;
		while(DHT11_Dout_IN()==Bit_RESET)
		{
			if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(0);
		}
		/*DHT11以26~28us的高电平表示“0”,以70us高电平表示“1”,
		 *通过检测x us后的电平即可区别这两个状,x即下面的延时 
		 */
		DHT11_DELAY_US(40);	/*延时x us这个延时需要大于数据0持续的时间即可*/	   	  
		if(DHT11_Dout_IN()==Bit_SET)	/*x us后仍为高电平表示数据“1”*/
		{
			DHT11Timeout = DHT11T_FLAG_TIMEOUT;
			while(DHT11_Dout_IN()==Bit_SET)	/*等待数据1的高电平结束*/
			{
				if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(1);
			}
			temp|=(uint8_t)(0x01<<(7-i));	/*把第7-i位置1,MSB先行*/ 
		}
		else	/*x us后为低电平表示数据“0”*/
		{			   
			temp&=(uint8_t)~(0x01<<(7-i));	/*把第7-i位置0,MSB先行*/
		}
	}
	return temp;
}

/*
 * 一次完整的数据传输为40bit,高位先出
 * 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
 */
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{
	DHT11_Dout_0;	/*主机拉低*/
	DHT11_DELAY_MS(20);	/*延时>18ms*/
	DHT11_Dout_1;	/*总线拉高,主机延时30us*/ 
	DHT11_DELAY_US(30);	/*延时30us*/
	if(DHT11_Dout_IN()==Bit_RESET)	/*判断从机是否有低电平响应信号,如不响应则跳出,响应则向下运行*/      
	{
		DHT11Timeout = DHT11T_FLAG_TIMEOUT;
		while(DHT11_Dout_IN()==Bit_RESET)	/*轮询直到从机发出的80us低电平,响应信号结束*/
		{
			if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(2);
		}
		DHT11Timeout = DHT11T_FLAG_TIMEOUT;		
		while(DHT11_Dout_IN()==Bit_SET)	/*轮询直到从机发出的80us高电平,标置信号结束*/
		{
			if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(3);
		}
		/*开始接收数据*/   
		DHT11_Data->humi_int= DHT11_ReadByte();
		DHT11_Data->humi_deci= DHT11_ReadByte();
		DHT11_Data->temp_int= DHT11_ReadByte();
		DHT11_Data->temp_deci= DHT11_ReadByte();
		DHT11_Data->check_sum= DHT11_ReadByte();
		DHT11_Dout_1;	/*主机拉高*/
		/*检查读取的数据是否正确*/
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return SUCCESS;
		else return ERROR;
	}
	else return ERROR;
	
}

static uint32_t DHT11_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* Block communication and all processes */
    DHT11_ERROR("DHT11等待超时!errorCode = %d",errorCode);
    return 0;
}
//DHT11Timeout = DHT11T_FLAG_TIMEOUT;
//while(...)
//{
//	(...);
//	if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(0);
//}
