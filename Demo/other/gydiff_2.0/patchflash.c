#include "patchflash.h"
#include "bsp_flash.h"

const uint32_t APP1_START = BOOTENDADR;
const uint32_t APP2_START = APP1ENDADR;
const uint32_t PATCH_START = PATCHADR;

void patchGotoBoot(void)
{
    FlashMakeSureBoot();
    RCC->SOFTRST = 0x5c5caabb;
}

uint8_t patchFlashErase(void)
{
    uint16_t i = 0;
    uint32_t *PFlash = NULL;
    /* 擦除boot区内，保留的pathc块大小 */
    for (i = 0; i < BOOTSIZE - PATCHDIVSIZE; i++)
    {
        IWDT_Clr();
        if ((uint32_t)(0x200 * i) > APP2ENDADR) /* 要擦除的扇区超限 */
            return 0;
        PFlash = (uint32_t *)(uint32_t)(PATCHADR + 0x200 * i);

        RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash擦写控制器时钟使能，用完就关 */
        FLASH_Erase_Sector((uint32_t)PFlash);
        RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
        for (i = 0; i < 128; i++)
        {
            if (PFlash[i] != 0xFFFFFFFF)
            {
                return 0;
            }
        }
    }
    return 1;
}

/* 0:获取旧文件 1:获取新文件 2:获取patch文件 */
uint8_t *patchGetAdr(uint8_t op, uint32_t *len)
{
    *len = *len;
    if (op == 0)
    {
        /* 获取旧文件地址 */
        if (SCB->VTOR != BOOTENDADR)
        {
            *len = APP2ENDADR - APP1ENDADR;
            return (uint8_t *)APP2_START;
        }
        *len = APP1ENDADR - BOOTENDADR;
        return (uint8_t *)APP1_START;
    }
    else if (op == 1)
    {
        /* 获取新文件地址 */
        if (SCB->VTOR != BOOTENDADR)
        {
            *len = APP1ENDADR - BOOTENDADR;
            return (uint8_t *)APP2_START;
        }
        *len = APP2ENDADR - APP1ENDADR;
        return (uint8_t *)APP1_START;
    }
    else if (op == 2)
    {
        return (uint8_t *)PATCH_START;
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
    FLASH_EPCON_PREQ_Set(FLASH_EPCON_PREQ_PROG);
    (op) ? (*buf = *data) : (*buf += *data);
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
    uint32_t i = 0;
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
        if (buf[i] != data[i])
        {
            return 0;
        }
    }
    return 1;
}
