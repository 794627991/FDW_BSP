/*
*********************************************************************************************************
*
*	ģ������ : NB�������
*	�ļ����� : iot_api.c
*	��    �� : V1.0.0
*	˵    �� : һЩ���������°�iot����������ʹ���˲���������Ϊ����ԭ�Ȱ汾��������õ������޸�
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V0.0.1  2021-1-5    ����    �ɰ汾������
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#ifndef __API_IOT_H__
#define __API_IOT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if API_UseNB > 0

#include "bsp_def.h"

#define WEIGOU IWDT_Clr()

#define NEW_Send_mode

#define USE_Band_All 0 //ȫƵ
#define USE_Band_5 1   //����
#define USE_Band_8 2   //�ƶ���ͨ
#define USE_Band_3 3   //��ͨ

#define OpenFun 1
#define CloseFun 0

#define NB_UART0 0
#define NB_UART1 1
#define NB_UART2 2
#define NB_UART3 3
#define NB_UART4 4
#define NB_UART5 5

#define NB_Connecting 0xAA

#define Delay_Ms(xMs) TicksDelayMs(xMs);
#define extradd 0

#define vTxFlagTime 3
#define vReFlagTime 5
#define CoapNum 11
#define InitNum 22
#define TCPNum 33
#define UDPNum 55
#define OneNET 77

#define nbbuflen 60

#ifndef Use_BC35_BC95_BC28
#define Use_BC35_BC95_BC28 1
#endif

#if Use_BC35_BC95_BC28 > 0
#define NBStep_NRB 0
#define NBStep_AT 1
#define NBStep_CFUN1 2
#define NBStep_NCDP 3
#define NBStep_NNMI 4
#define NBStep_CMEE 5
#define NBStep_CFUNx 6
#define NBStep_CGSN 7
#define NBStep_NCCID 8
#define NBStep_CGDCONT 9
#define NBStep_CPSMS 10
#define NBStep_NPSMR 11
#define NBStep_CGATT 12
#define NBStep_CGATTx 13
#define NBStep_NUESTATS 14
#define NBStep_CGPADDR 15
#define NBStep_NSOCR 16
#define NBStep_CSCON 17
#define NBStep_CSQ 18
#define NBStep_CFUN0 19
#define NBStep_NCSEARFCN 20
#define NBStep_CEREG 21
#define NBStep_NSOCO 22
#define NBStep_QREGSWT 23
#define NBStep_Clk 24
#define NBStep_OneNetCfg 25
#define NBStep_OneNetObj 26
#define NBStep_OneNetOpen 27
#define NBStep_OneNetRsp 28
#endif
#if Use_BC260 > 0
#define NBStep_NRB 0
#define NBStep_AT 1
#define NBStep_CFUNx 2
#define NBStep_CFUN0 3
#define NBStep_CFUN1 4
#define NBStep_QSCLK 5
#define NBStep_CGSN 6
#define NBStep_NCCID 7
#define NBStep_CIMI 8
#define NBStep_CGATTx 9
#define NBStep_NUESTATS 10
#define NBStep_CSQ 11
#define NBStep_Clk 12
#define NBStep_YIPIN 13 //��Ƶ��

#define NBStep_UDP_TCP_1 20
#define NBStep_UDP_TCP_2 21
#define NBStep_UDP_TCP_3 22
#define NBStep_UDP_TCP_4 23
#define NBStep_UDP_TCP_5 24

#define NBStep_Coap_1 30
#define NBStep_Coap_2 31
#define NBStep_Coap_3 32
#define NBStep_Coap_4 33
#define NBStep_Coap_5 34

#define NBStep_OneNet_1 40
#define NBStep_OneNet_2 41
#define NBStep_OneNet_3 42
#define NBStep_OneNet_4 43
#define NBStep_OneNet_5 44

#endif
#define NBStep_Wait 0x55
#define NBStep_Jude 0xAA
#define NBStep_Off 0xfe

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
        unsigned short int year;
        WEEK weekday;
    } DATE;

    typedef struct
    {
        DATE g_cur_date; //NBУʱ����
        TIME g_cur_time;
    } Clk_Type;

    typedef union
    {
        uint32_t NBStat;
        struct
        {
            uint32_t Link_Enable : 1; //nb�������ӱ�־
            uint32_t NBOnline : 1;
            uint32_t NBok : 1;       //(����AT)ͨѶ�Ƿ�����
            uint32_t CGATT : 1;      //NB��������
            uint32_t RssiOk : 1;     //�ź�ǿ������
            uint32_t CFUNOk : 1;     //��������
            uint32_t TxFlag : 1;     //NB�������ݵı�־���˱�־������3���ڲ�����cscon��������յ������������
            uint32_t RxFlag : 1;     //��Ҫ�������ݵı�־��UDPģʽר��
            uint32_t ReFlag : 1;     //UDPģʽר��,��ReReadFlag=0,RxFlag=1ʱ��1��
            uint32_t ReReadFlag : 1; //UDPģʽר��,��ReFlag=1,TxFlag����0���2����1,�����ڷ���һ��NB��ȡ����
            uint32_t NeedYIPIN : 1;  //��Ҫ��Ƶ����ı�־
            uint32_t UpXinTiao : 1;  //�����������ݰ�
            uint32_t EndPack : 1;    //�յ�������,��������NB����
            uint32_t DataRxSuc : 1;  //���յ�����
            uint32_t fotaupflag : 1; //Fota�����ı�־
            uint32_t ReadDREAM : 1;
            uint32_t ReadNetCoe : 1;
            uint32_t ERRORAgainUpFlag : 1;
            uint32_t OneNetOline : 1;
            uint32_t rdy : 1;
            uint32_t oberve : 1;
        };
    } NB_Stat_Type;

    /*NB�������Ӳ���ʧ��״̬*/
    typedef union
    {
        uint8_t NBStat;
        struct
        {
            uint8_t psmenter : 1;
            uint8_t psmquit : 1;
        };
    } NB_Stat2_Type;

    typedef struct
    {
        uint8_t ProTime1;
        uint8_t ProTime2;
        uint8_t ReCount; //���������Ĵ������3��
        uint8_t ERRORAgainUp;
        uint8_t ERRORAgainUpTimeCount;
        uint16_t LinkMaxTime; //��NB�ϵ翪ʼ����ʱ��
        uint16_t LinkTime;    //�ɹ�ע��������ʱ��
        uint16_t TxDatalen;
        uint16_t RxDatalen;
        uint16_t PsmCount;
        uint8_t NBCurrentStep;
        uint8_t NBNextStep;
        uint8_t NBTimer;
        uint8_t NBAtCount;
    } NB_Count_Type;

    typedef struct
    {
        /*����Ĳ�����Ҫ�ֶ�����*/
        char ServerIP[15];
        char PORT[5];
        uint8_t LinkMode;
        char *NBAPN;
#if UseDMN
        *char NBDMN;
#endif
    } NB_IP_Type;

    typedef struct
    {
        uint8_t Simword[10]; //NCCID
        uint8_t CGSNNum[15];
        uint16_t rsrp;
        uint32_t cellid;
        uint16_t EARFCN;
        uint16_t PCI;
        uint8_t ECL;
        uint16_t SNR;
        uint8_t RSSI;
        uint8_t DGRAM;
        uint8_t Band;
#if Use_BC35_BC95_BC28 > 0
        uint32_t OneNetMsgId;
#endif
#if Use_BC260 > 0
        uint8_t oberve[6];
        uint8_t msgid1[20];
        uint8_t msgid2[20];
#endif
    } NB_Boty_Type;

    typedef struct
    {
        uint8_t *rxbuff;
        uint8_t *txbuff;
        uint8_t TSUart;
        uint8_t SendUart;
        uint16_t NBBaud;
        uint8_t NBBAND;
        uint8_t TSMode;
        NB_IP_Type NB_IP; //ÿ���ϱ�����Ҫ���¸�ֵ
    } NB_cfg_Type;

    typedef struct
    {
        NB_cfg_Type NB_Cfg;
        NB_Boty_Type NB_Boty;   //��Ƭ����ʼ��ʱ��ֵ
        NB_Count_Type NB_Count; //ÿ�ν���������0
        NB_Stat_Type NB_Stat;   //ÿ�ν���������0
        NB_Stat2_Type NB_SaveStat;
#if NB_CLK > 0
        Clk_Type NB_Time;
#endif
    } NB_Type;
    extern NB_Type NB;

