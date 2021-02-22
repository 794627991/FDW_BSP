/*
*********************************************************************************************************
*
*	模块名称 : NB联网程序
*	文件名称 : iot_api.c
*	版    本 : V0.0.1
*	说    明 : NB联网程序，内含BC28,BC260联网流程
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V0.0.1  2021-1-5    高屹    旧版本，作废
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "iot_api.h"

#if API_UseNB > 0
NB_Type NB; /* 结构体定义 */

__weak void NBRxDeal(void)
{
}

__weak void NBTxDeal(void)
{
}

#if ThisBoot == 0
/*----------------------------------------------------------------------
//NB使用例程
//在调用NB_Init();前需要调用NB_Set设置一下参数
//详细的NB初始化流程见void NB_Init_Exp(void)
//
//void TimeCpm(void)函数的功能是在联上网络后自动校时
//int NB_Start(NB_IP_Type configcoe)是NB开启联网的函数
//
//详细的NB连接流程见void NB_Connect_Exp(void)
//NB连接完成后需要自己编写发送和接收函数:void NBTxDeal(void)和void NBRxDeal(void)
//
//NB结束连接前需要调用 void NB_SetEndPackStat(void);代表是确认NB真的要结束连接
//只有调用该函数,int NB_Start(NB_IP_Type configcoe)才会返回1,表示连接成功
//否则一律返回0
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
    //	NB.NB_Cfg.NB_IP.LinkMode=UDPNum;          //用UDP连接
    //	NB.NB_Cfg.NB_IP.NBAPN=APN;                //APN
    //	memcpy(NB.NB_Cfg.NB_IP.ServerIP,IP1,15);  //IP
    //	memcpy(NB.NB_Cfg.NB_IP.PORT,PORT1,5);     //PORT
}

/* --------------------------------------------------------------
返回0：连接成功
返回1：连接失败
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
            ExecuteEvent; //在event.h中提供了宏定义,目的是在NB连接过程中,也能处理事件
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
        //连接成功
        TimeCpm(); //NB自动校时
        while (NB_ReadLinkStat() && NB_ReadOnlineStat())
        {
#if Use_uCOS == 0
            ExecuteEvent; //在event.h中提供了宏定义,目的是在NB连接过程中,也能处理事件
#endif
            WEIGOU;
            if (NB_RevData())
            {
                //接收程序自己写
                NBRxDeal();
            }
            if (NB_SendData())
            {
                //发送程序自己写
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
        //由于各种原因导致连接失败
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
            API_SetTIME(&TempTime); //设置时间
        }
    }
#endif
}

/*NB初始化函数*/
void NB_Init(void)
{
    NB_Set();
    GY_NB_Init(&NB);
}

/*在NB开始连接时调用*/
int NB_Link(void)
{
    return GY_NB_Link(&NB);
}

/*串口接收中调用*/
void NB_UartRx(uint8_t *buf, uint16_t len)
{
    NB.NB_Cfg.rxbuff = buf;
    NB.NB_Count.RxDatalen = len;
    GY_NB_UartRx(&NB);
}

/*发送数据*/
void NB_UartTx(uint8_t *buf, uint16_t len)
{
    NB.NB_Cfg.txbuff = buf;
    NB.NB_Count.TxDatalen = len;
    GY_NB_UartTx(&NB);
}

/*联网过程(单程)*/
int NB_Connect(void)
{
    return GY_NB_Connect(&NB);
}

/*联网过程(多程,3次重复)*/
int NB_ConPro(void)
{
    return GY_NB_ConPro(&NB);
}

/*联网结束调用*/
void NB_End(void)
{
    GY_NB_End(&NB);
}

/*发送数据*/
void NBTXdata(uint16_t len)
{
    NB.NB_Count.TxDatalen = len;
    GY_NB_UartTx(&NB);
}

/****************************************
//	秒定时器中调用
****************************************/
void NB_Clock(void)
{
    GY_NB_Clock(&NB);
}

