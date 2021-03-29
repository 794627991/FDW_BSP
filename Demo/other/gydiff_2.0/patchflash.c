/*
*********************************************************************************************************
*
*	模块名称 : 差分升级硬件相关
*	文件名称 : patchflash.c
*	版    本 : V1.0.0
*	说    明 : 文件格式：boot+patch+app boot分区在 0-PATCH_START patch分区 在 PATCH_START-PATCH_END
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021.3.26   高屹    随时修改
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "patchflash.h"
#include "bsp_flash.h"
#include "gylzo.h"

const uint32_t FLASH_BLOCK = FlashEraseSize;
const uint32_t FLASH_END = APP2ENDADR;
const uint32_t PATCH_START = PATCHADR;
const uint32_t PATCH_END = BOOTENDADR;
static uint32_t OLDADR, NEWADR, OLDSIZE, NEWSIZE;

typedef struct _patchdownload
{
    uint16_t crc;
    uint16_t num;
    uint32_t len;
    uint32_t offset;
} DownLoadType;

DownLoadType patchdownload;

#define SECTORCNT(size) ((size / FlashEraseSize) + 1)

uint8_t judgeSize(int64_t oldsize, int64_t newsize)
{
    if (oldsize < 0 || newsize < 0)
        return 0;
    /* 如果两个文件大小加一起比 app区还大，肯定不能行 */
    if (oldsize + newsize > FLASH_END - PATCH_END)
        return 0;
    memcpy(&OLDADR, (unsigned char *)FlashStartAdr, 4);
    /* new最后一定是要放在old原位置的，要确保old到flash末尾的大小要大于newsize */
    if (FLASH_END - OLDADR < newsize)
        return 0;
    return 1;
}

void patchGotoBoot(void)
{
    char magicword[7] = {"GYCFSJ"};
    FlashMakeSureBoot();
    Flsah_Write_String(FlashStartAdr + 8, (uint8_t *)&magicword, 6, FLASHOPKEY);
    RCC->SOFTRST = 0x5c5caabb; /* 测试先把这个屏蔽 */
}

void patchGotoAPP(void)
{
    uint32_t WrADR;
    memcpy(&WrADR, (unsigned char *)FlashStartAdr, 4); /* 获取旧文件的地址 */
    API_Erase_Sector(FlashStartAdr);
    Flsah_Write_String(FlashStartAdr, (unsigned char *)&WrADR, 4, FLASHOPKEY);
}

uint8_t patchFlashErase(uint32_t start, uint32_t size)
{
    uint16_t i = 0, j = 0;
    uint32_t *PFlash = NULL, adr;
    /* 擦除boot区内，保留的pathc块大小 */
    for (i = 0; i < size; i++)
    {
        IWDT_Clr();
        adr = (uint32_t)(start + 0x200 * i);
        if (adr < PATCH_START || adr > FLASH_END)
            return 0; /* 要擦除的扇区超限 */
        PFlash = (uint32_t *)adr;

        RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash擦写控制器时钟使能，用完就关 */
        FLASH_Erase_Sector((uint32_t)PFlash);
        RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
        for (j = 0; j < 128; j++)
        {
            if (PFlash[j] != 0xFFFFFFFF)
            {
                return 0;
            }
        }
    }
    return 1;
}

/* 0:获取old文件 1:获取new文件 2:获取patch文件 */
uint32_t patchGetAdr(uint8_t op, uint32_t *oldlen, uint32_t *newlen)
{
    if (op == 0)
    {
        /* 获取旧文件地址 */
        memcpy(&OLDADR, (unsigned char *)FlashStartAdr, 4);
        return OLDADR;
    }
    else if (op == 1)
    {
        /* 获取new文件地址 */
        OLDSIZE = *oldlen;
        NEWSIZE = *newlen;
        if (OLDADR > (SECTORCNT(NEWSIZE) + 1) * FLASH_BLOCK + PATCH_END)
        {
            /* old 文件地址到 boot 结尾的 Flash 大小就足以放入 new文件 */
            NEWADR = PATCH_END;
        }
        else
        {
            /* 在 old 文件尾添加 new */
            if (OLDSIZE > NEWSIZE)
            {
                NEWADR = OLDSIZE;
            }
            else
            {
                NEWADR = NEWSIZE;
            }
            NEWADR /= FLASH_BLOCK;
            NEWADR += 2; /* 预留出两个扇区 */
            NEWADR *= FLASH_BLOCK;
            NEWADR += PATCH_END;
            if (NEWADR + NEWSIZE > FLASH_END)
            {
                /* 在 old 文件后加 new 的长度超过了 flash 的大小 */
                return 0;
            }
        }
        patchFlashErase(NEWADR, SECTORCNT(NEWSIZE));
        return NEWADR;
    }
    else if (op == 2)
    {
        /* 获取patch文件地址 */
        return PATCH_START;
    }
    else
    {
        return 0;
    }
}

