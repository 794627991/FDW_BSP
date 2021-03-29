#include "boot_define_all.h"
/*  
//  ʹ��˵����
//	1.�޸� UseHowMany ��ֵ���õ��������ھ͵��ڼ�
//	2.����ʵ�����ӳ�� U_U0_U-U_U6_U��ֵ
//	3.�� main������ʵ��ʹ�õĴ��ڼ�������
*/

uint08 BootEnable;
uint16 JumpCount;
uint32 FlashOpID;

//ET2, 5ms��ʱ���ж�
void Init_Etim2(void)
{
    ETIM_INIT_Type ETIM_init;

    ETIM_init.Nvicpry = 2; //�ж����ȼ�ѡ��  ԽС���ȼ�Խ��  ���ܴ���3
    ETIM_init.PRE = 100;   //1-255;   ����Ƶ��Ƶ  1Ϊ1��Ƶ������Ƶ
    ETIM_init.count = 400; //����ֵ   ����Ƶ8M,100��Ƶ,400/80000=5ms

    API_Etim_Init(ETIM2, &ETIM_init);

    ETIMx_ETxCR_CEN_Setable(ETIM2, ENABLE); //������ʱ��
}

void DeinitSystem(void)
{
    __disable_irq(); //�ر�ȫ���ж�ʹ��

    NVIC_DisableIRQ(ETIM2_IRQn);
    ETIMx_ETxCR_CEN_Setable(ETIM2, DISABLE); //�رն�ʱ��
    RCC_PERCLK_SetableEx(ET2CLK, DISABLE);

    // UART0
#ifdef IRQ_U0
    CloseeIO(GPIOF, GPIO_Pin_3); //PF3 UART0 RX
    CloseeIO(GPIOF, GPIO_Pin_4); //PF4 UART0 TX
    NVIC_DisableIRQ(UART0_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART0, TxInt, DISABLE); //�رշ����ж�
    UART_UARTIE_RxTxIE_SetableEx(UART0, RxInt, DISABLE); //�رս����ж�
    UARTx_TXSTA_TXEN_Setable(UART0, DISABLE);            //�رշ���ʹ��
    UARTx_RXSTA_RXEN_Setable(UART0, DISABLE);            //�رս���ʹ��
#endif

    // UART1
#ifdef IRQ_U1
    CloseeIO(GPIOB, GPIO_Pin_0); //PB0 UART1 RX
    CloseeIO(GPIOB, GPIO_Pin_1); //PB1 UART1 TX
    CloseeIO(GPIOE, GPIO_Pin_3); //PE3 UART1 RX
    CloseeIO(GPIOE, GPIO_Pin_4); //PE4 UART1 TX
    NVIC_DisableIRQ(UART1_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART1, TxInt, DISABLE); //�رշ����ж�
    UART_UARTIE_RxTxIE_SetableEx(UART1, RxInt, DISABLE); //�رս����ж�
    UARTx_TXSTA_TXEN_Setable(UART1, DISABLE);            //�رշ���ʹ��
    UARTx_RXSTA_RXEN_Setable(UART1, DISABLE);            //�رս���ʹ��
#endif

    // UART2
#ifdef IRQ_U2
    CloseeIO(GPIOB, GPIO_Pin_2); //PB2 UART2 RX
    CloseeIO(GPIOB, GPIO_Pin_3); //PB3 UART2 TX
    NVIC_DisableIRQ(UART2_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART2, TxInt, DISABLE); //�رշ����ж�
    UART_UARTIE_RxTxIE_SetableEx(UART2, RxInt, DISABLE); //�رս����ж�
    UARTx_TXSTA_TXEN_Setable(UART2, DISABLE);            //�رշ���ʹ��
    UARTx_RXSTA_RXEN_Setable(UART2, DISABLE);            //�رս���ʹ��
#endif

    // UART3
#ifdef IRQ_U3
    CloseeIO(GPIOC, GPIO_Pin_10); //PC10 UART3 RX
    CloseeIO(GPIOC, GPIO_Pin_11); //PC11 UART3 TX
    NVIC_DisableIRQ(UART3_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART3, TxInt, DISABLE); //�رշ����ж�
    UART_UARTIE_RxTxIE_SetableEx(UART3, RxInt, DISABLE); //�رս����ж�
    UARTx_TXSTA_TXEN_Setable(UART3, DISABLE);            //�رշ���ʹ��
    UARTx_RXSTA_RXEN_Setable(UART3, DISABLE);            //�رս���ʹ��
#endif

    // UART4
#ifdef IRQ_U4
    CloseeIO(GPIOD, GPIO_Pin_0); //PD0 UART4 RX
    CloseeIO(GPIOD, GPIO_Pin_1); //PD1 UART4 TX
    NVIC_DisableIRQ(UART4_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART4, TxInt, DISABLE); //�رշ����ж�
    UART_UARTIE_RxTxIE_SetableEx(UART4, RxInt, DISABLE); //�رս����ж�
    UARTx_TXSTA_TXEN_Setable(UART4, DISABLE);            //�رշ���ʹ��
    UARTx_RXSTA_RXEN_Setable(UART4, DISABLE);            //�رս���ʹ��
#endif

    // UART5
#ifdef IRQ_U5
    CloseeIO(GPIOC, GPIO_Pin_4); //PC4 UART5 RX
    CloseeIO(GPIOC, GPIO_Pin_5); //PC5 UART5 TX
    NVIC_DisableIRQ(UART5_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART5, TxInt, DISABLE); //�رշ����ж�
    UART_UARTIE_RxTxIE_SetableEx(UART5, RxInt, DISABLE); //�رս����ж�
    UARTx_TXSTA_TXEN_Setable(UART5, DISABLE);            //�رշ���ʹ��
    UARTx_RXSTA_RXEN_Setable(UART5, DISABLE);            //�رս���ʹ��
#endif
}

