/*
*********************************************************************************************************
*
*	模块名称 : 定时器串口超时接收
*	文件名称 : bsp_uart_ex.c
*	版    本 : V1.0.0
*	说    明 : 定时器串口超时接收的处理
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-1-6    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "bsp_uart_ex.h"

#if Use_UartTimeOut > 0
static TimStruct vTim; /* 串口超时接收结构体 */

/*
*********************************************************************************************************
*	函 数 名: API_TimSet
*	功能说明: 串口超时接收配置
*	形    参: xUart：串口x使用超时功能
*             num：要超时接收的时间，单位ms
*	返 回 值: 无
*	备    注：关于串口超时接收机制的说明：
*            以9600波特率为例，1字节由起始位，数据和停止位组成，每一位时间为104us
*            则一字节的时间大约为1ms多。因此，假定串口收到第一个数据时当作一帧的数据头
*            若连续两个字节间隔时间不超过'num'(ms)则认为是同一帧，如果超过'num'(ms)则认为当前帧已经接收完成
*            每次接收到串口数据都要将num进行重新配置，保证后续数据接收，直到超时为止
*            因此，该函数应该放在串口接收中断的位置，详情可查看interrupt_api.c
*********************************************************************************************************
*/
void API_TimSet(uint8_t xUart, uint8_t num)
{
    if (num < 2)
        num = 2;

    if (vTim.TimFlag == 0)
    {
        ETIM_INIT_Type ETIM_init;

        ETIM_init.Nvicpry = 2;          /* 中断优先级选择  越小优先级越高  不能大于3 */
        ETIM_init.PRE = 100;            /* 1-255  以主频分频  1为1分频即不分频 */
        ETIM_init.count = 80 * clkmode; /* 计数值  以主频8M,100分频,80/80000=1ms */

        API_Etim_Init(UseTim, &ETIM_init);
    }

#if TmOut_Uart0 > 0
    if (xUart == UartTim0)
    {
        vTim.TimFlag |= UartTim0;
        vTim.TimNum0 = num;
    }
#endif
#if TmOut_Uart1 > 0
    if (xUart == UartTim1)
    {
        vTim.TimFlag |= UartTim1;
        vTim.TimNum1 = num;
    }
#endif
#if TmOut_Uart2 > 0
    if (xUart == UartTim2)
    {
        vTim.TimFlag |= UartTim2;
        vTim.TimNum2 = num;
    }
#endif
#if TmOut_Uart3 > 0
    if (xUart == UartTim3)
    {
        vTim.TimFlag |= UartTim3;
        vTim.TimNum3 = num;
    }
#endif
#if TmOut_Uart4 > 0
    if (xUart == UartTim4)
    {
        vTim.TimFlag |= UartTim4;
        vTim.TimNum4 = num;
    }
#endif
#if TmOut_Uart5 > 0
    if (xUart == UartTim5)
    {
        vTim.TimFlag |= UartTim5;
        vTim.TimNum5 = num;
    }
#endif
    API_ETim_ReSet(UseTim);
}
/*
*********************************************************************************************************
*	函 数 名: UartTimeOut
*	功能说明: 串口超时接收完成
*	形    参: 无
*	返 回 值: 无
*	备    注：该函数放在定时器中断完成的地方，详情可查看interrupt_api.c
*********************************************************************************************************
*/
void UartTimeOut(void)
{
#if TmOut_Uart0 > 0 && UseUart0 > 0
    if (vTim.TimFlag0 == 1)
    {
        if (--vTim.TimNum0 == 0)
        {
            vTim.TimFlag0 = 0;
            Uart0TimCallBack();
        }
    }
#endif
#if TmOut_Uart1 > 0 && UseUart1 > 0
    if (vTim.TimFlag1 == 1)
    {
        if (--vTim.TimNum1 == 0)
        {
            vTim.TimFlag1 = 0;
            Uart1TimCallBack();
        }
    }
#endif
#if TmOut_Uart2 > 0 && UseUart2 > 0
    if (vTim.TimFlag2 == 1)
    {
        if (--vTim.TimNum2 == 0)
        {
            vTim.TimFlag2 = 0;
            Uart2TimCallBack();
        }
    }
#endif
#if TmOut_Uart3 > 0 && UseUart3 > 0
    if (vTim.TimFlag3 == 1)
    {
        if (--vTim.TimNum3 == 0)
        {
            vTim.TimFlag3 = 0;
            Uart3TimCallBack();
        }
    }
#endif
#if TmOut_Uart4 > 0 && UseUart4 > 0
    if (vTim.TimFlag4 == 1)
    {
        if (--vTim.TimNum4 == 0)
        {
            vTim.TimFlag4 = 0;
            Uart4TimCallBack();
        }
    }
#endif
#if TmOut_Uart5 > 0 && UseUart5 > 0
    if (vTim.TimFlag5 == 1)
    {
        if (--vTim.TimNum5 == 0)
        {
            vTim.TimFlag5 = 0;
            Uart5TimCallBack();
        }
    }
#endif
    if (vTim.TimFlag == 0)
    {
        ETIMx_ETxCR_CEN_Setable(UseTim, DISABLE);
    }
}
/*
*********************************************************************************************************
*	函 数 名: UartxTimCallBack
*	功能说明: 串口超时接收完成回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱函数，调用时自行编写，也可以直接使用下面的函数
*            如果使用系统，直接发送到串口任务，否则使用事件方式，直接发送UARTxRXSUCC事件
*********************************************************************************************************
*/
__weak void Uart0TimCallBack(void)
{
    API_UART_Rx_Queue_Send(0);
#if Use_uCOS > 0 && TmOut_Uart0 > 0 && UseUart0 > 0
    OS_ERR os_err;

    OSTaskQPost((OS_TCB *)&TaskUart0TCB,
                (void *)0,
                (OS_MSG_SIZE)0,
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&os_err);

    /*传统方式*/
    /*OSTaskQPost((OS_TCB *)&TaskUart0TCB,
                (void *)&U0Buf.RxBuf,
                (OS_MSG_SIZE)U0Buf.RxOpc,
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&os_err);
    U0Buf.RxOpc = 0;*/
#endif
#if Use_Event == 1
    SendMessage(UART0RXSUCC);
#endif
}