#ifndef UseNBBaud
#define UseNBBaud 9600 //ͨѶ�õĲ�����
#endif
#ifndef UseNBBand
#define UseNBBand USE_Band_All //ʹ�õ�Ƶ��
#endif
#ifndef UseTSMode
#define UseTSMode OpenFun //�Ƿ�ʹ�õ���ģʽ
#endif
#ifndef vLinkTime
#define vLinkTime 60 //����ʱ��(����������ʼ����)  ��λΪ��
#endif
#ifndef vLinkMaxTime
#define vLinkMaxTime 360 //�������ʱ��(�ӿ�ʼ����NB�ͼ���)  ��λΪ��
#endif
#ifndef vFotaLinkMaxTime
#define vFotaLinkMaxTime 900 //Fota�������ʱ��  ��λΪ��
#endif
#ifndef ReCgatt
#define ReCgatt 40 //�������ӵĴ�����2��һ�� ��2����  ��λΪ��
#endif
#ifndef NB_CLK
#define NB_CLK 1 //NB�����Զ�Уʱ����,���rtcͬʱʹ��  0��������  ����������
#endif

    /*����Ĳ�������ָ��*/
    // #define TS_Uart NB_UART1	   //����ģʽ���������
    // #define NB_Uart NB_UART0	   //����NBͨѶ�Ĵ���
    // #define NBRXlen U0Txlen
    // #define NBRxBuf U0Buf.RxBuf
    // #define NBTxBuf U0Buf.TxBuf
    // #define CoapProfile 1		 //�������1�ǵ��Լ�дprofile
    // #define MastYP				 //һ����Ƶ
    // #define NBUseTwoStopBit		 //NBʹ����λֹͣλ
    // #define OneNetobjID			 //"3200"
    // #define OneNetresID			 //"5750"

    /**************************����ָ�붨��**************************/
    typedef int (*gy_nbtya)(NB_Type *NB);
    typedef void (*gy_nbtyb)(NB_Type *NB);
    typedef uint16_t (*gy_nbrxdatatype)(NB_Type *NB, uint8_t *buf, const char *word);
    typedef void (*input)(void);
    /****************************��������****************************/
    char *strnstr(const char *src, const char *substr, uint8_t len, uint8_t limt_len);
    void SendAtCommand(NB_Type *NB, char *patcm);
    char judgeok(NB_Type *NB, uint8_t vtime);
    void TimeCpm(void);
    void NB_Set(void);
    void NB_Init(void);
    void NB_UartRx(uint8_t *buf, uint16_t len);
    void NB_UartTx(uint8_t *buf, uint16_t len);
    void NBTXdata(uint16_t len);
    void NB_End(void);
    void NB_Clock(void);
    int NB_Link(void);
    int NB_Connect(void);
    int NB_ConPro(void);
    char NB_RevData(void);
    char NB_SendData(void);
    char NB_ReadLinkStat(void);
    char NB_ReadOnlineStat(void);
    void NB_OffOnline(void);
    void NB_SetLinkStat(bool setval);
    char NB_ReadEndPackStat(void);
    void NB_SetEndPackStat(void);
    uint16_t NB_RxData(uint8_t *buf, const char *word);

    int GY_NB_Link(NB_Type *NB);
    int GY_NB_Connect(NB_Type *NB);
    int GY_NB_ConPro(NB_Type *NB);
    void GY_NB_Init(NB_Type *NB);
    void GY_NB_UartRx(NB_Type *NB);
    void GY_NB_Clock(NB_Type *NB);
    void GY_NB_End(NB_Type *NB);
    void GY_NB_UartTx(NB_Type *NB);
    uint16_t GY_NB_RxData(NB_Type *NB, uint8_t *buf, const char *word);
    int NB_printf(NB_Type *NB, const char *format, ...);
    /**************************����������****************************/
    int NB_Start(NB_IP_Type configcoe, input Hook);
    void NBRxDeal(void);
    void NBTxDeal(void);

#endif

#ifdef __cplusplus
}
#endif

#endif
