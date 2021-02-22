/*
*********************************************************************************************************
*
*	模块名称 : 计数功能模块
*	文件名称 : etim_api.c
*	版    本 : V1.0.0
*	说    明 : 实现计数相关功能 需要使用rtc_api.h中关于时间的部分定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-1-14   高屹    正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "Countingmodule.h"

#include "event.h"

uint8_t SCALER_ERR_flag = 0;
uint8_t MAGNETIC_ERR_flag = 0;
uint8_t VALVE_ERR_flag = 0;
uint8_t VALVE_STATE_flag = 0;
uint8_t Max_Flow_flag = 0;
uint8_t Max_Flow_Vale = 0;

uint8_t hex_sec;
uint8_t hex_min;
uint8_t hex_hour;
uint8_t hex_day;
uint8_t hex_mon;
uint8_t hex_yearL;

#ifndef Hex_Sec
#define Hex_Sec hex_sec
#endif
#ifndef Hex_Min
#define Hex_Min hex_min
#endif
#ifndef Hex_Hour
#define Hex_Hour hex_hour
#endif
#ifndef Hex_Day
#define Hex_Day hex_day
#endif
#ifndef Hex_Mon
#define Hex_Mon hex_mon
#endif
#ifndef Hex_YearL
#define Hex_YearL hex_yearL
#endif

#define JS1_GPIO GPIOA
#define JS1_PIN GPIO_Pin_12
#define JS1 GPIO_ReadInputDataBit(JS1_GPIO, JS1_PIN)

#define JS2_GPIO GPIOA
#define JS2_PIN GPIO_Pin_13
#define JS2 GPIO_ReadInputDataBit(JS2_GPIO, JS2_PIN)

#define SaveData(base, buf, len)
#define ReadData(base, buf, len)

/* 宏定义需要提供定义，一定不能为空 */
#define USER_SCALER_ERR SCALER_ERR_flag     /* 干簧坏标志 */
#define USER_MAGNETIC_ERR MAGNETIC_ERR_flag /* 磁干扰标志 */
#define USER_VALVE_ERR VALVE_ERR_flag       /* 阀门异常标志 */
#define USER_VALVE_STATE VALVE_STATE_flag   /* 阀门标志 OPENFA CLOSEFA */
#define USER_MAX_FLOW Max_Flow_flag         /* 超大流量标志 */
#define USER_MAX_FLOW_VALE Max_Flow_Vale    /* 超大流量值 */
#define SCALER_1 JS1                        /* 计数口线1 */
#define SCALER_2 JS2                        /* 计数口线2 */
#define OPENFA 0                            /* 根据实际情况修改 */
#define CLOSEFA 1                           /* 根据实际情况修改 */
#define JSVAL_MIN_BASE 0                    /* 记录存储基址首地址 */
#define MIN_SAVE_MAX 3                      /* 分钟存储记录最大数 */
#define HOUR_SAVE_MAX 30                    /* 小时存储记录最大数 */
#define DAY_SAVE_MAX 3650                   /* 天存储记录最大数 */
#define ABNORMAL_SAVE_MAX 300               /* 异常存储记录最大数量 */
#define MAX_READ_DAY 7                      /* 可读取的最大天数 */
#define LifeCycle 365 * 20                  /* 生命周期20年 */
const DataType StandardTime = {2021, 1, 1}; /* 生命周期的起始标准时间点 */

