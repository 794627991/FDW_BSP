/*
*********************************************************************************************************
*
*	ģ������ : FLASH����
*	�ļ����� : bsp_flash.c
*	��    �� : V1.0.0
*	˵    �� : FLASH��ز�����Զ������
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "bsp_flash.h"

/*
*********************************************************************************************************
*	�� �� ��: API_Erase_Sector
*	����˵��: FLASH�����
*	��    ��: Adr��Ҫ�����ĵ�ַ
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void API_Erase_Sector(uint32_t Adr)
{
    RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash��д������ʱ��ʹ�ܣ�����͹� */
    FLASH_Erase_Sector(Adr);
    RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
}
/*
*********************************************************************************************************
*	�� �� ��: Flsah_Write_String
*	����˵��: ����дflash����
*	��    ��: prog_addr����дflash�׵�ַ
*             prog_data����д���ݻ�����
*             Len��д�볤��(һ�β�Ҫд��̫�࣬���⿴�Ź����)
*             OperateKey  flash������֤��(���ڿɿ��Կ��ǣ����������û��Զ��壬
*                         �洢��ee����flash�У�����flashǰ��ȡ��ֵ��OperateKey)
*	�� �� ֵ: 0���ɹ� 1��ʧ��
*	��    ע����
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
        RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash��д������ʱ��ʹ�ܣ�����͹� */
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
*	�� �� ��: Flash_Erase_Sector
*	����˵��: ����������(ÿ������512�ֽ�)
*	��    ��: SectorNum����Ҫ����������������
*             OperateKey  flash������֤��(���ڿɿ��Կ��ǣ����������û��Զ��壬
*                         �洢��ee����flash�У�����flashǰ��ȡ��ֵ��OperateKey)
*	�� �� ֵ: 0���ɹ� 1��ʧ��  2������
*	��    ע����
*********************************************************************************************************
*/
uint8_t Flash_Erase_Sector(uint16_t SectorNum, uint32_t OperateKey)
{
    uint16_t i = 0;
    uint32_t *PFlash = NULL;

    if (SectorNum < BOOTSIZE) /* ��ֹ����boot�� */
        return 2;
    if ((uint32_t)(0x200 * SectorNum) > FlashAllSize) /* Ҫ�������������� */
        return 2;

    PFlash = (uint32_t *)(uint32_t)(0x200 * SectorNum);

    if (OperateKey == FLASHOPKEY)
    {
        RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash��д������ʱ��ʹ�ܣ�����͹� */
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
*	�� �� ��: Flash_App_Erase_Sector
*	����˵��: ������һAPP����������(ÿ������512�ֽ�)
*	��    ��: SectorNum����Ҫ����������������
*             OperateKey ��flash������֤��(���ڿɿ��Կ��ǣ����������û��Զ��壬
*                         �洢��ee����flash�У�����flashǰ��ȡ��ֵ��OperateKey)
*             APP_Start_Adr����ǰAPP���ڵ�ַ
*	�� �� ֵ: 0���ɹ� 1��ʧ��  2������
*	��    ע��Զ������ʹ�ã�Ĭ��APP������2������ǰAPP����������BOOT����ֹ����
*********************************************************************************************************
*/
uint8_t Flash_App_Erase_Sector(uint16_t SectorNum, uint32_t OperateKey, uint32_t APP_Start_Adr)
{
    uint16_t i = 0;
    uint32_t *PFlash = NULL;

    if (APP_Start_Adr < APP1ENDADR)
    {
        if (SectorNum < APPDIVSIZE) /* ��ֹ����APP��ǰ�� */
            return 2;
    }
    else if (APP_Start_Adr < APP2ENDADR)
    {
        if ((SectorNum < BOOTSIZE) || (SectorNum > APPDIVSIZE)) /* ��ֹ����APP��ǰ����boot�� */
            return 2;
    }
    else
    {
        return 2; /* ������APP��ʼ��ַ�������� */
    }

    if ((uint32_t)(0x200 * SectorNum) > APP2ENDADR) /* Ҫ�������������� */
        return 2;
    PFlash = (uint32_t *)(uint32_t)(0x200 * SectorNum);

    if (OperateKey == FLASHOPKEY)
    {
        RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE); /* Flash��д������ʱ��ʹ�ܣ�����͹� */
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
*	�� �� ��: FlashMakeSureBoot
*	����˵��: ��֤ boot ������ǵ�ǰ app �����е�ַ
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void FlashMakeSureBoot(void)
{
    uint32_t APP_Start_Adr = 0, OffSet = 0;
    memcpy(&APP_Start_Adr, (uint8_t *)FlashStartAdr, 4); /* ��Flash��ȡ��ǰ���е�APP��ʼ��ַ */
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
*	�� �� ��: FlashErase
*	����˵��: ��ȫ������һAPP����
*	��    ��: ��
*	�� �� ֵ: 0���ɹ� 1��ʧ��  2������
*	��    ע��Զ������ʹ�ã�Ĭ��APP������2������ǰAPP����������BOOT����ֹ����
*            �궨��˵����
*            FlashStartAdr����BOOT�������ڴ洢��ǰ���е�APP����ʼ��ַ
*            APP1ENDADR��APP����1���Ľ�����ַ���� APP_CFG.h �ж��壬������0x200�ı���
*            APP2ENDADR��ȫ������APP���С(����BOOT����С)����ΪFlash���������������������
*            BOOTSIZE��BOOT���������С
*            APPMAXDIVSIZE��ȫ�������������С
*            APPDIVSIZE��APP�ֽ��������С,�� APP1ENDADR/0x200
*                        APPMAXDIVSIZE-APPDIVSIZE ����APP2�������С
*                        APPDIVSIZE - BOOTSIZE ���� APP1�������С
*********************************************************************************************************
*/
uint8_t FlashErase(void)
{
    uint32_t APP_Start_Adr = 0;
    uint16_t i = 0;
    uint8_t Result = 0;

    if (APP_Start_Adr < APP1ENDADR)
    {
        /* ��ǰAPP����1��������2��:APP1ENDADR-APP2ENDADR */
        for (i = 0; i < APPMAXDIVSIZE - APPDIVSIZE; i++)
        {
            IWDT_Clr();
            Result = Flash_App_Erase_Sector(APPDIVSIZE + i, FLASHOPKEY, APP_Start_Adr);
        }
    }
    else
    {
        /*��ǰAPP����2��������1��:BOOTENDADR-APP1ENDADR*/
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
*	�� �� ��: CtsFlashWrite
*	����˵��: ����дFlash
*	��    ��: WRADR��д��ĵ�ַ
*             buf��Ҫд������ݻ���
*             len��Ҫд�ĳ���
*	�� �� ֵ: 0���ɹ� 1��ʧ��  2������
*	��    ע��Զ������ʹ�ã�Ĭ��APP������2������ǰAPP����������BOOT����ֹ����
*********************************************************************************************************
*/
uint8_t CtsFlashWrite(uint32_t WRADR, uint8_t *buf, uint16_t len)
{
    uint32_t APP_Start_Adr = 0;
    uint8_t Result = 0;

    if (APP_Start_Adr < APP1ENDADR)
    {
        /*��ǰAPP����1��������ֻ��д2��*/
        if (WRADR < APP1ENDADR)
            return 2; //��ֹдAPP��ǰ��
    }
    else if (APP_Start_Adr < APP2ENDADR)
    {
        /*��ǰAPP����2��������д1��*/
        if ((WRADR < BOOTENDADR) || (WRADR > APP1ENDADR))
            return 2; //��ֹдAPP��ǰ����boot��
    }
    else
    {
        return 2; //������APP��ʼ��ַ��������
    }

    Result = Flsah_Write_String(WRADR, buf, len, FLASHOPKEY);

    return Result;
}

#if API_UseDataUp > 0
Upstruct UPstr; /* �ṹ�嶨�� */
/*
*********************************************************************************************************
*	�� �� ��: API_UpData
*	����˵��: Զ������
*	��    ��: buf���������ݰ�
*             Upstruct *UpStc�������ṹ��ָ��
*	�� �� ֵ: 1����� 0������������
*	��    ע��Զ��������֧�ֶϵ�����
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
                /* ȷ�ϰ汾��,�ܰ���,����ͱ��ڴ����������ȣ���֧�ֶϵ� */
                /* ע������ýṹ�����ݲ�����EPROM�����߲���������綪ʧ��֧�ֶϵ� */
                if ((UpStc->VER == buf[4 + MeterNum]) && (UpStc->TotalNum == buf[5 + MeterNum] + buf[6 + MeterNum] * 0x100))
                {
                    if (UpStc->DivNum == 0) /* �ְ���Ϊ0 */
                    {
                        re = FlashErase();
                        if (re == 0)
                        {
                            /* �����ɹ� */
                            buf[3 + MeterNum] = UpgradeStartCmd + 1; /* Reply */
                            UpStc->DivNum = 0;
                            UpStc->UpCount = 0;
                        }
                        else
                        {
                            /* ����ʧ�� */
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
                        /* �ϵ�����,���ϴ����ڷְ��� */
                        buf[3 + MeterNum] = UpgradeStartCmd + 3; /* Reply */
                    }
                }
                else
                {
                    re = FlashErase();
                    if (re == 0)
                    {
                        /* �����ɹ� */
                        buf[3 + MeterNum] = UpgradeStartCmd + 1; /* Reply */
                        UpStc->VER = buf[4 + MeterNum];
                        UpStc->TotalNum = buf[5 + MeterNum] + buf[6 + MeterNum] * 0x100;
                        UpStc->DivNum = 0;
                        UpStc->UpCount = 0;
                    }
                    else
                    {
                        /* ����ʧ�� */
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
                /* ȷ�ϰ汾��,�ܰ���,�ְ���,����ͱ��ڴ�������������д������ */
                if ((UpStc->VER == buf[4 + MeterNum]) && (UpStc->TotalNum == (buf[5 + MeterNum] + buf[6 + MeterNum] * 0x100)))
                {
                    if (UpStc->DivNum == (buf[7 + MeterNum] + buf[8 + MeterNum] * 0x100))
                    {
                        /* ����д�� */
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
                                    /* �ְ���Ϊ0ʱ���װ��ĵ�ַ����APP����ʼ��ַ */
                                    /* �����ַ������Ҫ */
                                    /* �������ɹ���ʱ����Ҫ��������ݱ��浽BOOT����0X3F00�� */
                                    API_Erase_Sector(APP2ENDADR);
                                    Flsah_Write_String(APP2ENDADR, (uint8_t *)&WrADR, 4, FLASHOPKEY);
                                }
                            }
                            else
                                re = 1;
                        }
                        if (re == 0)
                        {
                            /* д��ɹ�,�ְ���+1 */
                            UpStc->DivNum += 1;
                            UpStc->UpCount = 0;
                            buf[3 + MeterNum] = UpgradeDataCmd + 1; /* Reply */
#ifdef yctest
                            return 0;
#endif
                        }
                        else if (re == 1)
                        {
                            /* д��ʧ�� */
                            buf[3 + MeterNum] = UpgradeDataCmd + 2; /* Reply */
                            if (++UpStc->UpCount > 3)
                            {
                                buf[3 + MeterNum] = GiveUpCmd;
                                memset(&UpStc, 0, sizeof(UpStc));
                            }
                        }
                        else
                        {
                            /* APP������������ͬһ��ַ���� */
                            buf[3 + MeterNum] = UpgradeDataCmd + 6; /* Reply */
                            memset(&UpStc, 0, sizeof(UpStc));
                        }
                    }
                    else
                    {
                        /* �ְ����� */
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
                    /* �汾��,�ܰ������� */
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
                    re = 0xaa;                             /* ׼������Boot */
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
