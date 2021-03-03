/*
*********************************************************************************************************
*
*	模块名称 : LPTIM_BSP程序
*	文件名称 : lptim_api.c
*	版    本 : V1.0.0
*	说    明 : 低功耗定时器初始化
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "lptim_api.h"

#if API_UseLPTIM > 0
/*
*********************************************************************************************************
*	函 数 名: API_LPTIM_Start
*	功能说明: 低功耗定时器开启
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void API_LPTIM_Start(void)
{
    LPTIM_LPTIE_OVIE_Setable(DISABLE);    /* 使能LPTIMER的溢出中断 */
    LPTIM_LPTCTRL_LPTEN_Setable(DISABLE); /* LPTIMER模块使能 */
    LPTIM_LPTIE_OVIE_Setable(ENABLE);     /* 使能LPTIMER的溢出中断 */
    LPTIM_LPTCTRL_LPTEN_Setable(ENABLE);  /* LPTIMER模块使能 */
}
/*
*********************************************************************************************************
*	函 数 名: API_LPTIM_Stop
*	功能说明: 低功耗定时器关闭
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void API_LPTIM_Stop(void)
{
    LPTIM_LPTCTRL_LPTEN_Setable(DISABLE); /* LPTIMER模块关闭 */
}
/*
*********************************************************************************************************
*	函 数 名: API_LPTIM_Init
*	功能说明: 低功耗定时器初始化
*	形    参: LPTIM_INIT_Type *LPTIM_INIT_Type
*	返 回 值: 无
*	备    注：设置完LPTIM后不会立即启动定时器,需要手动开启
*********************************************************************************************************
*/
__weak void API_LPTIM_Init(LPTIM_INIT_Type *init)
{
    LPTIM_InitTypeDef init_para;

    RCC_PERCLK_SetableEx(LPTFCLK, ENABLE); /* LPTIM功能时钟使能 */
    RCC_PERCLK_SetableEx(LPTRCLK, ENABLE); /* LPTIM总线时钟使能 */

    init_para.LPTIM_TMODE = LPTIM_LPTCFG_TMODE_PWMIM;      /* 设置工作模式为带波形输出的普通定时器模式 */
    init_para.LPTIM_MODE = LPTIM_LPTCFG_MODE_CONTINUE;     /* 设置计数模式为连续计数模式 */
    init_para.LPTIM_PWM = LPTIM_LPTCFG_PWM_PWM;            /* 选择脉宽编码调制方式 */
    init_para.LPTIM_POLAR = LPTIM_LPTCFG_POLARITY_POS;     /* 第一次计数值=比较值是产生上升沿 */
    init_para.LPTIM_TRIG_CFG = LPTIM_LPTCFG_TRIGCFG_POS;   /* 外部输入信号上升沿trigger */
    init_para.LPTIM_FLTEN = ENABLE;                        /* 使能数字滤波 */
    init_para.LPTIM_CLK_SEL = LPTIM_LPTCFG_CLKSEL_LSCLK;   /* 选择LPTIMER的时钟源 */
    init_para.LPTIM_LPTIN_EDGE = LPTIM_LPTCFG_EDGESEL_POS; /* LPTIN的上升沿计数 */

    if (init->PRE < 8)
    {
        init_para.LPTIM_CLK_DIV = (uint32_t)init->PRE << LPTIM_LPTCFG_DIVSEL_Pos; /* 设置分频值 */
    }
    else
    {
        init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_128;
    }

    // switch (init->PRE)
    // {
    // case 0:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_1; //设置分频值
    //     break;
    // case 1:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_2; //设置分频值
    //     break;
    // case 2:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_4; //设置分频值
    //     break;
    // case 3:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_8; //设置分频值
    //     break;
    // case 4:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_16; //设置分频值
    //     break;
    // case 5:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_32; //设置分频值
    //     break;
    // case 6:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_64; //设置分频值
    //     break;
    // case 7:
    // default:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_128; //设置分频值
    //     break;
    // }

    init_para.LPTIM_compare_value = 0;          /* 设置比较值 */
    init_para.LPTIM_target_value = init->count; /* 设置目标值 */

    LPTIM_Init(&init_para);              /* 初始化LPTIMER */
    NVIC_SET(LPTIM_IRQn, init->Nvicpry); /* 使能LPTIMER的外设中断 */
    LPTIM_LPTIE_OVIE_Setable(ENABLE);    /* 使能LPTIMER的溢出中断 */
}

#endif
