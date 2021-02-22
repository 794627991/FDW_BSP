/*
*********************************************************************************************************
*
*	ģ������ : NB��������
*	�ļ����� : iot_api.c
*	��    �� : V0.0.1
*	˵    �� : NB���������ں�BC28,BC260��������
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V0.0.1  2021-1-5    ����    �ɰ汾������
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "iot_api.h"

#if API_UseNB > 0
NB_Type NB; /* �ṹ�嶨�� */

__weak void NBRxDeal(void)
{
}

__weak void NBTxDeal(void)
{
}

#if ThisBoot == 0
/*----------------------------------------------------------------------
//NBʹ������
//�ڵ���NB_Init();ǰ��Ҫ����NB_Set����һ�²���
//��ϸ��NB��ʼ�����̼�void NB_Init_Exp(void)
//
//void TimeCpm(void)�����Ĺ�����������������Զ�Уʱ
//int NB_Start(NB_IP_Type configcoe)��NB���������ĺ���
//
//��ϸ��NB�������̼�void NB_Connect_Exp(void)
//NB������ɺ���Ҫ�Լ���д���ͺͽ��պ���:void NBTxDeal(void)��void NBRxDeal(void)
//
//NB��������ǰ��Ҫ���� void NB_SetEndPackStat(void);������ȷ��NB���Ҫ��������
//ֻ�е��øú���,int NB_Start(NB_IP_Type configcoe)�Ż᷵��1,��ʾ���ӳɹ�
//����һ�ɷ���0
-----------------------------------------------------------------------*/
void NB_Set(void)
{
    NB.NB_Cfg.rxbuff = NBRxBuf;
    NB.NB_Cfg.txbuff = NBTxBuf;
    NB.NB_Cfg.NBBaud = UseNBBaud;
    NB.NB_Cfg.NBBAND = UseNBBand;
    NB.NB_Cfg.TSMode = UseTSMode;
    NB.NB_Cfg.TSUart = TS_Uart;
    NB.NB_Cfg.SendUart = NB_Uart;
    //	NB.NB_Cfg.NB_IP.LinkMode=UDPNum;          //��UDP����
    //	NB.NB_Cfg.NB_IP.NBAPN=APN;                //APN
    //	memcpy(NB.NB_Cfg.NB_IP.ServerIP,IP1,15);  //IP
    //	memcpy(NB.NB_Cfg.NB_IP.PORT,PORT1,5);     //PORT
}

/* --------------------------------------------------------------
����0�����ӳɹ�
����1������ʧ��
----------------------------------------------------------------*/
__weak int NB_Start(NB_IP_Type configcoe, input Hook)
{
    int res = 0xff;

    NB.NB_Cfg.NB_IP = configcoe;

    NB_Set();
    res = NB_Link();
    if (res == 0xAA)
    {
        do
        {
#if Use_uCOS == 0
            ExecuteEvent; //��event.h���ṩ�˺궨��,Ŀ������NB���ӹ�����,Ҳ�ܴ����¼�
#endif
            res = NB_ConPro();
            WEIGOU;
        } while (res == 0XAA);
    }

    if (Hook != NULL)
    {
        Hook();
    }

    if (res == 0)
    {
        //���ӳɹ�
        TimeCpm(); //NB�Զ�Уʱ
        while (NB_ReadLinkStat() && NB_ReadOnlineStat())
        {
#if Use_uCOS == 0
            ExecuteEvent; //��event.h���ṩ�˺궨��,Ŀ������NB���ӹ�����,Ҳ�ܴ����¼�
#endif
            WEIGOU;
            if (NB_RevData())
            {
                //���ճ����Լ�д
                NBRxDeal();
            }
            if (NB_SendData())
            {
                //���ͳ����Լ�д
                NBTxDeal();
            }
        }
        if (NB_ReadEndPackStat())
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        //���ڸ���ԭ��������ʧ��
        return 1;
    }
}
#else
void NB_Set(void)
{
}
#endif

void TimeCpm(void)
{
#if NB_CLK > 0 && API_UseRTC > 0
    RTC_TimeDateTypeDef TempTime;

    ddd.year = NB.NB_Time.g_cur_date.year;
    ddd.month = NB.NB_Time.g_cur_date.month;
    ddd.day = NB.NB_Time.g_cur_date.day;

    if ((Check_date(CLK.g_cur_date.year, CLK.g_cur_date.month, CLK.g_cur_date.day)) &&
            (CLK.g_cur_time.hour < 24) && (CLK.g_cur_time.minute < 60) && (CLK.g_cur_time.second < 60))
    {
        // struct tm stm;
        // time_t T1, T2;
        // stm.tm_year = NB.NB_Time.g_cur_date.year - 1900;
        // stm.tm_mon = NB.NB_Time.g_cur_date.month - 1;
        // stm.tm_mday = NB.NB_Time.g_cur_date.day;
        // stm.tm_hour = NB.NB_Time.g_cur_time.hour;
        // stm.tm_min = NB.NB_Time.g_cur_time.minute;
        // stm.tm_sec = NB.NB_Time.g_cur_time.second;
        // T1 = mktime(&stm);

        // stm.tm_year = Hex_YearL + 100;
        // stm.tm_mon = Hex_Mon - 1;
        // stm.tm_mday = Hex_Day;
        // stm.tm_hour = Hex_Hour;
        // stm.tm_min = Hex_Min;
        // stm.tm_sec = Hex_Sec;
        // T2 = mktime(&stm);
        // if ((T1 > (T2 + 60)) || (T2 > (T1 + 60)))
        {
            TempTime.Year = HEXtoBCD(NB.NB_Time.g_cur_date.year - 2000);
            TempTime.Month = HEXtoBCD(NB.NB_Time.g_cur_date.month);
            TempTime.Date = HEXtoBCD(NB.NB_Time.g_cur_date.day);
            TempTime.Hour = HEXtoBCD(NB.NB_Time.g_cur_time.hour);
            TempTime.Minute = HEXtoBCD(NB.NB_Time.g_cur_time.minute);
            TempTime.Second = HEXtoBCD(NB.NB_Time.g_cur_time.second);
            API_SetTIME(&TempTime); //����ʱ��
        }
    }
#endif
}

/*NB��ʼ������*/
void NB_Init(void)
{
    NB_Set();
    GY_NB_Init(&NB);
}

/*��NB��ʼ����ʱ����*/
int NB_Link(void)
{
    return GY_NB_Link(&NB);
}

/*���ڽ����е���*/
void NB_UartRx(uint8_t *buf, uint16_t len)
{
    NB.NB_Cfg.rxbuff = buf;
    NB.NB_Count.RxDatalen = len;
    GY_NB_UartRx(&NB);
}

/*��������*/
void NB_UartTx(uint8_t *buf, uint16_t len)
{
    NB.NB_Cfg.txbuff = buf;
    NB.NB_Count.TxDatalen = len;
    GY_NB_UartTx(&NB);
}

/*��������(����)*/
int NB_Connect(void)
{
    return GY_NB_Connect(&NB);
}

/*��������(���,3���ظ�)*/
int NB_ConPro(void)
{
    return GY_NB_ConPro(&NB);
}

/*������������*/
void NB_End(void)
{
    GY_NB_End(&NB);
}

/*��������*/
void NBTXdata(uint16_t len)
{
    NB.NB_Count.TxDatalen = len;
    GY_NB_UartTx(&NB);
}

/****************************************
//	�붨ʱ���е���
****************************************/
void NB_Clock(void)
{
    GY_NB_Clock(&NB);
}

/****************************************
//	�������ʱ�䣬����ʱ�䵽���������Զ���ֹ
****************************************/
void NB_ClearLinkTime(void)
{
    NB.NB_Count.LinkTime = 0;
}

/*****************************
//	��ȡNB�״̬
//	1�����
//	0���ǻ
*****************************/
char NB_ReadLinkStat(void)
{
    if (NB.NB_Stat.Link_Enable)
        return 1;
    else
        return 0;
}

/*****************************
//	��ȡ����״̬
//	1������
//	0��������
*****************************/
char NB_ReadOnlineStat(void)
{
    if (NB.NB_Stat.NBOnline)
        return 1;
    else
        return 0;
}

/*****************************
//	����NB�״̬
//	1��ʹ��
//	0��ֹͣ
*****************************/
void NB_SetLinkStat(bool setval)
{
    if (setval == 1)
        NB.NB_Stat.Link_Enable = 1;
    else
        NB.NB_Stat.Link_Enable = 0;
}

/*****************************
//	�ر�NB����
*****************************/
void NB_OffOnline(void)
{
    NB.NB_Stat.NBOnline = 0;
}

/*****************************
//	���յ�����
//	1����
//	0����
*****************************/
char NB_RevData(void)
{
    if (NB.NB_Stat.DataRxSuc == 1)
    {
        NB.NB_Stat.DataRxSuc = 0;
        return 1;
    }
    else
        return 0;
}

/*****************************
//	�Ƿ���Ҫ��������
//	1����
//	0����
*****************************/
char NB_SendData(void)
{
    if (NB.NB_Stat.UpXinTiao == 1)
    {
        NB.NB_Stat.UpXinTiao = 0;
        //ÿ���ϱ���������ʱ��
        NB_ClearLinkTime();
        return 1;
    }
    else
        return 0;
}

/*************************************
//	�յ���������ζ�ſ�����������NB������
*************************************/
void NB_SetEndPackStat(void)
{
    NB.NB_Stat.NBOnline = 0;
    NB.NB_Stat.EndPack = 1;
}

/*****************************
//	�Ƿ��յ�������
//	1����
//	0����
*****************************/
char NB_ReadEndPackStat(void)
{
    if (NB.NB_Stat.EndPack)
        return 1;
    else
        return 0;
}

/*****************************
//	����0���ӳɹ�
//	����1���źŵ���û���ųɹ�
//	����2���ź�
//	����3ģ��������
//	����4û�п�
//	����5δ֪
//	����55���ӱ���ֹ
//	����AA�������ӹ�����
*****************************/
int GY_NB_ConPro(NB_Type *NB)
{
    int re = 0;
    for (;;)
    {
        re = GY_NB_Connect(NB);
        if ((re == 0) || (re == NB_Connecting))
            break;
        else
        {
            if (re == 55)
                return re;
            else
            {
                NB->NB_Count.ReCount++;
                if (NB->NB_Count.ReCount >= 3)
                    return re;
            }
        }
    }
    return re;
}

/*************************************
//	������ջ��棬���ݽ������棬�ؼ���
*************************************/
uint16_t NB_RxData(uint8_t *buf, const char *word)
{
    return GY_NB_RxData(&NB, buf, word);
}

void NB_UART_BSP(uint8_t cc, uint8_t *p, uint16_t len)
{
    switch (cc)
    {
    case 0:
        API_Uart_Send(UART0, p, len);
        break;
    case 1:
        API_Uart_Send(UART1, p, len);
        break;
    case 2:
        API_Uart_Send(UART2, p, len);
        break;
    case 3:
        API_Uart_Send(UART3, p, len);
        break;
    case 4:
        API_Uart_Send(UART4, p, len);
        break;
    case 5:
        API_Uart_Send(UART5, p, len);
        break;
    default:
        break;
    }
}

void TiaoShiSend(NB_Type *NB, uint8_t *p, uint16_t len)
{
    if (NB->NB_Cfg.TSMode == 1)
        NB_UART_BSP(NB->NB_Cfg.TSUart, p, len);
}

void NB_Send(NB_Type *NB, uint8_t *buff, uint16_t len)
{
    TiaoShiSend(NB, buff, len);
    if (NB->NB_Stat.fotaupflag == 0)
    {
        NB_UART_BSP(NB->NB_Cfg.SendUart, buff, len);
    }
}

void SendAtCommand(NB_Type *NB, char *patcm)
{
    uint32_t ATlen = strlen(patcm);
    NB_Send(NB, (uint8_t *)patcm, ATlen);
}

char judgeok(NB_Type *NB, uint8_t vtime)
{
    uint32_t wiattime = vtime * 100 + 1;
    NB->NB_Stat.NBok = 0;
    do
    {
        if (NB->NB_Stat.NBok == 1)
            return 1;
        else
            Delay_Ms(10);
        WEIGOU;
    } while (wiattime--);

    return 0;
}

#if Use_BC260 > 0
char judgeRDY(NB_Type *NB, uint8_t vtime)
{
    uint32_t wiattime = vtime * 100 + 1;
    do
    {
        if (NB->NB_Stat.rdy == 1)
            return 1;
        else
            Delay_Ms(10);
    } while (wiattime--);

    return 0;
}

void SendCoapIPort(NB_Type *NB)
{
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%15s%s%5s%s%s%s", "AT+QLACONFIG=0,\"", NB->NB_Cfg.NB_IP.ServerIP, "\",",
              NB->NB_Cfg.NB_IP.PORT, ",\"", NB->NB_Boty.CGSNNum, "\",300,3\r\n");
#else
    uint8_t NBSendBuff[80];
    uint8_t ServeripNum, PortNum;
    ServeripNum = mystrlen(NB->NB_Cfg.NB_IP.ServerIP, 15);
    PortNum = mystrlen(NB->NB_Cfg.NB_IP.PORT, 5);
    memcpy(NBSendBuff, "AT+QLACONFIG=0,\"", 16);
    memcpy(NBSendBuff + 16, NB->NB_Cfg.NB_IP.ServerIP, ServeripNum);
    memcpy(NBSendBuff + 16 + ServeripNum, "\",", 2);
    memcpy(NBSendBuff + 18 + ServeripNum, NB->NB_Cfg.NB_IP.PORT, PortNum);
    memcpy(NBSendBuff + 18 + ServeripNum + PortNum, ",\"", 2);
    memcpy(NBSendBuff + 20 + ServeripNum + PortNum, NB->NB_Boty.CGSNNum, 15);
    memcpy(NBSendBuff + 35 + ServeripNum + PortNum, "\",300,3", 7);
    memcpy(NBSendBuff + 42 + ServeripNum + PortNum, "\r\n", 2);
    NB_Send(NB, NBSendBuff, 44 + ServeripNum + PortNum);
