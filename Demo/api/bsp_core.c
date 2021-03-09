/*
*********************************************************************************************************
*
*	ģ������ : BSP��������
*	�ļ����� : bsp_core.c
*	��    �� : V1.0.0
*	˵    �� : BSP�Ļ������ϵ�����ĳ�ʼ����ʱ��IO����
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-1-6    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "bsp_core.h"

#ifdef SYSCORE

const uint8_t UpgradeCode[] = {0x68, 0x00, 0x06, 0x68, 0x15, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x50, 0x52, 0x16};
IOEXTI_Type IOEXTI; /* ��ϵͳ��Ҫ�����ж���״̬ */
/*
*********************************************************************************************************
*	�� �� ��: �������ƣ�GPIO_EXTI_EXTIxIF_ChkEx_Ucos
*	����˵��: ��ȡ�жϱ�־״̬
*	��    ��: *GPIOx��io��ʵ�������ַ
*             GPIO_Pin��io�ں�
*	�� �� ֵ: 1�������ж�,0:δ�����ж�
*	��    ע����ϵͳʹ��
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
*	�� �� ��: API_GPIO_Judge
*	����˵��: IO�ж��жϺ���
*	��    ��: *GPIOx��io��ʵ�������ַ
*             GPIO_Pin��io�ں�
*	�� �� ֵ: 0����δ�����ж�,1���½����жϣ�2���������ж�
*	��    ע����
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
            delay_ms(SoftDelayTime); /* �����ʱȥ�� */
            if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == RESET)
            {
                return 1; /* �½����ж� */
            }
        }
        else
        {
            delay_ms(SoftDelayTime); /* �����ʱȥ�� */
            if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == SET)
            {
                return 2; /* �������ж� */
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
    NVIC_SetPriority(IRQn, priority); /* �ж����ȼ����� */
    NVIC_EnableIRQ(IRQn);
}

