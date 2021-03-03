#ifndef _API_Exp_H__
#define _API_Exp_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

    void User_IO_Init(void);
    void API_ShowEvery(char *ledbuf);
    void Sysinit_Exp(void);
    void LPTIM_Init_Exp(void);
    uint32_t ADC_Exp(uint8_t CH_INx);
    float Temp_Exp(void);
    void AES_Exp(uint8_t *in, uint8_t *key, uint8_t *out);
    void Uart_Init_Exp(UARTx_Type *Uartx, uint32_t baudrate, uint8_t useirm);
    void RTC_Init_Exp(void);
    void LCD_Init_Exp(void);
    void ETIM_INIT_Exp(ETIMx_Type *ETIMx, uint8_t pre, uint16_t count);

    void Uart_Init_Macro(UARTx_Type *Uartx, uint32_t baudrate, uint8_t useirm, bool RxIeEn, bool TxIeEn, bool Stopbit);
    void LPUart_Init_Macro(uint32_t baudrate);
    void LPTIM_Init_Macro(uint8_t pre, uint32_t cnt);

    void __API_EPROM_Read(uint32_t base, uint8_t *Buf, uint16_t len);
    void __API_EPROM_Save(uint32_t base, uint8_t *Buf, uint16_t len);
    void API_SaveAdr_Init(void);

#define API_EPROM_Save(base, Buf, len)    \
    {                                     \
        API_SaveAdr_Init();               \
        __API_EPROM_Save(base, Buf, len); \
    }

#define API_EPROM_Read(base, Buf, len)    \
    {                                     \
        API_SaveAdr_Init();               \
        __API_EPROM_Read(base, Buf, len); \
    }

#define API_IIC_SAVE_TO(name, buf) API_EPROM_Save(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, buf, GetLen(name))
#define API_IIC_READ_TO(name, buf) API_EPROM_Read(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, buf, GetLen(name))
#define API_IIC_SAVE(name) API_EPROM_Save(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, GetAdr(name), GetLen(name))
#define API_IIC_READ(name) API_EPROM_Read(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, GetAdr(name), GetLen(name))

#ifdef __cplusplus
}
#endif

#endif