#endif
}

/*set=0  udp  set=1 tcp*/
void SendUdpTcpIpPort(NB_Type *NB, bool set)
{
    /*AT+QIOPEN=0,0,"UDP",60.21.136.130,3380*/
#ifdef NEW_Send_mode
    if (set == 0)
    {
        NB_printf(NB, "%s%15s%s%5s%s", "AT+QIOPEN=0,0,\"UDP\",", NB->NB_Cfg.NB_IP.ServerIP, ",", NB->NB_Cfg.NB_IP.PORT, "\r\n");
    }
    else
    {
        NB_printf(NB, "%s%15s%s%5s%s", "AT+QIOPEN=0,0,\"TCP\",", NB->NB_Cfg.NB_IP.ServerIP, ",", NB->NB_Cfg.NB_IP.PORT, "\r\n");
    }
#else
    uint8_t NBSendBuff[50], ServeripNum, PortNum;
    ServeripNum = mystrlen(NB->NB_Cfg.NB_IP.ServerIP, 15);
    PortNum = mystrlen(NB->NB_Cfg.NB_IP.PORT, 5);
    if (set == 0)
    {
        memcpy(NBSendBuff, "AT+QIOPEN=0,0,\"UDP\",", 20);
    }
    else
    {
        memcpy(NBSendBuff, "AT+QIOPEN=0,0,\"TCP\",", 20);
    }
    memcpy(NBSendBuff + 20, NB->NB_Cfg.NB_IP.ServerIP, ServeripNum);
    memcpy(NBSendBuff + 20 + ServeripNum, ",", 1);
    memcpy(NBSendBuff + 21 + ServeripNum, NB->NB_Cfg.NB_IP.PORT, PortNum);
    memcpy(NBSendBuff + 21 + ServeripNum + PortNum, "\r\n", 2);
    NB_Send(NB, NBSendBuff, 23 + ServeripNum + PortNum);
#endif
}

void oberveSend(NB_Type *NB)
{
    /*AT+QLAOBSRSP=60722,1,19,0,0,2,1,"FF",0*/
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%6s%s", "AT+QLAOBSRSP=", NB->NB_Boty.oberve, ",1,19,0,0,2,1,\"FF\",0\r\n");
#else
    uint8_t NBSendBuff[50];
    int len = mystrlen((char *)NB->NB_Boty.oberve, 6);
    memcpy(NBSendBuff, "AT+QLAOBSRSP=", 13);
    memcpy(NBSendBuff + 13, NB->NB_Boty.oberve, len);
    memcpy(NBSendBuff + 13 + len, ",1,19,0,0,2,1,\"FF\",0\r\n", 22);
    NB_Send(NB, NBSendBuff, 35 + len);
#endif
}

void NB_Coap_Send(NB_Type *NB)
{
    /*AT+QLANOTIFY=19,0,0,2,75,"DATA",0,0*/
#ifdef NEW_Send_mode
#if CoapProfile > 0
    uint8_t NBSendBuff[10];
    memset(NBSendBuff, 0, 10);
    /*Coap�漰profile�ļ�ͷ��Ҫ����Э�������޸�*/
    NBSendBuff[0] = '0';
    NBSendBuff[1] = '0'; //profile�̶�д��

    ww = HIGHWORD(NB->NB_Count.TxDatalen);
    HexToAscll((char *)&NBSendBuff[2], (uint8_t *)&ww, 1);
    ww = LOWWORD(NB->NB_Count.TxDatalen);
    HexToAscll((char *)&NBSendBuff[4], (uint8_t *)&ww, 1);

    NB_printf(NB, "%s%d%s%s%l%a%s", "AT+QLANOTIFY=19,0,0,2,", NB->NB_Count.TxDatalen + 3, ",\"", NBSendBuff,
              NB->NB_Count.TxDatalen, NB->NB_Cfg.txbuff, "\",0,0\r\n");
#else
    NB_printf(NB, "%s%d%s%l%a%s", "AT+QLANOTIFY=19,0,0,2,", NB->NB_Count.TxDatalen, ",\"",
              NB->NB_Count.TxDatalen, NB->NB_Cfg.txbuff, "\",0,0\r\n");
#endif
#else
    uint32_t bbb;
    uint32_t profilelen = 0;
    uint8_t NBSendBuff[nbbuflen];
#if CoapProfile > 0
    uint8_t ww;
    profilelen = NB->NB_Count.TxDatalen + 3;
#else
    profilelen = NB->NB_Count.TxDatalen;
#endif

    memcpy(NBSendBuff, "AT+QLANOTIFY=19,0,0,2,", 22);

    if (profilelen < 10)
    {
        NBSendBuff[22] = profilelen + 0x30;
        NBSendBuff[23] = ',';
        NBSendBuff[24] = '\"';
        bbb = 25;
    }
    else if (profilelen < 100)
    {
        NBSendBuff[22] = (profilelen / 10) + 0x30;
        NBSendBuff[23] = (profilelen % 10) + 0x30;
        NBSendBuff[24] = ',';
        NBSendBuff[25] = '\"';
        bbb = 26;
    }
    else
    {
        NBSendBuff[22] = (profilelen / 100) + 0x30;
        NBSendBuff[23] = ((profilelen % 100) / 10) + 0x30;
        NBSendBuff[24] = (profilelen % 10) + 0x30;
        NBSendBuff[25] = ',';
        NBSendBuff[26] = '\"';
        bbb = 27;
    }
#if CoapProfile > 0
    /*Coap�漰profile�ļ�ͷ��Ҫ����Э�������޸�*/
    NBSendBuff[bbb] = '0';
    NBSendBuff[bbb + 1] = '0'; //profile�̶�д��

    ww = HIGHWORD(NB->NB_Count.TxDatalen);
    HexToAscll((char *)&NBSendBuff[bbb + 2], (uint8_t *)&ww, 1);
    ww = LOWWORD(NB->NB_Count.TxDatalen);
    HexToAscll((char *)&NBSendBuff[bbb + 4], (uint8_t *)&ww, 1);

    bbb += 6;
#endif

    NB_Send(NB, NBSendBuff, bbb);
#endif
}

void NB_Coap_SendTail(NB_Type *NB)
{
    /*AT+QLANOTIFY=19,0,0,2,75,"DATA",0,0     ��Ӧ����",0,0*/
    uint8_t NBSendBuff[10] = {0};
    memcpy(NBSendBuff, "\",0,0", 5);
    NB_Send(NB, NBSendBuff, 5);
}

void NB_UDP_Send(NB_Type *NB)
{
    //AT+QISEND=0,3,AA1133
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%d%s%l%a%s", "AT+QISEND=0,", NB->NB_Count.TxDatalen, ",", NB->NB_Count.TxDatalen, NB->NB_Cfg.txbuff, "\r\n");
#else
    uint8_t NBSendBuff[50];
    uint32_t bbb = 12;
    memcpy(NBSendBuff, "AT+QISEND=0,", 12);
    if (NB->NB_Count.TxDatalen < 10)
    {
        NBSendBuff[bbb] = NB->NB_Count.TxDatalen + 0x30;
        NBSendBuff[bbb + 1] = ',';
        bbb += 2;
    }
    else if (NB->NB_Count.TxDatalen < 100)
    {
        NBSendBuff[bbb] = (NB->NB_Count.TxDatalen / 10) + 0x30;
        NBSendBuff[bbb + 1] = (NB->NB_Count.TxDatalen % 10) + 0x30;
        NBSendBuff[bbb + 2] = ',';
        bbb += 3;
    }
    else
    {
        NBSendBuff[bbb] = (NB->NB_Count.TxDatalen / 100) + 0x30;
        NBSendBuff[bbb + 1] = ((NB->NB_Count.TxDatalen % 100) / 10) + 0x30;
        NBSendBuff[bbb + 2] = (NB->NB_Count.TxDatalen % 10) + 0x30;
        NBSendBuff[bbb + 3] = ',';
        bbb += 4;
    }
    NB_Send(NB, NBSendBuff, bbb);
#endif
}

void NB_TCP_Send(NB_Type *NB)
{
    NB_UDP_Send(NB);
}

void NB_OneNet_Send(NB_Type *NB)
{
//AT+MIPLNOTIFY=0,9523,3200,0,5750,1,5,��hello��,0,0 //���������ϱ�
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%20s%s%d%s%l%a%s", "AT+MIPLNOTIFY=0,", NB->NB_Boty.msgid1, ",3200,0,5750,1,",
              NB->NB_Count.TxDatalen * 2, ",\"", NB->NB_Count.TxDatalen, NB->NB_Cfg.txbuff, "\",0,0\r\n");
#else
    uint8_t NBSendBuff[60];
    int len = mystrlen((char *)NB->NB_Boty.msgid1, 20), bbb = 0;
    memcpy(NBSendBuff, "AT+MIPLNOTIFY=0,", 16);
    memcpy(NBSendBuff + 16, NB->NB_Boty.msgid1, len);
    memcpy(NBSendBuff + 16 + len, ",3200,0,5750,1,", 15); //,3200,0,5750,1,
    bbb = 31 + len;
    len = NB->NB_Count.TxDatalen * 2;
    if (len < 10)
    {
        NBSendBuff[bbb] = len + 0x30;
        NBSendBuff[bbb + 1] = ',';
        NBSendBuff[bbb + 2] = '\"';
        bbb += 3;
    }
    else if (len < 100)
    {
        NBSendBuff[bbb] = (len / 10) + 0x30;
        NBSendBuff[bbb + 1] = (len % 10) + 0x30;
        NBSendBuff[bbb + 2] = ',';
        NBSendBuff[bbb + 3] = '\"';
        bbb += 4;
    }
    else
    {
        NBSendBuff[bbb] = (len / 100) + 0x30;
        NBSendBuff[bbb + 1] = ((len % 100) / 10) + 0x30;
        NBSendBuff[bbb + 2] = (len % 10) + 0x30;
        NBSendBuff[bbb + 3] = ',';
        NBSendBuff[bbb + 4] = '\"';
        bbb += 5;
    }
    NB_Send(NB, NBSendBuff, bbb);
#endif
}

void NB_OneNet_SendTail(NB_Type *NB)
{
    uint8_t NBSendBuff[nbbuflen];
    memset(NBSendBuff, 0, nbbuflen);
    memcpy(NBSendBuff, "\",0,0", 5);
    NB_Send(NB, NBSendBuff, 5);
}

void OneNetojb1Rec(NB_Type *NB)
{
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%20s%s", "AT+MIPLOBSERVERSP=0,", NB->NB_Boty.msgid1, ",1\r\n");
#else
    uint8_t NBSendBuff[50];
    int len = mystrlen((char *)NB->NB_Boty.msgid1, 20);
    memcpy(NBSendBuff, "AT+MIPLOBSERVERSP=0,", 20);
    memcpy(NBSendBuff + 20, NB->NB_Boty.msgid1, len);
    memcpy(NBSendBuff + 20 + len, ",1\r\n", 4);
    NB_Send(NB, NBSendBuff, 24 + len);
#endif
}

void OneNetojb2Rec(NB_Type *NB)
{
    //AT+MIPLDISCOVERRSP=0,31605,1,4,"5750"
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%20s%s", "AT+MIPLDISCOVERRSP=0,", NB->NB_Boty.msgid2, ",1,4,\"5750\"\r\n");
#else
    uint8_t NBSendBuff[60];
    int len = mystrlen((char *)NB->NB_Boty.msgid2, 20);
    memcpy(NBSendBuff, "AT+MIPLDISCOVERRSP=0,", 21);
    memcpy(NBSendBuff + 21, NB->NB_Boty.msgid2, len);
    memcpy(NBSendBuff + 21 + len, ",1,4,\"5750\"\r\n", 13);
    NB_Send(NB, NBSendBuff, 34 + len);
#endif
}

void OneNetWriteRec(NB_Type *NB)
{
    //+MIPLWRITE: 0,11535,3200,0,5750,2,8,3131323233333434,0,0
    //AT+MIPLWRITERSP=0,16952,2
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%20s%s", "AT+MIPLWRITERSP=0,", NB->NB_Boty.msgid2, ",2\r\n");
#else
    uint8_t NBSendBuff[60];
    int len = mystrlen((char *)NB->NB_Boty.msgid2, 20);
    memcpy(NBSendBuff, "AT+MIPLWRITERSP=0,", 18);
    memcpy(NBSendBuff + 18, NB->NB_Boty.msgid2, len);
    memcpy(NBSendBuff + 18 + len, ",2\r\n", 4);
    NB_Send(NB, NBSendBuff, 22 + len);
#endif
}
#endif

#if Use_BC35_BC95_BC28 > 0
void SendAPNCommand(NB_Type *NB, char *patcm)
{
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%32s%s", "AT+CGDCONT=1,\"IP\",\"", patcm, "\"\r\n");
#else
    uint8_t len = mystrlen(patcm, 32);
    uint8_t NBSendBuff[nbbuflen];
    memcpy(NBSendBuff, "AT+CGDCONT=1,\"IP\",\"", 19);
    memcpy(NBSendBuff + 19, patcm, len);
    memcpy(NBSendBuff + 19 + len, "\"\r\n", 3);
    NB_Send(NB, NBSendBuff, 22 + len);
#endif
}

void SendOneNetCommand(NB_Type *NB)
{
    //AT+MIPLDISCOVERRSP=0,53576,1,4,"5750"
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%c%s%d%s", "AT+MIPLDISCOVERRSP=", NB->NB_Boty.DGRAM, ",", NB->NB_Boty.OneNetMsgId, ",1,4,\"5750\"\r\n");
#else
    uint8_t i = 0, k = 0;
    uint32_t j = NB->NB_Boty.OneNetMsgId;
    uint8_t NBSendBuff[nbbuflen] = {0};
    uint8_t TTPBUF[11];

    memcpy(NBSendBuff, "AT+MIPLDISCOVERRSP=", 19);
    NBSendBuff[19] = NB->NB_Boty.DGRAM;
    NBSendBuff[20] = ',';
    for (i = 0; i < 10; i++)
    {
        k = j % 10;
        j /= 10;
        TTPBUF[i] = k + 0x30;
        if (j == 0)
            break;
    }
    k = i + 1;
    for (i = 0; i < k; i++)
    {
        NBSendBuff[21 + i] = TTPBUF[k - i - 1];
    }
    memcpy(&NBSendBuff[21 + k], ",1,4,\"5750\"\r\n", 13); //OneNetresID
    NB_Send(NB, NBSendBuff, 34 + k);
