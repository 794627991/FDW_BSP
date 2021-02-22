#include "iot_n23.h"

/*全局变量声明*/
/*n23_dat_type IOTDAT(MODULE);*/
IOTDATTYPE(MODULE)
IOTDAT(MODULE);
/*静态变量声明*/
static uint8_t *membuf;
static uint16_t memlen;
/*静态函数声明*/
static uint8_t n23iotRst(iot_type *iot);
static uint8_t n23rxdeal(iot_type *iot);
static uint8_t n23udptcpsend2(iot_type *iot);
static uint8_t n23SendUdpTcpIpPort(iot_type *iot);

const cmd_type DIC_TCUDP(MODULE)[] = {
    {"rst", NULL, n23iotRst, NULL, NULL, 0, 1, "nrb", "end1", "end1"},
    {"nrb", "AT\r\n", NULL, "PBREADY", NULL, 3, 5, "ate", "rst", "rst"}, //如果收不到上电应答直接重启
    {"ate", "ATE0\r\n", NULL, "OK", NULL, 2, 3, "at", "at", "at"},
    {"at", "AT\r\n", NULL, "OK", NULL, 2, 3, "cfun", "end2", "end2"},
    {"cfun", "AT+CFUN=1\r\n", NULL, "OK", NULL, 10, 3, "creg", "end2", "end"}, //打不开全功能，没卡可以直接关了
    {"creg", "AT+CEREG=1\r\n", NULL, "OK", NULL, 2, 3, "cfg", "cfg", "cfg"},
    /*{"cfunx", "AT+CFUN?\r\n", NULL, NULL, n23rxdeal, 3, 5, "cfg", "end", "end"}, */
    {"cfg", "AT+NEONBIOTCFG=1,0,1,0\r\n", NULL, "OK", NULL, 2, 1, "cpsm", "cpsm", "cpsm"},
    {"cpsm", "AT+CPSMS=1\r\n", NULL, "OK", NULL, 2, 1, "lpow", "lpow", "lpow"},
    {"lpow", "AT+NVSETPM=2\r\n", NULL, "OK", NULL, 2, 1, "cgsn", "cgsn", "cgsn"}, //开超低功耗功能
    {"cgsn", "AT+CGSN\r\n", NULL, NULL, n23rxdeal, 2, 3, "nccid", "nccid", "nccid"},
    {"nccid", "AT+CCID\r\n", NULL, NULL, n23rxdeal, 2, 3, "cimi", "cimi", "cimi"},
    {"cimi", "AT+CIMI\r\n", NULL, "OK", NULL, 2, 3, "cgatt", "cgatt", "cgatt"},
    {"cgatt", "AT+CGATT?\r\n", NULL, NULL, n23rxdeal, 2, 60, "csq", "end", "end"}, //暂未发现清频点命令,若有以后再次添加
    /* {"isyp", "AT+CSQ\r\n", NULL, NULL, n23rxdeal, 2, 3, "yp1", "end", "end"},
       {"yp1", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 2, "yp2", "end", "end"},
       {"yp2", "AT+QCSEARFCN\r\n", NULL, "OK", NULL, 2, 2, "rst", "end", "end"},*/
    {"csq", "AT+CSQ\r\n", NULL, NULL, n23rxdeal, 1, 1, "cscon", "cscon", "cscon"},
    {"cscon", "AT+NEONBIOTCFG=1,0,1,1\r\n", NULL, "OK", NULL, 2, 1, "clk", "clk", "clk"}, //开启自动在线状态RCC上报
    {"clk", "AT+CCLK?\r\n", NULL, NULL, n23rxdeal, 1, 1, "nustat", "nustat", "nustat"},
    {"nustat", "AT+TUESTATS\r\n", NULL, NULL, n23rxdeal, 2, 1, "tcp1", "tcp1", "tcp1"},
    {"tcp1", "AT+TCPCLOSE=0\r\n", NULL, "TCPCLOSE", NULL, 2, 1, "tcp2", "tcp2", "tcp2"}, //成不成无所谓
    {"tcp2", NULL, n23SendUdpTcpIpPort, "TCPSETUP: 0,OK", NULL, 5, 1, "txdat1", "rst", "rst"},
    {"txdat1", NULL, iotSend, ">", NULL, 2, 1, "txdat2", "end", "txdat1"},
    {"txdat2", NULL, n23udptcpsend2, "TCPSEND: 0,", NULL, 10, 1, "wait", "txdat1", "txdat1"},
    {"wait", NULL, NULL, NULL, n23rxdeal, 60, 60, "txdat1", "end", "end"},
    /*{"rxdat", NULL, iotRead, NULL, NULL, 0, 1, "wait", "end", "wait"},*/
    {"end", "AT+RRCRLSREQ\r\n", NULL, "OK", NULL, 2, 1, "end1", "end1", "end1"},
    {"end1", "AT+CFUN=0\r\n", NULL, "OK", NULL, 5, 1, "end2", "end2", "end2"},
    {"end2", NULL, iotEnd, NULL, NULL, 0, 1, NULL, NULL, NULL}};

