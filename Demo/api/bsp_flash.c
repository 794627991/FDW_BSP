/*
*********************************************************************************************************
*
*	模块名称 : FLASH程序
*	文件名称 : bsp_flash.c
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
#include "bsp_flash.h"

/*
*********************************************************************************************************
*	函 数 名: API_Erase_Sector
*	功能说明: FLASH块擦除
*	形    参: Adr：要擦除的地址
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void API_Erase_Sector(uint32_t Adr)
{
    RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash擦写控制器时钟使能，用完就关 */
    FLASH_Erase_Sector(Adr);
    RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
}
/*
*********************************************************************************************************
*	函 数 名: Flsah_Write_String
*	功能说明: 连续写flash函数
*	形    参: prog_addr：被写flash首地址
*             prog_data：待写数据缓冲区
*             Len：写入长度(一次不要写入太多，以免看门狗溢出)
*             OperateKey  flash操作验证码(出于可靠性考虑，具体数据用户自定义，
*                         存储在ee或者flash中，操作flash前读取赋值给OperateKey)
*	返 回 值: 0：成功 1：失败
*	备    注：无
*********************************************************************************************************
*/
uint8_t Flsah_Write_String(uint32_t prog_addr, uint8_t *prog_data, uint16_t Len, uint32_t OperateKey)
{
    uint16_t i = 0;
    uint8_t Result = 0;
    uint8_t *PFlash = NULL;

    PFlash = (uint8_t *)prog_addr;
    if (OperateKey == FLASHOPKEY)
    {
        RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash擦写控制器时钟使能，用完就关 */
    }
    FLASH_Prog_ByteString(prog_addr, prog_data, Len);
    RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
    for (i = 0; i < Len; i++)
    {
        if (PFlash[i] != prog_data[i])
        {
            Result = 1;
            break;
        }
    }

    return Result;
}
/*
*********************************************************************************************************
*	函 数 名: Flash_Erase_Sector
*	功能说明: 扇区擦函数(每个扇区512字节)
*	形    参: SectorNum：需要擦除扇区的扇区号
*             OperateKey  flash操作验证码(出于可靠性考虑，具体数据用户自定义，
*                         存储在ee或者flash中，操作flash前读取赋值给OperateKey)
*	返 回 值: 0：成功 1：失败  2：错误
*	备    注：无
*********************************************************************************************************
*/
uint8_t Flash_Erase_Sector(uint16_t SectorNum, uint32_t OperateKey)
{
    uint16_t i = 0;
    uint32_t *PFlash = NULL;

    if (SectorNum < BOOTSIZE) /* 禁止擦除boot区 */
        return 2;
    if ((uint32_t)(0x200 * SectorNum) > FlashAllSize) /* 要擦除的扇区超限 */
        return 2;

    PFlash = (uint32_t *)(uint32_t)(0x200 * SectorNum);

    if (OperateKey == FLASHOPKEY)
    {
        RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash擦写控制器时钟使能，用完就关 */
    }
    FLASH_Erase_Sector((uint32_t)PFlash);
    RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
    for (i = 0; i < 128; i++)
    {
        if (PFlash[i] != 0xFFFFFFFF)
        {
            return 1;
        }
    }
    return 0;
}