#endif
}

void SendCoapIPort(NB_Type *NB)
{
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%15s%c%5s%s", "AT+NCDP=", NB->NB_Cfg.NB_IP.ServerIP, ',', NB->NB_Cfg.NB_IP.PORT, "\r\n");
#else
    uint8_t NBSendBuff[nbbuflen], ServeripNum, PortNum;
    ServeripNum = mystrlen(NB->NB_Cfg.NB_IP.ServerIP, 15);
    PortNum = mystrlen(NB->NB_Cfg.NB_IP.PORT, 5);
    memcpy(NBSendBuff, "AT+NCDP=", 8);
    memcpy(NBSendBuff + 8, NB->NB_Cfg.NB_IP.ServerIP, ServeripNum);
    memcpy(NBSendBuff + 8 + ServeripNum, ",", 1);
    memcpy(NBSendBuff + 9 + ServeripNum, NB->NB_Cfg.NB_IP.PORT, PortNum);
    memcpy(NBSendBuff + 9 + ServeripNum + PortNum, "\r\n", 2);
    NB_Send(NB, NBSendBuff, 11 + ServeripNum + PortNum);
#endif
}

void SendTcpIpPort(NB_Type *NB)
{
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%c%c%15s%c%5s%s", "AT+NSOCO=", NB->NB_Boty.DGRAM, ',', NB->NB_Cfg.NB_IP.ServerIP, ',', NB->NB_Cfg.NB_IP.PORT, "\r\n");
#else
    uint8_t NBSendBuff[nbbuflen];
#if UseDMN
    uint8_t PortNum, Dmnlen;
    Dmnlen = mystrlen(NB->NB_Boty.DMN, 32);
    PortNum = mystrlen(NB->NB_Cfg.NB_IP.PORT, 5);
    memcpy(NBSendBuff, "AT+NSOCO=", 9);
    NBSendBuff[10] = NB->NB_Boty.DGRAM;
    NBSendBuff[11] = '\"';
    memcpy(&NBSendBuff[12], NB->NB_Boty.DMN, Dmnlen);
    NBSendBuff[12 + Dmnlen] = '\"';
    memcpy(&NBSendBuff[13 + Dmnlen], NB->NB_Cfg.NB_IP.PORT, PortNum);
    memcpy(NBSendBuff + 13 + Dmnlen + PortNum, "\r\n", 2);
    if (NB->NB_Stat.fotaupflag == 0)
        NB_Send(NBSendBuff, 15 + Dmnlen + PortNum);
#else
    uint8_t ServeripNum, PortNum;
    memcpy(NBSendBuff, "AT+NSOCO=", 9);
    NBSendBuff[10] = NB->NB_Boty.DGRAM;
    NBSendBuff[11] = ',';
    ServeripNum = mystrlen(NB->NB_Cfg.NB_IP.ServerIP, 15);
    PortNum = mystrlen(NB->NB_Cfg.NB_IP.PORT, 5);
    memcpy(NBSendBuff + 12, NB->NB_Cfg.NB_IP.ServerIP, ServeripNum);
    memcpy(NBSendBuff + 12 + ServeripNum, ",", 1);
    memcpy(NBSendBuff + 13 + ServeripNum, NB->NB_Cfg.NB_IP.PORT, PortNum);
    memcpy(NBSendBuff + 13 + ServeripNum + PortNum, "\r\n", 2);
#endif
    NB_Send(NB, NBSendBuff, 15 + ServeripNum + PortNum);
#endif
}

void NB_TCP_Send(NB_Type *NB)
{
    /*AT+NSOSD=1,1,04*/
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%c%c%d%c%l%a%s", "AT+NSOSD=", NB->NB_Boty.DGRAM, ',', NB->NB_Count.TxDatalen,
              ',', NB->NB_Count.TxDatalen, NB->NB_Cfg.txbuff, "\r\n");
#else
    uint32_t LEN = 0;
    uint8_t NBSendBuff[nbbuflen];
    memset(NBSendBuff, 0, 35);
    memcpy(NBSendBuff, "AT+NSOSD=", 9);
    NBSendBuff[9] = NB->NB_Boty.DGRAM;
    NBSendBuff[10] = ',';
    if (NB->NB_Count.TxDatalen < 10)
    {
        NBSendBuff[11] = NB->NB_Count.TxDatalen + 0x30;
        NBSendBuff[12] = ',';
        LEN = 13;
    }
    else if (NB->NB_Count.TxDatalen < 100)
    {
        NBSendBuff[11] = (NB->NB_Count.TxDatalen / 10) + 0x30;
        NBSendBuff[12] = (NB->NB_Count.TxDatalen % 10) + 0x30;
        NBSendBuff[13] = ',';
        LEN = 14;
    }
    else
    {
        NBSendBuff[11] = (NB->NB_Count.TxDatalen / 100) + 0x30;
        NBSendBuff[12] = ((NB->NB_Count.TxDatalen % 100) / 10) + 0x30;
        NBSendBuff[13] = (NB->NB_Count.TxDatalen % 10) + 0x30;
        NBSendBuff[14] = ',';
        LEN = 15;
    }
    NB_Send(NB, NBSendBuff, LEN);
#endif
}

void NB_UDP_Send(NB_Type *NB)
{
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%c%c%15s%c%5s%c%d%c%l%a%s", "AT+NSOST=", NB->NB_Boty.DGRAM, ',', NB->NB_Cfg.NB_IP.ServerIP, ',',
              NB->NB_Cfg.NB_IP.PORT, ',', NB->NB_Count.TxDatalen, ',', NB->NB_Count.TxDatalen, NB->NB_Cfg.txbuff, "\r\n");
#else
    uint32_t bbb = 0;
    uint8_t NBSendBuff[nbbuflen], ServeripNum, PortNum;
    memset(NBSendBuff, 0, 35);
    memcpy(NBSendBuff, "AT+NSOST=", 9);
    NBSendBuff[9] = NB->NB_Boty.DGRAM;
    NBSendBuff[10] = ',';

    ServeripNum = mystrlen(NB->NB_Cfg.NB_IP.ServerIP, 15);
    PortNum = mystrlen(NB->NB_Cfg.NB_IP.PORT, 5);
    memcpy(NBSendBuff + 11, NB->NB_Cfg.NB_IP.ServerIP, ServeripNum);
    NBSendBuff[11 + ServeripNum] = ',';
    memcpy(NBSendBuff + 12 + ServeripNum, NB->NB_Cfg.NB_IP.PORT, PortNum);
    bbb = 12 + ServeripNum + PortNum;
    NBSendBuff[bbb] = ',';
    bbb += 1;

    if (NB->NB_Count.TxDatalen < 10)
    {
        NBSendBuff[bbb] = NB->NB_Count.TxDatalen + 0x30;
        NBSendBuff[bbb + 1] = ',';
        bbb += 2;
    }
    else if (NB->NB_Count.TxDatalen < 100)
    {
        NBSendBuff[bbb] = (NB->NB_Count.TxDatalen / 10) + 0x30;
        NBSendBuff[bbb + 1] = (NB->NB_Count.TxDatalen % 10) + 0x30;
        NBSendBuff[bbb + 2] = ',';
        bbb += 3;
    }
    else
    {
        NBSendBuff[bbb] = (NB->NB_Count.TxDatalen / 100) + 0x30;
        NBSendBuff[bbb + 1] = ((NB->NB_Count.TxDatalen % 100) / 10) + 0x30;
        NBSendBuff[bbb + 2] = (NB->NB_Count.TxDatalen % 10) + 0x30;
        NBSendBuff[bbb + 3] = ',';
        bbb += 4;
    }
    NB_Send(NB, NBSendBuff, bbb);
#endif
}

void NB_Coap_Send(NB_Type *NB)
{
    /*AT+NMGS=75,000048AA11C50...*/
#ifdef NEW_Send_mode
#if CoapProfile > 0
    uint8_t NBSendBuff[10];
    memset(NBSendBuff, 0, 10);
    /*Coap�漰profile�ļ�ͷ��Ҫ����Э�������޸�*/
    NBSendBuff[0] = '0';
    NBSendBuff[1] = '0'; //profile�̶�д��

    ww = HIGHWORD(NB->NB_Count.TxDatalen);
    HexToAscll((char *)&NBSendBuff[2], (uint8_t *)&ww, 1);
    ww = LOWWORD(NB->NB_Count.TxDatalen);
    HexToAscll((char *)&NBSendBuff[4], (uint8_t *)&ww, 1);

    NB_printf("%s%d%s%s%l%a%s", "AT+NMGS=", NB->NB_Count.TxDatalen + 3, ",", NBSendBuff,
              NB->NB_Count.TxDatalen, NB->NB_Cfg.txbuff, "\r\n");
#else
    NB_printf(NB, "%s%d%s%l%a%s", "AT+NMGS=", NB->NB_Count.TxDatalen, ",",
              NB->NB_Count.TxDatalen, NB->NB_Cfg.txbuff, "\r\n");
#endif
#else

    uint32_t bbb;
    uint32_t profilelen = 0;
    uint8_t NBSendBuff[nbbuflen] = {0};
#if CoapProfile > 0
    uint8_t ww;
    profilelen = NB->NB_Count.TxDatalen + 3;
#else
    profilelen = NB->NB_Count.TxDatalen;
#endif
    memcpy(NBSendBuff, "AT+NMGS=", 8);
    if (profilelen < 10)
    {
        NBSendBuff[8] = profilelen + 0x30;
        NBSendBuff[9] = ',';
        bbb = 10;
    }
    else if (profilelen < 100)
    {
        NBSendBuff[8] = (profilelen / 10) + 0x30;
        NBSendBuff[9] = (profilelen % 10) + 0x30;
        NBSendBuff[10] = ',';
        bbb = 11;
    }
    else
    {
        NBSendBuff[8] = (profilelen / 100) + 0x30;
        NBSendBuff[9] = ((profilelen % 100) / 10) + 0x30;
        NBSendBuff[10] = (profilelen % 10) + 0x30;
        NBSendBuff[11] = ',';
        bbb = 12;
    }
#if CoapProfile > 0
    /*Coap�漰profile�ļ�ͷ��Ҫ����Э�������޸�*/
    NBSendBuff[bbb] = '0';
    NBSendBuff[bbb + 1] = '0'; //profile�̶�д��

    ww = HIGHWORD(NB->NB_Count.TxDatalen);
    HexToAscll((char *)&NBSendBuff[bbb + 2], (uint8_t *)&ww, 1);
    ww = LOWWORD(NB->NB_Count.TxDatalen);
    HexToAscll((char *)&NBSendBuff[bbb + 4], (uint8_t *)&ww, 1);

    bbb += 6;
#endif

    NB_Send(NB, NBSendBuff, bbb);
#endif
}

void NB_OneNet_Send(NB_Type *NB)
{
    //AT+MIPLNOTIFY=0,0,3200,0,5750,1,2,"68",0,0
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%c%s%d%s%l%a%s", "AT+MIPLNOTIFY=", NB->NB_Boty.DGRAM, ",0,3200,0,5750,1,",
              NB->NB_Count.TxDatalen * 2, ",\"", NB->NB_Count.TxDatalen, NB->NB_Cfg.txbuff, "\",0,0");
#else
    uint8_t NBSendBuff[nbbuflen], bbb;
    uint32_t OneNetSendLen;
    memset(NBSendBuff, 0, nbbuflen);
    memcpy(NBSendBuff, "AT+MIPLNOTIFY=", 14);
    NBSendBuff[14] = NB->NB_Boty.DGRAM;

    memcpy(&NBSendBuff[15], ",0,3200,0,5750,1,", 17); //OneNetresID,OneNetobjID

    OneNetSendLen = NB->NB_Count.TxDatalen * 2; //����*2

    if (OneNetSendLen < 10)
    {
        NBSendBuff[32] = OneNetSendLen + 0x30;
        NBSendBuff[33] = ',';
        NBSendBuff[34] = '\"';
        bbb = 35;
    }
    else if (OneNetSendLen < 100)
    {
        NBSendBuff[32] = (OneNetSendLen / 10) + 0x30;
        NBSendBuff[33] = (OneNetSendLen % 10) + 0x30;
        NBSendBuff[34] = ',';
        NBSendBuff[35] = '\"';
        bbb = 36;
    }
    else
    {
        NBSendBuff[32] = (OneNetSendLen / 100) + 0x30;
        NBSendBuff[33] = ((OneNetSendLen % 100) / 10) + 0x30;
        NBSendBuff[34] = (OneNetSendLen % 10) + 0x30;
        NBSendBuff[35] = ',';
        NBSendBuff[36] = '\"';
        bbb = 37;
    }
    NB_Send(NB, NBSendBuff, bbb);
#endif
}

void NB_OneNet_SendTail(NB_Type *NB)
{
    uint8_t NBSendBuff[nbbuflen];
    memset(NBSendBuff, 0, nbbuflen);
    memcpy(NBSendBuff, "\",0,0", 5);
    NB_Send(NB, NBSendBuff, 5);
}

#endif

