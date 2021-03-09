/*
*********************************************************************************************************
*
*	模块名称 : 段码led程序
*	文件名称 : bsp_led.c
*	版    本 : V1.0.0
*	说    明 : 主要适配小于32脚的4com液晶
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹    正式发布
*		V1.0.1  2021-3-3    高屹    新增宏定义沾粘方式
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "bsp_led.h"

const char disptable[] = {
    LCD_a + LCD_b + LCD_c + LCD_d + LCD_e + LCD_f,         //0
    LCD_b + LCD_c,                                         //1
    LCD_a + LCD_b + LCD_d + LCD_e + LCD_g,                 //2
    LCD_a + LCD_b + LCD_c + LCD_d + LCD_g,                 //3
    LCD_b + LCD_c + LCD_f + LCD_g,                         //4
    LCD_a + LCD_c + LCD_d + LCD_f + LCD_g,                 //5
    LCD_a + LCD_c + LCD_d + LCD_e + LCD_f + LCD_g,         //6
    LCD_a + LCD_b + LCD_c,                                 //7
    LCD_a + LCD_b + LCD_c + LCD_d + LCD_e + LCD_f + LCD_g, //8
    LCD_a + LCD_b + LCD_c + LCD_d + LCD_f + LCD_g,         //9
    LCD_a + LCD_b + LCD_c + LCD_e + LCD_f + LCD_g,         //A
    LCD_c + LCD_d + LCD_e + LCD_f + LCD_g,                 //b
    LCD_d + LCD_e + LCD_g,                                 //c
    LCD_b + LCD_c + LCD_d + LCD_e + LCD_g,                 //d
    LCD_a + LCD_d + LCD_e + LCD_f + LCD_g,                 //E
    LCD_a + LCD_e + LCD_f + LCD_g,                         //F
    LCD_g,                                                 //-
};
const char diatable1[] = {
    LCD_a + LCD_b + LCD_c + LCD_e + LCD_f + LCD_g, //0---A
    LCD_c + LCD_d + LCD_e + LCD_f + LCD_g,         //1---b
    LCD_d + LCD_e + LCD_g,                         //2---c
    LCD_a + LCD_d + LCD_e + LCD_f,                 //3---C
    LCD_b + LCD_c + LCD_d + LCD_e + LCD_g,         //4---d
    LCD_a + LCD_d + LCD_e + LCD_f + LCD_g,         //5---E
    LCD_a + LCD_e + LCD_f + LCD_g,                 //6---F
    LCD_a + LCD_c + LCD_d + LCD_e + LCD_f,         //7---G
    LCD_c + LCD_e + LCD_f + LCD_g,                 //8---h
    LCD_b + LCD_c + LCD_e + LCD_f + LCD_g,         //9---H
    LCD_e + LCD_f,                                 //10---I
    LCD_a + LCD_b + LCD_c + LCD_d + LCD_e,         //11---J
    LCD_d + LCD_e + LCD_f,                         //12---L
    LCD_c + LCD_e + LCD_g,                         //13---n
    LCD_c + LCD_d + LCD_e + LCD_g,                 //14---o
    LCD_a + LCD_b + LCD_e + LCD_f + LCD_g,         //15---P
    LCD_e + LCD_g,                                 //16---r
    LCD_a + LCD_c + LCD_d + LCD_f + LCD_g,         //17---S
    LCD_b + LCD_c + LCD_d + LCD_e + LCD_f,         //18---U
    LCD_c + LCD_d + LCD_e,                         //19---u
    LCD_b + LCD_c + LCD_d + LCD_f + LCD_g,         //20---y
    LCD_g,                                         //21---"-"
    LCD_d,                                         //22---"_"
    LCD_a + LCD_b + LCD_c + LCD_f + LCD_g,         //23---q
    LCD_d + LCD_e + LCD_f + LCD_g,                 //24---t
    LCD_b + LCD_c + LCD_g,                         //25---"+"
};

#if API_UseLCD > 0
uint32 DispBuf[6];
/*
*********************************************************************************************************
*	函 数 名: GY_LCD_COMSEGEN_Init
*	功能说明: 使能液晶COM,SEG驱动波形输出
*	形    参: LCD_Init_Type *init：LCD_Init_Type型结构体指针
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void GY_LCD_COMSEGEN_Init(LCD_Init_Type *init)
{
    uint32 Temp32;
    RCC_PERCLK_SetableEx(LCDCLK, ENABLE); /* LCD时钟使能 */

    /* LCD COM0~3输出使能控制 */
    Temp32 = (LCD_COM_EN_COMEN0_Msk |
              LCD_COM_EN_COMEN1_Msk |
              LCD_COM_EN_COMEN2_Msk |
              LCD_COM_EN_COMEN3_Msk);
    LCD_COM_EN_Write(Temp32);

    /* LCD SEG0~31输出使能控制 */
    LCD_SEG_EN0_Write(init->Com0_31Set);
    /* LCD SEG32~43输出使能控制 */
    LCD_SEG_EN1_Write(init->Com32_43Set);
}
/*
*********************************************************************************************************
*	函 数 名: LCD_DF_Calc
*	功能说明: 显示频率寄存器计算
*	形    参: LMUX：COM数量:LCD_LCDSET_LMUX_4COM,LCD_LCDSET_LMUX_6COM,LCD_LCDSET_LMUX_8COM
*             WFT：驱动波形：LCD_LCDSET_WFT_ATYPE,LCD_LCDSET_WFT_BTYPE
*             FreqWanted：刷新频率：1~100
*	返 回 值: 显示频率寄存器配置值
*	备    注：无
*********************************************************************************************************
*/
uint32_t LCD_DF_Calc(uint32_t LMUX, uint32_t WFT, uint32_t FreqWanted)
{
    uint8_t DivWFT;
    uint32_t DFResult;

    if (LCD_LCDSET_WFT_ATYPE == WFT)
        DivWFT = 2;
    else
        DivWFT = 4;

    if ((FreqWanted > 0) && (FreqWanted <= 100))
    {
        DFResult = (uint32_t)(32768.0 / (float)((LMUX * 2 + 4) * FreqWanted * DivWFT) + 0.5);
    }
    else
    {
        DFResult = 32;
    }

    return DFResult;
}
/*
*********************************************************************************************************
*	函 数 名: LCD_DF_Calc
*	功能说明: Lcd模块初始化
*	形    参: LCD_Init_Type *init：LCD_Init_Type型结构体指针
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void API_Init_LCD(LCD_Init_Type *init)
{
    LCD_InitTypeDef LCD_InitStruct;

    RCC_PERCLK_SetableEx(LCDCLK, ENABLE); /* LCD时钟使能 */

    LCD_InitStruct.LMUX = LCD_LCDSET_LMUX_4COM;              /* !<COM数量选择 */
    LCD_InitStruct.ENMODE = LCD_LCDDRV_ENMODE_INNERRESISTER; /* !<驱动模式选择 */
    LCD_InitStruct.WFT = LCD_LCDSET_WFT_ATYPE;               /* !<驱动波形选择 */
    if (init->Refreshfrequency > 100)
        init->Refreshfrequency = 64;
    LCD_InitStruct.DF = LCD_DF_Calc(LCD_InitStruct.LMUX, LCD_InitStruct.WFT, init->Refreshfrequency); /* !<显示频率控制寄存器 */
    LCD_InitStruct.BIASMD = LCD_LCDSET_BIASMD_3BIAS;                                                  /* !<偏置类型控制 */
    LCD_InitStruct.SCFSEL = LCD_LCDDRV_SCFSEL_X1;                                                     /* !<SC频率选择，仅在片外电容模式下有效*/
    LCD_InitStruct.SC_CTRL = LCD_LCDDRV_SC_CTRL_ONE;                                                  /* !<片外电容驱动模式下，驱动方式控制 */
    LCD_InitStruct.IC_CTRL = LCD_LCDDRV_IC_CTRL_L0;                                                   /* !<片内电阻模式下，驱动电流大小 */
    if (init->Brightness > 15)
        init->Brightness = 15;
    LCD_InitStruct.LCDBIAS = init->Brightness; /* !<显示灰度设置(设置显示深度) */
    LCD_InitStruct.ANTIPOLAR = ENABLE;         /* !<防极化使能控制 */

    LCD_InitStruct.TEST = DISABLE;   /* !<测试模式使能控制，仅在DISPMD=1的情况下有效 */
    LCD_InitStruct.DISPMD = DISABLE; /* !<测试模式使能控制 */

    LCD_InitStruct.LCCTRL = LCD_LCDTEST_LCCTRL_0; /* !<保持为0 */
    LCD_InitStruct.TESTEN = DISABLE;              /* !<保持关闭 */

    LCD_InitStruct.FLICK = DISABLE;  /* !<显示闪烁使能控制 */
    LCD_InitStruct.TON = 0;          /* !<闪烁显示时的点亮时间＝TON×0.25秒 */
    LCD_InitStruct.TOFF = 0;         /* !<闪烁显示时的熄灭时间＝TOFF×0.25秒 */
    LCD_InitStruct.DONIE = DISABLE;  /* !<显示点亮中断使能控制 */
    LCD_InitStruct.DOFFIE = DISABLE; /* !<显示熄灭中断使能控制 */

    LCD_InitStruct.LCDEN = DISABLE; /* !<LCD显示使能控制 */

    LCD_Init(&LCD_InitStruct);

    GY_LCD_COMSEGEN_Init(init);
    if (init->control != 0)
        LCD_DISPCTRL_LCDEN_Setable(ENABLE); /* 打开显示 */
    else
        LCD_DISPCTRL_LCDEN_Setable(DISABLE); /* 关闭显示 */
}
/*
*********************************************************************************************************
*	函 数 名: LCD_IO_Init
*	功能说明: Lcd的IO口初始化
*	形    参: lcd_io_0_31：0-31脚用那个置位哪个
*             lcd_io_32_43 hook：函数指针，32-43需要自己配置
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void LCD_IO_Init(uint32_t lcd_io_0_31, lcd_io_32_43 hook)
{
    //	/* PC15模拟功能选择 */
    //	GPIO_ANASEL_PC15ANS_Set(GPIO_ANASEL_PC15ANS_SEG39);

    //	/* PC14模拟功能选择 */
    //	GPIO_ANASEL_PC14ANS_Set(GPIO_ANASEL_PC14ANS_SEG38);

    //	/* PC13模拟功能选择 */
    //	GPIO_ANASEL_PC13ANS_Set(GPIO_ANASEL_PC13ANS_SEG37);

    //	/* PC12模拟功能选择 */
    //	GPIO_ANASEL_PC12ANS_Set(GPIO_ANASEL_PC12ANS_SEG36);

    AnalogIO(GPIOA, GPIO_Pin_0); /* PA0 COM0 */
    AnalogIO(GPIOA, GPIO_Pin_1); /* PA1 COM1 */
    AnalogIO(GPIOA, GPIO_Pin_2); /* PA2 COM2 */
    AnalogIO(GPIOA, GPIO_Pin_3); /* PA3 COM3 */

    if ((lcd_io_0_31 & SEG0) == SEG0)
    {
        AnalogIO(GPIOA, GPIO_Pin_8); /* PA8 SEG0 */
    }
    if ((lcd_io_0_31 & SEG1) == SEG1)
    {
        AnalogIO(GPIOA, GPIO_Pin_9); /* PA9 SEG1 */
    }
    if ((lcd_io_0_31 & SEG2) == SEG2)
    {
        AnalogIO(GPIOA, GPIO_Pin_10); /* PA10 SEG2 */
    }
    if ((lcd_io_0_31 & SEG3) == SEG3)
    {
        AnalogIO(GPIOA, GPIO_Pin_11); /* PA11 SEG3 */
    }
    if ((lcd_io_0_31 & SEG4) == SEG4)
    {
        AnalogIO(GPIOA, GPIO_Pin_12); /* PA12 SEG4 */
    }
    if ((lcd_io_0_31 & SEG5) == SEG5)
    {
        AnalogIO(GPIOA, GPIO_Pin_13); /* PA13 SEG5 */
    }
    if ((lcd_io_0_31 & SEG6) == SEG6)
    {
        AnalogIO(GPIOB, GPIO_Pin_4); /* PB4 SEG6 */
    }
    if ((lcd_io_0_31 & SEG7) == SEG7)
    {
        AnalogIO(GPIOB, GPIO_Pin_5); /* PB5 SEG7 */
    }
    if ((lcd_io_0_31 & SEG8) == SEG8)
    {
        AnalogIO(GPIOB, GPIO_Pin_6); /* PB6 SEG8 */
    }
    if ((lcd_io_0_31 & SEG9) == SEG9)
    {
        AnalogIO(GPIOB, GPIO_Pin_7); /* PB7 SEG9 */
    }
    if ((lcd_io_0_31 & SEG10) == SEG10)
    {
        AnalogIO(GPIOB, GPIO_Pin_8); /* PB8 SEG10 */
    }
    if ((lcd_io_0_31 & SEG11) == SEG11)
    {
        AnalogIO(GPIOB, GPIO_Pin_9); /* PB9 SEG11 */
    }
    if ((lcd_io_0_31 & SEG12) == SEG12)
    {
        AnalogIO(GPIOB, GPIO_Pin_10); /* PB10 SEG12 */
    }
    if ((lcd_io_0_31 & SEG13) == SEG13)
    {
        AnalogIO(GPIOB, GPIO_Pin_11); /* PB11 SEG13 */
    }
    if ((lcd_io_0_31 & SEG14) == SEG14)
    {
        AnalogIO(GPIOB, GPIO_Pin_12); /* PB12 SEG14 */
    }
    if ((lcd_io_0_31 & SEG15) == SEG15)
    {
        AnalogIO(GPIOB, GPIO_Pin_13); /* PB13 SEG15 */
    }
    if ((lcd_io_0_31 & SEG16) == SEG16)
    {
        AnalogIO(GPIOB, GPIO_Pin_14); /* PB14 SEG16 */
    }
    if ((lcd_io_0_31 & SEG17) == SEG17)
    {
        AnalogIO(GPIOB, GPIO_Pin_15); /* PB15 SEG17 */
    }
    if ((lcd_io_0_31 & SEG18) == SEG18)
    {
        /* PE3模拟功能选择 */
        GPIO_ANASEL_PE3ANS_Set(GPIO_ANASEL_PE3ANS_SEG18); /* PE3 SEG18 */
    }
    if ((lcd_io_0_31 & SEG19) == SEG19)
    {
        /* PE4模拟功能选择 */
        GPIO_ANASEL_PE4ANS_Set(GPIO_ANASEL_PE4ANS_SEG19); /* PE4 SEG19 */
    }
    if ((lcd_io_0_31 & SEG20) == SEG20)
    {
        AnalogIO(GPIOD, GPIO_Pin_2); /* PD2 SEG20 */
    }
    if ((lcd_io_0_31 & SEG21) == SEG21)
    {
        AnalogIO(GPIOD, GPIO_Pin_3); /* PD3 SEG21 */
    }
    if ((lcd_io_0_31 & SEG22) == SEG22)
    {
        AnalogIO(GPIOD, GPIO_Pin_4); /* PD4 SEG22 */
    }
    if ((lcd_io_0_31 & SEG23) == SEG23)
    {
        AnalogIO(GPIOD, GPIO_Pin_5); /* PD5 SEG23 */
    }
    /* SEG24  SEG25没有 */
    if ((lcd_io_0_31 & SEG26) == SEG26)
    {
        AnalogIO(GPIOC, GPIO_Pin_2); /* PC2 SEG26 */
    }
    if ((lcd_io_0_31 & SEG27) == SEG27)
    {
        AnalogIO(GPIOC, GPIO_Pin_3); /* PC3 SEG27 */
    }
    if ((lcd_io_0_31 & SEG28) == SEG28)
    {
        AnalogIO(GPIOC, GPIO_Pin_4); /* PC4 SEG28 */
    }
    if ((lcd_io_0_31 & SEG29) == SEG29)
    {
        AnalogIO(GPIOC, GPIO_Pin_5); /* PC5 SEG29 */
    }
    if ((lcd_io_0_31 & SEG30) == SEG30)
    {
        AnalogIO(GPIOC, GPIO_Pin_6); /* PC6 SEG30 */
    }
    if ((lcd_io_0_31 & SEG31) == SEG31)
    {
        AnalogIO(GPIOC, GPIO_Pin_7); /* PC7 SEG31 */
    }
    if (hook != NULL)
    {
        hook();
    }
}
/*
*********************************************************************************************************
*	函 数 名: Lcd_clear
*	功能说明: 清空液晶显示
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void Lcd_clear(void)
{
    memset(DispBuf, 0, sizeof(DispBuf));
    LCD->DISPDATA0 = DispBuf[0];
    LCD->DISPDATA1 = DispBuf[1];
    LCD->DISPDATA2 = DispBuf[2];
    LCD->DISPDATA3 = DispBuf[3];
}
/*
*********************************************************************************************************
*	函 数 名: Lcd_All
*	功能说明: 液晶全显
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void Lcd_All(void)
{
    memset(DispBuf, 0xff, sizeof(DispBuf));
    LCD->DISPDATA0 = DispBuf[0];
    LCD->DISPDATA1 = DispBuf[1];
    LCD->DISPDATA2 = DispBuf[2];
    LCD->DISPDATA3 = DispBuf[3];
}
/*
*********************************************************************************************************
*	函 数 名: LCD_Display
*	功能说明: 液晶显示除dp之外的段码
*	形    参: Num：第n-1个数字
*             Show：要显示的数字
*	返 回 值: 无
*	备    注：根据实际液晶的对应引脚修改
*********************************************************************************************************
*/
void LCD_Display(uint8_t Num, uint8_t Show)
{
    Num += 1;
    switch (Num)
    {
#define LNx 1
    case LNx:
        //------------ 第LNx个"8"字符 ----------
#if LCD_X_EFGDP(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCDP(LNx);
        }
#endif
        break;
#undef LNx

#define LNx 2
    case LNx:
        //------------ 第LNx个"8"字符 ----------
#if LCD_X_EFGDP(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCDP(LNx);
        }
