/*
*********************************************************************************************************
*
*	ģ������ : ADC_BSP����
*	�ļ����� : adc_api.c
*	��    �� : V1.0.0
*	˵    �� : ADC��ʼ��(ʹ���жϷ�ʽ)��ADC���ݲɼ����¶Ȳɼ�, ADC�ж����ݵĴ���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "adc_api.h"

#if API_UseADC > 0
ADC_INIT_Type ADC_init; /* ADC�������� */
/*
*********************************************************************************************************
*	�� �� ��: ADC_IRQ_CallBack
*	����˵��: ADC�ɵ�ѹ�жϻص�
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void ADC_IRQ_CallBack(void)
{
    /* ADC�̶�д��,��Ҫ�� */
    ANAC_ADCIF_ADC_IF_Clr();                                                           /* ����жϱ�־ */
    ADC_init.ADC_Struct.Bufu16[ADC_init.ADC_Struct.SampleCount] = ANAC_ADCDATA_Read(); /* ��ȡADֵ */
    ADC_init.ADC_Struct.SampleCount++;
    if (ADC_init.ADC_Struct.SampleCount >= ADC_init.ADC_Struct.SampleNeed) /* ������� */
    {
        ANAC_ADCCON_ADC_IE_Setable(DISABLE);  /* �жϽ�ֹ */
        ANAC_ADCCON_ADC_EN_Setable(DISABLE);  /* ADC�ر� */
        ANAC_ADCINSEL_BUFEN_Setable(DISABLE); /* ���ؾ��ж���Ĺ��� */
    }
}

/*
*********************************************************************************************************
*	�� �� ��: G_ADC_IO_Init
*	����˵��: ADCIO�ڳ�ʼ��  ����ͨ�����Զ����ö�ӦIO
*	��    ��: ChSel_def ͨ����
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void G_ADC_IO_Init(uint8_t ChSel_def)
{
    switch (ChSel_def)
    {
    case CH_IN1: /* �ⲿ����ͨ�� */
    {
        GPIO_ANASEL_PC12ANS_Set(GPIO_ANASEL_PC12ANS_ADC_IN1);
        AnalogIO(GPIOC, GPIO_Pin_12); /* CH_IN1	PC12 */
    }
    break;
    case CH_IN2:
    {
        GPIO_ANASEL_PC13ANS_Set(GPIO_ANASEL_PC13ANS_ADC_IN2);
        AnalogIO(GPIOC, GPIO_Pin_13); /* CH_IN2	PC13 */
    }
    break;
    case CH_IN3:
    {
        AnalogIO(GPIOD, GPIO_Pin_0); /* CH_IN3 PD0 */
    }
    break;
    case CH_IN4:
    {
        AnalogIO(GPIOD, GPIO_Pin_1); /* CH_IN4 PD1 */
    }
    break;
    case CH_IN5:
    {
        AnalogIO(GPIOF, GPIO_Pin_6); /* CH_IN5 PF6 */
    }
    break;
    case CH_IN6:
    {
        GPIO_ANASEL_PC15ANS_Set(GPIO_ANASEL_PC15ANS_ACMP2_INP0_ADC_IN6);
        AnalogIO(GPIOC, GPIO_Pin_15); /* CH_IN6	PC15 */
    }
    break;
    case CH_IN7:
    {
        AnalogIO(GPIOB, GPIO_Pin_2); /* CH_IN7 PB2 */
    }
    break;
    case CH_IN8:
    {
        AnalogIO(GPIOB, GPIO_Pin_3); /* CH_IN8 PB3 */
    }
    break;
    default:
        break;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: G_ADC_Init