void NB_UDP_Read(NB_Type *NB)
{
#if Use_BC35_BC95_BC28 > 0
#ifdef NEW_Send_mode
    NB_printf(NB, "%s%c%s", "AT+NSORF=", NB->NB_Boty.DGRAM, ",512\r\n");
#else
    uint8_t NBSendBuff[nbbuflen];
    NB->NB_Stat.RxFlag = 1;
    if (NB->NB_Stat.ReReadFlag == 0)
    {
        NB->NB_Stat.ReFlag = 1;
    }
    NB->NB_Count.ProTime1 = 0;
    NB->NB_Count.ProTime2 = 0;
    memcpy(NBSendBuff, "AT+NSORF=", 9);
    NBSendBuff[9] = NB->NB_Boty.DGRAM;
    memcpy(NBSendBuff + 10, ",512\r\n", 6);
    NB_Send(NB, NBSendBuff, 16);
#endif
#endif
}

__weak void GY_NB_UartTx(NB_Type *NB)
{
    if (NB->NB_Cfg.NB_IP.LinkMode == CoapNum)
    {
        NB_Coap_Send(NB);
    }
    else if (NB->NB_Cfg.NB_IP.LinkMode == TCPNum)
    {
        NB_TCP_Send(NB);
    }
    else if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
    {
        NB_OneNet_Send(NB);
    }
    else
    {
        NB_UDP_Send(NB);
    }
#ifndef NEW_Send_mode
    uint8_t NBSendBuff[10];
    for (uint32_t i = 0; i < NB->NB_Count.TxDatalen; i++)
    {
        HexToAscll((char *)NBSendBuff, &NB->NB_Cfg.txbuff[i], 1);
        NB_Send(NB, NBSendBuff, 2);
    }

#if Use_BC35_BC95_BC28 > 0
    if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
    {
        NB_OneNet_SendTail(NB);
    }
#endif

#if Use_BC260 > 0
    if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
    {
        NB_OneNet_SendTail(NB);
    }
    if (NB->NB_Cfg.NB_IP.LinkMode == CoapNum)
    {
        NB_Coap_SendTail(NB);
    }
#endif

    NBSendBuff[0] = 0x0D;
    NBSendBuff[1] = 0x0A;
    NB_Send(NB, NBSendBuff, 2);
#endif
    NB->NB_Stat.TxFlag = 1;
    NB->NB_Stat.ReReadFlag = 0;
}

__weak void GY_NB_UartRx(NB_Type *NB)
{
    if (NB->NB_Cfg.TSMode == 1)
    {
        TiaoShiSend(NB, NB->NB_Cfg.rxbuff, NB->NB_Count.RxDatalen);
    }

#if Use_BC35_BC95_BC28 > 0

    if (NB->NB_Stat.RxFlag == 1)
    {
        NB->NB_Stat.RxFlag = 0;
        NB->NB_Stat.DataRxSuc = 1;
    }

    if (NB->NB_Stat.ReadDREAM)
    {
        char i = 0, j = 0;
        if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
        {
            j = 15;
        }
        else
        {
            j = 5;
        }
        for (i = 0; i < j; i++)
        {
            if ((NB->NB_Cfg.rxbuff[i] >= '0') && (NB->NB_Cfg.rxbuff[i] <= '9'))
            {
                NB->NB_Boty.DGRAM = NB->NB_Cfg.rxbuff[i];
                NB->NB_Stat.NBok = 1;
                break;
            }
        }
        NB->NB_Stat.ReadDREAM = 0;
    }

    if (NB->NB_Stat.ReadNetCoe)
    {
        char *Location, i;
        NB->NB_Boty.rsrp = 0;
        NB->NB_Boty.cellid = 0;
        NB->NB_Boty.EARFCN = 0;
        NB->NB_Boty.PCI = 0;
        NB->NB_Boty.ECL = 0;
        NB->NB_Boty.SNR = 0;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "Signal power");
        if (Location[13] == '-')
        {
            for (i = 0; i < 4; i++)
            {
                if (Location[i + 14] == 0x0D)
                    break;

                NB->NB_Boty.rsrp = NB->NB_Boty.rsrp * 10 - (Location[i + 14] - 0x30);
            }
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                if (Location[i + 13] == 0x0D)
                    break;
                NB->NB_Boty.rsrp = NB->NB_Boty.rsrp * 10 + (Location[i + 13] - 0x30);
            }
        }

        Location = strstr((char *)NB->NB_Cfg.rxbuff, "ECL");
        for (i = 0; i < 2; i++)
        {
            if (Location[i + 4] == 0x0D)
                break;
            NB->NB_Boty.ECL = NB->NB_Boty.ECL * 10 + (Location[i + 4] - 0x30);
        }

        Location = strstr((char *)NB->NB_Cfg.rxbuff, "SNR");
        if (Location[4] == '-')
        {
            for (i = 0; i < 4; i++)
            {
                if (Location[i + 5] == 0x0D)
                    break;
                NB->NB_Boty.SNR = NB->NB_Boty.SNR * 10 - (Location[i + 5] - 0x30);
            }
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                if (Location[i + 4] == 0x0D)
                    break;
                NB->NB_Boty.SNR = NB->NB_Boty.SNR * 10 + (Location[i + 4] - 0x30);
            }
        }

        Location = strstr((char *)NB->NB_Cfg.rxbuff, "PCI");
        for (i = 0; i < 6; i++)
        {
            if (Location[i + 4] == 0x0D)
                break;
            NB->NB_Boty.PCI = NB->NB_Boty.PCI * 10 + (Location[i + 4] - 0x30);
        }

        Location = strstr((char *)NB->NB_Cfg.rxbuff, "EARFCN");
        for (i = 0; i < 6; i++)
        {
            if (Location[i + 7] == 0x0D)
                break;
            NB->NB_Boty.EARFCN = NB->NB_Boty.EARFCN * 10 + (Location[i + 7] - 0x30);
        }

        Location = strstr((char *)NB->NB_Cfg.rxbuff, "Cell ID");
        for (i = 0; i < 10; i++)
        {
            if (Location[i + 8] == 0x0D)
                break;
            NB->NB_Boty.cellid = NB->NB_Boty.cellid * 10 + (Location[i + 8] - 0x30);
        }

        Location = strstr((char *)NB->NB_Cfg.rxbuff, "CURRENT BAND"); //CURRENT BAND:5
        for (i = 0; i < 2; i++)
        {
            if (Location[i + 14] == 0x0D)
                break;
            NB->NB_Boty.Band = NB->NB_Boty.Band * 10 + (Location[i + 14] - 0x30);
        }

        NB->NB_Stat.ReadNetCoe = 0;
        NB->NB_Stat.NBok = 1;
    }

    if (strstr((char *)NB->NB_Cfg.rxbuff, "+NSONMI:"))
    {
        memset(NB->NB_Cfg.rxbuff, 0, NB->NB_Count.RxDatalen);
        NB_UDP_Read(NB);
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+NNMI:"))
    {
        NB->NB_Stat.DataRxSuc = 1;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+MIPLWRITE:"))
    {
        NB->NB_Stat.DataRxSuc = 1;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+CSCON:0"))
    {
        if ((NB->NB_Stat.TxFlag == 0) && (NB->NB_Stat.Link_Enable == 1))
        {
            NB->NB_Stat.UpXinTiao = 1;
        }
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+CGSN:")) //+CGSN:869976030074034
    {
        char *Location;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "+CGSN:");
        memcpy(NB->NB_Boty.CGSNNum, &Location[6], 15);
        NB->NB_Stat.NBok = 1;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+NCCID:")) //+NCCID:89860317482032905738
    {
        char *Location;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "+NCCID:");
        CharToBety((uint8_t *)&Location[7], NB->NB_Boty.Simword, 10);
        NB->NB_Stat.NBok = 1;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+NPSMR:1"))
    {
        NB->NB_Stat.Link_Enable = 0;
        NB->NB_SaveStat.psmenter = 1;
        NB->NB_SaveStat.psmquit = 0;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+NPSMR:0"))
    {
        NB->NB_SaveStat.psmenter = 0;
        NB->NB_SaveStat.psmquit = 1;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+CFUN:1"))
    {
        NB->NB_Count.NBCurrentStep = NBStep_CGATT;
        NB->NB_Stat.CFUNOk = 1;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+CSQ:")) //+CSQ:25,99
    {
        char *Location;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "+CSQ:");
        if (Location[6] == ',')
            NB->NB_Boty.RSSI = Location[5] - 0x30;
        else
            NB->NB_Boty.RSSI = ((Location[5] - 0x30) << 4) + (Location[6] - 0x30);
        if ((NB->NB_Boty.RSSI > 0) && (NB->NB_Boty.RSSI != 0x99))
            NB->NB_Stat.RssiOk = 1;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+CEREG:")) //+CEREG:1
    {
        char *Location;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "+CEREG:");
        /*1:��ע��,2:����ע��,5:����ע�᣻0:δע��,3:ע�ᱻ�ܾ�,4:δ֪*/
        if ((Location[7] == '1') || (Location[7] == '5'))
        {
            NB->NB_Count.NBCurrentStep = NBStep_NUESTATS;
            NB->NB_Stat.CGATT = 1;
            NB->NB_Stat.NBOnline = 1;
        }
        else if ((Location[7] == '0') || (Location[7] == '3'))
        {
            NB->NB_Count.NBCurrentStep = NBStep_Off;
        }
        else
        {
        }
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+CGATT:1"))
    {
        NB->NB_Count.NBCurrentStep = NBStep_NUESTATS;
        NB->NB_Stat.CGATT = 1;
        NB->NB_Stat.NBOnline = 1;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+MIPLDISCOVER:"))
    {
        //+MIPLDISCOVER: 0,53576,3200
        char *Location, jklh;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "+MIPLDISCOVER:");
        NB->NB_Boty.OneNetMsgId = 0;
        if (Location[16] == ',')
        {
            for (jklh = 0; jklh < 10; jklh++)
            {
                if (Location[jklh + 17] == ',')
                    break;
                NB->NB_Boty.OneNetMsgId = NB->NB_Boty.OneNetMsgId * 10 + (Location[jklh + 17] - 0x30);
            }
        }

        NB->NB_Count.NBCurrentStep = NBStep_OneNetRsp;
        NB->NB_Stat.OneNetOline = 1;
        NB->NB_Stat.NBok = 1;
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+QLWEVTIND:3")) //fota������ɱ�־��ע����ƽ̨��־
    {
        if (NB->NB_Stat.fotaupflag == 1)
        {
            NB->NB_Stat.fotaupflag = 0;
            SendAtCommand(NB, "ATI\r\n");
            judgeok(NB, 1);
            SendAtCommand(NB, "AT+CSCON=1\r\n");
            judgeok(NB, 1);
            NB->NB_Stat.UpXinTiao = 1;
        }
        else
        {
            if ((NB->NB_Stat.TxFlag == 0) && (NB->NB_Stat.Link_Enable == 1))
            {
                NB->NB_Stat.UpXinTiao = 1;
            }
        }
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+QLWEVTIND:5")) //fota������ʼ��־
    {
        NB->NB_Stat.fotaupflag = 1;
    }
#if NB_CLK > 0
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+CCLK:"))
    {
        char *Location;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "+CCLK:");

        NB->NB_Time.g_cur_time.second = (Location[21] - 0x30) * 10 + (Location[22] - 0x30);
        NB->NB_Time.g_cur_time.minute = (Location[18] - 0x30) * 10 + (Location[19] - 0x30);
        NB->NB_Time.g_cur_time.hour = (Location[15] - 0x30) * 10 + (Location[16] - 0x30) + (((Location[24] - 0x30) * 10 + (Location[25] - 0x30)) / 4);
        NB->NB_Time.g_cur_date.day = (Location[12] - 0x30) * 10 + (Location[13] - 0x30);
        NB->NB_Time.g_cur_date.month = (MONTH)((Location[9] - 0x30) * 10 + (Location[10] - 0x30));
        NB->NB_Time.g_cur_date.year = (Location[6] - 0x30) * 10 + (Location[7] - 0x30) + 2000;
        if (NB->NB_Time.g_cur_time.hour >= 24)
        {
            NB->NB_Time.g_cur_time.hour = NB->NB_Time.g_cur_time.hour - 24;
            switch (NB->NB_Time.g_cur_date.month)
            {
            case Feb: //ƽ��
            {
                if (((NB->NB_Time.g_cur_date.year % 100 != 0) && (NB->NB_Time.g_cur_date.year % 4 == 0)) || (NB->NB_Time.g_cur_date.year % 400 == 0)) //����
                {
                    if (++NB->NB_Time.g_cur_date.day > 29)
                    {
                        NB->NB_Time.g_cur_date.day = 1;
                        ++NB->NB_Time.g_cur_date.month; //�¼���
                    }
                }
                else if (++NB->NB_Time.g_cur_date.day > 28)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    ++NB->NB_Time.g_cur_date.month; //�¼���
                }
            }
            break;
            case Jan:
            case Mar:
            case May:
            case Jul:
            case Aug:
            case Oct:
            case Dec: //����
            {
                if (++NB->NB_Time.g_cur_date.day > 31)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    if (++NB->NB_Time.g_cur_date.month > Dec) //�¼���
                    {
                        NB->NB_Time.g_cur_date.month = Jan;
                        NB->NB_Time.g_cur_date.year++; //�����
                    }
                }
            }
            break;
            case Apr:
            case Jun:
            case Sep:
            case Nov: //С��
            {
                if (++NB->NB_Time.g_cur_date.day > 30)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    ++NB->NB_Time.g_cur_date.month; //�¼���
                }
            }
            break;
            }
        }
        NB->NB_Stat.NBok = 1;
        NB->NB_Count.NBCurrentStep = NBStep_Jude;
    }
#endif
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "ERROR"))
    {
        if (NB->NB_Stat.CGATT == 1) //˵���Ѿ�������������
        {
            if (++NB->NB_Count.ERRORAgainUp < 3)
            {
                NB->NB_Stat.ERRORAgainUpFlag = 1;

                //DisplayErroy(NoZhuCe);//�����������ΪCoApûע��erro5
            }
            else
            {
                NB->NB_Stat.Link_Enable = 0;
            }
        }
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "OK"))
    {
        /*ֻҪ����OK˵��ģ������*/
        NB->NB_Stat.NBok = 1;
        switch (NB->NB_Count.NBNextStep)
        {
        case NBStep_AT:
        {
            if (NB->NB_Stat.NeedYIPIN == 1)
            {
                NB->NB_Stat.NeedYIPIN = 0;
                NB->NB_Count.NBCurrentStep = NBStep_CFUN0;
            }
            else
            {
                NB->NB_Count.NBCurrentStep = NBStep_CFUN1;
            }
        }
        break;
        case NBStep_CFUN1:
        {
            NB->NB_Count.NBCurrentStep = NBStep_CFUNx;
        }
        break;
        case NBStep_NCDP:
        {
            NB->NB_Count.NBCurrentStep = NBStep_NNMI;
        }
        break;
        case NBStep_NNMI:
        {
            NB->NB_Count.NBCurrentStep = NBStep_CMEE;
        }
        break;
        case NBStep_CPSMS:
        {
            NB->NB_Count.NBCurrentStep = NBStep_NPSMR;
        }
        break;
        case NBStep_NPSMR:
        {
            NB->NB_Count.NBCurrentStep = NBStep_CEREG;
        }
        break;
        case NBStep_CFUN0:
        {
            NB->NB_Count.NBCurrentStep = NBStep_NCSEARFCN;
        }
        break;
        case NBStep_NCSEARFCN:
        {
            NB->NB_Count.NBCurrentStep = NBStep_CFUN1;
        }
        break;
        case NBStep_NSOCO:
        {
            NB->NB_Count.NBCurrentStep = NBStep_CSCON;
        }
        break;
        case NBStep_CSCON:
        {
            NB->NB_Count.NBCurrentStep = NBStep_CSQ;
        }
        break;
        case NBStep_OneNetCfg:
        {
            NB->NB_Count.NBCurrentStep = NBStep_NSOCR;
        }
        break;
        case NBStep_OneNetObj:
        {
            NB->NB_Count.NBCurrentStep = NBStep_OneNetOpen;
        }
        break;
        case NBStep_OneNetRsp:
        {
            NB->NB_Count.NBCurrentStep = NBStep_CSCON;
        }
        break;
        default:
            break;
        }
    }
    else
    {
    }
#endif

#if Use_BC260 > 0

    if (strstr((char *)NB->NB_Cfg.rxbuff, "QLAURC: \"write\""))
    {
        NB->NB_Stat.DataRxSuc = 1;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "QIURC: \"recv\""))
    {
        NB->NB_Stat.DataRxSuc = 1;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "MIPLWRITE: "))
    {
        NB->NB_Stat.DataRxSuc = 1;
        //+MIPLWRITE: 0,11535,3200,0,5750,2,8,3131323233333434,0,0
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "QLAURC:"))
    {
        //+QLAURC: "observe",60722,0,19,0,0
        char *Location, *Locationl;
        int len = strlen("\"observe\",");
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "\"observe\",");
        Locationl = strstr(&Location[len], ",");
        if ((Locationl != NULL) && (Locationl > (Location + len)))
        {
            memset(NB->NB_Boty.oberve, 0, 6);
            memcpy(NB->NB_Boty.oberve, &Location[len], Locationl - Location - len);
            NB->NB_Stat.oberve = 1;
            NB->NB_Count.NBCurrentStep = NBStep_Coap_5;
        }
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "QENG: "))
    {
        //+QENG: 0,2506,,176,"0BF576D1",-59,-6,31,21,5,3095,0,-17,3
        char *Location, *Locationl, choutdou = 1;
        int len = strlen("QENG: 0,"), ti = 0, tj = 0;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "QENG: 0,");
        NB->NB_Boty.rsrp = 0;
        NB->NB_Boty.cellid = 0;
        NB->NB_Boty.EARFCN = 0;
        NB->NB_Boty.PCI = 0;
        NB->NB_Boty.ECL = 0;
        NB->NB_Boty.SNR = 0;
        for (;;)
        {
            Locationl = strstr(&Location[len], ",");
            if ((Locationl != NULL) && (Locationl >= (Location + len)))
            {
                if (choutdou == 1) //С����
                {
                    for (ti = 0; ti < Locationl - Location - len; ti++)
                    {
                        NB->NB_Boty.EARFCN = (NB->NB_Boty.EARFCN * 10) + (Location[len + ti] - 0x30);
                    }
                }
                else if (choutdou == 3) //PCI
                {
                    for (ti = 0; ti < Locationl - Location - len; ti++)
                    {
                        NB->NB_Boty.PCI = (NB->NB_Boty.PCI * 10) + (Location[len + ti] - 0x30);
                    }
                }
                else if (choutdou == 4) //cellid
                {
                    for (ti = 0; ti < 8; ti++)
                    {
                        if (Location[len] == '\"')
                        {
                            NB->NB_Boty.cellid = (NB->NB_Boty.cellid << 4) + CharToHex(Location[len + ti + 1]);
                        }
                    }
                }
                else if (choutdou == 5) //rsrp
                {
                    if (Location[len] == '-') //һ���Ǹ�����
                    {
                        ti = Locationl - Location - len - 1;
                        for (tj = 0; tj < ti; tj++)
                        {
                            NB->NB_Boty.rsrp = NB->NB_Boty.rsrp * 10 - (Location[len + tj + 1] - 0x30);
                        }
                    }
                }
                else if (choutdou == 8) //snr
                {
                    if (Location[len] == '-')
                    {
                        ti = Locationl - Location - len - 1;
                        for (tj = 0; tj < ti; tj++)
                        {
                            NB->NB_Boty.SNR = NB->NB_Boty.SNR * 10 - (Location[len + tj + 1] - 0x30);
                        }
                    }
                    else
                    {
                        ti = Locationl - Location - len;
                        for (tj = 0; tj < ti; tj++)
                        {
                            NB->NB_Boty.SNR = NB->NB_Boty.SNR * 10 + (Location[len + tj] - 0x30);
                        }
                    }
                }
                else if (choutdou == 9) //band
                {
                    NB->NB_Boty.Band = Location[len] - 0x30;
                }
                else if (choutdou == 11) //ecl
                {
                    NB->NB_Boty.ECL = Location[len] - 0x30;
                }
                else if (choutdou > 11)
                {
                    break;
                }
            }
            else
            {
                break;
            }
            len = 0;
            choutdou++;
            Location = Locationl + 1;
        }
        NB->NB_Stat.NBok = 1;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "QIOPEN: 0,0"))
    {
        NB->NB_Count.NBCurrentStep = NBStep_Jude;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "MIPLCREATE: 0"))
    {
        //+MIPLCREATE: 0
        NB->NB_Count.NBCurrentStep = NBStep_OneNet_2;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "MIPLOBSERVE:"))
    {
        //+MIPLOBSERVE: 0,81895,1,3200,0,-1
        char *Location, *Locationl;
        int len = strlen("MIPLOBSERVE: 0,");
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "MIPLOBSERVE: 0,");
        Locationl = strstr(&Location[len], ",");
        if ((Locationl != NULL) && (Locationl > (Location + len)))
        {
            memset(NB->NB_Boty.msgid1, 0, 20);
            memcpy(NB->NB_Boty.msgid1, &Location[len], Locationl - Location - len);
            NB->NB_Count.NBCurrentStep = NBStep_OneNet_4;
        }
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "MIPLDISCOVER:"))
    {
        //+MIPLDISCOVER: 0,31605,3311
        char *Location, *Locationl;
        int len = strlen("MIPLDISCOVER: 0,");
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "MIPLDISCOVER: 0,");
        Locationl = strstr(&Location[len], ",");
        if ((Locationl != NULL) && (Locationl > (Location + len)))
        {
            memset(NB->NB_Boty.msgid2, 0, 20);
            memcpy(NB->NB_Boty.msgid2, &Location[len], Locationl - Location - len);
            NB->NB_Count.NBCurrentStep = NBStep_OneNet_5;
        }
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "RDY"))
    {
        NB->NB_Stat.rdy = 1;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "CPIN: READY"))
    {
        NB->NB_Stat.CFUNOk = 1;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "CFUN: 1"))
    {
        NB->NB_Stat.CFUNOk = 1;
        NB->NB_Count.NBCurrentStep = NBStep_QSCLK;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "QLAOBSRSP: 0"))
    {
        NB->NB_Count.NBCurrentStep = NBStep_Jude;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "CEREG: "))
    {
        char *Location;
        int len = strlen("CEREG: ");
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "CEREG: ");
        if ((Location[len] == '0') || (Location[len] == '3'))
        {
            NB->NB_Count.NBCurrentStep = NBStep_Off; //δע�ᣬֱ�ӹ�
        }
        if ((Location[len] == '1') || (Location[len] == '5'))
        {
            NB->NB_Stat.CGATT = 1; //��ע��
            NB->NB_Stat.NBOnline = 1;
            NB->NB_Count.NBCurrentStep = NBStep_CSQ;
        }
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "QCCID: ")) //+QCCID: 89860317482032831892
    {
        char *Location;
        int len = strlen("QCCID: "), c = 0;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "QCCID: ");
        for (c = 0; c < 20; c++)
        {
            if ((Location[c] <= 0x30) && (Location[c] >= 0x39))
            {
                break;
            }
        }
        if (c == 20)
        {
            CharToBety((uint8_t *)&Location[len], NB->NB_Boty.Simword, 10);
            NB->NB_Count.NBCurrentStep = NBStep_CIMI;
        }
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "CGSN: ")) //+CGSN: 869951040044046
    {
        char *Location;
        int len = strlen("CGSN: "), c = 0;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "CGSN: ");
        memcpy(NB->NB_Boty.CGSNNum, &Location[len], 15);
        for (c = 0; c < 15; c++)
        {
            if ((NB->NB_Boty.CGSNNum[c] <= 0x30) && (NB->NB_Boty.CGSNNum[c] >= 0x39))
            {
                break; //��ʹ��coap����ʱ�����뱣֤ģ�����ȷ
            }
        }
        if (c == 15)
        {
            NB->NB_Count.NBCurrentStep = NBStep_NCCID;
        }
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "CSQ: ")) //+CSQ: 31,0
    {
        char *Location;
        int len = strlen("CSQ: ");
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "CSQ: ");
        if (Location[len + 1] == ',')
            NB->NB_Boty.RSSI = Location[len] - 0x30;
        else
            NB->NB_Boty.RSSI = ((Location[len] - 0x30) << 4) + (Location[len + 1] - 0x30);
        if ((NB->NB_Boty.RSSI > 0) && (NB->NB_Boty.RSSI != 0x99))
            NB->NB_Stat.RssiOk = 1;
        NB->NB_Stat.NBok = 1;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "CGATT: 1")) //+CGATT: 1
    {
        NB->NB_Stat.CGATT = 1;
        NB->NB_Stat.NBOnline = 1;
        NB->NB_Count.NBCurrentStep = NBStep_CSQ;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "CSCON: 0"))
    {
        if ((NB->NB_Stat.TxFlag == 0) && (NB->NB_Stat.Link_Enable == 1))
        {
            NB->NB_Stat.UpXinTiao = 1;
        }
    }
