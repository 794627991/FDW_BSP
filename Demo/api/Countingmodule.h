#ifndef __UCOSINIT_H__
#define __UCOSINIT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

#define PulseEquivalent 10 /* 脉冲当量 */

#define JS1Val 1
#define JS2Val 2

    typedef union
    {
        uint16_t ST;
        struct
        {
            uint8_t SCALER_ERR : 1;   /* 计数器坏 */
            uint8_t MAGNETIC_ERR : 1; /* 磁干扰 */
            uint8_t VALVE_ERR : 1;    /* 阀门异常 */
            uint8_t VALVE_STATE : 1;  /* 阀门状态 */
            uint8_t MAX_FLOW : 1;     /* 超大流 */
            uint8_t NC1 : 1;
            uint8_t NC2 : 1;
            uint8_t NC3 : 1;

            uint8_t NC4 : 1;
            uint8_t NC5 : 1;
            uint8_t NC6 : 1;
            uint8_t NC7 : 1;
            uint8_t NC8 : 1;
            uint8_t NC9 : 1;
            uint8_t NC10 : 1;
            uint8_t NC11 : 1;
        };
    } JS_STATE;

    typedef struct
    {
        uint8_t memjs;      /* 判断上一次是哪一个干簧 */
        uint8_t jsallow;    /* 允许计数的标志 */
        uint8_t CGRTimeCnt; /* 磁干扰持续了多长时间 */
        uint8_t CGRCnt;     /* 磁干扰连续产生了几次 */
        uint8_t GHHCnt;     /* 干簧坏连续产生了几次 */
        uint8_t ZTYQCnt;    /* 阀门直通连续产生了几次 */
        uint8_t MaxRateCnt; /* 超大流连续产生了几次 */
        time_t MemT;
    } JS_MEM;

    typedef struct
    {
        uint8_t VALUE_MIN;    /* 计数值分钟量 */
        uint16_t VALUE_HOUR;  /* 计数值小时量 */
        uint32_t VALUE_TOTAL; /* 计数值总量 */
        uint8_t SAVE_MIN;
        uint8_t SAVE_HOUR;
        uint16_t SAVE_DAY;
        uint16_t SAVE_ABNORMAL;
    } JS_VAL, *jsval;

#define SM_LEN (sizeof(((jsval)0)->VALUE_MIN))
#define SH_LEN (sizeof(((jsval)0)->SAVE_HOUR))
#define SD_LEN (sizeof(((jsval)0)->SAVE_DAY))
#define SA_LEN (sizeof(((jsval)0)->SAVE_ABNORMAL))
#define STATE_LEN (sizeof(JS_STATE))

#ifdef __cplusplus
}
#endif

#endif
