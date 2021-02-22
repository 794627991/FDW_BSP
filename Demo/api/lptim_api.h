/*
*********************************************************************************************************
*
*	模块名称 : LPTIM_BSP相关定义
*	文件名称 : lptim_api.h
*	版    本 : V1.0.0
*	说    明 : 低功耗定时器声明和定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __API_LPTIM_H__
#define __API_LPTIM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

    typedef struct
    {
        uint8_t Nvicpry; /* 中断优先级 */
        uint8_t PRE;     /* 分频系数选择：0-7 */
        uint16_t count;  /* 计数值：0-0xffff */
    } LPTIM_INIT_Type;

    /**************************函数指针定义**************************/
    typedef void (*gy_lptiminit)(LPTIM_INIT_Type *init);

    /****************************函数声明****************************/
    void API_LPTIM_Init(LPTIM_INIT_Type *init);
    void API_LPTIM_Start(void);
    void API_LPTIM_Stop(void);

#ifdef __cplusplus
}
#endif

#endif