#if UseBootFixSys > 0 && ThisBoot == 0
/* ����Ϊ�̻��������֣�����˵�� */
uint32_t *sys_base = (uint32_t *)(Boot_SYS_Base_Adr); /* ϵͳ������ */

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
//�̻�SYS������,���ɸ���
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
*	�� �� ��: ET12_32bit
*	����˵��: ����ʱ��E1,E2������չ��32λ��ʱ��(E1,E2������16Ϊ��ʱ��)
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��������ET1���������ź�Դ��Ԥ��Ƶ��Ԥ������Ƚ�ƥ������ET12��ͬ����
*********************************************************************************************************
*/
void ET12_32bit(void)
{
    ETIM_InitTypeDef init_para;

    RCC_PERCLK_SetableEx(ET1CLK, ENABLE); /* ET1ʱ��ʹ�� */
    NVIC_DisableIRQ(ETIM1_IRQn);          /* NVIC�ж����� */

    /* ʱ������ */
    init_para.sig_src_para.SIG1SEL = ETIMx_ETxINSEL_SIG1SEL_GROUP1;     /* �ڲ��ź�1Դѡ��GROUP1 */
    init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET1_APBCLK; /* GROUP1 �ź�ѡ��APBCLK */
    init_para.sig_src_para.PRESCALE1 = RCHFCLKCFG - 1;                  /* ��ʱ�ӷ�Ƶ��1M */
    init_para.sig_src_para.PRESCALE2 = 0;                               /* 1��Ƶ */

    /* ET1���� */
    init_para.ctrl_para.EXFLT = ENABLE;                       /* �������������˲�ʹ�ܴ� */
    init_para.ctrl_para.MOD = ETIMx_ETxCR_MOD_COUNTER;        /* ����ģʽѡ�����ģʽ */
    init_para.ctrl_para.CASEN = ENABLE;                       /* ��չ��ʱ������ */
    init_para.ctrl_para.EDGESEL = ETIMx_ETxCR_EDGESEL_RISING; /* ����ģʽ���ط�ʽѡ�񣨼���ʱ��ѡ��mcu_clkʱ��λ��Ч�����ǲ���mcu_clkʱ�������ؼ����� */
    init_para.ctrl_para.CAPCLR = DISABLE;                     /* �����㲶׽ģʽ���� */
    init_para.ctrl_para.INITVALUE = 0;                        /* ETx��ֵ�Ĵ��� */
    init_para.ctrl_para.CMP = 0;                              /* ETx�ȽϼĴ��� */
    init_para.ctrl_para.CEN = DISABLE;                        /* �������� */
    ETIMx_Init(ETIM1, &init_para);                            /* ��ʼ��ETIM1 */

    /* ET2���� */
    RCC_PERCLK_SetableEx(ET2CLK, ENABLE); /* ET2ʱ��ʹ�� */
    NVIC_DisableIRQ(ETIM2_IRQn);          /* NVIC�ж����� */
    init_para.ctrl_para.CMPIE = DISABLE;  /* ��չ��ʱ���Ƚ��ж�ʹ�� */
    init_para.ctrl_para.CAPIE = DISABLE;  /* ��չ��ʱ����׽�ж�ʹ�� */
    init_para.ctrl_para.OVIE = DISABLE;   /* ��չ��ʱ������ж�ʹ�� */
    init_para.ctrl_para.INITVALUE = 0;    /* ETx��ֵ�Ĵ��� */
    init_para.ctrl_para.CMP = 0;          /* ETx�ȽϼĴ��� */
    init_para.ctrl_para.CEN = DISABLE;    /* �������� */
    ETIMx_Init(ETIM2, &init_para);        /* ��ʼ��ETIM2 */

    ETIMx_ETxCR_CEN_Setable(ETIM1, ENABLE); //������ʱ��
    ETIMx_ETxCR_CEN_Setable(ETIM2, ENABLE); //������ʱ��
}
/*
*********************************************************************************************************
*	�� �� ��: uCosTimer
*	����˵��: ucos����timer
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע������ucos��Ҫʹ��systick������΢���������ʱʹ�ø�ʱ�ӣ�
*             �����Ҫ�������ã����������ʱ�ṩһ��ʱ��
*             ��ʱ���ǽ���Ƶ��Ƶ��1M  ����ʱ��1us
*********************************************************************************************************
*/
void uCosTimer()
{
#if Use32BitEtim > 0
    ET12_32bit();
#else

    ETIM_InitTypeDef init_para;

    init_para.sig_src_para.SIG1SEL = ETIMx_ETxINSEL_SIG1SEL_GROUP1;     /* �ڲ��ź�1Դѡ��GROUP1 */
    init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET4_APBCLK; /* GROUP1 �ź�ѡ��APBCLK */
    RCC_PERCLK_SetableEx(ET4CLK, ENABLE);
    init_para.sig_src_para.PRESCALE1 = RCHFCLKCFG - 1;        /* ��ʱ�ӷ�Ƶ��1M */
    init_para.sig_src_para.PRESCALE2 = 0;                     /* 1��Ƶ */
    init_para.ctrl_para.EXFLT = DISABLE;                      /* �������������˲� */
    init_para.ctrl_para.MOD = ETIMx_ETxCR_MOD_COUNTER;        /* ����ģʽѡ�����ģʽ */
    init_para.ctrl_para.CASEN = DISABLE;                      /* ��չ��ʱ�������ر� */
    init_para.ctrl_para.EDGESEL = ETIMx_ETxCR_EDGESEL_RISING; /* ����ģʽ���ط�ʽѡ�񣨼���ʱ��ѡ��mcu_clkʱ��λ��Ч�����ǲ���mcu_clkʱ�������ؼ����� */
    init_para.ctrl_para.CAPCLR = DISABLE;                     /* �����㲶׽ģʽ���� */
    init_para.ctrl_para.CMPIE = DISABLE;                      /* ��չ��ʱ���Ƚ��ж�ʹ�� */
    init_para.ctrl_para.CAPIE = DISABLE;                      /* ��չ��ʱ����׽�ж�ʹ�� */
    init_para.ctrl_para.OVIE = DISABLE;                       /* ��չ��ʱ������ж�ʹ�� */
    init_para.ctrl_para.INITVALUE = 0;                        /* ������ֵ */
    init_para.ctrl_para.CEN = DISABLE;                        /* �������� */

    ETIMx_Init(ETIM4, &init_para);          /* ��ʼ��ETIM */
    ETIMx_ETxCR_CEN_Setable(ETIM4, ENABLE); /* ������ʱ�� */
#endif
}
/*
*********************************************************************************************************
*	�� �� ��: Init_SysTick
*	����˵��: ��ϵͳ���������ʱ
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע������ϵͳʹ��systick����ϵͳʹ��ETMx��
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
                    SysTick_CTRL_ENABLE_Msk; /* �ر��ж� */
