#include "boot_define_all.h"

uint32 Count32 = 0;

//定时时间4000us,
void ETIM2_IRQHandler(void)
{
    unsigned char Ch;

    ETIMx_ETxIF_OVIF_Clr(ETIM2); //ETIM2->ETxIF = 0x03;//清中断标志
    JumpCount++;
    Count32++;

    for (Ch = 0; Ch < 3; Ch++)
    {
        if (Uart[Ch].Overtime_Timer != 0)
        {
            Uart[Ch].Overtime_Timer--;
            if (Uart[Ch].Overtime_Timer == 0)
            {
                if (Ch == U_U0)
                {
                    UART_UARTIE_RxTxIE_SetableEx(U_U0_Uart, TxInt, DISABLE); //UART_common->UARTIE &= (~UART_TX_IE2);	//关闭TXIEx
                    UART_UARTIE_RxTxIE_SetableEx(U_U0_Uart, RxInt, ENABLE);	 //UART_common->UARTIE |= UART_RX_IE2;	//打开RXIEx
                }
#if UseHowMany > 1
                if (Ch == U_U1)
                {
                    UART_UARTIE_RxTxIE_SetableEx(U_U1_Uart, TxInt, DISABLE); //UART_common->UARTIE &= (~UART_TX_IE2);	//关闭TXIEx
                    UART_UARTIE_RxTxIE_SetableEx(U_U1_Uart, RxInt, ENABLE);	 //UART_common->UARTIE |= UART_RX_IE2;	//打开RXIEx
                }
#endif
#if UseHowMany > 2
                if (Ch == U_U2)
                {
                    UART_UARTIE_RxTxIE_SetableEx(U_U2_Uart, TxInt, DISABLE); //UART_common->UARTIE &= (~UART_TX_IE0);	//关闭TXIEx
                    UART_UARTIE_RxTxIE_SetableEx(U_U2_Uart, RxInt, ENABLE);	 //UART_common->UARTIE |= UART_RX_IE0;	//打开RXIEx
                }
#endif
#if UseHowMany > 3
                if (Ch == U_U3)
                {
                    UART_UARTIE_RxTxIE_SetableEx(U_U3_Uart, TxInt, DISABLE); //UART_common->UARTIE &= (~UART_TX_IE0);	//关闭TXIEx
                    UART_UARTIE_RxTxIE_SetableEx(U_U3_Uart, RxInt, ENABLE);	 //UART_common->UARTIE |= UART_RX_IE0;	//打开RXIEx
                }
#endif
#if UseHowMany > 4
                if (Ch == U_U4)
                {
                    UART_UARTIE_RxTxIE_SetableEx(U_U4_Uart, TxInt, DISABLE); //UART_common->UARTIE &= (~UART_TX_IE0);	//关闭TXIEx
                    UART_UARTIE_RxTxIE_SetableEx(U_U4_Uart, RxInt, ENABLE);	 //UART_common->UARTIE |= UART_RX_IE0;	//打开RXIEx
                }
#endif
#if UseHowMany > 5
                if (Ch == U_U5)
                {
                    UART_UARTIE_RxTxIE_SetableEx(U_U5_Uart, TxInt, DISABLE); //UART_common->UARTIE &= (~UART_TX_IE0);	//关闭TXIEx
                    UART_UARTIE_RxTxIE_SetableEx(U_U5_Uart, RxInt, ENABLE);	 //UART_common->UARTIE |= UART_RX_IE0;	//打开RXIEx
                }
#endif
                Uart[Ch].Step = 0x00;
                Uart[Ch].Flag = 0x00;
                Uart[Ch].Response_Timer = 0x00;

                Uart[Ch].Len = 0;
            }
        }
    }

    for (Ch = 0; Ch < 3; Ch++)
    {
        if (Uart[Ch].Response_Timer != 0)
        {
            Uart[Ch].Response_Timer--;
            if (Uart[Ch].Response_Timer == 0)
            {
                if (Uart[Ch].Flag & B1000_1000)
                {
                    if (Ch == U_U0) //允许中断发送数据
                    {
                        UARTx_TXREG_Write(U_U0_Uart, Uart[Ch].Buf[0]);			 //UART2->TXREG  = Uart[Ch].Buf[0];		//发送0xfe
                        UART_UARTIE_RxTxIE_SetableEx(U_U0_Uart, RxInt, DISABLE); //UART_common->UARTIE &= (~UART_RX_IE2);	//关闭RXIEx
                        UART_UARTIE_RxTxIE_SetableEx(U_U0_Uart, TxInt, ENABLE);	 //UART_common->UARTIE |= UART_TX_IE2;	//打开TXIEx
                    }
#if UseHowMany > 1
                    if (Ch == U_U1)
                    {
                        UARTx_TXREG_Write(U_U1_Uart, Uart[Ch].Buf[0]);			 //UART1->TXREG  = Uart[Ch].Buf[0];		//发送0xfe
                        UART_UARTIE_RxTxIE_SetableEx(U_U1_Uart, RxInt, DISABLE); //UART_common->UARTIE &= (~UART_RX_IE1);	//关闭RXIEx
                        UART_UARTIE_RxTxIE_SetableEx(U_U1_Uart, TxInt, ENABLE);	 //UART_common->UARTIE |= UART_TX_IE1;	//打开TXIEx
                    }
#endif
#if UseHowMany > 2
                    if (Ch == U_U2)
                    {
                        UARTx_TXREG_Write(U_U2_Uart, Uart[Ch].Buf[0]);			 //UART0->TXREG  = Uart[Ch].Buf[0];		//发送0xfe
                        UART_UARTIE_RxTxIE_SetableEx(U_U2_Uart, RxInt, DISABLE); //UART_common->UARTIE &= (~UART_RX_IE0);	//关闭RXIEx
                        UART_UARTIE_RxTxIE_SetableEx(U_U2_Uart, TxInt, ENABLE);	 //UART_common->UARTIE |= UART_TX_IE0;	//打开TXIEx
                    }
#endif
#if UseHowMany > 3
                    if (Ch == U_U3)
                    {
                        UARTx_TXREG_Write(U_U3_Uart, Uart[Ch].Buf[0]);			 //UART0->TXREG  = Uart[Ch].Buf[0];		//发送0xfe
                        UART_UARTIE_RxTxIE_SetableEx(U_U3_Uart, RxInt, DISABLE); //UART_common->UARTIE &= (~UART_RX_IE0);	//关闭RXIEx
                        UART_UARTIE_RxTxIE_SetableEx(U_U3_Uart, TxInt, ENABLE);	 //UART_common->UARTIE |= UART_TX_IE0;	//打开TXIEx
                    }
#endif
#if UseHowMany > 4
                    if (Ch == U_U4)
                    {
                        UARTx_TXREG_Write(U_U4_Uart, Uart[Ch].Buf[0]);			 //UART0->TXREG  = Uart[Ch].Buf[0];		//发送0xfe
                        UART_UARTIE_RxTxIE_SetableEx(U_U4_Uart, RxInt, DISABLE); //UART_common->UARTIE &= (~UART_RX_IE0);	//关闭RXIEx
                        UART_UARTIE_RxTxIE_SetableEx(U_U4_Uart, TxInt, ENABLE);	 //UART_common->UARTIE |= UART_TX_IE0;	//打开TXIEx
                    }
#endif
#if UseHowMany > 5
                    if (Ch == U_U5)
                    {
                        UARTx_TXREG_Write(U_U5_Uart, Uart[Ch].Buf[0]);			 //UART0->TXREG  = Uart[Ch].Buf[0];		//发送0xfe
                        UART_UARTIE_RxTxIE_SetableEx(U_U5_Uart, RxInt, DISABLE); //UART_common->UARTIE &= (~UART_RX_IE0);	//关闭RXIEx
                        UART_UARTIE_RxTxIE_SetableEx(U_U5_Uart, TxInt, ENABLE);	 //UART_common->UARTIE |= UART_TX_IE0;	//打开TXIEx
                    }
#endif
                    Uart[Ch].Len = 1;				//开始发送数据
                    if (Uart[Ch].Flag & B0000_1000) //645

                        Uart[Ch].Total_Len = Uart[Ch].Buf[1] * 256 + Uart[Ch].Buf[2] + 4 + 3;
                    Uart[Ch].Overtime_Timer = txtimeout_max;
                }
                else
                {
                    Uart[Ch].Response_Timer++; //继续等待
                }
            }
        }
    }
}

