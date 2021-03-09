/*
*********************************************************************************************************
*
*	ģ������ : FLASH����
*	�ļ����� : bsp_flash.h
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
#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_def.h"

#define FLASHOPKEY 0x12ABF00F
#define FlashEraseSize 0x200 /* �������С */
#define UpgradeStartCmd 0x10 /* ��ʼ�������� */
#define UpgradeDataCmd 0x20  /* ���������� */
#define UpgradeSucCmd 0x30   /* �����ɹ����� */
#define UnKonwnCmd 0x0A      /* δ֪���� */
#define CheckSumError 0x0B   /* У����� */
#define GiveUpCmd 0xCC       /* �������� */
#define MeterNum 4           /* Զ�������б�ŵĳ��� */

#ifndef FlashAllSize
#define FlashAllSize 0x40000
#endif

#ifndef BOOTENDADR
#define BOOTENDADR 0X10000
#endif

#ifndef APP1ENDADR
#define APP1ENDADR 0X28000
#endif

#define FlashStartAdr (BOOTENDADR - FlashEraseSize)    /* ��BOOT�������ڴ洢��ǰ���е�APP����ʼ��ַ */
#define BOOTSIZE (BOOTENDADR / FlashEraseSize)         /* BOOT���������С  */
#define APPDIVSIZE (APP1ENDADR / FlashEraseSize)       /* APP�ֽ��������С,�� APP1ENDADR/0x200 APPMAXDIVSIZE-APPDIVSIZE ����APP2�������С APPDIVSIZE - BOOTSIZE ���� APP1�������С*/
#define APP2ENDADR (FlashAllSize - FlashEraseSize * 2) /* ȫ������APP���С(����BOOT����С)����ΪFlash��������������������� */
#define APPMAXDIVSIZE (APP2ENDADR / FlashEraseSize)    /* ȫ�������������С */

    typedef struct
    {
        uint8_t FlashBlock;   /* Flash����---��С64K */
        uint8_t FlashPageAdr; /* Flashҳ���ַ0-0xff */
        uint32_t FlashPage;   /* Flashҳ��---��С256�ֽ� */
    } Flashstruct;

    typedef struct
    {
        uint8_t VER;       /* �汾��0-0xff */
        uint8_t UpCount;   /* ����ʧ�ܴ�������� */
        uint32_t TotalNum; /* �ܰ��� */
        uint32_t DivNum;   /* �ְ��� */
    } Upstruct;
    extern Upstruct UPstr;

    typedef struct
    {
        unsigned long int FarmStartAdd; /* Fram��ʼ��ַ */
        unsigned long int BootStartAdd; /* Boot��ʼ��ַ */
        unsigned long int VectorAdd;    /* �ж�������ʼ��ַ */
        unsigned long int FarmEndAdd;   /* Fram������ַ */
    } FlashBFstruct;

    /**************************����ָ�붨��**************************/
    typedef uint8_t (*gy_updata)(uint8_t *buf, uint16_t len, Upstruct *UpStc);

    /****************************��������****************************/
    void API_Erase_Sector(uint32_t Adr);
    uint8_t API_UpData(uint8_t *buf, uint16_t len, Upstruct *UpStc);
    uint8_t Flash_Erase_Sector(uint16_t SectorNum, uint32_t OperateKey);
    uint8_t Flsah_Write_String(uint32 prog_addr, uint8_t *prog_data, uint16_t Len, uint32_t OperateKey);

#ifdef __cplusplus
}
#endif

#endif
