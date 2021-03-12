#include "iot_bc260.h"

/*全局变量声明*/
/*bc260_dat_type IOTDAT(MODULE);*/
IOTDATTYPE(MODULE)
IOTDAT(MODULE);
/*静态函数声明*/
static uint8_t bc260iotRst(iot_type *iot);
static uint8_t bc260rxdeal(iot_type *iot);
static uint8_t bc260SendUdpTcpIpPort(iot_type *iot);
static uint8_t bc260OneNetojb1Rec(iot_type *iot);
static uint8_t bc260OneNetojb2Rec(iot_type *iot);
static uint8_t bc260SendCoapIPort(iot_type *iot);
static uint8_t bc260OberveSend(iot_type *iot);

/*字典声明*/
const cmd_type DIC_TCUDP(MODULE)[] = {
    {"rst", NULL, bc260iotRst, NULL, NULL, 0, 1, "nrb", "end2", "end2"},
    {"nrb", "AT+NRB\r\n", NULL, "RDY", NULL, 10, 1, "at", "at", "at"},
    {"at", "AT\r\n", NULL, "OK", NULL, 2, 3, "creg", "end3", "end3"},
    {"creg", "AT+CEREG=1\r\n", NULL, "OK", NULL, 2, 3, "cfunx", "cfunx", "cfunx"},
    {"cfunx", "AT+CFUN?\r\n", NULL, "CFUN: 1", NULL, 3, 3, "qsclk", "end3", "end3"},
    {"qsclk", "AT+QSCLK=0\r\n", NULL, "OK", NULL, 2, 1, "cpsm", "cpsm", "cpsm"},
    {"cpsm", "AT+CPSMS=1,,,\"01000011\",\"01000011\"\r\n", NULL, "OK", NULL, 2, 1, "qevent", "qevent", "qevent"},
    {"qevent", "AT+QNBIOTEVENT=1,1\r\n", NULL, "OK", NULL, 1, 1, "cgsn", "cgsn", "cgsn"},
    {"cgsn", "AT+CGSN=1\r\n", NULL, NULL, bc260rxdeal, 2, 3, "nccid", "rst", "rst"},
    {"nccid", "AT+QCCID\r\n", NULL, NULL, bc260rxdeal, 2, 3, "cimi", "rst", "rst"},
    {"cimi", "AT+CIMI\r\n", NULL, "OK", NULL, 2, 3, "con0", "con0", "con0"},
    {"con0", "AT+CSCON=0\r\n", NULL, "OK", NULL, 2, 1, "cgatt", "cgatt", "cgatt"},
    {"cgatt", "AT+CGATT?\r\n", NULL, NULL, bc260rxdeal, 2, 60, "csq", "end", "isyp"},
    {"isyp", "AT+CSQ\r\n", NULL, NULL, bc260rxdeal, 2, 3, "yp1", "end2", "end2"},
    {"yp1", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 2, "yp2", "end3", "end3"},
    {"yp2", "AT+QCSEARFCN\r\n", NULL, "OK", NULL, 2, 2, "rst", "end3", "end3"},
    {"csq", "AT+CSQ\r\n", NULL, NULL, bc260rxdeal, 1, 1, "clk", "clk", "clk"},
    {"clk", "AT+CCLK?\r\n", NULL, NULL, bc260rxdeal, 1, 1, "nustat", "nustat", "nustat"},
    {"nustat", "AT+QENG=0\r\n", NULL, NULL, bc260rxdeal, 2, 1, "udp1", "udp1", "udp1"},
    {"udp1", "AT+QICFG=\"dataformat\",1,1\r\n", NULL, "OK", NULL, 2, 3, "udp2", "rst", "rst"},
    {"udp2", NULL, bc260SendUdpTcpIpPort, NULL, bc260rxdeal, 5, 1, "con1", "rst", "rst"},
    {"con1", "AT+CSCON=1\r\n", NULL, "OK", NULL, 2, 1, "txdat", "txdat", "txdat"},
    {"txdat", NULL, iotSend, "OK", NULL, 10, 3, "wait", "end", "end"},
    {"wait", NULL, NULL, NULL, bc260rxdeal, 1, 60, "txdat", "end", "end"},
    /*{"rxdat", NULL, iotRead, NULL, NULL, 0, 1, "wait", "end", "wait"},*/
    {"end", "AT+QICLOSE=0\r\n", NULL, "OK", NULL, 2, 1, "end1", "end1", "end1"},
    {"end1", "AT+QSCLK=1\r\n", NULL, "OK", NULL, 2, 1, "end2", "end2", "end2"},
    {"end2", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 1, "end3", "end3", "end3"},
    {"end3", NULL, iotEnd, NULL, NULL, 0, 1, NULL, NULL, NULL}};

