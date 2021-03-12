#include "iot_bc28_35_95.h"

/*全局变量声明*/
IOTDATTYPE(MODULE)
IOTDAT(MODULE);
static uint8_t bc28rxdeal(iot_type *iot);
static uint8_t bc28SendApn(iot_type *iot);
static uint8_t bc28OpenSocket(iot_type *iot);
static uint8_t bc28CloseSocket(iot_type *iot);
static uint8_t bc28SendCoapIpPort(iot_type *iot);
static uint8_t bc28SendUdpTcpIpPort(iot_type *iot);
/*字典声明*/
const cmd_type DIC_INT(MODULE)[] = {
    {"nrb", "AT+NRB\r\n", NULL, "Neul", NULL, 15, 1, "sped", "sped", "sped"},
    {"sped", "AT+NATSPEED=9600,30,1,0,2\r\n", NULL, "OK", NULL, 2, 1, "at", "at", "at"}, //设置波特率:9600两位停止位
    {"at", "AT\r\n", NULL, "OK", NULL, 2, 3, "ati", "end2", "end2"},                     //at不好使就是模块有问题
    {"ati", "ATI\r\n", NULL, "OK", NULL, 2, 1, "cgmr", "cgmr", "cgmr"},
    {"cgmr", "AT+CGMR\r\n", NULL, "OK", NULL, 2, 1, "cpsm", "cpsm", "cpsm"},
    {"cpsm", "AT+CPSMS=1\r\n", NULL, "OK", NULL, 2, 1, "cfun0", "cfun0", "cfun0"},
    {"cfun0", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 1, "ncnf", "end2", "end2"},
    {"ncnf", "AT+NCONFIG=CELL_RESELECTION,TRUE\r\n", NULL, "OK", NULL, 2, 1, "band", "band", "band"},
#if UseNBBand == USE_Band_3
    {"band", "AT+NBAND=3\r\n", NULL, "OK", NULL, 2, 1, "end2", "end2", "end2"}, //AT+NBAND=3,5,8\r\n  设置波特率
#endif
#if UseNBBand == USE_Band_5
    {"band", "AT+NBAND=5\r\n", NULL, "OK", NULL, 2, 1, "end2", "end2", "end2"}, //AT+NBAND=3,5,8\r\n  设置波特率
#endif
#if UseNBBand == USE_Band_8
    {"band", "AT+NBAND=8\r\n", NULL, "OK", NULL, 2, 1, "end2", "end2", "end2"}, //AT+NBAND=3,5,8\r\n  设置波特率
#endif
#if UseNBBand == USE_Band_All
    {"band", "AT+NBAND=3,5,8\r\n", NULL, "OK", NULL, 2, 1, "end2", "end2", "end2"}, //AT+NBAND=3,5,8\r\n  设置波特率
#endif
    {"end2", NULL, iotEnd, NULL, NULL, 0, 1, NULL, NULL, NULL}};

