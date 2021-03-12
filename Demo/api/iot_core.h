/*
*********************************************************************************************************
*
*	ģ������ : �°�IOT��������
*	�ļ����� : iot_core.h
*	��    �� : V1.0.0
*	˵    �� : ����������Լ�����մճ����
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
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
#define MAX_CMD_NAME_LENGTH 10 /* �ֵ�����������ַ����� */

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

/* մճ���������� */
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
        uint8_t LinkMode;     /* ���ӷ�ʽ */
        uint16_t linkmaxtime; /* �������ʱ�� */
    } iot_foo_type;

    typedef struct
    {
        uint8_t iotStart;  /* ��ʼ���ӵı�־ */
        uint8_t rstcount;  /* ��λ���� */
        uint8_t sendcount; /* ������������ */
        char *rxbuf;       /* ���ڽ��ջ��� */
        uint16_t rxlen;    /* �Ӵ��ڽ��յ������ݳ��� */
        uint16_t timeout;  /* ���ڳ�ʱʱ����� */
        char *execute;     /* ��ǰҪִ�е����� */
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

    /**************************����ָ�붨��**************************/
    typedef uint8_t (*handler)(iot_type *arg);
    typedef void (*sendfun)(uint8_t *buf, uint16_t len);
    typedef iot_buf_type (*rxanalyfun)(iot_type *iot);
    typedef uint8_t (*faildofun)(char *name);

    typedef struct
    {
        char *name;      /* ��ǰҪ�ɵĻ� */
        char *atcommand; /* Ҫ���͵�AT����  ��������ֳɵĴ���Ҫ��NULL */
        handler sendop;  /* ���atcommand=NULL ���ø÷��ͺ��� */
        char *sucword;   /* ���ճɹ��Ĺؼ���  �������Ҫ�����Ĺؼ�����Ҫ��NULL */
        handler rxop;    /* ���sucword=NULL ���øú��� */
        uint8_t timeout; /* ��ʱʱ�� */
        uint8_t recount; /* �ط����� */
        char *sucname;   /* �ɹ�֮����ʲô */
        char *failname;  /* ʧ��֮����ʲô */
        char *exname;    /* �����ط��Ĵ���֮����ʲô */
    } cmd_type;          /* �ֵ����Ͷ��� */

    typedef struct
    {
        cmd_type *cmd;    /* ��ȡ�ֵ� */
        uint8_t len;      /* �ֵ䳤�� */
        sendfun SendFun;  /* ���ͺ���ָ�� */
        rxanalyfun RxFun; /* ���մ�����ָ�� */
        faildofun Faildo; /* ����ֵ����ָ�� */
    } getcmdinfo_type;

    typedef getcmdinfo_type (*getcmd)(uint8_t linkmode);

    /****************************��������****************************/
    extern iot_type *getiot(void);                  /* ��ȡiot�ṹ��ָ�� */
    extern uint8_t iotRst(iot_type *iot);           /* ���ֵ��õ� */
    extern uint8_t iotEnd(iot_type *iot);           /* ���ֵ��õ� */
    extern uint8_t iotRead(iot_type *iot);          /* ���ֵ��õ� */
    extern uint8_t iotSend(iot_type *iot);          /* ���ֵ��õ� */
    extern void iotprintf(const char *format, ...); /* ʹ��printf����NB���� */

    extern void iotTim(void);                                       /* ��������붨ʱ���� */
    extern uint8_t iotState(void);                                  /* �����ж��Ƿ�������״̬ */
    extern void iotRecDat(uint8_t *buf, uint16_t len);              /* ������ڴ��ڽ������ݵĵط� */
    extern void iotTxData(uint8_t *buf, uint16_t len);              /* ģ�����ݷ��� */
    extern uint16_t iotRxData(iot_type *iot, const char *word);     /* ģ�����ݽ��� �̶��÷�: iotRxData(getiot(), "�ؼ���") */
    extern uint8_t iot_init(iot_foo_type *FOO);                     /* ģ���ʼ�� */
    extern uint8_t iot_start(iot_foo_type *FOO, iot_net_type *NET); /* ģ�鿪ʼ���� */

    /**************************����������****************************/
    uint8_t iotTxDeal(void);
    uint8_t iotRxDeal(void);

    /***********************�����̻��ú���ָ��***********************/
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