#if NB_CLK > 0
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "CCLK: "))
    {
        //+CCLK: "2020/08/17,08:33:55+32"
        char *Location;
        int len = strlen("CCLK: ");
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "CCLK: ");
        NB->NB_Time.g_cur_time.second = (Location[24] - 0x30) * 10 + (Location[25] - 0x30);
        NB->NB_Time.g_cur_time.minute = (Location[21] - 0x30) * 10 + (Location[22] - 0x30);
        NB->NB_Time.g_cur_time.hour = (Location[18] - 0x30) * 10 + (Location[19] - 0x30) + (((Location[27] - 0x30) * 10 + (Location[28] - 0x30)) / 4);
        NB->NB_Time.g_cur_date.day = (Location[15] - 0x30) * 10 + (Location[16] - 0x30);
        NB->NB_Time.g_cur_date.month = (MONTH)((Location[12] - 0x30) * 10 + (Location[13] - 0x30));
        NB->NB_Time.g_cur_date.year = (Location[7] - 0x30) * 10 + (Location[8] - 0x30) + 2000;
        if (NB->NB_Time.g_cur_time.hour >= 24)
        {
            NB->NB_Time.g_cur_time.hour = NB->NB_Time.g_cur_time.hour - 24;
            switch (NB->NB_Time.g_cur_date.month)
            {
            case Feb: //ƽ��
            {
                if (((NB->NB_Time.g_cur_date.year % 100 != 0) && (NB->NB_Time.g_cur_date.year % 4 == 0)) || (NB->NB_Time.g_cur_date.year % 400 == 0)) //����
                {
                    if (++NB->NB_Time.g_cur_date.day > 29)
                    {
                        NB->NB_Time.g_cur_date.day = 1;
                        ++NB->NB_Time.g_cur_date.month; //�¼���
                    }
                }
                else if (++NB->NB_Time.g_cur_date.day > 28)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    ++NB->NB_Time.g_cur_date.month; //�¼���
                }
            }
            break;
            case Jan:
            case Mar:
            case May:
            case Jul:
            case Aug:
            case Oct:
            case Dec: //����
            {
                if (++NB->NB_Time.g_cur_date.day > 31)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    if (++NB->NB_Time.g_cur_date.month > Dec) //�¼���
                    {
                        NB->NB_Time.g_cur_date.month = Jan;
                        NB->NB_Time.g_cur_date.year++; //�����
                    }
                }
            }
            break;
            case Apr:
            case Jun:
            case Sep:
            case Nov: //С��
            {
                if (++NB->NB_Time.g_cur_date.day > 30)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    ++NB->NB_Time.g_cur_date.month; //�¼���
                }
            }
            break;
            }
        }
        NB->NB_Stat.NBok = 1;
        NB->NB_Count.NBCurrentStep = NBStep_NUESTATS;
    }
