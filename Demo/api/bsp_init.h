/*
*********************************************************************************************************
*
*	ģ������ : BSP��ʼ��
*	�ļ����� : bsp_init.h
*	��    �� : V1.0.0
*	˵    �� : �ж����
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#ifndef _BSP_INIT_H__
#define _BSP_INIT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_def.h"
#include "bsp_inc.h"

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

    void Uart_Init_Macro(UARTx_Type *Uartx, uint32_t baudrate, uint8_t useirm, bool RxIeEn, bool TxIeEn, UART_StopBitTypeDef Stopbit);
    void LPUart_Init_Macro(uint32_t baudrate);
    void LPTIM_Init_Macro(uint8_t pre, uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif
