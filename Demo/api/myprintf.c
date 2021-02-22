/*
*********************************************************************************************************
*
*	模块名称 : 重定义
*	文件名称 : myprintf.c
*	版    本 : V1.0.0
*	说    明 : 重定义printf，该文件不要修改
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "myprintf.h"

#if Uart_Output > 0
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};
FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}

void _ttywrch(int ch) {}

int fputc(int ch, FILE *f)
{
    uint32_t timeout = 0;
#if Use_Uartx_Send == 0
    while ((UART0->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //自行修改串口打印的Uart口
    UART0->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 1
    while ((UART1->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //自行修改串口打印的Uart口
    UART1->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 2
    while ((UART2->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //自行修改串口打印的Uart口
    UART2->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 3
    while ((UART3->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //自行修改串口打印的Uart口
    UART3->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 4
    while ((UART4->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //自行修改串口打印的Uart口
    UART4->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 5
    while ((UART5->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //自行修改串口打印的Uart口
    UART5->TXREG = (uint8_t)ch;
#endif
    //TicksDelayUs(50);
    return ch;
}
#endif
