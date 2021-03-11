/*
*********************************************************************************************************
*
*	ģ������ : �°�IOT��������
*	�ļ����� : iot_core.c
*	��    �� : V1.0.0
*	˵    �� : ʹ���ֵ䷽ʽ�������Լ�ʹ�ö໺�����ݴ�������malloc.c,conver.c,uart_api.c�еĳ���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
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

/* ��̬�������� */
static iot_type iot = {NULL};
static getcmdinfo_type fun = {NULL};
static ComQueue *iotQuene = NULL; /* �������Ĵ������ݶ��� ����Ŵ������Ĵ������� */

/* �ײ㺯������ */
static char *match_cmd(cmd_type *cmd, uint8_t len, iot_type *iot);
static void FooSend(char *buff, uint16_t len);

/* ���ο� */
__weak uint8_t iotRxDeal(void)
{
    uint16_t len = 0;
    static uint8_t thisend = 0;
    if ((len = _iotRxData("68")) != 0)
    {
        /* ���ݽ��մ���������д */
        debug("\r\n������ɺ���յ�������Ϊ:\r\n");
        for (uint16_t j = 0; j < len; j++)
        {
            debug("%x ", IOTRXBUF[j]);
        }

        thisend = 1;
        /* ����н��������ƣ������չ̶����̻ᷢ��3������ */
        if (thisend == 1)
        {
            /* �յ�����������0,���򷵻�1 */
            thisend = 0;
            return 0;
        }
    }
    return 1;
}
/* ���ο� */
__weak void NBTxDeal(void){};
/* ���ο� */
__weak uint8_t iotTxDeal(void)
{
    /* ������������� */
    NBTxDeal();
    return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: iotRecDat
*	����˵��: ���ݽ��մ������ڴ��ڽ���������ɵĵط�
*	��    ��: *buf�����ݻ���
*             len������
*	�� �� ֵ: ��
*	��    ע��ʹ�� malloc ��̬�����ڴ棬�����Ҫ���û��泤��
*             ���� bsp_uart.c �й������ݴ������غ���
*********************************************************************************************************
*/
void iotRecDat(uint8_t *buf, uint16_t len)
{
    if (len == 0)
        return;
    if (!QueueIsFull(iotQuene))
    {
        ComData *DealBuf = (ComData *)mymalloc(sizeof(ComData) + sizeof(char) * len); /* ����ռ�洢���������� */
        if (DealBuf)
        {
            DealBuf->len = len;
            memcpy(DealBuf->pData, buf, len);  /* ��������������ݿ����������������� */
            SendComMessage(iotQuene, DealBuf); /* ������������ָ�뼰���ݳ��ȼ������������ */
        }
    }
}
/*
*********************************************************************************************************
*	�� �� ��: iot_init
*	����˵��: ��ʼ��ʱ���ã����ĺ���
*	��    ��: iot_foo_type *FOO��iot_foo_type�ͽṹ��ָ�����
*             typedef struct
*             {
*                 uint8_t LinkMode;     //���ӷ�ʽ
*                 uint16_t linkmaxtime; //�������ʱ��
*             } iot_foo_type;
*
*	�� �� ֵ: �ֵ�Faildo�ж����ֵ������鿴��Ӧ��ģ�麯��
*	��    ע����
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

    /* �ٴ��ж�һ���Ƿ���δ����յ��ڴ� */
    DelComMessage(iotQuene);

    memset(&iot, 0, sizeof(iot));
    return result;
}
/*
*********************************************************************************************************
*	�� �� ��: iot_init
*	����˵��: ��ʼ��ʱ���ã����ĺ���
*	��    ��: iot_foo_type *FOO��iot_foo_type�ͽṹ��ָ�����
*             iot_net_type *NET��iot_net_type�ͽṹ��ָ�����
*             typedef struct
*             {
*                 char *Ip;   //ָ��IP�����ָ��
*                 char *Port; //ָ��˿ڻ����ָ��
*                 char *Apn;  //ָ��APN�����ָ��
*             } iot_net_type;
*
*	�� �� ֵ: 0��������������ֵ���ֵ�Faildo�ж����ֵ������鿴��Ӧ��ģ�麯��
*	��    ע��ȷ��IP�Ͷ˿ڵĴ������ݸ�ʽ��ȷ������ȷ�Ļ����Ͳ��������������
*********************************************************************************************************
*/
uint8_t iot_start(iot_foo_type *FOO, iot_net_type *NET)
{
    if ((NET->Ip == NULL) || (NET->Port == NULL))
        return 0;

    /* �����ip���߶˿���һ�����ԾͲ��ɻ��� */
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

    IPReduceZero(iot.net.Ip);         /* ת��ģ����ʹ�õ�IP */
    PortReduceZero_New(iot.net.Port); /* ת��ģ����ʹ�õ�PORT */
    return iot_init(FOO);
}
/*
*********************************************************************************************************
*	�� �� ��: getiot
*	����˵��: ��ȡiot�ṹ���ַ
*	��    ��: ��
*	�� �� ֵ: iot�ṹ���ַ
*	��    ע�����ڻ�ȡiot�ڲ����ݣ���Ϊiot�Ǿ�̬�����������ⲻ�ɼ�
*********************************************************************************************************
*/
iot_type *getiot(void)
{
    return &iot;
}
/*
*********************************************************************************************************
*	�� �� ��: iotState
*	����˵��: ��ȡiot.core.iotStart��ֵ
*	��    ��: ��
*	�� �� ֵ: 0��δ��������״̬��1����������״̬
*	��    ע����������ǰ���øú����ж�һ���Ƿ��Ѿ���������״̬
*********************************************************************************************************
*/
uint8_t iotState(void)
{
    return iot.core.iotStart;
}
/*
*********************************************************************************************************
*	�� �� ��: iotTxData
*	����˵��: ���ݷ��ͺ���
*	��    ��: *buf�����ݻ���
*             len������
*	�� �� ֵ: ��
*	��    ע������Ҫ��������ʱ�����ô˺���
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
*	�� �� ��: iotRxData
*	����˵��: ���ݽ��ս�������
*	��    ��: iot_type *iot��iot_type�ͽṹ��ָ��
*             *word��ascii�͵��ַ���
*	�� �� ֵ: �����������ݳ��ȣ�����������ɵ����ݷŵ�iot->core.rxbuf��
*	��    ע����ο�iotRxDeal(void)�����е����ӣ�ʹ�÷���Ϊ��len = _iotRxData("68");
*            _iotRxData�Ǹ��궨�壬ԭ��Ϊ��iotRxData(getiot(), word)������"68"�ǹؼ��֣�Ҫ���յ����ݵ����ֽ�
*            ������ע�⣺�ؼ��ֱ�����'0'-'9','a'-'f','A'-'F',�ؼ��ֲ�Ҫ����5�ֽڣ�"68"��һ���ֽ�
*            ��Ҫ��ȡ�����������ֱ��ʹ�ú궨�壺IOTRXBUF
*            �ú궨��ԭ��Ϊ��getiot()->core.rxbuf ��iot->core.rxbuf
*            ����char buf[len]; memcpy(buf,IOTRXBUF,len);����ֱ�ӵ��á�
*            ������ע�⣺�ú�������������������ʹ�ã���������� __weak uint8_t iotRxDeal(void) ��
*                       ��Ϊiot->core.rxbuf�е��������뿪�ú�����ͻᱻ�ͷŵ�        
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
        if (strnstr(dat.loc, word, wordlen, 10 + wordlen)) /* ��10���ֽ�֮���ҵ��ؼ��� */
        {
            AsciiToHex((uint8_t *)strstr(dat.loc, word), (uint8_t *)iot->core.rxbuf, dat.len);
            return dat.len;
        }
        else
        {
            uint8_t i = 0;
            char nword[10] = {0};
            /* ʵ�ֽ��ؼ��ִ�Сдת�� */
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
            if (strnstr(dat.loc, nword, wordlen, 10 + wordlen)) /* ��10���ֽ�֮���ҵ��ؼ��� */
            {
                AsciiToHex((uint8_t *)strstr(dat.loc, nword), (uint8_t *)iot->core.rxbuf, dat.len);
                return dat.len;
            }
            else
            {
                /* ʵ�ֽ��ؼ���ת��hex�� */
                uint8_t hexword[5] = {0}, tt = 0;
                AsciiToHex((uint8_t *)word, hexword, wordlen >> 1);

                for (tt = 0; tt < 10; tt++) /* ��10���ֽ�֮���ҵ��ؼ��� */
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
*	�� �� ��: iotEnd
*	����˵��: ��������
*	��    ��: iot_type *iot��iot_type�ͽṹ��ָ��
*	�� �� ֵ: �̶�2
*	��    ע���ֵ�ʹ��       
*********************************************************************************************************
*/
uint8_t iotEnd(iot_type *iot)
{
    iot->core.iotStart = 0;
    return 2;
}
/*
*********************************************************************************************************
*	�� �� ��: iotRst
*	����˵��: ��λ����
*	��    ��: iot_type *iot��iot_type�ͽṹ��ָ��
*	�� �� ֵ: 0��ʧ�ܣ�1���ɹ�
*	��    ע���ֵ�ʹ��       
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
*	�� �� ��: iotRead
*	����˵��: �ֵ����ݶ�ȡ
*	��    ��: iot_type *iot��iot_type�ͽṹ��ָ��
*	�� �� ֵ: 0��ʧ�ܣ�1���ɹ�
*	��    ע���ֵ�ʹ��       
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
*	�� �� ��: iotRead
*	����˵��: �ֵ����ݷ���
*	��    ��: iot_type *iot��iot_type�ͽṹ��ָ��
*	�� �� ֵ: 0��ʧ�ܣ�2������
*	��    ע���ֵ�ʹ��       
*********************************************************************************************************
*/
uint8_t iotSend(iot_type *iot)
{
    if (iot->core.sendcount++ < 3)
    {
        /* �������� */
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
*	�� �� ��: iotTim
*	����˵��: �ֵ���ļ�ʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����������붨ʱ�����棬�����޷���������     
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
*	�� �� ��: iotTim
*	����˵��: �ֵ���ĺ��� 
*	��    ��: cmd_type *cmd���ֵ�
*             len���ֵ䳤��
*             iot_type *iot��iot_type�ͽṹ��ָ��
*	�� �� ֵ: ʧ�ܲ������
*	��    ע�������ֵ�ĺ���    
*********************************************************************************************************
*/
static char *match_cmd(cmd_type *cmd, uint8_t len, iot_type *iot)
{
    volatile uint8_t i = 0, j = 0;

    if (mystrlen(iot->core.execute, MAX_CMD_NAME_LENGTH + 1) > MAX_CMD_NAME_LENGTH) /* ���뱣ִ֤�����������޳��� */
    {
        iot->core.iotStart = 0;
        return NULL;
    }

    for (i = 0; i < len; i++) /* ���������б� */
    {
        if (strcmp(cmd[i].name, iot->core.execute) == 0)
        {
            for (j = 0; j < cmd[i].recount; j++) /* �ظ����� */
            {
                uint32_t timout = 0, timmem = 0;

                /* ���ʹ������ */
                if (cmd[i].atcommand != NULL) /* �ж�Ӧ��AT���� */
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
                            return cmd[i].name; /* ����ʧ�ܲ���������� */
                        }
                        else
                        {
                            /* ɶҲ���� */
                        }
                    }
                }
                iot->core.timeout = cmd[i].timeout;
                timmem = cmd[i].timeout;
                while (iot->core.timeout != 0) /* ��δ��ʱ */
                {
#if Use_uCOS > 0
                    OS_ERR err;
                    OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_DLY, &err);
#elif Use_Event > 0
                    WEIGOU;
                    ExecuteEvent;
#endif
                    /* ���մ������ */
                    if (!QueueIsEmpty(iotQuene)) /* �ж϶����Ƿ�λ��,��Ϊ����һ�������� */
                    {
                        ComData *data = GetComMessage(iotQuene); /* �ڶ����л�ȡδ���������� */
                        iot->core.rxbuf = (char *)data->pData;   /* ȡ����������һ��Ҫ�ͷ� */
                        iot->core.rxlen = data->len;
#ifdef TS_Uart
                        API_UART_SEND(TS_Uart, (uint8_t *)iot->core.rxbuf, iot->core.rxlen); /* ������� */
#endif
                        if (cmd[i].sucword != NULL) /* �������ȷ�ϲ�Ϊ�� */
                        {
                            char *result = strstr(iot->core.rxbuf, cmd[i].sucword);
                            memset(iot->core.rxbuf, 0, iot->core.rxlen);
                            myfree(data); /* �ͷ��Ѿ����������� */
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
                                myfree(data); /* �ͷ��Ѿ����������� */
                                if (re == 1)
                                {
                                    iot->core.execute = cmd[i].sucname;
                                    return NULL;
                                }
                                else if (re == 0)
                                {
                                    iot->core.execute = cmd[i].failname;
                                    return cmd[i].name; /* ����ʧ�ܲ���������� */
                                }
                                else
                                {
                                    /* ɶҲ���� */
                                }
                            }
                            else
                            {
                                myfree(data); /* �ͷ��Ѿ����������� */
                                return NULL;
                            }
                        }
                    }
                    else
                    {
                        if (timout++ >= 0xfffff)
                        {
                            if (timmem == iot->core.timeout) /* �ܳ�ʱ���ȥ����ԭ�ȵ�ʱ����ͬ��˵����ʱ������ */
                            {
                                iot->core.execute = cmd[i].failname;
                                return NULL;
                            }
                        }
                    }
                }
            }
            iot->core.execute = cmd[i].exname; /* ���������Դ���Ҫ�������� */
            return cmd[i].name;                /* ����ʧ�ܲ���������� */
        }
    }
    iot->core.iotStart = 0; /* ȫ�������˶�û���ҵ���Ӧ���� */
    return NULL;
}
/*
*********************************************************************************************************
*	�� �� ��: FooSend
*	����˵��: �ֵ���ķ���
*	��    ��: *buff�����ͻ���
*             len������
*	�� �� ֵ: ��
*	��    ע�����º����漰Ӳ���Լ������շ���ز������ⲿ����API_Uart_Send����  
*********************************************************************************************************
*/
static void FooSend(char *buff, uint16_t len)
{
#ifdef TS_Uart
    API_UART_SEND(TS_Uart, (uint8_t *)buff, len); /* ������� */
#endif
    API_UART_SEND(NB_Uart, (uint8_t *)buff, len); /* ģ��ͨѶ */
}
/*
*********************************************************************************************************
*	�� �� ��: iotprintf
*	����˵��: AT�����
*	��    ��: ���
*	�� �� ֵ: ��
*	��    ע���������printfһ�����øú���
*            ����˵����c������һ���ַ�
*                     d������һ������  
*                     s������һ���ַ���
*                     l����ȡ��Ҫ���͵�hex���ݵĳ���
*                     a����hexת��ascii���ڷ��ͣ�ʹ��a����ǰ����ʹ��l����
*                     ���֣����Ʒ��ͳ��ȣ����s����
*********************************************************************************************************
*/
void iotprintf(const char *format, ...)
{
    char c;
    char *p = NULL;
    int len = 0, limlen = 0, a = 0;
    char buf[20] = {0};
    char *sendbuf = NULL;
    va_list ap;           /* ����һ��int *�͵�ָ��ap */
    va_start(ap, format); /* ��ʼ��ap������ָ���һ���ɱ�����ĵ�ַ */

    while ((c = *format++) != '\0') /* ��ʼ����printf�����е��ַ���(����һ���̶�����) */
    {
        switch (c) /* ��while�У����ζ����ַ����е��ַ��������������ؽ����жϺͽ��� */
        {
        case '%':          /* ����ַ���%������ʽ�������ţ�������ø�ʽ */
            c = *format++; /* ��c����%��ĵ�һ����ĸ */
            switch (c)     /* ��������ĸ���н��� */
            {
            case '0': /* ���ڳ������� */
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
            case 'c':                   /* �����%c���������ĸ */
                c = va_arg(ap, int);    /* ��ȡ��ǰ�ɱ������ֵ����ָ����һ���ɱ���� */
                FooSend((char *)&c, 1); /* ���õײ�(ʵ�ʲ���Ӳ�����ǲ�)����������� */
                break;
            case 'l':                  /* �����%l������ֵ���� */
                len = va_arg(ap, int); /* ��ȡ��ǰ�ɱ������ֵ����ָ����һ���ɱ���� */
                break;
            case 's': /*�����%s��������ַ���*/
                p = va_arg(ap, char *);
                FooSend(p, strlen(p));
                break;
            case 'a': /* �����%a��������ת��ascii�����  ��a����ǰ��������%l�ĳ��� */
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
            case 'd': /* �����%d������ʮ������� */
                a = va_arg(ap, int);
                sprintf(buf, "%d", a);
                FooSend(buf, strlen(buf));
                break;
            default:
                break;
            }
            break;
        default:
            FooSend((char *)&c, 1); /* ����ַ�����Ϊ��ͨ�ַ����ַ� */
            break;
        }
    }
    va_end(ap);
}