*	����˵��: ADC�����ʼ��
*	��    ��: ADC_INIT_Type �ͽṹ�����  ���忴�ṹ���Ա����
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void G_ADC_Init(ADC_INIT_Type *init)
{
    /* ʹ�ü򵥺������� */
    RCC_PERCLK_SetableEx(ANACCLK, ENABLE); /* ģ���·����ʱ��ʹ�� */
    RCC_PERCLK_SetableEx(ADCCLK, ENABLE);  /* ADCʱ��ʹ�� */
    if (RCHFCLKCFG <= 8)
        RCC_PERCLKCON2_ADCCKSEL_Set(RCC_PERCLKCON2_ADCCKSEL_RCHFDIV8); /* ADC����ʱ�����ã����ɸ���1M */
    else if (RCHFCLKCFG <= 16)
        RCC_PERCLKCON2_ADCCKSEL_Set(RCC_PERCLKCON2_ADCCKSEL_RCHFDIV16);
    else
        RCC_PERCLKCON2_ADCCKSEL_Set(RCC_PERCLKCON2_ADCCKSEL_RCHFDIV32);

    if (init->TrimV == 2)
    {
        ANAC_ADCTRIM_Write(const_adc_TrimV_3FF); /* adcƵ��1M ʱ ����ʱ��2ms */
    }
    else if (init->TrimV == 1)
    {
        ANAC_ADCTRIM_Write(const_adc_TrimV_1FF); /* adcƵ��1M ʱ ����ʱ��1ms */
    }
    else
    {
        ANAC_ADCTRIM_Write(const_adc_TrimV_0FF); /* adcƵ��1M ʱ ����ʱ��0.5ms */
    }

    ANAC_ADC_Channel_SetEx(init->ChSel_def); /* ADC����ͨ��ѡ�� */
    ANAC_ADCCON_ADC_IE_Setable(ENABLE);      /* �ж�ʹ�� */
    ANAC_ADCCON_ADC_EN_Setable(DISABLE);     /* ADC�ر� */

    /* NVIC�ж����� */
    NVIC_SET(ADC_IRQn, init->Nvicpry);
}

/*
*********************************************************************************************************
*	�� �� ��: G_ADC_Wait_Finish
*	����˵��: ADC�ȴ������ʱ
*	��    ��: ��
*	�� �� ֵ: 0����� 1����ʱ
*	��    ע����
*********************************************************************************************************
*/
uint8_t G_ADC_Wait_Finish(void)
{
    Do_DelayStart();
    {
        if (SET == ANAC_ADCIF_ADC_IF_Chk())
            return 0;
    }
    While_DelayMsEnd(8 * clkmode);
    return 1; /* ��ʱ */
}

/*
*********************************************************************************************************
*	�� �� ��: G_CloseeADIO
*	����˵��: ADCIO�ڹر�  ����ͨ�����Զ����ö�ӦIO
*	��    ��: ChSel_def ͨ����
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void G_CloseeADIO(uint8_t ChSel_def)
{
    switch (ChSel_def)
    {
    case CH_IN1: /* �ⲿ����ͨ�� */
    {
        CloseeIO(GPIOC, GPIO_Pin_12); /* CH_IN1	PC12 */
    }
    break;
    case CH_IN2:
    {
        CloseeIO(GPIOC, GPIO_Pin_13); /* CH_IN2	PC13 */
    }
    break;
    case CH_IN3:
    {
        CloseeIO(GPIOD, GPIO_Pin_0); /* CH_IN3 PD0 */
    }
    break;
    case CH_IN4:
    {
        CloseeIO(GPIOD, GPIO_Pin_1); /* CH_IN4 PD1 */
    }
    break;
    case CH_IN5:
    {
        CloseeIO(GPIOF, GPIO_Pin_6); /* CH_IN5 PF6 */
    }
    break;
    case CH_IN6:
    {
        CloseeIO(GPIOC, GPIO_Pin_15); /* CH_IN6	PC15 */
    }
    break;
    case CH_IN7:
    {
        CloseeIO(GPIOB, GPIO_Pin_2); /* CH_IN7 PB2 */
    }
    break;
    case CH_IN8:
    {
        CloseeIO(GPIOB, GPIO_Pin_3); /* CH_IN8 PB3 */
    }
    break;
    default:
        break;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: API_ADC
