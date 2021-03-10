/*
*********************************************************************************************************
*
*	模块名称 : 中断处理及回调程序
*	文件名称 : bsp_interrupt.c
*	版    本 : V1.0.0
*	说    明 : 中断相关
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "bsp_interrupt.h"
/*
*********************************************************************************************************
*	函 数 名: GPIO_IRQ_CallBack
*	功能说明: IO口中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果没使用系统会进入该回调
*********************************************************************************************************
*/
__weak void GPIO_IRQ_CallBack(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: ETIM1_IRQ_CallBack
*	功能说明: 定时器1中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果开串口超时功能且用该定时器，不会进该回调
*********************************************************************************************************
*/
__weak void ETIM1_IRQ_CallBack(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: ETIM2_IRQ_CallBack
*	功能说明: 定时器2中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果开串口超时功能且用该定时器，不会进该回调
*********************************************************************************************************
*/
__weak void ETIM2_IRQ_CallBack(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: ETIM3_IRQ_CallBack
*	功能说明: 定时器3中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果开串口超时功能且用该定时器，不会进该回调
*********************************************************************************************************
*/
__weak void ETIM3_IRQ_CallBack(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: ETIM4_IRQ_CallBack
*	功能说明: 定时器4中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果开串口超时功能且用该定时器，不会进该回调
*********************************************************************************************************
*/
__weak void ETIM4_IRQ_CallBack(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: UART0_IRQ_Rx_CallBack
*	功能说明: 串口0中断回调
*	形    参: data：该次中断接收到的数据
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果没开串口超时功能，会进此回调函数
*********************************************************************************************************
*/
__weak void UART0_IRQ_Rx_CallBack(uint8_t data)
{
}
/*
*********************************************************************************************************
*	函 数 名: UART1_IRQ_Rx_CallBack
*	功能说明: 串口1中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果没开串口超时功能，会进此回调函数
*********************************************************************************************************
*/
__weak void UART1_IRQ_Rx_CallBack(uint8_t data)
{
}
/*
*********************************************************************************************************
*	函 数 名: UART2_IRQ_Rx_CallBack
*	功能说明: 串口2中断回调
*	形    参: data：该次中断接收到的数据
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果没开串口超时功能，会进此回调函数
*********************************************************************************************************
*/
__weak void UART2_IRQ_Rx_CallBack(uint8_t data)
{
}
/*
*********************************************************************************************************
*	函 数 名: UART3_IRQ_Rx_CallBack
*	功能说明: 串口3中断回调
*	形    参: data：该次中断接收到的数据
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果没开串口超时功能，会进此回调函数
*********************************************************************************************************
*/
__weak void UART3_IRQ_Rx_CallBack(uint8_t data)
{
}
/*
*********************************************************************************************************
*	函 数 名: UART4_IRQ_Rx_CallBack
*	功能说明: 串口4中断回调
*	形    参: data：该次中断接收到的数据
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果没开串口超时功能，会进此回调函数
*********************************************************************************************************
*/
__weak void UART4_IRQ_Rx_CallBack(uint8_t data)
{
}
/*
*********************************************************************************************************
*	函 数 名: UART5_IRQ_Rx_CallBack
*	功能说明: 串口5中断回调
*	形    参: data：该次中断接收到的数据
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，如果没开串口超时功能，会进此回调函数
*********************************************************************************************************
*/
__weak void UART5_IRQ_Rx_CallBack(uint8_t data)
{
    /* 以下仅供参考
    #define Txlen 1
    #define Rxlen 1
	typedef struct
    {
        uint8_t TxBuf[Txlen]; //发送数据缓存
        uint8_t RxBuf[Rxlen]; //接收数据缓存
        unsigned short RxOpc; //已接收数据长度
    } UART5Struct;
	UART5Struct U5Buf;

	if (U5Buf.RxOpc > (U5Rxlen - 1))
    {
        U5Buf.RxOpc = 0;
    }
    U5Buf.RxBuf[U5Buf.RxOpc] = data;
    U5Buf.RxOpc++;
    */
}
/*
*********************************************************************************************************
*	函 数 名: LPUART_IRQ_Rx_CallBack
*	功能说明: 低功耗串口中断回调
*	形    参: data：该次中断接收到的数据
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，开了低功耗串口中断就会进来
*********************************************************************************************************
*/
__weak void LPUART_IRQ_Rx_CallBack(uint8_t data)
{
}
/*
*********************************************************************************************************
*	函 数 名: LPTIM_IRQ_CallBack
*	功能说明: 低功耗定时器中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写，开了低功耗定时器中断就会进来
*********************************************************************************************************
*/
__weak void LPTIM_IRQ_CallBack(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: RTC_IRQ_SEC_CallBack
*	功能说明: RTC秒中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写
*********************************************************************************************************
*/
__weak void RTC_IRQ_SEC_CallBack(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: RTC_IRQ_MIN_CallBack
*	功能说明: RTC分中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写
*********************************************************************************************************
*/
__weak void RTC_IRQ_MIN_CallBack(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: RTC_IRQ_HOUR_CallBack
*	功能说明: RTC小时中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写
*********************************************************************************************************
*/
__weak void RTC_IRQ_HOUR_CallBack(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: RTC_IRQ_DATE_CallBack
*	功能说明: RTC天中断回调
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，调用时自己写
*********************************************************************************************************
*/
__weak void RTC_IRQ_DATE_CallBack(void)
{
}

/***********************************************************************
//-------------------以下为部分中断处理，不需要额外处理------------------//
***********************************************************************/
#if ThisBoot == 0

void GPIO_IRQHandler(void)
{
    /* 全部IO口中断在此处理 */
#if Use_uCOS > 0
    OS_ERR os_err;
    os_err = os_err;
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

#if Use_uCOS > 0
    uCOS_Get_IO()->EXTI0IF |= GPIO->EXTI0IF;
    uCOS_Get_IO()->EXTI1IF |= GPIO->EXTI1IF;
    uCOS_Get_IO()->EXTI2IF |= GPIO->EXTI2IF;
    GPIO->EXTI0IF = 0xff; /* 写1清标志 */
    GPIO->EXTI1IF = 0xff;
    GPIO->EXTI2IF = 0xff;
    OSTaskQPost((OS_TCB *)&TaskIOTCB,
                (void *)0,
                (OS_MSG_SIZE)0,
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&os_err);
#else
    GPIO_IRQ_CallBack();
#endif

#if Use_uCOS > 0
    OSIntExit();
#endif
}

#if API_UseADC > 0
void ADC_IRQHandler(void)
{
    /* ADC中断 */
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    ADC_IRQ_CallBack();

#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if UseUart0 > 0
void UART0_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    /* 接收中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART0, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART0, RxInt)))
    {
        uint8_t tmp08 = UARTx_RXREG_Read(UART0);
#if (Use_UartTimeOut > 0 && TmOut_Uart0 > 0)
        API_TimSet(UartTim0, 10); /* 10ms超时 */
        UartRxInterupt(tmp08, 0, U0Rxlen);
#else
        UART0_IRQ_Rx_CallBack(tmp08);
#endif
    }
    /* 发送中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART0, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART0, TxInt)))
    {
        UartTxInterupt(0);
    }
#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if UseUart1 > 0
void UART1_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    /* 接收中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART1, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART1, RxInt)))
    {
        uint8_t tmp08 = UARTx_RXREG_Read(UART1);
#if (Use_UartTimeOut > 0 && TmOut_Uart1 > 0)
        API_TimSet(UartTim1, 10); /* 10ms超时 */
        UartRxInterupt(tmp08, 0, U1Rxlen);
#else
        UART1_IRQ_Rx_CallBack(tmp08);
#endif
    }
    /* 发送中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART1, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART1, TxInt)))
    {
        UartTxInterupt(1);
    }
#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if UseUart2 > 0
void UART2_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    /* 接收中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART2, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART2, RxInt)))
    {
        uint8_t tmp08 = UARTx_RXREG_Read(UART2);
#if (Use_UartTimeOut > 0 && TmOut_Uart2 > 0)
        API_TimSet(UartTim2, 10); /* 10ms超时 */
        UartRxInterupt(tmp08, 2, U2Rxlen);
#else
        UART2_IRQ_Rx_CallBack(tmp08);
#endif
    }
    /* 发送中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART2, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART2, TxInt)))
    {
        UartTxInterupt(2);
    }
#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if UseUart3 > 0
void UART3_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    /* 接收中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART3, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART3, RxInt)))
    {
        uint8_t tmp08 = UARTx_RXREG_Read(UART3);
#if (Use_UartTimeOut > 0 && TmOut_Uart3 > 0)
        API_TimSet(UartTim3, 10); /* 10ms超时 */
        UartRxInterupt(tmp08, 3, U3Rxlen);
#else
        UART3_IRQ_Rx_CallBack(tmp08)
#endif
    }
    /* 发送中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART3, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART3, TxInt)))
    {
        UartTxInterupt(3);
    }
#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if UseUart4 > 0
void UART4_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    /* 接收中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART4, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART4, RxInt)))
    {
        uint8_t tmp08 = UARTx_RXREG_Read(UART4);
#if (Use_UartTimeOut > 0 && TmOut_Uart4 > 0)
        API_TimSet(UartTim4, 10); /* 10ms超时 */
        UartRxInterupt(tmp08, 4, U4Rxlen);
#else
        UART4_IRQ_Rx_CallBack(tmp08);
#endif
    }
    /* 发送中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART4, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART4, TxInt)))
    {
        UartTxInterupt(4);
    }
#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if UseUart5 > 0
void UART5_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    /* 接收中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART5, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART5, RxInt)))
    {
        uint8_t tmp08 = UARTx_RXREG_Read(UART5);
#if (Use_UartTimeOut > 0 && TmOut_Uart5 > 0)
        API_TimSet(UartTim5, 10); /* 10ms超时 */
        UartRxInterupt(tmp08, 5, U5Rxlen);
#else
        UART5_IRQ_Rx_CallBack(tmp08);
#endif
    }
    /* 发送中断处理 */
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART5, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART5, TxInt)))
    {
        UartTxInterupt(5);
    }
#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if API_UseLPUart > 0

void LPUART_IRQHandler(void)
{
    /* 低功耗串口不具备超时功能 */
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif
    //接收中断处理
    if ((ENABLE == LPUART_LPUCON_RXIE_Getable()) && (SET == LPUART_LPUIF_RXIF_Chk())) //接收数据中断
    {
        uint8_t tmp08 = LPUART_LPURXD_Read();
        LPUART_IRQ_Rx_CallBack(tmp08);
        LPUART_LPUIF_RXIF_Clr();
    }
#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if ((UseETIM1 > 0 || (Use_UartTimeOut > 0 && UseWhichTim == 1)) && (Use_uCOS == 0 || (Use_uCOS > 0 && Use32BitEtim == 0)))
void ETIM1_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    if (SET == ETIMx_ETxIF_OVIF_Chk(ETIM1))
    {
        ETIMx_ETxIF_OVIF_Clr(ETIM1);
#if Use_UartTimeOut > 0 && UseWhichTim == 1
        UartTimeOut();
#else
        ETIM1_IRQ_CallBack();
#endif
    }

#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if ((UseETIM2 > 0 || (Use_UartTimeOut > 0 && UseWhichTim == 2)) && (Use_uCOS == 0 || (Use_uCOS > 0 && Use32BitEtim == 0)))
void ETIM2_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    if (SET == ETIMx_ETxIF_OVIF_Chk(ETIM2))
    {
        ETIMx_ETxIF_OVIF_Clr(ETIM2);
#if Use_UartTimeOut > 0 && UseWhichTim == 2
        UartTimeOut();
#else
        ETIM2_IRQ_CallBack();
#endif
    }

#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if (UseETIM3 > 0 || (Use_UartTimeOut > 0 && UseWhichTim == 3))
void ETIM3_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    if (SET == ETIMx_ETxIF_OVIF_Chk(ETIM3))
    {
        ETIMx_ETxIF_OVIF_Clr(ETIM3);
#if Use_UartTimeOut > 0 && UseWhichTim == 3
        UartTimeOut();
#else
        ETIM3_IRQ_CallBack();
#endif
    }

#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if ((UseETIM4 > 0 || (Use_UartTimeOut > 0 && UseWhichTim == 4)) && (Use_uCOS == 0 || (Use_uCOS > 0 && Use32BitEtim > 0)))
void ETIM4_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif
    if (SET == ETIMx_ETxIF_OVIF_Chk(ETIM4))
    {
        ETIMx_ETxIF_OVIF_Clr(ETIM4);
#if Use_UartTimeOut > 0 && UseWhichTim == 4
        UartTimeOut();
#else
        ETIM4_IRQ_CallBack();
#endif
    }
#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if API_UseLPTIM > 0
void LPTIM_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif

    if ((LPTIM_LPTIE_OVIE_Getable() == ENABLE) && (LPTIM_LPTIF_OVIF_Chk() == SET))
    {
        LPTIM_LPTIF_OVIF_Clr();
#if Use_uCOS > 0
        uCOS_CLKType *clk = uCOS_Get_Clk();
        clk->lptim = 1;
        OSTaskQPost((OS_TCB *)&TaskClockTCB,
                    (void *)clk,
                    (OS_MSG_SIZE)sizeof(uCOS_CLKType),
                    (OS_OPT)OS_OPT_POST_FIFO,
                    (OS_ERR *)&os_err);
#else
        LPTIM_IRQ_CallBack();
#endif
    }
    else
    {
        LPTIM_LPTIF_OVIF_Clr();
    }

#if Use_uCOS > 0
    OSIntExit();
#endif
}
#endif

#if API_UseRTC > 0
void RTC_IRQHandler(void)
{
#if Use_uCOS > 0
    OS_ERR os_err;
    CPU_SR_ALLOC();
    os_err = os_err;
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
    uCOS_CLKType *clk = uCOS_Get_Clk();
#endif
    /* 查询秒钟断标志是否置起 */
    if ((SET == RTC_RTCIF_ChkEx(RTC_RTCIF_SEC_IF_Msk)) && (ENABLE == RTC_RTCIE_GetableEx(RTC_RTCIE_SEC_IE_Msk)))
    {
        RTC_RTCIF_ClrEx(RTC_RTCIE_SEC_IE_Msk); /* 清除秒中断标志 */
#if Use_uCOS > 0
        clk->rtc_sec = 1;
#else
        RTC_IRQ_SEC_CallBack();
#endif
    }
    /* 查询分钟断标志是否置起 */
    if ((SET == RTC_RTCIF_ChkEx(RTC_RTCIF_MIN_IF_Msk)) && (ENABLE == RTC_RTCIE_GetableEx(RTC_RTCIE_MIN_IE_Msk)))
    {
        RTC_RTCIF_ClrEx(RTC_RTCIE_MIN_IE_Msk); /* 清除分中断标志 */
#if Use_uCOS > 0
        clk->rtc_min = 1;
#else
        RTC_IRQ_MIN_CallBack();
#endif
    }
    /* 查询时钟断标志是否置起 */
    if ((SET == RTC_RTCIF_ChkEx(RTC_RTCIF_HOUR_IF_Msk)) && (ENABLE == RTC_RTCIE_GetableEx(RTC_RTCIE_HOUR_IE_Msk)))
    {
        RTC_RTCIF_ClrEx(RTC_RTCIE_HOUR_IE_Msk); /* 清除时中断标志 */
#if Use_uCOS > 0
        clk->rtc_hour = 1;
#else
        RTC_IRQ_HOUR_CallBack();
#endif
    }
    if ((SET == RTC_RTCIF_ChkEx(RTC_RTCIF_DATE_IF_Msk)) && (ENABLE == RTC_RTCIE_GetableEx(RTC_RTCIE_DATE_IE_Msk))) //查询时钟断标志是否置起
    {
        RTC_RTCIF_ClrEx(RTC_RTCIE_DATE_IE_Msk); /* 清除天中断标志 */
#if Use_uCOS > 0
        clk->rtc_day = 1;
#else
        RTC_IRQ_DATE_CallBack();
#endif
    }

#if Use_uCOS > 0
    if (clk->up != 0)
    {
        OSTaskQPost((OS_TCB *)&TaskClockTCB,
                    (void *)clk,
                    (OS_MSG_SIZE)sizeof(uCOS_CLKType),
                    (OS_OPT)OS_OPT_POST_FIFO,
                    (OS_ERR *)&os_err);
    }

    OSIntExit();
#endif
}
#endif

#endif
