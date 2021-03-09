/*
*********************************************************************************************************
*
*	模块名称 : 串口BSP(FOR复旦微G045/G048)
*	文件名称 : uart_api.h
*	版    本 : V1.0.0
*	说    明 : 串口初始化，串口中断处理，中断发送或循环发送BSP
*             采用队列方式多重缓存串口数据，必须外部调用malloc.c
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-12-31  高屹  正式发布
*
*	Copyright (C), 2020-2030, 辽宁思凯-高屹
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
        uint8_t Nvicpry; /* 中断优先级 */
        uint8_t SEL;     /* 只有使用uart1时候才有用 */
        uint8_t IRMod;   /* 只有置为1才使用红外调制 */
        uint8_t RxIEEN;  /* 接收中断使能 */
        uint8_t TxIEEN;  /* 发送中断使能 */
        UART_SInitTypeDef uartint;
    } Uart_INIT_Type;

#define USEIRMod 1        /* 使用调制红外 */
#define NoUSEIRMod 0      /* 不使用调制红外 */
#define Uart1_UsePB0PB1 1 /* 使用UART1的PB0PB1口 */
#define Uart1_UsePE3PE4 0 /* 使用UART1的PE3PE4口 */

/* 定义串口接收的最大长度 */
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

#define MAXSIZE 8 /* 最大缓存数量 */
#define MAXUART 6 /* 最大串口数量 */

    typedef struct
    {
        uint16_t len;
        uint16_t maxlen;
        uint8_t pData[]; /* 柔性数组,必须要C99以上 */
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

    /**************************函数指针定义**************************/
    typedef void (*gy_uartinit)(UARTx_Type *Uartx, Uart_INIT_Type *init);
    typedef void (*gy_uartsend)(UARTx_Type *UARTx, uint8_t *str, uint16_t len);
    typedef void (*rxdohandle)(uint8_t *buf, uint16_t len);
    /****************************函数声明****************************/
    void API_Uartx_Init(UARTx_Type *Uartx, Uart_INIT_Type *init);      /* 串口初始化 */
    void API_Uart_Send(UARTx_Type *UARTx, uint8_t *str, uint16_t len); /* 串口数据发送1 使用实际uart */
    void API_UART_SEND(uint8_t uartx, uint8_t *data, uint16_t len);    /* 串口数据发送2 使用串口号 */
    void API_Uartx_Op(bool op, uint8_t uartx);                         /* 串口操作0:关 1:开 */

    void API_UART_Rx_Queue_Get_Do(uint8_t uartx, rxdohandle rxdo); /* 从队列中获取对应串口号的数据，然后做函数指针里的事 */
    ComData *API_UART_Rx_Queue_Get(uint8_t uartx);                 /* 从队列中获取对应串口号的数据，需要记得myfree获取的数据 */
    void API_UART_Rx_Queue_Send(uint8_t uartx);                    /* 串口数据接收完成后将对应串口号的数据发送到队列中 */
    /****************************串口队列****************************/
    uint8_t QueueIsFull(ComQueue *queueHead);                          /* 队列满 */
    uint8_t QueueIsEmpty(ComQueue *queueHead);                         /* 队列空 */
    ComQueue *InitComMessage(void);                                    /* 建立并初始化队列 */
    void SendComMessage(ComQueue *queueHead, ComData *_dat);           /* 像队列发送消息 */
    ComData *GetComMessage(ComQueue *queueHead);                       /* 从队列获取消息 */
    void __DelComMessage(ComQueue *queue);                             /* 删除队列 */
    void UartTxInterupt(uint8_t uartx);                                /* 串口中断发调用 */
    void UartRxInterupt(uint8_t data, uint8_t uartx, uint16_t maxlen); /* 串口中断收调用 */
    ComData *GetComInfo(uint8_t uartx);                                /* 直接获取串口收中断中的数据 */
    void FreeComInfo(uint8_t uartx);                                   /* 使用GetComInfo后调用 */
    
#define DelComMessage(x)    \
    {                       \
        __DelComMessage(x); \
        x = NULL;           \
    }

#ifdef __cplusplus
}
#endif

#endif