*	����˵��: ADC��API  ���鿴API_Exp��ADC������
*	��    ��: ADC_INIT_Type �ͽṹ�����  ���忴�ṹ���Ա����
*	�� �� ֵ: AD����ֵ
*	��    ע����
*********************************************************************************************************
*/
__weak uint32_t API_ADC(ADC_INIT_Type *init)
{
    uint8_t i;
    volatile uint32_t fVlotage = 0;
    uint32_t fTempADC = 0;

    G_ADC_IO_Init(init->ChSel_def); /* IO������ΪADC���� */
    G_ADC_Init(init);               /* ����ADC */

    memset(init->ADC_Struct.Bufu16, 0, SampleMax);

    if (init->SampleNeed > SampleMax)
        init->SampleNeed = SampleMax - 1;
    if (init->SampleNeed < 2)
        init->SampleNeed = 2;

    init->ADC_Struct.SampleNeed = init->SampleNeed; /* ��Ҫ�������ݵĸ�������һ������������� */
    init->ADC_Struct.SampleCount = 0;               /* ����������� */

    ANAC_ADCCON_ADC_EN_Setable(ENABLE); /* ADC���� */

    Do_DelayStart();
    {
        if (init->ADC_Struct.SampleCount >= init->ADC_Struct.SampleNeed) /* �ȴ�ת����� */
        {
            fTempADC = 0;
            for (i = 1; i < init->SampleNeed; i++)
            {
                fTempADC += init->ADC_Struct.Bufu16[i];
            }

            fTempADC = fTempADC / (init->SampleNeed - 1);

            if (init->SampleLevel == 1)
                fVlotage = ANAC_ADC_VoltageCalc(fTempADC, 5);
            else
                fVlotage = ANAC_ADC_VoltageCalc(fTempADC, 3);
            break;
        }
    }
    While_DelayMsEnd(init->SampleNeed * 8);

    G_CloseeADIO(init->ChSel_def);
    ANAC_ADCCON_ADC_IE_Setable(DISABLE);  /* �жϽ�ֹ */
    ANAC_ADCCON_ADC_EN_Setable(DISABLE);  /* ADC�ر� */
    ANAC_ADCINSEL_BUFEN_Setable(DISABLE); /* ���ؾ��ж���Ĺ��� */
    return fVlotage;
}
/*
*********************************************************************************************************
*	�� �� ��: API_Temperature
*	����˵��: ADC�¶ȴ��������ݲɼ�  ���鿴API_Exp��ADC������
*	��    ��: ADC_INIT_Type �ͽṹ�����  ���忴�ṹ���Ա����
*	�� �� ֵ: AD����ֵ
*	��    ע����
*********************************************************************************************************
*/
float API_Temperature(ADC_INIT_Type *init)
{
    uint8_t i;
    volatile float fTemperature = 0;
    float fTempADC = 0;

    G_ADC_Init(init); /* ����ADC */
    memset(init->ADC_Struct.Bufu16, 0, SampleMax);

    if (init->SampleNeed > SampleMax)
        init->SampleNeed = SampleMax - 1;
    if (init->SampleNeed < 2)
        init->SampleNeed = 2;

    init->ADC_Struct.SampleNeed = init->SampleNeed; /* ��Ҫ�������ݵĸ���,��һ������������� */

    init->ADC_Struct.SampleCount = 0;   /* ����������� */
    ANAC_ADCCON_ADC_EN_Setable(ENABLE); /* ADC���� */

    Do_DelayStart();
    {
        if (init->ADC_Struct.SampleCount >= init->ADC_Struct.SampleNeed) /* �ȴ�ת����� */
        {
            fTempADC = 0;
            for (i = 1; i < init->SampleNeed; i++)
            {
                fTempADC += init->ADC_Struct.Bufu16[i];
            }

            fTempADC = fTempADC / (init->SampleNeed - 1);

            if (init->SampleLevel == 1)
                fTemperature = ANAC_ADC_TemperatureCalc(fTempADC, 5); /* ADֵת��Ϊ�¶� */
            else
                fTemperature = ANAC_ADC_TemperatureCalc(fTempADC, 3);

            break;
        }
    }
    While_DelayMsEnd(init->SampleNeed * 10);

    ANAC_ADCCON_ADC_IE_Setable(DISABLE);  /* �жϽ�ֹ */
    ANAC_ADCCON_ADC_EN_Setable(DISABLE);  /* ADC�ر� */
    ANAC_ADCINSEL_BUFEN_Setable(DISABLE); /* ���ؾ��ж���Ĺ��� */

    return fTemperature;
}

#endif
