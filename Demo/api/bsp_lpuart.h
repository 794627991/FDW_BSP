/*
*********************************************************************************************************
*
*	模块名称 : LPUART_BSP相关定义
*	文件名称 : bsp_lpuart.h
*	版    本 : V1.0.0
*	说    明 : 相关定义和声明
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __BSP_LPUART_H__
#define __BSP_LPUART_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_def.h"

    typedef struct
    {
        uint8_t Nvicpry; /* 中断优先级 */
        uint8_t RxIEEN;  /* 接收中断使能 */
        uint8_t TxIEEN;  /* 接收中断使能 */
        LPUART_SInitTypeDef uartint;
    } LPUart_INIT_Type;

    /**************************函数指针定义**************************/
    typedef void (*gy_lputinit)(LPUart_INIT_Type *init);
    typedef void (*gy_lputsend)(uint8_t *str, uint16_t len);

    /****************************函数声明****************************/
    void API_LPUart_Init(LPUart_INIT_Type *init);
    void API_LPUart_Send(uint8_t *str, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