const cmd_type DIC_COAP(MODULE)[] = {NULL};

const cmd_type DIC_ONET(MODULE)[] = {NULL};

/*n23复位比较特殊,因为没找到软复位的命令*/
static uint8_t n23iotRst(iot_type *iot)
{
    NB_OFF;
    delay_ms(100);
    NB_ON;
    return iotRst(iot);
}

/*返回1 收到成功的数据
  返回0 收到失败的数据
  返回2 未收到有效数据*/
static uint8_t n23rxdeal(iot_type *iot)
{
    char *buf = iot->core.rxbuf;
    volatile uint8_t re = 2;

    if (strstr(buf, "TCPRECV:"))
    {
        //+TCPRECV: 0,4,FF13
        if (iotRead(iot) == 0)
        {
            /*说明收到结束包*/
            re = 0;
        }
    }
    else if (strstr(buf, "PBREADY"))
    {
        re = 1;
    }
    else if (strstr(buf, "CFUN: 1"))
    {
        re = 1;
    }
    else if (strstr(buf, "CPIN: NO SIM"))
    {
        re = 0;
    }
    else if (strstr(buf, "CGSN:"))
    {
        //+CGSN: 865602040365279
        char *Location;
        int len = strlen("CGSN: "), c = 0;
        Location = strstr(buf, "CGSN: ");
        memcpy(IOTDAT(MODULE).CGSNNum, &Location[len], 15);
        for (c = 0; c < 15; c++)
        {
            if ((IOTDAT(MODULE).CGSNNum[c] <= 0x30) && (IOTDAT(MODULE).CGSNNum[c] >= 0x39))
            {
                break;
            }
        }
        if (c == 15)
        {
            re = 1;
        }
        else
        {
            re = 0;
        }
    }
    else if (strstr(buf, "CCID:"))
    {
        //+CCID: 898602B5061730220087
        char *Location;
        int len = strlen("CCID: "), c = 0;
        Location = strstr(buf, "CCID: ");
        for (c = 0; c < 20; c++)
        {
            if ((Location[c] <= 0x30) && (Location[c] >= 0x39))
            {
                break;
            }
        }
        if (c == 20)
        {
            CharToBety((uint8_t *)&Location[len], IOTDAT(MODULE).Simword, 10);
            re = 1;
        }
    }
    else if (strstr(buf, "CEREG: "))
    {
        char *Location;
        int len = strlen("CEREG: ");
        Location = strstr(buf, "CEREG: ");
        if ((Location[len] == '0') || (Location[len] == '3'))
        {
            re = 0; //未注册，直接关
        }
        if ((Location[len] == '1') || (Location[len] == '5'))
        {
            re = 1;
        }
    }
    else if (strstr(buf, "CGATT: 1")) //+CGATT: 1
    {
        re = 1;
    }
    else if (strstr(buf, "Signal"))
    {
        /*Signal power,10
        Total power,66
        Tx power,23
        TX time,17
        RX time,21
        Cell ID,54916679
        ECL,0
        SNR,10
        EARFCN,3688
        PCI,54
        RSRQ,29
        OK*/
        char *Location, i;
        uint8_t len = 0;
        IOTDAT(MODULE).rsrp = 0;
        IOTDAT(MODULE).cellid = 0;
        IOTDAT(MODULE).EARFCN = 0;
        IOTDAT(MODULE).PCI = 0;
        IOTDAT(MODULE).ECL = 0;
        IOTDAT(MODULE).SNR = 0;
        Location = strstr(buf, "Signal power");
        len = strlen("Signal power");
        if (Location[len + 1] == '-') //把逗号让出1个 所以+1
        {
            for (i = 0; i < 4; i++)
            {
                if (Location[i + len + 2] == 0x0D) //吧负号让出1个  所以在+1
                    break;

                IOTDAT(MODULE).rsrp = IOTDAT(MODULE).rsrp * 10 - (Location[i + len + 2] - 0x30);
            }
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                if (Location[i + len + 1] == 0x0D)
                    break;
                IOTDAT(MODULE).rsrp = IOTDAT(MODULE).rsrp * 10 + (Location[i + len + 1] - 0x30);
            }
        }

        Location = strstr(buf, "ECL");
        len = strlen("ECL");
        for (i = 0; i < 2; i++)
        {
            if (Location[i + len + 1] == 0x0D)
                break;
            IOTDAT(MODULE).ECL = IOTDAT(MODULE).ECL * 10 + (Location[i + len + 1] - 0x30);
        }

        Location = strstr(buf, "SNR");
        len = strlen("SNR");
        if (Location[len + 1] == '-')
        {
            for (i = 0; i < 4; i++)
            {
                if (Location[i + len + 2] == 0x0D)
                    break;
                IOTDAT(MODULE).SNR = IOTDAT(MODULE).SNR * 10 - (Location[i + len + 2] - 0x30);
            }
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                if (Location[i + len + 1] == 0x0D)
                    break;
                IOTDAT(MODULE).SNR = IOTDAT(MODULE).SNR * 10 + (Location[i + len + 1] - 0x30);
            }
        }

        Location = strstr(buf, "PCI");
        len = strlen("PCI");
        for (i = 0; i < 6; i++)
        {
            if (Location[i + len + 1] == 0x0D)
                break;
            IOTDAT(MODULE).PCI = IOTDAT(MODULE).PCI * 10 + (Location[i + len + 1] - 0x30);
        }

        Location = strstr(buf, "EARFCN");
        len = strlen("EARFCN");
        for (i = 0; i < 6; i++)
        {
            if (Location[i + len + 1] == 0x0D)
                break;
            IOTDAT(MODULE).EARFCN = IOTDAT(MODULE).EARFCN * 10 + (Location[i + len + 1] - 0x30);
        }

        Location = strstr(buf, "Cell ID");
        len = strlen("Cell ID");
        for (i = 0; i < 10; i++)
        {
            if (Location[i + len + 1] == 0x0D)
                break;
            IOTDAT(MODULE).cellid = IOTDAT(MODULE).cellid * 10 + (Location[i + len + 1] - 0x30);
        }
        re = 1;
    }
    else if (strstr(buf, "CSQ: ")) //+CSQ: 31,0
    {
        char *Location;
        int len = strlen("CSQ: ");
        Location = strstr(buf, "CSQ: ");
        if (Location[len + 1] == ',')
            IOTDAT(MODULE).RSSI = Location[len] - 0x30;
        else
            IOTDAT(MODULE).RSSI = ((Location[len] - 0x30) << 4) + (Location[len + 1] - 0x30);
        if ((IOTDAT(MODULE).RSSI > 0) && (IOTDAT(MODULE).RSSI != 0x99))
            re = 1;
        else
            re = 0;
    }
    else if (strstr(buf, "CSCON: 0"))
    {
        re = 1;
    }
    else if (strstr(buf, "ENTER PSM")) //+PSMEVENT: ENTER PSM
    {
        re = 0;
    }
    else if (strstr(buf, "CCLK: "))
    {
#if NB_CLK > 0 && API_UseRTC > 0
        //+CCLK: "20/11/27,03:37:40+32"
        Clk_Type CLK;
        char *Location;
        int len = strlen("CCLK: \"");
        Location = strstr(buf, "CCLK: \"");

        CLK.g_cur_time.second = (Location[len + 15] - 0x30) * 10 + (Location[len + 16] - 0x30);
        CLK.g_cur_time.minute = (Location[len + 12] - 0x30) * 10 + (Location[len + 13] - 0x30);
        CLK.g_cur_time.hour = (Location[len + 9] - 0x30) * 10 + (Location[len + 10] - 0x30) + (((Location[len + 18] - 0x30) * 10 + (Location[len + 19] - 0x30)) / 4);
        CLK.g_cur_date.day = (Location[len + 6] - 0x30) * 10 + (Location[len + 7] - 0x30);
        CLK.g_cur_date.month = (MONTH)((Location[len + 3] - 0x30) * 10 + (Location[len + 4] - 0x30));
        CLK.g_cur_date.year = (Location[len] - 0x30) * 10 + (Location[len + 1] - 0x30) + 2000;

        if (CLK.g_cur_time.hour >= 24)
        {
            CLK.g_cur_time.hour = CLK.g_cur_time.hour - 24;
            switch (CLK.g_cur_date.month)
            {
            case Feb: //平月
            {
                if (((CLK.g_cur_date.year % 100 != 0) && (CLK.g_cur_date.year % 4 == 0)) || (CLK.g_cur_date.year % 400 == 0)) //闰年
                {
                    if (++CLK.g_cur_date.day > 29)
                    {
                        CLK.g_cur_date.day = 1;
                        ++CLK.g_cur_date.month; //月计数
                    }
                }
                else if (++CLK.g_cur_date.day > 28)
                {
                    CLK.g_cur_date.day = 1;
                    ++CLK.g_cur_date.month; //月计数
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
                if (++CLK.g_cur_date.day > 31)
                {
                    CLK.g_cur_date.day = 1;
                    if (++CLK.g_cur_date.month > Dec) //月计数
                    {
                        CLK.g_cur_date.month = Jan;
                        CLK.g_cur_date.year++; //年计数
                    }
                }
            }
            break;
            case Apr:
            case Jun:
            case Sep:
            case Nov: //小月
            {
                if (++CLK.g_cur_date.day > 30)
                {
                    CLK.g_cur_date.day = 1;
                    ++CLK.g_cur_date.month; //月计数
                }
            }
            break;
            }
        }

        if ((Check_date(CLK.g_cur_date.year, CLK.g_cur_date.month, CLK.g_cur_date.day)) &&
            (CLK.g_cur_time.hour < 24) && (CLK.g_cur_time.minute < 60) && (CLK.g_cur_time.second < 60))
        {
            RTC_TimeDateTypeDef TempTime;
            TempTime.Year = HEXtoBCD(CLK.g_cur_date.year - 2000);
            TempTime.Month = HEXtoBCD(CLK.g_cur_date.month);
            TempTime.Date = HEXtoBCD(CLK.g_cur_date.day);
            TempTime.Hour = HEXtoBCD(CLK.g_cur_time.hour);
            TempTime.Minute = HEXtoBCD(CLK.g_cur_time.minute);
            TempTime.Second = HEXtoBCD(CLK.g_cur_time.second);
            API_SetTIME(&TempTime); //设置时间
        }

#endif
        re = 1;
    }
    return re;
}

