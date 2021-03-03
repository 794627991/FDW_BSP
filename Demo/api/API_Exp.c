#include "API_Exp.h"

__weak void User_IO_Init(void)
{
}

/*
*********************************************************************************************************
*	函 数 名: Sysinit_Exp
*	功能说明: 单片机上电初始化的例程，必须在程序头调用,根据API_CFG.h 进行初步配置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Sysinit_Exp(void)
{
    Init_System();

    /* 以下根据实际需要进行配置 */
    IWDT_Clr();
    User_IO_Init();

#if Uart_Output > 0
#if Use_Uartx_Send == 0
    Uart_Init_Macro(UART0, OutBaud, NoUSEIRMod, DISABLE, DISABLE, OneBit);
#endif
#if Use_Uartx_Send == 1
    Uart_Init_Macro(UART1, OutBaud, NoUSEIRMod, DISABLE, DISABLE, OneBit);
#endif
#if Use_Uartx_Send == 2
    Uart_Init_Macro(UART2, OutBaud, NoUSEIRMod, DISABLE, DISABLE, OneBit);
#endif
#if Use_Uartx_Send == 3
    Uart_Init_Macro(UART3, OutBaud, NoUSEIRMod, DISABLE, DISABLE, OneBit);
#endif
#if Use_Uartx_Send == 4
    Uart_Init_Macro(UART4, OutBaud, NoUSEIRMod, DISABLE, DISABLE, OneBit);
#endif
#if Use_Uartx_Send == 5
    Uart_Init_Macro(UART5, OutBaud, NoUSEIRMod, DISABLE, DISABLE, OneBit);
#endif
#endif

#ifdef LPPRE
#if API_UseLPTIM > 0
    LPTIM_Init_Macro(LPPRE, LPCount);
#endif
#endif

#if API_UseLCD > 0
    LCD_Init_Exp();
#endif

#if API_UseRTC > 0
    RTC_Init_Exp();
#endif

#ifdef LPUBaud
#if API_UseLPUart > 0
    LPUart_Init_Macro(LPUBaud);
#endif
#endif

#if UseUart0 > 0
#if U1USEIRMod == 0
#ifndef U0Stopbit
    Uart_Init_Macro(UART0, U0Baud, 0, U0UseRxIe, U0UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART0, U0Baud, 0, U0UseRxIe, U0UseTxIe, U0Stopbit);
#endif
#else
#ifndef U0Stopbit
    Uart_Init_Macro(UART0, 2400, 1, U0UseRxIe, U0UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART0, 2400, 1, U0UseRxIe, U0UseTxIe, U0Stopbit);
#endif
#endif
#endif

#if UseUart1 > 0
#if U1USEIRMod == 0
#ifndef U1Stopbit
    Uart_Init_Macro(UART1, U1Baud, 0, U1UseRxIe, U1UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART1, U1Baud, 0, U1UseRxIe, U1UseTxIe, U1Stopbit);
#endif
#else
#ifndef U1Stopbit
    Uart_Init_Macro(UART1, 2400, 1, U1UseRxIe, U1UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART1, 2400, 1, U1UseRxIe, U1UseTxIe, U1Stopbit);
#endif
#endif
#endif

#if UseUart2 > 0
#if U2USEIRMod == 0
#ifndef U2Stopbit
    Uart_Init_Macro(UART2, U2Baud, 0, U2UseRxIe, U2UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART2, U2Baud, 0, U2UseRxIe, U2UseTxIe, U2Stopbit);
#endif
#else
#ifndef U2Stopbit
    Uart_Init_Macro(UART2, 2400, 1, U2UseRxIe, U2UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART2, 2400, 1, U2UseRxIe, U2UseTxIe, U2Stopbit);
#endif
#endif
#endif

#if UseUart3 > 0
#if U3USEIRMod == 0
#ifndef U3Stopbit
    Uart_Init_Macro(UART3, U3Baud, 0, U3UseRxIe, U3UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART3, U3Baud, 0, U3UseRxIe, U3UseTxIe, U3Stopbit);
