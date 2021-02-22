/*
*********************************************************************************************************
*
*	模块名称 : RTC_BSP程序
*	文件名称 : rtc_api.c
*	版    本 : V1.0.0
*	说    明 : 硬件RTC初始化等
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹    正式发布
*		V1.0.1  2021-1-20   高屹    新增 GetTimeStamp 和 GetTimStmpDif
*		V1.0.2  2021-1-21   高屹    修复 dateDelta 闰年返回3月0日的问题
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "rtc_api.h"

uint8_t RealTime[14]; /* 取时间用,详情见 ALL_API.h 中Bcd_YearH-Hex_Sec等定义 */

const uint8_t dpm[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* 平年每月天数 */
/*
*********************************************************************************************************
*	函 数 名: isLeap
*	功能说明: 判断闰年
*	形    参: year：年份
*	返 回 值: 1:是; 0:否
*	备    注：闰年特点：年份是4的倍数但不是100的倍数、年份是400的倍数
*********************************************************************************************************
*/
int isLeap(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
/*
*********************************************************************************************************
*	函 数 名: isLeap
*	功能说明: 日期校验
*	形    参: year：年份
*	返 回 值: 错误返回1，正确返回0
*	备    注：闰年特点：年份是4的倍数但不是100的倍数、年份是400的倍数
*********************************************************************************************************
*/
int dateIsErr(DataType d)
{
    if (d.year < 1900 && d.month < 1 && d.day < 1) /* 仅支持1900年1月1日之后的日期计算 */
        return 1;
    if (d.month < 1 || d.month > 12) /* 月校验 */
        return 1;
    if (d.day < 1 || d.day > 31) /* 日初步校验 */
        return 1;
    if (d.month != 2 && d.day > dpm[d.month]) /* 非2月日校验 */
        return 1;
    if (d.month == 2 && isLeap(d.year) && d.day > 29) /* 2月闰年日校验 */
        return 1;
    if (d.month == 2 && isLeap(d.year) == 0 && d.day > 28) /* 2月平年日校验 */
        return 1;
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: dateDelta
*	功能说明: 计算与日期start相差delta天的日期
*	形    参: start：开始年月日
*             delta：相差的天数，可以输入负数
*	返 回 值: 计算后返回的年月日
*	备    注：无
*********************************************************************************************************
*/
DataType dateDelta(DataType start, int delta)
{
    DataType end = start;

    if (delta >= 0)
    {
        /* 日期start向后计算 */
        end.day = start.day + delta; /* start日加intervalDays天 */
        while (end.day > dpm[end.month])
        {
            /* 不断减去整月天数，向月、年进位，直至其不超过当前月份天数 */
            end.day -= dpm[end.month];
            if (isLeap(end.year) && end.month == 2) /* 闰年2月天数-1 */
            {
                end.day--;
                if (end.day == 0)
                {
                    /* 说明是29日 */
                    end.day = 29;
                    break;
                }
            }

            end.month++; /* 月进位+1 */
            if (end.month == 13)
            {
                /* 满1年 */
                end.year++;    /* 年进位+1 */
                end.month = 1; /* 月归位至1月 */
            }
        }
    }
    else
    {
        /* 日期start向前计算 */
        end.day = start.day + delta; /* start日加intervalDays天 */
        while (end.day < 1)
        {
            /* 不断加整月天数，向月、年借位，直至其不小于1 */
            end.month--; /* 月借位-1 */
            if (end.month == 0)
            {
                /* 向年借位 */
                end.year--;     /* 年借位-1 */
                end.month = 12; /* 月归位至12月 */
            }
            end.day += dpm[end.month];
            if (isLeap(end.year) && end.month == 2) /* 闰年2月天数+1 */
                end.day++;
        }
    }

    return end;
}
/*
*********************************************************************************************************
*	函 数 名: GetTimeStamp
*	功能说明: 获取距离某个时间点,正负某一天的时间戳
*	形    参: day：时间点
*             cnt：相差的天数，可以为负
*	返 回 值: 非0：时间戳值，0：错误
*	备    注：无
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
*	函 数 名: GetTimStmpDif
*	功能说明: 获取两个时间戳之间相差的天数
*	形    参: day1：时间戳1
*             day2：时间戳2
*             lmit：限制数值
*	返 回 值: 非-1：相差的天数，-1：错误
*	备    注：传参时间戳必须是整天的时间戳即由 GetTimeStamp 函数产生的时间戳
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
*	函 数 名: GY_RTC_Get
*	功能说明: 读硬件RTC时间
*	形    参: *para：年月日时分秒
*	返 回 值: 0：失败，1：成功
*	备    注：无
*********************************************************************************************************
*/
uint8_t GY_RTC_Get(RTC_TimeDateTypeDef *para)
{
    uint8_t n = 0, i = 0;
    uint8_t Result = 0;

    RTC_TimeDateTypeDef TempTime1, TempTime2;

    for (n = 0; n < 3; n++)
    {
        RTC_TimeDate_GetEx(&TempTime1); /* 读一次时间 */
        RTC_TimeDate_GetEx(&TempTime2); /* 再读一次时间 */

        for (i = 0; i < 7; i++) /* 两者一致, 表示读取成功 */
        {
            if (((uint8_t *)(&TempTime1))[i] != ((uint8_t *)(&TempTime2))[i])
                break;
        }
        if (i == 7)
        {
            Result = 1;
            memcpy((uint8_t *)(para), (uint8_t *)(&TempTime1), 7); /* 读取正确则更新新的时间 */
            break;
        }
    }
    return Result;
}
/*
*********************************************************************************************************
*	函 数 名: Check_date
*	功能说明: 判断日期合法性
*	形    参: w_year：年
*             w_month：月
*             w_date：日
*	返 回 值: 0：错误，1：正确
*	备    注：无
*********************************************************************************************************
*/
bool Check_date(uint16_t w_year, uint8_t w_month, uint8_t w_date)
{
    uint8_t Month_temp;
    if (w_month > 12 || w_month < 1) /* 判断月份是否合法 */
        return false;

    if (w_month == 2) /* 闰年2月+1天 */
    {
        (isLeap(w_year)) ? (Month_temp = dpm[2] + 1) : (Month_temp = dpm[2]);
    }
    else
    {
        Month_temp = dpm[w_month];
    }

    if (w_date > Month_temp || w_date < 1) /* 判断日期是否合法 */
        return false;

    return true;
}
/*
*********************************************************************************************************
*	函 数 名: GY_IsLegal
*	功能说明: 判断时间和日期的合法性
*	形    参: RTC_TimeDateTypeDef *para
*	返 回 值: 0：错误，1：正确
*	备    注：无
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

    //大：1 3 5 7 8 10 12
    //小：4 6 9 11
    //平：2
    //uint8_t型变量不可能小于零
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
*	函 数 名: API_SetTIME
*	功能说明: 设置RTC
*	形    参: RTC_TimeDateTypeDef *para
*	返 回 值: 0：错误，1：正确
*	备    注：无
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
        RTC_RTCWE_Write(RTC_WRITE_ENABLE);  /* 解除RTC写保护 */
        RTC_TimeDate_SetEx(para);           /* 设置RTC */
        RTC_RTCWE_Write(RTC_WRITE_DISABLE); /* 打开RTC写保护 */

        if (GY_RTC_Get(&TempTime1)) /* 读取确认设置结果 */
        {
            for (i = 0; i < 6; i++)
            {
                if (((uint8_t *)(&TempTime1))[i] != ((uint8_t *)(para))[i])
                    break;
            }
            if (i == 6)
            {
                Result = 1; /* 两者一致, 表示设置成功,不判断周 */
                break;
            }
        }
    }
    return Result;
}
/*
*********************************************************************************************************
*	函 数 名: API_RTC_Init
*	功能说明: 初始化RTC
*	形    参: RTC_INIT_Type *init
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void API_RTC_Init(RTC_INIT_Type *init)
{
    RTC_TimeDateTypeDef TempTime;
    RCC_PERCLK_SetableEx(RTCCLK, ENABLE); /* RTC总线时钟使能 */

    RTC_FSEL_FSEL_Set(RTC_FSEL_FSEL_PLL1HZ); /* 频率输出选择信号 */
    RTC_Trim_Proc(0);                        /* 补偿值清零(相当于关闭补偿) */
    RTC_PR1SEN_PR1SEN_Setable(DISABLE);      /* 虚拟调校使能关闭 */

    RTC_STAMPEN_STAMP0EN_Setable(DISABLE); /* 关闭PB4时间戳功能 注意时间戳功能仅在休眠模式下有效 */
    RTC_STAMPEN_STAMP1EN_Setable(DISABLE); /* 关闭PB5时间戳功能 */

    RTC_RTCIE_SetableEx(DISABLE, 0xFFFFFFFF); /* 关闭所有RTC中断 */
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

    RTC_RTCIF_ClrEx(RTC_RTCIE_SEC_IE_Msk); /* 清除秒中断标志 */

    NVIC_SET(RTC_IRQn, init->Nvicpry);

    GY_RTC_Get(&TempTime);
    if (!GY_IsLegal(&TempTime))
    {
        uint8_t Time[7] = {0x19, 0x03, 0x18, 0x10, 0x29, 0x58, 0x01};
        memcpy((uint8_t *)(&TempTime), Time, 7);
        API_SetTIME(&TempTime); /* 设置时间 */
    }
}
/*
*********************************************************************************************************
*	函 数 名: API_GetTime
*	功能说明: 获取RTC时间
*	形    参: RTC_INIT_Type *init
*	返 回 值: 无
*	备    注：0-6 BCD:年 年 月 日 时 分 秒  7-13 HEX:年 年 月 日 时 分 秒
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
