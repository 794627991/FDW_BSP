/*
*********************************************************************************************************
*
*	ģ������ : ucosiii��س���
*	�ļ����� : ucosinit.c
*	��    �� : V1.0.0
*	˵    �� : ucosiii��ʼ�����������õȣ�����malloc.c�еĺ����������ض���printf
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-1-6    ����    ��ʽ����
*		V1.0.0  2020-1-7    ����    �޸�ucosiii��ʹ�õ͹��ĵ�����£���ͳ����������ʱ����������⡣
*                                   ԭ�����ڵ��� OSStatTaskCPUUsageInit(&err) ʱ���н���������
*                                   ����ڿ��������������� if (TaskTempTCB.StkBasePtr == NULL)������
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "ucosinit.h"

#if Use_uCOS > 0

uCOS_CLKType uCOS_Clk;

OS_TCB TaskTempTCB;  /* ��ʱ����TCB�����ظ�ʹ�� */
OS_TCB TaskIOTCB;    /* IO����TCB */
OS_TCB TaskClockTCB; /* ʱ������TCB */

static void uCOS_TaskStart(void *p_arg);
static void uCOS_TaskClock(void *p_arg);
static void uCOS_TaskIO(void *p_arg);
static void uCOS_TaskCreate(void);
static void uCOS_EventCreate(void);

#if UseUart0 > 0
OS_TCB TaskUart0TCB;
static void uCOS_TaskUart0(void *p_arg);
#endif

#if UseUart1 > 0
OS_TCB TaskUart1TCB;
static void uCOS_TaskUart1(void *p_arg);
#endif

#if UseUart2 > 0
OS_TCB TaskUart2TCB;
static void uCOS_TaskUart2(void *p_arg);
#endif

#if UseUart3 > 0
OS_TCB TaskUart3TCB;
static void uCOS_TaskUart3(void *p_arg);
#endif

#if UseUart4 > 0
OS_Q uCOS_Uart4Mbox;
static void uCOS_TaskUart4(void *p_arg);
OS_TCB TaskUart4TCB;
#endif

#if UseUart5 > 0
OS_TCB TaskUart5TCB;
static void uCOS_TaskUart5(void *p_arg);
#endif

void HardFault_Handler(void)
{
    OS_ERR os_err;
    OSSchedLock(&os_err); /* ��ֹ������� */
    printf("Ӳ������\r\n");
    while (1)
        ;
}