#endif
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "ENTER PSM")) //QNBIOTEVENT: "ENTER PSM"
    {
        NB->NB_SaveStat.psmenter = 1;
        NB->NB_Stat.NBOnline = 0;
        NB->NB_Stat.CGATT = 0;
        NB->NB_Count.NBCurrentStep = NBStep_Off;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "EXIT PSM")) //QNBIOTEVENT: "EXIT PSM"
    {
        NB->NB_SaveStat.psmquit = 1;
    }
    if (strstr((char *)NB->NB_Cfg.rxbuff, "OK"))
    {
        /*ֻҪ����OK˵��ģ������*/
        NB->NB_Stat.NBok = 1;
        switch (NB->NB_Count.NBNextStep)
        {
        case NBStep_AT:
        {
            if (NB->NB_Stat.NeedYIPIN == 1)
            {
                NB->NB_Stat.NeedYIPIN = 0;
                NB->NB_Count.NBCurrentStep = NBStep_CFUN0;
            }
            else
            {
                NB->NB_Count.NBCurrentStep = NBStep_CFUNx;
            }
        }
        break;
        case NBStep_CFUN0:
        {
            NB->NB_Count.NBCurrentStep = NBStep_YIPIN;
        }
        break;
        case NBStep_YIPIN:
        {
            NB->NB_Count.NBCurrentStep = NBStep_CFUN1;
        }
        break;
        case NBStep_CFUN1:
        {
            NB->NB_Count.NBCurrentStep = NBStep_CFUNx;
        }
        break;
        case NBStep_UDP_TCP_1:
        {
            NB->NB_Count.NBCurrentStep = NBStep_UDP_TCP_2;
        }
        break;
        case NBStep_Coap_1:
        {
            NB->NB_Count.NBCurrentStep = NBStep_Coap_2;
        }
        break;
        case NBStep_Coap_2:
        {
            NB->NB_Count.NBCurrentStep = NBStep_Coap_3;
        }
        break;
        case NBStep_Coap_3:
        {
            NB->NB_Count.NBCurrentStep = NBStep_Coap_4;
        }
        break;
        case NBStep_OneNet_2:
        {
            NB->NB_Count.NBCurrentStep = NBStep_OneNet_3;
        }
        break;
        case NBStep_OneNet_5:
        {
            NB->NB_Count.NBCurrentStep = NBStep_Jude;
        }
        break;
        default:
            break;
        }
    }
#endif

    if (NB->NB_Stat.DataRxSuc != 1)
    {
        memset(NB->NB_Cfg.rxbuff, 0, NB->NB_Count.RxDatalen);
    }
}

__weak void GY_NB_Init(NB_Type *NB)
{
    NB->NB_Stat.Link_Enable = 1;
#if Use_BC35_BC95_BC28 > 0
    SendAtCommand(NB, "AT+NRB\r\n");
    judgeok(NB, 15);
#ifdef NBUseTwoStopBit
    if (NB->NB_Cfg.NBBaud == 4800)
    {
        SendAtCommand(NB, "AT+NATSPEED=4800,30,1,0,2\r\n");
    }
    else
    {
        SendAtCommand(NB, "AT+NATSPEED=9600,30,1,0,2\r\n");
    }
#endif
    judgeok(NB, 1);
    SendAtCommand(NB, "AT\r\n");
    judgeok(NB, 1);
    SendAtCommand(NB, "ATI\r\n");
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CGMR\r\n");
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CPSMS=1,,,01000011,00000011\r\n"); //��PSM
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CFUN=0\r\n");
    judgeok(NB, 10);
    SendAtCommand(NB, "AT+NCONFIG=CELL_RESELECTION,TRUE\r\n"); //��С����ѡ
    judgeok(NB, 1);
    if (NB->NB_Cfg.NBBAND == USE_Band_All)
    {
        SendAtCommand(NB, "AT+NBAND=3,5,8\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_5) //����
    {
        SendAtCommand(NB, "AT+NBAND=5\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_8) //�ƶ���ͨ
    {
        SendAtCommand(NB, "AT+NBAND=8\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_3) //��ͨ1800
    {
        SendAtCommand(NB, "AT+NBAND=3\r\n");
    }
    judgeok(NB, 1);

    SendAtCommand(NB, "AT+CEDRXS=0,5\r\n"); //�ر�ģ��eDRX����
    judgeok(NB, 1);
#endif

#if Use_BC260 > 0
    judgeRDY(NB, 5);               //�ȴ��յ���λ��־
    SendAtCommand(NB, "ATE0\r\n"); //�رջ���
    judgeok(NB, 1);
    SendAtCommand(NB, "AT\r\n");
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CMEE=1\r\n"); //������
    judgeok(NB, 1);
    SendAtCommand(NB, "ATI\r\n");
    judgeok(NB, 2);
    SendAtCommand(NB, "AT+QCFG=\"dsevent\",0\r\n"); //�ر�������ֹ��SLEEP�ϱ�
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+QCFG=\"autoapn\",0\r\n"); //���� APN �Զ�����ѡ��ر�
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CEREG=1\r\n"); //��������ע��״̬�Զ��ظ�
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CSCON=1\r\n"); //��������״̬�Զ��ظ�
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CPSMS=1,,,\"01000011\",\"01000011\"\r\n"); //��PSM
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CEDRXS=0\r\n"); //��eDRX
    judgeok(NB, 1);

    SendAtCommand(NB, "AT+QNBIOTEVENT=1,1\r\n"); //��������PSM״̬�ϱ�
    judgeok(NB, 1);
    if (NB->NB_Cfg.NBBAND == USE_Band_All)
    {
        SendAtCommand(NB, "AT+QBAND=3,5,8,3\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_5) //����
    {
        SendAtCommand(NB, "AT+QBAND=1,5\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_8) //�ƶ���ͨ
    {
        SendAtCommand(NB, "AT+QBAND=1,8\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_3) //��ͨ1800
    {
        SendAtCommand(NB, "AT+QBAND=2,8,3\r\n");
    }
    judgeok(NB, 5);

#endif
    NB->NB_Stat.Link_Enable = 0;
}

char ReOperation(NB_Type *NB, uint8_t recount, uint8_t times)
{
    if (recount == 0)
    {
        NB->NB_Count.NBAtCount = 0;
        NB->NB_Count.NBTimer = times;
        NB->NB_Count.NBNextStep = NB->NB_Count.NBCurrentStep;
        judgeok(NB, times);
    }
    else
    {
        if (NB->NB_Count.NBNextStep != NB->NB_Count.NBCurrentStep)
        {
            NB->NB_Count.NBNextStep = NB->NB_Count.NBCurrentStep;
            NB->NB_Count.NBAtCount = 0;
        }

        if (++NB->NB_Count.NBAtCount <= recount)
        {
            NB->NB_Count.NBCurrentStep = NBStep_Wait;
        }
        else
        {
            NB->NB_Count.NBNextStep = 0xff;
            NB->NB_Count.NBCurrentStep = NBStep_Off;
            return 1;
        }
        NB->NB_Count.NBTimer = times;
    }
    return 0;
}

//����0���ӳɹ�
//����1���źŵ���û���ųɹ�
//����2���ź�
//����3ģ��������
//����4û�п�
//����5δ֪
//����55���ӱ���ֹ
//����AA�������ӹ�����
__weak int GY_NB_Connect(NB_Type *NB)
{
    switch (NB->NB_Count.NBCurrentStep)
    {
#if Use_BC260 > 0
    case NBStep_NRB:
    {
        NB->NB_Stat.rdy = 0;
        SendAtCommand(NB, "AT+QRST=1\r\n");
        judgeRDY(NB, 5); //�ȴ��յ���λ��־
        NB->NB_Count.NBCurrentStep = NBStep_AT;
    }
    break;
    case NBStep_AT:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT\r\n");
    }
    break;
    case NBStep_CFUNx:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+CFUN?\r\n");
    }
    break;
    case NBStep_CFUN0:
    {
        if (ReOperation(NB, 2, 10) == 0)
            SendAtCommand(NB, "AT+CFUN=0\r\n");
    }
    break;
    case NBStep_YIPIN:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+QCSEARFCN\r\n");
    }
    break;
    case NBStep_CFUN1:
    {
        if (ReOperation(NB, 2, 10) == 0)
            SendAtCommand(NB, "AT+CFUN=1\r\n");
    }
    break;
    case NBStep_QSCLK:
    {
        SendAtCommand(NB, "AT+QSCLK=0\r\n");
        ReOperation(NB, 0, 2);
        SendAtCommand(NB, "AT+CPSMS=1,,,\"01000011\",\"01000011\"\r\n"); //��PSM
        ReOperation(NB, 0, 2);
        SendAtCommand(NB, "AT+QNBIOTEVENT=1,1\r\n"); //��������PSM״̬�ϱ�
        ReOperation(NB, 0, 2);
        NB->NB_Count.NBCurrentStep = NBStep_CGSN;
    }
    break;
    case NBStep_CGSN:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+CGSN=1\r\n"); //��ģ���
    }
    break;
    case NBStep_NCCID:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+QCCID\r\n"); //��sim����
    }
    break;
    case NBStep_CIMI:
    {
        SendAtCommand(NB, "AT+CIMI\r\n"); //��cimi��
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_CGATTx;
    }
    break;
    case NBStep_CGATTx:
    {
        //NB->NB_Stat.CGATT = 0;

        if (NB->NB_SaveStat.psmenter == 1)
        {
            NB->NB_SaveStat.psmenter = 0;
            NB->NB_Count.NBNextStep = NBStep_NRB;
            NB->NB_Count.NBCurrentStep = NBStep_Wait;
            NB->NB_Count.NBTimer = 2;
            SendAtCommand(NB, "AT+CGATT?\r\n");
        }
        else
        {
            if (ReOperation(NB, ReCgatt, 2) == 0)
                SendAtCommand(NB, "AT+CGATT?\r\n");
            if (NB->NB_Count.NBCurrentStep == NBStep_Off)
            {
                NB->NB_Count.NBCurrentStep = NBStep_CSQ;
            }
        }
    }
    break;
    case NBStep_NUESTATS:
    {
        NB->NB_Stat.ReadNetCoe = 1;
        SendAtCommand(NB, "AT+QENG=0\r\n");
        if (NB->NB_Cfg.NB_IP.LinkMode == CoapNum)
        {
            ReOperation(NB, 0, 2);
            NB->NB_Count.NBCurrentStep = NBStep_Coap_1;
        }
        else if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
        {
            ReOperation(NB, 0, 2);
            NB->NB_Count.NBCurrentStep = NBStep_OneNet_1;
        }
        else
        {
            ReOperation(NB, 0, 2);
            NB->NB_Count.NBCurrentStep = NBStep_UDP_TCP_1;
        }
    }
    break;
    case NBStep_Coap_1:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+QLACFG=\"platform\",0\r\n"); //����Ŀ��ƽ̨Ϊ��������������ƽ̨
    }
    break;
    case NBStep_Coap_2:
    {
        //AT+QLACONFIG=0,"180.101.147.115",5683,"867724030023557",300,3
        if (ReOperation(NB, 3, 2) == 0)
            SendCoapIPort(NB);
    }
    break;
    case NBStep_Coap_3:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+QLAADDOBJ=19,0,1,0\r\n"); //��Ӷ��� 19/0/0
    }
    break;
    case NBStep_Coap_4:
    {
        if (ReOperation(NB, 1, 5) == 0) //ֻ������һ�Σ�ʧ�ܾ�����
            SendAtCommand(NB, "AT+QLAREG\r\n");
    }
    break;
    case NBStep_Coap_5:
    {
        //+QLAURC: "observe",50408,0,19,0,0
        if (ReOperation(NB, 1, 5) == 0) //ֻ������һ�Σ�ʧ�ܾ�����
            oberveSend(NB);             //����Ӧ��
    }
    break;
    case NBStep_UDP_TCP_1:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+QICFG=\"dataformat\",1,1\r\n");
    }
    break;
    case NBStep_UDP_TCP_2:
    {
        if ((NB->NB_Boty.DGRAM < 0X30) || (NB->NB_Boty.DGRAM > 0X34))
        {
            NB->NB_Boty.DGRAM = 0X30;
        }
        if (ReOperation(NB, 1, 5) == 0) //ֻ������һ�Σ�ʧ�ܾ�����
        {
            if (NB->NB_Cfg.NB_IP.LinkMode == UDPNum)
            {
                SendUdpTcpIpPort(NB, 0); //udp
            }
            else
            {
                SendUdpTcpIpPort(NB, 1); //tcp
            }
        }
    }
    break;
    case NBStep_OneNet_1:
    {
        if (ReOperation(NB, 1, 5) == 0)             //ֻ������һ�Σ�ʧ�ܾ�����
            SendAtCommand(NB, "AT+MIPLCREATE\r\n"); //����һ�� LwM2M �ͻ���ʵ��
    }
    break;
    case NBStep_OneNet_2:
    {
        if (ReOperation(NB, 1, 5) == 0)                                //ֻ������һ�Σ�ʧ�ܾ�����
            SendAtCommand(NB, "AT+MIPLADDOBJ=0,3200,1,\"1\",1,0\r\n"); //��� Object, Instance �� Resource
    }
    break;
    case NBStep_OneNet_3:
    {
        if (ReOperation(NB, 2, 30) == 0) //��Ҫ��ȴ�һ��ʱ��
        {
            SendAtCommand(NB, "AT+MIPLOPEN=0,3600\r\n"); //�ͻ���ע�� OneNET ƽ̨
            NB->NB_Count.LinkTime = 0;
        }
    }
    break;
    case NBStep_OneNet_4:
    {
        if (ReOperation(NB, 1, 5) == 0) //ֻ������һ�Σ�ʧ�ܾ�����
            OneNetojb1Rec(NB);
    }
    break;
    case NBStep_OneNet_5:
    {
        if (ReOperation(NB, 3, 2) == 0)
            OneNetojb2Rec(NB);
    }
    break;
    case NBStep_CSQ:
    {
        NB->NB_Stat.RssiOk = 0;
        SendAtCommand(NB, "AT+CSQ\r\n");
        ReOperation(NB, 0, 2);
        if (NB->NB_Stat.CGATT == 1) //��������Ƶ��֧�ж�
        {
#if NB_CLK > 0
            NB->NB_Count.NBCurrentStep = NBStep_Clk;
#else
            NB->NB_Count.NBCurrentStep = NBStep_NUESTATS;
#endif
        }
        else
        {
            NB->NB_Count.NBCurrentStep = NBStep_Jude;
        }
    }
    break;
#if NB_CLK > 0
    case NBStep_Clk:
    {
        SendAtCommand(NB, "AT+CCLK?\r\n");
        ReOperation(NB, 0, 2);
        NB->NB_Count.NBCurrentStep = NBStep_NUESTATS;
    }
    break;
#endif

#endif

#if Use_BC35_BC95_BC28 > 0
    case NBStep_NRB:
    {
        SendAtCommand(NB, "AT+NRB\r\n");
        ReOperation(NB, 0, 10);
        NB->NB_Count.NBCurrentStep = NBStep_AT;
    }
    break;
    case NBStep_AT:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT\r\n");
    }
    break;
    case NBStep_CFUN1:
    {
        if (ReOperation(NB, 2, 10) == 0)
            SendAtCommand(NB, "AT+CFUN=1\r\n");
    }
    break;
    case NBStep_CFUNx:
    {
        NB->NB_Stat.CFUNOk = 0;
        if (ReOperation(NB, 10, 2) == 0)
            SendAtCommand(NB, "AT+CFUN?\r\n");
    }
    break;
    case NBStep_CGATT:
    {
        SendAtCommand(NB, "AT+CGATT=1\r\n");
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_QREGSWT;
    }
    break;
    case NBStep_QREGSWT:
    {
        if (NB->NB_Cfg.NB_IP.LinkMode == CoapNum)
        {
            SendAtCommand(NB, "AT+QREGSWT=1\r\n");
            ReOperation(NB, 0, 1);
            NB->NB_Count.NBCurrentStep = NBStep_NCDP;
        }
        else
        {
            SendAtCommand(NB, "AT+QREGSWT=2\r\n");
            ReOperation(NB, 0, 1);
            NB->NB_Count.NBCurrentStep = NBStep_CMEE;
        }
    }
    break;
    case NBStep_NCDP:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendCoapIPort(NB);
    }
    break;
    case NBStep_NNMI:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+NNMI=1\r\n");
    }
    break;
    case NBStep_CMEE:
    {
        SendAtCommand(NB, "AT+CMEE=1\r\n");
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_CGSN;
    }
    break;
    case NBStep_CGSN:
    {
        SendAtCommand(NB, "AT+CGSN=1\r\n"); //��ģ���
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_NCCID;
    }
    break;
    case NBStep_NCCID:
    {
        SendAtCommand(NB, "AT+NCCID\r\n"); //��sim����
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_CGDCONT;
    }
    break;
    case NBStep_CGDCONT:
    {
        SendAPNCommand(NB, NB->NB_Cfg.NB_IP.NBAPN); //APN���óɲ�������ν
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_CPSMS;
    }
    break;
    case NBStep_CPSMS:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+CPSMS=1,,,01000011,00000011\r\n"); //��PSM
    }
    break;
    case NBStep_NPSMR:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+NPSMR=1\r\n"); //����PSM�Զ�����
    }
    break;
    case NBStep_CEREG:
    {
        SendAtCommand(NB, "AT+CEREG=1\r\n");
        ReOperation(NB, 0, 2);
        NB->NB_Count.NBCurrentStep = NBStep_CGATTx;
    }
    break;
    case NBStep_CGATTx:
    {
        NB->NB_Stat.CGATT = 0;

        if (NB->NB_SaveStat.psmenter == 1)
        {
            NB->NB_SaveStat.psmenter = 0;
            NB->NB_Count.NBNextStep = NBStep_NRB;
            NB->NB_Count.NBCurrentStep = NBStep_Wait;
            NB->NB_Count.NBTimer = 2;
            SendAtCommand(NB, "AT+CGATT?\r\n");
        }
        else
        {
            if (ReOperation(NB, ReCgatt, 2) == 0)
                SendAtCommand(NB, "AT+CGATT?\r\n");
            if (NB->NB_Count.NBCurrentStep == NBStep_Off)
            {
                NB->NB_Count.NBCurrentStep = NBStep_CSQ;
            }
        }
    }
    break;
    case NBStep_NUESTATS:
    {
        NB->NB_Stat.ReadNetCoe = 1;
        SendAtCommand(NB, "AT+NUESTATS\r\n");
        if (NB->NB_Cfg.NB_IP.LinkMode == CoapNum)
        {
            ReOperation(NB, 0, 2);
            NB->NB_Count.NBCurrentStep = NBStep_CGPADDR;
        }
        else if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
        {
            ReOperation(NB, 0, 2);
            NB->NB_Count.NBCurrentStep = NBStep_OneNetCfg;
        }
        else
        {
            ReOperation(NB, 0, 2);
            NB->NB_Count.NBCurrentStep = NBStep_NSOCR;
        }
    }
    break;
    case NBStep_CGPADDR:
    {
        SendAtCommand(NB, "AT+CGPADDR\r\n"); //PDP��ַ
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_CSCON;
    }
    break;
    case NBStep_NSOCR:
    {
        NB->NB_Stat.ReadDREAM = 1;
        if (NB->NB_Cfg.NB_IP.LinkMode == TCPNum)
        {
            SendAtCommand(NB, "AT+NSOCR=STREAM,6,56000,1\r\n");
            ReOperation(NB, 0, 2);
            NB->NB_Count.NBCurrentStep = NBStep_NSOCO;
        }
        else if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
        {
            SendAtCommand(NB, "AT+MIPLCREATE\r\n");
            ReOperation(NB, 0, 2);
            NB->NB_Count.NBCurrentStep = NBStep_OneNetObj;
        }
        else
        {
            SendAtCommand(NB, "AT+NSOCR=DGRAM,17,4028,1\r\n");
            ReOperation(NB, 0, 2);
            NB->NB_Count.NBCurrentStep = NBStep_CSCON;
        }
        if ((NB->NB_Boty.DGRAM >= '0') && (NB->NB_Boty.DGRAM <= '9'))
        {
            //���socket�Ŷ���͹�������
        }
        else
        {
            NB->NB_Count.NBCurrentStep = NBStep_Off;
        }
    }
    break;
    case NBStep_CSCON:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+CSCON=1\r\n");
    }
    break;
    case NBStep_CSQ:
    {
        NB->NB_Stat.RssiOk = 0;
        SendAtCommand(NB, "AT+CSQ\r\n");
        ReOperation(NB, 0, 2);
#if NB_CLK > 0
        NB->NB_Count.NBCurrentStep = NBStep_Clk;
#else
        NB->NB_Count.NBCurrentStep = NBStep_Jude;
#endif
    }
    break;
    case NBStep_CFUN0:
    {
        if (ReOperation(NB, 2, 10) == 0)
            SendAtCommand(NB, "AT+CFUN=0\r\n");
    }
    break;
    case NBStep_NCSEARFCN:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+NCSEARFCN\r\n");
    }
    break;
    case NBStep_NSOCO:
    {
        //��������һ��ʧ����������
        if (ReOperation(NB, 1, 15) == 0)
            SendTcpIpPort(NB);
    }
    break;

