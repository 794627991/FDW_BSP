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

#if !defined(__MICROLIB)
#if defined ( __CC_ARM )
#pragma import(__use_no_semihosting)
#elif defined(__GNUC__)
__asm(".global __use_no_semihosting\n\t");
#endif

void _sys_exit(int x) //避免使用半主机模式
{
    x = x;
}
//__use_no_semihosting was requested, but _ttywrch was
void _ttywrch(int ch)
{
    ch = ch;
}
struct __FILE
{
    int handle;
};
FILE __stdout;

#endif

#if defined(__GNUC__) && !defined(__clang__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
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
