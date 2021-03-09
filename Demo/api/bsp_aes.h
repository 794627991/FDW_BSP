/*
*********************************************************************************************************
*
*	模块名称 : AES_BSP定义(FOR复旦微G045/G048)
*	文件名称 : bsp_aes.h
*	版    本 : V1.0.0
*	说    明 : AES相关定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-12-31  高屹  正式发布
*
*	Copyright (C), 2020-2030, 辽宁思凯-高屹
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

#define AESECBMODE 0 /* ECB模式 */
#define AESCTRMODE 1 /* CTR模式 */

#define AESENCRYPT 0 /* 加密 */
#define AESDECRYPT 1 /* 解密 */

#define AES_128Bit 0 /* AES_AESCR_KEYLEN_128BIT */
#define AES_192Bit 1 /* AES_AESCR_KEYLEN_192BIT */
#define AES_256Bit 2 /* AES_AESCR_KEYLEN_256BIT */

    typedef struct
    {
        uint8_t control; /* 加密解密 */
        uint8_t mode;    /* ECB or CTR */
        uint8_t len;     /* 加密长度选择  128 or 256 */
        uint8_t *in;     /* 输入 */
        uint8_t *key;    /* 密钥 */
        uint8_t *out;    /* 输出 */
    } AES_Init_Type;

    /**************************函数指针定义**************************/
    typedef void (*gy_aestype)(AES_Init_Type *init);
    /****************************函数声明****************************/
    void API_AES(AES_Init_Type *init);

#ifdef __cplusplus
}
#endif

#endif
