/*
*********************************************************************************************************
*
*	ģ������ : LPUART_BSP����
*	�ļ����� : bsp_lpuart.c
*	��    �� : V1.0.0
*	˵    �� : �͹��Ĵ��ڳ�ʼ���Ͳ�ѯ����
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "bsp_lpuart.h"

#if API_UseLPUart > 0
/*
*********************************************************************************************************
*	�� �� ��: API_LPUart_Init
*	����˵��: �͹����ڳ�ʼ��
*	��    ��: LPUart_INIT_Type *init��LPUart_INIT_Type�ͽṹ��ָ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
__weak void API_LPUart_Init(LPUart_INIT_Type *init)
{
    RCC_PERCLK_SetableEx(LPUFCKEN, ENABLE);   /* LPUART����ʱ��ʹ�� */
    RCC_PERCLK_SetableEx(LPUARTCKEN, ENABLE); /* LPUART�Ĵ�������ʱ��ʹ�� */

    /* LPUART IO ���� */
    GPIO_PF4AFSEL_PF4AFS_Set(GPIO_PF4AFSEL_PF4AFS_LPUART_TX); /* PF4ѡ��LPUART_TX */

    AltFunIO(GPIOF, GPIO_Pin_4, 0); /* PF4 LPUART TX */
    AltFunIO(GPIOF, GPIO_Pin_3, 2); /* PF3 LPUART RX ��ƥ����ռ����ⲿû���������轨�������������� */

    /* NVIC�ж����� */
    NVIC_SET(LPUART_IRQn, init->Nvicpry);

    LPUART_SInit(&init->uartint); /* ��ʼ��uart(�����ж�ʹ�õ��ǽ�������ƥ��) */

    LPUART_LPUEN_RXEN_Setable(ENABLE); /* �򿪽���ʹ�� */
    LPUART_LPUEN_TXEN_Setable(ENABLE); /* �򿪷���ʹ�� */

    if (init->RxIEEN != DISABLE)
        LPUART_LPUCON_RXIE_Setable(ENABLE); /* ʹ�ܽ����ж� */
    else
        LPUART_LPUCON_RXIE_Setable(DISABLE); /* �رս����ж� */

    LPUART_LPUCON_TCIE_Setable(DISABLE); /* �رշ����жϣ��͹��Ķ�ʱ����ֹ�жϷ� */
}
/*
*********************************************************************************************************
*	�� �� ��: API_LPUart_Send
*	����˵��: �͹����ڲ�ѯ��
*	��    ��: *str��Ҫ���͵����ݻ���
*             len�����͵ĳ���
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
__weak void API_LPUart_Send(uint8_t *str, uint16_t len)
{
    uint16_t i = 0, j = 0;

    /* ��������жϱ�־ */
    LPUART_LPUIF_TXIF_Clr();
    LPUART_LPUIF_TC_IF_Clr();

    for (i = 0; i < len; i++)
    {
        LPUART_LPUTXD_Write(str[i]); /* ����������д�뷢�ͼĴ��� */
        while (j < 0x5555)           /* �ȴ�������� */
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
        LPUART_LPUIF_TC_IF_Clr(); /* ��������жϱ�־ */
        if (j >= 0x5554)
        {
            i = len; /* ��ʱ�� */
        }
    }
}

#endif