/****************************************
//	清除连接时间，连接时间到后联网将自动终止
****************************************/
void NB_ClearLinkTime(void)
{
    NB.NB_Count.LinkTime = 0;
}

/*****************************
//	读取NB活动状态
//	1：活动中
//	0：非活动
*****************************/
char NB_ReadLinkStat(void)
{
    if (NB.NB_Stat.Link_Enable)
        return 1;
    else
        return 0;
}

/*****************************
//	读取在线状态
//	1：在线
//	0：不在线
*****************************/
char NB_ReadOnlineStat(void)
{
    if (NB.NB_Stat.NBOnline)
        return 1;
    else
        return 0;
}

/*****************************
//	设置NB活动状态
//	1：使能
//	0：停止
*****************************/
void NB_SetLinkStat(bool setval)
{
    if (setval == 1)
        NB.NB_Stat.Link_Enable = 1;
    else
        NB.NB_Stat.Link_Enable = 0;
}

/*****************************
//	关闭NB连接
*****************************/
void NB_OffOnline(void)
{
    NB.NB_Stat.NBOnline = 0;
}

/*****************************
//	接收到数据
//	1：是
//	0：否
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
//	是否需要发送数据
//	1：是
//	0：否
*****************************/
char NB_SendData(void)
{
    if (NB.NB_Stat.UpXinTiao == 1)
    {
        NB.NB_Stat.UpXinTiao = 0;
        //每次上报都清连接时间
        NB_ClearLinkTime();
        return 1;
    }
    else
        return 0;
}

/*************************************
//	收到结束包意味着可以立即结束NB连接了
*************************************/
void NB_SetEndPackStat(void)
{
    NB.NB_Stat.NBOnline = 0;
    NB.NB_Stat.EndPack = 1;
}

/*****************************
//	是否收到结束包
//	1：是
//	0：否
*****************************/
char NB_ReadEndPackStat(void)
{
    if (NB.NB_Stat.EndPack)
        return 1;
    else
        return 0;
}

