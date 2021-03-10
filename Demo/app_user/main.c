/*
*********************************************************************************************************
*
*	模块名称 : 主函数
*	文件名称 : main.c
*	版    本 : V0.0.0
*	说    明 : 主函数和中断回调在这里处理
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V0.0.0     无       高屹    随时修改
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "main.h"

extern void yasuotest(void);
extern void PressureAndTempTest(void);
extern iot_foo_type FOO;
extern uint8_t notintosleep;

uint8_t showemu = 0;
uint32_t sysvdd;
float systemp;
float _60ya[3], _60yw[3], _60yangle[3];

int main(void)
{
    Sysinit_Exp();
    uCOS_Start();
}

void User_IO_Init(void)
{
    OutputIO(NB_EN_GPIO, NB_EN_PIN, 0);
    NB_OFF;
    InputtIO(KEY1_GPIO, KEY1_PIN, 1); /* 如果外部电路有上拉配成0 */
    InputtIO(KEY2_GPIO, KEY2_PIN, 1); /* 如果外部电路有上拉配成0 */
    InputtIO(KEY3_GPIO, KEY3_PIN, 1); /* 如果外部电路有上拉配成0 */

    /* FM33A0XX的7组GPIO（A~G）最多可以产生24个外部引脚中断，部分IO不能同时开启中断功能 */
    GPIO_EXTI_Init(KEY1_GPIO, KEY1_PIN, EXTI_BOTH);
    GPIO_EXTI_Init(KEY2_GPIO, KEY2_PIN, EXTI_BOTH);
    GPIO_EXTI_Init(KEY3_GPIO, KEY3_PIN, EXTI_BOTH);

    /* NVIC中断配置 */
    NVIC_SET(GPIO_IRQn, 0);
}

void LowPower_IO_Init(void)
{
    /* 低功耗使用 */
    NB_OFF;
}

void uCOS_SystemInit(void)
{
    /* 系统初始化函数都放在这里 */
    LowPower_IO_Init();
    uCOS_LEDCreate();
    GUI_Init();
    printf("\r\n程序已开始运行\r\n");
    SIMLT_I2C_OP_INIT();
}

void uCOS_LowPower(void)
{
    /* 空闲任务 */
    // LowPower_IO_Init();
    // Sleep(0);
}

void UART1_IRQ_Rx_CallBack(uint8_t data)
{
    uint8_t os_err;
    static uint8_t Buf[20], LEN = 0;

    Buf[LEN] = data;
    if (LEN == 0 && Buf[0] != 0x55)
        return;

    LEN++;
    if (LEN >= 11) //接收到 11 个数据
    {
        OSTaskQPost((OS_TCB *)&TaskUart1TCB,
                    (void *)&Buf,
                    (OS_MSG_SIZE)LEN,
                    (OS_OPT)OS_OPT_POST_FIFO,
                    (OS_ERR *)&os_err);
        LEN = 0;
    }
}

void uCOS_APP_Uart1(uint8_t *buf, uint16_t len)
{
    if (buf[0] == 0x55)
    {
        //检查帧头
        switch (buf[1])
        {
        case 0x51:
            _60ya[0] = ((short)(buf[3] << 8 | buf[2])) / 32768.0 * 16;
            _60ya[1] = ((short)(buf[5] << 8 | buf[4])) / 32768.0 * 16;
            _60ya[2] = ((short)(buf[7] << 8 | buf[6])) / 32768.0 * 16;

            break;
        case 0x52:
            _60yw[0] = ((short)(buf[3] << 8 | buf[2])) / 32768.0 * 2000;
            _60yw[1] = ((short)(buf[5] << 8 | buf[4])) / 32768.0 * 2000;
            _60yw[2] = ((short)(buf[7] << 8 | buf[6])) / 32768.0 * 2000;

            break;
        case 0x53:
            _60yangle[0] = ((short)(buf[3] << 8 | buf[2])) / 32768.0 * 180;
            _60yangle[1] = ((short)(buf[5] << 8 | buf[4])) / 32768.0 * 180;
            _60yangle[2] = ((short)(buf[7] << 8 | buf[6])) / 32768.0 * 180;

            break;
        }
    }
    (abs((int)_60yw[0]) > 5) ? (LED1ON) : (LED1OFF);
    (abs((int)_60yw[1]) > 5) ? (LED2ON) : (LED2OFF);
    (abs((int)_60yw[2]) > 5) ? (LED3ON) : (LED3OFF);
}

