/*
*********************************************************************************************************
*
*	模块名称 : ETIM_BSP程序
*	文件名称 : bsp_etim.c
*	版    本 : V1.0.0
*	说    明 : ETIM定时模式初始化(使用中断方式)
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-12-31  高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "bsp_etim.h"

#if UseETIM1 > 0 || UseETIM2 > 0 || UseETIM3 > 0 || UseETIM4 > 0 || Use_UartTimeOut > 0 || ThisBoot == 1
/*
*********************************************************************************************************
*	函 数 名: API_ETim_Start
*	功能说明: 启动定时器
*	形    参: *ETIMx：定时器物理地址
*	返 回 值: 无
*	备    注：设置完ETIM后不会立即启动定时器,需要手动开启
*********************************************************************************************************
*/
void API_ETim_Start(ETIMx_Type *ETIMx)
{
    ETIMx_ETxCR_CEN_Setable(ETIMx, ENABLE);
}
/*
*********************************************************************************************************
*	函 数 名: API_ETim_Stop
*	功能说明: 停止定时器
*	形    参: *ETIMx：定时器物理地址
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void API_ETim_Stop(ETIMx_Type *ETIMx)
{
    ETIMx_ETxCR_CEN_Setable(ETIMx, DISABLE);
}
/*
*********************************************************************************************************
*	函 数 名: API_ETim_ReSet
*	功能说明: 重置定时器计数值
*	形    参: *ETIMx：定时器物理地址
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void API_ETim_ReSet(ETIMx_Type *ETIMx)
{
    ETIMx_ETxCR_CEN_Setable(ETIMx, DISABLE);
    ETIMx_ETxCR_CEN_Setable(ETIMx, ENABLE);
}
/*
*********************************************************************************************************
*	函 数 名: API_Etim_Init
*	功能说明: 定时器初始化
*	形    参: *ETIMx：定时器物理地址
*             ETIM_INIT_Type *init：初始化的结构体指针
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void API_Etim_Init(ETIMx_Type *ETIMx, ETIM_INIT_Type *init)
{
    ETIM_InitTypeDef init_para;
    volatile uint8_t EtimNum;
    EtimNum = ((uint32_t)ETIMx - ETIMER1_BASE) >> 5;
    init_para.sig_src_para.SIG1SEL = ETIMx_ETxINSEL_SIG1SEL_GROUP1; /* 内部信号1源选择GROUP1 */

    switch (EtimNum)
    {
#if Use_uCOS > 0 && Use32BitEtim == 0 || ThisBoot == 1
    case 0:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET1_APBCLK; /* 定时器1 */
        RCC_PERCLK_SetableEx(ET1CLK, ENABLE);
        NVIC_SET(ETIM1_IRQn, init->Nvicpry);
        break;
    case 1:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET2_APBCLK; /* 定时器2 */
        RCC_PERCLK_SetableEx(ET2CLK, ENABLE);
        NVIC_SET(ETIM2_IRQn, init->Nvicpry);
        break;
#endif
    case 2:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET3_APBCLK; /* 定时器3 */
        RCC_PERCLK_SetableEx(ET3CLK, ENABLE);
        NVIC_SET(ETIM3_IRQn, init->Nvicpry);
        break;
#if Use_uCOS > 0 && Use32BitEtim == 1 || ThisBoot == 1
    case 3:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET4_APBCLK; /* 定时器4 */
        RCC_PERCLK_SetableEx(ET4CLK, ENABLE);
        NVIC_SET(ETIM4_IRQn, init->Nvicpry);
        break;
#endif
    default:
        return;
    }
    if (init->PRE < 1)
        init->PRE = 1;
    init_para.sig_src_para.PRESCALE1 = clkmode + (init->PRE) - 2; /* ETx预分频寄存器1 00表示1分频，FF表示256分频*/
    init_para.sig_src_para.PRESCALE2 = clkmode - 1;               /* ETx预分频寄存器2 00表示1分频，FF表示256分频*/

    init_para.ctrl_para.EXFLT = ENABLE;                       /* 引脚输入数字滤波使能打开 */
    init_para.ctrl_para.MOD = ETIMx_ETxCR_MOD_COUNTER;        /* 工作模式选择计数模式 */
    init_para.ctrl_para.CASEN = DISABLE;                      /* 扩展定时器级联关闭 */
    init_para.ctrl_para.EDGESEL = ETIMx_ETxCR_EDGESEL_RISING; /* 计数模式采沿方式选择（计数时钟选择mcu_clk时该位无效，总是采用mcu_clk时钟上升沿计数） */
    init_para.ctrl_para.CAPCLR = DISABLE;                     /* 带清零捕捉模式控制 */

    init_para.ctrl_para.CMPIE = DISABLE; /* 扩展定时器比较中断使能 */
    init_para.ctrl_para.CAPIE = DISABLE; /* 扩展定时器捕捉中断使能 */
    init_para.ctrl_para.OVIE = ENABLE;   /* 扩展定时器溢出中断使能 */

    init_para.ctrl_para.INITVALUE = 0xffff - (init->count); /* ETx初值寄存器 */

    init_para.ctrl_para.CEN = DISABLE; /* 启动控制 */

    ETIMx_Init(ETIMx, &init_para);
}

#endif
