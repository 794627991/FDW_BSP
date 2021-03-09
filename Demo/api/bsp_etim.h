/*
*********************************************************************************************************
*
*	ģ������ : ETIM����(FOR����΢G045/G048)
*	�ļ����� : bsp_etim.h
*	��    �� : V1.0.0
*	˵    �� : ETIM��ض���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
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
        uint8_t Nvicpry; /* �ж����ȼ� */
        uint8_t PRE;     /* ��Ƶϵ��ѡ��  �������1С��0xff */
        uint16_t count;  /* ����ֵ  0-0xffff */
    } ETIM_INIT_Type;

    /**************************����ָ�붨��**************************/
    typedef void (*gy_etiminit)(ETIMx_Type *ETIMx, ETIM_INIT_Type *init);

    /****************************��������****************************/
    void API_ETim_Start(ETIMx_Type *ETIMx);
    void API_ETim_Stop(ETIMx_Type *ETIMx);
    void API_ETim_ReSet(ETIMx_Type *ETIMx);
    void API_Etim_Init(ETIMx_Type *ETIMx, ETIM_INIT_Type *init);

#ifdef __cplusplus
}
#endif

#endif
