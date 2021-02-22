/*
*********************************************************************************************************
*
*	ģ������ : ��������ģ��
*	�ļ����� : etim_api.c
*	��    �� : V1.0.0
*	˵    �� : ʵ�ּ�����ع��� ��Ҫʹ��rtc_api.h�й���ʱ��Ĳ��ֶ���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-1-14   ����    ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
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

/* �궨����Ҫ�ṩ���壬һ������Ϊ�� */
#define USER_SCALER_ERR SCALER_ERR_flag     /* �ɻɻ���־ */
#define USER_MAGNETIC_ERR MAGNETIC_ERR_flag /* �Ÿ��ű�־ */
#define USER_VALVE_ERR VALVE_ERR_flag       /* �����쳣��־ */
#define USER_VALVE_STATE VALVE_STATE_flag   /* ���ű�־ OPENFA CLOSEFA */
#define USER_MAX_FLOW Max_Flow_flag         /* ����������־ */
#define USER_MAX_FLOW_VALE Max_Flow_Vale    /* ��������ֵ */
#define SCALER_1 JS1                        /* ��������1 */
#define SCALER_2 JS2                        /* ��������2 */
#define OPENFA 0                            /* ����ʵ������޸� */
#define CLOSEFA 1                           /* ����ʵ������޸� */
#define JSVAL_MIN_BASE 0                    /* ��¼�洢��ַ�׵�ַ */
#define MIN_SAVE_MAX 3                      /* ���Ӵ洢��¼����� */
#define HOUR_SAVE_MAX 30                    /* Сʱ�洢��¼����� */
#define DAY_SAVE_MAX 3650                   /* ��洢��¼����� */
#define ABNORMAL_SAVE_MAX 300               /* �쳣�洢��¼������� */
#define MAX_READ_DAY 7                      /* �ɶ�ȡ��������� */
#define LifeCycle 365 * 20                  /* ��������20�� */
const DataType StandardTime = {2021, 1, 1}; /* �������ڵ���ʼ��׼ʱ��� */