static uint8_t n23SendUdpTcpIpPort(iot_type *iot)
{
    if (iot->foo.LinkMode != TCPNum)
    {
        //AT+UDPSETUP=0,60.21.136.130,3380
        iotprintf("%s%15s%s%5s%s", "AT+UDPSETUP=0,", iot->net.Ip, ",", iot->net.Port, "\r\n");
    }
    else
    {
        //AT+TCPSETUP=0,60.21.136.130,3380
        iotprintf("%s%15s%s%5s%s", "AT+TCPSETUP=0,", iot->net.Ip, ",", iot->net.Port, "\r\n");
    }
    return 2; //发命令函数除非有额外需求否则一律返回2
}

static uint8_t n23udptcpsend2(iot_type *iot)
{
    iotprintf("%l%a%s", memlen, membuf, "\r\n");
    return 2;
}

static uint8_t FAILDO(MODULE)(char *name)
{
    if (name == NULL)
        return 0;
    if (strcmp(name, "at") == 0)
    {
        /*at都不好使*/
        return 1;
    }
    if (strcmp(name, "cfun") == 0)
    {
        /*没卡*/
        return 2;
    }
    if (strcmp(name, "isyp") == 0)
    {
        /*没信号*/
        return 3;
    }
    if (strcmp(name, "tcp2") == 0)
    {
        /* tcp没开 */
        return 4;
    }
    if (strcmp(name, "txdat2") == 0)
    {
        /*发3次都没成功,超时了*/
        return 5;
    }
    if (strcmp(name, "wait") == 0)
    {
        /*如果是接收来的实际是主动停止的*/
        return 6;
    }
    return 0;
}
static iot_buf_type RXANALY(MODULE)(iot_type *iot)
{
    uint8_t ta = 0;
    iot_buf_type analy = {0, NULL};
    char *buf = iot->core.rxbuf;
    //+TCPRECV: 0,4,FF13
    if (strstr(buf, "TCPRECV: 0,"))
    {
        uint16_t len = strlen("TCPRECV: 0,");
        analy.loc = strstr(buf, "TCPRECV: 0,");
        for (ta = 0; ta < 4; ta++)
        {
            if (analy.loc[len + ta] == ',')
            {
                break;
            }
            else
            {
                analy.len *= 10;
                analy.len += (analy.loc[len + ta] - 0x30);
            }
        }

        ta += (len + 1); //+2是因为 rxlen之后还有 , 1个字符
        analy.loc = &analy.loc[ta];
    }
    return analy;
}