const cmd_type DIC_COAP(MODULE)[] = {
    {"rst", NULL, bc260iotRst, NULL, NULL, 0, 1, "nrb", "end2", "end2"},
    {"nrb", "AT+NRB\r\n", NULL, "RDY", NULL, 10, 1, "at", "at", "at"},
    {"at", "AT\r\n", NULL, "OK", NULL, 2, 3, "creg", "end3", "end3"},
    {"creg", "AT+CEREG=1\r\n", NULL, "OK", NULL, 2, 3, "cfunx", "cfunx", "cfunx"},
    {"cfunx", "AT+CFUN?\r\n", NULL, "CFUN: 1", NULL, 3, 3, "qsclk", "end3", "end3"},
    {"qsclk", "AT+QSCLK=0\r\n", NULL, "OK", NULL, 2, 1, "cpsm", "cpsm", "cpsm"},
    {"cpsm", "AT+CPSMS=1,,,\"01000011\",\"01000011\"\r\n", NULL, "OK", NULL, 2, 1, "qevent", "qevent", "qevent"},
    {"qevent", "AT+QNBIOTEVENT=1,1\r\n", NULL, "OK", NULL, 1, 1, "cgsn", "cgsn", "cgsn"},
    {"cgsn", "AT+CGSN=1\r\n", NULL, NULL, bc260rxdeal, 2, 3, "nccid", "rst", "rst"},
    {"nccid", "AT+QCCID\r\n", NULL, NULL, bc260rxdeal, 2, 3, "cimi", "rst", "rst"},
    {"cimi", "AT+CIMI\r\n", NULL, "OK", NULL, 2, 3, "con0", "con0", "con0"},
    {"con0", "AT+CSCON=0\r\n", NULL, "OK", NULL, 2, 1, "cgatt", "cgatt", "cgatt"},
    {"cgatt", "AT+CGATT?\r\n", NULL, NULL, bc260rxdeal, 2, 60, "csq", "end", "isyp"},
    {"isyp", "AT+CSQ\r\n", NULL, NULL, bc260rxdeal, 2, 3, "yp1", "end2", "end2"},
    {"yp1", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 2, "yp2", "end3", "end3"},
    {"yp2", "AT+QCSEARFCN\r\n", NULL, "OK", NULL, 2, 2, "rst", "end3", "end3"},
    {"csq", "AT+CSQ\r\n", NULL, NULL, bc260rxdeal, 1, 1, "clk", "clk", "clk"},
    {"clk", "AT+CCLK?\r\n", NULL, NULL, bc260rxdeal, 1, 1, "nustat", "nustat", "nustat"},
    {"nustat", "AT+QENG=0\r\n", NULL, NULL, bc260rxdeal, 2, 1, "coap1", "coap1", "coap1"},
    {"coap1", "AT+QLACFG=\"platform\",0\r\n", NULL, "OK", NULL, 2, 3, "coap2", "rst", "rst"},
    {"coap2", NULL, bc260SendCoapIPort, "OK", NULL, 2, 3, "coap3", "rst", "rst"},
    {"coap3", "AT+QLAADDOBJ=19,0,1,0\r\n", NULL, "OK", NULL, 2, 3, "coap4", "rst", "rst"},
    {"coap4", "AT+QLAREG\r\n", NULL, NULL, bc260rxdeal, 5, 1, "coap5", "rst", "rst"}, //只允许发一次失败就重来
    {"coap5", NULL, bc260OberveSend, "OK", NULL, 5, 1, "con1", "rst", "rst"},         //只允许发一次失败就重来
    {"con1", "AT+CSCON=1\r\n", NULL, "OK", NULL, 2, 1, "txdat", "txdat", "txdat"},
    {"txdat", NULL, iotSend, "OK", NULL, 10, 3, "wait", "end", "end"},
    {"wait", NULL, NULL, NULL, bc260rxdeal, 1, 60, "txdat", "end", "end"},
    /*{"rxdat", NULL, iotRead, NULL, NULL, 0, 1, "wait", "end", "wait"},*/
    {"end", "AT+QICLOSE=0\r\n", NULL, "OK", NULL, 2, 1, "end1", "end1", "end1"},
    {"end1", "AT+QSCLK=1\r\n", NULL, "OK", NULL, 2, 1, "end2", "end2", "end2"},
    {"end2", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 1, "end3", "end3", "end3"},
    {"end3", NULL, iotEnd, NULL, NULL, 0, 1, NULL, NULL, NULL}};

