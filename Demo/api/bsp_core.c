/*
*********************************************************************************************************
*
*	模块名称 : BSP基础程序
*	文件名称 : bsp_core.c
*	版    本 : V1.0.0
*	说    明 : BSP的基础，上电最初的初始化和时钟IO配置
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-1-6    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "bsp_core.h"

#ifdef SYSCORE

const uint8_t UpgradeCode[] = {0x68, 0x00, 0x06, 0x68, 0x15, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x50, 0x52, 0x16};
IOEXTI_Type IOEXTI; /* 跑系统需要读出中断线状态 */
/*
*********************************************************************************************************
*	函 数 名: 函数名称：GPIO_EXTI_EXTIxIF_ChkEx_Ucos
*	功能说明: 读取中断标志状态
*	形    参: *GPIOx：io的实际物理地址
*             GPIO_Pin：io口号
*	返 回 值: 1：产生中断,0:未产生中断
*	备    注：跑系统使用
*********************************************************************************************************
*/
FlagStatus GPIO_EXTI_EXTIxIF_ChkEx_Ucos(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
    uint32_t pin_num = 0, i;
    FlagStatus Result = RESET;

    for (i = 0; i < 16; i++)
    {
        if (GPIO_Pin & (1 << i))
        {
            pin_num = i;
            break;
        }
    }

    switch ((uint32_t)GPIOx)
    {
    case (uint32_t)GPIOA:
    case (uint32_t)GPIOB:
        if (IOEXTI.EXTI0IF & (0x00000001 << (pin_num & 0x07)))
        {
            Result = SET;
            IOEXTI.EXTI0IF ^= (0x00000001 << (pin_num & 0x07));
        }
        break;

    case (uint32_t)GPIOC:
        if (IOEXTI.EXTI1IF & (0x00000001 << (pin_num & 0x07)))
        {
            Result = SET;
            IOEXTI.EXTI1IF ^= (0x00000001 << (pin_num & 0x07));
        }
        break;

    case (uint32_t)GPIOD: /* 0~10 */
        if (pin_num <= 10)
        {
            if (IOEXTI.EXTI1IF & (0x00000001 << (pin_num & 0x07)))
            {
                Result = SET;
                IOEXTI.EXTI1IF ^= (0x00000001 << (pin_num & 0x07));
            }
        }
        break;

    case (uint32_t)GPIOE: /* 2~9(-2) */
        if ((pin_num >= 2) && (pin_num <= 9))
        {
            if (IOEXTI.EXTI2IF & (0x00000001 << (pin_num - 2)))
            {
                Result = SET;
                IOEXTI.EXTI2IF ^= (0x00000001 << (pin_num - 2));
            }
        }
        break;

    case (uint32_t)GPIOF: /* 1~7   8~11(-4) */
        if ((pin_num >= 1) && (pin_num <= 7))
        {
            if (IOEXTI.EXTI2IF & (0x00000001 << (pin_num & 0x07)))
            {
                Result = SET;
                IOEXTI.EXTI2IF ^= (0x00000001 << (pin_num & 0x07));
            }
        }
        else if ((pin_num >= 8) && (pin_num <= 11))
        {
            if (IOEXTI.EXTI2IF & (0x00000001 << (pin_num - 4)))
            {
                Result = SET;
                IOEXTI.EXTI2IF ^= (0x00000001 << (pin_num - 4));
            }
        }
        break;

    case (uint32_t)GPIOG: /* 2(+1) 5~8(-1) */
        if (pin_num == 2)
        {
            if (IOEXTI.EXTI1IF & (0x00000001 << (pin_num + 1)))
            {
                Result = SET;
                IOEXTI.EXTI1IF ^= (0x00000001 << (pin_num + 1));
            }
        }
        else if ((pin_num >= 5) && (pin_num <= 8))
        {
            if (IOEXTI.EXTI1IF & (0x00000001 << (pin_num - 1)))
            {
                Result = SET;
                IOEXTI.EXTI1IF ^= (0x00000001 << (pin_num - 1));
            }
        }
        break;

    default:
        break;
    }

    return Result;
}
/*
*********************************************************************************************************
*	函 数 名: API_GPIO_Judge
*	功能说明: IO中断判断函数
*	形    参: *GPIOx：io的实际物理地址
*             GPIO_Pin：io口号
*	返 回 值: 0：并未产生中断,1：下降沿中断，2：上升沿中断
*	备    注：无
*********************************************************************************************************
*/
char API_GPIO_Judge(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{

#if Use_uCOS > 0
    if (SET == GPIO_EXTI_EXTIxIF_ChkEx_Ucos(GPIOx, GPIO_Pin))
    {
#else
    if (SET == GPIO_EXTI_EXTIxIF_ChkEx(GPIOx, GPIO_Pin))
    {
        GPIO_EXTI_EXTIxIF_ClrEx(GPIOx, GPIO_Pin);
#endif
        if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == RESET)
        {
            delay_ms(SoftDelayTime); /* 软件延时去抖 */
            if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == RESET)
            {
                return 1; /* 下降沿中断 */
            }
        }
        else
        {
            delay_ms(SoftDelayTime); /* 软件延时去抖 */
            if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == SET)
            {
                return 2; /* 上升沿中断 */
            }
        }
    }
    return 0;
}

void NVIC_SET(IRQn_Type IRQn, uint32_t priority)
{
    if (priority > MAX_NVIC_PRIO)
        priority = MAX_NVIC_PRIO - 1;
    NVIC_DisableIRQ(IRQn);
    NVIC_SetPriority(IRQn, priority); /* 中断优先级配置 */
    NVIC_EnableIRQ(IRQn);
}

#if UseBootFixSys > 0 && ThisBoot == 0
/* 以下为固化函数部分，不作说明 */
uint32_t *sys_base = (uint32_t *)(Boot_SYS_Base_Adr); /* 系统函数表 */

