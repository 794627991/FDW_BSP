/*
*********************************************************************************************************
*
*	ģ������ : LPTIM_BSP��ض���
*	�ļ����� : lptim_api.h
*	��    �� : V1.0.0
*	˵    �� : �͹��Ķ�ʱ�������Ͷ���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
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
        uint8_t Nvicpry; /* �ж����ȼ� */
        uint8_t PRE;     /* ��Ƶϵ��ѡ��0-7 */
        uint16_t count;  /* ����ֵ��0-0xffff */
    } LPTIM_INIT_Type;

    /**************************����ָ�붨��**************************/
    typedef void (*gy_lptiminit)(LPTIM_INIT_Type *init);

    /****************************��������****************************/
    void API_LPTIM_Init(LPTIM_INIT_Type *init);
    void API_LPTIM_Start(void);
    void API_LPTIM_Stop(void);

#ifdef __cplusplus
}
#endif

#endif