void uCOS_APP_Uart2(uint8_t *buf, uint16_t len)
{
    iotRecDat(buf, len);
}

void uCOS_APP_Uart3(uint8_t *buf, uint16_t len)
{
    GotoBoot(buf, len);

    if (strstr((char *)buf, "coap") || strstr((char *)buf, "COAP"))
    {
        FOO.LinkMode = CoapNum;
        uCOS_NBTaskCreate();
    }
    else if (strstr((char *)buf, "tcp") || strstr((char *)buf, "TCP"))
    {
        FOO.LinkMode = TCPNum;
        uCOS_NBTaskCreate();
    }
    else if (strstr((char *)buf, "udp") || strstr((char *)buf, "UDP"))
    {
        FOO.LinkMode = UDPNum;
        uCOS_NBTaskCreate();
    }
    else
    {
        cJSON_to_cmddata((char *)buf, &cmddata);
    }
}

void uCOS_APP_1s(void)
{
    static uint8_t ss = 0;
    ss++;

    // if (ss % 2)
    // {
    //     API_LPTIM_Start();
    //     printf("lptim start\r\n");
    // }
    // else
    // {
    //     printf("lptim end\r\n");
    // }
    // printf("lptim cnt:%d,tar:%d\r\n", LPTIM_LPTCNT_Read(), LPTIM_LPTTARGET_Read());

    GETTIME;
    iotTim();

    sysvdd = ADC_Exp(CH_VDD);
    systemp = Temp_Exp();

    //printf("\r\n当前时间为：%d%d年 %d月 %d日 %d:%d:%d\r\n",Hex_YearH,Hex_YearL,Hex_Mon,Hex_Day,Hex_Hour,Hex_Min,Hex_Sec);
}

void uCOS_APP_1m(void)
{
#if OS_CFG_DBG_EN > 0
    CPU_STK_SIZE free;
    CPU_STK_SIZE used;
    OS_ERR err;

    CPU_SR_ALLOC();
    OS_TCB *p_tcb;
    CPU_CRITICAL_ENTER();
    p_tcb = OSTaskDbgListPtr;
    CPU_CRITICAL_EXIT();
    printf("\r\n已用栈(B)   剩余栈(B)   栈使用率(%%)  优先级  CPU使用率(%%)   CPU最大使用率(%%)   调用次数     任务名\r\n");
    for (;;)
    {
        if (p_tcb != (OS_TCB *)0)
        {
            OSTaskStkChk(p_tcb, &free, &used, &err);
            printf("%4d        %4d         %3d%%        %3d  %10.2f%%      %10.2f%%       %8d       %s   \r\n",
                   used, free, (used * 100) / (used + free), p_tcb->Prio,
                   p_tcb->CPUUsage / 100.0, p_tcb->CPUUsageMax / 100.0, p_tcb->CtxSwCtr, p_tcb->NamePtr);
            CPU_CRITICAL_ENTER();
            p_tcb->CPUUsageMax = 0;
            p_tcb = p_tcb->DbgNextPtr; //指向下一个任务的TCB
            CPU_CRITICAL_EXIT();
        }
        else
        {
            break;
        }
    }
#endif
    printf("memuse:%d%%", mem_perused());
    RchfAdjust(ETIM4, clkmode);
}

void uCOS_APP_1h(void)
{
}

void uCOS_APP_1d(void)
{
}

void uCOS_APP_Lptim(void)
{
    static uint32_t a = 0;
    printf("lptim:%d\r\n", ++a);
}

void uCOS_APP_IO(void)
{
    uint8_t re;

    re = API_GPIO_Judge(KEY1_GPIO, KEY1_PIN);
    if (re == 1)
    {
        /* 下降沿中断处理 */
        uCOS_NBTaskCreate();
    }

    re = API_GPIO_Judge(KEY2_GPIO, KEY2_PIN);
    if (re == 1)
    {
        /* 下降沿中断处理 */
        showemu++;
        GUI_Clear();
        if (showemu >= 4)
        {
            showemu = 0;
        }
    }

    re = API_GPIO_Judge(KEY3_GPIO, KEY3_PIN);
    if (re == 1)
    {
        /* 下降沿中断处理 */
        yasuotest();
    }
}
