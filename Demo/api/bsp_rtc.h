/*
*********************************************************************************************************
*
*	模块名称 : 硬件RTC定义
*	文件名称 : bsp_rtc.h
*	版    本 : V1.0.0
*	说    明 : 声明和定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹    正式发布
*		V1.0.1  2021-1-20   高屹    新增 GetTimeStamp 和 GetTimStmpDif
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_def.h"

    typedef enum week
    {
        Mon = 1,
        Tue,
        Wed,
        Tur,
        Fri,
        Sat,
        Sun
    } WEEK;

    typedef enum mon
    {
        Jan = 1,
        Feb,
        Mar,
        Apr,
        May,
        Jun,
        Jul,
        Aug,
        Sep,
        Oct,
        Nov,
        Dec
    } MONTH;

    typedef struct
    {
        uint8_t second;
        uint8_t minute;
        uint8_t hour;
    } TIME;

    typedef struct
    {
        uint8_t day;
        MONTH month;
        uint16_t year;
        WEEK weekday;
    } DATE;

    typedef struct
    {
        DATE date; //NB校时功能
        TIME time;
    } Calendar_Type;

    /* 日期存储结构 */
    typedef struct
    {
        int year;  /* 年 */
        int month; /* 月 */
        int day;   /* 日 */
    } DataType;

    typedef struct
    {
        uint8_t Nvicpry; /* 中断优先级 */
        uint8_t day;     /* 0:不开启，1：开启 rtc天中断 */
        uint8_t hour;    /* 0:不开启，1：开启 rtc小时中断 */
        uint8_t min;     /* 0:不开启，1：开启 rtc分中断 */
        uint8_t sec;     /* 0:不开启，1：开启 rtc秒中断 */
    } RTC_INIT_Type;

    extern uint8_t RealTime[14];
    extern const uint8_t dpm[13];

#ifndef Bcd_YearH
#define Bcd_YearH RealTime[0]
#endif
#ifndef Bcd_YearL
#define Bcd_YearL RealTime[1]
#endif
#ifndef Bcd_Mon
#define Bcd_Mon RealTime[2]
#endif
#ifndef Bcd_Day
#define Bcd_Day RealTime[3]
#endif
#ifndef Bcd_Hour
#define Bcd_Hour RealTime[4]
#endif
#ifndef Bcd_Min
#define Bcd_Min RealTime[5]
#endif
#ifndef Bcd_Sec
#define Bcd_Sec RealTime[6]
#endif
#ifndef Hex_YearH
#define Hex_YearH RealTime[7]
#endif
#ifndef Hex_YearL
#define Hex_YearL RealTime[8]
#endif
#ifndef Hex_Mon
#define Hex_Mon RealTime[9]
#endif
#ifndef Hex_Day
#define Hex_Day RealTime[10]
#endif
#ifndef Hex_Hour
#define Hex_Hour RealTime[11]
#endif
#ifndef Hex_Min
#define Hex_Min RealTime[12]
#endif
#ifndef Hex_Sec
#define Hex_Sec RealTime[13]
#endif

#define GETTIME API_GetTime(RealTime)

    /**************************函数指针定义*************************/
    typedef void (*gy_rtcinit)(RTC_INIT_Type *init);
    typedef void (*gy_gttim)(uint8_t *tim);
    typedef uint8_t (*gy_sttim)(RTC_TimeDateTypeDef *para);

    /****************************函数声明****************************/
    int isLeap(int year);
    bool Check_date(uint16_t w_year, uint8_t w_month, uint8_t w_date);
    int dateIsErr(DataType d);
    DataType dateDelta(DataType start, int delta);
    void API_RTC_Init(RTC_INIT_Type *init);
    void API_GetTime(uint8_t *tim);
    void API_Set_Time_HEX(Calendar_Type *TIM);
    uint8_t API_SetTIME(RTC_TimeDateTypeDef *para);
    uint32_t GetTimeStamp(DataType day, int cnt);
    int GetTimStmpDif(uint32_t day1, uint32_t day2, uint16_t lmit);
    void API_Calendar(Calendar_Type *CLK);

#ifdef __cplusplus
}
#endif

#endif