#endif
}
/*
*********************************************************************************************************
*	�� �� ��: TicksDelay
*	����˵��: ϵͳ�����ʱ
*	��    ��: ClkNum��Ҫ��ʱ��ʱ��������
*	�� �� ֵ: ��
*	��    ע�����������ʱǰ����������SysTick
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
        ClkNum = ClkNum * clkmode; /* ��Ӧ��ͬ��Ƶ��8Mʱ���ʱ2ms��32Mʱ���ʱ0.5ms */
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
*	�� �� ��: TicksDelayMs
*	����˵��: ϵͳ�����ʱms
*	��    ��: ms��Ҫ��ʱ��ms��
*	�� �� ֵ: ��
*	��    ע����
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
*	�� �� ��: TicksDelayUs
*	����˵��: ϵͳ�����ʱus
*	��    ��: us��Ҫ��ʱ��us��
*	�� �� ֵ: ��
*	��    ע����ϵͳʱ���������ü���ʱ�����0xf000=61440=61ms�����ü���ʱ���0xF0000000=4,026,531,000=4026s
*             ����ϵͳʱ��������100ms
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
*	�� �� ��: AnalogIO
*	����˵��: IOģ�⹦������:LCD/ADC
*	��    ��: *GPIOx��IO��ʵ�������ַ
*             PinNum��IO�ں�
*	�� �� ֵ: ��
*	��    ע����
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
*	�� �� ��: InputtIO
*	����˵��: IO���������
*	��    ��: *GPIOx��IO��ʵ�������ַ
*             PinNum��IO�ں�
*             type��0 = ��ͨ��1 = ����
*	�� �� ֵ: ��
*	��    ע��IN_NORMAL	0 �� IN_PULLUP	1
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
*	�� �� ��: OutputIO
*	����˵��: IO���������
*	��    ��: *GPIOx��IO��ʵ�������ַ
*             PinNum��IO�ں�
*             type��0 = ��ͨ��1 = OD
*	�� �� ֵ: ��
*	��    ע��OUT_PUSHPULL	0 �� OUT_OPENDRAIN	1
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
*	�� �� ��: AltFunIO
*	����˵��: IO�������⹦�ܿ�
*	��    ��: *GPIOx��IO��ʵ�������ַ
*             PinNum��IO�ں�
*             type��0 = ��ͨ��1 = OD (OD���ܽ��������⹦��֧��)��2 = ��ͨ+������3 = OD+����
*	�� �� ֵ: ��
*	��    ע��ALTFUN_NORMAL	0 ��ALTFUN_OPENDRAIN 1��ALTFUN_PULLUP  2 ��ALTFUN_OPENDRAIN_PULLUP  3
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
*	�� �� ��: CloseeIO
*	����˵��: �ر�IO��od����ߣ������͹���
*	��    ��: *GPIOx��IO��ʵ�������ַ
*             PinNum��IO�ں�
*	�� �� ֵ: ��
*	��    ע��������ʹ����Чʱ������ⲿ�źŸ��գ�Ҳ���ܵ���FM385�ܽ�©�磻
*             ���Խ�FCR����Ϊ01��GPIO�������ODEN����Ϊ1����α��©������ر�����ʹ�ܣ��������������Ϊ1
*             ע��SWD�ӿڵ�PG8,9�������ı����ǵ����ý��޷�����
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
*	�� �� ��: Close_AllIO_GPIO_80pin
*	����˵��: �ر�80��оƬ��ʹ�õ�ȫ��IO(SWD�ڳ��⣬�رպ��ܷ���)
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��ע��SWD�ӿڵ�PG8,9�������ı����ǵ����ý��޷�����
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
*	�� �� ��: Init_RCC_PERIPH_clk
*	����˵��: ����ʱ������
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��Ĭ�Ͽ����󲿷�����ʱ�ӣ��ɰ�����رղ���Ҫ��ʱ�ӣ�ʱ�ӿ����رնԹ���Ӱ�첻��
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
*	�� �� ��: Init_PLL
*	����˵��: PLLʱ������
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��Ĭ��ʹ��RCHF���������Ч����������PLLȻ�����
*********************************************************************************************************
*/
void Init_PLL(void)
{
    RCC_PLL_InitTypeDef PLL_InitStruct;

    PLL_InitStruct.PLLDB = 499;                         /* pll��Ƶ�� = PLLDB + 1 */
    PLL_InitStruct.PLLINSEL = RCC_PLLCON_PLLINSEL_XTLF; /* PLLʱ��Դѡ��XTLF */
    PLL_InitStruct.PLLOSEL = RCC_PLLCON_PLLOSEL_MUL1;   /* Ĭ��ѡ��1�������������PLLDB��1023ʱ����ʹ��2�����ʵ�ָ��ߵı�Ƶ */
    PLL_InitStruct.PLLEN = DISABLE;                     /* Ĭ�Ϲر�PLL */

    RCC_PLL_Init(&PLL_InitStruct);
    RCC_PLLCON_PLLEN_Setable(DISABLE); /* �ر�PLL */
}
/*
*********************************************************************************************************
*	�� �� ��: Init_SysClk
*	����˵��: ϵͳʱ������
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��ʹ��RCHF����ʱ��,define_all.h ��SYSCLKdef�����ϵͳʱ��Ƶ��
*********************************************************************************************************
*/
void Init_SysClk(void)
{
    RCC_RCHF_InitTypeDef RCHF_InitStruct;
    RCC_SYSCLK_InitTypeDef SYSCLK_InitStruct;

    RCHF_InitStruct.FSEL = SYSCLKdef;
    RCHF_InitStruct.RCHFEN = ENABLE;

    RCC_RCHF_Init(&RCHF_InitStruct);

    SYSCLK_InitStruct.SYSCLKSEL = RCC_SYSCLKSEL_SYSCLKSEL_RCHF;   /* ѡ��RCHF����ʱ�� */
    SYSCLK_InitStruct.AHBPRES = RCC_SYSCLKSEL_AHBPRES_DIV1;       /* AHB����Ƶ */
    SYSCLK_InitStruct.APBPRES = RCC_SYSCLKSEL_APBPRES_DIV1;       /* APB����Ƶ */
    SYSCLK_InitStruct.EXTICKSEL = RCC_SYSCLKSEL_EXTICKSEL_AHBCLK; /* EXTI,�����˲�ʱ��ʹ��AHBʱ�� */
    SYSCLK_InitStruct.SLP_ENEXTI = ENABLE;                        /* ����ģʽʹ���ⲿ�жϲ��� */
    SYSCLK_InitStruct.LPM_RCLP_OFF = DISABLE;                     /* ����ģʽ�²�����RCLP */

    RCC_SysClk_Init(&SYSCLK_InitStruct);
}
/*
*********************************************************************************************************
*	�� �� ��: SCU_Init
*	����˵��: ����ʹ��
*	��    ��: Mode������ע
*	�� �� ֵ: ��
*	��    ע��Mode:0 ����ģʽ�����п��Ź����������ж�ʱ��
*             Mode:1 ����ģʽ�رտ��Ź����������ж�ʱ��
*********************************************************************************************************
*/
void SCU_Init(uint8_t Mode)
{
    if (Mode == 1) /* debug */
    {
        SCU_MCUDBGCR_DBG_WWDT_STOP_Setable(ENABLE); /* ����ģʽ�¹ر�WWDT */
        SCU_MCUDBGCR_DBG_IWDT_STOP_Setable(ENABLE); /* ����ģʽ�¹ر�IWDT */
    }
    else /* release */
    {
        SCU_MCUDBGCR_DBG_WWDT_STOP_Setable(DISABLE); /* ����ģʽ������WWDT */
        SCU_MCUDBGCR_DBG_IWDT_STOP_Setable(DISABLE); /* ����ģʽ������IWDT */
    }

    SCU_MCUDBGCR_DBG_ET4_STOP_Setable(DISABLE); /* ����ģʽ������ET4 */
    SCU_MCUDBGCR_DBG_ET3_STOP_Setable(DISABLE); /* ����ģʽ������ET3 */
    SCU_MCUDBGCR_DBG_ET2_STOP_Setable(DISABLE); /* ����ģʽ������ET2 */
    SCU_MCUDBGCR_DBG_ET1_STOP_Setable(DISABLE); /* ����ģʽ������ET1 */
    SCU_MCUDBGCR_DBG_BT2_STOP_Setable(DISABLE); /* ����ģʽ������BT2 */
    SCU_MCUDBGCR_DBG_BT1_STOP_Setable(DISABLE); /* ����ģʽ������BT1 */
}
/*
*********************************************************************************************************
*	�� �� ��: Init_SysClk_Gen
*	����˵��: ʱ��ѡ�����
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��PLLĬ�Ϲرգ�Ĭ��ʹ��RCHF����ʱ�ӣ�Ĭ�Ͽ����󲿷�����ʱ�ӣ�Ĭ��AHB Master���ȼ�����DMA����
*            �µ縴λ����
*            pdr��bor�����µ縴λ����Ҫ��һ��
*            ����Դ��ѹ�����µ縴λ��ѹʱ��оƬ�ᱻ��λס
*            pdr��ѹ��λ��׼���ǹ��ļ��ͣ������޷�������
*            bor��ѹ��λ׼ȷ������Ҫ����2uA����
*********************************************************************************************************
*/
void Init_SysClk_Gen(void)
{
    /* ϵͳʱ�ӳ���24M����Ҫ��wait1 */
    if (RCHFCLKCFG > 24)
        FLASH_FLSRDCON_WAIT_Set(FLASH_FLSRDCON_WAIT_1CYCLE);
    else
        FLASH_FLSRDCON_WAIT_Set(FLASH_FLSRDCON_WAIT_0CYCLE);

    RCC_PLLCON_PLLEN_Setable(DISABLE);        /* PLL�ر� */
    Init_SysClk();                            /* ϵͳʱ������ */
    Init_RCC_PERIPH_clk();                    /* ����ʱ��ʹ������ */
    RCC_MPRIL_MPRIL_Set(RCC_MPRIL_MPRIL_DMA); /* DMA����RAM���ȼ����� */
    ANAC_PDRCON_PDREN_Setable(ENABLE);        /* ��PDR */
    ANAC_BORCON_OFF_BOR_Setable(DISABLE);     /* ��BOR */

#ifdef __DEBUG
    SCU_Init(1);                              /*������ƼĴ�������*/
#else
    SCU_Init(0);
#endif
}
/*
*********************************************************************************************************
*	�� �� ��: IWDT_Init
*	����˵��: ���Ź�����
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע�����Ź����ó�8s
*********************************************************************************************************
*/
void IWDT_Init(void)
{
    RCC_PERCLK_SetableEx(IWDTCLK, ENABLE);             /* IWDT����ʱ��ʹ�� */
    IWDT_Clr();                                        /* ��IWDT */
    IWDT_IWDTCFG_IWDTOVP_Set(IWDT_IWDTCFG_IWDTOVP_8s); /* ����IWDT������� */
    IWDT_IWDTCFG_IWDTSLP4096S_Setable(DISABLE);        /* ��������ʱ�Ƿ�����4096s������ */
}
/*
*********************************************************************************************************
*	�� �� ��: ETIMxCapture
*	����˵��: ����ETIMУ׼RCHF
*	��    ��: *ETIMx��ʵ�ʵ�ETIM�����ַ
*	�� �� ֵ: ��
*	��    ע���������ĳ��ʱ��ע�ⲻҪ�ظ�ʹ���Ǹ���ʱ��
*********************************************************************************************************
*/
static int32_t ETIMxCapture(ETIMx_Type *ETIMx)
{
    int32_t n, temp = -1;

    ETIMx_ETxIF_CAPIF_Clr(ETIMx);           /* �����׽�жϱ�־ */
    ETIMx_ETxCR_CEN_Setable(ETIMx, ENABLE); /* ������ʱ�� */
    /* �������ж���Ҫ3.906ms = 128/32768 s */
    for (n = 0; n < 2; n++)
    {
        Do_DelayStart();
        {
            if (SET == ETIMx_ETxIF_CAPIF_Chk(ETIMx))
            {
                ETIMx_ETxIF_CAPIF_Clr(ETIMx);             /* �����׽�жϱ�־ */
                temp = ETIMx_ETxIVR_Read(ETIMx) & 0xffff; /* ��ȡ����ֵ */
                break;
            }
        }
        While_DelayMsEnd(8 * clkmode);
    }
    ETIMx_ETxCR_CEN_Setable(ETIMx, DISABLE); /* �رն�ʱ�� */

    return temp;
}
/*
*********************************************************************************************************
*	�� �� ��: ETIMxConfig
*	����˵��: ����ETIMУ׼RCHF
*	��    ��: *ETIMx��ʵ�ʵ�ETIM�����ַ
*	�� �� ֵ: ��
*	��    ע���������ĳ��ʱ��ע�ⲻҪ�ظ�ʹ���Ǹ���ʱ��
*********************************************************************************************************
*/
static void ETIMxConfig(ETIMx_Type *ETIMx)
{
    ETIM_InitTypeDef init_para;
    volatile uint8_t EtimNum;

    EtimNum = ((uint32_t)ETIMx - ETIMER1_BASE) >> 5;

    init_para.sig_src_para.SIG1SEL = ETIMx_ETxINSEL_SIG1SEL_GROUP1; /* �ڲ��ź�1Դѡ��GROUP1 */
    init_para.sig_src_para.SIG2SEL = ETIMx_ETxINSEL_SIG2SEL_GROUP2; /* �ڲ��ź�2Դѡ��GROUP2 */

    switch (EtimNum)
    {
#if UseuCOS > 0 && Use32BitEtim == 0
    case 0:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET1_APBCLK; /* GROUP1 �ź�ѡ��APBCLK */
        init_para.sig_src_para.GRP2SEL = ETIMx_ETxINSEL_GRP2SEL_ET1_XTLF;   /* GROUP2 �ź�ѡ��XTLF */
        RCC_PERCLK_SetableEx(ET1CLK, ENABLE);
        NVIC_DisableIRQ(ETIM1_IRQn);
        break;

    case 1:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET2_APBCLK; /* GROUP1 �ź�ѡ��APBCLK */
        init_para.sig_src_para.GRP2SEL = ETIMx_ETxINSEL_GRP2SEL_ET2_XTLF;   /* GROUP2 �ź�ѡ��XTLF */
        RCC_PERCLK_SetableEx(ET2CLK, ENABLE);
        NVIC_DisableIRQ(ETIM2_IRQn);
        break;
#endif
#if Use_UartTimeOut == 0
    case 2:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET3_APBCLK; /* GROUP1 �ź�ѡ��APBCLK */
        init_para.sig_src_para.GRP2SEL = ETIMx_ETxINSEL_GRP2SEL_ET3_XTLF;   /* GROUP2 �ź�ѡ��XTLF */
        RCC_PERCLK_SetableEx(ET3CLK, ENABLE);
        NVIC_DisableIRQ(ETIM3_IRQn);
        break;
#endif
#if UseuCOS > 0 && Use32BitEtim == 1
    case 3:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET4_APBCLK; /* GROUP1 �ź�ѡ��APBCLK */
        init_para.sig_src_para.GRP2SEL = ETIMx_ETxINSEL_GRP2SEL_ET4_XTLF;   /* GROUP2 �ź�ѡ��XTLF */
        RCC_PERCLK_SetableEx(ET4CLK, ENABLE);
        NVIC_DisableIRQ(ETIM4_IRQn);
        break;
#endif
    default:
        break;
    }
    init_para.sig_src_para.PRESCALE1 = 0;   /* ETxԤ��Ƶ�Ĵ���1 1��Ƶ */
    init_para.sig_src_para.PRESCALE2 = 127; /* ETxԤ��Ƶ�Ĵ���2 128��Ƶ */

    init_para.ctrl_para.EXFLT = ENABLE;                       /* �������������˲�ʹ�ܴ� */
    init_para.ctrl_para.MOD = ETIMx_ETxCR_MOD_CAPTURE;        /* ����ģʽѡ��׽ģʽ */
    init_para.ctrl_para.CASEN = DISABLE;                      /* ��չ��ʱ�������ر� */
    init_para.ctrl_para.EDGESEL = ETIMx_ETxCR_EDGESEL_RISING; /* ����ģʽ���ط�ʽѡ�� */
    init_para.ctrl_para.PWM = DISABLE;                        /* PWM����ر� */
    init_para.ctrl_para.CAPMOD = ETIMx_ETxCR_CAPMOD_PERIOD;   /* ��׽ģʽ���� */
    init_para.ctrl_para.CAPCLR = ENABLE;                      /* �����㲶׽ģʽ���� */
    init_para.ctrl_para.CAPONCE = ENABLE;                     /* ���β�׽���� */
    init_para.ctrl_para.CAPEDGE = ETIMx_ETxCR_CAPEDGE_RISING; /* ��׽��ѡ�� */
    init_para.ctrl_para.CMPIE = DISABLE;                      /* ��չ��ʱ���Ƚ��ж�ʹ�� */
    init_para.ctrl_para.CAPIE = DISABLE;                      /* ��չ��ʱ����׽�ж�ʹ�� */
    init_para.ctrl_para.OVIE = DISABLE;                       /* ��չ��ʱ������ж�ʹ�� */
    init_para.ctrl_para.INITVALUE = 0;                        /* ETx��ֵ�Ĵ��� */
    init_para.ctrl_para.CMP = 0;                              /* ETx�ȽϼĴ��� */
    init_para.ctrl_para.CEN = DISABLE;                        /* �������� */

    ETIMx_Init(ETIMx, &init_para);
}
/*
*********************************************************************************************************
*	�� �� ��: RchfAdjust
*	����˵��: ����ETIMУ׼RCHF
*	��    ��: *ETIMx��ʵ�ʵ�ETIM�����ַ
*            clkMode 1 = 8M,2 = 16M,3 = 24M,4 = 32M
*	�� �� ֵ: ��
*	��    ע���������ĳ��ʱ��ע�ⲻҪ�ظ�ʹ���Ǹ���ʱ��
*********************************************************************************************************
*/
__weak int32_t RchfAdjust(ETIMx_Type *ETIMx, uint8_t clkMode)
{
    const uint32_t TRY_TIMES = 5; /* ����ʱʱ���Դ��� */
    uint32_t i, rchfTrim, tempbyte;
    int32_t captureTemp;
    float DIVclkmode = 1.0;

    if (RESET == ANAC_FDETIF_FDETO_Chk())
        return -1; /* ��⵽XTLFͣ��ֱ���˳� */

    ETIMxConfig(ETIMx);
    switch (clkMode)
    {
    case 1:
        ETIMx_ETxPESCALE1_Write(ETIMx, 0); /* 8M ����Դ1��Ƶ*/
        DIVclkmode = 1.0;
        break;

    case 2:
        ETIMx_ETxPESCALE1_Write(ETIMx, 1); /* 16M ����Դ2��Ƶ */
        DIVclkmode = 1.0;
        break;

    case 3:
        ETIMx_ETxPESCALE1_Write(ETIMx, 3); /* 24M ����Դ4��Ƶ */
        DIVclkmode = 8.0 / 6.0;
        break;

    case 4:
        ETIMx_ETxPESCALE1_Write(ETIMx, 3); /* 36M ����Դ4��Ƶ */
        DIVclkmode = 8.0 / 9.0;
        break;

    default:
        return -2; /* �Ƿ�������ֱ���˳� */
    }

    for (i = 0; i < TRY_TIMES; i++)
    {
        ANAC_FDETIF_FDETIF_Clr();
        captureTemp = ETIMxCapture(ETIMx);
        if (SET == ANAC_FDETIF_FDETIF_Chk())
            return -3; /* �����ڼ�ֹͣ��־����ֱ���˳� */

        if (captureTemp <= 0)
            continue;

        if ((captureTemp > (31250 - 120) / DIVclkmode) &&
            (captureTemp < (31250 + 120) / DIVclkmode))
        {
            break; /* ����������ֱֵ�ӷ��� */
        }
        else
        {
            rchfTrim = RCC_RCHFTRIM_Read(); /* ��ȡRCHFУ׼ֵ */

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

                /* ����RCHFУ׼ֵ */
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

                /* ����RCHFУ׼ֵ */
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
        /* δ�ҵ�����ֵ,�������2�ε�ֵȡƽ�� */
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
*	�� �� ��: Init_System
*	����˵��: �ϵ��ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע���ϵ����
*********************************************************************************************************
*/
__weak void Init_System(void)
{
    /*����ϵͳ����*/
    __disable_irq();             /* �ر�ȫ���ж�ʹ�� */
    IWDT_Init();                 /* ϵͳ���Ź����� */
    IWDT_Clr();                  /* �忴�Ź� */
    Init_SysClk_Gen();           /* ϵͳʱ������ */
    Init_SysTick();              /* cpu�δ�ʱ������(�����ʱ��) */
    RCC_Init_RCHF_Trim(clkmode); /* RCHF����У׼ֵ���룬ֻ�ǵ�УRCHF���¾��� */
    Close_AllIO_GPIO_80pin();    /* �����ʼ�����ã��ر�ȫ��IO */
    RTC_ADJUST_Write(0);         /* RTC��ֵ�����Ĵ��� RTCʱ���¶Ȳ���ֵд0�����粻���������Ĵ���������ֵ����һ���������RTCʱ�ӿ��ܻ�ƫ��ǳ��� */
}
/*
*********************************************************************************************************
*	�� �� ��: Sleep
*	����˵��: ���ߣ�����͹���
*	��    ��: sleepmode��0 sleep��1 deepsleep
*	�� �� ֵ: ��
*	��    ע������ʱSWD�������������գ����ܻ��Ӧ��ƽ����IO����Ĵ�����ת���𹦺ı��
*            ���SWD�����ⲿ�����Ļ�������ǰ�ɿ�������ʹ��
*            �µ縴λ����
*            pdr��bor�����µ縴λ����Ҫ��һ��
*            ����Դ��ѹ�����µ縴λ��ѹʱ��оƬ�ᱻ��λס
*            pdr��ѹ��λ��׼���ǹ��ļ��ͣ������޷�������
*            bor��ѹ��λ׼ȷ������Ҫ����2uA����
*********************************************************************************************************
*/
__weak void Sleep(uint8_t sleepmode)
{
    PMU_SleepCfg_InitTypeDef SleepCfg_InitStruct;

    AltFunIO(GPIOG, GPIO_Pin_8, ALTFUN_PULLUP); /* PG8 SWDTCK */
    AltFunIO(GPIOG, GPIO_Pin_9, ALTFUN_PULLUP); /* PG9 SWDTDO */

    IWDT_IWDTCFG_IWDTSLP4096S_Setable(ENABLE); /* ��������ʱ�Ƿ�����4096s������ */

    ANAC_PDRCON_PDREN_Setable(ENABLE);               /* ��PDR */
    ANAC_PDRCON_PDRCFG_Set(ANAC_PDRCON_PDRCFG_1P5V); /* pdr��ѹ������1.5V */
    ANAC_BORCON_OFF_BOR_Setable(ENABLE);             /* �ر�BOR */

    RCC_SYSCLKSEL_LPM_RCLP_OFF_Setable(ENABLE); /* �ر�rclp 0.2uA */

    SleepCfg_InitStruct.PMOD = PMU_LPMCFG_PMOD_SLEEP; /* ����ģʽ���� */
    if (sleepmode == 0)
        SleepCfg_InitStruct.SLPDP = PMU_LPMCFG_SLPDP_SLEEP; /* Sleep���� */
    else
        SleepCfg_InitStruct.SLPDP = PMU_LPMCFG_SLPDP_DEEPSLEEP;
    SleepCfg_InitStruct.CVS = DISABLE;                /* �ں˵�ѹ���Ϳ��� */
    SleepCfg_InitStruct.XTOFF = PMU_LPMCFG_XTOFF_DIS; /* ����XTLF */
    SleepCfg_InitStruct.SCR = 0;                      /* M0ϵͳ���ƼĴ�����һ������Ϊ0���� */

    PMU_SleepCfg_Init(&SleepCfg_InitStruct); /* �������� */

    __enable_irq(); /* ��ȫ���ж�ʹ�� */
    IWDT_Clr();
    __WFI(); /* �������� */
    IWDT_Clr();
#if RCHFCLKCFG > 8
    Init_SysClk_Gen();
#endif
}
/*
*********************************************************************************************************
*	�� �� ��: ReadResetState
*	����˵��: ��ȡ��λ״̬
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
uint32_t ReadResetState(void)
{
    uint32_t ResetFlag = RCC->RSTFLAG; /* ��ȡ��λԭ�� */
    RCC->RSTFLAG = 0xffff;             /* �帴λԭ�� */
    return ResetFlag;
}
/*
*********************************************************************************************************
*	�� �� ��: GotoBoot
*	����˵��: ���յ���λ����������֮����ת��boot
*	��    ��: *buf�����ݻ���,len�����ݳ��ȣ�ʵ��Ϊ����13
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void GotoBoot(uint8_t *buf, uint8_t len)
{
    if (len != 13)
        return;
    if (camper(buf, (uint8_t *)&UpgradeCode, 13))
    {
        //debug("��ת����");
        RCC->SOFTRST = 0x5c5caabb;
    }
}

#endif

#endif
