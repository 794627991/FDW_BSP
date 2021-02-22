/*
*********************************************************************************************************
*
*	模块名称 : AES_BSP程序
*	文件名称 : aes_api.c
*	版    本 : V1.0.0
*	说    明 : 硬件AES目前只支持128和256
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-12-31  高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "aes_api.h"

#if API_UseAES > 0

/*
*********************************************************************************************************
*	函 数 名: API_AES
*	功能说明: 硬件AES的API  详情看API_Exp中AES的例程
*	形    参: AES_Init_Type型结构体变量  具体看成员
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void API_AES(AES_Init_Type *init)
{
    AES_InitTypeDef init_para;

    /* 打开模块时钟 */
    RCC_PERCLK_SetableEx(AESCLK, ENABLE);
    AES_AESCR_AESEN_Setable(DISABLE); /* 重置AES模块 */

    /* AES加密密钥长度，AESEN=1时不可修改 */
    if (init->len == AES_192Bit)
        /*init_para.KEYLEN = AES_AESCR_KEYLEN_192BIT;*/
        return; /* 先不支持192 */
    else if (init->len == AES_256Bit)
        init_para.KEYLEN = AES_AESCR_KEYLEN_256BIT;
    else
        init_para.KEYLEN = AES_AESCR_KEYLEN_128BIT;

    if (init->mode != AESCTRMODE)
        init_para.CHMOD = AES_AESCR_CHMOD_ECB; /* AES数据流处理模式，AESEN=1时不可修改 */
    else
        init_para.CHMOD = AES_AESCR_CHMOD_CTR;

    if (init->control == AESENCRYPT)
        init_para.MODE = AES_AESCR_MODE_ENCRYPT; /* AES工作模式，AESEN=1时不可修改 */
    else
        init_para.MODE = AES_AESCR_MODE_KEYEXPDECRYPT; /* AES工作模式，AESEN=1时不可修改//解密时需要先扩展密钥，或者直接使用“密钥扩展+解密”模式 */

    init_para.DATATYP = AES_AESCR_DATATYP_32BITNOEX; /* 选择数据类型，AESEN=1时不可修改。具体交换规则可参考AES数据类型章节 */
    init_para.DMAOEN = DISABLE;                      /* DMA数据自动读出使能 */
    init_para.DMAIEN = DISABLE;                      /* DMA数据自动写入使能 */
    init_para.ERRIE = DISABLE;                       /* 错误标志（RDERR和WRERR）中断使能 */
    init_para.CCFIE = DISABLE;                       /* CCF标志中断使能 */
    init_para.AESEN = DISABLE;                       /* AES使能 */

    AES_Init(&init_para); /* 配置AES */

    if (init->len == AES_256Bit)
        AES_AESKEY_WriteEx(init->key, 32); /* 设置密钥 */
    else
        AES_AESKEY_WriteEx(init->key, 16); /* 设置密钥 */

    AES_AESCR_AESEN_Setable(ENABLE); /* 启动AES */

    /* 加解密数据 */
    if (init->len == AES_256Bit)
    {
        AES_GroupWriteAndRead_128BIT(init->in, init->out);
        AES_GroupWriteAndRead_128BIT(init->in + 16, init->out + 16);
    }
    else
    {
        AES_GroupWriteAndRead_128BIT(init->in, init->out);
    }

    AES_AESCR_AESEN_Setable(DISABLE); /* 关闭AES模块 */
}

#endif