#endif
#else
#ifndef U3Stopbit
    Uart_Init_Macro(UART3, 2400, 1, U3UseRxIe, U3UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART3, 2400, 1, U3UseRxIe, U3UseTxIe, U3Stopbit);
#endif
#endif
#endif

#if UseUart4 > 0
#if U4USEIRMod == 0
#ifndef U4Stopbit
    Uart_Init_Macro(UART4, U4Baud, 0, U4UseRxIe, U4UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART4, U4Baud, 0, U4UseRxIe, U4UseTxIe, U4Stopbit);
#endif
#else
#ifndef U4Stopbit
    Uart_Init_Macro(UART4, 2400, 1, U4UseRxIe, U4UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART4, 2400, 1, U4UseRxIe, U4UseTxIe, U4Stopbit);
#endif
#endif
#endif

#if UseUart5 > 0
#if U5USEIRMod == 0
#ifndef U5Stopbit
    Uart_Init_Macro(UART5, U5Baud, 0, U5UseRxIe, U5UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART5, U5Baud, 0, U5UseRxIe, U5UseTxIe, U5Stopbit);
#endif
#else
#ifndef U5Stopbit
    Uart_Init_Macro(UART5, 2400, 1, U5UseRxIe, U5UseTxIe, OneBit);
#else
    Uart_Init_Macro(UART5, 2400, 1, U5UseRxIe, U5UseTxIe, U5Stopbit);
#endif
#endif
#endif

#if API_UseIIC > 0
    API_SaveAdr_Init();
#endif

    mem_init();
    __enable_irq(); //打开全局中断使能
}

#if API_UseADC > 0
/*
*********************************************************************************************************
*	函 数 名: ADC_Exp
*	功能说明: ADC使用例程，若采集成功返回4位电压值,3位小数，若采集失败,固定返回0xffffffff
*            采集完成后回调函数会自动关闭ADC中断,并在完成后关闭管脚
*	形    参: CH_INx 需要使用的AD通道号
*	返 回 值: 采集到的电压值
*********************************************************************************************************
*/
uint32_t ADC_Exp(uint8_t CH_INx)
{
    ADC_init.Nvicpry = 2;             //中断优先级选择  越小优先级越高  不能大于3
    ADC_init.ChSel_def = CH_INx;      //选择使用的采集通道
    ADC_init.TrimV = SmpSpd_Fast;     //选择最快采集速度
    ADC_init.SampleNeed = 2;          //采集次数最小为2
    ADC_init.SampleLevel = SmpLve_3V; //采样的电压小于3V，若大于3V小于5V选择 SmpLve_5V  采样电压不能大于5V

    return API_ADC(&ADC_init); //返回4位数的电压值,3位小数
}

float Temp_Exp(void)
{
    ADC_init.Nvicpry = 2;             //中断优先级选择  越小优先级越高  不能大于3
    ADC_init.ChSel_def = CH_PTAT;     //选择使用的采集通道(温采)
    ADC_init.TrimV = SmpSpd_Fast;     //选择最快采集速度
    ADC_init.SampleNeed = 4;          //采集次数最小为2
    ADC_init.SampleLevel = SmpLve_3V; //采样的电压小于3V，若大于3V小于5V选择 SmpLve_5V  采样电压不能大于5V

    return API_Temperature(&ADC_init); //返回4位数的电压值,3位小数
}

#endif

