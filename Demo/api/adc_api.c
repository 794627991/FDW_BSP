/*
*********************************************************************************************************
*
*	模块名称 : ADC_BSP程序
*	文件名称 : adc_api.c
*	版    本 : V1.0.0
*	说    明 : ADC初始化(使用中断方式)，ADC数据采集，温度采集, ADC中断数据的处理
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-12-31  高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "adc_api.h"

#if API_UseADC > 0
ADC_INIT_Type ADC_init; /* ADC采样缓存 */
/*
*********************************************************************************************************
*	函 数 名: ADC_IRQ_CallBack
*	功能说明: ADC采电压中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void ADC_IRQ_CallBack(void)
{
    /* ADC固定写法,不要改 */
    ANAC_ADCIF_ADC_IF_Clr();                                                           /* 清除中断标志 */
    ADC_init.ADC_Struct.Bufu16[ADC_init.ADC_Struct.SampleCount] = ANAC_ADCDATA_Read(); /* 读取AD值 */
    ADC_init.ADC_Struct.SampleCount++;
    if (ADC_init.ADC_Struct.SampleCount >= ADC_init.ADC_Struct.SampleNeed) /* 采样完成 */
    {
        ANAC_ADCCON_ADC_IE_Setable(DISABLE);  /* 中断禁止 */
        ANAC_ADCCON_ADC_EN_Setable(DISABLE);  /* ADC关闭 */
        ANAC_ADCINSEL_BUFEN_Setable(DISABLE); /* 不关就有额外的功耗 */
    }
}

/*
*********************************************************************************************************
*	函 数 名: G_ADC_IO_Init
*	功能说明: ADCIO口初始化  根据通道号自动配置对应IO
*	形    参: ChSel_def 通道号
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void G_ADC_IO_Init(uint8_t ChSel_def)
{
    switch (ChSel_def)
    {
    case CH_IN1: /* 外部输入通道 */
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
*	函 数 名: G_ADC_Init
*	功能说明: ADC整体初始化
*	形    参: ADC_INIT_Type 型结构体变量  具体看结构体成员变量
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void G_ADC_Init(ADC_INIT_Type *init)
{
    /* 使用简单函数配置 */
    RCC_PERCLK_SetableEx(ANACCLK, ENABLE); /* 模拟电路总线时钟使能 */
    RCC_PERCLK_SetableEx(ADCCLK, ENABLE);  /* ADC时钟使能 */
    if (RCHFCLKCFG <= 8)
        RCC_PERCLKCON2_ADCCKSEL_Set(RCC_PERCLKCON2_ADCCKSEL_RCHFDIV8); /* ADC工作时钟配置，不可高于1M */
    else if (RCHFCLKCFG <= 16)
        RCC_PERCLKCON2_ADCCKSEL_Set(RCC_PERCLKCON2_ADCCKSEL_RCHFDIV16);
    else
        RCC_PERCLKCON2_ADCCKSEL_Set(RCC_PERCLKCON2_ADCCKSEL_RCHFDIV32);

    if (init->TrimV == 2)
    {
        ANAC_ADCTRIM_Write(const_adc_TrimV_3FF); /* adc频率1M 时 计算时间2ms */
    }
    else if (init->TrimV == 1)
    {
        ANAC_ADCTRIM_Write(const_adc_TrimV_1FF); /* adc频率1M 时 计算时间1ms */
    }
    else
    {
        ANAC_ADCTRIM_Write(const_adc_TrimV_0FF); /* adc频率1M 时 计算时间0.5ms */
    }

    ANAC_ADC_Channel_SetEx(init->ChSel_def); /* ADC输入通道选择 */
    ANAC_ADCCON_ADC_IE_Setable(ENABLE);      /* 中断使能 */
    ANAC_ADCCON_ADC_EN_Setable(DISABLE);     /* ADC关闭 */

    /* NVIC中断配置 */
    NVIC_SET(ADC_IRQn, init->Nvicpry);
}

/*
*********************************************************************************************************
*	函 数 名: G_ADC_Wait_Finish
*	功能说明: ADC等待完成延时
*	形    参: 无
*	返 回 值: 0：完成 1：超时
*	备    注：无
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
    return 1; /* 超时 */
}

