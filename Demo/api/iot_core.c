/*
*********************************************************************************************************
*
*	模块名称 : 新版IOT联网程序
*	文件名称 : iot_core.c
*	版    本 : V1.0.0
*	说    明 : 使用字典方式联网，以及使用多缓的数据处理，调用malloc.c,conver.c,uart_api.c中的程序
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "iot_core.h"
#ifdef USEBC260
#include "iot_bc260.h"
#elif USEN23
#include "iot_n23.h"
#else
#include "iot_bc28_35_95.h"
#endif

/* 静态变量声明 */
static iot_type iot = {NULL};
static getcmdinfo_type fun = {NULL};
static ComQueue *iotQuene = NULL; /* 待解析的串口数据队列 ，存放待解析的串口数据 */

/* 底层函数声明 */
static char *match_cmd(cmd_type *cmd, uint8_t len, iot_type *iot);
static void FooSend(char *buff, uint16_t len);

/* 供参考 */
__weak uint8_t iotRxDeal(void)
{
    uint16_t len = 0;
    static uint8_t thisend = 0;
    if ((len = _iotRxData("68")) != 0)
    {
        /* 数据接收处理在这里写 */
        debug("\r\n解析完成后接收到的数据为:\r\n");
        for (uint16_t j = 0; j < len; j++)
        {
            debug("%x ", IOTRXBUF[j]);
        }

        thisend = 1;
        /* 最好有结束包控制，否则按照固定流程会发送3次心跳 */
        if (thisend == 1)
        {
            /* 收到结束包返回0,否则返回1 */
            thisend = 0;
            return 0;
        }
    }
    return 1;
}
/* 供参考 */
__weak void NBTxDeal(void){};
/* 供参考 */
__weak uint8_t iotTxDeal(void)
{
    /* 心跳在这里添加 */
    NBTxDeal();
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: iotRecDat
*	功能说明: 数据接收处理，放在串口接收数据完成的地方
*	形    参: *buf：数据缓存
*             len：长度
*	返 回 值: 无
*	备    注：使用 malloc 动态申请内存，因此需要配置缓存长度
*             调用 bsp_uart.c 中关于数据处理的相关函数
*********************************************************************************************************
*/
void iotRecDat(uint8_t *buf, uint16_t len)
{
    if (len == 0)
        return;
    if (!QueueIsFull(iotQuene))
    {
        ComData *DealBuf = (ComData *)mymalloc(sizeof(ComData) + sizeof(char) * len); /* 分配空间存储待解析数据 */
        if (DealBuf)
        {
            DealBuf->len = len;
            memcpy(DealBuf->pData, buf, len);  /* 将接收数组的数据拷贝到待解析数组中 */
            SendComMessage(iotQuene, DealBuf); /* 将待解析数组指针及数据长度加入待解析队列 */
        }
    }
}
/*
*********************************************************************************************************
*	函 数 名: iot_init
*	功能说明: 初始化时调用，核心函数
*	形    参: iot_foo_type *FOO：iot_foo_type型结构体指针变量
*             typedef struct
*             {
*                 uint8_t LinkMode;     //连接方式
*                 uint16_t linkmaxtime; //连接最大时长
*             } iot_foo_type;
*
*	返 回 值: 字典Faildo中定义的值，具体查看相应的模块函数
*	备    注：无
*********************************************************************************************************
*/
uint8_t iot_init(iot_foo_type *FOO)
{
    char *failname = NULL;
    uint8_t result = 0;
    fun = GETCMD(MODULE)(FOO->LinkMode);

    if (fun.cmd == NULL)
        return 0;

    if ((iotQuene = InitComMessage()) == NULL)
        return 0;

    iot.foo.LinkMode = FOO->LinkMode;
    iot.foo.linkmaxtime = FOO->linkmaxtime;

    iot.core.execute = fun.cmd[0].name;
    iot.core.iotStart = 1;
    while (iot.core.iotStart)
    {
        failname = match_cmd(fun.cmd, fun.len, &iot);
        if (fun.Faildo != NULL)
        {
            if (fun.Faildo(failname) != 0)
                result = fun.Faildo(failname);
        }
    }

    /* 再次判断一下是否还有未被清空的内存 */
    DelComMessage(iotQuene);

    memset(&iot, 0, sizeof(iot));
    return result;
}
/*
*********************************************************************************************************
*	函 数 名: iot_init
*	功能说明: 初始化时调用，核心函数
*	形    参: iot_foo_type *FOO：iot_foo_type型结构体指针变量
*             iot_net_type *NET：iot_net_type型结构体指针变量
*             typedef struct
*             {
*                 char *Ip;   //指向IP缓存的指针
*                 char *Port; //指向端口缓存的指针
*                 char *Apn;  //指向APN缓存的指针
*             } iot_net_type;
*
*	返 回 值: 0：参数错误，其他值：字典Faildo中定义的值，具体查看相应的模块函数
*	备    注：确保IP和端口的传入数据格式正确，不正确的话，就不会进行联网操作
*********************************************************************************************************
*/
uint8_t iot_start(iot_foo_type *FOO, iot_net_type *NET)
{
    if ((NET->Ip == NULL) || (NET->Port == NULL))
        return 0;

    /* 传入的ip或者端口有一个不对就不干活了 */
    if (TheIPFormatIsCorrect(NET->Ip, 0) == 0 || TheIPFormatIsCorrect(NET->Port, 1) == 0)
        return 0;

    iot.net.Ip = NET->Ip;
    iot.net.Port = NET->Port;

    if (NET->Apn != NULL)
    {
        iot.net.Apn = NET->Apn;
    }
    else
    {
        char tapn[1] = {0};
        iot.net.Apn = tapn;
    }

    IPReduceZero(iot.net.Ip);         /* 转成模组能使用的IP */
    PortReduceZero_New(iot.net.Port); /* 转成模组能使用的PORT */
    return iot_init(FOO);
}
/*
*********************************************************************************************************
*	函 数 名: getiot
*	功能说明: 获取iot结构体地址
*	形    参: 无
*	返 回 值: iot结构体地址
*	备    注：用于获取iot内部数据，因为iot是静态变量，函数外不可见
*********************************************************************************************************
*/
iot_type *getiot(void)
{
    return &iot;
}
/*
*********************************************************************************************************
*	函 数 名: iotState
*	功能说明: 获取iot.core.iotStart的值
*	形    参: 无
*	返 回 值: 0：未处于连接状态，1：处于连接状态
*	备    注：建议联网前调用该函数判断一下是否已经处于连接状态
*********************************************************************************************************
*/
uint8_t iotState(void)
{
    return iot.core.iotStart;
}
/*
*********************************************************************************************************
*	函 数 名: iotTxData
*	功能说明: 数据发送函数
*	形    参: *buf：数据缓存
*             len：长度
*	返 回 值: 无
*	备    注：当需要发送数据时，调用此函数
*********************************************************************************************************
*/
void iotTxData(uint8_t *buf, uint16_t len)
{
    if (fun.SendFun == NULL)
        return;
    fun.SendFun(buf, len);
}
/*
*********************************************************************************************************
*	函 数 名: iotRxData
*	功能说明: 数据接收解析函数
*	形    参: iot_type *iot：iot_type型结构体指针
*             *word：ascii型的字符串
*	返 回 值: 解析出的数据长度，并将解析完成的数据放到iot->core.rxbuf中
*	备    注：请参考iotRxDeal(void)函数中的例子，使用方法为：len = _iotRxData("68");
*            _iotRxData是个宏定义，原型为：iotRxData(getiot(), word)，其中"68"是关键字，要接收的数据的首字节
*            ！！！注意：关键字必须是'0'-'9','a'-'f','A'-'F',关键字不要超过5字节，"68"算一个字节
*            想要获取解析后的数据直接使用宏定义：IOTRXBUF
*            该宏定义原型为：getiot()->core.rxbuf 即iot->core.rxbuf
*            即：char buf[len]; memcpy(buf,IOTRXBUF,len);或者直接调用。
*            ！！！注意：该函数必须在其作用域内使用，即必须放在 __weak uint8_t iotRxDeal(void) 中
*                       因为iot->core.rxbuf中的数据在离开该函数后就会被释放掉        
*********************************************************************************************************
*/
uint16_t iotRxData(iot_type *iot, const char *word)
{
    iot_buf_type dat = {0, NULL};
    uint8_t wordlen = mystrlen(word, 10);
    if (fun.RxFun == NULL)
        return 0;
    dat = fun.RxFun(iot);

    if (dat.len > (iot->core.rxlen >> 1))
    {
        dat.len = (iot->core.rxlen >> 1);
    }

    if ((dat.len != 0) && (dat.loc != NULL))
    {
        if (strnstr(dat.loc, word, wordlen, 10 + wordlen)) /* 在10个字节之内找到关键字 */
        {
            AsciiToHex((uint8_t *)strstr(dat.loc, word), (uint8_t *)iot->core.rxbuf, dat.len);
            return dat.len;
        }
        else
        {
            uint8_t i = 0;
            char nword[10] = {0};
            /* 实现将关键字大小写转化 */
            for (i = 0; i < wordlen; i++)
            {
                if ((word[i] >= 0x40) && (word[i] <= 0x46))
                {
                    nword[i] = word[i] + 0x20;
                }
                else if ((word[i] >= 0x60) && (word[i] <= 0x66))
                {
                    nword[i] = word[i] - 0x20;
                }
                else if ((word[i] >= 0x30) && (word[i] <= 0x39))
                {
                    nword[i] = word[i];
                }
            }
            if (strnstr(dat.loc, nword, wordlen, 10 + wordlen)) /* 在10个字节之内找到关键字 */
            {
                AsciiToHex((uint8_t *)strstr(dat.loc, nword), (uint8_t *)iot->core.rxbuf, dat.len);
                return dat.len;
            }
            else
            {
                /* 实现将关键字转成hex码 */
                uint8_t hexword[5] = {0}, tt = 0;
                AsciiToHex((uint8_t *)word, hexword, wordlen >> 1);

                for (tt = 0; tt < 10; tt++) /* 在10个字节之内找到关键字 */
                {
                    if (dat.loc[tt] == hexword[0])
                    {
                        if (camper((uint8_t *)&dat.loc[tt], hexword, wordlen >> 1))
                        {
                            memcpy(iot->core.rxbuf, &dat.loc[tt], dat.len);
                            return dat.len;
                        }
                    }
                }
            }
        }
    }

    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: iotEnd
*	功能说明: 结束连接
*	形    参: iot_type *iot：iot_type型结构体指针
*	返 回 值: 固定2
*	备    注：字典使用       
*********************************************************************************************************
*/
uint8_t iotEnd(iot_type *iot)
{
    iot->core.iotStart = 0;
    return 2;
}
/*
*********************************************************************************************************
*	函 数 名: iotRst
*	功能说明: 复位操作
*	形    参: iot_type *iot：iot_type型结构体指针
*	返 回 值: 0：失败，1：成功
*	备    注：字典使用       
*********************************************************************************************************
*/
uint8_t iotRst(iot_type *iot)
{
    if (iot->core.rstcount++ >= 3)
    {
        return 0;
    }
    return 1;
}
/*
*********************************************************************************************************
*	函 数 名: iotRead
*	功能说明: 字典数据读取
*	形    参: iot_type *iot：iot_type型结构体指针
*	返 回 值: 0：失败，1：成功
*	备    注：字典使用       
*********************************************************************************************************
*/
uint8_t iotRead(iot_type *iot)
{
    if (iotRxDeal() == 0)
        return 0;
    return 1;
}
/*
*********************************************************************************************************
*	函 数 名: iotRead
*	功能说明: 字典数据发送
*	形    参: iot_type *iot：iot_type型结构体指针
*	返 回 值: 0：失败，2：其他
*	备    注：字典使用       
*********************************************************************************************************
*/
uint8_t iotSend(iot_type *iot)
{
    if (iot->core.sendcount++ < 3)
    {
        /* 发送心跳 */
        iotTxDeal();
    }
    else
    {
        return 0;
    }
    return 2;
}
/*
*********************************************************************************************************
*	函 数 名: iotTim
*	功能说明: 字典核心计时器
*	形    参: 无
*	返 回 值: 无
*	备    注：必须放在秒定时器里面，否则无法正常运行     
*********************************************************************************************************
*/
void iotTim(void)
{
    if (iot.core.timeout > 0)
    {
        iot.core.timeout--;
    }

    (iot.foo.linkmaxtime > 0) ? (iot.foo.linkmaxtime--) : (iot.core.iotStart = 0);
}
/*
*********************************************************************************************************
*	函 数 名: iotTim
*	功能说明: 字典核心函数 
*	形    参: cmd_type *cmd：字典
*             len：字典长度
*             iot_type *iot：iot_type型结构体指针
*	返 回 值: 失败步骤的名
*	备    注：调用字典的函数    
*********************************************************************************************************
*/
static char *match_cmd(cmd_type *cmd, uint8_t len, iot_type *iot)
{
    volatile uint8_t i = 0, j = 0;

    if (mystrlen(iot->core.execute, MAX_CMD_NAME_LENGTH + 1) > MAX_CMD_NAME_LENGTH) /* 必须保证执行命令是有限长度 */
    {
        iot->core.iotStart = 0;
        return NULL;
    }

    for (i = 0; i < len; i++) /* 遍历命令列表 */
    {
        if (strcmp(cmd[i].name, iot->core.execute) == 0)
        {
            for (j = 0; j < cmd[i].recount; j++) /* 重复次数 */
            {
                uint32_t timout = 0, timmem = 0;

                /* 发送处理过程 */
                if (cmd[i].atcommand != NULL) /* 有对应的AT命令 */
                {
                    FooSend(cmd[i].atcommand, strlen(cmd[i].atcommand));
                }
                else
                {
                    if (cmd[i].sendop != NULL)
                    {
                        uint8_t re = cmd[i].sendop(iot);
                        if (re == 1)
                        {
                            iot->core.execute = cmd[i].sucname;
                            return NULL;
                        }
                        else if (re == 0)
                        {
                            iot->core.execute = cmd[i].failname;
                            return cmd[i].name; /* 返回失败步骤的任务名 */
                        }
                        else
                        {
                            /* 啥也不干 */
                        }
                    }
                }
                iot->core.timeout = cmd[i].timeout;
                timmem = cmd[i].timeout;
                while (iot->core.timeout != 0) /* 还未超时 */
                {
#if Use_uCOS > 0
                    OS_ERR err;
                    OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_DLY, &err);
#elif Use_Event > 0
                    WEIGOU;
                    ExecuteEvent;
#endif
                    /* 接收处理过程 */
                    if (!QueueIsEmpty(iotQuene)) /* 判断队列是否位空,不为空则一定有数据 */
                    {
                        ComData *data = GetComMessage(iotQuene); /* 在队列中获取未解析的数据 */
                        iot->core.rxbuf = (char *)data->pData;   /* 取出来的数据一定要释放 */
                        iot->core.rxlen = data->len;
#ifdef TS_Uart
                        API_UART_SEND(TS_Uart, (uint8_t *)iot->core.rxbuf, iot->core.rxlen); /* 调试输出 */
#endif
                        if (cmd[i].sucword != NULL) /* 如果接收确认不为空 */
                        {
                            char *result = strstr(iot->core.rxbuf, cmd[i].sucword);
                            memset(iot->core.rxbuf, 0, iot->core.rxlen);
                            myfree(data); /* 释放已经解析的数据 */
                            if (result)
                            {
                                iot->core.execute = cmd[i].sucname;
                                return NULL;
                            }
                        }
                        else
                        {
                            if (cmd[i].rxop != NULL)
                            {
                                uint8_t re = cmd[i].rxop(iot);
                                memset(iot->core.rxbuf, 0, iot->core.rxlen);
                                myfree(data); /* 释放已经解析的数据 */
                                if (re == 1)
                                {
                                    iot->core.execute = cmd[i].sucname;
                                    return NULL;
                                }
                                else if (re == 0)
                                {
                                    iot->core.execute = cmd[i].failname;
                                    return cmd[i].name; /* 返回失败步骤的任务名 */
                                }
                                else
                                {
                                    /* 啥也不干 */
                                }
                            }
                            else
                            {
                                myfree(data); /* 释放已经解析的数据 */
                                return NULL;
                            }
                        }
                    }
                    else
                    {
                        if (timout++ >= 0xfffff)
                        {
                            if (timmem == iot->core.timeout) /* 很长时间过去都和原先的时间相同，说明定时器坏了 */
                            {
                                iot->core.execute = cmd[i].failname;
                                return NULL;
                            }
                        }
                    }
                }
            }
            iot->core.execute = cmd[i].exname; /* 超过了重试次数要做的事情 */
            return cmd[i].name;                /* 返回失败步骤的任务名 */
        }
    }
    iot->core.iotStart = 0; /* 全部遍历了都没有找到对应命令 */
    return NULL;
}
/*
*********************************************************************************************************
*	函 数 名: FooSend
*	功能说明: 字典核心发送
*	形    参: *buff：发送缓存
*             len：长度
*	返 回 值: 无
*	备    注：以下函数涉及硬件以及串口收发相关操作，外部引用API_Uart_Send函数  
*********************************************************************************************************
*/
static void FooSend(char *buff, uint16_t len)
{
#ifdef TS_Uart
    API_UART_SEND(TS_Uart, (uint8_t *)buff, len); /* 调试输出 */
#endif
    API_UART_SEND(NB_Uart, (uint8_t *)buff, len); /* 模块通讯 */
}
/*
*********************************************************************************************************
*	函 数 名: iotprintf
*	功能说明: AT命令发送
*	形    参: 变参
*	返 回 值: 无
*	备    注：就像调用printf一样调用该函数
*            命令说明：c：发送一个字符
*                     d：发送一个数字  
*                     s：发送一串字符串
*                     l：获取将要发送的hex数据的长度
*                     a：将hex转成ascii码在发送，使用a命令前必须使用l命令
*                     数字：限制发送长度，针对s命令
*********************************************************************************************************
*/
void iotprintf(const char *format, ...)
{
    char c;
    char *p = NULL;
    int len = 0, limlen = 0, a = 0;
    char buf[20] = {0};
    char *sendbuf = NULL;
    va_list ap;           /* 定义一个int *型的指针ap */
    va_start(ap, format); /* 初始化ap，让它指向第一个可变参数的地址 */

    while ((c = *format++) != '\0') /* 开始解析printf函数中的字符串(即第一个固定参数) */
    {
        switch (c) /* 在while中，依次读出字符串中的字符，在这里依依地进行判断和解析 */
        {
        case '%':          /* 如果字符是%，即格式申明符号，则解析该格式 */
            c = *format++; /* 让c等于%后的第一个字母 */
            switch (c)     /* 对上述字母进行解析 */
            {
            case '0': /* 用于长度限制 */
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
                    FooSend(p, mystrlen(p, limlen));
                }
                break;
            case 'c':                   /* 如果是%c，即输出字母 */
                c = va_arg(ap, int);    /* 获取当前可变参数的值，并指向下一个可变参数 */
                FooSend((char *)&c, 1); /* 调用底层(实际操作硬件的那层)来完成输出结果 */
                break;
            case 'l':                  /* 如果是%l，即赋值长度 */
                len = va_arg(ap, int); /* 获取当前可变参数的值，并指向下一个可变参数 */
                break;
            case 's': /*如果是%s，即输出字符串*/
                p = va_arg(ap, char *);
                FooSend(p, strlen(p));
                break;
            case 'a': /* 如果是%a，即数据转成ascii后输出  用a命令前必须设置%l的长度 */
                p = va_arg(ap, char *);
                sendbuf = (char *)mymalloc(len * 2);
                if (sendbuf != NULL)
                {
                    for (a = 0; a < len; a++)
                    {
                        HexToAscll(&sendbuf[a * 2], (uint8_t *)p, 1);
                        p++;
                    }
                    FooSend(sendbuf, len * 2);
                    myfree(sendbuf);
                }
                len = 0;
                break;
            case 'd': /* 如果是%d，即以十进制输出 */
                a = va_arg(ap, int);
                sprintf(buf, "%d", a);
                FooSend(buf, strlen(buf));
                break;
            default:
                break;
            }
            break;
        default:
            FooSend((char *)&c, 1); /* 输出字符串中为普通字符的字符 */
            break;
        }
    }
    va_end(ap);
}
