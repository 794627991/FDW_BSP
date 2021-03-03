#include "API_Exp.h"

__weak void User_IO_Init(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: Sysinit_Exp
*	����˵��: ��Ƭ���ϵ��ʼ�������̣������ڳ���ͷ����,����API_CFG.h ���г�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Sysinit_Exp(void)
{
    Init_System();

    /* ���¸���ʵ����Ҫ�������� */
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
    __enable_irq(); //��ȫ���ж�ʹ��
}

#if API_UseADC > 0
/*
*********************************************************************************************************
*	�� �� ��: ADC_Exp
*	����˵��: ADCʹ�����̣����ɼ��ɹ�����4λ��ѹֵ,3λС�������ɼ�ʧ��,�̶�����0xffffffff
*            �ɼ���ɺ�ص��������Զ��ر�ADC�ж�,������ɺ�رչܽ�
*	��    ��: CH_INx ��Ҫʹ�õ�ADͨ����
*	�� �� ֵ: �ɼ����ĵ�ѹֵ
*********************************************************************************************************
*/
uint32_t ADC_Exp(uint8_t CH_INx)
{
    ADC_init.Nvicpry = 2;             //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
    ADC_init.ChSel_def = CH_INx;      //ѡ��ʹ�õĲɼ�ͨ��
    ADC_init.TrimV = SmpSpd_Fast;     //ѡ�����ɼ��ٶ�
    ADC_init.SampleNeed = 2;          //�ɼ�������СΪ2
    ADC_init.SampleLevel = SmpLve_3V; //�����ĵ�ѹС��3V��������3VС��5Vѡ�� SmpLve_5V  ������ѹ���ܴ���5V

    return API_ADC(&ADC_init); //����4λ���ĵ�ѹֵ,3λС��
}

float Temp_Exp(void)
{
    ADC_init.Nvicpry = 2;             //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
    ADC_init.ChSel_def = CH_PTAT;     //ѡ��ʹ�õĲɼ�ͨ��(�²�)
    ADC_init.TrimV = SmpSpd_Fast;     //ѡ�����ɼ��ٶ�
    ADC_init.SampleNeed = 4;          //�ɼ�������СΪ2
    ADC_init.SampleLevel = SmpLve_3V; //�����ĵ�ѹС��3V��������3VС��5Vѡ�� SmpLve_5V  ������ѹ���ܴ���5V

    return API_Temperature(&ADC_init); //����4λ���ĵ�ѹֵ,3λС��
}

#endif

#if API_UseETIM > 0
/*
*********************************************************************************************************
*	�� �� ��: ETIM_INIT_Exp
*	����˵��: ETIMʹ�����̣�����Ĭ�����ÿ��жϷ�ʽ���粻��Ҫ�����޸�API������ж�ʹ��
*            ������ETIM�󲻻�����������ʱ��,��Ҫ����API_ETim_Start����
*	��    ��: ETIMx_Type �ͽṹ�����
*             pre����Ƶϵ��
*             count������ֵ ���0xffff
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ETIM_INIT_Exp(ETIMx_Type *ETIMx, uint8_t pre, uint16_t count)
{
    ETIM_INIT_Type ETIM_init;

    ETIM_init.Nvicpry = 2;   //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
    ETIM_init.PRE = pre;     //1-255;   ����Ƶ��Ƶ  1Ϊ1��Ƶ������Ƶ
    ETIM_init.count = count; //����ֵ   ����Ƶ8M,100��Ƶ,800/80000=10ms

    API_Etim_Init(ETIMx, &ETIM_init);

    //	API_ETim_Start(ETIMx);
}

#endif

#if API_UseLPTIM > 0
/*----------------------------------------------------------------------
//LPTIMʹ������
//�̻���LPTIMΪ��������ж�,�����ʹ�ñ���ʹ���ж�
//������LPTIM�󲻻�����������ʱ��,��Ҫ����API_LPTIM_Start����
//LPTIM_init.PRE��˵��
//0:����ʱ��1��Ƶ  1:����ʱ��2��Ƶ  2:����ʱ��4��Ƶ
//3:����ʱ��8��Ƶ  4:����ʱ��16��Ƶ  5:����ʱ��32��Ƶ
//6:����ʱ��64��Ƶ  7��7����:����ʱ��128��Ƶ
-----------------------------------------------------------------------*/
void LPTIM_Init_Exp(void)
{
    LPTIM_INIT_Type LPTIM_init;

    LPTIM_init.Nvicpry = 2; //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
    LPTIM_init.PRE = 7;     //0-7
    LPTIM_init.count = 256; //128*256=32768--->1s 256

    API_LPTIM_Init(&LPTIM_init);

    //API_LPTIM_Start();
}
/*
*********************************************************************************************************
*	�� �� ��: LPTIM_Init_Macro
*	����˵��: LPTIMʹ�����̣�����Ĭ�����ÿ��жϷ�ʽ���粻��Ҫ�����޸�API������ж�ʹ��
*            ������󲻻�����������ʱ��,��Ҫ����API_LPTIM_Start����
*	��    ��: pre����Ƶϵ��  0-7  ��Ƶ����Ϊ 2^(pre+1)
*             count������ֵ ���0xffffffff
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LPTIM_Init_Macro(uint8_t pre, uint32_t cnt)
{
    LPTIM_INIT_Type LPTIM_init;

    LPTIM_init.Nvicpry = 2; //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
    LPTIM_init.PRE = pre;   //0-7
    LPTIM_init.count = cnt; //128*256=32768--->1s 256

    API_LPTIM_Init(&LPTIM_init);

    //API_LPTIM_Start();
}
#endif

#if API_UseRTC > 0
/*
*********************************************************************************************************
*	�� �� ��: RTC_Init_Exp
*	����˵��: RTCʹ�����̣�����Ĭ�����ÿ��жϷ�ʽ���粻��Ҫ�����޸�API���ж�ʹ��
*            ���õ�RTC�ж�ȫ��
*            API_GetTime(RealTime) �ܹ���ȡ7B��BCD��ʽ��7B��DEC��ʽ��ʱ��
*            API_SetTIME() ����ʹ��BCD��ʽ�������ã��Դ����ںϷ��ж�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RTC_Init_Exp(void)
{
    RTC_INIT_Type RTC_init;
    //	RTC_TimeDateTypeDef TempTime;

    RTC_init.Nvicpry = 0; //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
    RTC_init.day = 1;     //����RTC���ж�
    RTC_init.hour = 1;    //����RTCʱ�ж�
    RTC_init.min = 1;     //����RTC���ж�
    RTC_init.sec = 1;     //����RTC���ж�

    API_RTC_Init(&RTC_init); //��ʼ��

    // API_GetTime(RealTime); //��ȡRTCʱ��

    // TempTime.Year = 0x20;
    // TempTime.Month = 0x6;
    // TempTime.Date = 0x18;
    // TempTime.Hour = 0x15;
    // TempTime.Minute = 0x56;
    // TempTime.Second = 0x23;
    // API_SetTIME(&TempTime); //����ʹ��BCD��ʽ
}
#endif

#if API_UseIIC > 0 /* Ӳ��IIC���̣�ʹ�ýṹ����д���������ο� */
uint32_t epromstart;

#define EPROM_POWER_ON            /* Ӳ��iic�ϵ磬��Ҫ���� */
#define EPROM_POWER_OFF           /* Ӳ��iic�µ磬��Ҫ���� */
#define EPROM_I2cStart epromstart /* �ṹ���������߽ṹ��ĵ�һ����Ա */

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
*	�� �� ��: Uart_Init_Macro
*	����˵��: UARTʹ������
*	��    ��: UARTx_Type �ͽṹ�����
*             baudrate ����Ҫ�Ĳ�����
*             RxIeEn �������ж��Ƿ�ʹ��
*             TxIeEn �� �����ж��Ƿ�ʹ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Uart_Init_Macro(UARTx_Type *Uartx, uint32_t baudrate, uint8_t useirm, bool RxIeEn, bool TxIeEn, bool Stopbit)
{
    Uart_INIT_Type Uart_init;

    Uart_init.Nvicpry = 1; //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
#ifdef U1UsePE3PE4
    Uart_init.SEL = Uart1_UsePE3PE4; //ֻ��ʹ��UART1ʱ�����Ҫѡ��
#else
    Uart_init.SEL = Uart1_UsePB0PB1; //ֻ��ʹ��UART1ʱ�����Ҫѡ��
#endif
    Uart_init.IRMod = useirm;              //��ʹ�ú���38K����
    Uart_init.RxIEEN = RxIeEn;             //���������ж�
    Uart_init.TxIEEN = TxIeEn;             //���������ж�
    Uart_init.uartint.BaudRate = baudrate; //���ò�����
    Uart_init.uartint.DataBit = Eight8Bit; //8λ����λ
    Uart_init.uartint.ParityBit = NONE;    //����У��
    Uart_init.uartint.StopBit = Stopbit;   //ֹͣλ

    API_Uartx_Init(Uartx, &Uart_init); //����Uart
}

#endif

#if API_UseLPUart > 0
/*----------------------------------------------------------------------
//LPUartʹ������
//����ʹ���жϷ�,�ṩ�˲�ѯ���͵�Api����
//void API_LPUart_Send(uint8_t *str,uint16_t len)
-----------------------------------------------------------------------*/
void LPUart_Init_Exp(void)
{
    LPUart_INIT_Type Uart_init;

    Uart_init.Nvicpry = 1;                 //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
    Uart_init.RxIEEN = ENABLE;             //���������ж�
    Uart_init.uartint.BaudRate = 9600;     //���ò�����
    Uart_init.uartint.DataBit = Eight8Bit; //8λ����λ
    Uart_init.uartint.ParityBit = NONE;    //����У��
    Uart_init.uartint.StopBit = TwoBit;    //��λֹͣλ

    API_LPUart_Init(&Uart_init); //����Uart
}
/*
*********************************************************************************************************
*	�� �� ��: LPUart_Init_Macro
*	����˵��: LPUARTʹ������
*	��    ��: baudrate ����Ҫ�Ĳ����� ��Ҫ����9600
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LPUart_Init_Macro(uint32_t baudrate)
{
    LPUart_INIT_Type Uart_init;

    Uart_init.Nvicpry = 1;                 //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
    Uart_init.RxIEEN = ENABLE;             //���������ж�
    Uart_init.uartint.BaudRate = baudrate; //���ò�����
    Uart_init.uartint.DataBit = Eight8Bit; //8λ����λ
    Uart_init.uartint.ParityBit = NONE;    //����У��
    Uart_init.uartint.StopBit = TwoBit;    //��λֹͣλ

    API_LPUart_Init(&Uart_init); //����Uart
}
#endif

#if API_UseAES > 0
/*
*********************************************************************************************************
*	�� �� ��: AES_Exp
*	����˵��: AESʹ������
*	��    ��: in ������Ҫ���ܵ����ݣ�������16�ֽڵ�����
*             key ��������Կ��������16�ֽڵ�����
*             out �� ���������ɵ����ݣ���������16�ֽڵ�����
*	�� �� ֵ: ��
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

    init.control = AESENCRYPT; //����
    init.mode = AESECBMODE;    //ECB
    init.len = AES_128Bit;     //16�ֽڳ���

    API_AES(&init);
}

#endif

#if API_UseLCD > 0
/*----------------------------------------------------------------------
//LCDʹ������
//ע��ܽŵĳ�ʼ�� ��LCD_IO_Init()�г�ʼ��
//�̻���APIĬ��Ϊ4COM��1/3ƫ��
-----------------------------------------------------------------------*/
void LCD_Init_Exp(void)
{
    LCD_Init_Type lcd_init;

    lcd_init.control = LcdOpen;
    lcd_init.Brightness = 15;      //�ҶȲ��ܴ���15  Խ��Խ�������ǹ���Խ��
    lcd_init.Refreshfrequency = 3; //�����޸�ˢ��Ƶ�ʽ��͹���
    lcd_init.Com32_43Set = 0;      //����ʹ��Com32���ϵ�Һ���ܽţ�һ�����ó�0
    lcd_init.Com0_31Set = 0;
    /*��������ֻ������0-31com�����  ���ʹ���˳���com31����Ҫ������д*/
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