/*
*********************************************************************************************************
*	函 数 名: Flash_App_Erase_Sector
*	功能说明: 擦除另一APP分区的扇区(每个扇区512字节)
*	形    参: SectorNum：需要擦除扇区的扇区号
*             OperateKey ：flash操作验证码(出于可靠性考虑，具体数据用户自定义，
*                         存储在ee或者flash中，操作flash前读取赋值给OperateKey)
*             APP_Start_Adr：当前APP所在地址
*	返 回 值: 0：成功 1：失败  2：错误
*	备    注：远程升级使用，默认APP分区成2区，当前APP所在扇区和BOOT区禁止擦除
*********************************************************************************************************
*/
uint8_t Flash_App_Erase_Sector(uint16_t SectorNum, uint32_t OperateKey, uint32_t APP_Start_Adr)
{
    uint16_t i = 0;
    uint32_t *PFlash = NULL;

    if (APP_Start_Adr < APP1ENDADR)
    {
        if (SectorNum < APPDIVSIZE) /* 禁止擦除APP当前区 */
            return 2;
    }
    else if (APP_Start_Adr < APP2ENDADR)
    {
        if ((SectorNum < BOOTSIZE) || (SectorNum > APPDIVSIZE)) /* 禁止擦除APP当前区和boot区 */
            return 2;
    }
    else
    {
        return 2; /* 读出的APP起始地址都有问题 */
    }

    if ((uint32_t)(0x200 * SectorNum) > APP2ENDADR) /* 要擦除的扇区超限 */
        return 2;
    PFlash = (uint32_t *)(uint32_t)(0x200 * SectorNum);

    if (OperateKey == FLASHOPKEY)
    {
        RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash擦写控制器时钟使能，用完就关 */
    }
    FLASH_Erase_Sector((uint32_t)PFlash);
    RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
    for (i = 0; i < 128; i++)
    {
        if (PFlash[i] != 0xFFFFFFFF)
        {
            return 1;
        }
    }

    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: FlashMakeSureBoot
*	功能说明: 保证 boot 保存的是当前 app 的运行地址
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void FlashMakeSureBoot(void)
{
    uint32_t APP_Start_Adr = 0, OffSet = 0;
    memcpy(&APP_Start_Adr, (uint8_t *)FlashStartAdr, 4); /* 从Flash读取当前运行的APP起始地址 */
    OffSet = SCB->VTOR;
    if (APP_Start_Adr != OffSet)
    {
        APP_Start_Adr = OffSet;
        API_Erase_Sector(FlashStartAdr);
        Flsah_Write_String(FlashStartAdr, (uint8_t *)&APP_Start_Adr, 4, FLASHOPKEY);
    }
}
/*
*********************************************************************************************************
*	函 数 名: FlashErase
*	功能说明: 完全擦除另一APP分区
*	形    参: 无
*	返 回 值: 0：成功 1：失败  2：错误
*	备    注：远程升级使用，默认APP分区成2区，当前APP所在扇区和BOOT区禁止擦除
*            宏定义说明：
*            FlashStartAdr：在BOOT区，用于存储当前运行的APP的起始地址
*            APP1ENDADR：APP区分1区的结束地址，由 APP_CFG.h 中定义，必须是0x200的倍数
*            APP2ENDADR：全部可用APP块大小(包含BOOT区大小)，因为Flash的最后两个扇区留作备用
*            BOOTSIZE：BOOT区扇区块大小
*            APPMAXDIVSIZE：全部可用扇区块大小
*            APPDIVSIZE：APP分界扇区块大小,即 APP1ENDADR/0x200
*                        APPMAXDIVSIZE-APPDIVSIZE 就是APP2扇区块大小
*                        APPDIVSIZE - BOOTSIZE 就是 APP1扇区块大小
*********************************************************************************************************
*/
uint8_t FlashErase(void)
{
    uint32_t APP_Start_Adr = 0;
    uint16_t i = 0;
    uint8_t Result = 0;

    if (APP_Start_Adr < APP1ENDADR)
    {
        /* 当前APP处于1区，擦除2区:APP1ENDADR-APP2ENDADR */
        for (i = 0; i < APPMAXDIVSIZE - APPDIVSIZE; i++)
        {
            IWDT_Clr();
            Result = Flash_App_Erase_Sector(APPDIVSIZE + i, FLASHOPKEY, APP_Start_Adr);
        }
    }
    else
    {
        /*当前APP处于2区，擦除1区:BOOTENDADR-APP1ENDADR*/
        for (i = 0; i < (APPDIVSIZE - BOOTSIZE); i++)
        {
            IWDT_Clr();
            Result = Flash_App_Erase_Sector(BOOTSIZE + i, FLASHOPKEY, APP_Start_Adr);
        }
    }
    return Result;
}
/*
*********************************************************************************************************
*	函 数 名: CtsFlashWrite
*	功能说明: 连续写Flash
*	形    参: WRADR：写入的地址
*             buf：要写入的数据缓存
*             len：要写的长度
*	返 回 值: 0：成功 1：失败  2：错误
*	备    注：远程升级使用，默认APP分区成2区，当前APP所在扇区和BOOT区禁止擦除
*********************************************************************************************************
*/
uint8_t CtsFlashWrite(uint32_t WRADR, uint8_t *buf, uint16_t len)
{
    uint32_t APP_Start_Adr = 0;
    uint8_t Result = 0;

    if (APP_Start_Adr < APP1ENDADR)
    {
        /*当前APP处于1区，数据只能写2区*/
        if (WRADR < APP1ENDADR)
            return 2; //禁止写APP当前区
    }
    else if (APP_Start_Adr < APP2ENDADR)
    {
        /*当前APP处于2区，数据写1区*/
        if ((WRADR < BOOTENDADR) || (WRADR > APP1ENDADR))
            return 2; //禁止写APP当前区和boot区
    }
    else
    {
        return 2; //读出的APP起始地址都有问题
    }

    Result = Flsah_Write_String(WRADR, buf, len, FLASHOPKEY);

    return Result;
}

#if API_UseDataUp > 0
Upstruct UPstr; /* 结构体定义 */
/*
*********************************************************************************************************
*	函 数 名: API_UpData
*	功能说明: 远程升级
*	形    参: buf：升级数据包
*             Upstruct *UpStc：升级结构体指针
*	返 回 值: 1：完成 0：升级过程中
*	备    注：远程升级，支持断点续传
*********************************************************************************************************
*/
__weak uint8_t API_UpData(uint8_t *buf, uint16_t len, Upstruct *UpStc)
{
    uint8_t re = 0;
    uint16_t rvlen = 0;
    uint32_t WrADR = 0;

    rvlen = buf[1] + buf[2] * 0x100;
    if (rvlen > len)
    {
        buf[3 + MeterNum] = UnKonwnCmd;
    }
    else
    {
        if (buf[rvlen - 2] == CheckSum(buf, rvlen - 2, 0))
        {
            switch (buf[3 + MeterNum])
            {
            case UpgradeStartCmd:
            {
                /* 确认版本号,总包数,如果和表内存入的数据相等，则支持断点 */
                /* 注：如果该结构体数据不存入EPROM，或者不存铁电掉电丢失则不支持断点 */
                if ((UpStc->VER == buf[4 + MeterNum]) && (UpStc->TotalNum == buf[5 + MeterNum] + buf[6 + MeterNum] * 0x100))
                {
                    if (UpStc->DivNum == 0) /* 分包数为0 */
                    {
                        re = FlashErase();
                        if (re == 0)
                        {
                            /* 擦除成功 */
                            buf[3 + MeterNum] = UpgradeStartCmd + 1; /* Reply */
                            UpStc->DivNum = 0;
                            UpStc->UpCount = 0;
                        }
                        else
                        {
                            /* 擦除失败 */
                            buf[3 + MeterNum] = UpgradeStartCmd + 2; /* Reply */
                            if (++UpStc->UpCount > 3)
                            {
                                buf[3 + MeterNum] = GiveUpCmd;
                                memset(&UpStc, 0, sizeof(UpStc));
                            }
                        }
                    }
                    else
                    {
                        /* 断点请求,并上传表内分包数 */
                        buf[3 + MeterNum] = UpgradeStartCmd + 3; /* Reply */
                    }
                }
                else
                {
                    re = FlashErase();
                    if (re == 0)
                    {
                        /* 擦除成功 */
                        buf[3 + MeterNum] = UpgradeStartCmd + 1; /* Reply */
                        UpStc->VER = buf[4 + MeterNum];
                        UpStc->TotalNum = buf[5 + MeterNum] + buf[6 + MeterNum] * 0x100;
                        UpStc->DivNum = 0;
                        UpStc->UpCount = 0;
                    }
                    else
                    {
                        /* 擦除失败 */
                        buf[3 + MeterNum] = UpgradeStartCmd + 2; /* Reply */
                        if (++UpStc->UpCount > 3)
                        {
                            buf[3 + MeterNum] = GiveUpCmd;
                            memset(&UpStc, 0, sizeof(UpStc));
                        }
                    }
                }
            }
            break;
            case UpgradeDataCmd:
            {
                /* 确认版本号,总包数,分包数,如果和表内存入的数据相等则写入数据 */
                if ((UpStc->VER == buf[4 + MeterNum]) && (UpStc->TotalNum == (buf[5 + MeterNum] + buf[6 + MeterNum] * 0x100)))
                {
                    if (UpStc->DivNum == (buf[7 + MeterNum] + buf[8 + MeterNum] * 0x100))
                    {
                        /* 数据写入 */
                        uint16_t bootlen = 0;
                        uint16_t CRC16 = 0, RECRC = 0;

                        bootlen = buf[9 + MeterNum] + buf[10 + MeterNum] * 0x100;

                        if ((bootlen < 7) || (bootlen > 262))
                            re = 1;
                        else
                        {
                            CRC16 = calcCheckCrc(&buf[11 + MeterNum], bootlen - 2);
                            RECRC = (buf[9 + MeterNum + bootlen] + (buf[10 + MeterNum + bootlen] << 8));
                            if (CRC16 == RECRC)
                            {
                                memcpy(&WrADR, &buf[11 + MeterNum], 4);
                                re = CtsFlashWrite(WrADR, &buf[15 + MeterNum], bootlen - 6);
                                if (UpStc->DivNum == 0)
                                {
                                    /* 分包数为0时即首包的地址就是APP的起始地址 */
                                    /* 这个地址极其重要 */
                                    /* 当升级成功的时候需要将这个数据保存到BOOT区的0X3F00处 */
                                    API_Erase_Sector(APP2ENDADR);
                                    Flsah_Write_String(APP2ENDADR, (uint8_t *)&WrADR, 4, FLASHOPKEY);
                                }
                            }
                            else
                                re = 1;
                        }
                        if (re == 0)
                        {
                            /* 写入成功,分包数+1 */
                            UpStc->DivNum += 1;
                            UpStc->UpCount = 0;
                            buf[3 + MeterNum] = UpgradeDataCmd + 1; /* Reply */
#ifdef yctest
                            return 0;
#endif
                        }
                        else if (re == 1)
                        {
                            /* 写入失败 */
                            buf[3 + MeterNum] = UpgradeDataCmd + 2; /* Reply */
                            if (++UpStc->UpCount > 3)
                            {
                                buf[3 + MeterNum] = GiveUpCmd;
                                memset(&UpStc, 0, sizeof(UpStc));
                            }
                        }
                        else
                        {
                            /* APP与升级包处于同一地址区域 */
                            buf[3 + MeterNum] = UpgradeDataCmd + 6; /* Reply */
                            memset(&UpStc, 0, sizeof(UpStc));
                        }
                    }
                    else
                    {
                        /* 分包不符 */
                        buf[3 + MeterNum] = UpgradeDataCmd + 4; /* Reply */
                        if (++UpStc->UpCount > 3)
                        {
                            buf[3 + MeterNum] = GiveUpCmd;
                            memset(&UpStc, 0, sizeof(UpStc));
                        }
                    }
                }
                else
                {
                    /* 版本号,总包数不符 */
                    buf[3 + MeterNum] = UpgradeDataCmd + 5; /* Reply */
                    if (++UpStc->UpCount > 3)
                    {
                        buf[3 + MeterNum] = GiveUpCmd;
                        memset(&UpStc, 0, sizeof(UpStc));
                    }
                }
            }
            break;
            case UpgradeSucCmd:
            {
                if ((UpStc->TotalNum == UpStc->DivNum) && (UpStc->TotalNum != 0))
                {
                    buf[3 + MeterNum] = UpgradeSucCmd + 1; /* Reply */
                    re = 0xaa;                             /* 准备进入Boot */
                }
                else
                {
                    buf[3 + MeterNum] = UpgradeSucCmd + 2; /* Reply */
                    memset(&UpStc, 0, sizeof(UpStc));
                }
            }
            break;
            default:
            {
                buf[3 + MeterNum] += UnKonwnCmd; /* Reply */
                if (++UpStc->UpCount > 3)
                {
                    buf[3 + MeterNum] = GiveUpCmd;
                    memset(&UpStc, 0, sizeof(UpStc));
                }
            }
            break;
            }
        }
        else
        {
            buf[3 + MeterNum] += CheckSumError; /* Reply */
            if (++UpStc->UpCount > 3)
            {
                buf[3 + MeterNum] = GiveUpCmd;
                memset(&UpStc, 0, sizeof(UpStc));
            }
        }
    }

    buf[0] = 0xa6;          /* head */
    buf[1] = 11 + MeterNum; /* lenl */
    buf[2] = 0;             /* lenh */
    buf[4 + MeterNum] = UpStc->VER;
    buf[5 + MeterNum] = UpStc->TotalNum & 0xff; /* TotalNumL */
    buf[6 + MeterNum] = UpStc->TotalNum >> 8;   /* TotalNumH */
    buf[7 + MeterNum] = UpStc->DivNum & 0xff;   /* DivNumL */
    buf[8 + MeterNum] = UpStc->DivNum >> 8;     /* DivNumH */
    buf[9 + MeterNum] = CheckSum(buf, 9 + MeterNum, 0);
    buf[10 + MeterNum] = 0x5a; /* end */
#ifdef yctest
    return 2;
#else
    if (re == 0xaa)
    {
        memcpy(&WrADR, (uint8_t *)APP2ENDADR, 4);
        API_Erase_Sector(FlashStartAdr);
        Flsah_Write_String(FlashStartAdr, (uint8_t *)&WrADR, 4, FLASHOPKEY);
        return 1;
    }
    else
    {
        return 0;
    }
#endif
}

#endif
