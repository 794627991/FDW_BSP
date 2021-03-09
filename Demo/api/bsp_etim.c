/*
*********************************************************************************************************
*
*	ģ������ : ETIM_BSP����
*	�ļ����� : bsp_etim.c
*	��    �� : V1.0.0
*	˵    �� : ETIM��ʱģʽ��ʼ��(ʹ���жϷ�ʽ)
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "bsp_etim.h"

#if UseETIM1 > 0 || UseETIM2 > 0 || UseETIM3 > 0 || UseETIM4 > 0 || Use_UartTimeOut > 0 || ThisBoot == 1
/*
*********************************************************************************************************
*	�� �� ��: API_ETim_Start
*	����˵��: ������ʱ��
*	��    ��: *ETIMx����ʱ�������ַ
*	�� �� ֵ: ��
*	��    ע��������ETIM�󲻻�����������ʱ��,��Ҫ�ֶ�����
*********************************************************************************************************
*/
void API_ETim_Start(ETIMx_Type *ETIMx)
{
    ETIMx_ETxCR_CEN_Setable(ETIMx, ENABLE);
}
/*
*********************************************************************************************************
*	�� �� ��: API_ETim_Stop
*	����˵��: ֹͣ��ʱ��
*	��    ��: *ETIMx����ʱ�������ַ
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void API_ETim_Stop(ETIMx_Type *ETIMx)
{
    ETIMx_ETxCR_CEN_Setable(ETIMx, DISABLE);
}
/*
*********************************************************************************************************
*	�� �� ��: API_ETim_ReSet
*	����˵��: ���ö�ʱ������ֵ
*	��    ��: *ETIMx����ʱ�������ַ
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void API_ETim_ReSet(ETIMx_Type *ETIMx)
{
    ETIMx_ETxCR_CEN_Setable(ETIMx, DISABLE);
    ETIMx_ETxCR_CEN_Setable(ETIMx, ENABLE);
}
/*
*********************************************************************************************************
*	�� �� ��: API_Etim_Init
*	����˵��: ��ʱ����ʼ��
*	��    ��: *ETIMx����ʱ�������ַ
*             ETIM_INIT_Type *init����ʼ���Ľṹ��ָ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
__weak void API_Etim_Init(ETIMx_Type *ETIMx, ETIM_INIT_Type *init)
{
    ETIM_InitTypeDef init_para;
    volatile uint8_t EtimNum;
    EtimNum = ((uint32_t)ETIMx - ETIMER1_BASE) >> 5;
    init_para.sig_src_para.SIG1SEL = ETIMx_ETxINSEL_SIG1SEL_GROUP1; /* �ڲ��ź�1Դѡ��GROUP1 */

    switch (EtimNum)
    {
#if Use_uCOS > 0 && Use32BitEtim == 0 || ThisBoot == 1
    case 0:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET1_APBCLK; /* ��ʱ��1 */
        RCC_PERCLK_SetableEx(ET1CLK, ENABLE);
        NVIC_SET(ETIM1_IRQn, init->Nvicpry);
        break;
    case 1:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET2_APBCLK; /* ��ʱ��2 */
        RCC_PERCLK_SetableEx(ET2CLK, ENABLE);
        NVIC_SET(ETIM2_IRQn, init->Nvicpry);
        break;
#endif
    case 2:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET3_APBCLK; /* ��ʱ��3 */
        RCC_PERCLK_SetableEx(ET3CLK, ENABLE);
        NVIC_SET(ETIM3_IRQn, init->Nvicpry);
        break;
#if Use_uCOS > 0 && Use32BitEtim == 1 || ThisBoot == 1
    case 3:
        init_para.sig_src_para.GRP1SEL = ETIMx_ETxINSEL_GRP1SEL_ET4_APBCLK; /* ��ʱ��4 */
        RCC_PERCLK_SetableEx(ET4CLK, ENABLE);
        NVIC_SET(ETIM4_IRQn, init->Nvicpry);
        break;
#endif
    default:
        return;
    }
    if (init->PRE < 1)
        init->PRE = 1;
    init_para.sig_src_para.PRESCALE1 = clkmode + (init->PRE) - 2; /* ETxԤ��Ƶ�Ĵ���1 00��ʾ1��Ƶ��FF��ʾ256��Ƶ*/
    init_para.sig_src_para.PRESCALE2 = clkmode - 1;               /* ETxԤ��Ƶ�Ĵ���2 00��ʾ1��Ƶ��FF��ʾ256��Ƶ*/

    init_para.ctrl_para.EXFLT = ENABLE;                       /* �������������˲�ʹ�ܴ� */
    init_para.ctrl_para.MOD = ETIMx_ETxCR_MOD_COUNTER;        /* ����ģʽѡ�����ģʽ */
    init_para.ctrl_para.CASEN = DISABLE;                      /* ��չ��ʱ�������ر� */
    init_para.ctrl_para.EDGESEL = ETIMx_ETxCR_EDGESEL_RISING; /* ����ģʽ���ط�ʽѡ�񣨼���ʱ��ѡ��mcu_clkʱ��λ��Ч�����ǲ���mcu_clkʱ�������ؼ����� */
    init_para.ctrl_para.CAPCLR = DISABLE;                     /* �����㲶׽ģʽ���� */

    init_para.ctrl_para.CMPIE = DISABLE; /* ��չ��ʱ���Ƚ��ж�ʹ�� */
    init_para.ctrl_para.CAPIE = DISABLE; /* ��չ��ʱ����׽�ж�ʹ�� */
    init_para.ctrl_para.OVIE = ENABLE;   /* ��չ��ʱ������ж�ʹ�� */

    init_para.ctrl_para.INITVALUE = 0xffff - (init->count); /* ETx��ֵ�Ĵ��� */

    init_para.ctrl_para.CEN = DISABLE; /* �������� */

    ETIMx_Init(ETIMx, &init_para);
}

#endif