const cmd_type DIC_TCUDP(MODULE)[] = {
    {"rst", NULL, iotRst, NULL, NULL, 0, 1, "nrb", "end1", "end1"},
    {"nrb", "AT+NRB\r\n", NULL, "Neul", NULL, 15, 1, "at", "at", "at"},
    {"at", "AT\r\n", NULL, "OK", NULL, 2, 3, "cfun", "end2", "end2"},           //at不好使就是模块有问题
    {"cfun", "AT+CFUN=1\r\n", NULL, "OK", NULL, 10, 2, "creg", "end2", "end2"}, //2次都失败意味着没卡
    {"creg", "AT+CEREG=1\r\n", NULL, "OK", NULL, 2, 3, "cfunx", "cfunx", "cfunx"},
    {"cfunx", "AT+CFUN?\r\n", NULL, "CFUN:1", NULL, 3, 3, "cmee", "end2", "end2"},
    {"cmee", "AT+CMEE=1\r\n", NULL, "OK", NULL, 2, 1, "setcg", "setcg", "setcg"},
    {"setcg", "AT+CGATT=1\r\n", NULL, "OK", NULL, 2, 1, "setapn", "setapn", "setapn"},
    {"setapn", NULL, bc28SendApn, "OK", NULL, 2, 1, "cpsm", "cpsm", "cpsm"},
    {"cpsm", "AT+CPSMS=1\r\n", NULL, "OK", NULL, 2, 1, "psmr", "psmr", "psmr"}, //AT+CPSMS=1,,,\"01000011\",\"01000011\"\r\n
    {"psmr", "AT+NPSMR=1\r\n", NULL, "OK", NULL, 2, 1, "con0", "con0", "con0"},
    {"con0", "AT+CSCON=0\r\n", NULL, "OK", NULL, 2, 1, "qreg", "qreg", "qreg"},
    {"qreg", "AT+QREGSWT=2\r\n", NULL, "OK", NULL, 2, 1, "cgsn", "cgsn", "cgsn"}, //关闭自动注册平台
    {"cgsn", "AT+CGSN=1\r\n", NULL, NULL, bc28rxdeal, 2, 3, "nccid", "nccid", "nccid"},
    {"nccid", "AT+NCCID\r\n", NULL, NULL, bc28rxdeal, 2, 3, "cimi", "cimi", "cimi"},
    {"cimi", "AT+CIMI\r\n", NULL, "OK", NULL, 2, 3, "cgatt", "cgatt", "cgatt"},
    {"cgatt", "AT+CGATT?\r\n", NULL, NULL, bc28rxdeal, 2, 60, "csq", "end1", "isyp"},
    {"isyp", "AT+CSQ\r\n", NULL, NULL, bc28rxdeal, 2, 3, "yp1", "end", "end"},
    {"yp1", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 2, "yp2", "end1", "end1"},
    {"yp2", "AT+NCSEARFCN\r\n", NULL, "OK", NULL, 2, 2, "rst", "end2", "end2"}, //清频点
    {"csq", "AT+CSQ\r\n", NULL, NULL, bc28rxdeal, 2, 1, "clk", "clk", "clk"},
    {"clk", "AT+CCLK?\r\n", NULL, NULL, bc28rxdeal, 2, 1, "nustat", "nustat", "nustat"},
    {"nustat", "AT+NUESTATS\r\n", NULL, NULL, bc28rxdeal, 2, 1, "udp1", "udp1", "udp1"},
    {"udp1", NULL, bc28OpenSocket, NULL, bc28rxdeal, 2, 3, "udp2", "rst", "rst"},  //建立socket
    {"udp2", NULL, bc28SendUdpTcpIpPort, "OK", NULL, 15, 1, "con1", "rst", "rst"}, //如果是tcp，需要等15秒，没建立成功立即关闭
    {"con1", "AT+CSCON=1\r\n", NULL, "OK", NULL, 2, 1, "txdat", "txdat", "txdat"},
    {"txdat", NULL, iotSend, "OK", NULL, 10, 3, "wait", "end", "end"},
    {"wait", NULL, NULL, NULL, bc28rxdeal, 1, 60, "txdat", "end", "end"},
    /*{"rxdat", NULL, iotRead, NULL, NULL, 0, 1, "wait", "end", "wait"},*/
    {"end", NULL, bc28CloseSocket, "OK", NULL, 1, 1, "end1", "end1", "end1"}, //"AT+NSOCL=1\r\n"
    {"end1", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 1, "end2", "end2", "end2"},
    {"end2", NULL, iotEnd, NULL, NULL, 0, 1, NULL, NULL, NULL}};