/* 以下为不用修改的数据或者常量 */
const uint32_t DaySec = 86400; /* 一天的秒数 */
#define STS GetTimeStamp(StandardTime, 0)
#define JSVAL_HOUR_BASE (JSVAL_MIN_BASE + SM_LEN + (1440 * 1 + 4) * MIN_SAVE_MAX) /* 1440*1为1440分钟，占用1字节 +游标 */
#define JSVAL_DAY_BASE (JSVAL_HOUR_BASE + SH_LEN + (24 * 2 + 4) * HOUR_SAVE_MAX)  /* 24*2为24小时，占用2字节 +游标 */
#define ABNORMAL_BASE (JSVAL_DAY_BASE + SD_LEN + (4 + 4) * DAY_SAVE_MAX)          /* 4字节量 + 4字节时间戳 + 游标 */
#define SAVE_END (ABNORMAL_BASE + SA_LEN + (STATE_LEN + 4) * ABNORMAL_SAVE_MAX)
/* 全局变量 */
JS_MEM JSMEM; /* 计数相关的全局变量 */
JS_VAL JSVAL; /* 计数值 */
/*
*********************************************************************************************************
*	函 数 名: XXX_HANDLE
*	功能说明: XXX处理函数
*	形    参: 无
*	返 回 值: 无
*	备    注：弱定义函数，需要自己写处理过程
*********************************************************************************************************
*/
__weak void MAGNETIC_HANDLE(void)
{
}
__weak void SCALER_HANDLE(void)
{
}
__weak void VALVE_ERR_HANDLE(void)
{
}
__weak void MAX_FLOW_HANDLE(void)
{
}
__weak void OTHER_HANDLE(void)
{
}
/*
*********************************************************************************************************
*	函 数 名: Abnormal_Save
*	功能说明: 异常的存储
*	形    参: state：异常状态
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void Abnormal_Save(JS_STATE state)
{
    static uint8_t isread = 0;
    struct tm stm = {0};
    uint32_t TT;

    if (!state.ST)
        return;
    if (isread == 0)
    {
        ReadData(ABNORMAL_BASE, &JSVAL.SAVE_ABNORMAL, SA_LEN);
        isread = 1;
    }
    if (JSVAL.SAVE_ABNORMAL >= ABNORMAL_SAVE_MAX)
        JSVAL.SAVE_ABNORMAL = 0;

    stm.tm_year = 100 + Hex_YearL;
    stm.tm_mon = Hex_Mon - 1;
    stm.tm_mday = Hex_Day;
    stm.tm_hour = Hex_Hour;
    stm.tm_min = Hex_Min;
    stm.tm_sec = Hex_Sec;
    SaveData(ABNORMAL_BASE + SA_LEN + JSVAL.SAVE_ABNORMAL * (STATE_LEN + 4), &state.ST, STATE_LEN);
    TT = mktime(&stm);
    SaveData(ABNORMAL_BASE + SA_LEN + JSVAL.SAVE_ABNORMAL * (STATE_LEN + 4) + 4, &TT, 4);

    JSVAL.SAVE_ABNORMAL++;
    JSVAL.SAVE_ABNORMAL %= ABNORMAL_SAVE_MAX;

    SaveData(ABNORMAL_BASE, &JSVAL.SAVE_ABNORMAL, SA_LEN);
}
/*
*********************************************************************************************************
*	函 数 名: JSVAL_Save
*	功能说明: 累积量的存储
*	形    参: 无
*	返 回 值: 无
*	备    注：放在分钟处理
*             存储格式：
*             分存储计数 SM_LEN 字节 + 分钟量 1440个1字节量(差值) + 4字节时间戳，连续最大存储MIN_SAVE_MAX
*             时存储计数 SH_LEN 字节 + 小时量 24个2字节量(差值) + 4字节时间戳，连续最大存储HOUR_SAVE_MAX
*             天存储计数 SD_LEN 字节 + 天用量 1个4字节量(总值) + 4字节时间戳，连续最大存储DAY_SAVE_MAX
*********************************************************************************************************
*/
void JSVAL_Save(void)
{
    static uint8_t isread = 0;
    uint32_t current, last, n;
    DataType tt;

    if (isread == 0)
    {
        ReadData(JSVAL_MIN_BASE, &JSVAL.SAVE_MIN, SM_LEN);
        ReadData(JSVAL_HOUR_BASE, &JSVAL.SAVE_HOUR, SH_LEN);
        ReadData(JSVAL_DAY_BASE, &JSVAL.SAVE_DAY, SD_LEN);
        isread = 1;
    }

    if (JSVAL.SAVE_MIN >= MIN_SAVE_MAX)
        JSVAL.SAVE_MIN = 0;
    if (JSVAL.SAVE_HOUR >= HOUR_SAVE_MAX)
        JSVAL.SAVE_HOUR = 0;
    if (JSVAL.SAVE_DAY >= DAY_SAVE_MAX)
        JSVAL.SAVE_DAY = 0;

    SaveData(JSVAL_MIN_BASE + SM_LEN + (JSVAL.SAVE_MIN * 1444) + (Hex_Hour * 60 + Hex_Min), &JSVAL.VALUE_MIN, 1);
    JSVAL.VALUE_MIN = 0; /* 分钟差量，每分钟清一次 */
    if (Hex_Min == 0)
    {
        /* 说明到了整点 */
        SaveData(JSVAL_HOUR_BASE + SH_LEN + (JSVAL.SAVE_HOUR * 52) + (Hex_Hour * 2), &JSVAL.VALUE_HOUR, 2);
        JSVAL.VALUE_HOUR = 0; /* 小时差量，每小时清一次 */
        if (Hex_Hour == 0)
        {
            /* 说明到了整天，当日0时 */
            tt.year = 2000 + Hex_YearL;
            tt.month = Hex_Mon;
            tt.day = Hex_Day;
            current = GetTimeStamp(tt, -1); /* 存上一天的时间戳 */

            SaveData(JSVAL_DAY_BASE + SD_LEN + JSVAL.SAVE_DAY * 8, &JSVAL.VALUE_TOTAL, 4);

            /* 必须保证上一个时间戳小于等于当天的时间戳+DaySec */
            (JSVAL.SAVE_MIN == 0) ? (n = MIN_SAVE_MAX - 1) : (n = JSVAL.SAVE_MIN - 1);
            ReadData(JSVAL_MIN_BASE + SM_LEN + 1444 * n + 1440, &last, 4);
            if ((last > current) && (GetTimStmpDif(STS, last, LifeCycle) != -1))
            {
                /* 说明当前时间异常了 */
                current = last + DaySec;
            }
            SaveData(JSVAL_MIN_BASE + SM_LEN + 1444 * JSVAL.SAVE_MIN + 1440, &current, 4);

            (JSVAL.SAVE_HOUR == 0) ? (n = HOUR_SAVE_MAX - 1) : (n = JSVAL.SAVE_HOUR - 1);
            ReadData(JSVAL_HOUR_BASE + SH_LEN + 52 * n + 48, &last, 4);
            if ((last > current) && (GetTimStmpDif(STS, last, LifeCycle) != -1))
            {
                /* 说明当前时间异常了 */
                current = last + DaySec;
            }
            SaveData(JSVAL_HOUR_BASE + SH_LEN + 52 * JSVAL.SAVE_HOUR + 48, &current, 4);

            (JSVAL.SAVE_DAY == 0) ? (n = DAY_SAVE_MAX - 1) : (n = JSVAL.SAVE_DAY - 1);
            ReadData(JSVAL_DAY_BASE + SD_LEN + 8 * JSVAL.SAVE_DAY + 4, &last, 4);
            if ((last > current) && (GetTimStmpDif(STS, last, LifeCycle) != -1))
            {
                /* 说明当前时间异常了 */
                current = last + DaySec;
            }
            SaveData(JSVAL_DAY_BASE + SD_LEN + 8 * JSVAL.SAVE_DAY + 4, &current, 4);

            JSVAL.SAVE_MIN++;
            JSVAL.SAVE_MIN %= MIN_SAVE_MAX;

            JSVAL.SAVE_HOUR++;
            JSVAL.SAVE_HOUR %= HOUR_SAVE_MAX;

            JSVAL.SAVE_DAY++;
            JSVAL.SAVE_DAY %= DAY_SAVE_MAX;

            SaveData(JSVAL_MIN_BASE, &JSVAL.SAVE_MIN, SM_LEN);
            SaveData(JSVAL_HOUR_BASE, &JSVAL.SAVE_HOUR, SH_LEN);
            SaveData(JSVAL_DAY_BASE, &JSVAL.SAVE_DAY, SD_LEN);
        }
    }
}
/*
*********************************************************************************************************
*	函 数 名: DataFillingZero
*	功能说明: JSVAL_Read 需要使用的子函数，用于数据填充
*	形    参: day：时间点
*             distance：游标距离 day 时间点的天数差 distance必须大于等于cnt
*             cnt：需要填充的次数
*             *buf：用于填充的缓存
*	返 回 值: 非0：时间戳值，0：错误
*	备    注：数据填充格式为：4字节的数据量 + 4字节的时间戳，即8字节为1组
*********************************************************************************************************
*/
void DataFillingRead_Day(DataType day, uint16_t distance, uint16_t cnt, uint8_t *buf)
{
    uint32_t TT, RTT, i, j, k, n;
    if ((distance < cnt) || (distance > DAY_SAVE_MAX))
        return;
    for (j = 0; j < cnt; j++)
    {
        /* 将 day 到 cnt 之间的数据由存储的记录进行填充 */
        k = 0;
        TT = GetTimeStamp(day, j);
        do
        {
            n = distance - j - k;
            (JSVAL.SAVE_DAY >= n) ? (i = JSVAL.SAVE_DAY - n) : (i = DAY_SAVE_MAX + JSVAL.SAVE_DAY - n);
            ReadData(JSVAL_DAY_BASE + SD_LEN + 8 * i + 4, &RTT, 4);
            if (RTT == TT)
            {
                /* 读出了符合的时间，填充累积量 */
                ReadData(JSVAL_DAY_BASE + SD_LEN + 8 * i, &buf[j * 8], 4);
                break;
            }
            k++;
        } while (k < distance - j);
    }
}
/*
*********************************************************************************************************
*	函 数 名: JSVAL_Read
*	功能说明: 数据的读取
*	形    参: start：开始日期
*             end：结束日期
*             type：查找的数据类型
*             buf：填充缓存
*	返 回 值: 0：出现错误，其他返回填充的长度
*	备    注：允许 start 和 end 为同一天
*             当type=2时，buf 需要需要 MAX_READ_DAY * 8 的大小 
*             type=2时即使没有对应数据也会进行填充
*             数据填充格式为：4字节的数据量 + 4字节的时间戳，即8字节为1组
*********************************************************************************************************
*/
uint32_t JSVAL_Read(DataType start, DataType end, uint8_t type, uint8_t *buf)
{
    /* T1:start时间戳;T2:end时间戳;T3:游标时间戳; */
    uint32_t T1, T2, T3;
    uint32_t TT, j, k;
    DataType tt;
    volatile uint16_t i = 0;
    volatile int difday = 0;

    T1 = GetTimeStamp(start, 0);

    /* 输入的时间都不对，起始日期比结束日期还大 */
    if (T1 == 0 || type > 2)
        return 0;

    if (type == 0) /* 读取分 */
    {
        /* 一次只允许读一天的分钟量,只判断T1 */
        for (i = 0; i < MIN_SAVE_MAX; i++)
        {
            /* 由于数据量小，不查游标了 */
            ReadData(JSVAL_MIN_BASE + SM_LEN + 1444 * i + 1440, &TT, 4);
            if (TT == T1)
            {
                ReadData(JSVAL_MIN_BASE + SM_LEN + 1444 * i, buf, 1444);
                return 1444;
            }
        }
        return 0; /* 没找到 */
    }
    else if (type == 1)
    {
        /* 一次只允许读一天的分钟量,只判断T1 */
        /* 读取游标 */
        ReadData(JSVAL_HOUR_BASE, &JSVAL.SAVE_HOUR, SH_LEN);
        /* 游标读取异常，直接返回填充过的数据 */
        if (JSVAL.SAVE_HOUR >= HOUR_SAVE_MAX)
            return 0;
        /* 读取当前游标的时间戳 */
        (JSVAL.SAVE_HOUR == 0) ? (i = HOUR_SAVE_MAX - 1) : (i = JSVAL.SAVE_HOUR - 1);
        ReadData(JSVAL_HOUR_BASE + SH_LEN + 52 * i + 48, &T3, 4);
        if (T3 < T1)
        {
            /* 说明游标时间戳比要读取的时间还小，即存储的数据根本没有要读的或者游标时间戳有问题 */
            return 0;
        }
        else
        {
            /* 计算游标时间戳和起始时间时间戳之间相差的距离 */
            i = GetTimStmpDif(T1, T3, HOUR_SAVE_MAX) + 1;
            if (i > HOUR_SAVE_MAX)
                return 0;
            for (j = 0; j < i; j++)
            {
                /* 将 day 到 cnt 之间的数据由存储的记录进行填充 */
                (JSVAL.SAVE_HOUR >= i - j) ? (k = JSVAL.SAVE_HOUR - i + j) : (k = HOUR_SAVE_MAX + JSVAL.SAVE_HOUR - i + j);
                ReadData(JSVAL_HOUR_BASE + SH_LEN + 52 * k + 48, &TT, 4);
                if (T1 == TT)
                {
                    /* 读出了符合的时间，填充累积量 */
                    ReadData(JSVAL_HOUR_BASE + SH_LEN + 52 * k, buf, 52);
                    return 52;
                }
            }
        }
    }
    else if (type == 2) /* 读取天 */
    {
        T2 = GetTimeStamp(end, 0);

        if (T2 == 0 || T1 > T2)
            return 0;
        difday = GetTimStmpDif(T1, T2, DAY_SAVE_MAX) + 1;
        if ((difday == 0) || (difday > MAX_READ_DAY))
            return 0;

        /* 先将数据用0填充,并添加时间戳 */
        for (i = 0; i < difday; i++)
        {
            memset(&buf[i * 8], 0, 4); /* 累积量 */
            TT = GetTimeStamp(start, i);
            memcpy(&buf[i * 8 + 4], &TT, 4); /* 时间戳 */
        }

        /* 读取游标 */
        ReadData(JSVAL_DAY_BASE, &JSVAL.SAVE_DAY, SD_LEN);

        /* 游标读取异常，直接返回填充过的数据 */
        if (JSVAL.SAVE_DAY >= DAY_SAVE_MAX)
            return difday * 8;

        /* 读取当前游标的时间戳 */
        (JSVAL.SAVE_DAY == 0) ? (i = DAY_SAVE_MAX - 1) : (i = JSVAL.SAVE_DAY - 1);
        ReadData(JSVAL_DAY_BASE + SD_LEN + 8 * i + 4, &T3, 4);

        if (T3 < T1)
        {
            /* 说明游标时间戳比要读取的时间还小，即存储的数据根本没有要读的或者游标时间戳有问题 */
        }
        else if (T3 > T2)
        {
            /* 说明要么读取的数据是历史数据，要么游标时间戳有问题 */
            tt.year = 2000 + Hex_YearL;
            tt.month = Hex_Mon;
            tt.day = Hex_Day;
            TT = GetTimeStamp(tt, 0);

            /* 获取系统当天的时间戳，以此为依据判断读取的时间戳是否正确，正常情况下，
               都应该是读取的时间戳小于当前时间，如果当前时间异常，那就没办法了 */
            if (TT >= T3)
            {
                /* 计算游标时间戳和起始时间时间戳之间相差的距离 */
                i = GetTimStmpDif(T1, T3, DAY_SAVE_MAX) + 1;
                DataFillingRead_Day(start, i, difday, buf);
            }
        }
        else
        {
            /* 说明游标时间戳直接就在要读取的时间范围内 */
            i = GetTimStmpDif(T1, T3, difday) + 1;
            DataFillingRead_Day(start, i, i, buf);
        }

        return difday * 8;
    }
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: ReadState
*	功能说明: 读状态
*	形    参: 无
*	返 回 值: JS_STATE共用体
*	备    注：无
*********************************************************************************************************
*/
JS_STATE ReadState(void)
{
    JS_STATE state;
    USER_SCALER_ERR ? (state.SCALER_ERR = 1) : (state.SCALER_ERR = 0);
    USER_MAGNETIC_ERR ? (state.MAGNETIC_ERR = 1) : (state.MAGNETIC_ERR = 0);
    USER_VALVE_ERR ? (state.VALVE_ERR = 1) : (state.VALVE_ERR = 0);
    USER_VALVE_STATE ? (state.VALVE_STATE = 1) : (state.VALVE_STATE = 0);
    USER_MAX_FLOW ? (state.MAX_FLOW = 1) : (state.MAX_FLOW = 0);
    return state;
}
/*
*********************************************************************************************************
*	函 数 名: WriteState
*	功能说明: 写状态
*	形    参: JS_STATE共用体
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void WriteState(JS_STATE state)
{
    state.SCALER_ERR ? (USER_SCALER_ERR = 1) : (USER_SCALER_ERR = 0);
    state.MAGNETIC_ERR ? (USER_MAGNETIC_ERR = 1) : (USER_MAGNETIC_ERR = 0);
    state.VALVE_ERR ? (USER_VALVE_ERR = 1) : (USER_VALVE_ERR = 0);
    state.VALVE_STATE ? (USER_VALVE_STATE = 1) : (USER_VALVE_STATE = 0);
    state.MAX_FLOW ? (USER_MAX_FLOW = 1) : (USER_MAX_FLOW = 0);
    Abnormal_Save(state);
}
/*
*********************************************************************************************************
*	函 数 名: MAGNETIC_ERR_IN_1S
*	功能说明: 磁干扰判断
*	形    参: 无
*	返 回 值: 无
*	备    注：放在1s的定时器里
*********************************************************************************************************
*/
void MAGNETIC_ERR_IN_1S(void)
{
    JS_STATE state = ReadState();
    if ((SCALER_1 == RESET) && (SCALER_1 == RESET)) //磁干扰
    {
        if ((++JSMEM.CGRTimeCnt >= 5) && (state.MAGNETIC_ERR == 0))
        {
            /* 磁干扰连续5秒，置磁干扰状态 */
            JSMEM.CGRTimeCnt = 0;
            state.MAGNETIC_ERR = 1;
            /* 磁干扰处理 */
            MAGNETIC_HANDLE();
            debug("\r\n磁干扰\r\n");
        }
        if (state.MAGNETIC_ERR)
        {
            debug("\r\n磁干扰已经产生了\r\n");
            JSMEM.CGRTimeCnt = 0;
        }
        debug("\r\n磁干扰持续了%d秒\r\n", JSMEM.CGRTimeCnt);
    }
    else
    {
        JSMEM.CGRTimeCnt = 0;
    }
    WriteState(state);
}
/*
*********************************************************************************************************
*	函 数 名: ReedSwitchesFun
*	功能说明: 双干簧计数功能函数
*	形    参: Which：JS1Val 或 JS2Val
*	返 回 值: 返回脉冲当量(若产生计数)
*	备    注：放在计数的地方
*********************************************************************************************************
*/
uint8_t ReedSwitchesFun(uint8_t Which)
{
    uint8_t increment = 0;
    JS_STATE state = ReadState();
    /*磁干扰判断*/
    if ((SCALER_1 == RESET) && (SCALER_2 == RESET)) /* 磁干扰 */
    {
        if ((++JSMEM.CGRCnt > 10) && (state.MAGNETIC_ERR == 0))
        {
            /* 短时间内连续磁干扰10次也算磁干扰,置磁干扰标志 */
            JSMEM.CGRCnt = 0;
            state.MAGNETIC_ERR = 1;
            /* 磁干扰处理 */
            MAGNETIC_HANDLE();
            debug("磁干扰\r\n");
        }
        if (state.MAGNETIC_ERR)
        {
            JSMEM.CGRCnt = 0;
            debug("已经处于磁干扰状态\r\n");
        }
        debug("磁干扰连续了%d次\r\n", JSMEM.CGRCnt);
    }
    else
    {
        /* 至少不是磁干扰 */
        JSMEM.CGRCnt = 0;
        state.MAGNETIC_ERR = 0;
    }

    /* 干簧坏判断 */
    if (JSMEM.memjs == Which)
    {
        if ((++JSMEM.GHHCnt > 10) && (state.SCALER_ERR == 0))
        {
            //连续计数10次，单边计数，干簧坏
            JSMEM.GHHCnt = 0;
            state.SCALER_ERR = 1;
            /* 干簧坏处理 */
            SCALER_HANDLE();
            debug("干簧坏\r\n");
        }
        if (state.SCALER_ERR)
        {
            JSMEM.GHHCnt = 0;
            debug("已经处于干簧坏状态\r\n");
        }
        debug("干簧坏连续了%d次\r\n", JSMEM.GHHCnt);
    }
    else
    {
        JSMEM.memjs = Which;
        JSMEM.GHHCnt = 0;
        state.SCALER_ERR = 0;
        debug("干簧%d计数\r\n", Which);
    }

    /* 阀门直通 */
    if (state.VALVE_STATE == CLOSEFA)
    {
        if ((state.VALVE_ERR == 0) && (++JSMEM.ZTYQCnt > 10))
        {
            JSMEM.ZTYQCnt = 0;
            state.VALVE_ERR = 1;
            /* 阀门异常处理 */
            VALVE_ERR_HANDLE();
            debug("阀门直通\r\n");
        }
        if (state.VALVE_ERR)
        {
            JSMEM.ZTYQCnt = 0;
            debug("已经处于阀门直通状态\r\n");
        }
        debug("阀门直通连续了%d次\r\n", JSMEM.ZTYQCnt);
    }
    else
    {
        //阀门开阀状态时计数，清除限制
        JSMEM.ZTYQCnt = 0;
        state.VALVE_ERR = 0;
    }

    if ((JS1Val == Which) && (JSMEM.jsallow == 1))
    {
        JSMEM.jsallow = 0;
        /*计数*/
        increment = PulseEquivalent;

        /* 超大流相关 */
        if ((USER_MAX_FLOW_VALE != 0) && (state.MAX_FLOW == 1))
        {
            struct tm stm;
            time_t MemTT;
            uint8_t FlowCnt = 0;
            stm.tm_year = Hex_YearL + 100;
            stm.tm_mon = Hex_Mon - 1;
            stm.tm_mday = Hex_Day;
            stm.tm_hour = Hex_Hour;
            stm.tm_min = Hex_Min;
            stm.tm_sec = Hex_Sec;
            MemTT = mktime(&stm);
            if (MemTT > JSMEM.MemT)
            {
                JSMEM.MemT = MemTT - JSMEM.MemT;
                (JSMEM.MemT > 0xff) ? (FlowCnt = 0xff) : (FlowCnt = JSMEM.MemT);
                JSMEM.MemT = MemTT;
            }

            float frate = (float)10 / FlowCnt;
            debug("两次计数间隔了%d秒\r\n换算成流量为:%2.3fL/s\r\n", FlowCnt, frate);

            if (FlowCnt <= USER_MAX_FLOW_VALE)
            {
                if ((++JSMEM.MaxRateCnt > 10) && (state.MAX_FLOW == 0))
                {
                    state.MAX_FLOW = 1;
                    JSMEM.MaxRateCnt = 0;
                    /* 超大流相关 */
                    MAX_FLOW_HANDLE();
                    debug("产生超大流\r\n");
                }
                debug("超大流连续了%d次\r\n", JSMEM.MaxRateCnt);
            }
            else
            {
                JSMEM.MaxRateCnt = 0;
            }
        }
        else
        {
            debug("未开启超大流或已经产生了超大流异常\r\n");
        }

        /* 其他和计量相关的操作？ */
        OTHER_HANDLE();
    }

    if (JS2Val == Which)
    {
        JSMEM.jsallow = 1;
    }

    WriteState(state);

    return increment;
}
/*
*********************************************************************************************************
*	函 数 名: ReedSwitchesCount
*	功能说明: 双干簧计数功能函数延伸
*	形    参: Which：JS1Val 或 JS2Val
*	返 回 值: 无
*	备    注：放在计数的地方
*********************************************************************************************************
*/
void ReedSwitchesCount(uint8_t Which)
{
    uint8_t val = ReedSwitchesFun(Which);
    JSVAL.VALUE_HOUR += val;
    JSVAL.VALUE_MIN += val;
    JSVAL.VALUE_TOTAL += val;
    debug("当前累积量：%d\r\n", JSVAL.VALUE_TOTAL);
}
