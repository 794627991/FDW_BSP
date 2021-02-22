/*
*********************************************************************************************************
*
*	ģ������ : ����BSP����
*	�ļ����� : uart_api.c
*	��    �� : V1.0.1
*	˵    �� : ���ڳ�ʼ���������жϴ����жϷ��ͻ�ѭ������BSP
*             ���ö��з�ʽ���ػ��洮�����ݣ������ⲿ����malloc.c
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����    ��ʽ����
*		V1.0.1  2021-1-8    ����    �޸Ķ��б���,���Ӷ���ɾ����������
*		V1.0.2  2021-1-14   ����    ����жϷ���ͬʱ�յ����ݿ��ܳ�������������쳣������
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/

#include "uart_api.h"

#if UseUart0 > 0 || UseUart1 > 0 || UseUart2 > 0 || UseUart3 > 0 || UseUart4 > 0 || UseUart5 > 0 || Uart_Output > 0 || ThisBoot == 1
/* ȫ�ֱ������� */
static COMINFO ComRxInfoManage = {NULL}; /* ���ڽ���IOFO���� */
static COMINFO ComTxInfoManage = {NULL}; /* ���ڷ���IOFO���� */
static COMALL ComRxInQueue = {NULL};     /* ���ڽ��ն��� */
static COMALL ComTxInQueue = {NULL};     /* ���ڷ��Ͷ��� */

