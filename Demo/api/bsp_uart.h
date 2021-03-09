/*
*********************************************************************************************************
*
*	ģ������ : ����BSP(FOR����΢G045/G048)
*	�ļ����� : uart_api.h
*	��    �� : V1.0.0
*	˵    �� : ���ڳ�ʼ���������жϴ����жϷ��ͻ�ѭ������BSP
*             ���ö��з�ʽ���ػ��洮�����ݣ������ⲿ����malloc.c
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����  ��ʽ����
*
*	Copyright (C), 2020-2030, ����˼��-����
*
*********************************************************************************************************
*/

#ifndef __API_URAT_H__
#define __API_URAT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

    typedef struct
    {
        uint8_t Nvicpry; /* �ж����ȼ� */
        uint8_t SEL;     /* ֻ��ʹ��uart1ʱ������� */
        uint8_t IRMod;   /* ֻ����Ϊ1��ʹ�ú������ */
        uint8_t RxIEEN;  /* �����ж�ʹ�� */
        uint8_t TxIEEN;  /* �����ж�ʹ�� */
        UART_SInitTypeDef uartint;
    } Uart_INIT_Type;

#define USEIRMod 1        /* ʹ�õ��ƺ��� */
#define NoUSEIRMod 0      /* ��ʹ�õ��ƺ��� */
#define Uart1_UsePB0PB1 1 /* ʹ��UART1��PB0PB1�� */
#define Uart1_UsePE3PE4 0 /* ʹ��UART1��PE3PE4�� */

/* ���崮�ڽ��յ���󳤶� */
#ifndef U0Txlen
#define U0Txlen 1
#endif
#ifndef U0Rxlen
#define U0Rxlen 500
#endif

#ifndef U1Txlen
#define U1Txlen 1
#endif
#ifndef U1Rxlen
#define U1Rxlen 500
#endif

#ifndef U2Txlen
#define U2Txlen 1
#endif
#ifndef U2Rxlen
#define U2Rxlen 500
#endif

#ifndef U3Txlen
#define U3Txlen 1
#endif
#ifndef U3Rxlen
#define U3Rxlen 500
#endif

#ifndef U4Txlen
#define U4Txlen 1
#endif
#ifndef U4Rxlen
#define U4Rxlen 500
#endif

#ifndef U5Txlen
#define U5Txlen 1
#endif
#ifndef U5Rxlen
#define U5Rxlen 500
#endif

#define MAXSIZE 8 /* ��󻺴����� */
#define MAXUART 6 /* ��󴮿����� */

    typedef struct
    {
        uint16_t len;
        uint16_t maxlen;
        uint8_t pData[]; /* ��������,����ҪC99���� */
    } ComData;

    typedef struct
    {
        ComData *dat[MAXSIZE];
        uint8_t iHead;
        uint8_t iCount;
        uint8_t intodelete;
    } ComQueue;

    typedef struct
    {
        ComData *dat[MAXUART];
    } COMINFO;

    typedef struct
    {
        ComQueue *queue[MAXUART];
    } COMALL;

    /**************************����ָ�붨��**************************/
    typedef void (*gy_uartinit)(UARTx_Type *Uartx, Uart_INIT_Type *init);
    typedef void (*gy_uartsend)(UARTx_Type *UARTx, uint8_t *str, uint16_t len);
    typedef void (*rxdohandle)(uint8_t *buf, uint16_t len);
    /****************************��������****************************/
    void API_Uartx_Init(UARTx_Type *Uartx, Uart_INIT_Type *init);      /* ���ڳ�ʼ�� */
    void API_Uart_Send(UARTx_Type *UARTx, uint8_t *str, uint16_t len); /* �������ݷ���1 ʹ��ʵ��uart */
    void API_UART_SEND(uint8_t uartx, uint8_t *data, uint16_t len);    /* �������ݷ���2 ʹ�ô��ں� */
    void API_Uartx_Op(bool op, uint8_t uartx);                         /* ���ڲ���0:�� 1:�� */

    void API_UART_Rx_Queue_Get_Do(uint8_t uartx, rxdohandle rxdo); /* �Ӷ����л�ȡ��Ӧ���ںŵ����ݣ�Ȼ��������ָ������� */
    ComData *API_UART_Rx_Queue_Get(uint8_t uartx);                 /* �Ӷ����л�ȡ��Ӧ���ںŵ����ݣ���Ҫ�ǵ�myfree��ȡ������ */
    void API_UART_Rx_Queue_Send(uint8_t uartx);                    /* �������ݽ�����ɺ󽫶�Ӧ���ںŵ����ݷ��͵������� */
    /****************************���ڶ���****************************/
    uint8_t QueueIsFull(ComQueue *queueHead);                          /* ������ */
    uint8_t QueueIsEmpty(ComQueue *queueHead);                         /* ���п� */
    ComQueue *InitComMessage(void);                                    /* ��������ʼ������ */
    void SendComMessage(ComQueue *queueHead, ComData *_dat);           /* ����з�����Ϣ */
    ComData *GetComMessage(ComQueue *queueHead);                       /* �Ӷ��л�ȡ��Ϣ */
    void __DelComMessage(ComQueue *queue);                             /* ɾ������ */
    void UartTxInterupt(uint8_t uartx);                                /* �����жϷ����� */
    void UartRxInterupt(uint8_t data, uint8_t uartx, uint16_t maxlen); /* �����ж��յ��� */
    ComData *GetComInfo(uint8_t uartx);                                /* ֱ�ӻ�ȡ�������ж��е����� */
    void FreeComInfo(uint8_t uartx);                                   /* ʹ��GetComInfo����� */
    
#define DelComMessage(x)    \
    {                       \
        __DelComMessage(x); \
        x = NULL;           \
    }

#ifdef __cplusplus
}
#endif

#endif