int main(void)
{
    uint32 JumpAddress;
    uint32 WrADR;

    Init_System(); //ϵͳ��ʼ��

#if UseUart0 > 0 && defined(IRQ_U0)
#if U0USEIRMod == 0
    Uart_Init_Macro(UART0, U0Baud, 0, 1, 0, OneBit);
#else
    Uart_Init_Macro(UART0, 2400, 1, 1, 0, OneBit);
#endif
#endif

#if UseUart1 > 0 && defined(IRQ_U1)
#if U1USEIRMod == 0
    Uart_Init_Macro(UART1, U1Baud, 0, 1, 0, OneBit);
#else
    Uart_Init_Macro(UART1, 2400, 1, 1, 0, OneBit);
#endif
#endif

#if UseUart2 > 0 && defined(IRQ_U2)
#if U2USEIRMod == 0
    Uart_Init_Macro(UART2, U2Baud, 0, 1, 0, OneBit);
#else
    Uart_Init_Macro(UART2, 2400, 1, 1, 0, OneBit);
#endif
#endif

#if UseUart3 > 0 && defined(IRQ_U3)
#if U3USEIRMod == 0
    Uart_Init_Macro(UART3, U3Baud, 0, 1, 0, OneBit);
#else
    Uart_Init_Macro(UART3, 2400, 1, 1, 0, OneBit);
#endif
#endif

#if UseUart4 > 0 && defined(IRQ_U4)
#if U4USEIRMod == 0
    Uart_Init_Macro(UART4, U4Baud, 0, 1, 0, OneBit);
#else
    Uart_Init_Macro(UART4, 2400, 1, 1, 0, OneBit);
#endif
#endif

#if UseUart5 > 0 && defined(IRQ_U5)
#if U5USEIRMod == 0
    Uart_Init_Macro(UART5, U5Baud, 0, 1, 0, OneBit);
#else
    Uart_Init_Macro(UART5, 2400, 1, 1, 0, OneBit);
#endif
#endif

    Init_Etim2();

    __enable_irq(); //��ȫ���ж�ʹ��
    /********************************************************************/

    BootEnable = 0;
    JumpCount = 0;
    //�洢��ee����flash�У�����flashǰ��ȡ��ֵ��FlashOpID,ƽʱ��Ƿ�ֵ����ֹ�����
    FlashOpID = 0; //FLASHOPKEY
    patchUpGrade();
    for (;;)
    {
        IWDT_Clr(); //��ϵͳ���Ź�

        if (Uart[U_U0].Flag & B0000_0100)
        {
            Uart_Proc(U_U0);
            IWDT_Clr();
        }

#if UseHowMany > 1
        if (Uart[U_U1].Flag & B0000_0100)
        {
            Uart_Proc(U_U1);
            IWDT_Clr();
        }
#endif

#if UseHowMany > 2
        if (Uart[U_U2].Flag & B0000_0100)
        {
            Uart_Proc(U_U2);
            IWDT_Clr();
        }
#endif

#if UseHowMany > 3
        if (Uart[U_U3].Flag & B0000_0100)
        {
            Uart_Proc(U_U3);
            IWDT_Clr();
        }
#endif

#if UseHowMany > 4
        if (Uart[U_U4].Flag & B0000_0100)
        {
            Uart_Proc(U_U4);
            IWDT_Clr();
        }
#endif

#if UseHowMany > 5
        if (Uart[U_U5].Flag & B0000_0100)
        {
            Uart_Proc(U_U5);
            IWDT_Clr();
        }
#endif

        if ((BootEnable == 0) && (JumpCount >= WAITTIME))
        {
            DeinitSystem(); //�ر�����

            FlashOpID = 0;
            /* ��ת��Ӧ�ó���λ�� */
            memcpy(&WrADR, (unsigned char *)FlashStartAdr, 4);
            if ((WrADR != 0) && (WrADR < FlashAllSize))
            {
                /* ˵�������APP��ʼ��ַ��Ч */
                JumpAddress = *(__IO uint32 *)(WrADR + 4);
                __set_MSP(*(__IO uint32 *)WrADR);
                (*(void (*)())JumpAddress)();
            }
            else
            {
                WrADR = BOOTENDADR; /* дһ��Ĭ�ϵ�APP1����ʼ��ַ */
                Flsah_Write_String(FlashStartAdr, (unsigned char *)&WrADR, 4, FLASHOPKEY);
                JumpAddress = *(__IO uint32 *)(APPLICATION_ADDRESS + 4);
                __set_MSP(*(__IO uint32 *)APPLICATION_ADDRESS);
                (*(void (*)())JumpAddress)();
            }
        }
    }
}