void TicksDelayMs(uint32_t ms)
{
    gy_delay delayms = (void (*)(uint32_t time))(sys_base[SYS_DelayMs_Loc] | 0x1);
    delayms(ms);
}

void TicksDelayUs(uint32_t us)
{
    gy_delay delayus = (void (*)(uint32_t time))(sys_base[SYS_DelayUs_Loc] | 0x1);
    delayus(us);
}

void AnalogIO(GPIOx_Type *GPIOx, uint32_t PinNum)
{
    gy_tyioa analogio = (void (*)(GPIOx_Type * GPIOx, uint32_t PinNum))(sys_base[SYS_AnalogIO_Loc] | 0x1);
    analogio(GPIOx, PinNum);
}

void InputtIO(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type)
{
    gy_tyiob inputtio = (void (*)(GPIOx_Type * GPIOx, uint32_t PinNum, uint8_t Type))(sys_base[SYS_InputtIO_Loc] | 0x1);
    inputtio(GPIOx, PinNum, Type);
}

void OutputIO(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type)
{
    gy_tyiob outputio = (void (*)(GPIOx_Type * GPIOx, uint32_t PinNum, uint8_t Type))(sys_base[SYS_OutputIO_Loc] | 0x1);
    outputio(GPIOx, PinNum, Type);
}

void AltFunIO(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type)
{
    gy_tyiob altfunio = (void (*)(GPIOx_Type * GPIOx, uint32_t PinNum, uint8_t Type))(sys_base[SYS_AltfunIO_Loc] | 0x1);
    altfunio(GPIOx, PinNum, Type);
}

void CloseeIO(GPIOx_Type *GPIOx, uint32_t PinNum)
{
    gy_tyioa closeeio = (void (*)(GPIOx_Type * GPIOx, uint32_t PinNum))(sys_base[SYS_CloseeIO_Loc] | 0x1);
    closeeio(GPIOx, PinNum);
}

void Init_System(void)
{
    gy_initsystem initsystem = (void (*)(void))(sys_base[SYS_SysmInit_Loc] | 0x1);
    initsystem();
}

void Sleep(uint8_t sleepmode)
{
    gy_sleep sleep = (void (*)(uint8_t sleepmode))(sys_base[SYS_Sleep_Loc] | 0x1);
    sleep(sleepmode);
}

int32_t RchfAdjust(ETIMx_Type *ETIMx, uint8_t clkMode)
{
    gy_rchfadjust rchfadjust = (int32_t(*)(ETIMx_Type * ETIMx, uint8_t clkMode))(sys_base[SYS_RchfadJust_Loc] | 0x1);
    return rchfadjust(ETIMx, clkMode);
}
#else
#if ThisBoot > 0 && UseBootFixSys > 0
/*----------------------------------------------------------------------
//固化SYS函数表,不可更改
-----------------------------------------------------------------------*/
const uint32_t sys_table[] __attribute__((section(".ARM.__at_0x00000950"))) =
    {
        (uint32_t)&TicksDelayMs,
        (uint32_t)&TicksDelayUs,
        (uint32_t)&AnalogIO,
        (uint32_t)&InputtIO,
        (uint32_t)&OutputIO,
        (uint32_t)&AltFunIO,
        (uint32_t)&CloseeIO,
        (uint32_t)&Init_System,
        (uint32_t)&Sleep,
        (uint32_t)&RchfAdjust};
#endif