/*
*********************************************************************************************************
*	�� �� ��: QueueIsFull
*	����˵��: �жϴ��ڶ����Ƿ���
*	��    ��: *queue   �����ڶ��е�ָ��
*	�� �� ֵ:  0������
*             1����
*             0xff�����п�
*	��    ע����
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
*	�� �� ��: QueueIsEmpty
*	����˵��: �жϴ��ڶ����Ƿ��
*	��    ��: *queue   �����ڶ��е�ָ��
*	�� �� ֵ:  0������
*             1����
*             0xff�����п�
*	��    ע����
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
*	�� �� ��: InitComMessage
*	����˵��: �������в���ʼ��
*	��    ��: ��
*	�� �� ֵ: ��NULL��Ϊ���еĵ�ַ
*	��    ע���������������һ��Ҫ�ǵ���myfree���ĵط�
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
*	�� �� ��: SendComMessage
*	����˵��: ����з��ʹ�������
*	��    ��: *queue�����ڶ���ָ��
*             *_dat:��������,�������Ⱥͻ�����յĵ�ַ(ComData�Ǹ���������)
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void SendComMessage(ComQueue *queue, ComData *_dat)
{
    if (_dat == NULL)
    {
        return;
    }
    if (QueueIsFull(queue)) /* ����ֱ���жϳ����пջ��߶����� */
    {
        myfree(_dat);
        return;
    }
    queue->dat[(queue->iHead + queue->iCount) % MAXSIZE] = _dat;
    queue->iCount++;
}
/*
*********************************************************************************************************
*	�� �� ��: GetComMessage
*	����˵��: ��ȡ������Ĵ�������
*	��    ��: *queue�����ڶ���ָ��
*	�� �� ֵ: ComData* ����������,�������Ⱥͻ�����յĵ�ַ(ComData�Ǹ���������)
*	��    ע��ע����øú���֮��һ��Ҫmyfree������
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
*	�� �� ��: DelComMessage
*	����˵��: ɾ�����кͶ����������
*	��    ��: *queue�����ڶ���ָ��
*	�� �� ֵ: ��
*	��    ע������ͷ�ļ��к궨�� DelComMessage ���ߵ��øú������ֶ������� = NULL
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
            myfree(data); /* �ͷŶ����л�ȡδ���������� */
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
*	�� �� ��: API_UART_It_Start
*	����˵��: �����жϷ���ʼ,�����жϷ���һ������
*	��    ��: uartx�����ں�,0-5
*             data��Ҫ���͵�����
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void API_UART_It_Start(uint8_t uartx, uint8_t data)
{
    volatile uint32_t UARTx;
    UARTx = UART0_BASE + ((uint32_t)uartx << 5);
    UART_UARTIF_RxTxIF_ClrEx((UARTx_Type *)UARTx); /* ��������жϱ�־ */
    UARTx_TXREG_Write((UARTx_Type *)UARTx, data);  /* ����һ�����ݣ��������� */
}
/*
*********************************************************************************************************
*	�� �� ��: API_UART_It_End
*	����˵��: �����жϽ���,��Ҫ�����жϱ�־
*	��    ��: uartx�����ں�,0-5
*	�� �� ֵ: ��
*	��    ע�����ﲻ�ر��жϷ���ʹ��
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
*	�� �� ��: API_UART_It_Send
*	����˵��: �����жϷ�
*	��    ��: uartx�����ں�,0-5
*             data��Ҫ�������ݵĻ����ַ
*             len��Ҫ�������ݵĳ���
*	�� �� ֵ: ��
*	��    ע��ʹ�ö��ػ��淽ʽ���з��ͣ������жϷ���ε��õ��·������ݴ���
*             ͬʱ��Ҫע�⻺��������ΪMAXSIZE�������ο��ٵ��øú��������������ޣ����ܵ���Ҫ���͵����ݶ�ʧ
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
                /* �ѵ�һ�����������жϷ�ʽ�ȷ��� */
                API_UART_It_Start(uartx, _dat->pData[0]);
            }
        }
    }
}
/*
*********************************************************************************************************
*	�� �� ��: UartTxInterupt
*	����˵��: �����жϷ�
*	��    ��: uartx�����ں�,0-5
*	�� �� ֵ: ��
*	��    ע������Ӳ�������жϷ��ĵط�
*********************************************************************************************************
*/
void UartTxInterupt(uint8_t uartx)
{
    if (uartx > MAXUART - 1)
        return;
    if (ComTxInfoManage.dat[uartx] == NULL)
    {
        if (!QueueIsEmpty(ComTxInQueue.queue[uartx])) /* ���ڶ��в�Ϊ�� */
        {
            __disable_irq();
            ComTxInfoManage.dat[uartx] = GetComMessage(ComTxInQueue.queue[uartx]); /* ȡ������ */
            __enable_irq();
            if (ComTxInfoManage.dat[uartx] == NULL)
            {
                DelComMessage(ComTxInQueue.queue[uartx]); /* ����ȡ���쳣��ֱ��ɾ�˶��� */
                API_UART_It_End(uartx);                   /* ֹͣ�������� */
            }
        }
        else
        {
            DelComMessage(ComTxInQueue.queue[uartx]); /* ����ȡ���쳣��ֱ��ɾ�˶��� */
            API_UART_It_End(uartx);                   /* ����Ҳȫ��˵��ȷʵû��Ҫ���͵������ˣ����Թر��ж� */
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
            myfree(ComTxInfoManage.dat[uartx]); /* �����Ѿ����������,�Ϳ����ͷŵ� */
        }
    }
}
/*
*********************************************************************************************************
*	�� �� ��: UartRxInterupt
*	����˵��: �����ж���
*	��    ��: data�������ж��յ�������
*             uartx�����ں�,0-5
*             maxlen�����ڽ��տ��ٵ���󻺴泤��
*	�� �� ֵ: ��
*	��    ע������Ӳ�������ж��յĵط�
*            �ڽ���һ������֡�ĵ�һ���ֽڵ�ʱ��ᶯ̬�����ڴ棬���ܻ���Ϊ����ʱ�䳤����ʧ�����ֽ�
*            ʵ�ʲ���115200��������32M�²��ᶪʧ���ݣ�9600��������8M��Ҳ����(mymalloc���Ż������ٶ�)
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
            /*˵����󳤶ȷ����˸ı䣬��Ҫ������һ��������ڴ���������*/
            myfree(ComRxInfoManage.dat[uartx]);
        }
    }
    if ((ComRxInfoManage.dat[uartx] == NULL) && (data != 0)) /* ����ĸ��\0������ */
    {
        /* �ڽ����ж��з����ڴ���ܻ���Ϊ����ʱ�䳤����ʧ�����ֽ� */
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
            if ((data == 0) && (ComRxInfoManage.dat[uartx]->len == 0)) /* ����ĸ��\0������ */
                return;
            ComRxInfoManage.dat[uartx]->pData[ComRxInfoManage.dat[uartx]->len] = data;
            ComRxInfoManage.dat[uartx]->len++;
        }
    }
}
/*
*********************************************************************************************************
*	�� �� ��: GetComInfo
*	����˵��: ��ȡ�����ж��յ�����������
*	��    ��: uartx�����ں�,0-5
*	�� �� ֵ: ComData *�������������ݼ�����
*	��    ע��ע����øú���֮��һ��Ҫ����FreeComInfo���������
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
*	�� �� ��: FreeComInfo
*	����˵��: ������ڻ���
*	��    ��: uartx�����ں�,0-5
*	�� �� ֵ: ��
*	��    ע����
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
*	�� �� ��: API_UART_Rx_Queue_Send
*	����˵��: �������жϽ���������ݷ��͵�����
*	��    ��: uartx�����ں�,0-5
*	�� �� ֵ: ��
*	��    ע�����������ڳ�ʱ�������ã��ú�����ֱ��ͨ���궨��������ɣ���uarttimeout.c�е���
*            �ú�����Ҫ��� API_UART_Rx_Queue_Get_Do �� API_UART_Rx_Queue_Get һ��ʹ�ã�
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
    if (!QueueIsFull(ComRxInQueue.queue[uartx])) /* ���ڶ��в��� */
    {
        ComData *_dat = myrealloc(ComRxInfoManage.dat[uartx], sizeof(ComData) + ComRxInfoManage.dat[uartx]->len);
        if (_dat != NULL)
        {
            __disable_irq();
            SendComMessage(ComRxInQueue.queue[uartx], _dat);
            __enable_irq();
        }
    }
    myfree(ComRxInfoManage.dat[uartx]); /* ���۳ɲ��ɣ���Ҫ��������Ĵ��ڻ��� */
}
/*
*********************************************************************************************************
*	�� �� ��: API_UART_Rx_Queue_Get
*	����˵��: �Ӵ��ڽ��ն����л�ȡ�����л��������
*	��    ��: uartx�����ں�,0-5
*	�� �� ֵ: ComData *�������������ݼ�����
*	��    ע�����øú�����Ҫ����myfree
*             �ú����� API_UART_Rx_Queue_Get_Do ��ǰ�ð棬
*             ���ϣ���Լ��������ݲ�������û��棬���Ե��øú�����
*********************************************************************************************************
*/
ComData *API_UART_Rx_Queue_Get(uint8_t uartx)
{
    static ComData *_dat = NULL;
    if (uartx > MAXUART - 1)
        return NULL;
    myfree(_dat); /* ��ֹ��������һ��ȡ�������� */
    if (_dat == NULL)
    {
        if (!QueueIsEmpty(ComRxInQueue.queue[uartx])) /* ���ڶ��в�Ϊ�� */
        {
            __disable_irq();
            ComData *_dat = GetComMessage(ComRxInQueue.queue[uartx]); /* ȡ������ */
            __enable_irq();
            if (_dat != NULL)
            {
                return _dat;
            }
        }
    }
    /* �ܽ�����˵��Ҫô������ǿգ�Ҫôȷʵû������ */
    DelComMessage(ComRxInQueue.queue[uartx]);
    return NULL;
}
/*
*********************************************************************************************************
*	�� �� ��: API_UART_Rx_Queue_Get_Do
*	����˵��: �������ݽ�����ɺ�Ҫ������
*	��    ��: uartx�����ں�,0-5
*             rxdohandle rxdo������ָ�룬����Ϊ��void (*rxdohandle)(uint8_t *buf, uint16_t len)
*	�� �� ֵ: ��
*	��    ע���ú���ֱ�ӷ��ڴ��ڽ��ճɹ��ĵط��������ݷ�������
*             ���������ڳ�ʱ�������ã��ú�����ֱ��ͨ���궨��������ɣ���uarttimeout.c�е���
*             ���£�API_UART_Rx_Queue_Get_Do(x, API_UART0_Dx);
*             ������ʱ��Ҫ��д API_UARTx_DO ����
*********************************************************************************************************
*/
void API_UART_Rx_Queue_Get_Do(uint8_t uartx, rxdohandle rxdo)
{
    if (uartx > MAXUART - 1)
        return;

    while (!QueueIsEmpty(ComRxInQueue.queue[uartx])) /* ���ڶ��в�Ϊ�� */
    {
        __disable_irq();
        ComData *_dat = GetComMessage(ComRxInQueue.queue[uartx]); /* ȡ������ */
        __enable_irq();
        if (_dat != NULL)
        {
            rxdo(_dat->pData, _dat->len);
            myfree(_dat);
        }
    }

    /* �ܽ�����˵��Ҫô������ǿգ�Ҫôȷʵû������ */
    DelComMessage(ComRxInQueue.queue[uartx]);
}
/*
*********************************************************************************************************
*	�� �� ��: API_Uartx_Op
*	����˵��: ����uartio��2���ܿ���
*	��    ��: op 0����  1����
*             uartx 0-5  ��Ӧ����
*	�� �� ֵ: ��
*	��    ע����
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
*	�� �� ��: API_Uartx_Init
*	����˵��: ���ڳ�ʼ������
*	��    ��: *Uartx�����ڵ�ʵ�������ַ
*             Uart_INIT_Type *init����ʼ���Ľṹ��ָ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
__weak void API_Uartx_Init(UARTx_Type *Uartx, Uart_INIT_Type *init)
{
    RCC_ClocksType RCC_Clocks;
    volatile uint8_t tmp08;

    RCC_PERCLK_SetableEx(UARTCOMCLK, ENABLE); /* UART0~5����Ĵ���ʱ��ʹ�� */

    tmp08 = ((uint32_t)Uartx - UART0_BASE) >> 5; /* ��ȡuart */

    switch (tmp08)
    {
    case 0:
        RCC_PERCLK_SetableEx(UART0CLK, ENABLE); /* UARTxʱ��ʹ�� */
        /* UART0 IO ���� */
        AltFunIO(GPIOF, GPIO_Pin_3, 2); //PF3 UART0 RX
        AltFunIO(GPIOF, GPIO_Pin_4, 0); //PF4 UART0 TX
        /*NVIC�ж�����*/
        NVIC_SET(UART0_IRQn, init->Nvicpry);
        break;

    case 1:
        RCC_PERCLK_SetableEx(UART1CLK, ENABLE); /* UARTxʱ��ʹ�� */
        /* UART1 IO ���� */
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
        /*NVIC�ж�����*/
        NVIC_SET(UART1_IRQn, init->Nvicpry);
        break;

    case 2:
        RCC_PERCLK_SetableEx(UART2CLK, ENABLE); /* UARTxʱ��ʹ�� */
        /* UART2 IO ���� */
        AltFunIO(GPIOB, GPIO_Pin_2, 2); /* PB2 UART2 RX */
        AltFunIO(GPIOB, GPIO_Pin_3, 0); /* PB3 UART2 TX */

        /*NVIC�ж�����*/
        NVIC_SET(UART2_IRQn, init->Nvicpry);
        break;

    case 3:
        RCC_PERCLK_SetableEx(UART3CLK, ENABLE); /* UARTxʱ��ʹ�� */
        /* UART3 IO ���� */
        AltFunIO(GPIOC, GPIO_Pin_10, 2); /* PC10 UART3 RX */
        AltFunIO(GPIOC, GPIO_Pin_11, 0); /* PC11 UART3 TX */
        /*NVIC�ж�����*/
        NVIC_SET(UART3_IRQn, init->Nvicpry);
        break;

    case 4:
        RCC_PERCLK_SetableEx(UART4CLK, ENABLE); /* UARTxʱ��ʹ�� */
        /* UART4 IO ���� */
        AltFunIO(GPIOD, GPIO_Pin_0, 2); /* PD0 UART4 RX */
        AltFunIO(GPIOD, GPIO_Pin_1, 0); /* PD1 UART4 TX */
        /*NVIC�ж�����*/
        NVIC_SET(UART4_IRQn, init->Nvicpry);
        break;

    case 5:
        RCC_PERCLK_SetableEx(UART5CLK, ENABLE); /* UARTxʱ��ʹ�� */
        /* UART5 IO ���� */
        AltFunIO(GPIOC, GPIO_Pin_4, 2); /* PC4 UART5 RX */
        AltFunIO(GPIOC, GPIO_Pin_5, 0); /* PC5 UART5 TX */
        /*NVIC�ж�����*/
        NVIC_SET(UART5_IRQn, init->Nvicpry);
        break;

    default:
        break;
    }

    RCC_GetClocksFreq(&RCC_Clocks);                                 /* ��ȡAPBʱ��Ƶ�� */
    UART_SInit(Uartx, &init->uartint, RCC_Clocks.APBCLK_Frequency); /* ��ʼ��uart */

    UARTx_RXSTA_RXEN_Setable(Uartx, ENABLE); /* �򿪽���ʹ�� */
    UARTx_TXSTA_TXEN_Setable(Uartx, ENABLE); /* �򿪷���ʹ�� */

    if (init->RxIEEN == ENABLE)
        UART_UARTIE_RxTxIE_SetableEx(Uartx, RxInt, ENABLE); /* �򿪽����ж� */
    else
        UART_UARTIE_RxTxIE_SetableEx(Uartx, RxInt, DISABLE); /* �رս����ж� */

    if (init->TxIEEN == ENABLE)
        UART_UARTIE_RxTxIE_SetableEx(Uartx, TxInt, ENABLE); /* �򿪷����ж� */
    else
        UART_UARTIE_RxTxIE_SetableEx(Uartx, TxInt, DISABLE); /* �رշ����ж� */

    if (init->IRMod == USEIRMod)
    {
        UART_IRModulation_Init(38000, 50, RCC_Clocks.APBCLK_Frequency);
        UARTx_TXSTA_IREN_Setable(Uartx, ENABLE); /* ���ͺ������ʹ��λ */
    }
}
/*
*********************************************************************************************************
*	�� �� ��: API_Uart_Send
*	����˵��: ���ڷ��ͺ���
*	��    ��: *Uartx�����ڵ�ʵ�������ַ
*             *str�����ͻ���ĵ�ַָ��
*             len�����͵ĳ���
*	�� �� ֵ: ��
*	��    ע����APIû���޶����ȣ�ע������Խ��(��ѯ����)
*********************************************************************************************************
*/
__weak void API_Uart_Send(UARTx_Type *UARTx, uint8_t *str, uint16_t len)
{
    uint16_t i = 0, j = 0;
#if UseBootFixFun == 0
    /* �����̻��Ķ��Ǻ����ڲ�����ȫ�ֱ����ģ�������ʹ�ù̻������޷�ʹ�øú��������жϷ��� */
    if (ENABLE == UART_UARTIE_RxTxIE_GetableEx(UARTx, TxInt))
    {
        /* ���˴����жϷ��� */
        volatile uint8_t tmp08;
        tmp08 = ((uint32_t)UARTx - UART0_BASE) >> 5; /* ��ȡuart */
        API_UART_It_Send(tmp08, str, len);
    }
    else
#endif
    {
        /*��ѯ���ͷ�ʽ*/
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
*	�� �� ��: API_UART_SEND
*	����˵��: ���ڷ��ͺ���
*	��    ��: uartx��0-5 ���ں�
*             *data�����ͻ���ĵ�ַָ��
*             len�����͵ĳ���
*	�� �� ֵ: ��
*	��    ע����APIû���޶����ȣ�ע������Խ��(��ѯ����)��ֱ��ͨ�����ںŽ��з��͵�api
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
