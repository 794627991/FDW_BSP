/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ�����ڳ�ʱ����
*	�ļ����� : bsp_uart_ex.c
*	��    �� : V1.0.0
*	˵    �� : ��ʱ�����ڳ�ʱ���յĴ���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-1-6    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "bsp_uart_ex.h"

#if Use_UartTimeOut > 0
static TimStruct vTim; /* ���ڳ�ʱ���սṹ�� */

/*
*********************************************************************************************************
*	�� �� ��: API_TimSet
*	����˵��: ���ڳ�ʱ��������
*	��    ��: xUart������xʹ�ó�ʱ����
*             num��Ҫ��ʱ���յ�ʱ�䣬��λms
*	�� �� ֵ: ��
*	��    ע�����ڴ��ڳ�ʱ���ջ��Ƶ�˵����
*            ��9600������Ϊ����1�ֽ�����ʼλ�����ݺ�ֹͣλ��ɣ�ÿһλʱ��Ϊ104us
*            ��һ�ֽڵ�ʱ���ԼΪ1ms�ࡣ��ˣ��ٶ������յ���һ������ʱ����һ֡������ͷ
*            �����������ֽڼ��ʱ�䲻����'num'(ms)����Ϊ��ͬһ֡���������'num'(ms)����Ϊ��ǰ֡�Ѿ��������
*            ÿ�ν��յ��������ݶ�Ҫ��num�����������ã���֤�������ݽ��գ�ֱ����ʱΪֹ
*            ��ˣ��ú���Ӧ�÷��ڴ��ڽ����жϵ�λ�ã�����ɲ鿴interrupt_api.c
*********************************************************************************************************
*/
void API_TimSet(uint8_t xUart, uint8_t num)
{
    if (num < 2)
        num = 2;

    if (vTim.TimFlag == 0)
    {
        ETIM_INIT_Type ETIM_init;

        ETIM_init.Nvicpry = 2;          /* �ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3 */
        ETIM_init.PRE = 100;            /* 1-255  ����Ƶ��Ƶ  1Ϊ1��Ƶ������Ƶ */
        ETIM_init.count = 80 * clkmode; /* ����ֵ  ����Ƶ8M,100��Ƶ,80/80000=1ms */

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
*	�� �� ��: UartTimeOut
*	����˵��: ���ڳ�ʱ�������
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע���ú������ڶ�ʱ���ж���ɵĵط�������ɲ鿴interrupt_api.c
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
*	�� �� ��: UartxTimCallBack
*	����˵��: ���ڳ�ʱ������ɻص�
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��������������ʱ���б�д��Ҳ����ֱ��ʹ������ĺ���
*            ���ʹ��ϵͳ��ֱ�ӷ��͵��������񣬷���ʹ���¼���ʽ��ֱ�ӷ���UARTxRXSUCC�¼�
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

    /*��ͳ��ʽ*/
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
*	�� �� ��: UARTxRXSUCC
*	����˵��: ʹ���¼�ʱ�����¼���ȡ�����յ������ݣ����ɺ���ָ�����API_UARTx_DO
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע�����������������û����б�д��Ҳ����ֱ��ʹ������ĺ���
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
*	�� �� ��: API_UARTx_DO
*	����˵��: ��Ҫ���б�д�ĳ���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע���������ݽ�����ɣ��ṩ���ݻ�������ݳ��ȣ����б�д����
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
