#include "bsp_key.h"

#define HARD_KEY_NUM	3	   				/* ʵ�尴������ */
static KEY_T	s_tBtn[HARD_KEY_NUM] = {0};	/* �������Ʊ���,�ṹ�� */
static KEY_FIFO_T s_tKey;					/* ����FIFO����,�ṹ�� */
static uint8_t IsKey1Down(void);			/* K1�������µ��жϺ��� */
static uint8_t IsKey2Down(void);			/* K2�������µ��жϺ��� */
static uint8_t IsKey3Down(void);			/* K3�������µ��жϺ��� */

/*
*********************************************************************************************************
*	�� �� ��: KEY_FIFO_Init
*	����˵��: ��ʼ����������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void KEY_FIFO_Init(void)
{
    uint8_t i;
    /* �԰���FIFO��дָ������ */
    s_tKey.Read = 0;
    s_tKey.Write = 0;

    /* ��ÿ�������ṹ���Ա������һ��ȱʡֵ */
    for (i = 0; i < HARD_KEY_NUM; i++)
    {
        s_tBtn[i].LongTime = KEY_LONG_TIME;		/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
        s_tBtn[i].Count = KEY_FILTER_TIME / 2;	/* ����������Ϊ�˲�ʱ���һ�� */
        s_tBtn[i].State = 0;					/* ����ȱʡ״̬��0Ϊδ���� */
        s_tBtn[i].RepeatSpeed = 0;				/* �����������ٶȣ�0��ʾ��֧������ */
        s_tBtn[i].RepeatCount = 0;				/* ���������� */
    }
    /* �жϰ������µĺ��� */
    s_tBtn[0].IsKeyDownFunc = IsKey1Down;
    s_tBtn[1].IsKeyDownFunc = IsKey2Down;
    s_tBtn[2].IsKeyDownFunc = IsKey3Down;
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_GPIO_Config
*	����˵��: ���ð�����Ӧ��GPIO
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_KEY;

    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK|KEY3_GPIO_CLK, ENABLE);
    GPIO_KEY.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_KEY.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_KEY);
    GPIO_KEY.GPIO_Pin = KEY2_GPIO_PIN;
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_KEY);
    GPIO_KEY.GPIO_Pin = KEY3_GPIO_PIN;
    GPIO_Init(KEY3_GPIO_PORT, &GPIO_KEY);

    KEY_FIFO_Init();
}

/*
*********************************************************************************************************
*	�� �� ��: IsKeyDownFunc
*	����˵��: �жϰ����Ƿ��¡���������ϼ����֡������¼������������������¡�
*	��    ��: ��
*	�� �� ֵ: ����ֵ1 ��ʾ����(��ͨ����0��ʾδ���£��ͷţ�
*********************************************************************************************************
*/
static uint8_t IsKey1Down(void)
{
    if (GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_OFF)
        return 1;
    else
        return 0;
}
static uint8_t IsKey2Down(void)
{
    if (GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_OFF)
        return 1;
    else
        return 0;
}