void patchFlashStart(void)
{
    RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash擦写控制器时钟使能，用完就关 */
    FLASH_EPCON_PREQ_Set(FLASH_EPCON_PREQ_PROG);
    FLASH_FLSKEY_Write(flash_PROG_key0);
    FLASH_FLSKEY_Write(flash_PROG_key1);
}

void patchFlashAddData(uint8_t *buf, uint8_t *data, uint8_t op)
{
    uint8_t dd = *data;
    if (op == 0)
    {
        dd += *buf;
    }
    FLASH_EPCON_PREQ_Set(FLASH_EPCON_PREQ_PROG);
    *buf = dd;
    while (RESET == FLASH_FLSIF_PRDIF_Chk())
    {
        __NOP();
    }
    FLASH_FLSIF_PRDIF_Clr();
}

void patchFlashEnd(void)
{
    FLASH_FLSKEY_Write(0x00000000);
    RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
}

void patchFlashWDog(uint32_t cnt)
{
    if ((cnt % 1000) == 0)
        IWDT_Clr();
}

uint8_t patchFlashWrite(uint8_t *buf, uint8_t *data, uint32_t Len)
{
    volatile uint32_t i = 0;
    uint8_t *membuf = buf, *memdat = data;
    patchFlashStart();
    for (i = 0; i < Len; i++)
    {
        patchFlashWDog(i);
        patchFlashAddData(buf, data, 1);
        buf++;
        data++;
    }
    patchFlashEnd();
    for (i = 0; i < Len; i++)
    {
        if (membuf[i] != memdat[i])
        {
            return 0;
        }
    }
    return 1;
}

