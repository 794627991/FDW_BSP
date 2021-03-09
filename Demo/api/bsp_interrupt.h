/*
*********************************************************************************************************
*
*	模块名称 : 中断处理及回调程序
*	文件名称 : bsp_interrupt.h
*	版    本 : V1.0.0
*	说    明 : 中断相关
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
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
#include "bsp_adc.h"
#include "bsp_uart_ex.h"

    /****************************中断回调****************************/
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
