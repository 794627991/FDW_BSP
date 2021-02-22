/*
*********************************************************************************************************
*
*	ģ������ : RTC_BSP����
*	�ļ����� : rtc_api.c
*	��    �� : V1.0.0
*	˵    �� : Ӳ��RTC��ʼ����
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����    ��ʽ����
*		V1.0.1  2021-1-20   ����    ���� GetTimeStamp �� GetTimStmpDif
*		V1.0.2  2021-1-21   ����    �޸� dateDelta ���귵��3��0�յ�����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "rtc_api.h"

uint8_t RealTime[14]; /* ȡʱ����,����� ALL_API.h ��Bcd_YearH-Hex_Sec�ȶ��� */

const uint8_t dpm[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* ƽ��ÿ������ */
/*
*********************************************************************************************************
*	�� �� ��: isLeap
*	����˵��: �ж�����
*	��    ��: year�����
*	�� �� ֵ: 1:��; 0:��
*	��    ע�������ص㣺�����4�ı���������100�ı����������400�ı���
*********************************************************************************************************
*/
int isLeap(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
/*
*********************************************************************************************************
*	�� �� ��: isLeap
*	����˵��: ����У��
*	��    ��: year�����
*	�� �� ֵ: ���󷵻�1����ȷ����0
*	��    ע�������ص㣺�����4�ı���������100�ı����������400�ı���
*********************************************************************************************************
*/
int dateIsErr(DataType d)
{
    if (d.year < 1900 && d.month < 1 && d.day < 1) /* ��֧��1900��1��1��֮������ڼ��� */
        return 1;
    if (d.month < 1 || d.month > 12) /* ��У�� */
        return 1;
    if (d.day < 1 || d.day > 31) /* �ճ���У�� */
        return 1;
    if (d.month != 2 && d.day > dpm[d.month]) /* ��2����У�� */
        return 1;
    if (d.month == 2 && isLeap(d.year) && d.day > 29) /* 2��������У�� */
        return 1;
    if (d.month == 2 && isLeap(d.year) == 0 && d.day > 28) /* 2��ƽ����У�� */
        return 1;
    return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: dateDelta
*	����˵��: ����������start���delta�������
*	��    ��: start����ʼ������
*             delta�������������������븺��
*	�� �� ֵ: ����󷵻ص�������
*	��    ע����
*********************************************************************************************************
*/
DataType dateDelta(DataType start, int delta)
{
    DataType end = start;

    if (delta >= 0)
    {
        /* ����start������ */
        end.day = start.day + delta; /* start�ռ�intervalDays�� */
        while (end.day > dpm[end.month])
        {
            /* ���ϼ�ȥ�������������¡����λ��ֱ���䲻������ǰ�·����� */
            end.day -= dpm[end.month];
            if (isLeap(end.year) && end.month == 2) /* ����2������-1 */
            {
                end.day--;
                if (end.day == 0)
                {
                    /* ˵����29�� */
                    end.day = 29;
                    break;
                }
            }

            end.month++; /* �½�λ+1 */
            if (end.month == 13)
            {
                /* ��1�� */
                end.year++;    /* ���λ+1 */
                end.month = 1; /* �¹�λ��1�� */
            }
        }
    }
    else
    {
        /* ����start��ǰ���� */
        end.day = start.day + delta; /* start�ռ�intervalDays�� */
        while (end.day < 1)
        {
            /* ���ϼ��������������¡����λ��ֱ���䲻С��1 */
            end.month--; /* �½�λ-1 */
            if (end.month == 0)
            {
                /* �����λ */
                end.year--;     /* ���λ-1 */
                end.month = 12; /* �¹�λ��12�� */
            }
            end.day += dpm[end.month];
            if (isLeap(end.year) && end.month == 2) /* ����2������+1 */
                end.day++;
        }
    }

    return end;
}
/*
*********************************************************************************************************
*	�� �� ��: GetTimeStamp
*	����˵��: ��ȡ����ĳ��ʱ���,����ĳһ���ʱ���
*	��    ��: day��ʱ���
*             cnt����������������Ϊ��
*	�� �� ֵ: ��0��ʱ���ֵ��0������
*	��    ע����
*********************************************************************************************************
*/
uint32_t GetTimeStamp(DataType day, int cnt)
{
    struct tm stm = {0};
    DataType tt;
    if (dateIsErr(day) == 1)
        return 0;
    if (cnt == 0)
    {
        stm.tm_year = day.year - 1900;
        stm.tm_mon = day.month - 1;
        stm.tm_mday = day.day;
    }
    else
    {
        tt = dateDelta(day, cnt);
        stm.tm_year = tt.year - 1900;
        stm.tm_mon = tt.month - 1;
        stm.tm_mday = tt.day;
    }
    return mktime(&stm);
}
/*
*********************************************************************************************************
*	�� �� ��: GetTimStmpDif
*	����˵��: ��ȡ����ʱ���֮����������
*	��    ��: day1��ʱ���1
*             day2��ʱ���2
*             lmit��������ֵ
*	�� �� ֵ: ��-1������������-1������
*	��    ע������ʱ��������������ʱ������� GetTimeStamp ����������ʱ���
*********************************************************************************************************
*/
int GetTimStmpDif(uint32_t day1, uint32_t day2, uint16_t lmit)
{
    uint32_t i = 0;
    struct tm *Tm, stm = {0};
    DataType t1, t2;
    int sign = 1;

    if (day1 == day2)
        return 0;
    Tm = localtime((time_t *)&day2);

    if (day1 < day2)
    {
        sign = -1;
    }

    t1.day = Tm->tm_mday;
    t1.month = Tm->tm_mon + 1;
    t1.year = Tm->tm_year + 1900;
    for (i = 1; i < lmit; i++)
    {
        t2 = dateDelta(t1, sign);
        memcpy(&t1, &t2, sizeof(DataType));
        stm.tm_year = t2.year - 1900;
        stm.tm_mon = t2.month - 1;
        stm.tm_mday = t2.day;
        if (mktime(&stm) == day1)
        {
            break;
        }
    }
    if (i >= lmit)
    {
        return -1;
    }
    return i;
}
#if API_UseRTC > 0
/*
*********************************************************************************************************
*	�� �� ��: GY_RTC_Get
*	����˵��: ��Ӳ��RTCʱ��
*	��    ��: *para��������ʱ����
*	�� �� ֵ: 0��ʧ�ܣ�1���ɹ�
*	��    ע����
*********************************************************************************************************
*/
uint8_t GY_RTC_Get(RTC_TimeDateTypeDef *para)
{
    uint8_t n = 0, i = 0;
    uint8_t Result = 0;

    RTC_TimeDateTypeDef TempTime1, TempTime2;

    for (n = 0; n < 3; n++)
    {
        RTC_TimeDate_GetEx(&TempTime1); /* ��һ��ʱ�� */
        RTC_TimeDate_GetEx(&TempTime2); /* �ٶ�һ��ʱ�� */

        for (i = 0; i < 7; i++) /* ����һ��, ��ʾ��ȡ�ɹ� */
        {
            if (((uint8_t *)(&TempTime1))[i] != ((uint8_t *)(&TempTime2))[i])
                break;
        }
        if (i == 7)
        {
            Result = 1;
            memcpy((uint8_t *)(para), (uint8_t *)(&TempTime1), 7); /* ��ȡ��ȷ������µ�ʱ�� */
            break;
        }
    }
    return Result;
}
/*
*********************************************************************************************************
*	�� �� ��: Check_date
*	����˵��: �ж����ںϷ���
*	��    ��: w_year����
*             w_month����
*             w_date����
*	�� �� ֵ: 0������1����ȷ
*	��    ע����
*********************************************************************************************************
*/
bool Check_date(uint16_t w_year, uint8_t w_month, uint8_t w_date)
{
    uint8_t Month_temp;
    if (w_month > 12 || w_month < 1) /* �ж��·��Ƿ�Ϸ� */
        return false;

    if (w_month == 2) /* ����2��+1�� */
    {
        (isLeap(w_year)) ? (Month_temp = dpm[2] + 1) : (Month_temp = dpm[2]);
    }
    else
    {
        Month_temp = dpm[w_month];
    }

    if (w_date > Month_temp || w_date < 1) /* �ж������Ƿ�Ϸ� */
        return false;

    return true;
}
/*
*********************************************************************************************************
*	�� �� ��: GY_IsLegal
*	����˵��: �ж�ʱ������ڵĺϷ���
*	��    ��: RTC_TimeDateTypeDef *para
*	�� �� ֵ: 0������1����ȷ
*	��    ע����
*********************************************************************************************************
*/
char GY_IsLegal(RTC_TimeDateTypeDef *para)
{
    uint16_t year = BCDtoHEX(para->Year) + 2000;
    uint8_t month = BCDtoHEX(para->Month);
    uint8_t date = BCDtoHEX(para->Date);
    uint8_t hour = BCDtoHEX(para->Hour);
    uint8_t minute = BCDtoHEX(para->Minute);
    uint8_t second = BCDtoHEX(para->Second);

    if (second > 59 || minute > 59 || hour > 23)
        return 0;
    if (!Check_date(year, month, date))
        return 0;
    return 1;

    //��1 3 5 7 8 10 12
    //С��4 6 9 11
    //ƽ��2
    //uint8_t�ͱ���������С����
    // if (para->Second > 0x59 || para->Minute > 0x59 || para->Hour > 0x23)
    //     return 0;
    // if (para->Year > 0x99 || para->Month == 0 || para->Month > 0x12 || para->Date == 0 || para->Date > 0x31)
    //     return 0;
    // if ((4 == para->Month || 6 == para->Month || 9 == para->Month || 0x11 == para->Month) && (para->Date > 0x30))
    //     return 0;
    // if (1 == para->Month || 3 == para->Month || 5 == para->Month || 7 == para->Month || 8 == para->Month || 0x10 == para->Month || 0x12 == para->Month)
    // {
    //     return 1;
    // }
    // year = (para->Year & 0xf0) >> 4;
    // year *= 10;
    // year += (para->Year & 0x0f);
    // year += 2000;

    // if (isLeap(year))
    // {
    //     if (2 == para->Month && (0x28 == para->Date || 0x30 == para->Date || 0x31 == para->Date))
    //         return 0;
    //     return 1;
    // }
    // else
    // {
    //     if (2 == para->Month && (0x29 == para->Date || 0x30 == para->Date || 0x31 == para->Date))
    //         return 0;
    //     return 1;
    // }
}
/*
*********************************************************************************************************
*	�� �� ��: API_SetTIME
*	����˵��: ����RTC
*	��    ��: RTC_TimeDateTypeDef *para
*	�� �� ֵ: 0������1����ȷ
*	��    ע����
*********************************************************************************************************
*/
__weak uint8_t API_SetTIME(RTC_TimeDateTypeDef *para)
{
    uint8_t n = 0, i = 0;
    uint8_t Result = 0;
    RTC_TimeDateTypeDef TempTime1;

    if (!GY_IsLegal(para))
        return 0;

    for (n = 0; n < 3; n++)
    {
        RTC_RTCWE_Write(RTC_WRITE_ENABLE);  /* ���RTCд���� */
        RTC_TimeDate_SetEx(para);           /* ����RTC */
        RTC_RTCWE_Write(RTC_WRITE_DISABLE); /* ��RTCд���� */

        if (GY_RTC_Get(&TempTime1)) /* ��ȡȷ�����ý�� */
        {
            for (i = 0; i < 6; i++)
            {
                if (((uint8_t *)(&TempTime1))[i] != ((uint8_t *)(para))[i])
                    break;
            }
            if (i == 6)
            {
                Result = 1; /* ����һ��, ��ʾ���óɹ�,���ж��� */
                break;
            }
        }
    }
    return Result;
}
/*
*********************************************************************************************************
*	�� �� ��: API_RTC_Init
*	����˵��: ��ʼ��RTC
*	��    ��: RTC_INIT_Type *init
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
__weak void API_RTC_Init(RTC_INIT_Type *init)
{
    RTC_TimeDateTypeDef TempTime;
    RCC_PERCLK_SetableEx(RTCCLK, ENABLE); /* RTC����ʱ��ʹ�� */

    RTC_FSEL_FSEL_Set(RTC_FSEL_FSEL_PLL1HZ); /* Ƶ�����ѡ���ź� */
    RTC_Trim_Proc(0);                        /* ����ֵ����(�൱�ڹرղ���) */
    RTC_PR1SEN_PR1SEN_Setable(DISABLE);      /* �����Уʹ�ܹر� */

    RTC_STAMPEN_STAMP0EN_Setable(DISABLE); /* �ر�PB4ʱ������� ע��ʱ������ܽ�������ģʽ����Ч */
    RTC_STAMPEN_STAMP1EN_Setable(DISABLE); /* �ر�PB5ʱ������� */

    RTC_RTCIE_SetableEx(DISABLE, 0xFFFFFFFF); /* �ر�����RTC�ж� */
    if (init->day != 0)
    {
        RTC_RTCIE_SetableEx(ENABLE, RTC_RTCIE_DATE_IE_Msk);
    }
    else
    {
        RTC_RTCIE_SetableEx(DISABLE, RTC_RTCIE_DATE_IE_Msk);
    }
    if (init->hour != 0)
    {
        RTC_RTCIE_SetableEx(ENABLE, RTC_RTCIE_HOUR_IE_Msk);
    }
    else
    {
        RTC_RTCIE_SetableEx(DISABLE, RTC_RTCIE_HOUR_IE_Msk);
    }
    if (init->min != 0)
    {
        RTC_RTCIE_SetableEx(ENABLE, RTC_RTCIE_MIN_IE_Msk);
    }
    else
    {
        RTC_RTCIE_SetableEx(DISABLE, RTC_RTCIE_MIN_IE_Msk);
    }
    if (init->sec != 0)
    {
        RTC_RTCIE_SetableEx(ENABLE, RTC_RTCIE_SEC_IE_Msk);
    }
    else
    {
        RTC_RTCIE_SetableEx(DISABLE, RTC_RTCIE_SEC_IE_Msk);
    }

    RTC_RTCIF_ClrEx(RTC_RTCIE_SEC_IE_Msk); /* ������жϱ�־ */

    NVIC_SET(RTC_IRQn, init->Nvicpry);

    GY_RTC_Get(&TempTime);
    if (!GY_IsLegal(&TempTime))
    {
        uint8_t Time[7] = {0x19, 0x03, 0x18, 0x10, 0x29, 0x58, 0x01};
        memcpy((uint8_t *)(&TempTime), Time, 7);
        API_SetTIME(&TempTime); /* ����ʱ�� */
    }
}
/*
*********************************************************************************************************
*	�� �� ��: API_GetTime
*	����˵��: ��ȡRTCʱ��
*	��    ��: RTC_INIT_Type *init
*	�� �� ֵ: ��
*	��    ע��0-6 BCD:�� �� �� �� ʱ �� ��  7-13 HEX:�� �� �� �� ʱ �� ��
*********************************************************************************************************
*/
__weak void API_GetTime(uint8_t *tim)
{
    uint8_t i;
    RTC_TimeDateTypeDef TempTime;
    GY_RTC_Get(&TempTime);
    memcpy(tim, &TempTime, 7);

    for (i = 0; i < 6; i++)
    {
        tim[6 - i] = tim[5 - i];
    }
    tim[0] = 0x20;

    memcpy(&tim[7], tim, 7);

    for (i = 7; i < 14; i++)
    {
        tim[i] = BCDtoHEX(tim[i]);
    }
}

#endif