/*
*********************************************************************************************************
*	�� �� ��: CPU_TS_TmrRd
*	����˵��: ʹ��ucosiii����ĺ���
*	��    ��: ��
*	�� �� ֵ: CPU������
*	��    ע����
*********************************************************************************************************
*/
CPU_TS_TMR CPU_TS_TmrRd(void)
{
    CPU_TS_TMR ts_tmr_cnts;

#if Use32BitEtim > 0
    ts_tmr_cnts = (uint32_t)(ETCNT->ET2CNT * 0x10000) + ETCNT->ET1CNT; /* ���ڻ�ȡCPUʹ���� */
#else
    ts_tmr_cnts = 0u;
#endif
    return (ts_tmr_cnts);
}
/*
*********************************************************************************************************
*	�� �� ��: CPU_TS_TmrInit
*	����˵��: ʹ��ucosiii����ĺ���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void CPU_TS_TmrInit(void)
{
}
/*
*********************************************************************************************************
*	�� �� ��: MyTaskCreate
*	����˵��: ���񴴽�
*	��    ��: *p_tcb������TCBָ��
*             *p_name��������
*             p_task��ʵ��Ҫִ�е�������ָ��
*             prio���������ȼ�
*             stk_size�����������Ķ�ջ��С stk_size * 4 �ֽ�
*	�� �� ֵ: ��
*	��    ע��p_task ��Ҫ���� OS_CFG_PRIO_MAX �ö����� os_cfg.h ��
*********************************************************************************************************
*/
void MyTaskCreate(OS_TCB *p_tcb,
                  CPU_CHAR *p_name,
                  OS_TASK_PTR p_task,
                  OS_PRIO prio,
                  CPU_STK_SIZE stk_size)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    if (p_tcb->StkBasePtr == NULL) /* ˵����û����������ջ��ַ���Խ������� */
    {
        CPU_STK *p_stk_base = (CPU_STK *)mymallocpro(sizeof(CPU_STK) * stk_size);
        if (p_stk_base == NULL)
        {
            printf("\r\n��������%s�ѿռ䲻��\r\n", p_name);
        }
        else
        {
            OSTaskCreate(p_tcb, /* Create the start task */
                         p_name,
                         p_task,
                         (void *)0,
                         prio,
                         (CPU_STK *)&p_stk_base[0],
                         (CPU_STK_SIZE)stk_size / 10,
                         (CPU_STK_SIZE)stk_size,
                         (OS_MSG_QTY)0,
                         (OS_TICK)0,
                         (void *)0,
                         (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                         (OS_ERR *)&err);
            printf("\r\n��������%s�ɹ�\r\n", p_name);
        }
    }
    else
    {
        printf("\r\n����%s�Ѵ���\r\n", p_name);
    }

    CPU_CRITICAL_EXIT();
}
/*
*********************************************************************************************************
*	�� �� ��: MyTaskDel
*	����˵��: ����ɾ��
*	��    ��: *p_tcb������TCBָ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void MyTaskDel(OS_TCB *p_tcb)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
#if OS_CFG_DBG_EN > 0
    char *name = p_tcb->NamePtr;
    printf("\r\nɾ������%s�ѳɹ�\r\n", name);
#endif

    CPU_STK *TempStk = p_tcb->StkBasePtr;
    OSTaskDel(p_tcb, &err); /* ɾ���Լ� */
    myfree(TempStk);
    CPU_CRITICAL_EXIT();
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_Start
*	����˵��: ����ucosiii�������
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����ʹ��ϵͳ�����������б�����õĺ��������ú󣬾ͻῪ���������
*********************************************************************************************************
*/
void uCOS_Start(void)
{
    OS_ERR err;
    CPU_Init();
    OSInit(&err);
    OS_AppIdleTaskHookPtr = App_TaskIdleHook; /* ��������ָ���ʼ�� */

    //    OSTaskCreate((OS_TCB *)&TaskStartTCB, /* Create the start task */
    //                 (CPU_CHAR *)"TaskStart",
    //                 (OS_TASK_PTR)uCOS_TaskStart,
    //                 (void *)0,
    //                 (OS_PRIO)uCOS_TaskStart_PRIO,
    //                 (CPU_STK *)&uCOS_TaskStart_Stk[0],
    //                 (CPU_STK_SIZE)uCOS_TaskStart_STK_SIZE / 10,
    //                 (CPU_STK_SIZE)uCOS_TaskStart_STK_SIZE,
    //                 (OS_MSG_QTY)0,
    //                 (OS_TICK)0,
    //                 (void *)0,
    //                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
    //                 (OS_ERR *)&err);

    MyTaskCreate((OS_TCB *)&TaskTempTCB,
                 (CPU_CHAR *)"TaskStart",
                 (OS_TASK_PTR)uCOS_TaskStart,
                 (OS_PRIO)uCOS_TaskStart_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskStart_STK_SIZE);

    OSStart(&err); /* Start multitasking (i.e. give control to uC/OS-II). */
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_TaskStart
*	����˵��: ��������(��ʱ����)
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����������������������ṩ���û�������ʼ���ĺ�����uCOS_SystemInit���û�ȥд
*             ������Ĭ�϶�ջ��С uCOS_TaskStart_STK_SIZE 256 ������������� ucosinit.h �޸�
*             ������ע����ջ�����ĺ���������������������
*********************************************************************************************************
*/
static void uCOS_TaskStart(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;

    OS_CPU_SysTickInit(SYSTICKPRE);

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err); /* ͳ������ */
#endif
#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset(); /* �����ж�ʱ��Ĺ��ܣ��궨����cpu_cfg.h�� */
#endif
    uCOS_EventCreate();
    uCOS_TaskCreate();
    uCOS_SystemInit();

    MyTaskDel(&TaskTempTCB);

    //OSTaskDel(&TaskStartTCB, &err); /* ɾ���Լ� */
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_SystemInit
*	����˵��: ��Ҫ����ʱд�ĺ���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
__weak void uCOS_SystemInit(void)
{
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_TaskCreate
*	����˵��: ��������ĺ���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��һ�������������У�ʱ������IO�����񣬿��ܴ����������У�����0-5
*********************************************************************************************************
*/
static void uCOS_TaskCreate(void)
{
    uint8_t os_err;
    os_err = os_err;

    MyTaskCreate((OS_TCB *)&TaskClockTCB,
                 (CPU_CHAR *)"TaskClock",
                 (OS_TASK_PTR)uCOS_TaskClock,
                 (OS_PRIO)uCOS_TaskClock_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskClock_STK_SIZE);

    MyTaskCreate((OS_TCB *)&TaskIOTCB,
                 (CPU_CHAR *)"TaskIO",
                 (OS_TASK_PTR)uCOS_TaskIO,
                 (OS_PRIO)uCOS_TaskIO_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskIO_SIZE);

#if UseUart0 > 0

    MyTaskCreate((OS_TCB *)&TaskUart0TCB,
                 (CPU_CHAR *)"TaskUart0",
                 (OS_TASK_PTR)uCOS_TaskUart0,
                 (OS_PRIO)uCOS_TaskUart0_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskUart0_STK_SIZE);

#endif

#if UseUart1 > 0

    MyTaskCreate((OS_TCB *)&TaskUart1TCB,
                 (CPU_CHAR *)"TaskUart1",
                 (OS_TASK_PTR)uCOS_TaskUart1,
                 (OS_PRIO)uCOS_TaskUart1_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskUart1_STK_SIZE);
#endif

#if UseUart2 > 0

    MyTaskCreate((OS_TCB *)&TaskUart2TCB,
                 (CPU_CHAR *)"TaskUart2",
                 (OS_TASK_PTR)uCOS_TaskUart2,
                 (OS_PRIO)uCOS_TaskUart2_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskUart2_STK_SIZE);
#endif

#if UseUart3 > 0

    MyTaskCreate((OS_TCB *)&TaskUart3TCB,
                 (CPU_CHAR *)"TaskUart3",
                 (OS_TASK_PTR)uCOS_TaskUart3,
                 (OS_PRIO)uCOS_TaskUart3_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskUart3_STK_SIZE);
#endif

#if UseUart4 > 0

    MyTaskCreate((OS_TCB *)&TaskUart4TCB,
                 (CPU_CHAR *)"TaskUart4",
                 (OS_TASK_PTR)uCOS_TaskUart4,
                 (OS_PRIO)uCOS_TaskUart4_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskUart4_STK_SIZE);
#endif

#if UseUart5 > 0

    MyTaskCreate((OS_TCB *)&TaskUart5TCB,
                 (CPU_CHAR *)"TaskUart5",
                 (OS_TASK_PTR)uCOS_TaskUart5,
                 (OS_PRIO)uCOS_TaskUart5_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskUart5_STK_SIZE);
#endif
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_EventCreate
*	����˵��: ������Ϣ����ȣ����ú���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
static void uCOS_EventCreate(void)
{
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_TaskClock
*	����˵��: ʱ������(��פ����)
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��������RTC�жϺ͵͹��Ķ�ʱ���ж�
*             uCOS_APP_1s��uCOS_APP_1m,uCOS_APP_1h,uCOS_APP_1d,uCOS_APP_Lptim��Ҫ�û���д
*             ������Ĭ�϶�ջ��С uCOS_TaskClock_STK_SIZE 256 ������������� ucosinit.h �޸�
*             ������ע����ջ�����ĺ���ܵ��������˳�������
*********************************************************************************************************
*/
static void uCOS_TaskClock(void *p_arg)
{
    uCOS_CLKType *msg;
    OS_MSG_SIZE msg_size;
    OS_ERR os_err;
    (void)p_arg;
    os_err = os_err;

    while (1)
    {
        msg = (uCOS_CLKType *)OSTaskQPend((OS_TICK)0,
                                          (OS_OPT)OS_OPT_PEND_BLOCKING,
                                          (OS_MSG_SIZE *)&msg_size,
                                          (CPU_TS *)0,
                                          (OS_ERR *)&os_err);
        if (msg->rtc_sec)
        {
            msg->rtc_sec = 0;
            uCOS_APP_1s();
        }
        if (msg->rtc_min)
        {
            msg->rtc_min = 0;
            uCOS_APP_1m();
        }
        if (msg->rtc_hour)
        {
            msg->rtc_hour = 0;
            uCOS_APP_1h();
        }
        if (msg->rtc_day)
        {
            msg->rtc_day = 0;
            uCOS_APP_1d();
        }
        if (msg->lptim)
        {
            msg->lptim = 0;
            uCOS_APP_Lptim();
        }
    }
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_APP_1x,uCOS_APP_Lptim
*	����˵��: ��Ҫ�û�д�ĺ���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��������
*********************************************************************************************************
*/
__weak void uCOS_APP_1s(void)
{
}

__weak void uCOS_APP_1m(void)
{
}

__weak void uCOS_APP_1h(void)
{
}

__weak void uCOS_APP_1d(void)
{
}

__weak void uCOS_APP_Lptim(void)
{
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_TaskIO
*	����˵��: IO����(��פ����)
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��������IO�ж�
*             uCOS_APP_IO��Ҫ�û���д
*             ������Ĭ�϶�ջ��С uCOS_TaskIO_SIZE 128 ������������� ucosinit.h �޸�
*             ������ע����ջ�����ĺ���ܵ��������˳�������
*********************************************************************************************************
*/
static void uCOS_TaskIO(void *p_arg)
{
    (void)p_arg;

    while (1)
    {
        OS_ERR os_err;
        OS_MSG_SIZE msg_size;
        OSTaskQPend((OS_TICK)0,
                    (OS_OPT)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)&msg_size,
                    (CPU_TS *)0,
                    (OS_ERR *)&os_err);

        uCOS_APP_IO();
    }
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_APP_IO
*	����˵��: ��Ҫ�û�д�ĺ���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��������
*********************************************************************************************************
*/
__weak void uCOS_APP_IO(void)
{
}
/*
*********************************************************************************************************
*	�� �� ��: App_TaskIdleHook
*	����˵��: �͹��Ĺ��Ӻ���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��ʵ��Ϊ�����������ｫ����������Ϊ���Խ���͹��ĵĿ���������
*********************************************************************************************************
*/
void App_TaskIdleHook(void)
{
    OS_ERR os_err;
    OSSchedLock(&os_err);               /* ��ֹ������� */
    if (TaskTempTCB.StkBasePtr == NULL) /* �������񲻴��ڵ��ȣ�ֱ�ӿ��ܸ�ͳ���������CPU������ */
    {
        uCOS_LowPower();
    }
    IWDT_Clr();
    OS_CPU_SysTickInit(SYSTICKPRE); /* systick���³�ʼ�� �ǳ���Ҫ�ģ������ܷ� */
    OSSchedUnlock(&os_err);         /* ����������� */
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_LowPower
*	����˵��: ��Ҫ�û�д�ĺ���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע��������ν���͹��ģ�����ʲô���ĵ͹���ģʽ����û��������������͹��ģ���������д
*********************************************************************************************************
*/
__weak void uCOS_LowPower(void)
{
}

#if UseUart0 > 0
/*
*********************************************************************************************************
*	�� �� ��: uCOS_TaskUartx
*	����˵��: ��������(��פ����)
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע�����������ж�
*             uCOS_APP_Uartx ��Ҫ�û���д
*             ������Ĭ�϶�ջ��С uCOS_TaskUartx_STK_SIZE 128 ������������� ucosinit.h �޸�
*             ������ע����ջ�����ĺ���ܵ��������˳�������
*********************************************************************************************************
*/
static void uCOS_TaskUart0(void *p_arg)
{
    while (1)
    {
        OS_ERR os_err;
        OS_MSG_SIZE msg_size;
        OSTaskQPend((OS_TICK)0,
                    (OS_OPT)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)&msg_size,
                    (CPU_TS *)0,
                    (OS_ERR *)&os_err);

        API_UART_Rx_Queue_Get_Do(0, uCOS_APP_Uart0);
        // ComData *dat = GetComInfo(0);
        // if (dat != NULL)
        // {
        //     uCOS_APP_Uart0(dat->pData, dat->len);
        //     FreeComInfo(0);
        // }
    }
}
/*
*********************************************************************************************************
*	�� �� ��: uCOS_APP_Uartx
*	����˵��: ����x���ݴ���
*	��    ��: *buf�����ݻ���
*             len������
*	�� �� ֵ: ��
*	��    ע�������������Լ�д
*********************************************************************************************************
*/
__weak void uCOS_APP_Uart0(uint8_t *buf, uint16_t len)
{
}
#endif

#if UseUart1 > 0
static void uCOS_TaskUart1(void *p_arg)
{
    while (1)
    {
        OS_ERR os_err;
        OS_MSG_SIZE msg_size;
        OSTaskQPend((OS_TICK)0,
                    (OS_OPT)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)&msg_size,
                    (CPU_TS *)0,
                    (OS_ERR *)&os_err);

        API_UART_Rx_Queue_Get_Do(1, uCOS_APP_Uart1);
        // ComData *dat = GetComInfo(1);
        // if (dat != NULL)
        // {
        //     uCOS_APP_Uart1(dat->pData, dat->len);
        //     FreeComInfo(1);
        // }
    }
}

__weak void uCOS_APP_Uart1(uint8_t *buf, uint16_t len)
{
}
#endif

#if UseUart2 > 0
static void uCOS_TaskUart2(void *p_arg)
{
    while (1)
    {
        OS_ERR os_err;
        OS_MSG_SIZE msg_size;
        OSTaskQPend((OS_TICK)0,
                    (OS_OPT)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)&msg_size,
                    (CPU_TS *)0,
                    (OS_ERR *)&os_err);

        API_UART_Rx_Queue_Get_Do(2, uCOS_APP_Uart2);
        // ComData *dat = GetComInfo(2);
        // if (dat != NULL)
        // {
        //     uCOS_APP_Uart2(dat->pData, dat->len);
        //     FreeComInfo(2);
        // }
    }
}

__weak void uCOS_APP_Uart2(uint8_t *buf, uint16_t len)
{
}
#endif

#if UseUart3 > 0
static void uCOS_TaskUart3(void *p_arg)
{
    while (1)
    {
        OS_ERR os_err;
        OS_MSG_SIZE msg_size;
        OSTaskQPend((OS_TICK)0,
                    (OS_OPT)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)&msg_size,
                    (CPU_TS *)0,
                    (OS_ERR *)&os_err);

        API_UART_Rx_Queue_Get_Do(3, uCOS_APP_Uart3);
        // ComData *dat = GetComInfo(3);
        // if (dat != NULL)
        // {
        //     uCOS_APP_Uart3(dat->pData, dat->len);
        //     FreeComInfo(3);
        // }
    }
}

__weak void uCOS_APP_Uart3(uint8_t *buf, uint16_t len)
{
}
#endif

#if UseUart4 > 0
static void uCOS_TaskUart4(void *p_arg)
{
    while (1)
    {
        OS_ERR os_err;
        OS_MSG_SIZE msg_size;
        OSTaskQPend((OS_TICK)0,
                    (OS_OPT)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)&msg_size,
                    (CPU_TS *)0,
                    (OS_ERR *)&os_err);

        API_UART_Rx_Queue_Get_Do(4, uCOS_APP_Uart4);
        // ComData *dat = GetComInfo(4);
        // if (dat != NULL)
        // {
        //     uCOS_APP_Uart4(dat->pData, dat->len);
        //     FreeComInfo(4);
        // }
    }
}

__weak void uCOS_APP_Uart4(uint8_t *buf, uint16_t len)
{
}
#endif

#if UseUart5 > 0
static void uCOS_TaskUart5(void *p_arg)
{
    while (1)
    {
        /*��ͳ��ʽ*/
        /*uint8_t *msg;
        OS_ERR os_err;
        OS_MSG_SIZE msg_size;
        msg = (uint8_t *)OSTaskQPend((OS_TICK)0,
                                     (OS_OPT)OS_OPT_PEND_BLOCKING,
                                     (OS_MSG_SIZE *)&msg_size,
                                     (CPU_TS *)0,
                                     (OS_ERR *)&os_err);
        uCOS_APP_Uart5(msg, msg_size);*/

        OS_ERR os_err;
        OS_MSG_SIZE msg_size;
        OSTaskQPend((OS_TICK)0,
                    (OS_OPT)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)&msg_size,
                    (CPU_TS *)0,
                    (OS_ERR *)&os_err);

        API_UART_Rx_Queue_Get_Do(5, uCOS_APP_Uart5);

        // ComData *dat = GetComInfo(5);
        // if (dat != NULL)
        // {
        //     uCOS_APP_Uart5(dat->pData, dat->len);
        //     FreeComInfo(5);
        // }
    }
}

__weak void uCOS_APP_Uart5(uint8_t *buf, uint16_t len)
{
}
#endif

#endif