#if NB_CLK > 0
    case NBStep_Clk:
    {
        SendAtCommand(NB, "AT+CCLK?\r\n");
        ReOperation(NB, 0, 2);
        NB->NB_Count.NBCurrentStep = NBStep_Jude;
    }
    break;
#endif

    case NBStep_OneNetCfg:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+MIPLCONFIG=6,1\r\n");
    }
    break;
    case NBStep_OneNetObj:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+MIPLADDOBJ=0,3200,1,\"1\",1,0\r\n"); //OneNetobjID
    }
    break;
    case NBStep_OneNetOpen:
    {
        if (ReOperation(NB, 1, 30) == 0) //�ȴ�30sע�ᵽOneNet
            SendAtCommand(NB, "AT+MIPLOPEN=0,3000,30\r\n");
    }
    break;
    case NBStep_OneNetRsp:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendOneNetCommand(NB);
    }
    break;
#endif
    case NBStep_Jude:
    {
        NB->NB_Count.NBCurrentStep = 0;
        NB->NB_Count.NBNextStep = 0;
        if (NB->NB_Stat.CGATT)
        {
            //ֻҪ����������Ϳ��Դ�����
            //�����־��ζ�ſ��Է���������
            NB->NB_Stat.UpXinTiao = 1;
            return 0;
        }
        else
        {
#ifndef MastYP
            if (NB->NB_Stat.RssiOk)
#else
            if (1)
#endif
            {
                //���źŵ���5����δ���ųɹ�����Ҫ��Ƶ����
                NB->NB_Stat.RssiOk = 0;
                NB->NB_Stat.NeedYIPIN = 1;
                NB->NB_Count.LinkMaxTime = 0;
#if extradd
                DisplayErroy(NoCEREG);
#endif
                return 1;
            }
            else
            {
                //û�źŻ�û�ɹ���������ֱ�ӹ�
                NB->NB_Count.ReCount = 3;
#if extradd
                DisplayErroy(NoRSSI);
#endif
                return 2;
            }
        }
    }
    case NBStep_Off:
    {
        NB->NB_Count.NBCurrentStep = 0;
        NB->NB_Count.NBNextStep = 0;
        if (NB->NB_Stat.NBok == 0)
        {
            NB->NB_Count.ReCount += 1;
#if extradd
            DisplayErroy(NBERR);
#endif
            return 3;
        }
        else if (NB->NB_Stat.CFUNOk == 0)
        {
            NB->NB_Count.ReCount += 1;
#if extradd
            DisplayErroy(NoSimCard);
#endif
            return 4;
        }
        else
        {
            NB->NB_Count.ReCount += 1;
#if extradd
            DisplayErroy(UnknownERR);
#endif
            return 5;
        }
    }
    default:
    {
        if (NB->NB_Stat.Link_Enable == 0)
        {
            return 55;
        }
    }
    break;
    }

    return NB_Connecting;
}

__weak void GY_NB_End(NB_Type *NB)
{
#if Use_BC35_BC95_BC28 > 0
    if (NB->NB_Cfg.NB_IP.LinkMode != CoapNum)
    {
        //��������
        if (NB->NB_Boty.DGRAM == 0X30)
        {
            if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
            {
                SendAtCommand(NB, "AT+MIPLCLOSE=0\r\n");
            }
            else
            {
                SendAtCommand(NB, "AT+NSOCL=0\r\n");
            }
        }

        if (NB->NB_Boty.DGRAM == 0X31)
        {
            if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
            {
                SendAtCommand(NB, "AT+MIPLCLOSE=1\r\n");
            }
            else
            {
                SendAtCommand(NB, "AT+NSOCL=1\r\n");
            }
        }
        if (NB->NB_Boty.DGRAM == 0X32)
        {
            if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
            {
                SendAtCommand(NB, "AT+MIPLCLOSE=2\r\n");
            }
            else
            {
                SendAtCommand(NB, "AT+NSOCL=2\r\n");
            }
        }
        judgeok(NB, 1);
    }
		else
		{
			SendAtCommand(NB, "AT+QLWSREGIND=1\r\n");
			judgeok(NB, 2);
		}

    if (NB->NB_SaveStat.psmenter == 0)
    {
        /*δ�ɹ�����psm*/
        SendAtCommand(NB, "AT+CSCON=0\r\n");
        judgeok(NB, 1);
        SendAtCommand(NB, "AT+CFUN=0\r\n");
        judgeok(NB, 3);
    }
#endif

#if Use_BC260 > 0
    if ((NB->NB_Cfg.NB_IP.LinkMode == UDPNum) || (NB->NB_Cfg.NB_IP.LinkMode == TCPNum))
    {
        SendAtCommand(NB, "AT+QICLOSE=0\r\n"); //�ر�����
        judgeok(NB, 1);
    }
    if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
    {
        SendAtCommand(NB, "AT+MIPLCLOSE=0\r\n"); //�ر�����
        judgeok(NB, 1);
    }
    SendAtCommand(NB, "AT+QSCLK=1\r\n"); //�ͷ�������
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CFUN=0\r\n");
    judgeok(NB, 4);
#endif

    NB->NB_Stat.NBStat = 0;
    memset(&NB->NB_Count, 0, sizeof(NB->NB_Count));
}

void UDP_RE(NB_Type *NB)
{
    if (NB->NB_Stat.ReReadFlag == 1)
    {
        NB_UDP_Read(NB);
        NB->NB_Stat.ReReadFlag = 0;
    }
}

__weak void GY_NB_Clock(NB_Type *NB)
{
    if (NB->NB_Stat.Link_Enable == 1)
    {
        if (NB->NB_Count.NBTimer > 0)
        {
            NB->NB_Count.NBTimer--;
        }
        else
        {
            NB->NB_Count.NBCurrentStep = NB->NB_Count.NBNextStep;
        }

        //IWDT_Clr();

        if (NB->NB_Stat.ERRORAgainUpFlag == 1)
        {
            if (++NB->NB_Count.ERRORAgainUpTimeCount > 20)
            {
                NB->NB_Count.ERRORAgainUpTimeCount = 0;
                NB->NB_Stat.ERRORAgainUpFlag = 0;
                NB->NB_Stat.UpXinTiao = 1;
            }
        }

        if (NB->NB_Stat.TxFlag == 1)
        {
            if (++NB->NB_Count.ProTime1 > vTxFlagTime)
            {
                NB->NB_Stat.TxFlag = 0;
                NB->NB_Count.ProTime1 = 0;
            }
        }

        if ((NB->NB_Stat.ReFlag == 1) && (NB->NB_Stat.TxFlag == 0))
        {
            if (++NB->NB_Count.ProTime2 > vReFlagTime)
            {
                NB->NB_Count.ProTime2 = 0;
                NB->NB_Stat.ReReadFlag = 1;
                NB->NB_Stat.ReFlag = 0;
            }
        }

        UDP_RE(NB);

        if (NB->NB_Stat.fotaupflag == 0)
        {
            if (++NB->NB_Count.LinkMaxTime >= vLinkMaxTime) //NBģ���ϴ������������ʱ��
            {
                NB->NB_Stat.Link_Enable = 0;
            }
            if (NB->NB_Stat.CGATT == 1) //����ʱ����ʵ������20s���ߺ��Զ��ػ����������һ��ûʲô��
            {
                if (++NB->NB_Count.LinkTime >= vLinkTime)
                {
                    NB->NB_Stat.Link_Enable = 0;
                }
            }
        }
        else
        {
            NB->NB_Stat.fotaupflag = 0;
            if (++NB->NB_Count.LinkTime >= vFotaLinkMaxTime)
            {
                NB->NB_Stat.Link_Enable = 0;
            }
        }
    }
    else
    {
        if (NB->NB_SaveStat.psmquit == 1)
        {
            /*�˳���psm*/
            if (++NB->NB_Count.PsmCount >= vLinkTime)
            {
                NB->NB_SaveStat.psmquit = 0;
                NB->NB_Count.PsmCount = 0;
                SendAtCommand(NB, "AT+CFUN=0\r\n");
            }
        }
        else
        {
            NB->NB_Count.PsmCount = 0;
        }
    }
}

