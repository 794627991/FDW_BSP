/*
*********************************************************************************************************
*
*	模块名称 : 新版IOT联网声明
*	文件名称 : iot_core.h
*	版    本 : V1.0.0
*	说    明 : 相关声明，以及部分沾粘定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __IOT_CORE_H__
#define __IOT_CORE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MAX_CMD_NAME_LENGTH 10 /* 字典命令名最大字符长度 */

#ifndef CoapNum
#define CoapNum 11
#endif
#ifndef InitNum
#define InitNum 22
#endif
#ifndef TCPNum
#define TCPNum 33
#endif
#ifndef UDPNum
#define UDPNum 55
#endif
#ifndef OneNET
#define OneNET 77
#endif

#ifndef TS_Uart
#define TS_Uart 0
#endif
#ifndef NB_Uart
#define NB_Uart 0
#endif

#ifndef WEIGOU
#define WEIGOU IWDT_Clr()
#endif

#ifndef NB_OFF
#define NB_OFF
#endif

#ifndef NB_ON
#define NB_ON
#endif

/* 沾粘函数名定义 */
#define _DIC_COAP(x) x##coap
#define DIC_COAP(x) _DIC_COAP(x)
#define _COAP_SEND(x) x##coapsend
#define COAP_SEND(x) _COAP_SEND(x)
#define _DIC_ONET(x) x##onet
#define DIC_ONET(x) _DIC_ONET(x)
#define _ONET_SEND(x) x##onetsend
#define ONET_SEND(x) _ONET_SEND(x)
#define _DIC_TCUDP(x) x##tcpudp
#define DIC_TCUDP(x) _DIC_TCUDP(x)
#define _DIC_TCP(x) x##tcp
#define DIC_TCP(x) _DIC_TCP(x)
#define _DIC_UDP(x) x##udp
#define DIC_UDP(x) _DIC_UDP(x)
#define _UPD_SEND(x) x##udpsend
#define UDP_SEND(x) _UPD_SEND(x)
#define _TCD_SEND(x) x##tcpsend
#define TCP_SEND(x) _TCD_SEND(x)
#define _DIC_INT(x) x##int
#define DIC_INT(x) _DIC_INT(x)
#define _RXANALY(x) x##rxanaly
#define RXANALY(x) _RXANALY(x)
#define _FAILDO(x) x##faildo
#define FAILDO(x) _FAILDO(x)
#define _GETCMD(x) x##getcmd
#define GETCMD(x) _GETCMD(x)
#define _IOTDATTYPE(x) x##_dat_type
#define IOTDATTYPE(x) _IOTDATTYPE(x)
#define _IOTDAT(x) x##dat
#define IOTDAT(x) _IOTDAT(x)

    typedef struct
    {
        uint8_t LinkMode;     /* 连接方式 */
        uint16_t linkmaxtime; /* 连接最大时长 */
    } iot_foo_type;

    typedef struct
    {
        uint8_t iotStart;  /* 开始连接的标志 */
        uint8_t rstcount;  /* 复位次数 */
        uint8_t sendcount; /* 发送心跳次数 */
        char *rxbuf;       /* 串口接收缓存 */
        uint16_t rxlen;    /* 从串口接收到的数据长度 */
        uint16_t timeout;  /* 用于超时时间计算 */
        char *execute;     /* 当前要执行的命令 */
    } iot_core_type;

    typedef struct
    {
        char *Ip;
        char *Port;
        char *Apn;
    } iot_net_type;

    typedef struct
    {
        char *loc;
        uint16_t len;
    } iot_buf_type;

    typedef struct
    {
        iot_foo_type foo;
        iot_net_type net;
        iot_core_type core;
    } iot_type;

    /**************************函数指针定义**************************/
    typedef uint8_t (*handler)(iot_type *arg);
    typedef void (*sendfun)(uint8_t *buf, uint16_t len);
    typedef iot_buf_type (*rxanalyfun)(iot_type *iot);
    typedef uint8_t (*faildofun)(char *name);

    typedef struct
    {
        char *name;      /* 当前要干的活 */
        char *atcommand; /* 要发送的AT命令  如果不是现成的串需要填NULL */
        handler sendop;  /* 如果atcommand=NULL 调用该发送函数 */
        char *sucword;   /* 接收成功的关键字  如果是需要解析的关键字需要填NULL */
        handler rxop;    /* 如果sucword=NULL 调用该函数 */
        uint8_t timeout; /* 超时时间 */
        uint8_t recount; /* 重发次数 */
        char *sucname;   /* 成功之后做什么 */
        char *failname;  /* 失败之后做什么 */
        char *exname;    /* 超过重发的次数之后做什么 */
    } cmd_type;          /* 字典类型定义 */

    typedef struct
    {
        cmd_type *cmd;    /* 获取字典 */
        uint8_t len;      /* 字典长度 */
        sendfun SendFun;  /* 发送函数指针 */
        rxanalyfun RxFun; /* 接收处理函数指针 */
        faildofun Faildo; /* 返回值函数指针 */
    } getcmdinfo_type;

    typedef getcmdinfo_type (*getcmd)(uint8_t linkmode);

    /****************************函数声明****************************/
    extern iot_type *getiot(void);                  /* 获取iot结构体指针 */
    extern uint8_t iotRst(iot_type *iot);           /* 给字典用的 */
    extern uint8_t iotEnd(iot_type *iot);           /* 给字典用的 */
    extern uint8_t iotRead(iot_type *iot);          /* 给字典用的 */
    extern uint8_t iotSend(iot_type *iot);          /* 给字典用的 */
    extern void iotprintf(const char *format, ...); /* 使用printf发送NB命令 */

    extern void iotTim(void);                                       /* 必须放在秒定时器里 */
    extern uint8_t iotState(void);                                  /* 可以判断是否在连接状态 */
    extern void iotRecDat(uint8_t *buf, uint16_t len);              /* 必须放在串口接收数据的地方 */
    extern void iotTxData(uint8_t *buf, uint16_t len);              /* 模块数据发送 */
    extern uint16_t iotRxData(iot_type *iot, const char *word);     /* 模块数据解析 固定用法: iotRxData(getiot(), "关键字") */
    extern uint8_t iot_init(iot_foo_type *FOO);                     /* 模块初始化 */
    extern uint8_t iot_start(iot_foo_type *FOO, iot_net_type *NET); /* 模块开始连接 */

    /**************************弱函数声明****************************/
    uint8_t iotTxDeal(void);
    uint8_t iotRxDeal(void);

    /***********************函数固化用函数指针***********************/
    typedef uint8_t (*iotinit)(iot_foo_type *FOO);
    typedef uint8_t (*iotstart)(iot_foo_type *FOO, iot_net_type *NET);
    typedef uint16_t (*iotrxdat)(iot_type *iot, const char *word);

#define _iot getiot()
#define IOT _iot
#define IOTRXLEN IOT->core.rxlen
#define IOTRXBUF IOT->core.rxbuf
#define _iotRxData(word) iotRxData(IOT, word)

#ifdef __cplusplus
}
#endif

#endif
