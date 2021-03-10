/*
*********************************************************************************************************
*
*	模块名称 : LPUART_BSP程序
*	文件名称 : bsp_lpuart.c
*	版    本 : V1.0.0
*	说    明 : 低功耗串口初始化和查询发送
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "bsp_lpuart.h"

#if API_UseLPUart > 0
/*
*********************************************************************************************************
*	函 数 名: API_LPUart_Init
*	功能说明: 低功串口初始化
*	形    参: LPUart_INIT_Type *init：LPUart_INIT_Type型结构体指针
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void API_LPUart_Init(LPUart_INIT_Type *init)
{
    RCC_PERCLK_SetableEx(LPUFCKEN, ENABLE);   /* LPUART功能时钟使能 */
    RCC_PERCLK_SetableEx(LPUARTCKEN, ENABLE); /* LPUART寄存器总线时钟使能 */

    /* LPUART IO 配置 */
    GPIO_PF4AFSEL_PF4AFS_Set(GPIO_PF4AFSEL_PF4AFS_LPUART_TX); /* PF4选择LPUART_TX */

    AltFunIO(GPIOF, GPIO_Pin_4, 0); /* PF4 LPUART TX */
    AltFunIO(GPIOF, GPIO_Pin_3, 2); /* PF3 LPUART RX 除匹配接收假如外部没有上拉电阻建议配置上拉电阻 */

    /* NVIC中断配置 */
    NVIC_SET(LPUART_IRQn, init->Nvicpry);

    LPUART_SInit(&init->uartint); /* 初始化uart(接收中断使用的是接收数据匹配) */

    LPUART_LPUEN_RXEN_Setable(ENABLE); /* 打开接收使能 */
    LPUART_LPUEN_TXEN_Setable(ENABLE); /* 打开发送使能 */

    if (init->RxIEEN != DISABLE)
        LPUART_LPUCON_RXIE_Setable(ENABLE); /* 使能接收中断 */
    else
        LPUART_LPUCON_RXIE_Setable(DISABLE); /* 关闭接收中断 */

    LPUART_LPUCON_TCIE_Setable(DISABLE); /* 关闭发送中断，低功耗定时器禁止中断发 */
}
/*
*********************************************************************************************************
*	函 数 名: API_LPUart_Send
*	功能说明: 低功串口查询发
*	形    参: *str：要发送的数据缓存
*             len：发送的长度
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void API_LPUart_Send(uint8_t *str, uint16_t len)
{
    uint16_t i = 0, j = 0;

    /* 清除发送中断标志 */
    LPUART_LPUIF_TXIF_Clr();
    LPUART_LPUIF_TC_IF_Clr();

    for (i = 0; i < len; i++)
    {
        LPUART_LPUTXD_Write(str[i]); /* 将发送数据写入发送寄存器 */
        while (j < 0x5555)           /* 等待发送完成 */
        {
            if (0 == LPUART_LPUIF_TC_IF_Chk())
            {
                j++;
            }
            else
            {
                j = 0;
                break;
            }
        }
        LPUART_LPUIF_TC_IF_Clr(); /* 清除发送中断标志 */
        if (j >= 0x5554)
        {
            i = len; /* 超时了 */
        }
    }
}

#endif