#endif
        break;
#undef LNx

#define LNx 3
    case LNx:
        //------------ 第LNx个"8"字符 ----------
#if LCD_X_EFGDP(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCDP(LNx);
        }
#endif
        break;
#undef LNx

#define LNx 4
    case LNx:
        //------------ 第LNx个"8"字符 ----------
#if LCD_X_EFGDP(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCDP(LNx);
        }
#endif
        break;
#undef LNx

#define LNx 5
    case LNx:
        //------------ 第LNx个"8"字符 ----------
#if LCD_X_EFGDP(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCDP(LNx);
        }
#endif
        break;
#undef LNx

#define LNx 6
    case LNx:
        //------------ 第LNx个"8"字符 ----------
#if LCD_X_EFGDP(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCDP(LNx);
        }
#endif
        break;
#undef LNx

#define LNx 7
    case LNx:
        //------------ 第LNx个"8"字符 ----------
#if LCD_X_EFGDP(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCDP(LNx);
        }
#endif
        break;
#undef LNx

#define LNx 8
    case LNx:
        //------------ 第LNx个"8"字符 ----------
#if LCD_X_EFGDP(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCDP(LNx);
        }
#endif
        break;
#undef LNx

#define LNx 9
    case LNx:
        //------------ 第LNx个"8"字符 ----------