#if API_UseETIM > 0
/*
*********************************************************************************************************
*	函 数 名: ETIM_INIT_Exp
*	功能说明: ETIM使用例程，例程默认配置开中断方式，如不需要，请修改API中溢出中断使能
*            设置完ETIM后不会立即启动定时器,需要调用API_ETim_Start开启
*	形    参: ETIMx_Type 型结构体变量
*             pre：分频系数
*             count：计数值 最大0xffff
*	返 回 值: 无
*********************************************************************************************************
*/
void ETIM_INIT_Exp(ETIMx_Type *ETIMx, uint8_t pre, uint16_t count)
{
    ETIM_INIT_Type ETIM_init;

    ETIM_init.Nvicpry = 2;   //中断优先级选择  越小优先级越高  不能大于3
    ETIM_init.PRE = pre;     //1-255;   以主频分频  1为1分频即不分频
    ETIM_init.count = count; //计数值   以主频8M,100分频,800/80000=10ms

    API_Etim_Init(ETIMx, &ETIM_init);

    //	API_ETim_Start(ETIMx);
}

#endif

#if API_UseLPTIM > 0
/*----------------------------------------------------------------------
//LPTIM使用例程
//固化的LPTIM为计数溢出中断,因此若使用必须使用中断
//设置完LPTIM后不会立即启动定时器,需要调用API_LPTIM_Start开启
//LPTIM_init.PRE的说明
//0:计数时钟1分频  1:计数时钟2分频  2:计数时钟4分频
//3:计数时钟8分频  4:计数时钟16分频  5:计数时钟32分频
//6:计数时钟64分频  7及7以上:计数时钟128分频
-----------------------------------------------------------------------*/
void LPTIM_Init_Exp(void)
{
    LPTIM_INIT_Type LPTIM_init;

    LPTIM_init.Nvicpry = 2; //中断优先级选择  越小优先级越高  不能大于3
    LPTIM_init.PRE = 7;     //0-7
    LPTIM_init.count = 256; //128*256=32768--->1s 256

    API_LPTIM_Init(&LPTIM_init);

    //API_LPTIM_Start();
}
/*
*********************************************************************************************************
*	函 数 名: LPTIM_Init_Macro
*	功能说明: LPTIM使用例程，例程默认配置开中断方式，如不需要，请修改API中溢出中断使能
*            设置完后不会立即启动定时器,需要调用API_LPTIM_Start开启
*	形    参: pre：分频系数  0-7  分频倍数为 2^(pre+1)
*             count：计数值 最大0xffffffff
*	返 回 值: 无
*********************************************************************************************************
*/
void LPTIM_Init_Macro(uint8_t pre, uint32_t cnt)
{
    LPTIM_INIT_Type LPTIM_init;

    LPTIM_init.Nvicpry = 2; //中断优先级选择  越小优先级越高  不能大于3
    LPTIM_init.PRE = pre;   //0-7
    LPTIM_init.count = cnt; //128*256=32768--->1s 256

    API_LPTIM_Init(&LPTIM_init);

    //API_LPTIM_Start();
}
#endif

#if API_UseRTC > 0
/*
*********************************************************************************************************
*	函 数 名: RTC_Init_Exp
*	功能说明: RTC使用例程，例程默认配置开中断方式，如不需要，请修改API中中断使能
*            设置的RTC中断全开
*            API_GetTime(RealTime) 能够获取7B的BCD格式和7B的DEC格式的时间
*            API_SetTIME() 必须使用BCD格式进行设置，自带日期合法判断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RTC_Init_Exp(void)
{
    RTC_INIT_Type RTC_init;
    //	RTC_TimeDateTypeDef TempTime;

    RTC_init.Nvicpry = 0; //中断优先级选择  越小优先级越高  不能大于3
    RTC_init.day = 1;     //开启RTC天中断
    RTC_init.hour = 1;    //开启RTC时中断
    RTC_init.min = 1;     //开启RTC分中断
    RTC_init.sec = 1;     //开启RTC秒中断

    API_RTC_Init(&RTC_init); //初始化

    // API_GetTime(RealTime); //获取RTC时间

    // TempTime.Year = 0x20;
    // TempTime.Month = 0x6;
    // TempTime.Date = 0x18;
    // TempTime.Hour = 0x15;
    // TempTime.Minute = 0x56;
    // TempTime.Second = 0x23;
    // API_SetTIME(&TempTime); //必须使用BCD格式
}
#endif

#if API_UseIIC > 0 /* 硬件IIC例程，使用结构体进行存读，仅供参考 */
uint32_t epromstart;

