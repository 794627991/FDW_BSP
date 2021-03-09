/*
*********************************************************************************************************
*
*	ģ������ : AES_BSP����(FOR����΢G045/G048)
*	�ļ����� : bsp_aes.h
*	��    �� : V1.0.0
*	˵    �� : AES��ض���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����  ��ʽ����
*
*	Copyright (C), 2020-2030, ����˼��-����
*
*********************************************************************************************************
*/
#ifndef __BSP_AES_H__
#define __BSP_AES_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

#define AESECBMODE 0 /* ECBģʽ */
#define AESCTRMODE 1 /* CTRģʽ */

#define AESENCRYPT 0 /* ���� */
#define AESDECRYPT 1 /* ���� */

#define AES_128Bit 0 /* AES_AESCR_KEYLEN_128BIT */
#define AES_192Bit 1 /* AES_AESCR_KEYLEN_192BIT */
#define AES_256Bit 2 /* AES_AESCR_KEYLEN_256BIT */

    typedef struct
    {
        uint8_t control; /* ���ܽ��� */
        uint8_t mode;    /* ECB or CTR */
        uint8_t len;     /* ���ܳ���ѡ��  128 or 256 */
        uint8_t *in;     /* ���� */
        uint8_t *key;    /* ��Կ */
        uint8_t *out;    /* ��� */
    } AES_Init_Type;

    /**************************����ָ�붨��**************************/
    typedef void (*gy_aestype)(AES_Init_Type *init);
    /****************************��������****************************/
    void API_AES(AES_Init_Type *init);

#ifdef __cplusplus
}
#endif

#endif
