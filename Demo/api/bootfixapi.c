#include "bootfixapi.h"

#if UseBootFixFun > 0 && ThisBoot == 0

uint32_t *table_base = (uint32_t *)(Boot_API_Base_Adr); /* 功能函数表 */

#if API_UseADC > 0

uint32 API_ADC(ADC_INIT_Type *init)
{
    //初始化函数指针
    gy_adcfun adc = (uint32(*)(ADC_INIT_Type * init))(table_base[API_ADC_Loc] | 0x1);
    return adc(init);
}

#endif

#if UseETIM1 > 0 || UseETIM2 > 0 || UseETIM3 > 0 || UseETIM4 > 0 || Use_UartTimeOut > 0

void API_Etim_Init(ETIMx_Type *ETIMx, ETIM_INIT_Type *init)
{
    //初始化函数指针
    gy_etiminit etim = (void (*)(ETIMx_Type * ETIMx, ETIM_INIT_Type * init))(table_base[API_ETIM_Loc] | 0x1);
    etim(ETIMx, init);
}

#endif

#if API_UseIIC > 0

void API_I2C_Init(void)
{
    gy_iicinit iic = (void (*)(void))(table_base[API_I2C_Init_Loc] | 0x1);
    iic();
}

uint8_t API_EEPROM_Read(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    gy_iicop iicrd = (uint8_t(*)(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t * Buf, uint16_t Len))(table_base[API_I2C_Read_Loc] | 0x1);
    return iicrd(Device, Addr, AddrLen, Buf, Len);
}

uint8_t API_EEPROM_Write(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    gy_iicop iicwt = (uint8_t(*)(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t * Buf, uint16_t Len))(table_base[API_I2C_Write_Loc] | 0x1);
    return iicwt(Device, Addr, AddrLen, Buf, Len);
}

uint8_t API_I2C_Read_Com(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    gy_iicop iicrd = (uint8_t(*)(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t * Buf, uint16_t Len))(table_base[API_I2C_Read_Com_Loc] | 0x1);
    return iicrd(Device, Addr, AddrLen, Buf, Len);
}

uint8_t API_I2C_Write_Com(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    gy_iicop iicwt = (uint8_t(*)(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t * Buf, uint16_t Len))(table_base[API_I2C_Write_Com_Loc] | 0x1);
    return iicwt(Device, Addr, AddrLen, Buf, Len);
}
#endif

#if API_UseLPTIM > 0

void API_LPTIM_Init(LPTIM_INIT_Type *init)
{
    gy_lptiminit lp = (void (*)(LPTIM_INIT_Type * init))(table_base[API_LPTIM_Loc] | 0x1);
    lp(init);
}

#endif

#if API_UseRTC > 0

void API_RTC_Init(RTC_INIT_Type *init)
{
    gy_rtcinit rtc = (void (*)(RTC_INIT_Type * init))(table_base[API_RTC_Init_Loc] | 0x1);
    rtc(init);
}

void API_GetTime(uint8_t *tim)
{
    gy_gttim gtt = (void (*)(uint8_t * tim))(table_base[API_GetTime_Loc] | 0x1);
    gtt(tim);
}

uint8_t API_SetTIME(RTC_TimeDateTypeDef *para)
{
    gy_sttim stt = (uint8_t(*)(RTC_TimeDateTypeDef * para))(table_base[API_SetTIME_Loc] | 0x1);
    return stt(para);
}

#endif

#if UseUart0 > 0 || UseUart1 > 0 || UseUart2 > 0 || UseUart3 > 0 || UseUart4 > 0 || UseUart5 > 0 || Uart_Output > 0

void API_Uartx_Init(UARTx_Type *Uartx, Uart_INIT_Type *init)
{
    gy_uartinit uartinit = (void (*)(UARTx_Type * Uartx, Uart_INIT_Type * init))(table_base[API_Uart_Init_Loc] | 0x1);
    uartinit(Uartx, init);
}