__weak void Uart1TimCallBack(void)
{
    API_UART_Rx_Queue_Send(1);
#if Use_uCOS > 0 && TmOut_Uart1 > 0 && UseUart1 > 0
    OS_ERR os_err;
    OSTaskQPost((OS_TCB *)&TaskUart1TCB,
                (void *)0,
                (OS_MSG_SIZE)0,
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&os_err);
#endif
#if Use_Event == 1
    SendMessage(UART1RXSUCC);
#endif
}

__weak void Uart2TimCallBack(void)
{
    API_UART_Rx_Queue_Send(2);
#if Use_uCOS > 0 && TmOut_Uart2 > 0 && UseUart2 > 0
    OS_ERR os_err;

    OSTaskQPost((OS_TCB *)&TaskUart2TCB,
                (void *)0,
                (OS_MSG_SIZE)0,
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&os_err);
#endif
#if Use_Event == 1
    SendMessage(UART2RXSUCC);
#endif
}

__weak void Uart3TimCallBack(void)
{
    API_UART_Rx_Queue_Send(3);
#if Use_uCOS > 0 && TmOut_Uart3 > 0 && UseUart3 > 0
    OS_ERR os_err;

    OSTaskQPost((OS_TCB *)&TaskUart3TCB,
                (void *)0,
                (OS_MSG_SIZE)0,
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&os_err);
#endif
#if Use_Event == 1
    SendMessage(UART3RXSUCC);
#endif
}

__weak void Uart4TimCallBack(void)
{
    API_UART_Rx_Queue_Send(4);
#if Use_uCOS > 0 && TmOut_Uart4 > 0 && UseUart4 > 0
    OS_ERR os_err;

    OSTaskQPost((OS_TCB *)&TaskUart4TCB,
                (void *)0,
                (OS_MSG_SIZE)0,
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&os_err);
#endif
#if Use_Event == 1
    SendMessage(UART4RXSUCC);
#endif
}

__weak void Uart5TimCallBack(void)
{
    API_UART_Rx_Queue_Send(5);
#if Use_uCOS > 0 && TmOut_Uart5 > 0 && UseUart5 > 0
    OS_ERR os_err;

    OSTaskQPost((OS_TCB *)&TaskUart5TCB,
                (void *)0,
                (OS_MSG_SIZE)0,
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&os_err);
#endif
#if Use_Event == 1
    SendMessage(UART5RXSUCC);
#endif
}

#if Use_Event == 1
/*
*********************************************************************************************************
*	函 数 名: UARTxRXSUCC
*	功能说明: 使用事件时，由事件读取串口收到的数据，并由函数指针调用API_UARTx_DO
*	形    参: 无
*	返 回 值: 无
*	备    注：弱函数，可以由用户自行编写，也可以直接使用下面的函数
*********************************************************************************************************
*/
__weak void UART0RXSUCC(void)
{
    API_UART_Rx_Queue_Get_Do(0, API_UART0_DO);
}

__weak void UART1RXSUCC(void)
{
    API_UART_Rx_Queue_Get_Do(1, API_UART1_DO);
}

__weak void UART2RXSUCC(void)
{
    API_UART_Rx_Queue_Get_Do(2, API_UART2_DO);
}

__weak void UART3RXSUCC(void)
{
    API_UART_Rx_Queue_Get_Do(3, API_UART3_DO);
}

__weak void UART4RXSUCC(void)
{
    API_UART_Rx_Queue_Get_Do(4, API_UART4_DO);
}

__weak void UART5RXSUCC(void)
{
    API_UART_Rx_Queue_Get_Do(5, API_UART5_DO);
}

/*
*********************************************************************************************************
*	函 数 名: API_UARTx_DO
*	功能说明: 需要自行编写的程序
*	形    参: 无
*	返 回 值: 无
*	备    注：串口数据接收完成，提供数据缓存和数据长度，自行编写即可
*********************************************************************************************************
*/
__weak void API_UART0_DO(uint8_t *buf, uint16_t len)
{
}

__weak void API_UART1_DO(uint8_t *buf, uint16_t len)
{
}

__weak void API_UART2_DO(uint8_t *buf, uint16_t len)
{
}

__weak void API_UART3_DO(uint8_t *buf, uint16_t len)
{
}

__weak void API_UART4_DO(uint8_t *buf, uint16_t len)
{
}

__weak void API_UART5_DO(uint8_t *buf, uint16_t len)
{
}
#endif

#endif