/*****************************
//	返回0连接成功
//	返回1有信号但是没附着成功
//	返回2无信号
//	返回3模块有问题
//	返回4没有卡
//	返回5未知
//	返回55连接被中止
//	返回AA正在连接过程中
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
//	输入接收缓存，数据解析缓存，关键字
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
    /*Coap涉及profile文件头需要根据协议自行修改*/
    NBSendBuff[0] = '0';
    NBSendBuff[1] = '0'; //profile固定写法

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
    /*Coap涉及profile文件头需要根据协议自行修改*/
    NBSendBuff[bbb] = '0';
    NBSendBuff[bbb + 1] = '0'; //profile固定写法

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
    /*AT+QLANOTIFY=19,0,0,2,75,"DATA",0,0     即应发送",0,0*/
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
//AT+MIPLNOTIFY=0,9523,3200,0,5750,1,5,”hello”,0,0 //数据主动上报
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
    /*Coap涉及profile文件头需要根据协议自行修改*/
    NBSendBuff[0] = '0';
    NBSendBuff[1] = '0'; //profile固定写法

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
    /*Coap涉及profile文件头需要根据协议自行修改*/
    NBSendBuff[bbb] = '0';
    NBSendBuff[bbb + 1] = '0'; //profile固定写法

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

    OneNetSendLen = NB->NB_Count.TxDatalen * 2; //长度*2

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
        /*1:已注册,2:正在注册,5:漫游注册；0:未注册,3:注册被拒绝,4:未知*/
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

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+QLWEVTIND:3")) //fota升级完成标志和注册上平台标志
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

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "+QLWEVTIND:5")) //fota升级开始标志
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
            case Feb: //平月
            {
                if (((NB->NB_Time.g_cur_date.year % 100 != 0) && (NB->NB_Time.g_cur_date.year % 4 == 0)) || (NB->NB_Time.g_cur_date.year % 400 == 0)) //闰年
                {
                    if (++NB->NB_Time.g_cur_date.day > 29)
                    {
                        NB->NB_Time.g_cur_date.day = 1;
                        ++NB->NB_Time.g_cur_date.month; //月计数
                    }
                }
                else if (++NB->NB_Time.g_cur_date.day > 28)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    ++NB->NB_Time.g_cur_date.month; //月计数
                }
            }
            break;
            case Jan:
            case Mar:
            case May:
            case Jul:
            case Aug:
            case Oct:
            case Dec: //大月
            {
                if (++NB->NB_Time.g_cur_date.day > 31)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    if (++NB->NB_Time.g_cur_date.month > Dec) //月计数
                    {
                        NB->NB_Time.g_cur_date.month = Jan;
                        NB->NB_Time.g_cur_date.year++; //年计数
                    }
                }
            }
            break;
            case Apr:
            case Jun:
            case Sep:
            case Nov: //小月
            {
                if (++NB->NB_Time.g_cur_date.day > 30)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    ++NB->NB_Time.g_cur_date.month; //月计数
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
        if (NB->NB_Stat.CGATT == 1) //说明已经连接上网络了
        {
            if (++NB->NB_Count.ERRORAgainUp < 3)
            {
                NB->NB_Stat.ERRORAgainUpFlag = 1;

                //DisplayErroy(NoZhuCe);//这个错误是因为CoAp没注册erro5
            }
            else
            {
                NB->NB_Stat.Link_Enable = 0;
            }
        }
    }

    else if (strstr((char *)NB->NB_Cfg.rxbuff, "OK"))
    {
        /*只要返回OK说明模块正常*/
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
                if (choutdou == 1) //小区号
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
                    if (Location[len] == '-') //一定是个负数
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
            NB->NB_Count.NBCurrentStep = NBStep_Off; //未注册，直接关
        }
        if ((Location[len] == '1') || (Location[len] == '5'))
        {
            NB->NB_Stat.CGATT = 1; //已注册
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
                break; //当使用coap连接时，必须保证模块号正确
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
            case Feb: //平月
            {
                if (((NB->NB_Time.g_cur_date.year % 100 != 0) && (NB->NB_Time.g_cur_date.year % 4 == 0)) || (NB->NB_Time.g_cur_date.year % 400 == 0)) //闰年
                {
                    if (++NB->NB_Time.g_cur_date.day > 29)
                    {
                        NB->NB_Time.g_cur_date.day = 1;
                        ++NB->NB_Time.g_cur_date.month; //月计数
                    }
                }
                else if (++NB->NB_Time.g_cur_date.day > 28)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    ++NB->NB_Time.g_cur_date.month; //月计数
                }
            }
            break;
            case Jan:
            case Mar:
            case May:
            case Jul:
            case Aug:
            case Oct:
            case Dec: //大月
            {
                if (++NB->NB_Time.g_cur_date.day > 31)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    if (++NB->NB_Time.g_cur_date.month > Dec) //月计数
                    {
                        NB->NB_Time.g_cur_date.month = Jan;
                        NB->NB_Time.g_cur_date.year++; //年计数
                    }
                }
            }
            break;
            case Apr:
            case Jun:
            case Sep:
            case Nov: //小月
            {
                if (++NB->NB_Time.g_cur_date.day > 30)
                {
                    NB->NB_Time.g_cur_date.day = 1;
                    ++NB->NB_Time.g_cur_date.month; //月计数
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
        /*只要返回OK说明模块正常*/
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
    SendAtCommand(NB, "AT+CPSMS=1,,,01000011,00000011\r\n"); //开PSM
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CFUN=0\r\n");
    judgeok(NB, 10);
    SendAtCommand(NB, "AT+NCONFIG=CELL_RESELECTION,TRUE\r\n"); //打开小区重选
    judgeok(NB, 1);
    if (NB->NB_Cfg.NBBAND == USE_Band_All)
    {
        SendAtCommand(NB, "AT+NBAND=3,5,8\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_5) //电信
    {
        SendAtCommand(NB, "AT+NBAND=5\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_8) //移动联通
    {
        SendAtCommand(NB, "AT+NBAND=8\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_3) //联通1800
    {
        SendAtCommand(NB, "AT+NBAND=3\r\n");
    }
    judgeok(NB, 1);

    SendAtCommand(NB, "AT+CEDRXS=0,5\r\n"); //关闭模组eDRX功能
    judgeok(NB, 1);
#endif

#if Use_BC260 > 0
    judgeRDY(NB, 5);               //等待收到复位标志
    SendAtCommand(NB, "ATE0\r\n"); //关闭回显
    judgeok(NB, 1);
    SendAtCommand(NB, "AT\r\n");
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CMEE=1\r\n"); //错误码
    judgeok(NB, 1);
    SendAtCommand(NB, "ATI\r\n");
    judgeok(NB, 2);
    SendAtCommand(NB, "AT+QCFG=\"dsevent\",0\r\n"); //关闭永无休止的SLEEP上报
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+QCFG=\"autoapn\",0\r\n"); //配置 APN 自动激活选项关闭
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CEREG=1\r\n"); //开启网络注册状态自动回复
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CSCON=1\r\n"); //开启连接状态自动回复
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CPSMS=1,,,\"01000011\",\"01000011\"\r\n"); //开PSM
    judgeok(NB, 1);
    SendAtCommand(NB, "AT+CEDRXS=0\r\n"); //关eDRX
    judgeok(NB, 1);

    SendAtCommand(NB, "AT+QNBIOTEVENT=1,1\r\n"); //开启进入PSM状态上报
    judgeok(NB, 1);
    if (NB->NB_Cfg.NBBAND == USE_Band_All)
    {
        SendAtCommand(NB, "AT+QBAND=3,5,8,3\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_5) //电信
    {
        SendAtCommand(NB, "AT+QBAND=1,5\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_8) //移动联通
    {
        SendAtCommand(NB, "AT+QBAND=1,8\r\n");
    }
    else if (NB->NB_Cfg.NBBAND == USE_Band_3) //联通1800
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

//返回0连接成功
//返回1有信号但是没附着成功
//返回2无信号
//返回3模块有问题
//返回4没有卡
//返回5未知
//返回55连接被中止
//返回AA正在连接过程中
__weak int GY_NB_Connect(NB_Type *NB)
{
    switch (NB->NB_Count.NBCurrentStep)
    {
#if Use_BC260 > 0
    case NBStep_NRB:
    {
        NB->NB_Stat.rdy = 0;
        SendAtCommand(NB, "AT+QRST=1\r\n");
        judgeRDY(NB, 5); //等待收到复位标志
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
        SendAtCommand(NB, "AT+CPSMS=1,,,\"01000011\",\"01000011\"\r\n"); //开PSM
        ReOperation(NB, 0, 2);
        SendAtCommand(NB, "AT+QNBIOTEVENT=1,1\r\n"); //开启进入PSM状态上报
        ReOperation(NB, 0, 2);
        NB->NB_Count.NBCurrentStep = NBStep_CGSN;
    }
    break;
    case NBStep_CGSN:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+CGSN=1\r\n"); //查模组号
    }
    break;
    case NBStep_NCCID:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+QCCID\r\n"); //查sim卡号
    }
    break;
    case NBStep_CIMI:
    {
        SendAtCommand(NB, "AT+CIMI\r\n"); //查cimi号
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
            SendAtCommand(NB, "AT+QLACFG=\"platform\",0\r\n"); //配置目标平台为电信物联网开放平台
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
            SendAtCommand(NB, "AT+QLAADDOBJ=19,0,1,0\r\n"); //添加对象 19/0/0
    }
    break;
    case NBStep_Coap_4:
    {
        if (ReOperation(NB, 1, 5) == 0) //只允许发送一次，失败就重来
            SendAtCommand(NB, "AT+QLAREG\r\n");
    }
    break;
    case NBStep_Coap_5:
    {
        //+QLAURC: "observe",50408,0,19,0,0
        if (ReOperation(NB, 1, 5) == 0) //只允许发送一次，失败就重来
            oberveSend(NB);             //订阅应答
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
        if (ReOperation(NB, 1, 5) == 0) //只允许发送一次，失败就重来
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
        if (ReOperation(NB, 1, 5) == 0)             //只允许发送一次，失败就重来
            SendAtCommand(NB, "AT+MIPLCREATE\r\n"); //创建一个 LwM2M 客户端实例
    }
    break;
    case NBStep_OneNet_2:
    {
        if (ReOperation(NB, 1, 5) == 0)                                //只允许发送一次，失败就重来
            SendAtCommand(NB, "AT+MIPLADDOBJ=0,3200,1,\"1\",1,0\r\n"); //添加 Object, Instance 及 Resource
    }
    break;
    case NBStep_OneNet_3:
    {
        if (ReOperation(NB, 2, 30) == 0) //需要多等待一段时间
        {
            SendAtCommand(NB, "AT+MIPLOPEN=0,3600\r\n"); //客户端注册 OneNET 平台
            NB->NB_Count.LinkTime = 0;
        }
    }
    break;
    case NBStep_OneNet_4:
    {
        if (ReOperation(NB, 1, 5) == 0) //只允许发送一次，失败就重来
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
        if (NB->NB_Stat.CGATT == 1) //在这里异频分支判断
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
        SendAtCommand(NB, "AT+CGSN=1\r\n"); //查模组号
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_NCCID;
    }
    break;
    case NBStep_NCCID:
    {
        SendAtCommand(NB, "AT+NCCID\r\n"); //查sim卡号
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_CGDCONT;
    }
    break;
    case NBStep_CGDCONT:
    {
        SendAPNCommand(NB, NB->NB_Cfg.NB_IP.NBAPN); //APN设置成不成无所谓
        ReOperation(NB, 0, 1);
        NB->NB_Count.NBCurrentStep = NBStep_CPSMS;
    }
    break;
    case NBStep_CPSMS:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+CPSMS=1,,,01000011,00000011\r\n"); //开PSM
    }
    break;
    case NBStep_NPSMR:
    {
        if (ReOperation(NB, 3, 2) == 0)
            SendAtCommand(NB, "AT+NPSMR=1\r\n"); //进入PSM自动提醒
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
        SendAtCommand(NB, "AT+CGPADDR\r\n"); //PDP地址
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
            //如果socket号读错就关了重来
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
        //该条命令一旦失败立即重启
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
        if (ReOperation(NB, 1, 30) == 0) //等待30s注册到OneNet
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
            //只要附着上网络就可以传数据
            //这个标志意味着可以发送数据了
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
                //有信号但是5分钟未附着成功，需要异频处理
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
                //没信号还没成功附着网络直接关
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
        //结束连接
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
        /*未成功进入psm*/
        SendAtCommand(NB, "AT+CSCON=0\r\n");
        judgeok(NB, 1);
        SendAtCommand(NB, "AT+CFUN=0\r\n");
        judgeok(NB, 3);
    }
#endif

#if Use_BC260 > 0
    if ((NB->NB_Cfg.NB_IP.LinkMode == UDPNum) || (NB->NB_Cfg.NB_IP.LinkMode == TCPNum))
    {
        SendAtCommand(NB, "AT+QICLOSE=0\r\n"); //关闭连接
        judgeok(NB, 1);
    }
    if (NB->NB_Cfg.NB_IP.LinkMode == OneNET)
    {
        SendAtCommand(NB, "AT+MIPLCLOSE=0\r\n"); //关闭连接
        judgeok(NB, 1);
    }
    SendAtCommand(NB, "AT+QSCLK=1\r\n"); //释放休眠锁
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
            if (++NB->NB_Count.LinkMaxTime >= vLinkMaxTime) //NB模块上传过程最大允许时间
            {
                NB->NB_Stat.Link_Enable = 0;
            }
            if (NB->NB_Stat.CGATT == 1) //在线时长，实际正常20s掉线后自动关机，这个条件一般没什么用
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
            /*退出了psm*/
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
	务必将NBIP转化为15位带'.'格式IP
	务必将NBPORT转为5位长度Port
	需要初始化的数据
	NB->NB_Cfg.rxbuff=NBRxBuf;
	NB->NB_Cfg.txbuff=NBTxBuf;
	NB->NB_Cfg.NBBAND=0;
	NB->NB_Cfg.NBBaud=9600;
	memcpy(NB->NB_Cfg.NB_IP.ServerIP,IP1,15);
	memcpy(NB->NB_Cfg.NB_IP.PORT,PORT1,5);
*/
__weak int GY_NB_Link(NB_Type *NB)
{
    IPReduceZero(NB->NB_Cfg.NB_IP.ServerIP);   //转成NB能使用的IP
    PortReduceZero_New(NB->NB_Cfg.NB_IP.PORT); //转成NB能使用的PORT
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
*	函 数 名: RXdecoding
*	功能说明: 将ascii转hex  能够识别大小写
*	形    参: code   ：输入的要转换的数据
*             decode ：输出转换的数据
*             len ：需要转的长度
*	返 回 值: 无
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

/*关键字不要超过5字节*/
__weak uint16_t GY_NB_RxData(NB_Type *NB, uint8_t *buf, const char *word)
{
    char *Location = NULL;
    uint8_t ta = 0, tb = 0;
    uint16_t rxlen = 0;
    char nword[10] = {0};
    uint16_t len = mystrlen(word, 5);

    /*实现将关键字大小写转化*/
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
            ta = tb + 2; //+2是因为 rxlen之后还有 ," 两个字符
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
        ta = len + tb + 2; //+2是因为 rxlen之后还有 ," 两个字符
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
        // 解析待完成
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
            ta += 1; //+1是因为 rxlen之后还有 , 1个字符
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
    va_list ap;           //定义一个int *型的指针ap
    va_start(ap, format); //初始化ap，让它指向第一个可变参数的地址

    while ((c = *format++) != '\0') //开始解析printf函数中的字符串(即第一个固定参数)
    {
        switch (c) //在while中，依次读出字符串中的字符，在这里依依地进行判断和解析
        {
        case '%':          //如果字符是%，即格式申明符号，则解析该格式
            c = *format++; //让c等于%后的第一个字母
            switch (c)     //对上述字母进行解析
            {
            case '0': //用于长度限制
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
            case 'c':                          //如果是%c，即输出字母
                c = va_arg(ap, int);           //获取当前可变参数的值，并指向下一个可变参数
                NB_Send(NB, (uint8_t *)&c, 1); //调用底层(实际操作硬件的那层)来完成输出结果
                break;
            case 'l':                  //如果是%l，即赋值长度
                len = va_arg(ap, int); //获取当前可变参数的值，并指向下一个可变参数
                break;
            case 's': //如果是%s，即输出字符串
                p = va_arg(ap, char *);
                SendAtCommand(NB, p);
                break;
            case 'a': //如果是%a，即数据转成ascii后输出  用a命令前必须设置%l的长度
                p = va_arg(ap, char *);
                for (a = 0; a < len; a++)
                {
                    HexToAscll(buf, (uint8_t *)p, 1);
                    NB_Send(NB, (uint8_t *)buf, 2);
                    p++;
                }
                len = 0;
                break;
            case 'd': //如果是%d，即以十进制输出
                a = va_arg(ap, int);
                sprintf(buf, "%d", a);
                SendAtCommand(NB, buf);
                break;
            default:
                break;
            }
            break;
        default:
            NB_Send(NB, (uint8_t *)&c, 1); //输出字符串中为普通字符的字符
            break;
        }
    }
    return 0;
}

#endif