#if LCD_X_EFGDP(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //E
        if ((Show & LCD_e) == LCD_e)
        {
            DispBuf[L_E] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //G
        if ((Show & LCD_g) == LCD_g)
        {
            DispBuf[L_G] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_EFGDP(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGDP(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //F
        if ((Show & LCD_f) == LCD_f)
        {
            DispBuf[L_F] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_EFGD(LNx) != 0 //D
        if ((Show & LCD_d) == LCD_d)
        {
            DispBuf[L_D] |= LCD_X_EFGD(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //C
        if ((Show & LCD_c) == LCD_c)
        {
            DispBuf[L_C] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //B
        if ((Show & LCD_b) == LCD_b)
        {
            DispBuf[L_B] |= LCD_X_ABCDP(LNx);
        }
#endif

#if LCD_X_ABCD(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCD(LNx);
        }
#endif
#if LCD_X_ABCDP(LNx) != 0 //A
        if ((Show & LCD_a) == LCD_a)
        {
            DispBuf[L_A] |= LCD_X_ABCDP(LNx);
        }
#endif
        break;
#undef LNx

        //     case 0x01:
        //         //------------ 第1个"8"字符 ----------
        // #ifdef LCD_1_EFGDP //1E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_1_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_1_EFGD //1E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_1_EFGD;
        //         }
        // #endif

        // #ifdef LCD_1_EFGDP //1G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_1_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_1_EFGD //1G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_1_EFGD;
        //         }
        // #endif

        // #ifdef LCD_1_EFGDP //1F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_1_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_1_EFGD //1F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_1_EFGD;
        //         }
        // #endif

        // #ifdef LCD_1_ABCD //1D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_1_ABCD;
        //         }
        // #endif
        // #ifdef LCD_1_EFGD //1D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_1_EFGD;
        //         }
        // #endif

        // #ifdef LCD_1_ABCD //1C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_1_ABCD;
        //         }
        // #endif
        // #ifdef LCD_1_ABCDP //1C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_1_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_1_ABCD //1B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_1_ABCD;
        //         }
        // #endif
        // #ifdef LCD_1_ABCDP //1B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_1_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_1_ABCD //1A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_1_ABCD;
        //         }
        // #endif
        // #ifdef LCD_1_ABCDP //1A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_1_ABCDP;
        //         }
        // #endif

        //         break;

        //     //------------ 第2个"8"字符 ----------
        //     case 0x02:

        // #ifdef LCD_2_EFGDP //2E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_2_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_2_EFGD //2E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_2_EFGD;
        //         }
        // #endif

        // #ifdef LCD_2_EFGDP //2G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_2_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_2_EFGD //2G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_2_EFGD;
        //         }
        // #endif

        // #ifdef LCD_2_EFGDP //2F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_2_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_2_EFGD //2F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_2_EFGD;
        //         }
        // #endif

        // #ifdef LCD_2_ABCD //2D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_2_ABCD;
        //         }
        // #endif
        // #ifdef LCD_2_EFGD //2D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_2_EFGD;
        //         }
        // #endif

        // #ifdef LCD_2_ABCD //2C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_2_ABCD;
        //         }
        // #endif
        // #ifdef LCD_2_ABCDP //2C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_2_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_2_ABCD //2B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_2_ABCD;
        //         }
        // #endif
        // #ifdef LCD_2_ABCDP //2B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_2_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_2_ABCD //2A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_2_ABCD;
        //         }
        // #endif
        // #ifdef LCD_2_ABCDP //2A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_2_ABCDP;
        //         }
        // #endif
        //         break;

        //     //------------ 第3个"8"字符 ----------
        //     case 0x03:

        // #ifdef LCD_3_EFGDP //3E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_3_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_3_EFGD //3E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_3_EFGD;
        //         }
        // #endif

        // #ifdef LCD_3_EFGDP //3G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_3_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_3_EFGD //3G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_3_EFGD;
        //         }
        // #endif

        // #ifdef LCD_3_EFGDP //3F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_3_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_3_EFGD //3F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_3_EFGD;
        //         }
        // #endif

        // #ifdef LCD_3_ABCD //3D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_3_ABCD;
        //         }
        // #endif
        // #ifdef LCD_3_EFGD //3D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_3_EFGD;
        //         }
        // #endif

        // #ifdef LCD_3_ABCD //3C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_3_ABCD;
        //         }
        // #endif
        // #ifdef LCD_3_ABCDP //3C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_3_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_3_ABCD //3B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_3_ABCD;
        //         }
        // #endif
        // #ifdef LCD_3_ABCDP //3B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_3_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_3_ABCD //3A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_3_ABCD;
        //         }
        // #endif
        // #ifdef LCD_3_ABCDP //3A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_3_ABCDP;
        //         }
        // #endif
        //         break;

        //     //------------ 第4个"8"字符 ----------
        //     case 0x04:
        // #ifdef LCD_4_EFGDP //4E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_4_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_4_EFGD //4E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_4_EFGD;
        //         }
        // #endif

        // #ifdef LCD_4_EFGDP //4G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_4_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_4_EFGD //4G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_4_EFGD;
        //         }
        // #endif

        // #ifdef LCD_4_EFGDP //4F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_4_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_4_EFGD //4F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_4_EFGD;
        //         }
        // #endif

        // #ifdef LCD_4_ABCD //4D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_4_ABCD;
        //         }
        // #endif
        // #ifdef LCD_4_EFGD //4D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_4_EFGD;
        //         }
        // #endif

        // #ifdef LCD_4_ABCD //4C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_4_ABCD;
        //         }
        // #endif
        // #ifdef LCD_4_ABCDP //4C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_4_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_4_ABCD //4B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_4_ABCD;
        //         }
        // #endif
        // #ifdef LCD_4_ABCDP //4B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_4_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_4_ABCD //4A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_4_ABCD;
        //         }
        // #endif
        // #ifdef LCD_4_ABCDP //4A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_4_ABCDP;
        //         }
        // #endif
        //         break;

        //     //------------ 第5个"8"字符 ----------
        //     case 0x05:
        // #ifdef LCD_5_EFGDP //5E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_5_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_5_EFGD //5E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_5_EFGD;
        //         }
        // #endif

        // #ifdef LCD_5_EFGDP //5G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_5_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_5_EFGD //5G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_5_EFGD;
        //         }
        // #endif

        // #ifdef LCD_4_EFGDP //4F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_4_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_5_EFGD //5F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_5_EFGD;
        //         }
        // #endif

        // #ifdef LCD_5_ABCD //5D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_5_ABCD;
        //         }
        // #endif
        // #ifdef LCD_5_EFGD //5D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_5_EFGD;
        //         }
        // #endif

        // #ifdef LCD_5_ABCD //5C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_5_ABCD;
        //         }
        // #endif
        // #ifdef LCD_5_ABCDP //5C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_5_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_5_ABCD //5B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_5_ABCD;
        //         }
        // #endif
        // #ifdef LCD_5_ABCDP //5B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_5_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_5_ABCD //5A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_5_ABCD;
        //         }
        // #endif
        // #ifdef LCD_5_ABCDP //5A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_5_ABCDP;
        //         }
        // #endif
        //         break;

        //     //------------ 第6个"8"字符 ----------
        //     case 0x06:
        // #ifdef LCD_6_EFGDP //6E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_6_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_6_EFGD //6E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_6_EFGD;
        //         }
        // #endif

        // #ifdef LCD_6_EFGDP //6G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_6_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_6_EFGD //6G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_6_EFGD;
        //         }
        // #endif

        // #ifdef LCD_6_EFGDP //6F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_6_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_6_EFGD //6F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_6_EFGD;
        //         }
        // #endif

        // #ifdef LCD_6_ABCD //6D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_6_ABCD;
        //         }
        // #endif
        // #ifdef LCD_6_EFGD //6D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_6_EFGD;
        //         }
        // #endif

        // #ifdef LCD_6_ABCD //6C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_6_ABCD;
        //         }
        // #endif
        // #ifdef LCD_6_ABCDP //6C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_6_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_6_ABCD //6B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_6_ABCD;
        //         }
        // #endif
        // #ifdef LCD_6_ABCDP //6B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_6_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_6_ABCD //6A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_6_ABCD;
        //         }
        // #endif
        // #ifdef LCD_6_ABCDP //6A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_6_ABCDP;
        //         }
        // #endif
        //         break;

        //     //------------ 第7个"8"字符 ----------
        //     case 0x07:
        // #ifdef LCD_7_EFGDP //7E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_7_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_7_EFGD //7E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_7_EFGD;
        //         }
        // #endif

        // #ifdef LCD_7_EFGDP //7G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_7_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_7_EFGD //7G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_7_EFGD;
        //         }
        // #endif

        // #ifdef LCD_7_EFGDP //7F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_7_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_7_EFGD //7F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_7_EFGD;
        //         }
        // #endif

        // #ifdef LCD_7_ABCD //7D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_7_ABCD;
        //         }
        // #endif
        // #ifdef LCD_7_EFGD //7D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_7_EFGD;
        //         }
        // #endif

        // #ifdef LCD_7_ABCD //7C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_7_ABCD;
        //         }
        // #endif
        // #ifdef LCD_7_ABCDP //7C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_7_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_7_ABCD //7B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_7_ABCD;
        //         }
        // #endif
        // #ifdef LCD_7_ABCDP //7B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_7_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_7_ABCD //7A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_7_ABCD;
        //         }
        // #endif
        // #ifdef LCD_7_ABCDP //7A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_7_ABCDP;
        //         }
        // #endif
        //         break;

        //     //------------ 第8个"8"字符 ----------
        //     case 0x08:
        // #ifdef LCD_8_EFGDP //8E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_8_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_8_EFGD //8E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_8_EFGD;
        //         }
        // #endif

        // #ifdef LCD_8_EFGDP //8G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_8_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_8_EFGD //8G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_8_EFGD;
        //         }
        // #endif

        // #ifdef LCD_8_EFGDP //8F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_8_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_8_EFGD //8F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_8_EFGD;
        //         }
        // #endif

        // #ifdef LCD_8_ABCD //8D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_8_ABCD;
        //         }
        // #endif
        // #ifdef LCD_8_EFGD //8D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_8_EFGD;
        //         }
        // #endif

        // #ifdef LCD_8_ABCD //8C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_8_ABCD;
        //         }
        // #endif
        // #ifdef LCD_8_ABCDP //8C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_8_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_8_ABCD //8B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_8_ABCD;
        //         }
        // #endif
        // #ifdef LCD_8_ABCDP //8B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_8_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_8_ABCD //8A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_8_ABCD;
        //         }
        // #endif
        // #ifdef LCD_8_ABCDP //8A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_8_ABCDP;
        //         }
        // #endif
        //         break;

        //     //------------ 第9个"8"字符 ----------
        //     case 0x09:
        // #ifdef LCD_9_EFGDP //9E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_9_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_9_EFGD //9E
        //         if ((Show & LCD_e) == LCD_e)
        //         {
        //             DispBuf[L_E] |= LCD_9_EFGD;
        //         }
        // #endif

        // #ifdef LCD_9_EFGDP //9G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_9_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_9_EFGD //9G
        //         if ((Show & LCD_g) == LCD_g)
        //         {
        //             DispBuf[L_G] |= LCD_9_EFGD;
        //         }
        // #endif

        // #ifdef LCD_9_EFGDP //9F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_9_EFGDP;
        //         }
        // #endif
        // #ifdef LCD_9_EFGD //9F
        //         if ((Show & LCD_f) == LCD_f)
        //         {
        //             DispBuf[L_F] |= LCD_9_EFGD;
        //         }
        // #endif

        // #ifdef LCD_9_ABCD //9D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_9_ABCD;
        //         }
        // #endif
        // #ifdef LCD_9_EFGD //9D
        //         if ((Show & LCD_d) == LCD_d)
        //         {
        //             DispBuf[L_D] |= LCD_9_EFGD;
        //         }
        // #endif

        // #ifdef LCD_9_ABCD //9C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_9_ABCD;
        //         }
        // #endif
        // #ifdef LCD_9_ABCDP //9C
        //         if ((Show & LCD_c) == LCD_c)
        //         {
        //             DispBuf[L_C] |= LCD_9_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_9_ABCD //9B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_9_ABCD;
        //         }
        // #endif
        // #ifdef LCD_9_ABCDP //9B
        //         if ((Show & LCD_b) == LCD_b)
        //         {
        //             DispBuf[L_B] |= LCD_9_ABCDP;
        //         }
        // #endif

        // #ifdef LCD_9_ABCD //9A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_9_ABCD;
        //         }
        // #endif
        // #ifdef LCD_9_ABCDP //9A
        //         if ((Show & LCD_a) == LCD_a)
        //         {
        //             DispBuf[L_A] |= LCD_9_ABCDP;
        //         }
        // #endif
        //         break;

    default:
        break;
    }
    LCD->DISPDATA0 |= DispBuf[0];
    LCD->DISPDATA1 |= DispBuf[1];
    LCD->DISPDATA2 |= DispBuf[2];
    LCD->DISPDATA3 |= DispBuf[3];
}
/*
*********************************************************************************************************
*	函 数 名: Show_Table
*	功能说明: 根据要显示的字符查表返回对应段码值
*	形    参: show：要显示的字符
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
uint8_t Show_Table(char show)
{
    uint8_t showwhat;
    switch (show)
    {
    case 0:
    case '0':
    case 'O':
    {
        showwhat = disptable[0];
    }
    break;
    case 1:
    case '1':
    {
        showwhat = disptable[1];
    }
    break;
    case 2:
    case '2':
    {
        showwhat = disptable[2];
    }
    break;
    case 3:
    case '3':
    {
        showwhat = disptable[3];
    }
    break;
    case 4:
    case '4':
    {
        showwhat = disptable[4];
    }
    break;
    case 5:
    case '5':
    {
        showwhat = disptable[5];
    }
    break;
    case 6:
    case '6':
    {
        showwhat = disptable[6];
    }
    break;
    case 7:
    case '7':
    {
        showwhat = disptable[7];
    }
    break;
    case 8:
    case '8':
    {
        showwhat = disptable[8];
    }
    break;
    case 9:
    case '9':
    {
        showwhat = disptable[9];
    }
    break;
    case 10:
    case 'a':
    case 'A':
    {
        showwhat = disptable[10];
    }
    break;
    case 11:
    case 'B':
    {
        showwhat = disptable[11];
    }
    break;
    case 12:
    {
        showwhat = disptable[12];
    }
    break;
    case 13:
    case 'd':
    case 'D':
    {
        showwhat = disptable[13];
    }
    break;
    case 14:
    case 'e':
    case 'E':
    {
        showwhat = disptable[14];
    }
    break;
    case 15:
    case 'f':
    case 'F':
    {
        showwhat = disptable[15];
    }
    break;
    case 'b':
    {
        showwhat = diatable1[1];
    }
    break;
    case 'c':
    {
        showwhat = diatable1[2];
    }
    break;
    case 'C':
    {
        showwhat = diatable1[3];
    }
    break;
    case 'g':
    case 'G':
    {
        showwhat = diatable1[7];
    }
    break;
    case 'h':
    {
        showwhat = diatable1[8];
    }
    break;
    case 'H':
    {
        showwhat = diatable1[9];
    }
    break;
    case 'i':
    case 'I':
    {
        showwhat = diatable1[10];
    }
    break;
    case 'j':
    case 'J':
    {
        showwhat = diatable1[11];
    }
    break;
    case 'l':
    case 'L':
    {
        showwhat = diatable1[12];
    }
    break;
    case 'n':
    case 'N':
    {
        showwhat = diatable1[13];
    }
    break;
    case 'o':
    {
        showwhat = diatable1[14];
    }
    break;
    case 'p':
    case 'P':
    {
        showwhat = diatable1[15];
    }
    break;
    case 'q':
    case 'Q':
    {
        showwhat = diatable1[23];
    }
    break;
    case 'r':
    case 'R':
    {
        showwhat = diatable1[16];
    }
    break;
    case 's':
    case 'S':
    {
        showwhat = diatable1[17];
    }
    break;
    case 't':
    case 'T':
    {
        showwhat = diatable1[24];
    }
    break;
    case 'U':
    {
        showwhat = diatable1[18];
    }
    break;
    case 'u':
    {
        showwhat = diatable1[19];
    }
    break;
    case 'y':
    case 'Y':
    {
        showwhat = diatable1[20];
    }
    break;
    case '-':
    {
        showwhat = diatable1[21];
    }
    break;
    case '.':
    case '_':
    {
        showwhat = diatable1[22];
    }
    break;
    case ' ':
    {
        showwhat = 0;
    }
    break;
    case '+':
    {
        showwhat = diatable1[25];
    }
    break;

    default:
    {
        showwhat = 0;
    }
    break;
    }
    return showwhat;
}
/*
*********************************************************************************************************
*	函 数 名: API_ShowEvery
*	功能说明: 根据液晶最大显示数，显示放入缓存的内容
*	形    参: *ledbuf：要显示的数据缓存
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void API_ShowEvery(char *ledbuf)
{
    uint8_t i = 0, showwhat;
    for (i = 0; i < ShowMaxLen; i++)
    {
        showwhat = Show_Table(ledbuf[i]);
        LCD_Display(i, showwhat);
    }
}
/*
*********************************************************************************************************
*	函 数 名: API_ShowAlone
*	功能说明: 在第n-1位显示某个字符
*	形    参: Num：第n-1个位置
*             show：要显示的字符
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void API_ShowAlone(uint8_t Num, uint8_t show)
{
    uint8_t showwhat = 0;
    if (Num < ShowMaxLen)
    {
        showwhat = Show_Table(show);
        LCD_Display(Num, showwhat);
    }
}

#endif
