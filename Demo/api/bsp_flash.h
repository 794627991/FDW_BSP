/*
*********************************************************************************************************
*
*	模块名称 : FLASH程序
*	文件名称 : bsp_flash.h
*	版    本 : V1.0.0
*	说    明 : FLASH相关操作及远程升级
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-12-31  高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_def.h"

#define FLASHOPKEY 0x12ABF00F
#define FlashEraseSize 0x200 /* 扇区块大小 */
#define UpgradeStartCmd 0x10 /* 开始升级命令 */
#define UpgradeDataCmd 0x20  /* 升级包命令 */
#define UpgradeSucCmd 0x30   /* 升级成功命令 */
#define UnKonwnCmd 0x0A      /* 未知错误 */
#define CheckSumError 0x0B   /* 校验错误 */
#define GiveUpCmd 0xCC       /* 放弃升级 */
#define MeterNum 4           /* 远程升级中表号的长度 */

#ifndef FlashAllSize
#define FlashAllSize 0x40000
#endif

#ifndef BOOTENDADR
#define BOOTENDADR 0X10000
#endif

#ifndef APP1ENDADR
#define APP1ENDADR 0X28000
#endif

#define FlashStartAdr (BOOTENDADR - FlashEraseSize)    /* 在BOOT区，用于存储当前运行的APP的起始地址 */
#define BOOTSIZE (BOOTENDADR / FlashEraseSize)         /* BOOT区扇区块大小  */
#define APPDIVSIZE (APP1ENDADR / FlashEraseSize)       /* APP分界扇区块大小,即 APP1ENDADR/0x200 APPMAXDIVSIZE-APPDIVSIZE 就是APP2扇区块大小 APPDIVSIZE - BOOTSIZE 就是 APP1扇区块大小*/
#define APP2ENDADR (FlashAllSize - FlashEraseSize * 2) /* 全部可用APP块大小(包含BOOT区大小)，因为Flash的最后两个扇区留作备用 */
#define APPMAXDIVSIZE (APP2ENDADR / FlashEraseSize)    /* 全部可用扇区块大小 */

    typedef struct
    {
        uint8_t FlashBlock;   /* Flash块区---大小64K */
        uint8_t FlashPageAdr; /* Flash页面地址0-0xff */
        uint32_t FlashPage;   /* Flash页面---大小256字节 */
    } Flashstruct;

    typedef struct
    {
        uint8_t VER;       /* 版本号0-0xff */
        uint8_t UpCount;   /* 升级失败错误计数器 */
        uint32_t TotalNum; /* 总包数 */
        uint32_t DivNum;   /* 分包数 */
    } Upstruct;
    extern Upstruct UPstr;

    typedef struct
    {
        unsigned long int FarmStartAdd; /* Fram起始地址 */
        unsigned long int BootStartAdd; /* Boot起始地址 */
        unsigned long int VectorAdd;    /* 中断向量起始地址 */
        unsigned long int FarmEndAdd;   /* Fram结束地址 */
    } FlashBFstruct;

    /**************************函数指针定义**************************/
    typedef uint8_t (*gy_updata)(uint8_t *buf, uint16_t len, Upstruct *UpStc);

    /****************************函数声明****************************/
    void API_Erase_Sector(uint32_t Adr);
    uint8_t API_UpData(uint8_t *buf, uint16_t len, Upstruct *UpStc);
    uint8_t Flash_Erase_Sector(uint16_t SectorNum, uint32_t OperateKey);
    uint8_t Flsah_Write_String(uint32 prog_addr, uint8_t *prog_data, uint16_t Len, uint32_t OperateKey);

#ifdef __cplusplus
}
#endif

#endif
