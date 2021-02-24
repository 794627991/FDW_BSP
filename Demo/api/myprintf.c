/*
*********************************************************************************************************
*
*	ģ������ : �ض���
*	�ļ����� : myprintf.c
*	��    �� : V1.0.0
*	˵    �� : �ض���printf�����ļ���Ҫ�޸�
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
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

void _sys_exit(int x) //����ʹ�ð�����ģʽ
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
    } //�����޸Ĵ��ڴ�ӡ��Uart��
    UART0->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 1
    while ((UART1->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //�����޸Ĵ��ڴ�ӡ��Uart��
    UART1->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 2
    while ((UART2->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //�����޸Ĵ��ڴ�ӡ��Uart��
    UART2->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 3
    while ((UART3->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //�����޸Ĵ��ڴ�ӡ��Uart��
    UART3->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 4
    while ((UART4->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //�����޸Ĵ��ڴ�ӡ��Uart��
    UART4->TXREG = (uint8_t)ch;
#endif

#if Use_Uartx_Send == 5
    while ((UART5->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    } //�����޸Ĵ��ڴ�ӡ��Uart��
    UART5->TXREG = (uint8_t)ch;
#endif
    //TicksDelayUs(50);
    return ch;
}
#endif