#define EPROM_POWER_ON            /* 硬件iic上电，需要配置 */
#define EPROM_POWER_OFF           /* 硬件iic下电，需要配置 */
#define EPROM_I2cStart epromstart /* 结构体名，或者结构体的第一个成员 */

__weak void API_SaveAdr_Init(void)
{
    I2CBaseAdr = 0;
    I2CStartAdr = (uint32_t)(&EPROM_I2cStart);
}

__weak void __API_EPROM_Save(uint32_t base, uint8_t *Buf, uint16_t len)
{
    EPROM_POWER_ON;
    API_I2CSave(base, Buf, len);
    EPROM_POWER_OFF;
}

__weak void __API_EPROM_Read(uint32_t base, uint8_t *Buf, uint16_t len)
{
    EPROM_POWER_ON;
    API_I2CRead(base, Buf, len);
    EPROM_POWER_OFF;
}

#endif

#if UseUart0 > 0 || UseUart1 > 0 || UseUart2 > 0 || UseUart3 > 0 || UseUart4 > 0 || UseUart5 > 0 || Uart_Output > 0
/*
*********************************************************************************************************
*	函 数 名: Uart_Init_Macro
*	功能说明: UART使用例程
*	形    参: UARTx_Type 型结构体变量
*             baudrate ：需要的波特率
*             RxIeEn ：接收中断是否使能
*             TxIeEn ： 发送中断是否使能
*	返 回 值: 无
*********************************************************************************************************
*/
void Uart_Init_Macro(UARTx_Type *Uartx, uint32_t baudrate, uint8_t useirm, bool RxIeEn, bool TxIeEn, bool Stopbit)
{
    Uart_INIT_Type Uart_init;

    Uart_init.Nvicpry = 1; //中断优先级选择  越小优先级越高  不能大于3
#ifdef U1UsePE3PE4
    Uart_init.SEL = Uart1_UsePE3PE4; //只有使用UART1时候才需要选择
#else
    Uart_init.SEL = Uart1_UsePB0PB1; //只有使用UART1时候才需要选择
#endif
    Uart_init.IRMod = useirm;              //不使用红外38K调制
    Uart_init.RxIEEN = RxIeEn;             //开启接收中断
    Uart_init.TxIEEN = TxIeEn;             //开启发送中断
    Uart_init.uartint.BaudRate = baudrate; //设置波特率
    Uart_init.uartint.DataBit = Eight8Bit; //8位数据位
    Uart_init.uartint.ParityBit = NONE;    //不开校验
    Uart_init.uartint.StopBit = Stopbit;   //停止位

    API_Uartx_Init(Uartx, &Uart_init); //设置Uart
}

#endif

#if API_UseLPUart > 0
/*----------------------------------------------------------------------
//LPUart使用例程
//若不使用中断发,提供了查询发送的Api如下
//void API_LPUart_Send(uint8_t *str,uint16_t len)
-----------------------------------------------------------------------*/
void LPUart_Init_Exp(void)
{
    LPUart_INIT_Type Uart_init;

    Uart_init.Nvicpry = 1;                 //中断优先级选择  越小优先级越高  不能大于3
    Uart_init.RxIEEN = ENABLE;             //开启接收中断
    Uart_init.uartint.BaudRate = 9600;     //设置波特率
    Uart_init.uartint.DataBit = Eight8Bit; //8位数据位
    Uart_init.uartint.ParityBit = NONE;    //不开校验
    Uart_init.uartint.StopBit = TwoBit;    //两位停止位

    API_LPUart_Init(&Uart_init); //设置Uart
}
/*
*********************************************************************************************************
*	函 数 名: LPUart_Init_Macro
*	功能说明: LPUART使用例程
*	形    参: baudrate ：需要的波特率 不要超过9600
*	返 回 值: 无
*********************************************************************************************************
*/
void LPUart_Init_Macro(uint32_t baudrate)
{
    LPUart_INIT_Type Uart_init;

    Uart_init.Nvicpry = 1;                 //中断优先级选择  越小优先级越高  不能大于3
    Uart_init.RxIEEN = ENABLE;             //开启接收中断
    Uart_init.uartint.BaudRate = baudrate; //设置波特率
    Uart_init.uartint.DataBit = Eight8Bit; //8位数据位
    Uart_init.uartint.ParityBit = NONE;    //不开校验
    Uart_init.uartint.StopBit = TwoBit;    //两位停止位

    API_LPUart_Init(&Uart_init); //设置Uart
}
#endif