const cmd_type DIC_ONET(MODULE)[] = {
    {"rst", NULL, bc260iotRst, NULL, NULL, 0, 1, "nrb", "end2", "end2"},
    {"nrb", "AT+NRB\r\n", NULL, "RDY", NULL, 10, 1, "at", "at", "at"},
    {"at", "AT\r\n", NULL, "OK", NULL, 2, 3, "creg", "end3", "end3"},
    {"creg", "AT+CEREG=1\r\n", NULL, "OK", NULL, 2, 3, "cfunx", "cfunx", "cfunx"},
    {"cfunx", "AT+CFUN?\r\n", NULL, "CFUN: 1", NULL, 3, 3, "qsclk", "end3", "end3"},
    {"qsclk", "AT+QSCLK=0\r\n", NULL, "OK", NULL, 2, 1, "cpsm", "cpsm", "cpsm"},
    {"cpsm", "AT+CPSMS=1,,,\"01000011\",\"01000011\"\r\n", NULL, "OK", NULL, 2, 1, "qevent", "qevent", "qevent"},
    {"qevent", "AT+QNBIOTEVENT=1,1\r\n", NULL, "OK", NULL, 1, 1, "cgsn", "cgsn", "cgsn"},
    {"cgsn", "AT+CGSN=1\r\n", NULL, NULL, bc260rxdeal, 2, 3, "nccid", "rst", "rst"},
    {"nccid", "AT+QCCID\r\n", NULL, NULL, bc260rxdeal, 2, 3, "cimi", "rst", "rst"},
    {"cimi", "AT+CIMI\r\n", NULL, "OK", NULL, 2, 3, "con0", "con0", "con0"},
    {"con0", "AT+CSCON=0\r\n", NULL, "OK", NULL, 2, 1, "cgatt", "cgatt", "cgatt"},
    {"cgatt", "AT+CGATT?\r\n", NULL, NULL, bc260rxdeal, 2, 60, "csq", "end", "isyp"},
    {"isyp", "AT+CSQ\r\n", NULL, NULL, bc260rxdeal, 2, 3, "yp1", "end2", "end2"},
    {"yp1", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 2, "yp2", "end3", "end3"},
    {"yp2", "AT+QCSEARFCN\r\n", NULL, "OK", NULL, 2, 2, "rst", "end3", "end3"},
    {"csq", "AT+CSQ\r\n", NULL, NULL, bc260rxdeal, 1, 1, "clk", "clk", "clk"},
    {"clk", "AT+CCLK?\r\n", NULL, NULL, bc260rxdeal, 1, 1, "nustat", "nustat", "nustat"},
    {"nustat", "AT+QENG=0\r\n", NULL, NULL, bc260rxdeal, 2, 1, "onet0", "onet0", "onet0"},
    {"onet0", "AT+MIPLCONFIG=0,1\r\n", NULL, "OK", NULL, 2, 1, "onet1", "onet1", "onet1"},
    {"onet1", "AT+MIPLCREATE\r\n", NULL, NULL, bc260rxdeal, 5, 1, "onet2", "rst", "rst"},             //只允许发一次失败就重来
    {"onet2", "AT+MIPLADDOBJ=0,3200,1,\"1\",1,0\r\n", NULL, "OK", NULL, 5, 1, "onet3", "rst", "rst"}, //只允许发一次失败就重来
    {"onet3", "AT+MIPLOPEN=0,3600\r\n", NULL, NULL, bc260rxdeal, 30, 1, "onet4", "rst", "rst"},       //需要多等待一段时间 客户端注册 OneNET 平台
    {"onet4", NULL, bc260OneNetojb1Rec, NULL, bc260rxdeal, 5, 1, "onet5", "rst", "rst"},              //只允许发送一次，失败就重来
    {"onet5", NULL, bc260OneNetojb2Rec, "OK", NULL, 2, 3, "con1", "rst", "rst"},
    {"con1", "AT+CSCON=1\r\n", NULL, "OK", NULL, 2, 1, "txdat", "txdat", "txdat"},
    {"txdat", NULL, iotSend, "OK", NULL, 10, 3, "wait", "end", "end"},
    {"wait", NULL, NULL, NULL, bc260rxdeal, 1, 60, "txdat", "end", "end"},
    /*{"rxdat", NULL, iotRead, NULL, NULL, 0, 1, "wait", "end", "wait"},*/
    {"end", "AT+QICLOSE=0\r\n", NULL, "OK", NULL, 2, 1, "end1", "end1", "end1"},
    {"end1", "AT+QSCLK=1\r\n", NULL, "OK", NULL, 2, 1, "end2", "end2", "end2"},
    {"end2", "AT+CFUN=0\r\n", NULL, "OK", NULL, 10, 1, "end3", "end3", "end3"},
    {"end3", NULL, iotEnd, NULL, NULL, 0, 1, NULL, NULL, NULL}};