/*注意：该API没有限定长度，注意数组越界*/
void API_Uart_Send(UARTx_Type *UARTx, uint8_t *str, uint16_t len)
{
    gy_uartsend uartsend = (void (*)(UARTx_Type * UARTx, uint8_t * str, uint16_t len))(table_base[API_Uart_Send_Loc] | 0x1);
    uartsend(UARTx, str, len);
}

#endif

#if API_UseLPUart > 0

void API_LPUart_Init(LPUart_INIT_Type *init)
{
    gy_lputinit lputinit = (void (*)(LPUart_INIT_Type * init))(table_base[API_LPUart_Init_Loc] | 0x1);
    lputinit(init);
}

void API_LPUart_Send(uint8_t *str, uint16_t len)
{
    gy_lputsend lputsend = (void (*)(uint8_t * str, uint16_t len))(table_base[API_LPUart_Send_Loc] | 0x1);
    lputsend(str, len);
}

#endif

#if API_UseDataUp > 0

uint8_t API_UpData(uint8_t *buf, uint16_t len, Upstruct *UpStc)
{
    gy_updata updata = (uint8_t(*)(uint8_t * buf, uint16_t len, Upstruct * UpStc))(table_base[API_UpData_Loc] | 0x1);
    return updata(buf, len, UpStc);
}
#endif

#if API_UseNB > 0

int GY_NB_Link(NB_Type *NB)
{
    gy_nbtya nblink = (int (*)(NB_Type * NB))(table_base[API_NB_Link_Loc] | 0x1);
    return nblink(NB);
}

int GY_NB_Connect(NB_Type *NB)
{
    gy_nbtya nbcnet = (int (*)(NB_Type * NB))(table_base[API_NB_Connect_Loc] | 0x1);
    return nbcnet(NB);
}

void GY_NB_Init(NB_Type *NB)
{
    gy_nbtyb nbinit = (void (*)(NB_Type * NB))(table_base[API_NB_Init_Loc] | 0x1);
    nbinit(NB);
}

void GY_NB_UartRx(NB_Type *NB)
{
    gy_nbtyb nbrx = (void (*)(NB_Type * NB))(table_base[API_NB_UartRx_Loc] | 0x1);
    nbrx(NB);
}

void GY_NB_UartTx(NB_Type *NB)
{
    gy_nbtyb nbtx = (void (*)(NB_Type * NB))(table_base[API_NB_UartTx_Loc] | 0x1);
    nbtx(NB);
}

void GY_NB_Clock(NB_Type *NB)
{
    gy_nbtyb nbclk = (void (*)(NB_Type * NB))(table_base[API_NB_Clock_Loc] | 0x1);
    nbclk(NB);
}

void GY_NB_End(NB_Type *NB)
{
    gy_nbtyb nbend = (void (*)(NB_Type * NB))(table_base[API_NB_End_Loc] | 0x1);
    nbend(NB);
}

uint16_t GY_NB_RxData(NB_Type *NB, uint8_t *buf, const char *word)
{
    gy_nbrxdatatype nbrxdata = (uint16_t(*)(NB_Type * NB, uint8_t * buf, const char *word))(table_base[API_NB_RXDATA_Loc] | 0x1);
    return nbrxdata(NB, buf, word);
}

#endif

#if API_UseLCD > 0

void API_Init_LCD(LCD_Init_Type *init)
{
    gy_lcdinit lcdinit = (void (*)(LCD_Init_Type * init))(table_base[API_LCD_Loc] | 0x1);
    lcdinit(init);
}

#endif

#if API_UseAES > 0

void API_AES(AES_Init_Type *init)
{
    gy_aestype aes = (void (*)(AES_Init_Type * init))(table_base[API_AES_Loc] | 0x1);
    aes(init);
}

#endif

#endif