const cmd_type DIC_COAP(MODULE)[] = {
    {"rst", NULL, iotRst, NULL, NULL, 0, 1, "nrb", "end1", "end1"},
    {"nrb", "AT+NRB\r\n", NULL, "Neul", NULL, 15, 1, "at", "at", "at"},
    {"at", "AT\r\n", NULL, "OK", NULL, 2, 3, "cgmr", "end2", "end2"}, //at不好使就是模块有问题
    {"cgmr", "AT+CGMR\r\n", NULL, "OK", NULL, 2, 1, "cfun", "cfun", "cfun"},
    {"cfun", "AT+CFUN=1\r\n", NULL, "OK", NULL, 10, 2, "creg", "end2", "end2"}, //2次都失败意味着没卡
    {"creg", "AT+CEREG=1\r\n", NULL, "OK", NULL, 2, 3, "cfunx", "cfunx", "cfunx"},
    {"cfunx", "AT+CFUN?\r\n", NULL, "CFUN:1", NULL, 3, 3, "cmee", "end2", "end2"},
    {"cmee", "AT+CMEE=1\r\n", NULL, "OK", NULL, 2, 1, "setcg", "setcg", "setcg"},
    {"setcg", "AT+CGATT=1\r\n", NULL, "OK", NULL, 2, 1, "cpsm", "cpsm", "cpsm"},
    {"cpsm", "AT+CPSMS=1\r\n", NULL, "OK", NULL, 2, 1, "psmr", "psmr", "psmr"},
    {"psmr", "AT+NPSMR=1\r\n", NULL, "OK", NULL, 2, 1, "con0", "con0", "con0"},
    {"con0", "AT+CSCON=0\r\n", NULL, "OK", NULL, 2, 1, "qreg", "qreg", "qreg"},
    {"qreg", "AT+QREGSWT=0\r\n", NULL, "OK", NULL, 2, 1, "cgsn", "cgsn", "cgsn"}, //手动注册平台
    {"cgsn", "AT+CGSN=1\r\n", NULL, NULL, bc28rxdeal, 2, 3, "nccid", "nccid", "nccid"},
    {"nccid", "AT+NCCID\r\n", NULL, NULL, bc28rxdeal, 2, 3, "cimi", "cimi", "cimi"},
    {"cimi", "AT+CIMI\r\n", NULL, "OK", NULL, 2, 3, "cgatt", "cgatt", "cgatt"},
    {"cgatt", "AT+CGATT?\r\n", NULL, NULL, bc28rxdeal, 2, 60, "csq", "end1", "isyp"},
    {"isyp", "AT+CSQ\r\n", NULL, NULL, bc28rxdeal, 2, 3, "yp1", "end1", "end1"},
    {"yp1", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 2, "yp2", "end2", "end2"},
    {"yp2", "AT+NCSEARFCN\r\n", NULL, "OK", NULL, 2, 2, "rst", "end2", "end2"}, //清频点
    {"csq", "AT+CSQ\r\n", NULL, NULL, bc28rxdeal, 2, 1, "clk", "clk", "clk"},
    {"clk", "AT+CCLK?\r\n", NULL, NULL, bc28rxdeal, 2, 1, "nustat", "nustat", "nustat"},
    {"nustat", "AT+NUESTATS\r\n", NULL, NULL, bc28rxdeal, 2, 1, "coap1", "coap1", "coap1"},
    {"coap1", NULL, bc28SendCoapIpPort, "OK", NULL, 2, 2, "coap2", "coap2", "coap2"},
    {"coap2", "AT+QLWSREGIND=0\r\n", NULL, "QLWEVTIND:3", NULL, 15, 1, "con1", "rst", "rst"}, //等15秒如果还不出直接复位
    {"con1", "AT+CSCON=1\r\n", NULL, "OK", NULL, 2, 1, "txdat", "txdat", "txdat"},
    {"txdat", NULL, iotSend, "OK", NULL, 10, 3, "wait", "end", "end"},
    {"wait", NULL, NULL, NULL, bc28rxdeal, 1, 60, "txdat", "end", "end"},
    /*{"rxdat", NULL, iotRead, NULL, NULL, 0, 1, "wait", "end", "wait"},*/
    {"end", "AT+QLWSREGIND=1\r\n", NULL, "QLWEVTIND:1", NULL, 5, 1, "end1", "end1", "end1"}, //从平台注销
    {"end1", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 1, "end2", "end2", "end2"},
    {"end2", NULL, iotEnd, NULL, NULL, 0, 1, NULL, NULL, NULL}};

