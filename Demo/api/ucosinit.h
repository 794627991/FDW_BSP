#ifndef __UCOSINIT_H__
#define __UCOSINIT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"
#include "malloc.h"
#include "bsp_uart.h"

#if Use_uCOS > 0 && ThisBoot == 0
#include "includes.h"

#ifndef Use32BitEtim
#define Use32BitEtim 1
#endif

#define SYSTICKPRE __SYSTEM_CLOCK / OS_CFG_TICK_RATE_HZ

#define uCOS_TaskStart_PRIO 1
#define uCOS_TaskStart_STK_SIZE 256

#define uCOS_TaskIO_PRIO 2
#define uCOS_TaskIO_SIZE 128

#define uCOS_TaskClock_PRIO 3
#define uCOS_TaskClock_STK_SIZE 256

#define uCOS_TaskUart0_PRIO 4
#define uCOS_TaskUart0_STK_SIZE 128

#define uCOS_TaskUart1_PRIO 4
#define uCOS_TaskUart1_STK_SIZE 128

#define uCOS_TaskUart2_PRIO 4
#define uCOS_TaskUart2_STK_SIZE 128

#define uCOS_TaskUart3_PRIO 4
#define uCOS_TaskUart3_STK_SIZE 200

#define uCOS_TaskUart4_PRIO 4
#define uCOS_TaskUart4_STK_SIZE 128

#define uCOS_TaskUart5_PRIO 4
#define uCOS_TaskUart5_STK_SIZE 128

#if Use_uCOS > 0

    extern OS_TCB TaskTempTCB;
    extern OS_TCB TaskIOTCB;
    extern OS_TCB TaskClockTCB;
    extern OS_TCB TaskUart0TCB;
    extern OS_TCB TaskUart1TCB;
    extern OS_TCB TaskUart2TCB;
    extern OS_TCB TaskUart3TCB;
    extern OS_TCB TaskUart4TCB;
    extern OS_TCB TaskUart5TCB;

    typedef union
    {
        char up;
        struct
        {
            char rtc_sec : 1;
            char rtc_min : 1;
            char rtc_hour : 1;
            char rtc_day : 1;
            char lptim : 1;
        };
    } uCOS_CLKType;
    extern uCOS_CLKType uCOS_Clk;

    void App_TaskIdleHook(void);
    void uCOS_Start(void);
    void uCOS_SystemInit(void);
    void uCOS_LowPower(void);
    void uCOS_APP_1s(void);
    void uCOS_APP_1m(void);
    void uCOS_APP_1h(void);
    void uCOS_APP_1d(void);
    void uCOS_APP_Lptim(void);
    void uCOS_APP_IO(void);
    void uCOS_APP_Uart0(uint8_t *buf, uint16_t len);
    void uCOS_APP_Uart1(uint8_t *buf, uint16_t len);
    void uCOS_APP_Uart2(uint8_t *buf, uint16_t len);
    void uCOS_APP_Uart3(uint8_t *buf, uint16_t len);
    void uCOS_APP_Uart4(uint8_t *buf, uint16_t len);
    void uCOS_APP_Uart5(uint8_t *buf, uint16_t len);

    void MyTaskCreate(OS_TCB *p_tcb,
                      CPU_CHAR *p_name,
                      OS_TASK_PTR p_task,
                      OS_PRIO prio,
                      CPU_STK_SIZE stk_size);
    void MyTaskDel(OS_TCB *p_tcb);

#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