#if ThisBoot > 0 && UseBootFixFun > 0
/*----------------------------------------------------------------------
//固化API函数表
-----------------------------------------------------------------------*/
const uint32_t func_table[] __attribute__((section(".ARM.__at_0x00001000"))) =
    {
//注:这里会存在一些警告,无可避免
//注:顺序要想改必须同步更改 API_xxx_Loc
#if API_UseADC > 0
        (uint32_t)&API_ADC,
#else
        (uint32_t)API_ADC_Loc,      //如果不用于占位
#endif
#if UseETIM1 > 0 || UseETIM2 > 0 || UseETIM3 > 0 || UseETIM4 > 0 || Use_UartTimeOut > 0
        (uint32_t)&API_Etim_Init,
#else
        (uint32_t)API_ETIM_Loc,     //如果不用于占位
#endif
#if API_UseIIC > 0
        (uint32_t)&API_I2C_Init,
        (uint32_t)&API_EEPROM_Read,
        (uint32_t)&API_EEPROM_Write,
        (uint32_t)&API_I2C_Read_Com,
        (uint32_t)&API_I2C_Write_Com,
#else
        (uint32_t)API_I2C_Init_Loc, //如果不用于占位
        (uint32_t)API_I2C_Read_Loc,
        (uint32_t)API_I2C_Write_Loc,
        (uint32_t)API_I2C_Read_Com_Loc, //如果不用于占位
        (uint32_t)API_I2C_Write_Com_Loc,
#endif
#if API_UseLPTIM > 0
        (uint32_t)&API_LPTIM_Init,
#else
        (uint32_t)API_LPTIM_Loc,    //如果不用于占位
#endif
#if API_UseRTC > 0
        (uint32_t)&API_RTC_Init,
        (uint32_t)&API_GetTime,
        (uint32_t)&API_SetTIME,
#else
        (uint32_t)API_RTC_Init_Loc, //如果不用于占位
        (uint32_t)API_GetTime_Loc,
        (uint32_t)API_SetTIME_Loc,
#endif
#if UseUart0 > 0 || UseUart1 > 0 || UseUart2 > 0 || UseUart3 > 0 || UseUart4 > 0 || UseUart5 > 0 || Uart_Output > 0
        (uint32_t)&API_Uartx_Init,
        (uint32_t)&API_Uart_Send,
#else
        (uint32_t)API_Uart_Init_Loc, //如果不用于占位
        (uint32_t)API_Uart_Send_Loc,
#endif
#if API_UseLPUart > 0
        (uint32_t)&API_LPUart_Init,
        (uint32_t)&API_LPUart_Send,
#else
        (uint32_t)API_LPUart_Init_Loc, //如果不用于占位
        (uint32_t)API_LPUart_Send_Loc,
#endif
#if API_UseDataUp > 0
        (uint32_t)&API_UpData,
#else
        (uint32_t)API_UpData_Loc,
#endif
#if API_UseNB > 0
        (uint32_t)&GY_NB_Init,
        (uint32_t)&GY_NB_Link,
        (uint32_t)&GY_NB_Connect,
        (uint32_t)&GY_NB_UartRx,
        (uint32_t)&GY_NB_UartTx,
        (uint32_t)&GY_NB_Clock,
        (uint32_t)&GY_NB_End,
        (uint32_t)&GY_NB_RxData,
#else
        (uint32_t)API_NB_Init_Loc,
        (uint32_t)API_NB_Link_Loc,
        (uint32_t)API_NB_Connect_Loc,
        (uint32_t)API_NB_UartRx_Loc,
        (uint32_t)API_NB_UartTx_Loc,
        (uint32_t)API_NB_Clock_Loc,
        (uint32_t)API_NB_End_Loc,
        (uint32_t)API_NB_RXDATA_Loc,
#endif
#if API_UseLCD > 0
        (uint32_t)&API_Init_LCD,
#else
        (uint32_t)API_LCD_Loc,
#endif
#if API_UseAES > 0
        (uint32_t)&API_AES,
#else
        (uint32_t)API_AES_Loc,
#endif
        (uint32_t)99};

#endif