const cmd_type DIC_ONET(MODULE)[] = {NULL}; //待写

static uint8_t bc28rxdeal(iot_type *iot)
{
    /*返回1 收到成功的数据
      返回0 收到失败的数据
      返回2 未收到有效数据*/
    char *buf = iot->core.rxbuf;
    volatile uint8_t re = 2;

    if (IOTDAT(MODULE).readDGRAM == 1)
    {
        uint8_t i = 0;
        for (i = 0; i < 5; i++)
        {
            if ((buf[i] >= '0') && (buf[i] <= '9'))
            {
                IOTDAT(MODULE).DGRAM = buf[i];
                re = 1;
                break;
            }
        }
        IOTDAT(MODULE).readDGRAM = 0;
    }

    if (IOTDAT(MODULE).readData == 1)
    {
        IOTDAT(MODULE).readData = 0;
        if (iotRead(iot) == 0)
        {
            /*说明收到结束包*/
            re = 0;
        }
    }

    if (strstr(buf, "NSONMI:"))
    {
        /*+NSONMI:1,2*/
        IOTDAT(MODULE).readData = 1;
        iotprintf("%s%c%s", "AT+NSORF=", IOTDAT(MODULE).DGRAM, ",512\r\n");
    }
    else if (strstr(buf, "NNMI:"))
    {
        //NNMI:
        if (iotRead(iot) == 0)
        {
            /*说明收到结束包*/
            re = 0;
        }
    }
    else if (strstr(buf, "SECURITY_A,"))
    {
        /*SSB,V150R100C10B200SP1
		SECURITY_A,V150R100C20B300SP7
		PROTOCOL_A,V150R100C20B300SP7
		APPLICATION_A,V150R100C20B300SP7
		SECURITY_B,V150R100C20B300SP7
		RADIO,Hi2115_RF13*/
        char *Location = strstr(buf, "SECURITY_A,");
        uint8_t len = strlen("SECURITY_A,");
        Location = (char *)&Location[len];
        for (len = 0; len < 20; len++)
        {
            if (Location[len] == 0x0D)
                break;
        }
        memcpy(IOTDAT(MODULE).Ver, Location, len);
    }
    else if (strstr(buf, "CGSN:")) //+CGSN:869976030074034
    {
        char *Location = strstr(buf, "CGSN:");
        int len = strlen("CGSN:"), c = 0;
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
    }
    else if (strstr(buf, "NCCID:")) //+NCCID:89860317482032905738
    {
        char *Location;
        int len = strlen("NCCID:"), c = 0;
        Location = strstr(buf, "NCCID:");
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
    else if (strstr(buf, "CGATT:1")) //+CGATT:1
    {
        re = 1;
    }
    else if (strstr(buf, "CEREG:"))
    {
        char *Location;
        int len = strlen("CEREG:");
        Location = strstr(buf, "CEREG:");
        if ((Location[len] == '0') || (Location[len] == '3'))
        {
            re = 0; //未注册，直接关
        }
        if ((Location[len] == '1') || (Location[len] == '5'))
        {
            re = 1;
        }
    }
    else if (strstr(buf, "CSQ:")) //+CSQ:25,99
    {
        char *Location;
        int len = strlen("CSQ:");
        Location = strstr(buf, "CSQ:");
        if (Location[len + 1] == ',')
            IOTDAT(MODULE).RSSI = Location[len] - 0x30;
        else
            IOTDAT(MODULE).RSSI = ((Location[len] - 0x30) << 4) + (Location[len + 1] - 0x30);
        if ((IOTDAT(MODULE).RSSI >= 0x20) && (IOTDAT(MODULE).RSSI != 0x99))
            re = 1; /* 信号大于20才异频 */
        else
            re = 0;
    }
    else if (strstr(buf, "Signal"))
    {
        /*Signal power:-666
        Total power:-570
        TX power:-90
        TX time:563
        RX time:3228
        Cell ID:200636114
        ECL:0
        SNR:154
        EARFCN:2506
        PCI:174
        RSRQ:-108
        OPERATOR MODE:4
        CURRENT BAND:5*/
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
        if (Location[len + 1] == '-') //把:让出1个 所以+1
        {
            for (i = 0; i < 4; i++)
            {
                if (Location[i + len + 2] == 0x0D) //把:让出1个  所以在+1
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
        re = 1;
    }
    else if (strstr(buf, "CSCON:0"))
    {
        re = 1;
    }
    else if (strstr(buf, "CCLK:"))
    {
#if NB_CLK > 0 && API_UseRTC > 0
        //+CCLK:20/12/04,05:53:35+32
        Calendar_Type CLK;
        char *Location;
        int len = strlen("CCLK:");
        Location = strstr(buf, "CCLK:");

        CLK.time.second = (Location[len + 15] - 0x30) * 10 + (Location[len + 16] - 0x30);
        CLK.time.minute = (Location[len + 12] - 0x30) * 10 + (Location[len + 13] - 0x30);
        CLK.time.hour = (Location[len + 9] - 0x30) * 10 + (Location[len + 10] - 0x30) + (((Location[len + 18] - 0x30) * 10 + (Location[len + 19] - 0x30)) / 4);
        CLK.date.day = (Location[len + 6] - 0x30) * 10 + (Location[len + 7] - 0x30);
        CLK.date.month = (MONTH)((Location[len + 3] - 0x30) * 10 + (Location[len + 4] - 0x30));
        CLK.date.year = (Location[len] - 0x30) * 10 + (Location[len + 1] - 0x30) + 2000;

        API_Calendar(&CLK);
        API_Set_Time_HEX(&CLK);
#endif
        re = 1;
    }
    else if (strstr(buf, "NPSMR:1")) //进入psm
    {
        re = 0;
    }

    return re;
}
static uint8_t bc28OpenSocket(iot_type *iot)
{
    IOTDAT(MODULE).readDGRAM = 1;
    if (iot->foo.LinkMode == TCPNum)
        iotprintf("AT+NSOCR=STREAM,6,0,1\r\n");
    else
        iotprintf("AT+NSOCR=DGRAM,17,0,1\r\n");

    return 2;
}
static uint8_t bc28SendApn(iot_type *iot)
{
    iotprintf("%s%32s%s", "AT+CGDCONT=1,\"IP\",\"", iot->net.Apn, "\"\r\n");
    return 2;
}
static uint8_t bc28SendUdpTcpIpPort(iot_type *iot)
{
    if (iot->foo.LinkMode == UDPNum)
    {
        IOTDAT(MODULE).ip = iot->net.Ip;
        IOTDAT(MODULE).port = iot->net.Port;
        return 1;
    }
    else
    {
        iotprintf("%s%c%c%15s%c%5s%s", "AT+NSOCO=", IOTDAT(MODULE).DGRAM, ',', iot->net.Ip, ',', iot->net.Port, "\r\n");
    }

    return 2;
}
static uint8_t bc28SendCoapIpPort(iot_type *iot)
{
    iotprintf("%s%15s%c%5s%s", "AT+NCDP=", iot->net.Ip, ',', iot->net.Port, "\r\n");
    return 2;
}
static uint8_t bc28CloseSocket(iot_type *iot)
{
    iotprintf("%s%c%s", "AT+NSOCL=", IOTDAT(MODULE).DGRAM, "\r\n");
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
    if (strcmp(name, "coap2") == 0)
    {
        /*没注册到平台*/
        return 4;
    }
    if (strcmp(name, "txdat") == 0)
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

    if (strstr(buf, "+NNMI:"))
    {
        //+NNMI:49,FF1161389634170101B.....
        analy.loc = strstr(buf, "+NNMI:");
        for (ta = 0; ta < 4; ta++)
        {
            if (analy.loc[6 + ta] == ',')
            {
                break;
            }
            else
            {
                analy.len *= 10;
                analy.len += (analy.loc[6 + ta] - 0x30);
            }
        }
        ta += 6;
        analy.loc = &analy.loc[ta];
    }
    else if (strstr(buf, "+MIPLWRITE:"))
    {
        //+MIPLWRITE: 0,53583,3200,0,5750,2,5,12345,0,0
        uint8_t td = 0, tf = 0;
        analy.loc = strstr(buf, "+MIPLWRITE:");
        for (ta = 0; ta < 50; ta++)
        {
            if (analy.loc[11 + ta] == ',')
            {
                td++;
                if (td == 6)
                {
                    for (tf = 0; tf < 4; tf++)
                    {
                        if (analy.loc[12 + ta + tf] == ',')
                        {
                            break;
                        }
                        else
                        {
                            analy.len *= (10 * tf);
                            analy.len += (analy.loc[12 + ta + tf] - 0x30);
                        }
                    }
                    break;
                }
            }
        }
        analy.len >>= 1;
        ta += (12 + tf);
        analy.loc = &analy.loc[ta];
    }
    else
    {
        char tc = 0;
        uint8_t ServeripNum, PortNum;
        ServeripNum = mystrlen(iot->net.Ip, 15);
        PortNum = mystrlen(iot->net.Port, 5);
        analy.loc = strstr(buf, iot->net.Ip);
        for (ta = 0; ta < 5; ta++)
        {
            if (analy.loc[ServeripNum + 1 + PortNum + ta] == ',')
            {
                for (tc = 0; tc < 4; tc++)
                {
                    if (analy.loc[ServeripNum + 2 + PortNum + ta + tc] == ',')
                        break;
                    else
                    {
                        analy.len *= 10;
                        analy.len += (analy.loc[ServeripNum + 2 + PortNum + ta + tc] - 0x30);
                    }
                }
                break;
            }
        }
        ta += (ServeripNum + 3 + PortNum + tc);
        analy.loc = &analy.loc[ta];
    }

    return analy;
}
static void UDP_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
    iotprintf("%s%c%c%15s%c%5s%c%d%c%l%a%s", "AT+NSOST=", IOTDAT(MODULE).DGRAM, ',', IOTDAT(MODULE).ip, ',',
              IOTDAT(MODULE).port, ',', len, ',', len, buf, "\r\n");
}
static void TCP_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
    iotprintf("%s%c%c%d%c%l%a%s", "AT+NSOSD=", IOTDAT(MODULE).DGRAM, ',', len, ',', len, buf, "\r\n");
}
static void COAP_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
#if CoapProfile > 0
    uint8_t NBSendBuff[10];
    memset(NBSendBuff, 0, 10);
    /*Coap涉及profile文件头需要根据协议自行修改*/
    NBSendBuff[0] = '0';
    NBSendBuff[1] = '0'; //profile固定写法

    ww = HIGHWORD(len);
    HexToAscll((char *)&NBSendBuff[2], (uint8_t *)&ww, 1);
    ww = LOWWORD(len);
    HexToAscll((char *)&NBSendBuff[4], (uint8_t *)&ww, 1);

    iotprintf("%s%d%s%s%l%a%s", "AT+NMGS=", len + 3, ",", NBSendBuff,
              len, buf, "\r\n");
#else
    iotprintf("%s%d%s%l%a%s", "AT+NMGS=", len, ",", len, buf, "\r\n");
#endif
}
static void ONET_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
}
getcmdinfo_type GETCMD(MODULE)(uint8_t linkmode)
{
    getcmdinfo_type info;
    if (linkmode == InitNum)
    {
        info.cmd = (cmd_type *)&DIC_INT(MODULE);
        info.len = ARRAY_SIZE(DIC_INT(MODULE));
    }
    else if (linkmode == CoapNum)
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