void ClearClosedIF(UARTx_Type *UARTx)
{
    if ((DISABLE == UART_UARTIE_RxTxIE_GetableEx(UARTx, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UARTx, RxInt)))
    {
        UARTx_RXREG_Read(UARTx);
    }
    if ((DISABLE == UART_UARTIE_RxTxIE_GetableEx(UARTx, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UARTx, TxInt)))
    {
        UART_UARTIF_RxTxIF_ClrEx(UARTx);
    }
}

#ifdef IRQ_U0
void UART0_IRQHandler(void)
{
    uint08 Temp;

    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART0, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART0, RxInt))) //Receive
    {
        Temp = UARTx_RXREG_Read(UART0); //UART0->RXREG; //从UARTX数据寄存器中取接收数据
        Judge_RX_Frame(IRQ_U0, Temp);
    }

    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART0, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART0, TxInt))) //Transmit
    {
        Tx_Frame(IRQ_U0);
        UART_UARTIF_RxTxIF_ClrEx(UART0); //UART_common->UARTIF = UART_TX_IF0;	//clr UARTX.txif
    }

    ClearClosedIF(UART0); //UART_common->UARTIF &= ((~UART_common->UARTIE) &(UART_TX_IF0|UART_RX_IF0));
}
#endif

#ifdef IRQ_U1
void UART1_IRQHandler(void)
{
    uint08 Temp08;

    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART1, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART1, RxInt))) //Receive
    {
        Temp08 = UARTx_RXREG_Read(UART1); //UART1->RXREG; //从UARTX数据寄存器中取接收数据
        Judge_RX_Frame(IRQ_U1, Temp08);
    }
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART1, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART1, TxInt))) //Transmit
    {
        Tx_Frame(IRQ_U1);
        UART_UARTIF_RxTxIF_ClrEx(UART1); //UART_common->UARTIF = UART_TX_IF2;	//clr UARTX.txif
    }

    ClearClosedIF(UART1); //UART_common->UARTIF &= ((~UART_common->UARTIE) &(UART_TX_IF2|UART_RX_IF2));
}
#endif

