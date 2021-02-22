/*
*********************************************************************************************************
*
*	模块名称 : 串口BSP程序
*	文件名称 : uart_api.c
*	版    本 : V1.0.1
*	说    明 : 串口初始化，串口中断处理，中断发送或循环发送BSP
*             采用队列方式多重缓存串口数据，必须外部调用malloc.c
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-12-31  高屹    正式发布
*		V1.0.1  2021-1-8    高屹    修改队列保护,增加队列删除保护机制
*		V1.0.2  2021-1-14   高屹    解决中断发的同时收到数据可能出现入队列数据异常的问题
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/

#include "uart_api.h"

#if UseUart0 > 0 || UseUart1 > 0 || UseUart2 > 0 || UseUart3 > 0 || UseUart4 > 0 || UseUart5 > 0 || Uart_Output > 0 || ThisBoot == 1
/* 全局变量定义 */
static COMINFO ComRxInfoManage = {NULL}; /* 串口接收IOFO管理 */
static COMINFO ComTxInfoManage = {NULL}; /* 串口发送IOFO管理 */
static COMALL ComRxInQueue = {NULL};     /* 串口接收队列 */
static COMALL ComTxInQueue = {NULL};     /* 串口发送队列 */

/*
*********************************************************************************************************
*	函 数 名: QueueIsFull
*	功能说明: 判断串口队列是否满
*	形    参: *queue   ：串口队列的指针
*	返 回 值:  0：不满
*             1：满
*             0xff：队列空
*	备    注：无
*********************************************************************************************************
*/
uint8_t QueueIsFull(ComQueue *queue)
{
    if (queue != NULL)
        if (!queue->intodelete)
            return (queue->iCount == MAXSIZE);
    return 0xff;
}
/*
*********************************************************************************************************
*	函 数 名: QueueIsEmpty
*	功能说明: 判断串口队列是否空
*	形    参: *queue   ：串口队列的指针
*	返 回 值:  0：不空
*             1：空
*             0xff：队列空
*	备    注：无
*********************************************************************************************************
*/
uint8_t QueueIsEmpty(ComQueue *queue)
{
    if (queue != NULL)
        if (!queue->intodelete)
            return (queue->iCount == 0);
    return 0xff;
}
/*
*********************************************************************************************************
*	函 数 名: InitComMessage
*	功能说明: 建立队列并初始化
*	形    参: 无
*	返 回 值: 非NULL即为队列的地址
*	备    注：调用了这个函数一定要记得有myfree掉的地方
*********************************************************************************************************
*/
ComQueue *InitComMessage(void)
{
    ComQueue *queue = (ComQueue *)mymallocpro(sizeof(ComQueue));
    if (queue == NULL)
    {
        return NULL;
    }
    queue->iHead = 0;
    queue->iCount = 0;
    queue->intodelete = 0;
    return queue;
}
/*
*********************************************************************************************************
*	函 数 名: SendComMessage
*	功能说明: 像队列发送串口数据
*	形    参: *queue：串口队列指针
*             *_dat:串口数据,包括长度和缓存接收的地址(ComData是个柔性数组)
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void SendComMessage(ComQueue *queue, ComData *_dat)
{
    if (_dat == NULL)
    {
        return;
    }
    if (QueueIsFull(queue)) /* 可以直接判断出队列空或者队列满 */
    {
        myfree(_dat);
        return;
    }
    queue->dat[(queue->iHead + queue->iCount) % MAXSIZE] = _dat;
    queue->iCount++;
}
/*
*********************************************************************************************************
*	函 数 名: GetComMessage
*	功能说明: 获取队列里的串口数据
*	形    参: *queue：串口队列指针
*	返 回 值: ComData* ：串口数据,包括长度和缓存接收的地址(ComData是个柔性数组)
*	备    注：注意调用该函数之后一定要myfree掉缓存
*********************************************************************************************************
*/
ComData *GetComMessage(ComQueue *queue)
{
    ComData *_dat = queue->dat[queue->iHead];
    queue->iHead = (queue->iHead + 1) % MAXSIZE;
    queue->iCount--;
    return _dat;
}
/*
*********************************************************************************************************
*	函 数 名: DelComMessage
*	功能说明: 删除队列和队列里的数据
*	形    参: *queue：串口队列指针
*	返 回 值: 无
*	备    注：调用头文件中宏定义 DelComMessage 或者调用该函数后，手动将队列 = NULL
*********************************************************************************************************
*/
void __DelComMessage(ComQueue *queue)
{
    if (queue == NULL)
        return;
    queue->intodelete = 1;
    for (uint8_t i = 0; i < MAXSIZE; i++)
    {
        if (!(queue->iCount == 0))
        {
            ComData *data = GetComMessage(queue);
            myfree(data); /* 释放队列中获取未解析的数据 */
        }
        else
        {
            break;
        }
    }
    myfree(queue);
}
/*
*********************************************************************************************************
*	函 数 名: API_UART_It_Start
*	功能说明: 串口中断发开始,利用中断发送一个数据
*	形    参: uartx：串口号,0-5
*             data：要发送的数据
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void API_UART_It_Start(uint8_t uartx, uint8_t data)
{
    volatile uint32_t UARTx;
    UARTx = UART0_BASE + ((uint32_t)uartx << 5);
    UART_UARTIF_RxTxIF_ClrEx((UARTx_Type *)UARTx); /* 清除发送中断标志 */
    UARTx_TXREG_Write((UARTx_Type *)UARTx, data);  /* 发送一个数据，启动发送 */
}
/*
*********************************************************************************************************
*	函 数 名: API_UART_It_End
*	功能说明: 串口中断结束,主要是清中断标志
*	形    参: uartx：串口号,0-5
*	返 回 值: 无
*	备    注：这里不关闭中断发送使能
*********************************************************************************************************
*/
void API_UART_It_End(uint8_t uartx)
{
    volatile uint32_t UARTx;
    UARTx = UART0_BASE + ((uint32_t)uartx << 5);
    UART_UARTIF_RxTxIF_ClrEx((UARTx_Type *)UARTx);
}
/*
*********************************************************************************************************
*	函 数 名: API_UART_It_Send
*	功能说明: 串口中断发
*	形    参: uartx：串口号,0-5
*             data：要发送数据的缓存地址
*             len：要发送数据的长度
*	返 回 值: 无
*	备    注：使用多重缓存方式进行发送，避免中断发多次调用导致发送数据错误，
*             同时需要注意缓存有上限为MAXSIZE，如果多次快速调用该函数超过缓存上限，可能导致要发送的数据丢失
*********************************************************************************************************
*/
void API_UART_It_Send(uint8_t uartx, uint8_t *data, uint16_t len)
{
    uint8_t startsend = 0;

    if ((len == 0) || (uartx > MAXUART - 1))
        return;

    if (ComTxInQueue.queue[uartx] == NULL)
    {
        if ((ComTxInQueue.queue[uartx] = InitComMessage()) != NULL)
        {
            startsend = 1;
        }
    }
    if (ComTxInQueue.queue[uartx] != NULL)
    {
        ComData *_dat = (ComData *)mymalloc(sizeof(ComData) + len);
        if (_dat != NULL)
        {
            memcpy(_dat->pData, data, len);
            _dat->maxlen = len;
            (startsend == 1) ? (_dat->len = 1) : (_dat->len = 0);
            __disable_irq();
            SendComMessage(ComTxInQueue.queue[uartx], _dat);
            __enable_irq();
            if (startsend)
            {
                /* 把第一个数据利用中断方式先发走 */
                API_UART_It_Start(uartx, _dat->pData[0]);
            }
        }
    }
}
/*
*********************************************************************************************************
*	函 数 名: UartTxInterupt
*	功能说明: 串口中断发
*	形    参: uartx：串口号,0-5
*	返 回 值: 无
*	备    注：放在硬件串口中断发的地方
*********************************************************************************************************
*/
void UartTxInterupt(uint8_t uartx)
{
    if (uartx > MAXUART - 1)
        return;
    if (ComTxInfoManage.dat[uartx] == NULL)
    {
        if (!QueueIsEmpty(ComTxInQueue.queue[uartx])) /* 串口队列不为空 */
        {
            __disable_irq();
            ComTxInfoManage.dat[uartx] = GetComMessage(ComTxInQueue.queue[uartx]); /* 取出数据 */
            __enable_irq();
            if (ComTxInfoManage.dat[uartx] == NULL)
            {
                DelComMessage(ComTxInQueue.queue[uartx]); /* 数据取出异常，直接删了队列 */
                API_UART_It_End(uartx);                   /* 停止继续发送 */
            }
        }
        else
        {
            DelComMessage(ComTxInQueue.queue[uartx]); /* 数据取出异常，直接删了队列 */
            API_UART_It_End(uartx);                   /* 队列也全空说明确实没有要发送的数据了，可以关闭中断 */
        }
    }

    if (ComTxInfoManage.dat[uartx] != NULL)
    {
        if (ComTxInfoManage.dat[uartx]->len < ComTxInfoManage.dat[uartx]->maxlen)
        {
            API_UART_It_Start(uartx, ComTxInfoManage.dat[uartx]->pData[ComTxInfoManage.dat[uartx]->len]);
            ComTxInfoManage.dat[uartx]->len++;
        }
        if (ComTxInfoManage.dat[uartx]->len >= ComTxInfoManage.dat[uartx]->maxlen)
        {
            myfree(ComTxInfoManage.dat[uartx]); /* 数据已经发送完成了,就可以释放掉 */
        }
    }
}
/*
*********************************************************************************************************
*	函 数 名: UartRxInterupt
*	功能说明: 串口中断收
*	形    参: data：串口中断收到的数据
*             uartx：串口号,0-5
*             maxlen：串口接收开辟的最大缓存长度
*	返 回 值: 无
*	备    注：放在硬件串口中断收的地方
*            在接收一串数据帧的第一个字节的时候会动态分配内存，可能会因为分配时间长而丢失少量字节
*            实际测试115200波特率在32M下不会丢失数据，9600波特率在8M下也不会(mymalloc已优化分配速度)
*********************************************************************************************************
*/
void UartRxInterupt(uint8_t data, uint8_t uartx, uint16_t maxlen)
{
    if ((maxlen == 0) || (uartx > MAXUART - 1))
        return;
    if (ComRxInfoManage.dat[uartx] != NULL)
    {
        if (ComRxInfoManage.dat[uartx]->maxlen != maxlen)
        {
            /*说明最大长度发生了改变，需要销毁上一次申请的内存重新申请*/
            myfree(ComRxInfoManage.dat[uartx]);
        }
    }
    if ((ComRxInfoManage.dat[uartx] == NULL) && (data != 0)) /* 首字母是\0不接收 */
    {
        /* 在接收中断中分配内存可能会因为分配时间长而丢失少量字节 */
        ComRxInfoManage.dat[uartx] = (ComData *)mymalloc(sizeof(ComData) + maxlen);
        if (ComRxInfoManage.dat[uartx] != NULL)
        {
            ComRxInfoManage.dat[uartx]->len = 0;
            ComRxInfoManage.dat[uartx]->maxlen = maxlen;
        }
    }
    if (ComRxInfoManage.dat[uartx] != NULL)
    {
        if (ComRxInfoManage.dat[uartx]->len > maxlen - 1)
            ComRxInfoManage.dat[uartx]->len = 0;
        else
        {
            if ((data == 0) && (ComRxInfoManage.dat[uartx]->len == 0)) /* 首字母是\0不接收 */
                return;
            ComRxInfoManage.dat[uartx]->pData[ComRxInfoManage.dat[uartx]->len] = data;
            ComRxInfoManage.dat[uartx]->len++;
        }
    }
}
/*
*********************************************************************************************************
*	函 数 名: GetComInfo
*	功能说明: 获取串口中断收到的数据内容
*	形    参: uartx：串口号,0-5
*	返 回 值: ComData *：串口数据内容及长度
*	备    注：注意调用该函数之后一定要调用FreeComInfo，清除缓存
*********************************************************************************************************
*/
ComData *GetComInfo(uint8_t uartx)
{
    if (uartx > MAXUART - 1)
        return NULL;
    return ComRxInfoManage.dat[uartx];
}
/*
*********************************************************************************************************
*	函 数 名: FreeComInfo
*	功能说明: 清除串口缓存
*	形    参: uartx：串口号,0-5
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void FreeComInfo(uint8_t uartx)
{
    if (uartx > MAXUART - 1)
        return;
    myfree(ComRxInfoManage.dat[uartx]);
}
/*
*********************************************************************************************************
*	函 数 名: API_UART_Rx_Queue_Send
*	功能说明: 将串口中断接收完的数据发送到队列
*	形    参: uartx：串口号,0-5
*	返 回 值: 无
*	备    注：若开启串口超时接收配置，该函数会直接通过宏定义配置完成，在uarttimeout.c中调用
*            该函数需要配合 API_UART_Rx_Queue_Get_Do 或 API_UART_Rx_Queue_Get 一起使用，
*********************************************************************************************************
*/
void API_UART_Rx_Queue_Send(uint8_t uartx)
{
    if (uartx > MAXUART - 1)
        return;
    if (ComRxInfoManage.dat[uartx] == NULL)
        return;

    if (ComRxInQueue.queue[uartx] == NULL)
    {
        if ((ComRxInQueue.queue[uartx] = InitComMessage()) != NULL)
        {
            memset(ComRxInQueue.queue[uartx], 0, sizeof(ComQueue));
        }
    }
    if (!QueueIsFull(ComRxInQueue.queue[uartx])) /* 串口队列不满 */
    {
        ComData *_dat = myrealloc(ComRxInfoManage.dat[uartx], sizeof(ComData) + ComRxInfoManage.dat[uartx]->len);
        if (_dat != NULL)
        {
            __disable_irq();
            SendComMessage(ComRxInQueue.queue[uartx], _dat);
            __enable_irq();
        }
    }
    myfree(ComRxInfoManage.dat[uartx]); /* 无论成不成，都要销毁申请的串口缓存 */
}
/*
*********************************************************************************************************
*	函 数 名: API_UART_Rx_Queue_Get
*	功能说明: 从串口接收队列中获取队列中缓存的数据
*	形    参: uartx：串口号,0-5
*	返 回 值: ComData *：串口数据内容及长度
*	备    注：调用该函数需要自行myfree
*             该函数是 API_UART_Rx_Queue_Get_Do 的前置版，
*             如果希望自己处理数据并且能清好缓存，可以调用该函数。
*********************************************************************************************************
*/
ComData *API_UART_Rx_Queue_Get(uint8_t uartx)
{
    static ComData *_dat = NULL;
    if (uartx > MAXUART - 1)
        return NULL;
    myfree(_dat); /* 防止忘了清上一次取出的数据 */
    if (_dat == NULL)
    {
        if (!QueueIsEmpty(ComRxInQueue.queue[uartx])) /* 串口队列不为空 */
        {
            __disable_irq();
            ComData *_dat = GetComMessage(ComRxInQueue.queue[uartx]); /* 取出数据 */
            __enable_irq();
            if (_dat != NULL)
            {
                return _dat;
            }
        }
    }
    /* 能进这里说明要么本身就是空，要么确实没数据了 */
    DelComMessage(ComRxInQueue.queue[uartx]);
    return NULL;
}
/*
*********************************************************************************************************
*	函 数 名: API_UART_Rx_Queue_Get_Do
*	功能说明: 串口数据接收完成后要的事情
*	形    参: uartx：串口号,0-5
*             rxdohandle rxdo：函数指针，本体为：void (*rxdohandle)(uint8_t *buf, uint16_t len)
*	返 回 值: 无
*	备    注：该函数直接放在串口接收成功的地方进行数据分析处理
*             若开启串口超时接收配置，该函数会直接通过宏定义配置完成，在uarttimeout.c中调用
*             如下：API_UART_Rx_Queue_Get_Do(x, API_UART0_Dx);
*             即调用时需要编写 API_UARTx_DO 即可
*********************************************************************************************************
*/
void API_UART_Rx_Queue_Get_Do(uint8_t uartx, rxdohandle rxdo)
{
    if (uartx > MAXUART - 1)
        return;

    while (!QueueIsEmpty(ComRxInQueue.queue[uartx])) /* 串口队列不为空 */
    {
        __disable_irq();
        ComData *_dat = GetComMessage(ComRxInQueue.queue[uartx]); /* 取出数据 */
        __enable_irq();
        if (_dat != NULL)
        {
            rxdo(_dat->pData, _dat->len);
            myfree(_dat);
        }
    }

    /* 能进这里说明要么本身就是空，要么确实没数据了 */
    DelComMessage(ComRxInQueue.queue[uartx]);
}
/*
*********************************************************************************************************
*	函 数 名: API_Uartx_Op
*	功能说明: 操作uartio口2功能开关
*	形    参: op 0：关  1：开
*             uartx 0-5  对应串口
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void API_Uartx_Op(bool op, uint8_t uartx)
{
    if (op == 0)
    {
        if (uartx == 0)
        {
            CloseeIO(GPIOF, GPIO_Pin_3); //PF3 UART0 RX
            CloseeIO(GPIOF, GPIO_Pin_4); //PF4 UART0 TX
        }
        else if (uartx == 1)
        {
#if U1UsePE3PE4 == 0
            CloseeIO(GPIOB, GPIO_Pin_0); //PB0 UART1 RX
            CloseeIO(GPIOB, GPIO_Pin_1); //PB1 UART1 TX
#else
            CloseeIO(GPIOE, GPIO_Pin_3);    //PE3 UART1 RX
            CloseeIO(GPIOE, GPIO_Pin_4);    //PE4 UART1 TX
#endif
        }
        else if (uartx == 2)
        {
            CloseeIO(GPIOB, GPIO_Pin_2); //PB2 UART2 RX
            CloseeIO(GPIOB, GPIO_Pin_3); //PB3 UART2 TX
        }
        else if (uartx == 3)
        {
            CloseeIO(GPIOC, GPIO_Pin_10); //PC10 UART3 RX
            CloseeIO(GPIOC, GPIO_Pin_11); //PC11 UART3 TX
        }
        else if (uartx == 4)
        {
            CloseeIO(GPIOD, GPIO_Pin_0); //PD0 UART4 RX
            CloseeIO(GPIOD, GPIO_Pin_1); //PD1 UART4 TX
        }
        else if (uartx == 5)
        {
            CloseeIO(GPIOC, GPIO_Pin_4); //PC4 UART5 RX
            CloseeIO(GPIOC, GPIO_Pin_5); //PC5 UART5 TX
        }
    }
    else
    {
        if (uartx == 0)
        {
            AltFunIO(GPIOF, GPIO_Pin_3, 2); //PF3 UART0 RX
            AltFunIO(GPIOF, GPIO_Pin_4, 0); //PF4 UART0 TX
        }
        else if (uartx == 1)
        {
#if U1UsePE3PE4 == 0
            AltFunIO(GPIOB, GPIO_Pin_0, 2); //PB0 UART1 RX
            AltFunIO(GPIOB, GPIO_Pin_1, 0); //PB1 UART1 TX
#else
            AltFunIO(GPIOE, GPIO_Pin_3, 2); //PE3 UART1 RX
            AltFunIO(GPIOE, GPIO_Pin_4, 0); //PE4 UART1 TX
#endif
        }
        else if (uartx == 2)
        {
            AltFunIO(GPIOB, GPIO_Pin_2, 2); //PB2 UART2 RX
            AltFunIO(GPIOB, GPIO_Pin_3, 0); //PB3 UART2 TX
        }
        else if (uartx == 3)
        {
            AltFunIO(GPIOC, GPIO_Pin_10, 2); //PC10 UART3 RX
            AltFunIO(GPIOC, GPIO_Pin_11, 0); //PC11 UART3 TX
        }
        else if (uartx == 4)
        {
            AltFunIO(GPIOD, GPIO_Pin_0, 2); //PD0 UART4 RX
            AltFunIO(GPIOD, GPIO_Pin_1, 0); //PD1 UART4 TX
        }
        else if (uartx == 5)
        {
            AltFunIO(GPIOC, GPIO_Pin_4, 2); //PC4 UART5 RX
            AltFunIO(GPIOC, GPIO_Pin_5, 0); //PC5 UART5 TX
        }
    }
}
/*
*********************************************************************************************************
*	函 数 名: API_Uartx_Init
*	功能说明: 串口初始化函数
*	形    参: *Uartx：串口的实际物理地址
*             Uart_INIT_Type *init：初始化的结构体指针
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void API_Uartx_Init(UARTx_Type *Uartx, Uart_INIT_Type *init)
{
    RCC_ClocksType RCC_Clocks;
    volatile uint8_t tmp08;

    RCC_PERCLK_SetableEx(UARTCOMCLK, ENABLE); /* UART0~5共享寄存器时钟使能 */

    tmp08 = ((uint32_t)Uartx - UART0_BASE) >> 5; /* 获取uart */

    switch (tmp08)
    {
    case 0:
        RCC_PERCLK_SetableEx(UART0CLK, ENABLE); /* UARTx时钟使能 */
        /* UART0 IO 配置 */
        AltFunIO(GPIOF, GPIO_Pin_3, 2); //PF3 UART0 RX
        AltFunIO(GPIOF, GPIO_Pin_4, 0); //PF4 UART0 TX
        /*NVIC中断配置*/
        NVIC_SET(UART0_IRQn, init->Nvicpry);
        break;

    case 1:
        RCC_PERCLK_SetableEx(UART1CLK, ENABLE); /* UARTx时钟使能 */
        /* UART1 IO 配置 */
        if (init->SEL == Uart1_UsePB0PB1)
        {
            AltFunIO(GPIOB, GPIO_Pin_0, 2); /* PB0 UART1 RX */
            AltFunIO(GPIOB, GPIO_Pin_1, 0); /* PB1 UART1 TX */
        }
        else
        {
            AltFunIO(GPIOE, GPIO_Pin_3, 2); /* PE3 UART1 RX */
            AltFunIO(GPIOE, GPIO_Pin_4, 0); /* PE4 UART1 TX */
        }
        /*NVIC中断配置*/
        NVIC_SET(UART1_IRQn, init->Nvicpry);
        break;

    case 2:
        RCC_PERCLK_SetableEx(UART2CLK, ENABLE); /* UARTx时钟使能 */
        /* UART2 IO 配置 */
        AltFunIO(GPIOB, GPIO_Pin_2, 2); /* PB2 UART2 RX */
        AltFunIO(GPIOB, GPIO_Pin_3, 0); /* PB3 UART2 TX */

        /*NVIC中断配置*/
        NVIC_SET(UART2_IRQn, init->Nvicpry);
        break;

    case 3:
        RCC_PERCLK_SetableEx(UART3CLK, ENABLE); /* UARTx时钟使能 */
        /* UART3 IO 配置 */
        AltFunIO(GPIOC, GPIO_Pin_10, 2); /* PC10 UART3 RX */
        AltFunIO(GPIOC, GPIO_Pin_11, 0); /* PC11 UART3 TX */
        /*NVIC中断配置*/
        NVIC_SET(UART3_IRQn, init->Nvicpry);
        break;

    case 4:
        RCC_PERCLK_SetableEx(UART4CLK, ENABLE); /* UARTx时钟使能 */
        /* UART4 IO 配置 */
        AltFunIO(GPIOD, GPIO_Pin_0, 2); /* PD0 UART4 RX */
        AltFunIO(GPIOD, GPIO_Pin_1, 0); /* PD1 UART4 TX */
        /*NVIC中断配置*/
        NVIC_SET(UART4_IRQn, init->Nvicpry);
        break;

    case 5:
        RCC_PERCLK_SetableEx(UART5CLK, ENABLE); /* UARTx时钟使能 */
        /* UART5 IO 配置 */
        AltFunIO(GPIOC, GPIO_Pin_4, 2); /* PC4 UART5 RX */
        AltFunIO(GPIOC, GPIO_Pin_5, 0); /* PC5 UART5 TX */
        /*NVIC中断配置*/
        NVIC_SET(UART5_IRQn, init->Nvicpry);
        break;

    default:
        break;
    }

    RCC_GetClocksFreq(&RCC_Clocks);                                 /* 获取APB时钟频率 */
    UART_SInit(Uartx, &init->uartint, RCC_Clocks.APBCLK_Frequency); /* 初始化uart */

    UARTx_RXSTA_RXEN_Setable(Uartx, ENABLE); /* 打开接收使能 */
    UARTx_TXSTA_TXEN_Setable(Uartx, ENABLE); /* 打开发送使能 */

    if (init->RxIEEN == ENABLE)
        UART_UARTIE_RxTxIE_SetableEx(Uartx, RxInt, ENABLE); /* 打开接收中断 */
    else
        UART_UARTIE_RxTxIE_SetableEx(Uartx, RxInt, DISABLE); /* 关闭接收中断 */

    if (init->TxIEEN == ENABLE)
        UART_UARTIE_RxTxIE_SetableEx(Uartx, TxInt, ENABLE); /* 打开发送中断 */
    else
        UART_UARTIE_RxTxIE_SetableEx(Uartx, TxInt, DISABLE); /* 关闭发送中断 */

    if (init->IRMod == USEIRMod)
    {
        UART_IRModulation_Init(38000, 50, RCC_Clocks.APBCLK_Frequency);
        UARTx_TXSTA_IREN_Setable(Uartx, ENABLE); /* 发送红外调制使能位 */
    }
}
/*
*********************************************************************************************************
*	函 数 名: API_Uart_Send
*	功能说明: 串口发送函数
*	形    参: *Uartx：串口的实际物理地址
*             *str：发送缓存的地址指针
*             len：发送的长度
*	返 回 值: 无
*	备    注：该API没有限定长度，注意数组越界(查询发送)
*********************************************************************************************************
*/
__weak void API_Uart_Send(UARTx_Type *UARTx, uint8_t *str, uint16_t len)
{
    uint16_t i = 0, j = 0;
#if UseBootFixFun == 0
    /* 能做固化的都是函数内不调用全局变量的，因此如果使用固化法就无法使用该函数进行中断发送 */
    if (ENABLE == UART_UARTIE_RxTxIE_GetableEx(UARTx, TxInt))
    {
        /* 开了串口中断发送 */
        volatile uint8_t tmp08;
        tmp08 = ((uint32_t)UARTx - UART0_BASE) >> 5; /* 获取uart */
        API_UART_It_Send(tmp08, str, len);
    }
    else
#endif
    {
        /*查询发送方式*/
        for (i = 0; i < len; i++)
        {
            UARTx_TXREG_Write(UARTx, (uint32_t)str[i]);
            while (j < 0x5555)
            {
                if (UARTx_TXBUFSTA_TXFF_Chk(UARTx) == 0)
                {
                    j = 0;
                    break;
                }
                else
                {
                    j++;
                }
            }
            if (j >= 0x5554)
            {
                i = len;
            }
        }
    }
}
/*
*********************************************************************************************************
*	函 数 名: API_UART_SEND
*	功能说明: 串口发送函数
*	形    参: uartx：0-5 串口号
*             *data：发送缓存的地址指针
*             len：发送的长度
*	返 回 值: 无
*	备    注：该API没有限定长度，注意数组越界(查询发送)，直接通过串口号进行发送的api
*********************************************************************************************************
*/
void API_UART_SEND(uint8_t uartx, uint8_t *data, uint16_t len)
{
    volatile uint32_t UARTx;
    if (uartx > MAXUART - 1)
        return;
    UARTx = UART0_BASE + ((uint32_t)uartx << 5);
    API_Uart_Send((UARTx_Type *)UARTx, data, len);
}

#endif