/* bc260如果不硬复位，某些命令不好使 */
static uint8_t bc260iotRst(iot_type *iot)
{
    if (iotRst(iot) == 1)
    {
        NB_OFF;
        delay_ms(100);
        NB_ON;
        return 1;
    }
    return 0;
}
/*返回1 收到成功的数据
  返回0 收到失败的数据
  返回2 未收到有效数据*/
static uint8_t bc260rxdeal(iot_type *iot)
{
    char *buf = iot->core.rxbuf;
    volatile uint8_t re = 2;
    if (strstr(buf, "QLAURC: \"write\""))
    {
        if (iotRead(iot) == 0)
        {
            /*说明收到结束包*/
            re = 0;
        }
    }
    else if (strstr(buf, "QIURC: \"recv\""))
    {
        if (iotRead(iot) == 0)
        {
            /*说明收到结束包*/
            re = 0;
        }
    }
    else if (strstr(buf, "MIPLWRITE: "))
    {
        if (iotRead(iot) == 0)
        {
            /*说明收到结束包*/
            re = 0;
        }
        //+MIPLWRITE: 0,11535,3200,0,5750,2,8,3131323233333434,0,0
    }
    else if (strstr(buf, "QIOPEN: 0,0"))
    {
        re = 1;
    }
    else if (strstr(buf, "CFUN: 1"))
    {
        re = 1;
    }
    else if (strstr(buf, "MIPLCREATE: 0"))
    {
        //+MIPLCREATE: 0
        re = 1;
    }
    else if (strstr(buf, "MIPLOBSERVE:"))
    {
        //+MIPLOBSERVE: 0,81895,1,3200,0,-1
        char *Location, *Locationl;
        int len = strlen("MIPLOBSERVE: 0,");
        Location = strstr(buf, "MIPLOBSERVE: 0,");
        Locationl = strstr(&Location[len], ",");
        if ((Locationl != NULL) && (Locationl > (Location + len)))
        {
            memset(IOTDAT(MODULE).msgid1, 0, 20);
            memcpy(IOTDAT(MODULE).msgid1, &Location[len], Locationl - Location - len);
            re = 1;
        }
        else
        {
            re = 0;
        }
    }
    else if (strstr(buf, "MIPLDISCOVER:"))
    {
        //+MIPLDISCOVER: 0,31605,3311
        char *Location, *Locationl;
        int len = strlen("MIPLDISCOVER: 0,");
        Location = strstr(buf, "MIPLDISCOVER: 0,");
        Locationl = strstr(&Location[len], ",");
        if ((Locationl != NULL) && (Locationl > (Location + len)))
        {
            memset(IOTDAT(MODULE).msgid2, 0, 20);
            memcpy(IOTDAT(MODULE).msgid2, &Location[len], Locationl - Location - len);
            re = 1;
        }
        else
        {
            re = 0;
        }
    }
    else if (strstr(buf, "CGSN: ")) //+CGSN: 869951040044046
    {
        char *Location;
        int len = strlen("CGSN: "), c = 0;
        Location = strstr(buf, "CGSN: ");
        memcpy(IOTDAT(MODULE).CGSNNum, &Location[len], 15);
        for (c = 0; c < 15; c++)
        {
            if ((IOTDAT(MODULE).CGSNNum[c] <= 0x30) && (IOTDAT(MODULE).CGSNNum[c] >= 0x39))
            {
                break; //当使用coap连接时，必须保证模块号正确
            }
        }
        if (c == 15)
        {
            re = 1;
        }
    }
    else if (strstr(buf, "QCCID: ")) //+QCCID: 89860317482032831892
    {
        char *Location;
        int len = strlen("QCCID: "), c = 0;
        Location = strstr(buf, "QCCID: ");
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
    else if (strstr(buf, "CGATT: 1")) //+CGATT: 1
    {
        re = 1;
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
    else if (strstr(buf, "CSQ: ")) //+CSQ: 31,0
    {
        char *Location;
        int len = strlen("CSQ: ");
        Location = strstr(buf, "CSQ: ");
        if (Location[len + 1] == ',')
            IOTDAT(MODULE).RSSI = Location[len] - 0x30;
        else
            IOTDAT(MODULE).RSSI = ((Location[len] - 0x30) << 4) + (Location[len + 1] - 0x30);
        if ((IOTDAT(MODULE).RSSI >= 0x20) && (IOTDAT(MODULE).RSSI != 0x99))
            re = 1; /* 信号大于20才异频 */
        else
            re = 0;
    }
    else if (strstr(buf, "QLAURC:"))
    {
        //+QLAURC: "observe",60722,0,19,0,0
        char *Location, *Locationl;
        int len = strlen("\"observe\",");
        Location = strstr(buf, "\"observe\",");
        Locationl = strstr(&Location[len], ",");
        if ((Locationl != NULL) && (Locationl > (Location + len)))
        {
            memset(IOTDAT(MODULE).oberve, 0, 6);
            memcpy(IOTDAT(MODULE).oberve, &Location[len], Locationl - Location - len);
            re = 1;
        }
        else
        {
            re = 0;
        }
    }
    else if (strstr(buf, "QENG: "))
    {
        //+QENG: 0,2506,,176,"0BF576D1",-59,-6,31,21,5,3095,0,-17,3
        char *Location, *Locationl, choutdou = 1;
        int len = strlen("QENG: 0,"), ti = 0, tj = 0;
        Location = strstr(buf, "QENG: 0,");
        IOTDAT(MODULE).rsrp = 0;
        IOTDAT(MODULE).cellid = 0;
        IOTDAT(MODULE).EARFCN = 0;
        IOTDAT(MODULE).PCI = 0;
        IOTDAT(MODULE).ECL = 0;
        IOTDAT(MODULE).SNR = 0;
        for (;;)
        {
            Locationl = strstr(&Location[len], ",");
            if ((Locationl != NULL) && (Locationl >= (Location + len)))
            {
                if (choutdou == 1) //小区号
                {
                    for (ti = 0; ti < Locationl - Location - len; ti++)
                    {
                        IOTDAT(MODULE).EARFCN = (IOTDAT(MODULE).EARFCN * 10) + (Location[len + ti] - 0x30);
                    }
                }
                else if (choutdou == 3) //PCI
                {
                    for (ti = 0; ti < Locationl - Location - len; ti++)
                    {
                        IOTDAT(MODULE).PCI = (IOTDAT(MODULE).PCI * 10) + (Location[len + ti] - 0x30);
                    }
                }
                else if (choutdou == 4) //cellid
                {
                    for (ti = 0; ti < 8; ti++)
                    {
                        if (Location[len] == '\"')
                        {
                            IOTDAT(MODULE).cellid = (IOTDAT(MODULE).cellid << 4) + CharToHex(Location[len + ti + 1]);
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
                            IOTDAT(MODULE).rsrp = IOTDAT(MODULE).rsrp * 10 - (Location[len + tj + 1] - 0x30);
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
                            IOTDAT(MODULE).SNR = IOTDAT(MODULE).SNR * 10 - (Location[len + tj + 1] - 0x30);
                        }
                    }
                    else
                    {
                        ti = Locationl - Location - len;
                        for (tj = 0; tj < ti; tj++)
                        {
                            IOTDAT(MODULE).SNR = IOTDAT(MODULE).SNR * 10 + (Location[len + tj] - 0x30);
                        }
                    }
                }
                else if (choutdou == 9) //band
                {
                    IOTDAT(MODULE).Band = Location[len] - 0x30;
                }
                else if (choutdou == 11) //ecl
                {
                    IOTDAT(MODULE).ECL = Location[len] - 0x30;
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
        re = 1;
    }
    else if (strstr(buf, "CSCON: 0"))
    {
        re = 1;
    }
    else if (strstr(buf, "CCLK: "))
    {
#if NB_CLK > 0 && API_UseRTC > 0
        //+CCLK: "2020/08/17,08:33:55+32"
        Calendar_Type CLK;

        char *Location;
        int len = strlen("CCLK: ");
        Location = strstr(buf, "CCLK: ");

        CLK.time.second = (Location[24] - 0x30) * 10 + (Location[25] - 0x30);
        CLK.time.minute = (Location[21] - 0x30) * 10 + (Location[22] - 0x30);
        CLK.time.hour = (Location[18] - 0x30) * 10 + (Location[19] - 0x30) + (((Location[27] - 0x30) * 10 + (Location[28] - 0x30)) / 4);
        CLK.date.day = (Location[15] - 0x30) * 10 + (Location[16] - 0x30);
        CLK.date.month = (MONTH)((Location[12] - 0x30) * 10 + (Location[13] - 0x30));
        CLK.date.year = (Location[9] - 0x30) * 10 + (Location[10] - 0x30) + 2000;

        API_Calendar(&CLK);
        API_Set_Time_HEX(&CLK);
#endif
        re = 1;
    }
    else if (strstr(buf, "ENTER PSM")) //QNBIOTEVENT: "ENTER PSM"
    {
        re = 0;
    }
    return re;
}
static uint8_t bc260SendCoapIPort(iot_type *iot)
{
    iotprintf("%s%15s%s%5s%s%s%s", "AT+QLACONFIG=0,\"", iot->net.Ip, "\",",
              iot->net.Port, ",\"", IOTDAT(MODULE).CGSNNum, "\",300,3\r\n");
    return 2; //发命令函数除非有额外需求否则一律返回2
}
static uint8_t bc260SendUdpTcpIpPort(iot_type *iot)
{
    if (iot->foo.LinkMode != TCPNum)
    {
        iotprintf("%s%15s%s%5s%s", "AT+QIOPEN=0,0,\"UDP\",", iot->net.Ip, ",", iot->net.Port, "\r\n");
    }
    else
    {
        iotprintf("%s%15s%s%5s%s", "AT+QIOPEN=0,0,\"TCP\",", iot->net.Ip, ",", iot->net.Port, "\r\n");
    }
    return 2; //发命令函数除非有额外需求否则一律返回2
}
static uint8_t bc260OneNetojb1Rec(iot_type *iot)
{
    iotprintf("%s%20s%s", "AT+MIPLOBSERVERSP=0,", IOTDAT(MODULE).msgid1, ",1\r\n");
    return 2; //发命令函数除非有额外需求否则一律返回2
}
static uint8_t bc260OneNetojb2Rec(iot_type *iot)
{
    iotprintf("%s%20s%s", "AT+MIPLDISCOVERRSP=0,", IOTDAT(MODULE).msgid2, ",1,4,\"5750\"\r\n");
    return 2; //发命令函数除非有额外需求否则一律返回2
}
static uint8_t bc260OberveSend(iot_type *iot)
{
    /*AT+QLAOBSRSP=60722,1,19,0,0,2,1,"FF",0*/
    iotprintf("%s%6s%s", "AT+QLAOBSRSP=", IOTDAT(MODULE).oberve, ",1,19,0,0,2,1,\"FF\",0\r\n");
    return 2; //发命令函数除非有额外需求否则一律返回2
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
    if ((strcmp(name, "coap4") == 0) || (strcmp(name, "coap5") == 0))
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

    /*+QLAURC: "write",3735,19,1,0,2,50,"data",0*/
    if (strstr(buf, "QLAURC: "))
    {
        char *Locationl = NULL, countdou = 0;
        int len = strlen("QLAURC: \"write\",");
        analy.loc = strstr(buf, "QLAURC: \"write\",");
        for (;;)
        {
            Locationl = strstr(&analy.loc[len], ",");
            if (Locationl != NULL)
            {
                analy.loc = Locationl + 1;
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
                if (analy.loc[ta] == ',')
                {
                    break;
                }
                else
                {
                    analy.len *= 10;
                    analy.len += (analy.loc[ta] - 0x30);
                }
            }
            ta += 2; //+2是因为 rxlen之后还有 ," 两个字符
            analy.loc = &analy.loc[ta];
        }
    }
    /*+QIURC: "recv",0,4,"54657374"*/
    else if (strstr(buf, "QIURC: "))
    {
        int len = strlen("QIURC: \"recv\",0,");
        analy.loc = strstr(buf, "QIURC:");

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

        ta += (len + 2); //+2是因为 rxlen之后还有 ," 两个字符
        analy.loc = &analy.loc[ta];
    }
    else if (strstr(buf, "MIPLWRITE:"))
    {
        //+MIPLWRITE: 0,11535,3200,0,5750,2,8,3131323233333434,0,0
        char *Locationl, countdou = 0;
        int len = strlen("MIPLWRITE: 0,");
        analy.loc = strstr(buf, "MIPLWRITE: 0,");
        Locationl = strstr(&analy.loc[len], ",");
        if ((Locationl != NULL) && (Locationl > (analy.loc + len)))
        {
            memset(IOTDAT(MODULE).msgid2, 0, 20);
            memcpy(IOTDAT(MODULE).msgid2, &analy.loc[len], Locationl - analy.loc - len);
            iotprintf("%s%20s%s", "AT+MIPLWRITERSP=0,", IOTDAT(MODULE).msgid2, ",2\r\n");
        }
        // 123456789
        // +MIPLWRITE: 0,8506,3200,0,5750,2,9,313233343536373839,0,0
        // AT+MIPLWRITERSP=0,8506,2
        for (;;)
        {
            Locationl = strstr(&analy.loc[len], ",");
            if (Locationl != NULL)
            {
                analy.loc = Locationl + 1;
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
                if (analy.loc[ta] == ',')
                {
                    break;
                }
                else
                {
                    analy.len *= 10;
                    analy.len += (analy.loc[ta] - 0x30);
                }
            }
            ta += 1; //+1是因为 rxlen之后还有 , 1个字符
            analy.loc = &analy.loc[ta];
            AsciiToHex((uint8_t *)analy.loc, (uint8_t *)analy.loc, analy.len);
            analy.len >>= 1;
        }
    }

    return analy;
}
static void UDP_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
    //AT+QISEND=0,3,AA1133
    iotprintf("%s%d%s%l%a%s", "AT+QISEND=0,", len, ",", len, buf, "\r\n");
}
static void TCP_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
    bc260udpsend(buf, len);
}
static void COAP_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
    /*AT+QLANOTIFY=19,0,0,2,75,"DATA",0,0*/
    iotprintf("%s%d%s%l%a%s", "AT+QLANOTIFY=19,0,0,2,", len, ",\"", len, buf, "\",0,0\r\n");
}
static void ONET_SEND(MODULE)(uint8_t *buf, uint16_t len)
{
    //AT+MIPLNOTIFY=0,9523,3200,0,5750,1,5,”hello”,0,0 //数据主动上报
    iotprintf("%s%20s%s%d%s%l%a%s", "AT+MIPLNOTIFY=0,", IOTDAT(MODULE).msgid1, ",3200,0,5750,1,",
              len * 2, ",\"", len, buf, "\",0,0\r\n");
}
getcmdinfo_type GETCMD(MODULE)(uint8_t linkmode)
{
    getcmdinfo_type info;
    if (linkmode == InitNum)
    {
    }
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