/*
*********************************************************************************************************
*	函 数 名: ET12_32bit
*	功能说明: 将定时器E1,E2联合扩展成32位定时器(E1,E2是两个16为定时器)
*	形    参: 无
*	返 回 值: 无
*	备    注：级联后ET1控制输入信号源和预分频，预置数与比较匹配则由ET12共同控制
*********************************************************************************************************
*/
void ET12_32bit(void)
{
    ETIM_InitTypeDef init_para;

    RCC_PERCLK_SetableEx(ET1CLK, ENABLE); /* ET1时钟使能 */
    NVIC_DisableIRQ(ETIM1_IRQn);          /* NVIC中断配置 */

    /* 时钟配置 */
    init_para.sig_src_para.SIG1SEL = ETIMx_ETxINSEL_SIG1SEL_GROUP1;     /* 内部信号1源选择GROUP1 */
    init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET1_APBCLK; /* GROUP1 信号选择APBCLK */
    init_para.sig_src_para.PRESCALE1 = RCHFCLKCFG - 1;                  /* 按时钟分频成1M */
    init_para.sig_src_para.PRESCALE2 = 0;                               /* 1分频 */

    /* ET1配置 */
    init_para.ctrl_para.EXFLT = ENABLE;                       /* 引脚输入数字滤波使能打开 */
    init_para.ctrl_para.MOD = ETIMx_ETxCR_MOD_COUNTER;        /* 工作模式选择计数模式 */
    init_para.ctrl_para.CASEN = ENABLE;                       /* 扩展定时器级联 */
    init_para.ctrl_para.EDGESEL = ETIMx_ETxCR_EDGESEL_RISING; /* 计数模式采沿方式选择（计数时钟选择mcu_clk时该位无效，总是采用mcu_clk时钟上升沿计数） */
    init_para.ctrl_para.CAPCLR = DISABLE;                     /* 带清零捕捉模式控制 */
    init_para.ctrl_para.INITVALUE = 0;                        /* ETx初值寄存器 */
    init_para.ctrl_para.CMP = 0;                              /* ETx比较寄存器 */
    init_para.ctrl_para.CEN = DISABLE;                        /* 启动控制 */
    ETIMx_Init(ETIM1, &init_para);                            /* 初始化ETIM1 */

    /* ET2配置 */
    RCC_PERCLK_SetableEx(ET2CLK, ENABLE); /* ET2时钟使能 */
    NVIC_DisableIRQ(ETIM2_IRQn);          /* NVIC中断配置 */
    init_para.ctrl_para.CMPIE = DISABLE;  /* 扩展定时器比较中断使能 */
    init_para.ctrl_para.CAPIE = DISABLE;  /* 扩展定时器捕捉中断使能 */
    init_para.ctrl_para.OVIE = DISABLE;   /* 扩展定时器溢出中断使能 */
    init_para.ctrl_para.INITVALUE = 0;    /* ETx初值寄存器 */
    init_para.ctrl_para.CMP = 0;          /* ETx比较寄存器 */
    init_para.ctrl_para.CEN = DISABLE;    /* 启动控制 */
    ETIMx_Init(ETIM2, &init_para);        /* 初始化ETIM2 */

    ETIMx_ETxCR_CEN_Setable(ETIM1, ENABLE); //启动定时器
    ETIMx_ETxCR_CEN_Setable(ETIM2, ENABLE); //启动定时器
}
/*
*********************************************************************************************************
*	函 数 名: uCosTimer
*	功能说明: ucos配置timer
*	形    参: 无
*	返 回 值: 无
*	备    注：由于ucos需要使用systick而复旦微自身软件延时使用该时钟，
*             因此需要重新配置，额外给软延时提供一个时钟
*             该时钟是将主频分频成1M  即定时出1us
*********************************************************************************************************
*/
void uCosTimer()
{
#if Use32BitEtim > 0
    ET12_32bit();
#else

    ETIM_InitTypeDef init_para;

    init_para.sig_src_para.SIG1SEL = ETIMx_ETxINSEL_SIG1SEL_GROUP1;     /* 内部信号1源选择GROUP1 */
    init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET4_APBCLK; /* GROUP1 信号选择APBCLK */
    RCC_PERCLK_SetableEx(ET4CLK, ENABLE);
    init_para.sig_src_para.PRESCALE1 = RCHFCLKCFG - 1;        /* 按时钟分频成1M */
    init_para.sig_src_para.PRESCALE2 = 0;                     /* 1分频 */
    init_para.ctrl_para.EXFLT = DISABLE;                      /* 引脚输入数字滤波 */
    init_para.ctrl_para.MOD = ETIMx_ETxCR_MOD_COUNTER;        /* 工作模式选择计数模式 */
    init_para.ctrl_para.CASEN = DISABLE;                      /* 扩展定时器级联关闭 */
    init_para.ctrl_para.EDGESEL = ETIMx_ETxCR_EDGESEL_RISING; /* 计数模式采沿方式选择（计数时钟选择mcu_clk时该位无效，总是采用mcu_clk时钟上升沿计数） */
    init_para.ctrl_para.CAPCLR = DISABLE;                     /* 带清零捕捉模式控制 */
    init_para.ctrl_para.CMPIE = DISABLE;                      /* 扩展定时器比较中断使能 */
    init_para.ctrl_para.CAPIE = DISABLE;                      /* 扩展定时器捕捉中断使能 */
    init_para.ctrl_para.OVIE = DISABLE;                       /* 扩展定时器溢出中断使能 */
    init_para.ctrl_para.INITVALUE = 0;                        /* 计数初值 */
    init_para.ctrl_para.CEN = DISABLE;                        /* 启动控制 */

    ETIMx_Init(ETIM4, &init_para);          /* 初始化ETIM */
    ETIMx_ETxCR_CEN_Setable(ETIM4, ENABLE); /* 启动定时器 */
#endif
}
/*
*********************************************************************************************************
*	函 数 名: Init_SysTick
*	功能说明: 给系统配置软件延时
*	形    参: 无
*	返 回 值: 无
*	备    注：不跑系统使用systick，跑系统使用ETMx。
*********************************************************************************************************
*/
void Init_SysTick(void)
{
#if Use_uCOS > 0
    uCosTimer();
#else
    SysTick_Config(0x1000000UL);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    //SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk; /* 关闭中断 */
#endif
}
/*
*********************************************************************************************************
*	函 数 名: TicksDelay
*	功能说明: 系统软件延时
*	形    参: ClkNum：要延时的时钟周期数
*	返 回 值: 无
*	备    注：调用软件延时前必须先配置SysTick
*********************************************************************************************************
*/
void TicksDelay(uint32_t ClkNum)
{
#if Use_uCOS > 0
#if Use32BitEtim > 0
    uint32_t last = ((uint32_t)ETCNT->ET2CNT << 16) + ETCNT->ET1CNT;
    if (ClkNum > 0xF0000000)
        ClkNum = 0xF0000000;
    while (((((uint32_t)ETCNT->ET2CNT << 16) + ETCNT->ET1CNT - last) & 0xFFFFFFFFUL) < ClkNum)
        ;
#else
    uint32_t last = ETCNT->ET4CNT;
    if (ClkNum > 0xF000)
        ClkNum = 0xF000;
    while (((ETCNT->ET4CNT - last) & 0xFFFFUL) < ClkNum)
        ;
#endif

#else
    uint32_t last = SysTick->VAL;

    if (clkmode != 4)
    {
        ClkNum = ClkNum * clkmode; /* 适应不同主频，8M时最大定时2ms，32M时最大定时0.5ms */
    }
    else
    {
        ClkNum = ClkNum * clkmode * 36 / 32;
    }
    if (ClkNum > 0xF00000)
    {
        ClkNum = 0xF00000;
    }
    while (((last - SysTick->VAL) & 0xFFFFFFUL) < ClkNum)
        ;

#endif
}
/*
*********************************************************************************************************
*	函 数 名: TicksDelayMs
*	功能说明: 系统软件延时ms
*	形    参: ms：要延时的ms数
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void TicksDelayMs(uint32_t ms)
{
    uint32_t ClkNum;
#if Use_uCOS > 0
    ClkNum = 1000;
#else
    ClkNum = (__SYSTEM_CLOCK / 1000);
#endif
    for (; ms > 0; ms--)
    {
        TicksDelay(ClkNum);
    }
}
/*
*********************************************************************************************************
*	函 数 名: TicksDelayUs
*	功能说明: 系统软件延时us
*	形    参: us：要延时的us数
*	返 回 值: 无
*	备    注：跑系统时：当不启用级联时，最大0xf000=61440=61ms，启用级联时最大0xF0000000=4,026,531,000=4026s
*             不跑系统时：不大于100ms
*********************************************************************************************************
*/
__weak void TicksDelayUs(uint32_t us)
{
#if Use_uCOS > 0
    TicksDelay(us);
#else
    uint32_t ClkNum;

    if (us > 100000)
    {
        us = 100000;
    }
    ClkNum = us * (__SYSTEM_CLOCK / 1000000);
    TicksDelay(ClkNum);
#endif
}
/*
*********************************************************************************************************
*	函 数 名: AnalogIO
*	功能说明: IO模拟功能配置:LCD/ADC
*	形    参: *GPIOx：IO的实际物理地址
*             PinNum：IO口号
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void AnalogIO(GPIOx_Type *GPIOx, uint32_t PinNum)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin = PinNum;
    GPIO_InitStructure.PxINEN = GPIO_IN_Dis;
    GPIO_InitStructure.PxODEN = GPIO_OD_En;
    GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
    GPIO_InitStructure.PxFCR = GPIO_FCR_ANA;

    GPIO_Init(GPIOx, &GPIO_InitStructure);
}
/*
*********************************************************************************************************
*	函 数 名: InputtIO
*	功能说明: IO输入口配置
*	形    参: *GPIOx：IO的实际物理地址
*             PinNum：IO口号
*             type：0 = 普通，1 = 上拉
*	返 回 值: 无
*	备    注：IN_NORMAL	0 ， IN_PULLUP	1
*********************************************************************************************************
*/
__weak void InputtIO(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin = PinNum;
    GPIO_InitStructure.PxINEN = GPIO_IN_En;
    GPIO_InitStructure.PxODEN = GPIO_OD_En;
    if (Type == IN_NORMAL)
        GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
    else
        GPIO_InitStructure.PxPUEN = GPIO_PU_En;
    GPIO_InitStructure.PxFCR = GPIO_FCR_IN;

    GPIO_Init(GPIOx, &GPIO_InitStructure);
}
/*
*********************************************************************************************************
*	函 数 名: OutputIO
*	功能说明: IO输出口配置
*	形    参: *GPIOx：IO的实际物理地址
*             PinNum：IO口号
*             type：0 = 普通，1 = OD
*	返 回 值: 无
*	备    注：OUT_PUSHPULL	0 ， OUT_OPENDRAIN	1
*********************************************************************************************************
*/
__weak void OutputIO(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin = PinNum;
    GPIO_InitStructure.PxINEN = GPIO_IN_Dis;
    if (Type == OUT_PUSHPULL)
        GPIO_InitStructure.PxODEN = GPIO_OD_Dis;
    else
        GPIO_InitStructure.PxODEN = GPIO_OD_En;
    GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
    GPIO_InitStructure.PxFCR = GPIO_FCR_OUT;

    GPIO_Init(GPIOx, &GPIO_InitStructure);
}
/*
*********************************************************************************************************
*	函 数 名: AltFunIO
*	功能说明: IO数字特殊功能口
*	形    参: *GPIOx：IO的实际物理地址
*             PinNum：IO口号
*             type：0 = 普通，1 = OD (OD功能仅部分特殊功能支持)，2 = 普通+上拉，3 = OD+上拉
*	返 回 值: 无
*	备    注：ALTFUN_NORMAL	0 ，ALTFUN_OPENDRAIN 1，ALTFUN_PULLUP  2 ，ALTFUN_OPENDRAIN_PULLUP  3
*********************************************************************************************************
*/
__weak void AltFunIO(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin = PinNum;
    GPIO_InitStructure.PxINEN = GPIO_IN_Dis;
    if ((Type & 0x01) == 0)
        GPIO_InitStructure.PxODEN = GPIO_OD_Dis;
    else
        GPIO_InitStructure.PxODEN = GPIO_OD_En;
    if ((Type & 0x02) == 0)
        GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
    else
        GPIO_InitStructure.PxPUEN = GPIO_PU_En;
    GPIO_InitStructure.PxFCR = GPIO_FCR_DIG;

    GPIO_Init(GPIOx, &GPIO_InitStructure);
}
/*
*********************************************************************************************************
*	函 数 名: CloseeIO
*	功能说明: 关闭IO（od输出高），降低功耗
*	形    参: *GPIOx：IO的实际物理地址
*             PinNum：IO口号
*	返 回 值: 无
*	备    注：当输入使能有效时，如果外部信号浮空，也可能导致FM385管脚漏电；
*             可以将FCR配置为01（GPIO输出），ODEN配置为1，即伪开漏输出，关闭上拉使能，并将输出数据设为1
*             注意SWD接口的PG8,9如果程序改变它们的配置将无法仿真
*********************************************************************************************************
*/
__weak void CloseeIO(GPIOx_Type *GPIOx, uint32_t PinNum)
{
    GPIO_InitTypeDef GPIO_InitStructureRun;

    GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

    if ((GPIO_InitStructureRun.PxFCR != GPIO_FCR_OUT))
    {
        GPIO_SetBits(GPIOx, PinNum);
        OutputIO(GPIOx, PinNum, OUT_OPENDRAIN);
    }
    else
    {
        OutputIO(GPIOx, PinNum, OUT_OPENDRAIN);
        GPIO_SetBits(GPIOx, PinNum);
    }
}
/*
*********************************************************************************************************
*	函 数 名: Close_AllIO_GPIO_80pin
*	功能说明: 关闭80脚芯片可使用的全部IO(SWD口除外，关闭后不能仿真)
*	形    参: 无
*	返 回 值: 无
*	备    注：注意SWD接口的PG8,9如果程序改变它们的配置将无法仿真
*********************************************************************************************************
*/
void Close_AllIO_GPIO_80pin(void)
{
    GPIO_ALL_Deinit();

    CloseeIO(GPIOA, GPIO_Pin_All);
    CloseeIO(GPIOB, GPIO_Pin_All);
    CloseeIO(GPIOC, GPIO_Pin_All);
    CloseeIO(GPIOD, GPIO_Pin_All);
    CloseeIO(GPIOE, GPIO_Pin_All);
    CloseeIO(GPIOF, GPIO_Pin_All);
    CloseeIO(GPIOG, GPIO_Pin_All);

    AltFunIO(GPIOG, GPIO_Pin_8, ALTFUN_NORMAL); /* PG8 SWDTCK */
    AltFunIO(GPIOG, GPIO_Pin_9, ALTFUN_NORMAL); /* PG9 SWDTDO */
}
/*
*********************************************************************************************************
*	函 数 名: Init_RCC_PERIPH_clk
*	功能说明: 外设时钟配置
*	形    参: 无
*	返 回 值: 无
*	备    注：默认开启大部分外设时钟，可按需求关闭不需要的时钟，时钟开启关闭对功耗影响不大
*********************************************************************************************************
*/
void Init_RCC_PERIPH_clk(void)
{
    RCC->PERCLKCON1 = 0xffffffff;
    RCC->PERCLKCON2 = 0xffffffff;
    RCC->PERCLKCON3 = 0xffffffff;
    RCC->PERCLKCON4 = 0xffffffff;
}
/*
*********************************************************************************************************
*	函 数 名: Init_PLL
*	功能说明: PLL时钟配置
*	形    参: 无
*	返 回 值: 无
*	备    注：默认使用RCHF，如果有特效需求在配置PLL然后调用
*********************************************************************************************************
*/
void Init_PLL(void)
{
    RCC_PLL_InitTypeDef PLL_InitStruct;

    PLL_InitStruct.PLLDB = 499;                         /* pll倍频数 = PLLDB + 1 */
    PLL_InitStruct.PLLINSEL = RCC_PLLCON_PLLINSEL_XTLF; /* PLL时钟源选择XTLF */
    PLL_InitStruct.PLLOSEL = RCC_PLLCON_PLLOSEL_MUL1;   /* 默认选择1倍输出，当超出PLLDB的1023时，可使用2倍输出实现更高的倍频 */
    PLL_InitStruct.PLLEN = DISABLE;                     /* 默认关闭PLL */

    RCC_PLL_Init(&PLL_InitStruct);
    RCC_PLLCON_PLLEN_Setable(DISABLE); /* 关闭PLL */
}
/*
*********************************************************************************************************
*	函 数 名: Init_SysClk
*	功能说明: 系统时钟配置
*	形    参: 无
*	返 回 值: 无
*	备    注：使用RCHF做主时钟,define_all.h 中SYSCLKdef宏控制系统时钟频率
*********************************************************************************************************
*/
void Init_SysClk(void)
{
    RCC_RCHF_InitTypeDef RCHF_InitStruct;
    RCC_SYSCLK_InitTypeDef SYSCLK_InitStruct;

    RCHF_InitStruct.FSEL = SYSCLKdef;
    RCHF_InitStruct.RCHFEN = ENABLE;

    RCC_RCHF_Init(&RCHF_InitStruct);

    SYSCLK_InitStruct.SYSCLKSEL = RCC_SYSCLKSEL_SYSCLKSEL_RCHF;   /* 选择RCHF做主时钟 */
    SYSCLK_InitStruct.AHBPRES = RCC_SYSCLKSEL_AHBPRES_DIV1;       /* AHB不分频 */
    SYSCLK_InitStruct.APBPRES = RCC_SYSCLKSEL_APBPRES_DIV1;       /* APB不分频 */
    SYSCLK_InitStruct.EXTICKSEL = RCC_SYSCLKSEL_EXTICKSEL_AHBCLK; /* EXTI,数字滤波时钟使用AHB时钟 */
    SYSCLK_InitStruct.SLP_ENEXTI = ENABLE;                        /* 休眠模式使能外部中断采样 */
    SYSCLK_InitStruct.LPM_RCLP_OFF = DISABLE;                     /* 休眠模式下不开启RCLP */

    RCC_SysClk_Init(&SYSCLK_InitStruct);
}
/*
*********************************************************************************************************
*	函 数 名: SCU_Init
*	功能说明: 仿真使用
*	形    参: Mode：看备注
*	返 回 值: 无
*	备    注：Mode:0 仿真模式下运行看门狗，运行所有定时器
*             Mode:1 仿真模式关闭看门狗，运行所有定时器
*********************************************************************************************************
*/
void SCU_Init(uint8_t Mode)
{
    if (Mode == 1) /* debug */
    {
        SCU_MCUDBGCR_DBG_WWDT_STOP_Setable(ENABLE); /* 仿真模式下关闭WWDT */
        SCU_MCUDBGCR_DBG_IWDT_STOP_Setable(ENABLE); /* 仿真模式下关闭IWDT */
    }
    else /* release */
    {
        SCU_MCUDBGCR_DBG_WWDT_STOP_Setable(DISABLE); /* 仿真模式下运行WWDT */
        SCU_MCUDBGCR_DBG_IWDT_STOP_Setable(DISABLE); /* 仿真模式下运行IWDT */
    }

    SCU_MCUDBGCR_DBG_ET4_STOP_Setable(DISABLE); /* 仿真模式下运行ET4 */
    SCU_MCUDBGCR_DBG_ET3_STOP_Setable(DISABLE); /* 仿真模式下运行ET3 */
    SCU_MCUDBGCR_DBG_ET2_STOP_Setable(DISABLE); /* 仿真模式下运行ET2 */
    SCU_MCUDBGCR_DBG_ET1_STOP_Setable(DISABLE); /* 仿真模式下运行ET1 */
    SCU_MCUDBGCR_DBG_BT2_STOP_Setable(DISABLE); /* 仿真模式下运行BT2 */
    SCU_MCUDBGCR_DBG_BT1_STOP_Setable(DISABLE); /* 仿真模式下运行BT1 */
}
/*
*********************************************************************************************************
*	函 数 名: Init_SysClk_Gen
*	功能说明: 时钟选择相关
*	形    参: 无
*	返 回 值: 无
*	备    注：PLL默认关闭，默认使用RCHF做主时钟，默认开启大部分外设时钟，默认AHB Master优先级配置DMA优先
*            下电复位配置
*            pdr和bor两个下电复位至少要打开一个
*            当电源电压低于下电复位电压时，芯片会被复位住
*            pdr电压档位不准但是功耗极低（几乎无法测量）
*            bor电压档位准确但是需要增加2uA功耗
*********************************************************************************************************
*/
void Init_SysClk_Gen(void)
{
    /* 系统时钟超过24M后需要打开wait1 */
    if (RCHFCLKCFG > 24)
        FLASH_FLSRDCON_WAIT_Set(FLASH_FLSRDCON_WAIT_1CYCLE);
    else
        FLASH_FLSRDCON_WAIT_Set(FLASH_FLSRDCON_WAIT_0CYCLE);

    RCC_PLLCON_PLLEN_Setable(DISABLE);        /* PLL关闭 */
    Init_SysClk();                            /* 系统时钟配置 */
    Init_RCC_PERIPH_clk();                    /* 外设时钟使能配置 */
    RCC_MPRIL_MPRIL_Set(RCC_MPRIL_MPRIL_DMA); /* DMA访问RAM优先级配置 */
    ANAC_PDRCON_PDREN_Setable(ENABLE);        /* 打开PDR */
    ANAC_BORCON_OFF_BOR_Setable(DISABLE);     /* 打开BOR */

#ifdef __DEBUG
    SCU_Init(1);                              /*仿真控制寄存器配置*/
#else
    SCU_Init(0);
#endif
}
/*
*********************************************************************************************************
*	函 数 名: IWDT_Init
*	功能说明: 看门狗配置
*	形    参: 无
*	返 回 值: 无
*	备    注：看门狗配置成8s
*********************************************************************************************************
*/
void IWDT_Init(void)
{
    RCC_PERCLK_SetableEx(IWDTCLK, ENABLE);             /* IWDT总线时钟使能 */
    IWDT_Clr();                                        /* 清IWDT */
    IWDT_IWDTCFG_IWDTOVP_Set(IWDT_IWDTCFG_IWDTOVP_8s); /* 配置IWDT溢出周期 */
    IWDT_IWDTCFG_IWDTSLP4096S_Setable(DISABLE);        /* 配置休眠时是否启用4096s长周期 */
}
/*
*********************************************************************************************************
*	函 数 名: ETIMxCapture
*	功能说明: 利用ETIM校准RCHF
*	形    参: *ETIMx：实际的ETIM物理地址
*	返 回 值: 无
*	备    注：如果用了某定时器注意不要重复使用那个定时器
*********************************************************************************************************
*/
static int32_t ETIMxCapture(ETIMx_Type *ETIMx)
{
    int32_t n, temp = -1;

    ETIMx_ETxIF_CAPIF_Clr(ETIMx);           /* 清除捕捉中断标志 */
    ETIMx_ETxCR_CEN_Setable(ETIMx, ENABLE); /* 启动定时器 */
    /* 理论上判断需要3.906ms = 128/32768 s */
    for (n = 0; n < 2; n++)
    {
        Do_DelayStart();
        {
            if (SET == ETIMx_ETxIF_CAPIF_Chk(ETIMx))
            {
                ETIMx_ETxIF_CAPIF_Clr(ETIMx);             /* 清除捕捉中断标志 */
                temp = ETIMx_ETxIVR_Read(ETIMx) & 0xffff; /* 读取捕获值 */
                break;
            }
        }
        While_DelayMsEnd(8 * clkmode);
    }
    ETIMx_ETxCR_CEN_Setable(ETIMx, DISABLE); /* 关闭定时器 */

    return temp;
}
/*
*********************************************************************************************************
*	函 数 名: ETIMxConfig
*	功能说明: 利用ETIM校准RCHF
*	形    参: *ETIMx：实际的ETIM物理地址
*	返 回 值: 无
*	备    注：如果用了某定时器注意不要重复使用那个定时器
*********************************************************************************************************
*/
static void ETIMxConfig(ETIMx_Type *ETIMx)
{
    ETIM_InitTypeDef init_para;
    volatile uint8_t EtimNum;

    EtimNum = ((uint32_t)ETIMx - ETIMER1_BASE) >> 5;

    init_para.sig_src_para.SIG1SEL = ETIMx_ETxINSEL_SIG1SEL_GROUP1; /* 内部信号1源选择GROUP1 */
    init_para.sig_src_para.SIG2SEL = ETIMx_ETxINSEL_SIG2SEL_GROUP2; /* 内部信号2源选择GROUP2 */

    switch (EtimNum)
    {
#if UseuCOS > 0 && Use32BitEtim == 0
    case 0:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET1_APBCLK; /* GROUP1 信号选择APBCLK */
        init_para.sig_src_para.GRP2SEL = ETIMx_ETxINSEL_GRP2SEL_ET1_XTLF;   /* GROUP2 信号选择XTLF */
        RCC_PERCLK_SetableEx(ET1CLK, ENABLE);
        NVIC_DisableIRQ(ETIM1_IRQn);
        break;

    case 1:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET2_APBCLK; /* GROUP1 信号选择APBCLK */
        init_para.sig_src_para.GRP2SEL = ETIMx_ETxINSEL_GRP2SEL_ET2_XTLF;   /* GROUP2 信号选择XTLF */
        RCC_PERCLK_SetableEx(ET2CLK, ENABLE);
        NVIC_DisableIRQ(ETIM2_IRQn);
        break;
#endif
#if Use_UartTimeOut == 0
    case 2:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET3_APBCLK; /* GROUP1 信号选择APBCLK */
        init_para.sig_src_para.GRP2SEL = ETIMx_ETxINSEL_GRP2SEL_ET3_XTLF;   /* GROUP2 信号选择XTLF */
        RCC_PERCLK_SetableEx(ET3CLK, ENABLE);
        NVIC_DisableIRQ(ETIM3_IRQn);
        break;
#endif
#if UseuCOS > 0 && Use32BitEtim == 1
    case 3:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET4_APBCLK; /* GROUP1 信号选择APBCLK */
        init_para.sig_src_para.GRP2SEL = ETIMx_ETxINSEL_GRP2SEL_ET4_XTLF;   /* GROUP2 信号选择XTLF */
        RCC_PERCLK_SetableEx(ET4CLK, ENABLE);
        NVIC_DisableIRQ(ETIM4_IRQn);
        break;
#endif
    default:
        break;
    }
    init_para.sig_src_para.PRESCALE1 = 0;   /* ETx预分频寄存器1 1分频 */
    init_para.sig_src_para.PRESCALE2 = 127; /* ETx预分频寄存器2 128分频 */

    init_para.ctrl_para.EXFLT = ENABLE;                       /* 引脚输入数字滤波使能打开 */
    init_para.ctrl_para.MOD = ETIMx_ETxCR_MOD_CAPTURE;        /* 工作模式选择捕捉模式 */
    init_para.ctrl_para.CASEN = DISABLE;                      /* 扩展定时器级联关闭 */
    init_para.ctrl_para.EDGESEL = ETIMx_ETxCR_EDGESEL_RISING; /* 计数模式采沿方式选择 */
    init_para.ctrl_para.PWM = DISABLE;                        /* PWM输出关闭 */
    init_para.ctrl_para.CAPMOD = ETIMx_ETxCR_CAPMOD_PERIOD;   /* 捕捉模式控制 */
    init_para.ctrl_para.CAPCLR = ENABLE;                      /* 带清零捕捉模式控制 */
    init_para.ctrl_para.CAPONCE = ENABLE;                     /* 单次捕捉控制 */
    init_para.ctrl_para.CAPEDGE = ETIMx_ETxCR_CAPEDGE_RISING; /* 捕捉沿选择 */
    init_para.ctrl_para.CMPIE = DISABLE;                      /* 扩展定时器比较中断使能 */
    init_para.ctrl_para.CAPIE = DISABLE;                      /* 扩展定时器捕捉中断使能 */
    init_para.ctrl_para.OVIE = DISABLE;                       /* 扩展定时器溢出中断使能 */
    init_para.ctrl_para.INITVALUE = 0;                        /* ETx初值寄存器 */
    init_para.ctrl_para.CMP = 0;                              /* ETx比较寄存器 */
    init_para.ctrl_para.CEN = DISABLE;                        /* 启动控制 */

    ETIMx_Init(ETIMx, &init_para);
}
/*
*********************************************************************************************************
*	函 数 名: RchfAdjust
*	功能说明: 利用ETIM校准RCHF
*	形    参: *ETIMx：实际的ETIM物理地址
*            clkMode 1 = 8M,2 = 16M,3 = 24M,4 = 32M
*	返 回 值: 略
*	备    注：如果用了某定时器注意不要重复使用那个定时器
*********************************************************************************************************
*/
__weak int32_t RchfAdjust(ETIMx_Type *ETIMx, uint8_t clkMode)
{
    const uint32_t TRY_TIMES = 5; /* 捕获超时时尝试次数 */
    uint32_t i, rchfTrim, tempbyte;
    int32_t captureTemp;
    float DIVclkmode = 1.0;

    if (RESET == ANAC_FDETIF_FDETO_Chk())
        return -1; /* 检测到XTLF停振，直接退出 */

    ETIMxConfig(ETIMx);
    switch (clkMode)
    {
    case 1:
        ETIMx_ETxPESCALE1_Write(ETIMx, 0); /* 8M 计数源1分频*/
        DIVclkmode = 1.0;
        break;

    case 2:
        ETIMx_ETxPESCALE1_Write(ETIMx, 1); /* 16M 计数源2分频 */
        DIVclkmode = 1.0;
        break;

    case 3:
        ETIMx_ETxPESCALE1_Write(ETIMx, 3); /* 24M 计数源4分频 */
        DIVclkmode = 8.0 / 6.0;
        break;

    case 4:
        ETIMx_ETxPESCALE1_Write(ETIMx, 3); /* 36M 计数源4分频 */
        DIVclkmode = 8.0 / 9.0;
        break;

    default:
        return -2; /* 非法参数，直接退出 */
    }

    for (i = 0; i < TRY_TIMES; i++)
    {
        ANAC_FDETIF_FDETIF_Clr();
        captureTemp = ETIMxCapture(ETIMx);
        if (SET == ANAC_FDETIF_FDETIF_Chk())
            return -3; /* 捕获期间停止标志置起直接退出 */

        if (captureTemp <= 0)
            continue;

        if ((captureTemp > (31250 - 120) / DIVclkmode) &&
            (captureTemp < (31250 + 120) / DIVclkmode))
        {
            break; /* 修正到理想值直接返回 */
        }
        else
        {
            rchfTrim = RCC_RCHFTRIM_Read(); /* 读取RCHF校准值 */

            /* 31250 = 8M * 128/32768 */
            /* 156.25 = (32768 / 8M) * 40k */
            if (captureTemp > 31250 / DIVclkmode)
            {
                captureTemp = captureTemp - 31250 / DIVclkmode;
                tempbyte = (float)captureTemp / (156.25 / DIVclkmode) + 0.5;

                if (tempbyte > 0x20)
                {
                    tempbyte = 0x20;
                }

                /* 更新RCHF校准值 */
                if (rchfTrim >= tempbyte)
                {
                    RCC_RCHFTRIM_Write(rchfTrim - tempbyte);
                }
                else
                {
                    RCC_RCHFTRIM_Write(0x00);
                }
            }
            else
            {
                captureTemp = 31250 / DIVclkmode - captureTemp;
                tempbyte = (float)captureTemp / (156.25 / DIVclkmode) + 0.5;

                if (tempbyte > 0x20)
                {
                    tempbyte = 0x20;
                }

                /* 更新RCHF校准值 */
                if ((rchfTrim + tempbyte) <= 0x7F)
                {
                    RCC_RCHFTRIM_Write(rchfTrim + tempbyte);
                }
                else
                {
                    RCC_RCHFTRIM_Write(0x7F);
                }
            }
        }
        /* 未找到理想值,采用最后2次的值取平均 */
        if (i == (TRY_TIMES - 1))
        {
            rchfTrim += RCC_RCHFTRIM_Read();
            RCC_RCHFTRIM_Write(rchfTrim / 2.0 + 0.5);
        }
    }

    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: Init_System
*	功能说明: 上电初始化
*	形    参: 无
*	返 回 值: 无
*	备    注：上电调用
*********************************************************************************************************
*/
__weak void Init_System(void)
{
    /*基础系统配置*/
    __disable_irq();             /* 关闭全局中断使能 */
    IWDT_Init();                 /* 系统看门狗设置 */
    IWDT_Clr();                  /* 清看门狗 */
    Init_SysClk_Gen();           /* 系统时钟配置 */
    Init_SysTick();              /* cpu滴答定时器配置(软件延时用) */
    RCC_Init_RCHF_Trim(clkmode); /* RCHF振荡器校准值载入，只是调校RCHF常温精度 */
    Close_AllIO_GPIO_80pin();    /* 外设初始化配置，关闭全部IO */
    RTC_ADJUST_Write(0);         /* RTC数值调整寄存器 RTC时钟温度补偿值写0，假如不操作调整寄存器，补偿值会是一个随机数，RTC时钟可能会偏差非常大 */
}
/*
*********************************************************************************************************
*	函 数 名: Sleep
*	功能说明: 休眠，进入低功耗
*	形    参: sleepmode：0 sleep，1 deepsleep
*	返 回 值: 无
*	备    注：休眠时SWD的输入脚如果浮空，可能会感应电平导致IO输出寄存器翻转引起功耗变大
*            如果SWD口无外部上拉的话，休眠前可开启上拉使能
*            下电复位配置
*            pdr和bor两个下电复位至少要打开一个
*            当电源电压低于下电复位电压时，芯片会被复位住
*            pdr电压档位不准但是功耗极低（几乎无法测量）
*            bor电压档位准确但是需要增加2uA功耗
*********************************************************************************************************
*/
__weak void Sleep(uint8_t sleepmode)
{
    PMU_SleepCfg_InitTypeDef SleepCfg_InitStruct;

    AltFunIO(GPIOG, GPIO_Pin_8, ALTFUN_PULLUP); /* PG8 SWDTCK */
    AltFunIO(GPIOG, GPIO_Pin_9, ALTFUN_PULLUP); /* PG9 SWDTDO */

    IWDT_IWDTCFG_IWDTSLP4096S_Setable(ENABLE); /* 配置休眠时是否启用4096s长周期 */

    ANAC_PDRCON_PDREN_Setable(ENABLE);               /* 打开PDR */
    ANAC_PDRCON_PDRCFG_Set(ANAC_PDRCON_PDRCFG_1P5V); /* pdr电压调整到1.5V */
    ANAC_BORCON_OFF_BOR_Setable(ENABLE);             /* 关闭BOR */

    RCC_SYSCLKSEL_LPM_RCLP_OFF_Setable(ENABLE); /* 关闭rclp 0.2uA */

    SleepCfg_InitStruct.PMOD = PMU_LPMCFG_PMOD_SLEEP; /* 功耗模式配置 */
    if (sleepmode == 0)
        SleepCfg_InitStruct.SLPDP = PMU_LPMCFG_SLPDP_SLEEP; /* Sleep控制 */
    else
        SleepCfg_InitStruct.SLPDP = PMU_LPMCFG_SLPDP_DEEPSLEEP;
    SleepCfg_InitStruct.CVS = DISABLE;                /* 内核电压降低控制 */
    SleepCfg_InitStruct.XTOFF = PMU_LPMCFG_XTOFF_DIS; /* 保持XTLF */
    SleepCfg_InitStruct.SCR = 0;                      /* M0系统控制寄存器，一般配置为0即可 */

    PMU_SleepCfg_Init(&SleepCfg_InitStruct); /* 休眠配置 */

    __enable_irq(); /* 打开全局中断使能 */
    IWDT_Clr();
    __WFI(); /* 进入休眠 */
    IWDT_Clr();
#if RCHFCLKCFG > 8
    Init_SysClk_Gen();
#endif
}
/*
*********************************************************************************************************
*	函 数 名: ReadResetState
*	功能说明: 获取复位状态
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
uint32_t ReadResetState(void)
{
    uint32_t ResetFlag = RCC->RSTFLAG; /* 读取复位原因 */
    RCC->RSTFLAG = 0xffff;             /* 清复位原因 */
    return ResetFlag;
}
/*
*********************************************************************************************************
*	函 数 名: GotoBoot
*	功能说明: 接收到上位机升级命令之后跳转到boot
*	形    参: *buf：数据缓存,len：数据长度，实际为定长13
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void GotoBoot(uint8_t *buf, uint8_t len)
{
    if (len != 13)
        return;
    if (camper(buf, (uint8_t *)&UpgradeCode, 13))
    {
        //debug("跳转升级");
        RCC->SOFTRST = 0x5c5caabb;
    }
}

#endif

#endif