#ifdef IRQ_U2
void UART2_IRQHandler(void)
{
    uint08 Temp08;

    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART2, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART2, RxInt))) //Receive
    {
        Temp08 = UARTx_RXREG_Read(UART2); //UART1->RXREG; //从UARTX数据寄存器中取接收数据
        Judge_RX_Frame(IRQ_U2, Temp08);
    }
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART2, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART2, TxInt))) //Transmit
    {
        Tx_Frame(IRQ_U2);
        UART_UARTIF_RxTxIF_ClrEx(UART2); //UART_common->UARTIF = UART_TX_IF2;	//clr UARTX.txif
    }

    ClearClosedIF(UART2); //UART_common->UARTIF &= ((~UART_common->UARTIE) &(UART_TX_IF2|UART_RX_IF2));
}
#endif

#ifdef IRQ_U3
void UART3_IRQHandler(void)
{
    uint08 Temp08;

    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART3, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART3, RxInt))) //Receive
    {
        Temp08 = UARTx_RXREG_Read(UART3); //UART1->RXREG; //从UARTX数据寄存器中取接收数据
        Judge_RX_Frame(IRQ_U3, Temp08);
    }
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART3, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART3, TxInt))) //Transmit
    {
        Tx_Frame(IRQ_U3);
        UART_UARTIF_RxTxIF_ClrEx(UART3); //UART_common->UARTIF = UART_TX_IF2;	//clr UARTX.txif
    }

    ClearClosedIF(UART3); //UART_common->UARTIF &= ((~UART_common->UARTIE) &(UART_TX_IF2|UART_RX_IF2));
}
#endif

#ifdef IRQ_U4
void UART4_IRQHandler(void)
{
    uint08 Temp08;

    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART4, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART4, RxInt))) //Receive
    {
        Temp08 = UARTx_RXREG_Read(UART4); //UART1->RXREG; //从UARTX数据寄存器中取接收数据
        Judge_RX_Frame(IRQ_U4, Temp08);
    }
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART4, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART4, TxInt))) //Transmit
    {
        Tx_Frame(IRQ_U4);
        UART_UARTIF_RxTxIF_ClrEx(UART4); //UART_common->UARTIF = UART_TX_IF2;	//clr UARTX.txif
    }

    ClearClosedIF(UART4); //UART_common->UARTIF &= ((~UART_common->UARTIE) &(UART_TX_IF2|UART_RX_IF2));
}
#endif

#ifdef IRQ_U5
void UART5_IRQHandler(void)
{
    uint08 Temp08;

    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART5, RxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART5, RxInt))) //Receive
    {
        Temp08 = UARTx_RXREG_Read(UART5); //UART1->RXREG; //从UARTX数据寄存器中取接收数据
        Judge_RX_Frame(IRQ_U5, Temp08);
    }
    if ((ENABLE == UART_UARTIE_RxTxIE_GetableEx(UART5, TxInt)) && (SET == UART_UARTIF_RxTxIF_ChkEx(UART5, TxInt))) //Transmit
    {
        Tx_Frame(IRQ_U5);
        UART_UARTIF_RxTxIF_ClrEx(UART5); //UART_common->UARTIF = UART_TX_IF2;	//clr UARTX.txif
    }

    ClearClosedIF(UART5); //UART_common->UARTIF &= ((~UART_common->UARTIE) &(UART_TX_IF2|UART_RX_IF2));
}
#endif