static uint8_t IsKey3Down(void)
{
    if (GPIO_ReadInputDataBit(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == KEY_OFF)
        return 1;
    else
        return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_FIFO_Put
*	����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*	��    ��:  _KeyCode : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void KEY_FIFO_Put(uint8_t _KeyCode)
{
    s_tKey.Buf[s_tKey.Write] = _KeyCode;

    if (++s_tKey.Write  >= KEY_FIFO_SIZE)
    {
        s_tKey.Write = 0;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_FIFO_Get
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*	��    ��: ��
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t KEY_FIFO_Get(void)
{
    uint8_t ret;

    if (s_tKey.Read == s_tKey.Write)
    {
        return KEY_NONE;
    }
    else
    {
        ret = s_tKey.Buf[s_tKey.Read];
        if (++s_tKey.Read >= KEY_FIFO_SIZE)
        {
            s_tKey.Read = 0;
        }
        return ret;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_GetState
*	����˵��: ��ȡ������״̬
*	��    ��:  _ucKeyID : ����ID����0��ʼ
*	�� �� ֵ: 1 ��ʾ���£� 0 ��ʾδ����
*********************************************************************************************************
*/
uint8_t KEY_GetState(KEY_ID_E _ucKeyID)
{
    return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_SetParam
*	����˵��: ���ð�������
*	��    �Σ�_ucKeyID : ����ID����0��ʼ
*					 _LongTime : �����¼�ʱ��
*					 _RepeatSpeed : �����ٶ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void KEY_SetParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
    s_tBtn[_ucKeyID].LongTime = _LongTime;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
    s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;	/* �����������ٶȣ�0��ʾ��֧������ */
    s_tBtn[_ucKeyID].RepeatCount = 0;				/* ���������� */
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_FIFO_Clear
*	����˵��: ��հ���FIFO������
*	��    �Σ���
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void KEY_FIFO_Clear(void)
{
    s_tKey.Read = s_tKey.Write;
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_Detect
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*	��    ��: IO��id�� ��0��ʼ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void KEY_Detect(uint8_t i)
{
    KEY_T *pBtn;
    pBtn = &s_tBtn[i];/*��ȡ��Ӧ�����Ľṹ���ַ����������ÿ�����������Լ��Ľṹ�塣*/
    if(pBtn->IsKeyDownFunc())
    {
        /*�������ִ�е��ǰ������µĴ���*/
        if(pBtn->Count < KEY_FILTER_TIME)	/*���ڰ����˲�ǰ�� Count ����һ����ֵ*/
        {
            pBtn->Count = KEY_FILTER_TIME;
        }
        else if(pBtn->Count < 2*KEY_FILTER_TIME)	/*����ʵ�� KEY_FILTER_TIME ʱ�䳤�ȵ��ӳ�*/
        {
            pBtn->Count++;
        }
        else
        {
            if(pBtn->State == 0)
            {
                pBtn->State = 1;	/*������������ˣ�����ͽ�������Ϊ 1*/

                /* ���Ͱ�ť���µ���Ϣ */
                KEY_FIFO_Put((uint8_t)(3 * i + 1));
            }
            if (pBtn->LongTime > 0) /*LongTime��ʼֵ��200����λ5ms������1�룬��Ϊ�����¼�*/
            {
                if (pBtn->LongCount < pBtn->LongTime)	/*LongCount��������������λ5ms������1�룬��Ϊ�����¼�*/
                {
                    /* ���Ͱ�ť�������µ���Ϣ */
                    if (++pBtn->LongCount == pBtn->LongTime)/*LongCount����LongTime(200),5ms����һ�Σ�������200��Ҳ����˵����ʱ��Ϊ��1s*/
                    {
                        /* ��ֵ���밴��FIFO */
                        KEY_FIFO_Put((uint8_t)(3 * i + 3));
                    }
                }
                /*LongCount����LongTime(200),Ҳ����˵����ʱ�����1s*/
                else if (pBtn->RepeatSpeed > 0)	/* RepeatSpeed������������ */
                {
                    if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
                    {
                        pBtn->RepeatCount = 0;
                        /* ��������ÿ��200ms����1����������Ϊ����Ҳ��Ҫ���ͼ�ֵ�ã�200ms����һ�Ρ�*/
                        KEY_FIFO_Put((uint8_t)(3 * i + 1));
                    }
                }
            }
        }
    }
    else
    {
        /*�������ִ�е��ǰ������ֵĴ�����߰���û�а��µĴ���*/
        if(pBtn->Count > KEY_FILTER_TIME)
        {
            pBtn->Count = KEY_FILTER_TIME;
        }
        else if(pBtn->Count != 0)
        {
            pBtn->Count--;
        }
        else if(pBtn->State == 1)
        {
            pBtn->State = 0;
            /* ���Ͱ�ť�������Ϣ */
            KEY_FIFO_Put((uint8_t)(3 * i + 2));
        }
        pBtn->LongCount = 0;
        pBtn->RepeatCount = 0;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyScan10ms
*	����˵��: ɨ�����а���������������systick�ж������Եĵ��ã�10msһ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void KEY_Scan(void)
{
    uint8_t i;

    for (i = 0; i < HARD_KEY_NUM; i++)
    {
        KEY_Detect(i);
    }
}

uint8_t KEY_IO(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_OFF)
    {
        while(!GPIO_ReadInputDataBit(GPIOx,GPIO_Pin));
        return KEY_OFF;
    }
    else return KEY_ON;
}
