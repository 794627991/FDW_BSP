/*
*********************************************************************************************************
*
*	???? : ?????????
*	???? : bsp_interrupt.h
*	?    ? : V1.0.0
*	?    ? : ????
*	???? :
*
*		???     ??      ??    ??
*		V1.0.0  2021-1-5    ??  ????
*
*	Copyright (C), 2020-2030,  ????-??
*
*********************************************************************************************************
*/
#ifndef __BSP_INTERRUPT_H__
#define __BSP_INTERRUPT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

    /****************************ÖÐ¶Ï»Øµ÷****************************/
    void GPIO_IRQ_CallBack(void);

    void UART0_IRQ_Rx_CallBack(uint8_t data);
    void UART1_IRQ_Rx_CallBack(uint8_t data);
    void UART2_IRQ_Rx_CallBack(uint8_t data);
    void UART3_IRQ_Rx_CallBack(uint8_t data);
    void UART4_IRQ_Rx_CallBack(uint8_t data);
    void UART5_IRQ_Rx_CallBack(uint8_t data);
    void LPUART_IRQ_Rx_CallBack(uint8_t data);

    void ETIM1_IRQ_CallBack(void);
    void ETIM2_IRQ_CallBack(void);
    void ETIM3_IRQ_CallBack(void);
    void ETIM4_IRQ_CallBack(void);
    void LPTIM_IRQ_CallBack(void);

    void RTC_IRQ_SEC_CallBack(void);
    void RTC_IRQ_MIN_CallBack(void);
    void RTC_IRQ_HOUR_CallBack(void);
    void RTC_IRQ_DATE_CallBack(void);

#ifdef __cplusplus
}
#endif

#endif