uint8_t patchAddDiff(bspatchtype *bspatch, int64_t newpos, int64_t oldpos, int64_t len)
{
    uint32_t Divisor = newpos / FLASH_BLOCK;
    uint32_t remainder = newpos % FLASH_BLOCK;
    uint32_t LEN = (uint32_t)len;
    uint32_t i = 0, j, cnt, k = 0;
    uint8_t *buf = NULL, *oldbuf = (uint8_t *)bspatch->OldAdr;
    if ((buf = (uint8_t *)mymalloc(513)) == NULL)
        return 0;
    while (LEN)
    {
        if (remainder)
        {
            cnt = 512 - remainder;
            if (cnt > LEN)
            {
                cnt = LEN;
            }
        }
        else
        {
            if (LEN > 512)
            {
                cnt = 512;
            }
            else
            {
                cnt = LEN;
            }
        }
        LEN -= cnt;

        /* 把 newfile 写入缓存 */
        memcpy(buf, (uint8_t *)(bspatch->NewAdr + (Divisor + i) * FLASH_BLOCK), 512);
        patchFlashErase(bspatch->NewAdr + (Divisor + i) * FLASH_BLOCK, 1);
        for (j = 0; j < cnt; j++)
        {
            if ((oldpos + k >= 0) && (oldpos + k < bspatch->OldSize))
            {
                buf[j + remainder] += oldbuf[oldpos + k];
                k++;
            }
            else
            {
                myfree(buf);
                return 0;
            }
        }
        if (patchFlashWrite((uint8_t *)bspatch->NewAdr + (Divisor + i) * FLASH_BLOCK, buf, 512) == 0)
        {
            myfree(buf);
            return 0;
        }
        i++;
        remainder = 0;
    }
    myfree(buf);
    return 1;
}
/*
*********************************************************************************************************
*	函 数 名: patchUpGrade
*	功能说明: 差分包升级
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void patchUpGrade(void)
{
    char magicword[8];
    uint8_t i = 0;
    OLDADR = 0;
    NEWADR = 0;
    OLDSIZE = 0;
    NEWSIZE = 0;
    memset(magicword, 0, 8);
    memcpy(magicword, (unsigned char *)FlashStartAdr + 8, 6);
    /* Check for appropriate magic */
    if (memcmp(magicword, "GYCFSJ", 6) == 0)
    {
        if (patch())
        {
            /* 能走到这里说明差分升级的新文件已经合成完毕 */
            /* 擦除 old 区,按照 NEWSIZE 进行擦除 */
            for (i = 0; i < 3; i++)
            {
                patchFlashErase(OLDADR, SECTORCNT(NEWSIZE));
                /* 复制 new 到 old 验证后跳转*/
                patchFlashWrite((uint8_t *)OLDADR, (uint8_t *)NEWADR, NEWSIZE);
                if (camper((uint8_t *)OLDADR, (uint8_t *)NEWADR, NEWSIZE))
                {
                    patchGotoAPP();
                    return;
                }
            }
            /* 如果走到这，说明变砖了，基本不可能 */
            API_Erase_Sector(FlashStartAdr);
        }
    }
}
/*
*********************************************************************************************************
*	函 数 名: patchDownLoad
*	功能说明: 下载差分包
*	形    参: buf：数据缓存
*             len：数据长度  
*	返 回 值: 0:失败,1:成功
*	备    注：协议：'g'+cmd(1b)+datalen(2b)+data(datalen)+crc(2b)+'y' 
*                   crc='g'+cmd+datalen+data
*                   cmd=0:开始下载 data: patchlen(4b)+patchcrc(2b) 
*                   cmd=1:数据包 data: packnum(2b)+data(max=512)
*                   cmd=2:下载完成 data: "gycfsjsucc"
*                   成功应答：cmd=3 data="gycfsjsucc"
*                   失败应答：cmd=4 data="gycfsjfail"
*********************************************************************************************************
*/
uint8_t patchDownLoad(uint8_t *buf, uint16_t *len)
{
    uint16_t crc1 = 0, crc2 = 0, datalen = 0, num = 0;
    uint8_t *data, cmd;

    if (buf[0] != 'g')
        return 0;
    memcpy(&datalen, buf + 2, 2);
    if ((datalen + 7) > *len || datalen < 2)
        return 0;

    if (buf[datalen + 6] != 'y')
        return 0;

    memcpy(&crc1, buf + 4 + datalen, 2);
    crc2 = LzoCRC(buf, 4 + datalen);
    if (crc2 != crc1)
        return 0;

    data = buf + 4;
    cmd = buf[1];
    if (cmd == 0)
    {
        memset(&patchdownload, 0, sizeof(patchdownload));
        memcpy(&patchdownload.len, data, 4);
        memcpy(&patchdownload.crc, data + 4, 2);
        patchdownload.num = 0x8000;
        if (patchdownload.len > PATCH_END - PATCH_START)
            return 0;
        /* 接收到开始接收差分包命令后擦除 patch 分区 */
        if (patchFlashErase(PATCH_START, (PATCH_END - PATCH_START) / FLASH_BLOCK) == 0)
        {
            return 0;
        }
    }
    else if (cmd == 1)
    {
        /* 接收到数据包后写入 patch 分区 */
        if ((patchdownload.num & 0x8000) != 0x8000)
            return 0;
        memcpy(&num, data, 2);
        /* 必须保证下一包数据的包号比记录的包号大 */
        if ((patchdownload.num & 0x7fff) > num)
            return 0;
        patchdownload.num = 0x8000 + num;
        patchFlashWrite((uint8_t *)(PATCH_START + patchdownload.offset), data + 2, datalen - 2);
        patchdownload.offset += (datalen - 2);
    }
    else if (cmd == 2)
    {
        /* 接收到完成包后校验并跳转 boot 升级 */
        /* 如果升级数据正确，最后偏移数应该和长度相符 */
        if (patchdownload.offset != patchdownload.len)
            return 0;
        crc2 = LzoCRC((uint8_t *)PATCH_START, patchdownload.len);
        if (crc2 != patchdownload.crc)
            return 0;
        patchGotoBoot();
    }
    return 1;
}

void patchDownLoadAnswer(uint8_t *buf, uint16_t *len)
{
    uint16_t tlen, crc;
    if (patchDownLoad(buf, len))
    {
        buf[0] = 'g';
        buf[1] = 3;
        tlen = strlen("gycfsjsucc");
        memcpy(buf + 2, &tlen, 2);
        memcpy(buf + 4, "gycfsjsucc", tlen);
        crc = LzoCRC(buf, 4 + tlen);
        memcpy(buf + 4 + tlen, &crc, 2);
        buf[tlen + 6] = 'y';
        *len = tlen + 7;
    }
    else
    {
        buf[0] = 'g';
        buf[1] = 4;
        tlen = strlen("gycfsjfail");
        memcpy(buf + 2, &tlen, 2);
        memcpy(buf + 4, "gycfsjfail", tlen);
        crc = LzoCRC(buf, 4 + tlen);
        memcpy(buf + 4 + tlen, &crc, 2);
        buf[tlen + 6] = 'y';
        *len = tlen + 7;
    }
}
