/*
*********************************************************************************************************
*
*	ģ������ : LPUART_BSP��ض���
*	�ļ����� : bsp_lpuart.h
*	��    �� : V1.0.0
*	˵    �� : ��ض��������
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
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
        uint8_t Nvicpry; /* �ж����ȼ� */
        uint8_t RxIEEN;  /* �����ж�ʹ�� */
        uint8_t TxIEEN;  /* �����ж�ʹ�� */
        LPUART_SInitTypeDef uartint;
    } LPUart_INIT_Type;

    /**************************����ָ�붨��**************************/
    typedef void (*gy_lputinit)(LPUart_INIT_Type *init);
    typedef void (*gy_lputsend)(uint8_t *str, uint16_t len);

    /****************************��������****************************/
    void API_LPUart_Init(LPUart_INIT_Type *init);
    void API_LPUart_Send(uint8_t *str, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