/*
*********************************************************************************************************
*	函 数 名: G_CloseeADIO
*	功能说明: ADCIO口关闭  根据通道号自动配置对应IO
*	形    参: ChSel_def 通道号
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void G_CloseeADIO(uint8_t ChSel_def)
{
    switch (ChSel_def)
    {
    case CH_IN1: /* 外部输入通道 */
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
*	函 数 名: API_ADC
*	功能说明: ADC的API  详情看API_Exp中ADC的例程
*	形    参: ADC_INIT_Type 型结构体变量  具体看结构体成员变量
*	返 回 值: AD采样值
*	备    注：无
*********************************************************************************************************
*/
__weak uint32_t API_ADC(ADC_INIT_Type *init)
{
    uint8_t i;
    volatile uint32_t fVlotage = 0;
    uint32_t fTempADC = 0;

    G_ADC_IO_Init(init->ChSel_def); /* IO口配置为ADC输入 */
    G_ADC_Init(init);               /* 配置ADC */

    memset(init->ADC_Struct.Bufu16, 0, SampleMax);

    if (init->SampleNeed > SampleMax)
        init->SampleNeed = SampleMax - 1;
    if (init->SampleNeed < 2)
        init->SampleNeed = 2;

    init->ADC_Struct.SampleNeed = init->SampleNeed; /* 需要采样数据的个数，第一个采样结果丢弃 */
    init->ADC_Struct.SampleCount = 0;               /* 清零采样计数 */

    ANAC_ADCCON_ADC_EN_Setable(ENABLE); /* ADC启动 */

    Do_DelayStart();
    {
        if (init->ADC_Struct.SampleCount >= init->ADC_Struct.SampleNeed) /* 等待转换完成 */
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
    ANAC_ADCCON_ADC_IE_Setable(DISABLE);  /* 中断禁止 */
    ANAC_ADCCON_ADC_EN_Setable(DISABLE);  /* ADC关闭 */
    ANAC_ADCINSEL_BUFEN_Setable(DISABLE); /* 不关就有额外的功耗 */
    return fVlotage;
}
/*
*********************************************************************************************************
*	函 数 名: API_Temperature
*	功能说明: ADC温度传感器数据采集  详情看API_Exp中ADC的例程
*	形    参: ADC_INIT_Type 型结构体变量  具体看结构体成员变量
*	返 回 值: AD采样值
*	备    注：无
*********************************************************************************************************
*/
float API_Temperature(ADC_INIT_Type *init)
{
    uint8_t i;
    volatile float fTemperature = 0;
    float fTempADC = 0;

    G_ADC_Init(init); /* 配置ADC */
    memset(init->ADC_Struct.Bufu16, 0, SampleMax);

    if (init->SampleNeed > SampleMax)
        init->SampleNeed = SampleMax - 1;
    if (init->SampleNeed < 2)
        init->SampleNeed = 2;

    init->ADC_Struct.SampleNeed = init->SampleNeed; /* 需要采样数据的个数,第一个采样结果丢弃 */

    init->ADC_Struct.SampleCount = 0;   /* 清零采样计数 */
    ANAC_ADCCON_ADC_EN_Setable(ENABLE); /* ADC启动 */

    Do_DelayStart();
    {
        if (init->ADC_Struct.SampleCount >= init->ADC_Struct.SampleNeed) /* 等待转换完成 */
        {
            fTempADC = 0;
            for (i = 1; i < init->SampleNeed; i++)
            {
                fTempADC += init->ADC_Struct.Bufu16[i];
            }

            fTempADC = fTempADC / (init->SampleNeed - 1);

            if (init->SampleLevel == 1)
                fTemperature = ANAC_ADC_TemperatureCalc(fTempADC, 5); /* AD值转换为温度 */
            else
                fTemperature = ANAC_ADC_TemperatureCalc(fTempADC, 3);

            break;
        }
    }
    While_DelayMsEnd(init->SampleNeed * 10);

    ANAC_ADCCON_ADC_IE_Setable(DISABLE);  /* 中断禁止 */
    ANAC_ADCCON_ADC_EN_Setable(DISABLE);  /* ADC关闭 */
    ANAC_ADCINSEL_BUFEN_Setable(DISABLE); /* 不关就有额外的功耗 */

    return fTemperature;
}

#endif
