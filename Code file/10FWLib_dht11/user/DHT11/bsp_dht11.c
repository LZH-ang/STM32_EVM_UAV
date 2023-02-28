#include "bsp_dht11.h"
#include "core_delay.h"

static __IO uint32_t  DHT11Timeout = DHT11T_FLAG_TIMEOUT*10;
static uint32_t DHT11_TIMEOUT_UserCallback(uint8_t errorCode);

static void DHT11_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/

    DHT11_Dout_DATA_APBxClock_FUN(DHT11_Dout_GPIO_CLK, ENABLE);	/*����DHT11_Dout_GPIO_PORT������ʱ��*/
    GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;	/*ѡ��Ҫ���Ƶ�DHT11_Dout_GPIO_PORT����*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	/*��������ģʽΪͨ�ÿ�©���*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/*������������Ϊ50MHz*/
    GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);	/*���ÿ⺯��,��ʼ��DHT11_Dout_GPIO_PORT*/
}

void DHT11_Init(void)
{
    DHT11_GPIO_Config();
    DHT11_Dout_1;	/*����GPIOA12*/
	DHT11_INFO("DHT11 Init SUCCESS");
}

static uint8_t DHT11_ReadByte ( void )
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)	/*ÿbit��50us�͵�ƽ���ÿ�ʼ,��ѯֱ���ӻ�������50us�͵�ƽ����*/    
	{	 	
		DHT11Timeout = DHT11T_FLAG_TIMEOUT;
		while(DHT11_Dout_IN()==Bit_RESET)
		{
			if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(0);
		}
		/*DHT11��26~28us�ĸߵ�ƽ��ʾ��0��,��70us�ߵ�ƽ��ʾ��1��,
		 *ͨ�����x us��ĵ�ƽ��������������״,x���������ʱ 
		 */
		DHT11_DELAY_US(40);	/*��ʱx us�����ʱ��Ҫ��������0������ʱ�伴��*/	   	  
		if(DHT11_Dout_IN()==Bit_SET)	/*x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1��*/
		{
			DHT11Timeout = DHT11T_FLAG_TIMEOUT;
			while(DHT11_Dout_IN()==Bit_SET)	/*�ȴ�����1�ĸߵ�ƽ����*/
			{
				if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(1);
			}
			temp|=(uint8_t)(0x01<<(7-i));	/*�ѵ�7-iλ��1,MSB����*/ 
		}
		else	/*x us��Ϊ�͵�ƽ��ʾ���ݡ�0��*/
		{			   
			temp&=(uint8_t)~(0x01<<(7-i));	/*�ѵ�7-iλ��0,MSB����*/
		}
	}
	return temp;
}

/*
 * һ�����������ݴ���Ϊ40bit,��λ�ȳ�
 * 8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
 */
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{
	DHT11_Dout_0;	/*��������*/
	DHT11_DELAY_MS(20);	/*��ʱ>18ms*/
	DHT11_Dout_1;	/*��������,������ʱ30us*/ 
	DHT11_DELAY_US(30);	/*��ʱ30us*/
	if(DHT11_Dout_IN()==Bit_RESET)	/*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź�,�粻��Ӧ������,��Ӧ����������*/      
	{
		DHT11Timeout = DHT11T_FLAG_TIMEOUT;
		while(DHT11_Dout_IN()==Bit_RESET)	/*��ѯֱ���ӻ�������80us�͵�ƽ,��Ӧ�źŽ���*/
		{
			if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(2);
		}
		DHT11Timeout = DHT11T_FLAG_TIMEOUT;		
		while(DHT11_Dout_IN()==Bit_SET)	/*��ѯֱ���ӻ�������80us�ߵ�ƽ,�����źŽ���*/
		{
			if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(3);
		}
		/*��ʼ��������*/   
		DHT11_Data->humi_int= DHT11_ReadByte();
		DHT11_Data->humi_deci= DHT11_ReadByte();
		DHT11_Data->temp_int= DHT11_ReadByte();
		DHT11_Data->temp_deci= DHT11_ReadByte();
		DHT11_Data->check_sum= DHT11_ReadByte();
		DHT11_Dout_1;	/*��������*/
		/*����ȡ�������Ƿ���ȷ*/
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return SUCCESS;
		else return ERROR;
	}
	else return ERROR;
	
}

static uint32_t DHT11_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* Block communication and all processes */
    DHT11_ERROR("DHT11�ȴ���ʱ!errorCode = %d",errorCode);
    return 0;
}
//DHT11Timeout = DHT11T_FLAG_TIMEOUT;
//while(...)
//{
//	(...);
//	if((DHT11Timeout--) == 0) return DHT11_TIMEOUT_UserCallback(0);
//}
