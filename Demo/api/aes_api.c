/*
*********************************************************************************************************
*
*	ģ������ : AES_BSP����
*	�ļ����� : aes_api.c
*	��    �� : V1.0.0
*	˵    �� : Ӳ��AESĿǰֻ֧��128��256
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "aes_api.h"

#if API_UseAES > 0

/*
*********************************************************************************************************
*	�� �� ��: API_AES
*	����˵��: Ӳ��AES��API  ���鿴API_Exp��AES������
*	��    ��: AES_Init_Type�ͽṹ�����  ���忴��Ա
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
__weak void API_AES(AES_Init_Type *init)
{
    AES_InitTypeDef init_para;

    /* ��ģ��ʱ�� */
    RCC_PERCLK_SetableEx(AESCLK, ENABLE);
    AES_AESCR_AESEN_Setable(DISABLE); /* ����AESģ�� */

    /* AES������Կ���ȣ�AESEN=1ʱ�����޸� */
    if (init->len == AES_192Bit)
        /*init_para.KEYLEN = AES_AESCR_KEYLEN_192BIT;*/
        return; /* �Ȳ�֧��192 */
    else if (init->len == AES_256Bit)
        init_para.KEYLEN = AES_AESCR_KEYLEN_256BIT;
    else
        init_para.KEYLEN = AES_AESCR_KEYLEN_128BIT;

    if (init->mode != AESCTRMODE)
        init_para.CHMOD = AES_AESCR_CHMOD_ECB; /* AES����������ģʽ��AESEN=1ʱ�����޸� */
    else
        init_para.CHMOD = AES_AESCR_CHMOD_CTR;

    if (init->control == AESENCRYPT)
        init_para.MODE = AES_AESCR_MODE_ENCRYPT; /* AES����ģʽ��AESEN=1ʱ�����޸� */
    else
        init_para.MODE = AES_AESCR_MODE_KEYEXPDECRYPT; /* AES����ģʽ��AESEN=1ʱ�����޸�//����ʱ��Ҫ����չ��Կ������ֱ��ʹ�á���Կ��չ+���ܡ�ģʽ */

    init_para.DATATYP = AES_AESCR_DATATYP_32BITNOEX; /* ѡ���������ͣ�AESEN=1ʱ�����޸ġ����彻������ɲο�AES���������½� */
    init_para.DMAOEN = DISABLE;                      /* DMA�����Զ�����ʹ�� */
    init_para.DMAIEN = DISABLE;                      /* DMA�����Զ�д��ʹ�� */
    init_para.ERRIE = DISABLE;                       /* �����־��RDERR��WRERR���ж�ʹ�� */
    init_para.CCFIE = DISABLE;                       /* CCF��־�ж�ʹ�� */
    init_para.AESEN = DISABLE;                       /* AESʹ�� */

    AES_Init(&init_para); /* ����AES */

    if (init->len == AES_256Bit)
        AES_AESKEY_WriteEx(init->key, 32); /* ������Կ */
    else
        AES_AESKEY_WriteEx(init->key, 16); /* ������Կ */

    AES_AESCR_AESEN_Setable(ENABLE); /* ����AES */

    /* �ӽ������� */
    if (init->len == AES_256Bit)
    {
        AES_GroupWriteAndRead_128BIT(init->in, init->out);
        AES_GroupWriteAndRead_128BIT(init->in + 16, init->out + 16);
    }
    else
    {
        AES_GroupWriteAndRead_128BIT(init->in, init->out);
    }

    AES_AESCR_AESEN_Setable(DISABLE); /* �ر�AESģ�� */
}

#endif
