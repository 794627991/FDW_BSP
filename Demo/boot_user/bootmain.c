#include "boot_define_all.h"
/*  
//  使用说明：
//	1.修改 UseHowMany 的值，用到几个串口就等于几
//	2.根据实际情况映射 U_U0_U-U_U6_U的值
//	3.在 main中配置实际使用的串口及波特率
*/

uint08 BootEnable;
uint16 JumpCount;
uint32 FlashOpID;

//ET2, 5ms定时器中断
void Init_Etim2(void)
{
    ETIM_INIT_Type ETIM_init;

    ETIM_init.Nvicpry = 2; //中断优先级选择  越小优先级越高  不能大于3
    ETIM_init.PRE = 100;   //1-255;   以主频分频  1为1分频即不分频
    ETIM_init.count = 400; //计数值   以主频8M,100分频,400/80000=5ms

    API_Etim_Init(ETIM2, &ETIM_init);

    ETIMx_ETxCR_CEN_Setable(ETIM2, ENABLE); //启动定时器
}

void DeinitSystem(void)
{
    __disable_irq(); //关闭全局中断使能

    NVIC_DisableIRQ(ETIM2_IRQn);
    ETIMx_ETxCR_CEN_Setable(ETIM2, DISABLE); //关闭定时器
    RCC_PERCLK_SetableEx(ET2CLK, DISABLE);

    // UART0
#ifdef IRQ_U0
    CloseeIO(GPIOF, GPIO_Pin_3); //PF3 UART0 RX
    CloseeIO(GPIOF, GPIO_Pin_4); //PF4 UART0 TX
    NVIC_DisableIRQ(UART0_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART0, TxInt, DISABLE); //关闭发送中断
    UART_UARTIE_RxTxIE_SetableEx(UART0, RxInt, DISABLE); //关闭接收中断
    UARTx_TXSTA_TXEN_Setable(UART0, DISABLE);            //关闭发送使能
    UARTx_RXSTA_RXEN_Setable(UART0, DISABLE);            //关闭接收使能
#endif

    // UART1
#ifdef IRQ_U1
    CloseeIO(GPIOB, GPIO_Pin_0); //PB0 UART1 RX
    CloseeIO(GPIOB, GPIO_Pin_1); //PB1 UART1 TX
    CloseeIO(GPIOE, GPIO_Pin_3); //PE3 UART1 RX
    CloseeIO(GPIOE, GPIO_Pin_4); //PE4 UART1 TX
    NVIC_DisableIRQ(UART1_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART1, TxInt, DISABLE); //关闭发送中断
    UART_UARTIE_RxTxIE_SetableEx(UART1, RxInt, DISABLE); //关闭接收中断
    UARTx_TXSTA_TXEN_Setable(UART1, DISABLE);            //关闭发送使能
    UARTx_RXSTA_RXEN_Setable(UART1, DISABLE);            //关闭接收使能
#endif

    // UART2
#ifdef IRQ_U2
    CloseeIO(GPIOB, GPIO_Pin_2); //PB2 UART2 RX
    CloseeIO(GPIOB, GPIO_Pin_3); //PB3 UART2 TX
    NVIC_DisableIRQ(UART2_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART2, TxInt, DISABLE); //关闭发送中断
    UART_UARTIE_RxTxIE_SetableEx(UART2, RxInt, DISABLE); //关闭接收中断
    UARTx_TXSTA_TXEN_Setable(UART2, DISABLE);            //关闭发送使能
    UARTx_RXSTA_RXEN_Setable(UART2, DISABLE);            //关闭接收使能
#endif

    // UART3
#ifdef IRQ_U3
    CloseeIO(GPIOC, GPIO_Pin_10); //PC10 UART3 RX
    CloseeIO(GPIOC, GPIO_Pin_11); //PC11 UART3 TX
    NVIC_DisableIRQ(UART3_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART3, TxInt, DISABLE); //关闭发送中断
    UART_UARTIE_RxTxIE_SetableEx(UART3, RxInt, DISABLE); //关闭接收中断
    UARTx_TXSTA_TXEN_Setable(UART3, DISABLE);            //关闭发送使能
    UARTx_RXSTA_RXEN_Setable(UART3, DISABLE);            //关闭接收使能
#endif

    // UART4
#ifdef IRQ_U4
    CloseeIO(GPIOD, GPIO_Pin_0); //PD0 UART4 RX
    CloseeIO(GPIOD, GPIO_Pin_1); //PD1 UART4 TX
    NVIC_DisableIRQ(UART4_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART4, TxInt, DISABLE); //关闭发送中断
    UART_UARTIE_RxTxIE_SetableEx(UART4, RxInt, DISABLE); //关闭接收中断
    UARTx_TXSTA_TXEN_Setable(UART4, DISABLE);            //关闭发送使能
    UARTx_RXSTA_RXEN_Setable(UART4, DISABLE);            //关闭接收使能
#endif

    // UART5
#ifdef IRQ_U5
    CloseeIO(GPIOC, GPIO_Pin_4); //PC4 UART5 RX
    CloseeIO(GPIOC, GPIO_Pin_5); //PC5 UART5 TX
    NVIC_DisableIRQ(UART5_IRQn);
    UART_UARTIE_RxTxIE_SetableEx(UART5, TxInt, DISABLE); //关闭发送中断
    UART_UARTIE_RxTxIE_SetableEx(UART5, RxInt, DISABLE); //关闭接收中断
    UARTx_TXSTA_TXEN_Setable(UART5, DISABLE);            //关闭发送使能
    UARTx_RXSTA_RXEN_Setable(UART5, DISABLE);            //关闭接收使能
#endif
}