/*
	��ؽ�NBIPת��Ϊ15λ��'.'��ʽIP
	��ؽ�NBPORTתΪ5λ����Port
	��Ҫ��ʼ��������
	NB->NB_Cfg.rxbuff=NBRxBuf;
	NB->NB_Cfg.txbuff=NBTxBuf;
	NB->NB_Cfg.NBBAND=0;
	NB->NB_Cfg.NBBaud=9600;
	memcpy(NB->NB_Cfg.NB_IP.ServerIP,IP1,15);
	memcpy(NB->NB_Cfg.NB_IP.PORT,PORT1,5);
*/
__weak int GY_NB_Link(NB_Type *NB)
{
    IPReduceZero(NB->NB_Cfg.NB_IP.ServerIP);   //ת��NB��ʹ�õ�IP
    PortReduceZero_New(NB->NB_Cfg.NB_IP.PORT); //ת��NB��ʹ�õ�PORT
    memset(&NB->NB_Count, 0, sizeof(NB->NB_Count));
    NB->NB_Stat.NBStat = (uint32_t)0x01;

    if (NB->NB_SaveStat.psmenter == 1)
        NB->NB_Count.NBCurrentStep = NBStep_CGATTx;
    else
        NB->NB_Count.NBCurrentStep = NBStep_NRB;
    return GY_NB_ConPro(NB);
}

/*
*********************************************************************************************************
*	�� �� ��: RXdecoding
*	����˵��: ��asciiתhex  �ܹ�ʶ���Сд
*	��    ��: code   �������Ҫת��������
*             decode �����ת��������
*             len ����Ҫת�ĳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RXdecoding(uint8_t *code, uint8_t *decode, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        if ((code[i * 2] <= 0x39) && (code[i * 2] >= 0x30))
        {
            code[i * 2] = code[i * 2] - 0x30;
        }
        else if ((code[i * 2] <= 0x46) && (code[i * 2] >= 0x40))
        {
            code[i * 2] = code[i * 2] - 0x40 + 0x09;
        }
        else if ((code[i * 2] <= 0x66) && (code[i * 2] >= 0x60))
        {
            code[i * 2] = code[i * 2] - 0x60 + 0x09;
        }
        if ((code[i * 2 + 1] <= 0x39) && (code[i * 2 + 1] >= 0x30))
        {
            code[i * 2 + 1] = code[i * 2 + 1] - 0x30;
        }
        else if ((code[i * 2 + 1] <= 0x46) && (code[i * 2 + 1] >= 0x40))
        {
            code[i * 2 + 1] = code[i * 2 + 1] - 0x40 + 0x09;
        }
        else if ((code[i * 2 + 1] <= 0x66) && (code[i * 2 + 1] >= 0x60))
        {
            code[i * 2 + 1] = code[i * 2 + 1] - 0x60 + 0x09;
        }
        decode[i] = ((code[i * 2] << 4) + code[i * 2 + 1]);
    }
}

/*�ؼ��ֲ�Ҫ����5�ֽ�*/
__weak uint16_t GY_NB_RxData(NB_Type *NB, uint8_t *buf, const char *word)
{
    char *Location = NULL;
    uint8_t ta = 0, tb = 0;
    uint16_t rxlen = 0;
    char nword[10] = {0};
    uint16_t len = mystrlen(word, 5);

    /*ʵ�ֽ��ؼ��ִ�Сдת��*/
    for (ta = 0; ta < len; ta++)
    {
        if ((word[ta] >= 0x40) && (word[ta] <= 0x46))
        {
            nword[ta] = word[ta] + 0x20;
        }
        else if ((word[ta] >= 0x60) && (word[ta] <= 0x66))
        {
            nword[ta] = word[ta] - 0x20;
        }
        else if ((word[ta] >= 0x30) && (word[ta] <= 0x39))
        {
            nword[ta] = word[ta];
        }
    }
    ta = 0;
#if Use_BC35_BC95_BC28 > 0
    if (strstr((char *)NB->NB_Cfg.rxbuff, "+NNMI:"))
    {
        //+NNMI:49,FF1161389634170101B.....
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "+NNMI:");
        for (tb = 0; tb < 4; tb++)
        {
            if (Location[6 + tb] == ',')
            {
                break;
            }
            else
            {
                rxlen *= 10;
                rxlen += (Location[6 + tb] - 0x30);
            }
        }
        ta = 6 + tb;
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+MIPLWRITE:"))
    {
        //+MIPLWRITE: 0,53583,3200,0,5750,2,5,12345,0,0
        uint8_t td = 0, tf = 0;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "+MIPLWRITE:");
        for (tb = 0; tb < 50; tb++)
        {
            if (Location[11 + tb] == ',')
            {
                td++;
                if (td == 6)
                {
                    for (tf = 0; tf < 4; tf++)
                    {
                        if (Location[12 + tb + tf] == ',')
                        {
                            break;
                        }
                        else
                        {
                            rxlen *= (10 * tf);
                            rxlen += (Location[12 + tb + tf] - 0x30);
                        }
                    }
                    break;
                }
            }
        }
        rxlen >>= 1;
        ta = 12 + tb + tf;
    }
    else
    {
        char tc = 0;
        uint8_t ServeripNum, PortNum;
        ServeripNum = mystrlen(NB->NB_Cfg.NB_IP.ServerIP, 15);
        PortNum = mystrlen(NB->NB_Cfg.NB_IP.PORT, 5);
        Location = strstr((char *)NB->NB_Cfg.rxbuff, NB->NB_Cfg.NB_IP.ServerIP);
        for (tb = 0; tb < 5; tb++)
        {
            if (Location[ServeripNum + 1 + PortNum + tb] == ',')
            {
                for (tc = 0; tc < 4; tc++)
                {
                    if (Location[ServeripNum + 2 + PortNum + tb + tc] == ',')
                        break;
                    else
                    {
                        rxlen *= 10;
                        rxlen += (Location[ServeripNum + 2 + PortNum + tb + tc] - 0x30);
                    }
                }
                break;
            }
        }
        ta = ServeripNum + 3 + PortNum + tb + tc;
    }
#endif

#if Use_BC260 > 0

    /*+QLAURC: "write",3735,19,1,0,2,50,"data",0*/
    if (strstr((char *)NB->NB_Cfg.rxbuff, "QLAURC: "))
    {
        char *Locationl = NULL, countdou = 0;
        int len = strlen("QLAURC: \"write\",");
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "QLAURC: \"write\",");
        for (;;)
        {
            Locationl = strstr(&Location[len], ",");
            if (Locationl != NULL)
            {
                Location = Locationl + 1;
                len = 0;
                countdou++;
                if (countdou == 5)
                    break;
            }
            else
            {
                break;
            }
        }
        if (Locationl != NULL)
        {
            for (tb = 0; tb < 4; tb++)
            {
                if (Location[tb] == ',')
                {
                    break;
                }
                else
                {
                    rxlen *= 10;
                    rxlen += (Location[tb] - 0x30);
                }
            }
            ta = tb + 2; //+2����Ϊ rxlen֮���� ," �����ַ�
        }
    }
    /*+QIURC: "recv",0,4,"54657374"*/
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "QIURC: "))
    {
        int len = strlen("QIURC: \"recv\",0,");
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "QIURC:");

        for (tb = 0; tb < 4; tb++)
        {
            if (Location[len + tb] == ',')
            {
                break;
            }
            else
            {
                rxlen *= 10;
                rxlen += (Location[len + tb] - 0x30);
            }
        }
        ta = len + tb + 2; //+2����Ϊ rxlen֮���� ," �����ַ�
    }
    else if (strstr((char *)NB->NB_Cfg.rxbuff, "MIPLWRITE:"))
    {
        //+MIPLWRITE: 0,11535,3200,0,5750,2,8,3131323233333434,0,0
        char *Location, *Locationl;
        int len = strlen("MIPLWRITE: 0,"),countdou = 0;
        Location = strstr((char *)NB->NB_Cfg.rxbuff, "MIPLWRITE: 0,");
        Locationl = strstr(&Location[len], ",");
        if ((Locationl != NULL) && (Locationl > (Location + len)))
        {
            memset(NB->NB_Boty.msgid2, 0, 20);
            memcpy(NB->NB_Boty.msgid2, &Location[len], Locationl - Location - len);
            OneNetWriteRec(NB);
            memset(Location, 0, len);
        }
        // 123456789
        // +MIPLWRITE: 0,8506,3200,0,5750,2,9,313233343536373839,0,0
        // AT+MIPLWRITERSP=0,8506,2
        // ���������
        for (;;)
        {
            Locationl = strstr(&Location[len], ",");
            if (Locationl != NULL)
            {
                Location = Locationl + 1;
                len = 0;
                countdou++;
                if (countdou == 5)
                    break;
            }
            else
            {
                break;
            }
        }
        if (Locationl != NULL)
        {
            for (ta = 0; ta < 4; ta++)
            {
                if (Location[ta] == ',')
                {
                    break;
                }
                else
                {
                    rxlen *= 10;
                    rxlen += (Location[ta] - 0x30);
                }
            }
            ta += 1; //+1����Ϊ rxlen֮���� , 1���ַ�
            Location = &Location[ta];
            AsciiToHex((uint8_t *)Location, (uint8_t *)Location, rxlen);
            rxlen >>= 1;
        }
    }
#endif

    if (rxlen > (NBRXlen >> 1))
    {
        rxlen = (NBRXlen >> 1) - 1;
    }

    if (rxlen != 0)
    {
        if (strnstr(&Location[ta], word, mystrlen(word, 5), 10))
        {
            RXdecoding((uint8_t *)strstr(&Location[ta], word), buf, rxlen);
            return rxlen;
        }
        else if (strnstr(&Location[ta], nword, mystrlen(nword, 5), 10))
        {
            RXdecoding((uint8_t *)strstr(&Location[ta], nword), buf, rxlen);
            return rxlen;
        }
    }

    return 0;
}

int NB_printf(NB_Type *NB, const char *format, ...)
{
    char c;
    char *p = NULL;
    int len = 0, limlen = 0, a = 0;
    char buf[20];
    va_list ap;           //����һ��int *�͵�ָ��ap
    va_start(ap, format); //��ʼ��ap������ָ���һ���ɱ�����ĵ�ַ

    while ((c = *format++) != '\0') //��ʼ����printf�����е��ַ���(����һ���̶�����)
    {
        switch (c) //��while�У����ζ����ַ����е��ַ��������������ؽ����жϺͽ���
        {
        case '%':          //����ַ���%������ʽ�������ţ�������ø�ʽ
            c = *format++; //��c����%��ĵ�һ����ĸ
            switch (c)     //��������ĸ���н���
            {
            case '0': //���ڳ�������
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                p = NULL;
                limlen = c - 0x30;
                for (;;)
                {
                    c = *format++;
                    if ((c >= '0') && (c <= '9'))
                    {
                        limlen *= 10;
                        limlen += (c - 0x30);
                    }
                    else if (c == 's')
                    {
                        p = va_arg(ap, char *);
                        break;
                    }
                    else if (c == 'd')
                    {
                        a = va_arg(ap, int);
                        sprintf(buf, "%d", a);
                        p = buf;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
                if (p != NULL)
                {
                    NB_Send(NB, (uint8_t *)p, mystrlen(p, limlen));
                }
                break;
            case 'c':                          //�����%c���������ĸ
                c = va_arg(ap, int);           //��ȡ��ǰ�ɱ������ֵ����ָ����һ���ɱ����
                NB_Send(NB, (uint8_t *)&c, 1); //���õײ�(ʵ�ʲ���Ӳ�����ǲ�)�����������
                break;
            case 'l':                  //�����%l������ֵ����
                len = va_arg(ap, int); //��ȡ��ǰ�ɱ������ֵ����ָ����һ���ɱ����
                break;
            case 's': //�����%s��������ַ���
                p = va_arg(ap, char *);
                SendAtCommand(NB, p);
                break;
            case 'a': //�����%a��������ת��ascii�����  ��a����ǰ��������%l�ĳ���
                p = va_arg(ap, char *);
                for (a = 0; a < len; a++)
                {
                    HexToAscll(buf, (uint8_t *)p, 1);
                    NB_Send(NB, (uint8_t *)buf, 2);
                    p++;
                }
                len = 0;
                break;
            case 'd': //�����%d������ʮ�������
                a = va_arg(ap, int);
                sprintf(buf, "%d", a);
                SendAtCommand(NB, buf);
                break;
            default:
                break;
            }
            break;
        default:
            NB_Send(NB, (uint8_t *)&c, 1); //����ַ�����Ϊ��ͨ�ַ����ַ�
            break;
        }
    }
    return 0;
}

#endif
