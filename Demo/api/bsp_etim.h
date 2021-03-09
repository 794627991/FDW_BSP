/*
*********************************************************************************************************
*
*	模块名称 : ETIM定义(FOR复旦微G045/G048)
*	文件名称 : bsp_etim.h
*	版    本 : V1.0.0
*	说    明 : ETIM相关定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-12-31  高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __BSP_ETIM_H__
#define __BSP_ETIM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_def.h"

    typedef struct
    {
        uint8_t Nvicpry; /* 中断优先级 */
        uint8_t PRE;     /* 分频系数选择  必须大于1小于0xff */
        uint16_t count;  /* 计数值  0-0xffff */
    } ETIM_INIT_Type;

    /**************************函数指针定义**************************/
    typedef void (*gy_etiminit)(ETIMx_Type *ETIMx, ETIM_INIT_Type *init);

    /****************************函数声明****************************/
    void API_ETim_Start(ETIMx_Type *ETIMx);
    void API_ETim_Stop(ETIMx_Type *ETIMx);
    void API_ETim_ReSet(ETIMx_Type *ETIMx);
    void API_Etim_Init(ETIMx_Type *ETIMx, ETIM_INIT_Type *init);

#ifdef __cplusplus
}
#endif

#endif