int main(void)
{
    uint32 JumpAddress;
    uint32 WrADR;
    Uart_INIT_Type Uart_init;

    Init_System(); //系统初始化

    Uart_init.Nvicpry = 1;                 //中断优先级选择  越小优先级越高  不能大于3
    Uart_init.SEL = Uart1_UsePB0PB1;       //只有使用UART1时候才需要选择
    Uart_init.IRMod = NoUSEIRMod;          //使用红外38K调制
    Uart_init.RxIEEN = ENABLE;             //开启接收中断
    Uart_init.uartint.BaudRate = 9600;     //设置波特率
    Uart_init.uartint.DataBit = Eight8Bit; //8位数据位
    Uart_init.uartint.ParityBit = NONE;    //不开校验
    Uart_init.uartint.StopBit = OneBit;    //1位停止位

    API_Uartx_Init(UART1, &Uart_init); //设置Uart

    Init_Etim2();

    __enable_irq(); //打开全局中断使能
    /********************************************************************/

    BootEnable = 0;
    JumpCount = 0;
    //存储在ee或者flash中，操作flash前读取赋值给FlashOpID,平时存非法值，防止误操作
    FlashOpID = 0; //FLASHOPKEY

    for (;;)
    {
        IWDT_Clr(); //清系统看门狗

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
            DeinitSystem(); //关闭外设

            FlashOpID = 0;
            /* 跳转到应用程序位置 */
            memcpy(&WrADR, (unsigned char *)FlashStartAdr, 4);
            if ((WrADR != 0) && (WrADR < FlashAllSize))
            {
                /* 说明保存的APP起始地址有效 */
                JumpAddress = *(__IO uint32 *)(WrADR + 4);
                __set_MSP(*(__IO uint32 *)WrADR);
                (*(void (*)())JumpAddress)();
            }
            else
            {
                WrADR = BOOTENDADR; /* 写一个默认的APP1区起始地址 */
                Flsah_Write_String(FlashStartAdr, (unsigned char *)&WrADR, 4, FLASHOPKEY);
                JumpAddress = *(__IO uint32 *)(APPLICATION_ADDRESS + 4);
                __set_MSP(*(__IO uint32 *)APPLICATION_ADDRESS);
                (*(void (*)())JumpAddress)();
            }
        }
    }
}