/* ����Ϊ�����޸ĵ����ݻ��߳��� */
const uint32_t DaySec = 86400; /* һ������� */
#define STS GetTimeStamp(StandardTime, 0)
#define JSVAL_HOUR_BASE (JSVAL_MIN_BASE + SM_LEN + (1440 * 1 + 4) * MIN_SAVE_MAX) /* 1440*1Ϊ1440���ӣ�ռ��1�ֽ� +�α� */
#define JSVAL_DAY_BASE (JSVAL_HOUR_BASE + SH_LEN + (24 * 2 + 4) * HOUR_SAVE_MAX)  /* 24*2Ϊ24Сʱ��ռ��2�ֽ� +�α� */
#define ABNORMAL_BASE (JSVAL_DAY_BASE + SD_LEN + (4 + 4) * DAY_SAVE_MAX)          /* 4�ֽ��� + 4�ֽ�ʱ��� + �α� */
#define SAVE_END (ABNORMAL_BASE + SA_LEN + (STATE_LEN + 4) * ABNORMAL_SAVE_MAX)
/* ȫ�ֱ��� */
JS_MEM JSMEM; /* ������ص�ȫ�ֱ��� */
JS_VAL JSVAL; /* ����ֵ */
/*
*********************************************************************************************************
*	�� �� ��: XXX_HANDLE
*	����˵��: XXX������
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע�������庯������Ҫ�Լ�д�������
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
*	�� �� ��: Abnormal_Save
*	����˵��: �쳣�Ĵ洢
*	��    ��: state���쳣״̬
*	�� �� ֵ: ��
*	��    ע����
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
*	�� �� ��: JSVAL_Save
*	����˵��: �ۻ����Ĵ洢
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע�����ڷ��Ӵ���
*             �洢��ʽ��
*             �ִ洢���� SM_LEN �ֽ� + ������ 1440��1�ֽ���(��ֵ) + 4�ֽ�ʱ������������洢MIN_SAVE_MAX
*             ʱ�洢���� SH_LEN �ֽ� + Сʱ�� 24��2�ֽ���(��ֵ) + 4�ֽ�ʱ������������洢HOUR_SAVE_MAX
*             ��洢���� SD_LEN �ֽ� + ������ 1��4�ֽ���(��ֵ) + 4�ֽ�ʱ������������洢DAY_SAVE_MAX
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
    JSVAL.VALUE_MIN = 0; /* ���Ӳ�����ÿ������һ�� */
    if (Hex_Min == 0)
    {
        /* ˵���������� */
        SaveData(JSVAL_HOUR_BASE + SH_LEN + (JSVAL.SAVE_HOUR * 52) + (Hex_Hour * 2), &JSVAL.VALUE_HOUR, 2);
        JSVAL.VALUE_HOUR = 0; /* Сʱ������ÿСʱ��һ�� */
        if (Hex_Hour == 0)
        {
            /* ˵���������죬����0ʱ */
            tt.year = 2000 + Hex_YearL;
            tt.month = Hex_Mon;
            tt.day = Hex_Day;
            current = GetTimeStamp(tt, -1); /* ����һ���ʱ��� */

            SaveData(JSVAL_DAY_BASE + SD_LEN + JSVAL.SAVE_DAY * 8, &JSVAL.VALUE_TOTAL, 4);

            /* ���뱣֤��һ��ʱ���С�ڵ��ڵ����ʱ���+DaySec */
            (JSVAL.SAVE_MIN == 0) ? (n = MIN_SAVE_MAX - 1) : (n = JSVAL.SAVE_MIN - 1);
            ReadData(JSVAL_MIN_BASE + SM_LEN + 1444 * n + 1440, &last, 4);
            if ((last > current) && (GetTimStmpDif(STS, last, LifeCycle) != -1))
            {
                /* ˵����ǰʱ���쳣�� */
                current = last + DaySec;
            }
            SaveData(JSVAL_MIN_BASE + SM_LEN + 1444 * JSVAL.SAVE_MIN + 1440, &current, 4);

            (JSVAL.SAVE_HOUR == 0) ? (n = HOUR_SAVE_MAX - 1) : (n = JSVAL.SAVE_HOUR - 1);
            ReadData(JSVAL_HOUR_BASE + SH_LEN + 52 * n + 48, &last, 4);
            if ((last > current) && (GetTimStmpDif(STS, last, LifeCycle) != -1))
            {
                /* ˵����ǰʱ���쳣�� */
                current = last + DaySec;
            }
            SaveData(JSVAL_HOUR_BASE + SH_LEN + 52 * JSVAL.SAVE_HOUR + 48, &current, 4);

            (JSVAL.SAVE_DAY == 0) ? (n = DAY_SAVE_MAX - 1) : (n = JSVAL.SAVE_DAY - 1);
            ReadData(JSVAL_DAY_BASE + SD_LEN + 8 * JSVAL.SAVE_DAY + 4, &last, 4);
            if ((last > current) && (GetTimStmpDif(STS, last, LifeCycle) != -1))
            {
                /* ˵����ǰʱ���쳣�� */
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
*	�� �� ��: DataFillingZero
*	����˵��: JSVAL_Read ��Ҫʹ�õ��Ӻ����������������
*	��    ��: day��ʱ���
*             distance���α���� day ʱ���������� distance������ڵ���cnt
*             cnt����Ҫ���Ĵ���
*             *buf���������Ļ���
*	�� �� ֵ: ��0��ʱ���ֵ��0������
*	��    ע����������ʽΪ��4�ֽڵ������� + 4�ֽڵ�ʱ�������8�ֽ�Ϊ1��
*********************************************************************************************************
*/
void DataFillingRead_Day(DataType day, uint16_t distance, uint16_t cnt, uint8_t *buf)
{
    uint32_t TT, RTT, i, j, k, n;
    if ((distance < cnt) || (distance > DAY_SAVE_MAX))
        return;
    for (j = 0; j < cnt; j++)
    {
        /* �� day �� cnt ֮��������ɴ洢�ļ�¼������� */
        k = 0;
        TT = GetTimeStamp(day, j);
        do
        {
            n = distance - j - k;
            (JSVAL.SAVE_DAY >= n) ? (i = JSVAL.SAVE_DAY - n) : (i = DAY_SAVE_MAX + JSVAL.SAVE_DAY - n);
            ReadData(JSVAL_DAY_BASE + SD_LEN + 8 * i + 4, &RTT, 4);
            if (RTT == TT)
            {
                /* �����˷��ϵ�ʱ�䣬����ۻ��� */
                ReadData(JSVAL_DAY_BASE + SD_LEN + 8 * i, &buf[j * 8], 4);
                break;
            }
            k++;
        } while (k < distance - j);
    }
}
/*
*********************************************************************************************************
*	�� �� ��: JSVAL_Read
*	����˵��: ���ݵĶ�ȡ
*	��    ��: start����ʼ����
*             end����������
*             type�����ҵ���������
*             buf����仺��
*	�� �� ֵ: 0�����ִ��������������ĳ���
*	��    ע������ start �� end Ϊͬһ��
*             ��type=2ʱ��buf ��Ҫ��Ҫ MAX_READ_DAY * 8 �Ĵ�С 
*             type=2ʱ��ʹû�ж�Ӧ����Ҳ��������
*             ��������ʽΪ��4�ֽڵ������� + 4�ֽڵ�ʱ�������8�ֽ�Ϊ1��
*********************************************************************************************************
*/
uint32_t JSVAL_Read(DataType start, DataType end, uint8_t type, uint8_t *buf)
{
    /* T1:startʱ���;T2:endʱ���;T3:�α�ʱ���; */
    uint32_t T1, T2, T3;
    uint32_t TT, j, k;
    DataType tt;
    volatile uint16_t i = 0;
    volatile int difday = 0;

    T1 = GetTimeStamp(start, 0);

    /* �����ʱ�䶼���ԣ���ʼ���ڱȽ������ڻ��� */
    if (T1 == 0 || type > 2)
        return 0;

    if (type == 0) /* ��ȡ�� */
    {
        /* һ��ֻ�����һ��ķ�����,ֻ�ж�T1 */
        for (i = 0; i < MIN_SAVE_MAX; i++)
        {
            /* ����������С�������α��� */
            ReadData(JSVAL_MIN_BASE + SM_LEN + 1444 * i + 1440, &TT, 4);
            if (TT == T1)
            {
                ReadData(JSVAL_MIN_BASE + SM_LEN + 1444 * i, buf, 1444);
                return 1444;
            }
        }
        return 0; /* û�ҵ� */
    }
    else if (type == 1)
    {
        /* һ��ֻ�����һ��ķ�����,ֻ�ж�T1 */
        /* ��ȡ�α� */
        ReadData(JSVAL_HOUR_BASE, &JSVAL.SAVE_HOUR, SH_LEN);
        /* �α��ȡ�쳣��ֱ�ӷ������������� */
        if (JSVAL.SAVE_HOUR >= HOUR_SAVE_MAX)
            return 0;
        /* ��ȡ��ǰ�α��ʱ��� */
        (JSVAL.SAVE_HOUR == 0) ? (i = HOUR_SAVE_MAX - 1) : (i = JSVAL.SAVE_HOUR - 1);
        ReadData(JSVAL_HOUR_BASE + SH_LEN + 52 * i + 48, &T3, 4);
        if (T3 < T1)
        {
            /* ˵���α�ʱ�����Ҫ��ȡ��ʱ�仹С�����洢�����ݸ���û��Ҫ���Ļ����α�ʱ��������� */
            return 0;
        }
        else
        {
            /* �����α�ʱ�������ʼʱ��ʱ���֮�����ľ��� */
            i = GetTimStmpDif(T1, T3, HOUR_SAVE_MAX) + 1;
            if (i > HOUR_SAVE_MAX)
                return 0;
            for (j = 0; j < i; j++)
            {
                /* �� day �� cnt ֮��������ɴ洢�ļ�¼������� */
                (JSVAL.SAVE_HOUR >= i - j) ? (k = JSVAL.SAVE_HOUR - i + j) : (k = HOUR_SAVE_MAX + JSVAL.SAVE_HOUR - i + j);
                ReadData(JSVAL_HOUR_BASE + SH_LEN + 52 * k + 48, &TT, 4);
                if (T1 == TT)
                {
                    /* �����˷��ϵ�ʱ�䣬����ۻ��� */
                    ReadData(JSVAL_HOUR_BASE + SH_LEN + 52 * k, buf, 52);
                    return 52;
                }
            }
        }
    }
    else if (type == 2) /* ��ȡ�� */
    {
        T2 = GetTimeStamp(end, 0);

        if (T2 == 0 || T1 > T2)
            return 0;
        difday = GetTimStmpDif(T1, T2, DAY_SAVE_MAX) + 1;
        if ((difday == 0) || (difday > MAX_READ_DAY))
            return 0;

        /* �Ƚ�������0���,�����ʱ��� */
        for (i = 0; i < difday; i++)
        {
            memset(&buf[i * 8], 0, 4); /* �ۻ��� */
            TT = GetTimeStamp(start, i);
            memcpy(&buf[i * 8 + 4], &TT, 4); /* ʱ��� */
        }

        /* ��ȡ�α� */
        ReadData(JSVAL_DAY_BASE, &JSVAL.SAVE_DAY, SD_LEN);

        /* �α��ȡ�쳣��ֱ�ӷ������������� */
        if (JSVAL.SAVE_DAY >= DAY_SAVE_MAX)
            return difday * 8;

        /* ��ȡ��ǰ�α��ʱ��� */
        (JSVAL.SAVE_DAY == 0) ? (i = DAY_SAVE_MAX - 1) : (i = JSVAL.SAVE_DAY - 1);
        ReadData(JSVAL_DAY_BASE + SD_LEN + 8 * i + 4, &T3, 4);

        if (T3 < T1)
        {
            /* ˵���α�ʱ�����Ҫ��ȡ��ʱ�仹С�����洢�����ݸ���û��Ҫ���Ļ����α�ʱ��������� */
        }
        else if (T3 > T2)
        {
            /* ˵��Ҫô��ȡ����������ʷ���ݣ�Ҫô�α�ʱ��������� */
            tt.year = 2000 + Hex_YearL;
            tt.month = Hex_Mon;
            tt.day = Hex_Day;
            TT = GetTimeStamp(tt, 0);

            /* ��ȡϵͳ�����ʱ������Դ�Ϊ�����ж϶�ȡ��ʱ����Ƿ���ȷ����������£�
               ��Ӧ���Ƕ�ȡ��ʱ���С�ڵ�ǰʱ�䣬�����ǰʱ���쳣���Ǿ�û�취�� */
            if (TT >= T3)
            {
                /* �����α�ʱ�������ʼʱ��ʱ���֮�����ľ��� */
                i = GetTimStmpDif(T1, T3, DAY_SAVE_MAX) + 1;
                DataFillingRead_Day(start, i, difday, buf);
            }
        }
        else
        {
            /* ˵���α�ʱ���ֱ�Ӿ���Ҫ��ȡ��ʱ�䷶Χ�� */
            i = GetTimStmpDif(T1, T3, difday) + 1;
            DataFillingRead_Day(start, i, i, buf);
        }

        return difday * 8;
    }
    return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: ReadState
*	����˵��: ��״̬
*	��    ��: ��
*	�� �� ֵ: JS_STATE������
*	��    ע����
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
*	�� �� ��: WriteState
*	����˵��: д״̬
*	��    ��: JS_STATE������
*	�� �� ֵ: ��
*	��    ע����
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
*	�� �� ��: MAGNETIC_ERR_IN_1S
*	����˵��: �Ÿ����ж�
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע������1s�Ķ�ʱ����
*********************************************************************************************************
*/
void MAGNETIC_ERR_IN_1S(void)
{
    JS_STATE state = ReadState();
    if ((SCALER_1 == RESET) && (SCALER_1 == RESET)) //�Ÿ���
    {
        if ((++JSMEM.CGRTimeCnt >= 5) && (state.MAGNETIC_ERR == 0))
        {
            /* �Ÿ�������5�룬�ôŸ���״̬ */
            JSMEM.CGRTimeCnt = 0;
            state.MAGNETIC_ERR = 1;
            /* �Ÿ��Ŵ��� */
            MAGNETIC_HANDLE();
            debug("\r\n�Ÿ���\r\n");
        }
        if (state.MAGNETIC_ERR)
        {
            debug("\r\n�Ÿ����Ѿ�������\r\n");
            JSMEM.CGRTimeCnt = 0;
        }
        debug("\r\n�Ÿ��ų�����%d��\r\n", JSMEM.CGRTimeCnt);
    }
    else
    {
        JSMEM.CGRTimeCnt = 0;
    }
    WriteState(state);
}
/*
*********************************************************************************************************
*	�� �� ��: ReedSwitchesFun
*	����˵��: ˫�ɻɼ������ܺ���
*	��    ��: Which��JS1Val �� JS2Val
*	�� �� ֵ: �������嵱��(����������)
*	��    ע�����ڼ����ĵط�
*********************************************************************************************************
*/
uint8_t ReedSwitchesFun(uint8_t Which)
{
    uint8_t increment = 0;
    JS_STATE state = ReadState();
    /*�Ÿ����ж�*/
    if ((SCALER_1 == RESET) && (SCALER_2 == RESET)) /* �Ÿ��� */
    {
        if ((++JSMEM.CGRCnt > 10) && (state.MAGNETIC_ERR == 0))
        {
            /* ��ʱ���������Ÿ���10��Ҳ��Ÿ���,�ôŸ��ű�־ */
            JSMEM.CGRCnt = 0;
            state.MAGNETIC_ERR = 1;
            /* �Ÿ��Ŵ��� */
            MAGNETIC_HANDLE();
            debug("�Ÿ���\r\n");
        }
        if (state.MAGNETIC_ERR)
        {
            JSMEM.CGRCnt = 0;
            debug("�Ѿ����ڴŸ���״̬\r\n");
        }
        debug("�Ÿ���������%d��\r\n", JSMEM.CGRCnt);
    }
    else
    {
        /* ���ٲ��ǴŸ��� */
        JSMEM.CGRCnt = 0;
        state.MAGNETIC_ERR = 0;
    }

    /* �ɻɻ��ж� */
    if (JSMEM.memjs == Which)
    {
        if ((++JSMEM.GHHCnt > 10) && (state.SCALER_ERR == 0))
        {
            //��������10�Σ����߼������ɻɻ�
            JSMEM.GHHCnt = 0;
            state.SCALER_ERR = 1;
            /* �ɻɻ����� */
            SCALER_HANDLE();
            debug("�ɻɻ�\r\n");
        }
        if (state.SCALER_ERR)
        {
            JSMEM.GHHCnt = 0;
            debug("�Ѿ����ڸɻɻ�״̬\r\n");
        }
        debug("�ɻɻ�������%d��\r\n", JSMEM.GHHCnt);
    }
    else
    {
        JSMEM.memjs = Which;
        JSMEM.GHHCnt = 0;
        state.SCALER_ERR = 0;
        debug("�ɻ�%d����\r\n", Which);
    }

    /* ����ֱͨ */
    if (state.VALVE_STATE == CLOSEFA)
    {
        if ((state.VALVE_ERR == 0) && (++JSMEM.ZTYQCnt > 10))
        {
            JSMEM.ZTYQCnt = 0;
            state.VALVE_ERR = 1;
            /* �����쳣���� */
            VALVE_ERR_HANDLE();
            debug("����ֱͨ\r\n");
        }
        if (state.VALVE_ERR)
        {
            JSMEM.ZTYQCnt = 0;
            debug("�Ѿ����ڷ���ֱͨ״̬\r\n");
        }
        debug("����ֱͨ������%d��\r\n", JSMEM.ZTYQCnt);
    }
    else
    {
        //���ſ���״̬ʱ�������������
        JSMEM.ZTYQCnt = 0;
        state.VALVE_ERR = 0;
    }

    if ((JS1Val == Which) && (JSMEM.jsallow == 1))
    {
        JSMEM.jsallow = 0;
        /*����*/
        increment = PulseEquivalent;

        /* ��������� */
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
            debug("���μ��������%d��\r\n���������Ϊ:%2.3fL/s\r\n", FlowCnt, frate);

            if (FlowCnt <= USER_MAX_FLOW_VALE)
            {
                if ((++JSMEM.MaxRateCnt > 10) && (state.MAX_FLOW == 0))
                {
                    state.MAX_FLOW = 1;
                    JSMEM.MaxRateCnt = 0;
                    /* ��������� */
                    MAX_FLOW_HANDLE();
                    debug("����������\r\n");
                }
                debug("������������%d��\r\n", JSMEM.MaxRateCnt);
            }
            else
            {
                JSMEM.MaxRateCnt = 0;
            }
        }
        else
        {
            debug("δ�������������Ѿ������˳������쳣\r\n");
        }

        /* �����ͼ�����صĲ����� */
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
*	�� �� ��: ReedSwitchesCount
*	����˵��: ˫�ɻɼ������ܺ�������
*	��    ��: Which��JS1Val �� JS2Val
*	�� �� ֵ: ��
*	��    ע�����ڼ����ĵط�
*********************************************************************************************************
*/
void ReedSwitchesCount(uint8_t Which)
{
    uint8_t val = ReedSwitchesFun(Which);
    JSVAL.VALUE_HOUR += val;
    JSVAL.VALUE_MIN += val;
    JSVAL.VALUE_TOTAL += val;
    debug("��ǰ�ۻ�����%d\r\n", JSVAL.VALUE_TOTAL);
}
