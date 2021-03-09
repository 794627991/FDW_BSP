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
*		V1.0.0  2021-1-5    ����    ��ʽ����
*		V1.0.1  2021-3-9    ����    ����մճ��ʽ�����ӿ���ֲ��
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "myprintf.h"

#if Uart_Output > 0

#if !defined(__MICROLIB)
#if defined(__CC_ARM)
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
#if Use_Uartx_Send < 6
    uint32_t timeout = 0;
    while ((PRINTF_UART(Use_Uartx_Send)->TXBUFSTA & 0X00000001) == 1)
    {
        timeout++;
        if (timeout > 0xffff)
            return 0;
    }
    PRINTF_UART(Use_Uartx_Send)->TXREG = (uint8_t)ch;
#endif
    return ch;
}
#endif
