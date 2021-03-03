/*
*********************************************************************************************************
*
*	ģ������ : LPTIM_BSP����
*	�ļ����� : lptim_api.c
*	��    �� : V1.0.0
*	˵    �� : �͹��Ķ�ʱ����ʼ��
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "lptim_api.h"

#if API_UseLPTIM > 0
/*
*********************************************************************************************************
*	�� �� ��: API_LPTIM_Start
*	����˵��: �͹��Ķ�ʱ������
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void API_LPTIM_Start(void)
{
    LPTIM_LPTIE_OVIE_Setable(DISABLE);    /* ʹ��LPTIMER������ж� */
    LPTIM_LPTCTRL_LPTEN_Setable(DISABLE); /* LPTIMERģ��ʹ�� */
    LPTIM_LPTIE_OVIE_Setable(ENABLE);     /* ʹ��LPTIMER������ж� */
    LPTIM_LPTCTRL_LPTEN_Setable(ENABLE);  /* LPTIMERģ��ʹ�� */
}
/*
*********************************************************************************************************
*	�� �� ��: API_LPTIM_Stop
*	����˵��: �͹��Ķ�ʱ���ر�
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void API_LPTIM_Stop(void)
{
    LPTIM_LPTCTRL_LPTEN_Setable(DISABLE); /* LPTIMERģ��ر� */
}
/*
*********************************************************************************************************
*	�� �� ��: API_LPTIM_Init
*	����˵��: �͹��Ķ�ʱ����ʼ��
*	��    ��: LPTIM_INIT_Type *LPTIM_INIT_Type
*	�� �� ֵ: ��
*	��    ע��������LPTIM�󲻻�����������ʱ��,��Ҫ�ֶ�����
*********************************************************************************************************
*/
__weak void API_LPTIM_Init(LPTIM_INIT_Type *init)
{
    LPTIM_InitTypeDef init_para;

    RCC_PERCLK_SetableEx(LPTFCLK, ENABLE); /* LPTIM����ʱ��ʹ�� */
    RCC_PERCLK_SetableEx(LPTRCLK, ENABLE); /* LPTIM����ʱ��ʹ�� */

    init_para.LPTIM_TMODE = LPTIM_LPTCFG_TMODE_PWMIM;      /* ���ù���ģʽΪ�������������ͨ��ʱ��ģʽ */
    init_para.LPTIM_MODE = LPTIM_LPTCFG_MODE_CONTINUE;     /* ���ü���ģʽΪ��������ģʽ */
    init_para.LPTIM_PWM = LPTIM_LPTCFG_PWM_PWM;            /* ѡ�����������Ʒ�ʽ */
    init_para.LPTIM_POLAR = LPTIM_LPTCFG_POLARITY_POS;     /* ��һ�μ���ֵ=�Ƚ�ֵ�ǲ��������� */
    init_para.LPTIM_TRIG_CFG = LPTIM_LPTCFG_TRIGCFG_POS;   /* �ⲿ�����ź�������trigger */
    init_para.LPTIM_FLTEN = ENABLE;                        /* ʹ�������˲� */
    init_para.LPTIM_CLK_SEL = LPTIM_LPTCFG_CLKSEL_LSCLK;   /* ѡ��LPTIMER��ʱ��Դ */
    init_para.LPTIM_LPTIN_EDGE = LPTIM_LPTCFG_EDGESEL_POS; /* LPTIN�������ؼ��� */

    if (init->PRE < 8)
    {
        init_para.LPTIM_CLK_DIV = (uint32_t)init->PRE << LPTIM_LPTCFG_DIVSEL_Pos; /* ���÷�Ƶֵ */
    }
    else
    {
        init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_128;
    }

    // switch (init->PRE)
    // {
    // case 0:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_1; //���÷�Ƶֵ
    //     break;
    // case 1:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_2; //���÷�Ƶֵ
    //     break;
    // case 2:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_4; //���÷�Ƶֵ
    //     break;
    // case 3:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_8; //���÷�Ƶֵ
    //     break;
    // case 4:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_16; //���÷�Ƶֵ
    //     break;
    // case 5:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_32; //���÷�Ƶֵ
    //     break;
    // case 6:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_64; //���÷�Ƶֵ
    //     break;
    // case 7:
    // default:
    //     init_para.LPTIM_CLK_DIV = LPTIM_LPTCFG_DIVSEL_128; //���÷�Ƶֵ
    //     break;
    // }

    init_para.LPTIM_compare_value = 0;          /* ���ñȽ�ֵ */
    init_para.LPTIM_target_value = init->count; /* ����Ŀ��ֵ */

    LPTIM_Init(&init_para);              /* ��ʼ��LPTIMER */
    NVIC_SET(LPTIM_IRQn, init->Nvicpry); /* ʹ��LPTIMER�������ж� */
    LPTIM_LPTIE_OVIE_Setable(ENABLE);    /* ʹ��LPTIMER������ж� */
}

#endif