static void UDP_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
    //AT+TCPSEND=0,3
    membuf = buf;
    memlen = len;
    iotprintf("%s%d%s", "AT+TCPSEND=0,", len * 2, "\r\n");
}

static void TCP_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
    UDP_SEND(MODULE)
    (buf, len);
}

static void COAP_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
}

static void ONET_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
}

getcmdinfo_type GETCMD(MODULE)(uint8_t linkmode)
{
    getcmdinfo_type info;
    if (linkmode == CoapNum)
    {
        info.cmd = (cmd_type *)&DIC_COAP(MODULE);
        info.len = ARRAY_SIZE(DIC_COAP(MODULE));
        info.SendFun = COAP_SEND(MODULE);
    }
    else if (linkmode == OneNET)
    {
        info.cmd = (cmd_type *)&DIC_ONET(MODULE);
        info.len = ARRAY_SIZE(DIC_ONET(MODULE));
        info.SendFun = ONET_SEND(MODULE);
    }
    else if (linkmode == TCPNum)
    {
        info.cmd = (cmd_type *)&DIC_TCUDP(MODULE);
        info.len = ARRAY_SIZE(DIC_TCUDP(MODULE));
        info.SendFun = TCP_SEND(MODULE);
    }
    else
    {
        info.cmd = (cmd_type *)&DIC_TCUDP(MODULE);
        info.len = ARRAY_SIZE(DIC_TCUDP(MODULE));
        info.SendFun = UDP_SEND(MODULE);
    }
    info.RxFun = RXANALY(MODULE);
    info.Faildo = FAILDO(MODULE);
    return info;
}
