/*
*********************************************************************************************************
*
*	模块名称 : ucosiii相关程序
*	文件名称 : ucosinit.c
*	版    本 : V1.0.0
*	说    明 : ucosiii初始化，任务配置等，调用malloc.c中的函数，调用重定向printf
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-1-6    高屹    正式发布
*		V1.0.0  2020-1-7    高屹    修复ucosiii在使用低功耗的情况下，有统计任务启动时间过长的问题。
*                                   原因在于调用 OSStatTaskCPUUsageInit(&err) 时会切进空闲任务，
*                                   因此在空闲任务钩子中增加 if (TaskTempTCB.StkBasePtr == NULL)的条件
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "ucosinit.h"

#if Use_uCOS > 0

uCOS_CLKType uCOS_Clk;

OS_TCB TaskTempTCB;  /* 临时任务TCB，可重复使用 */
OS_TCB TaskIOTCB;    /* IO任务TCB */
OS_TCB TaskClockTCB; /* 时钟任务TCB */

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
    OSSchedLock(&os_err); /* 禁止任务调度 */
    printf("硬件错误\r\n");
    while (1)
        ;
}

/*
*********************************************************************************************************
*	函 数 名: CPU_TS_TmrRd
*	功能说明: 使用ucosiii必须的函数
*	形    参: 无
*	返 回 值: CPU利用率
*	备    注：无
*********************************************************************************************************
*/
CPU_TS_TMR CPU_TS_TmrRd(void)
{
    CPU_TS_TMR ts_tmr_cnts;

#if Use32BitEtim > 0
    ts_tmr_cnts = (uint32_t)(ETCNT->ET2CNT * 0x10000) + ETCNT->ET1CNT; /* 用于获取CPU使用率 */
#else
    ts_tmr_cnts = 0u;
#endif
    return (ts_tmr_cnts);
}
/*
*********************************************************************************************************
*	函 数 名: CPU_TS_TmrInit
*	功能说明: 使用ucosiii必须的函数
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void CPU_TS_TmrInit(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: MyTaskCreate
*	功能说明: 任务创建
*	形    参: *p_tcb：任务TCB指针
*             *p_name：任务名
*             p_task：实际要执行的任务函数指针
*             prio：任务优先级
*             stk_size：给任务分配的堆栈大小 stk_size * 4 字节
*	返 回 值: 无
*	备    注：p_task 不要大于 OS_CFG_PRIO_MAX 该定义在 os_cfg.h 中
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
    if (p_tcb->StkBasePtr == NULL) /* 说明还没给任务分配堆栈地址可以建立任务 */
    {
        CPU_STK *p_stk_base = (CPU_STK *)mymallocpro(sizeof(CPU_STK) * stk_size);
        if (p_stk_base == NULL)
        {
            printf("\r\n创建任务%s堆空间不足\r\n", p_name);
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
            printf("\r\n创建任务%s成功\r\n", p_name);
        }
    }
    else
    {
        printf("\r\n任务%s已存在\r\n", p_name);
    }

    CPU_CRITICAL_EXIT();
}
/*
*********************************************************************************************************
*	函 数 名: MyTaskDel
*	功能说明: 任务删除
*	形    参: *p_tcb：任务TCB指针
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void MyTaskDel(OS_TCB *p_tcb)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
#if OS_CFG_DBG_EN > 0
    char *name = p_tcb->NamePtr;
    printf("\r\n删除任务%s堆成功\r\n", name);
#endif

    CPU_STK *TempStk = p_tcb->StkBasePtr;
    OSTaskDel(p_tcb, &err); /* 删除自己 */
    myfree(TempStk);
    CPU_CRITICAL_EXIT();
}
/*
*********************************************************************************************************
*	函 数 名: uCOS_Start
*	功能说明: 开启ucosiii任务调度
*	形    参: 无
*	返 回 值: 无
*	备    注：若使用系统，在主函数中必须调用的函数，调用后，就会开启任务调度
*********************************************************************************************************
*/
void uCOS_Start(void)
{
    OS_ERR err;
    CPU_Init();
    OSInit(&err);
    OS_AppIdleTaskHookPtr = App_TaskIdleHook; /* 空闲任务指针初始化 */

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
*	函 数 名: uCOS_TaskStart
*	功能说明: 启动任务(临时任务)
*	形    参: 略
*	返 回 值: 无
*	备    注：该任务负责建立其他任务和提供给用户用来初始化的函数，uCOS_SystemInit由用户去写
*             该任务默认堆栈大小 uCOS_TaskStart_STK_SIZE 256 如果不够自行在 ucosinit.h 修改
*             ！！！注：堆栈不够的后果是跑完启动任务就死机
*********************************************************************************************************
*/
static void uCOS_TaskStart(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;

    OS_CPU_SysTickInit(SYSTICKPRE);

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err); /* 统计任务 */
#endif
#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset(); /* 测量中断时间的功能，宏定义在cpu_cfg.h中 */
#endif
    uCOS_EventCreate();
    uCOS_TaskCreate();
    uCOS_SystemInit();

    MyTaskDel(&TaskTempTCB);

    //OSTaskDel(&TaskStartTCB, &err); /* 删除自己 */
}
/*
*********************************************************************************************************
*	函 数 名: uCOS_SystemInit
*	功能说明: 需要调用时写的函数
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void uCOS_SystemInit(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: uCOS_TaskCreate
*	功能说明: 创建任务的函数
*	形    参: 无
*	返 回 值: 无
*	备    注：一定创建的任务有：时钟任务，IO口任务，可能创建的任务有：串口0-5
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
*	函 数 名: uCOS_EventCreate
*	功能说明: 创建消息邮箱等，备用函数
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
static void uCOS_EventCreate(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: uCOS_TaskClock
*	功能说明: 时钟任务(常驻任务)
*	形    参: 略
*	返 回 值: 无
*	备    注：负责处理RTC中断和低功耗定时器中断
*             uCOS_APP_1s，uCOS_APP_1m,uCOS_APP_1h,uCOS_APP_1d,uCOS_APP_Lptim需要用户编写
*             该任务默认堆栈大小 uCOS_TaskClock_STK_SIZE 256 如果不够自行在 ucosinit.h 修改
*             ！！！注：堆栈不够的后果跑到该任务退出后死机
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
*	函 数 名: uCOS_APP_1x,uCOS_APP_Lptim
*	功能说明: 需要用户写的函数
*	形    参: 无
*	返 回 值: 无
*	备    注：弱函数
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
*	函 数 名: uCOS_TaskIO
*	功能说明: IO任务(常驻任务)
*	形    参: 略
*	返 回 值: 无
*	备    注：负责处理IO中断
*             uCOS_APP_IO需要用户编写
*             该任务默认堆栈大小 uCOS_TaskIO_SIZE 128 如果不够自行在 ucosinit.h 修改
*             ！！！注：堆栈不够的后果跑到该任务退出后死机
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
*	函 数 名: uCOS_APP_IO
*	功能说明: 需要用户写的函数
*	形    参: 无
*	返 回 值: 无
*	备    注：弱函数
*********************************************************************************************************
*/
__weak void uCOS_APP_IO(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: App_TaskIdleHook
*	功能说明: 低功耗钩子函数
*	形    参: 无
*	返 回 值: 无
*	备    注：实际为空闲任务，这里将空闲任务当作为可以进入低功耗的可能性任务
*********************************************************************************************************
*/
void App_TaskIdleHook(void)
{
    OS_ERR os_err;
    OSSchedLock(&os_err);               /* 禁止任务调度 */
    if (TaskTempTCB.StkBasePtr == NULL) /* 启动任务不存在调度，直接空跑给统计任务计算CPU利用率 */
    {
        uCOS_LowPower();
    }
    IWDT_Clr();
    OS_CPU_SysTickInit(SYSTICKPRE); /* systick重新初始化 非常重要的，否则跑飞 */
    OSSchedUnlock(&os_err);         /* 允许任务调度 */
}
/*
*********************************************************************************************************
*	函 数 名: uCOS_LowPower
*	功能说明: 需要用户写的函数
*	形    参: 无
*	返 回 值: 无
*	备    注：具体如何进入低功耗，进入什么样的低功耗模式，有没有其他条件进入低功耗，都在这里写
*********************************************************************************************************
*/
__weak void uCOS_LowPower(void)
{
}

#if UseUart0 > 0
/*
*********************************************************************************************************
*	函 数 名: uCOS_TaskUartx
*	功能说明: 串口任务(常驻任务)
*	形    参: 略
*	返 回 值: 无
*	备    注：负责处理串口中断
*             uCOS_APP_Uartx 需要用户编写
*             该任务默认堆栈大小 uCOS_TaskUartx_STK_SIZE 128 如果不够自行在 ucosinit.h 修改
*             ！！！注：堆栈不够的后果跑到该任务退出后死机
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
*	函 数 名: uCOS_APP_Uartx
*	功能说明: 串口x数据处理
*	形    参: *buf：数据缓存
*             len：长度
*	返 回 值: 无
*	备    注：弱函数，需自己写
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
        /*传统方式*/
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