#if API_UseAES > 0
/*
*********************************************************************************************************
*	函 数 名: AES_Exp
*	功能说明: AES使用例程
*	形    参: in ：输入要加密的数据，必须是16字节的数组
*             key ：输入密钥，必须是16字节的数组
*             out ： 输出加密完成的数据，，必须是16字节的数组
*	返 回 值: 无
*********************************************************************************************************
*/
void AES_Exp(uint8_t *in, uint8_t *key, uint8_t *out)
{
    AES_Init_Type init;
    //	uint8_t in[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    //	uint8_t key[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    //	uint8_t out[16];

    init.in = in;
    init.key = key;
    init.out = out;

    init.control = AESENCRYPT; //加密
    init.mode = AESECBMODE;    //ECB
    init.len = AES_128Bit;     //16字节长度

    API_AES(&init);
}

#endif

#if API_UseLCD > 0
/*----------------------------------------------------------------------
//LCD使用例程
//注意管脚的初始化 在LCD_IO_Init()中初始化
//固化的API默认为4COM，1/3偏振
-----------------------------------------------------------------------*/
void LCD_Init_Exp(void)
{
    LCD_Init_Type lcd_init;

    lcd_init.control = LcdOpen;
    lcd_init.Brightness = 15;      //灰度不能大于15  越大越亮，但是功耗越高
    lcd_init.Refreshfrequency = 3; //酌情修改刷新频率降低功耗
    lcd_init.Com32_43Set = 0;      //若不使用Com32以上的液晶管脚，一定设置成0
    lcd_init.Com0_31Set = 0;
    /*下面设置只考虑了0-31com的情况  如果使用了超过com31的需要额外另写*/
#ifdef G_LCD_SEG0
    lcd_init.Com0_31Set |= G_LCD_SEG0;
#endif
#ifdef G_LCD_SEG1
    lcd_init.Com0_31Set |= G_LCD_SEG1;
#endif
#ifdef G_LCD_SEG2
    lcd_init.Com0_31Set |= G_LCD_SEG2;
#endif
#ifdef G_LCD_SEG3
    lcd_init.Com0_31Set |= G_LCD_SEG3;
#endif
#ifdef G_LCD_SEG4
    lcd_init.Com0_31Set |= G_LCD_SEG4;
#endif
#ifdef G_LCD_SEG5
    lcd_init.Com0_31Set |= G_LCD_SEG5;
#endif
#ifdef G_LCD_SEG6
    lcd_init.Com0_31Set |= G_LCD_SEG6;
#endif
#ifdef G_LCD_SEG7
    lcd_init.Com0_31Set |= G_LCD_SEG7;
#endif
#ifdef G_LCD_SEG8
    lcd_init.Com0_31Set |= G_LCD_SEG8;
#endif
#ifdef G_LCD_SEG9
    lcd_init.Com0_31Set |= G_LCD_SEG9;
#endif
#ifdef G_LCD_SEG10
    lcd_init.Com0_31Set |= G_LCD_SEG10;
#endif
#ifdef G_LCD_SEG11
    lcd_init.Com0_31Set |= G_LCD_SEG11;
#endif
#ifdef G_LCD_SEG12
    lcd_init.Com0_31Set |= G_LCD_SEG12;
#endif
#ifdef G_LCD_SEG13
    lcd_init.Com0_31Set |= G_LCD_SEG13;
#endif
#ifdef G_LCD_SEG14
    lcd_init.Com0_31Set |= G_LCD_SEG14;
#endif
#ifdef G_LCD_SEG15
    lcd_init.Com0_31Set |= G_LCD_SEG15;
#endif
#ifdef G_LCD_SEG16
    lcd_init.Com0_31Set |= G_LCD_SEG16;
#endif
#ifdef G_LCD_SEG17
    lcd_init.Com0_31Set |= G_LCD_SEG17;
#endif
#ifdef G_LCD_SEG18
    lcd_init.Com0_31Set |= G_LCD_SEG18;
#endif
#ifdef G_LCD_SEG19
    lcd_init.Com0_31Set |= G_LCD_SEG19;
#endif
#ifdef G_LCD_SEG20
    lcd_init.Com0_31Set |= G_LCD_SEG20;
#endif
#ifdef G_LCD_SEG21
    lcd_init.Com0_31Set |= G_LCD_SEG21;
#endif
#ifdef G_LCD_SEG22
    lcd_init.Com0_31Set |= G_LCD_SEG22;
#endif
#ifdef G_LCD_SEG23
    lcd_init.Com0_31Set |= G_LCD_SEG23;
#endif
#ifdef G_LCD_SEG24
    lcd_init.Com0_31Set |= G_LCD_SEG24;
#endif
#ifdef G_LCD_SEG25
    lcd_init.Com0_31Set |= G_LCD_SEG25;
#endif
#ifdef G_LCD_SEG26
    lcd_init.Com0_31Set |= G_LCD_SEG26;
#endif
#ifdef G_LCD_SEG27
    lcd_init.Com0_31Set |= G_LCD_SEG27;
#endif
#ifdef G_LCD_SEG28
    lcd_init.Com0_31Set |= G_LCD_SEG28;
#endif
#ifdef G_LCD_SEG29
    lcd_init.Com0_31Set |= G_LCD_SEG29;
#endif
#ifdef G_LCD_SEG30
    lcd_init.Com0_31Set |= G_LCD_SEG30;
#endif
#ifdef G_LCD_SEG31
    lcd_init.Com0_31Set |= G_LCD_SEG31;
#endif

    LCD_IO_Init(lcd_init.Com0_31Set, NULL);
    // lcd_init.Com0_31Set =
    // 	(LCD_SEG_EN0_SEGEN0_Msk |
    // 	 LCD_SEG_EN0_SEGEN1_Msk |
    // 	 LCD_SEG_EN0_SEGEN2_Msk |
    // 	 LCD_SEG_EN0_SEGEN3_Msk |
    // 	 LCD_SEG_EN0_SEGEN4_Msk |
    // 	 LCD_SEG_EN0_SEGEN5_Msk |
    // 	 LCD_SEG_EN0_SEGEN6_Msk |
    // 	 LCD_SEG_EN0_SEGEN7_Msk |
    // 	 LCD_SEG_EN0_SEGEN8_Msk |
    // 	 LCD_SEG_EN0_SEGEN9_Msk |
    // 	 LCD_SEG_EN0_SEGEN10_Msk |
    // 	 LCD_SEG_EN0_SEGEN11_Msk |
    // 	 LCD_SEG_EN0_SEGEN12_Msk |
    // 	 LCD_SEG_EN0_SEGEN13_Msk |
    // 	 LCD_SEG_EN0_SEGEN14_Msk |
    // 	 LCD_SEG_EN0_SEGEN15_Msk |
    // 	 LCD_SEG_EN0_SEGEN16_Msk |
    // 	 LCD_SEG_EN0_SEGEN26_Msk |
    // 	 LCD_SEG_EN0_SEGEN27_Msk |
    // 	 LCD_SEG_EN0_SEGEN28_Msk |
    // 	 LCD_SEG_EN0_SEGEN29_Msk |
    // 	 LCD_SEG_EN0_SEGEN30_Msk);
    API_Init_LCD(&lcd_init);
}

#endif
