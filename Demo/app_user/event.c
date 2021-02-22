#include "define_all.h"
#include "minilzo.h"
#include "quicklz.h"
#include "gzip.h"

extern void Refresh(void);
extern uint8_t showemu;

uchar ToCheckSum(unsigned char *parray, unsigned int size, unsigned char op)
{
    uint i;
    char sum = 0;
    for (i = 0; i < size; i++)
    {
        sum += parray[i];
    }
    if (op)
    {
        if (sum != parray[size])
            sum = 0;
        else
            sum = 1;
    }
    return (sum);
}

uint8_t NBTxBuf[72] = {NULL};
void NBTxDeal(void)
{
    uint16_t VersionNum = 100;
    uint16_t year;
    uint8_t Systemtime[6];
    //uint8_t *NBTxBuf = (uint8_t *)mymalloc(100);
    //if (NBTxBuf != NULL)
    {
        memset(NBTxBuf, 0, 72);
        NBTxBuf[0] = 0xaa; //����ͷ(��ʼ����)
        NBTxBuf[1] = 0x11;
        NBTxBuf[2] = 0xC5;
        NBTxBuf[3] = 0x11;
        NBTxBuf[4] = 0x22;
        NBTxBuf[5] = 0x33;
        NBTxBuf[6] = 0x44;

        NBTxBuf[7] = 0xAA; //������

        Systemtime[0] = Hex_Sec;
        Systemtime[1] = Hex_Min;
        Systemtime[2] = Hex_Hour;
        Systemtime[3] = Hex_Day;
        year = Hex_YearL + 2000;
        Systemtime[4] = LOWBYTE(LOWWORD(year)) * 16 + Hex_Mon;
        Systemtime[5] = HIGHWORD(year) * 16 + HIGHBYTE(LOWWORD(year));
        memcpy(&NBTxBuf[8], Systemtime, 6); //ʵʱʱ�� 6B
        NBTxBuf[14] = 1;                    //ֻȡ������
        NBTxBuf[15] = 1;                    //�ϴ�ԭ��

        memset(&NBTxBuf[16], 0, 12); //�ۼ���

        NBTxBuf[28] = 0;    //��������״̬
        NBTxBuf[29] = 0x31; //�ź�ǿ��
        memset(&NBTxBuf[30], 0, 49);
        //memcpy(&NBTxBuf[57], iotdat.Simword, 10);	   //sim����
        memcpy(&NBTxBuf[67], (uchar *)&VersionNum, 2); //����汾��
        NBTxBuf[69] = 0;
        NBTxBuf[70] = ToCheckSum(NBTxBuf, 70, 0);
        NBTxBuf[71] = 0xbb;

        iotTxData(NBTxBuf, 72);
        //myfree(NBTxBuf);
    }
}

#define uCOS_TaskNB_PRIO 6
#define uCOS_TaskNB_STK_SIZE 256

char NB_APN[10];
char NB_IP[15];
char NB_PORT[5];
iot_foo_type FOO;
uint8_t notintosleep = 0;
void uCOS_TaskNB(void *p_arg)
{
    uint8_t aaa;

    iot_net_type NET;
    notintosleep = 1;
    NB_ON;
    memcpy(NB_APN, "CMIOT", 5);
    if (FOO.LinkMode == CoapNum)
    {
        memcpy(NB_PORT, "5683", 5);
        memcpy(NB_IP, "117.60.157.137", 15);
    }
    else
    {
        memcpy(NB_PORT, "3380", 5);
        memcpy(NB_IP, "60.21.136.130", 14);
    }
    NET.Ip = (char *)NB_IP;     // ������ascii��С�����IP��ַ
    NET.Port = (char *)NB_PORT; // ������ascii�Ķ˿�
    NET.Apn = (char *)NB_APN;
    //FOO.LinkMode = CoapNum;
    FOO.linkmaxtime = vLinkMaxTime;

    aaa = iot_start(&FOO, &NET);
    aaa++;
    NB_OFF;
    notintosleep = 0;
    MyTaskDel(&TaskTempTCB);
}

void uCOS_NBTaskCreate(void)
{
    OS_ERR os_err;
    os_err = os_err;
    MyTaskCreate((OS_TCB *)&TaskTempTCB,
                 (CPU_CHAR *)"TaskNB",
                 (OS_TASK_PTR)uCOS_TaskNB,
                 (OS_PRIO)uCOS_TaskNB_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskNB_STK_SIZE);
}

//#define IN_LEN (128 * 1024ul)
#define OUT_LEN(IN_LEN) (IN_LEN + IN_LEN / 16 + 64 + 3)

//static unsigned char __LZO_MMODEL in[IN_LEN];
//static unsigned char __LZO_MMODEL out[OUT_LEN];

/* Work-memory needed for compression. Allocate memory in units
 * of 'lzo_align_t' (instead of 'char') to make sure it is properly aligned.
 */

#define HEAP_ALLOC(var, size) \
    lzo_align_t __LZO_MMODEL var[((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t)]
//static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);

#define IN_LEN 450
unsigned char in[IN_LEN];
uint8_t asd[15] = {3, 6, 2, 1, 9, 5, 78, 15, 98, 10, 61, 18, 97, 15, 64};

//void jiamitest3(uint8_t *buf, uint16_t len)
//{
//    char *out;
//    uint16_t len1, len2;
//    //printf("\r\n���ܲ���3��ʼ\r\n");
//    out = (char *)mymalloc(len + 100);
//    len1 = zipmem((char *)buf, len, out);
//    //printf("\r\nѹ�����,ѹ�������ݴ�СΪ:%d\r\n", len1);
//    //printf("\nѹ����Ϊ: %.2lf%c\n", (double)((len - len1) / (double)len) * 100.0, '%');
//    memset(buf, 0, len);
//    len2 = unzipmem(out, len1, (char *)buf);
//    //printf("��ѹ�յ�����Ϊ��");
//    for (len1 = 0; len1 < len2; len1++)
//        printf("%x", buf[len1]);
//    //printf("\r\n���ܲ���3����\r\n");
//}

void jiamitest2(uint8_t *buf, uint16_t len)
{
    char *compressed;
    uint16_t c, d;
    printf("\r\n���ܲ���2��ʼ\r\n");
    qlz_state_compress *state_compress = (qlz_state_compress *)mymalloc(sizeof(qlz_state_compress));

    // allocate "uncompressed size" + 400 bytes for the destination buffer where
    // "uncompressed size" = 10000 in worst case in this sample demo
    compressed = (char *)mymalloc(len + 100);

    // allocate and initially zero out the states. After this, make sure it is
    // preserved across calls and never modified manually
    memset(state_compress, 0, sizeof(qlz_state_compress));

    d = qlz_compress(buf, compressed, len, state_compress);
    printf("\nѹ����Ϊ: %.2f%c\n", (float)((len - d) / (float)len) * 100.0, '%');
    myfree(state_compress);

    qlz_state_decompress *state_decompress = (qlz_state_decompress *)mymalloc(sizeof(qlz_state_decompress));

    c = qlz_decompress(compressed, buf, state_decompress);

    printf("%u bytes decompressed into %u.\n", (unsigned int)d, (unsigned int)c);
    printf("��ѹ�������Ϊ��");
    for (uint16_t j = 0; j < c; j++)
        printf("%x", buf[j]);
    myfree(compressed);
    myfree(state_decompress);
    printf("\r\n���ܲ���2����\r\n");
}

void jiamitest(uint8_t *buf, uint16_t len)
{
    int r;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    unsigned char *out = NULL;
    lzo_align_t *wrkmem = NULL;

    out = mymalloc(OUT_LEN(len));
    wrkmem = mymalloc((lzo_uint32_t)(256 * lzo_sizeof_dict_t));
    if (wrkmem == NULL || out == NULL)
        return;

    printf("\nLZO real-time data compression library (v%s, %s).\n",
           lzo_version_string(), lzo_version_date());
    printf("Copyright (C) 1996-2017 Markus Franz Xaver Johannes Oberhumer\nAll Rights Reserved.\n\n");

    if (lzo_init() != LZO_E_OK)
    {
        printf("internal error - lzo_init() failed !!!\n");
        printf("(this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
        myfree(out);
        myfree(wrkmem);
        return;
    }

    in_len = len;
    r = lzo1x_1_compress(buf, in_len, out, &out_len, wrkmem);
    if (r == LZO_E_OK)
    {
        printf("\nѹ����Ϊ: %.2f%c\n", (float)((in_len - out_len) / (float)in_len) * 100.0, '%');
    }
    else
    {
        /* this should NEVER happen */
        printf("internal error - compression failed: %d\n", r);
        myfree(out);
        myfree(wrkmem);
        return;
    }
    /* check for an incompressible block */
    if (out_len >= in_len)
    {
        printf("This block contains incompressible data.\n");
        myfree(out);
        myfree(wrkmem);
        return;
    }

    new_len = in_len;
    memset(buf, 0, len);
    r = lzo1x_decompress_safe(out, out_len, buf, &new_len, NULL);
    if (r == LZO_E_OK && new_len == in_len)
    {
        printf("decompressed %lu bytes back into %lu bytes\n",
               (unsigned long)out_len, (unsigned long)in_len);
        printf("��ѹ�������Ϊ��");
        for (uint16_t j = 0; j < new_len; j++)
            printf("%x", buf[j]);
    }
    else
    {
        /* this should NEVER happen */
        printf("internal error - decompression failed: %d\n", r);
        myfree(out);
        myfree(wrkmem);
        return;
    }

    printf("\nminiLZO simple compression test passed.\n");
    myfree(out);
    myfree(wrkmem);
}

void uCOS_Taskyasuo(void *p_arg)
{
    int r;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    unsigned char *out = NULL;
    lzo_align_t *wrkmem = NULL;
    out = mymalloc(OUT_LEN(450));
    wrkmem = mymalloc((lzo_uint32_t)(256 * lzo_sizeof_dict_t));
    if (wrkmem == NULL || out == NULL)
    {
        myfree(out);
        myfree(wrkmem);
        return;
    }

    printf("\nLZO real-time data compression library (v%s, %s).\n",
           lzo_version_string(), lzo_version_date());
    printf("Copyright (C) 1996-2017 Markus Franz Xaver Johannes Oberhumer\nAll Rights Reserved.\n\n");

    if (lzo_init() != LZO_E_OK)
    {
        printf("internal error - lzo_init() failed !!!\n");
        printf("(this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
        myfree(out);
        myfree(wrkmem);
        MyTaskDel(&TaskTempTCB);
        return;
    }

    in_len = IN_LEN;
    for (uint8_t aaa = 0; aaa < 30; aaa++)
        memcpy(in + aaa * 15, asd, 15);

    r = lzo1x_1_compress(in, in_len, out, &out_len, wrkmem);
    if (r == LZO_E_OK)
        printf("\nѹ����Ϊ: %.2f%c\n", (float)((in_len - out_len) / (float)in_len) * 100.0, '%');
    else
    {
        /* this should NEVER happen */
        printf("internal error - compression failed: %d\n", r);
        myfree(out);
        myfree(wrkmem);
        MyTaskDel(&TaskTempTCB);
        return;
    }
    /* check for an incompressible block */
    if (out_len >= in_len)
    {
        printf("This block contains incompressible data.\n");
        myfree(out);
        myfree(wrkmem);
        MyTaskDel(&TaskTempTCB);
        return;
    }

    new_len = in_len;
    memset(in, 0, 450);
    r = lzo1x_decompress_safe(out, out_len, in, &new_len, NULL);
    if (r == LZO_E_OK && new_len == in_len)
        printf("decompressed %lu bytes back into %lu bytes\n",
               (unsigned long)out_len, (unsigned long)in_len);
    else
    {
        /* this should NEVER happen */
        printf("internal error - decompression failed: %d\n", r);
        myfree(out);
        myfree(wrkmem);
        MyTaskDel(&TaskTempTCB);
        return;
    }

    printf("\nminiLZO simple compression test passed.\n");
    myfree(out);
    myfree(wrkmem);
    MyTaskDel(&TaskTempTCB);
}

void yasuotest(void)
{
    OS_ERR os_err;
    os_err = os_err;
    MyTaskCreate((OS_TCB *)&TaskTempTCB,
                 (CPU_CHAR *)"Taskyasuo",
                 (OS_TASK_PTR)uCOS_Taskyasuo,
                 (OS_PRIO)uCOS_TaskNB_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskNB_STK_SIZE);
}

OS_TCB TaskLEDTCB;
#define uCOS_TaskLED_PRIO 5
#define uCOS_TaskLED_STK_SIZE 256

const GUI_POINT showBattery[] = {{0, 0}, {8, 0}, {8, 1}, {10, 1}, {10, 3}, {8, 3}, {8, 4}, {0, 4}};
const GUI_POINT showsign1[] = {{0, 0}, {2, 0}, {2, -1}, {0, -1}};
const GUI_POINT showsign2[] = {{0, 0}, {2, 0}, {2, -2}, {0, -2}};
const GUI_POINT showsign3[] = {{0, 0}, {2, 0}, {2, -4}, {0, -4}};

typedef struct
{
    uint8_t curloc;
    uint8_t isfull;
    float temp[100];
    float pre[100];
} table_type;
table_type table;

float getTemporPre(uint8_t getwhitch, uint8_t *buf)
{
    unsigned long int temp;
    float TemporPre;
    if (getwhitch)
    {
        temp = (((buf[0] << 8) + buf[1]) << 8) + buf[2];
        TemporPre = (float)temp / 8000;
    }
    else
    {
        if ((buf[3] & 0x80) == 0x00)
        {
            temp = (buf[3] << 8) + buf[4];
            TemporPre = (float)temp / 256.0;
        }
        else
        {
            temp = ((127 - (buf[3] & 0x7F)) << 8) + buf[4];
            TemporPre = 0.0 - (float)temp / 256.0;
        }
    }
    return TemporPre;
}

void PressureAndTempTest(void)
{
    uint8_t cmd = 0x0a, flag = 0;
    uint8_t buf[5];
    API_SIMLT_I2C_Write_Com(0xDA, 0x30, 1, &cmd, 1);
    Do_DelayStart();
    {
        API_SIMLT_I2C_Read_Com(0xDA, 0x2, 1, &cmd, 1);
        if (cmd & 0x1)
            flag = 1; /* ���õ�ַ�ɹ�, д���ڽ��� */
    }
    While_DelayMsEnd(8 * clkmode);
    if (flag)
    {
        API_SIMLT_I2C_Read_Com(0xDA, 0x6, 1, buf, 5);
        /* 0-2Ϊѹ������ˣ����λΪ����λ����6λΪС��λ */
        /* 3-4Ϊ�¶ȣ���ˣ����λΪ����λ����8λΪС��λ */
        float Pressure, Temp;

        printf("�ɼ������¶�Ϊ��%.3f�棬ѹ��Ϊ��%.3fKPa\r\n", Temp = getTemporPre(0, buf), Pressure = getTemporPre(1, buf));

        table.temp[table.curloc] = Temp;
        table.pre[table.curloc] = Pressure;
        table.curloc++;
        if (table.isfull == 0)
        {
            if (table.curloc == 100)
            {
                table.isfull = 1;
            }
        }
        table.curloc %= 100;
    }
}

void compute_m(float arr[], int len, float *min, float *max)
{
    for (int i = 0; i < len; i++)
    {
        if (*min > arr[i])
        {
            *min = arr[i];
        }

        if (*max < arr[i])
        {
            *max = arr[i];
        }
    }
}

void showtemptable()
{
    const uint8_t yy = 31, xx = 15, ylen = 15, xlen = 103;
    uint8_t i, accuracy = 1, cnt;
    int integer;
    float tmax = -99, tmin = 99;
    PressureAndTempTest();
    /* ������ */
    GUI_DrawLine(xx, yy - ylen - 3, xx, yy + ylen); /* y�� */
    GUI_DrawLine(xx, yy - ylen - 3, xx - 2, yy - ylen - 3 + 2);
    GUI_DrawLine(xx, yy - ylen - 3, xx + 2, yy - ylen - 3 + 2);
    GUI_DrawLine(xx, yy, xx + xlen, yy); /* x�� */
    GUI_DrawLine(xx + xlen, yy, xx + xlen - 2, yy + 2);
    GUI_DrawLine(xx + xlen, yy, xx + xlen - 2, yy - 2);

    GUI_SetTextMode(GUI_TEXTMODE_REV); //����
    GUI_DispStringAt(".C", 116, 10);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

    if (table.isfull)
    {
        /* ԭ��ֵ */
        compute_m(table.temp, 100, &tmin, &tmax);
        integer = (int)((tmin + tmax) / 2);
        GUI_DispDecAt(integer, 3, yy - 3, 2);

        for (accuracy = 1; accuracy < 100; accuracy++)
            if (((int)((tmax - integer) * 100) < accuracy * ylen) && ((int)((integer - tmin) * 100) < accuracy * ylen))
                break;

        GUI_DispStringAt("15*0.", 88, 11);
        GUI_DispDec(accuracy, 2); /* ��ʾ���� */

        for (i = 0, cnt = table.curloc; i < 100; i++, cnt++)
        {
            cnt %= 100;
            GUI_DrawLine(xx + i, yy, xx + i, yy - (int)((table.temp[cnt] - integer) * 100) / accuracy);
        }
    }
    else
    {
        compute_m(table.temp, table.curloc, &tmin, &tmax);
        integer = (int)((tmin + tmax) / 2);
        GUI_DispDecAt(integer, 3, 28, 2);

        for (accuracy = 1; accuracy < 100; accuracy++)
            if (((int)((tmax - integer) * 100) < accuracy * ylen) && ((int)((integer - tmin) * 100) < accuracy * ylen))
                break;

        GUI_DispStringAt("15*0.", 88, 11);
        GUI_DispDec(accuracy, 2); /* ��ʾ���� */
        for (i = 0, cnt = 0; i < table.curloc; i++, cnt++)
        {
            cnt %= 100;
            GUI_DrawLine(xx + i, yy, xx + i, yy - (int)((table.temp[cnt] - integer) * 100) / accuracy);
        }
    }
    GUI_DispStringAt(" T:", 3, 48);
    GUI_DispFloat(table.temp[(table.curloc - 1) % 100], 5);
    GUI_DispString(" maxT:");
    GUI_DispFloat(tmax, 5);
    GUI_DispString(" minT:");
    GUI_DispFloat(tmin, 5);
}

void showpretable()
{
    const uint8_t yy = 45, xx = 15, ylen = 30, xlen = 103;
    uint8_t i, cnt;
    uint32_t accuracy = 1; /* 0.01kpa */
    int integer;
    float tmax = 0, tmin = 9999;
    PressureAndTempTest();
    /* ������ */
    GUI_DrawLine(xx, yy - ylen - 3, xx, yy); /* y�� */
    GUI_DrawLine(xx, yy - ylen - 3, xx - 2, yy - ylen - 3 + 2);
    GUI_DrawLine(xx, yy - ylen - 3, xx + 2, yy - ylen - 3 + 2);
    GUI_DrawLine(xx, yy, xx + xlen, yy); /* x�� */
    GUI_DrawLine(xx + xlen, yy, xx + xlen - 2, yy + 2);
    GUI_DrawLine(xx + xlen, yy, xx + xlen - 2, yy - 2);

    GUI_SetTextMode(GUI_TEXTMODE_REV); //����
    GUI_DispStringAt(".Kp", 112, 10);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

    if (table.isfull)
    {
        /* ԭ��ֵ */
        compute_m(table.pre, 100, &tmin, &tmax);
        integer = (int)tmin;
        GUI_DispDecAt(integer, 3, yy - 3, 3);

        for (accuracy = 1; accuracy < 999999; accuracy++) /* 99������ѹ */
            if ((int)((tmax - integer) * 100) < accuracy * ylen)
                break;
        if (accuracy < 99)
        {
            GUI_DispStringAt("30*0.", 84, 11);
            GUI_DispDec(accuracy, 2); /* ��ʾ���� */
        }
        else
        {
            uint32_t q = accuracy / 100, ty;
            for (i = 0; i < 4; i++)
            {
                if (q / 10 != 0)
                {
                    ty = q % 10;
                    GUI_DispDecAt(ty, 96 - i * 4, 11, 1);
                }
                else
                {
                    break;
                }
            }
            GUI_DispStringAt("30*", 84 - i * 4, 11);
            q = accuracy % 100;
            GUI_DispStringAt(".", 100, 11);
            GUI_DispDecAt(q, 104, 11, 2);
        }

        for (i = 0, cnt = table.curloc; i < 100; i++, cnt++)
        {
            cnt %= 100;
            GUI_DrawLine(xx + i, yy, xx + i, yy - (int)((table.pre[cnt] - integer) * 100) / accuracy);
        }
    }
    else
    {
        compute_m(table.pre, table.curloc, &tmin, &tmax);
        integer = (int)tmin;
        GUI_DispDecAt(integer, 3, yy - 3, 3);

        for (accuracy = 1; accuracy < 999999; accuracy++) /* 99������ѹ */
            if ((int)((tmax - integer) * 100) < accuracy * ylen)
                break;
        if (accuracy < 99)
        {
            GUI_DispStringAt("30*0.", 84, 11);
            GUI_DispDec(accuracy, 2); /* ��ʾ���� */
        }
        else
        {
            uint32_t q = accuracy / 100, ty;
            for (i = 0; i < 4; i++)
            {
                if (q / 10 != 0)
                {
                    ty = q % 10;
                    GUI_DispDecAt(ty, 96 - i * 4, 11, 1);
                }
                else
                {
                    break;
                }
            }
            GUI_DispStringAt("30*", 84 - i * 4, 11);
            q = accuracy % 100;
            GUI_DispStringAt(".", 100, 11);
            GUI_DispDecAt(q, 104, 11, 2);
        }
        for (i = 0, cnt = 0; i < table.curloc; i++, cnt++)
        {
            cnt %= 100;
            GUI_DrawLine(xx + i, yy, xx + i, yy - (int)((table.pre[cnt] - integer) * 100) / accuracy);
        }
    }
    GUI_DispStringAt(" P:", 3, 48);
    GUI_DispFloat(table.pre[(table.curloc - 1) % 100], 5);
    GUI_DispString(" maxP:");
    GUI_DispFloat(tmax, 5);
    GUI_DispString(" minP:");
    GUI_DispFloat(tmin, 5);
}

extern uint32_t sysvdd;
extern float systemp;
void showgui()
{
    uint32_t vdd = sysvdd;
    float temp = systemp;

    GUI_FillPolygon(showBattery, ARRAY_SIZE(showBattery), 115, 1);
    if (vdd < 3600)
    {
        vdd = 3600 - vdd;
        GUI_InvertRect(122 - ((vdd / 100) % 6), 2, 122, 4);
    }

    GUI_FillPolygon(showsign1, ARRAY_SIZE(showsign1), 102, 5);
    GUI_FillPolygon(showsign2, ARRAY_SIZE(showsign2), 106, 5);
    GUI_FillPolygon(showsign3, ARRAY_SIZE(showsign3), 110, 5);
    GUI_SetFont(&GUI_Font4x6); //���������С
    GUI_DispDecAt(Hex_YearH, 1, 1, 2);
    GUI_DispDec(Hex_YearL, 2);
    GUI_DispString("-");
    GUI_DispDec(Hex_Mon, 2);
    GUI_DispString("-");
    GUI_DispDec(Hex_Day, 2);
    GUI_DispString(" ");
    GUI_DispDec(Hex_Hour, 2);
    GUI_DispString(":");
    GUI_DispDec(Hex_Min, 2);
    GUI_DispString(":");
    GUI_DispDec(Hex_Sec, 2);

    GUI_SetTextMode(GUI_TEXTMODE_REV); //����
    GUI_DispStringAt(".R", 80, 0);     /* ������ʾ��λ */
    GUI_DispStringAt(".E", 90, 0);     /* ������ʾ�洢���� */
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

    GUI_DispStringAt("CPU:", 1, 57);
    GUI_DispFloat((float)OSStatTaskCPUUsage / 100.0, 4);
    GUI_DispString("%  Ram:");
    GUI_DispDecSpace(mem_perused(), 3);
    GUI_DispString("%  Temp:");
    GUI_DispFloat(temp, 5);

    GUI_SetPenSize(2);          //�����߿�
    GUI_DrawLine(0, 7, 128, 7); /* ���� */
    GUI_DrawLine(0, 7, 0, 54);
    GUI_DrawLine(0, 54, 128, 54);
    GUI_DrawLine(126, 7, 126, 54);
    GUI_SetPenSize(1);

    Refresh();
}

void showclock(void)
{
    const uint16_t xx = 103, yy = 35;
    GUI_SetDrawMode(GUI_TEXTMODE_REV);
    GUI_FillEllipse(xx, yy, 20, 15);
    GUI_SetDrawMode(GUI_TEXTMODE_NORMAL); //���������

    GUI_DrawEllipse(xx, yy, 20, 15);
    //GUI_SetFont(&GUI_Font4x6); //���������С
    int x, y;
    float a, pi;
    for (char i = 1; i < 13; i++)
    {
        char ac[4];
        a = (90 + i * 30) * 3.1415926 / 180;
        //		x = -23 * cos(a) + 102;
        //		y = -17 * sin(a) + 45;

        x = -17 * cos(a) + xx + 2;
        y = -12 * sin(a) + yy + 1;
        GUI_SetTextAlign(GUI_TA_VCENTER);

        sprintf(ac, "%d", i);
        GUI_DispStringHCenterAt(ac, x, y);
    }
    //GUI_SetFont(&GUI_Font6x8); //�ָ������С

    if (Hex_Sec < 30)
    {
        pi = 3.1415927;
    }
    else
    {
        pi = 3.1415926;
    }
    a = (Hex_Sec * 6 + 90) * pi / 180;
    x = -16 * cos(a) + xx;
    y = -12 * sin(a) + yy;
    GUI_DrawLine(xx, yy, x, y); //���� ʱ��

    if (Hex_Min < 30)
    {
        pi = 3.1415927;
    }
    else
    {
        pi = 3.1415926;
    }
    GUI_SetPenSize(2); //�����߿�
    a = (Hex_Min * 6 + 90) * pi / 180;
    x = -14 * cos(a) + xx;
    y = -11 * sin(a) + yy;
    GUI_DrawLine(xx, yy, x, y); //���� ����

    if (Hex_Hour < 30)
    {
        pi = 3.1415927;
    }
    else
    {
        pi = 3.1415926;
    }
    a = (Hex_Hour % 12 * 5 * 6 + 90) * pi / 180;
    x = -10 * cos(a) + xx;
    y = -7 * sin(a) + yy;

    GUI_DrawLine(xx, yy, x, y); //���� ʱ��
    GUI_SetPenSize(1);
}

static void uCOS_TaskLED(void *p_arg)
{
    OS_ERR err;
    while (1)
    {
        GUI_Clear();
        if (showemu == 0)
        {
            showtemptable();
        }
        else if (showemu == 1)
        {
            showpretable();
        }
        else
        {
            GUI_SetFont(&GUI_FontD24x32); //���������С
            GUI_DispDecAt(Hex_Hour, 3, 16, 2);
            GUI_DispString(":");
            GUI_DispDec(Hex_Min, 2);
            GUI_SetFont(&GUI_Font4x6); //���������С
        }

        showgui();
        OSTimeDlyHMSM(0, 0, 0, 15, OS_OPT_TIME_HMSM_NON_STRICT, &err);
    }
}

void uCOS_LEDCreate(void)
{
    OS_ERR os_err;
    os_err = os_err;

    MyTaskCreate((OS_TCB *)&TaskLEDTCB,
                 (CPU_CHAR *)"TaskLCD",
                 (OS_TASK_PTR)uCOS_TaskLED,
                 (OS_PRIO)uCOS_TaskLED_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskLED_STK_SIZE);
}

void SIMLT_I2C_OP_INIT_EX(uint8_t selet)
{
    MONI_IIC_SDA_DIR_OUT;
    MONI_IIC_SCL_DIR_OUT;
    SIMLT_IIC.sda_set = iic_op_sda_set;
    SIMLT_IIC.sda_out = iic_op_sda_out;
    SIMLT_IIC.sda_in = iic_op_sda_in;
    SIMLT_IIC.scl = iic_op_scl;
}

typedef struct __minalgt
{
    uint8_t state;
    uint32_t timestamp;
    uint8_t buf[20];
} collection_data_type;

typedef struct __minalgt
{
    uint8_t isloop;
    uint16_t end;
    uint16_t lock;
    uint16_t start;
    uint16_t step;
    uint32_t frequency; /* ��λ�� */
} data_save_type;
data_save_type data_save;
collection_data_type collection_data;

#define ONEPACKMAX 400
#define savebase 100
#define MAXSAVE 5000
#define DATALEN sizeof(collection_data_type)

void SaveCore(void)
{
    struct tm stm = {0};
    uint8_t cmd = 0x0a, flag = 0;
    volatile uint8_t i;
    memset(&collection_data, 0, DATALEN);

    stm.tm_year = Hex_YearL + 100;
    stm.tm_mon = Hex_Mon - 1;
    stm.tm_mday = Hex_Day;
    stm.tm_hour = Hex_Hour;
    stm.tm_min = Hex_Min;
    stm.tm_sec = Hex_Sec;
    collection_data.timestamp = mktime(&stm);

    for (i = 0; i < 4; i++)
    {
        SIMLT_I2C_OP_INIT_EX(i);
        API_SIMLT_I2C_Write_Com(0xDA, 0x30, 1, &cmd, 1);
        Do_DelayStart();
        {
            API_SIMLT_I2C_Read_Com(0xDA, 0x2, 1, &cmd, 1);
            if (cmd & 0x1)
                flag = 1; /* ���õ�ַ�ɹ�, д���ڽ��� */
        }
        While_DelayMsEnd(8 * clkmode);
        if (flag)
        {
            flag = 0;
            collection_data.state <<= 1;
            API_SIMLT_I2C_Read_Com(0xDA, 0x6, 1, &collection_data.buf[5 * i], 5);

            /* 0-2Ϊѹ������ˣ����λΪ����λ����6λΪС��λ */
            /* 3-4Ϊ�¶ȣ���ˣ����λΪ����λ����8λΪС��λ */
            unsigned long int a_pre, T_mid;
            float Pressure, Temp;
            a_pre = (((collection_data.buf[0 + 5 * i] << 8) + collection_data.buf[1 + 5 * i]) << 8) + collection_data.buf[2 + 5 * i];
            Pressure = (float)a_pre / 8;
            if ((collection_data.buf[3 + 5 * i] & 0x80) == 0x00)
            {
                T_mid = (collection_data.buf[3 + 5 * i] << 8) + collection_data.buf[4 + 5 * i];
                Temp = (float)T_mid / 256.0;
            }
            else
            {
                T_mid = ((127 - (collection_data.buf[3 + 5 * i] & 0x7F)) << 8) + collection_data.buf[4 + 5 * i];
                Temp = 0.0 - (float)T_mid / 256.0;
            }
            printf("�ɼ������¶�Ϊ��%.3f�棬ѹ��Ϊ��%.3fPa\r\n", Temp, Pressure);

            collection_data.state++;
        }
    }
    API_I2CSave(savebase + DATALEN * data_save.end, (uint8_t *)&collection_data, DATALEN);
    data_save.end++; /* ���ֵÿ���һ��,Ҫ��̫Ƶ�����׻� */
    data_save.end %= MAXSAVE;
    if (data_save.start == data_save.end)
    {
        /* ˵���Ѿ�����һȦ�� */
        data_save.isloop = 1;
    }
    if (data_save.isloop == 1)
    {
        data_save.start = data_save.end;
    }
}

uint32_t SAVEJUDGE(uint32_t a, uint32_t b)
{
    uint32_t c;
    (a > b) ? (c = MAXSAVE - b) : (c = a - b);
    return c;
}

/* ����Ҫ��ȡ�����ݾ��뵱ǰ�α�ľ��� */
int ReadCore(uint32_t start, uint32_t end)
{
    /* T1:startʱ���;T2:endʱ���;T3:�α�ʱ���; */
    uint32_t T1 = start, T2 = end, TT = 0;
    struct tm stm = {0};
    collection_data_type tdat;

    /* �����ʱ�䶼���� */
    if (T1 == 0 || T2 == 0 || T1 > T2)
        return -1;

    if ((T2 - T1) > (MAXSAVE * data_save.frequency))
        return -1;

    /* �α�:data_save.end */

    /* ��ȡ��ǰ�α������ */
    API_I2CRead(savebase + DATALEN * SAVEJUDGE(data_save.end, 1), (uint8_t *)&tdat, DATALEN);

    if (tdat.timestamp < T1)
    {
        /* ˵���α�ʱ�����Ҫ��ȡ��ʱ�仹С�����洢�����ݸ���û��Ҫ���Ļ����α�ʱ��������� */
        return -1;
    }
    else if (tdat.timestamp > T2)
    {
        /* ˵��Ҫô��ȡ����������ʷ���ݣ�Ҫô�α�ʱ��������� */
        stm.tm_year = Hex_YearL + 100;
        stm.tm_mon = Hex_Mon - 1;
        stm.tm_mday = Hex_Day;
        stm.tm_hour = Hex_Hour;
        stm.tm_min = Hex_Min;
        stm.tm_sec = Hex_Sec;
        TT = mktime(&stm);

        /* ��ȡϵͳ�����ʱ������Դ�Ϊ�����ж϶�ȡ��ʱ����Ƿ���ȷ����������£�
               ��Ӧ���Ƕ�ȡ��ʱ���С�ڵ�ǰʱ�䣬�����ǰʱ���쳣���Ǿ�û�취�� */
        if (TT < tdat.timestamp)
            return -1;

        /* �����α�ʱ�������ʼʱ��ʱ���֮�����ľ��� */
        return (((tdat.timestamp - T1) / data_save.frequency) + 1);
    }
    else
    {
        /* ˵���α�ʱ���ֱ�Ӿ���Ҫ��ȡ��ʱ�䷶Χ�� */
        return (((tdat.timestamp - T1) / data_save.frequency) + 1);
    }
}
/*
*********************************************************************************************************
*	�� �� ��: FillCore
*	����˵��: �������
*	��    ��: start��Ҫ������ʼ��ַ
*             buf������û���
*	�� �� ֵ: 1���Ѿ������ϣ�0������Ҫ�������
*	��    ע������������뽫 data_save.step ��0
*********************************************************************************************************
*/
int FillCore(uint32_t start, uint8_t *buf, uint8_t *cnt)
{
    uint8_t i = 0, j = 0;
    uint32_t tstart;
    volatile uint16_t len = 0;
    collection_data_type tdat;
    /* ������ʼ�㣺tstart �ͽ����㣺data_save.lock */
    if (!data_save.step)
    {
        data_save.lock = data_save.end;
    }
    (data_save.isloop) ? (tstart = SAVEJUDGE(data_save.lock, ONEPACKMAX * 10)) : (tstart = start);
    for (i = 0; len < ONEPACKMAX; i++)
    {
        if (((tstart + data_save.step) % MAXSAVE) != data_save.lock)
        {
            *cnt++;
            /* ��ȡ��ʼ��+������������� */
            API_I2CRead(savebase + DATALEN * ((tstart + data_save.step) % MAXSAVE), (uint8_t *)&tdat, DATALEN);
            memcpy(&buf[len], (uint8_t *)&tdat.state, 1);
            len += 1;
            memcpy(&buf[len], (uint8_t *)&tdat.timestamp, 4);
            len += 4;
            for (j = 0; j < 4; j++)
            {
                if (tdat.state & BIT3)
                {
                    memcpy(&buf[len], (uint8_t *)&tdat.buf[j * 5], 5);
                    len += 5;
                    tdat.state <<= 1;
                }
            }
            data_save.step++;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}

void sendend(void)
{
}

void atuoup(void)
{
    uint8_t buf[500], end = 0, cnt = 0;
    /* Э��ͷ */

    /* ������� */
    end = FillCore(data_save.start, buf, &cnt);
    /* Э��β */

    /* ���ݷ��� */
    if (end)
    {
        /* ���ݷ�����ɺ���Ҫ����Ȧȡ����������ʼ��λ����һ��lock��end */
        data_save.start = data_save.lock;
        data_save.isloop = 0;
        data_save.step = 0;
    }
}

void askup(struct tm start, struct tm end)
{
    uint32_t T1 = mktime(&start), T2 = mktime(&end);
    static int distance = 0;
    static uint32_t memstart = 0, memend = 0;
    uint8_t buf[500], endflag = 0, cnt = 0;
    if (memstart != T1 || memend != T2)
    {
        memstart = T1;
        memend = T2;
        distance = 0;
    }
    /* Э��ͷ */

    /* ������� */
    if (distance == 0)
    {
        distance = ReadCore(memstart, memend);
    }

    if (distance != -1)
    {
        endflag = FillCore(distance, buf, &cnt);
    }

    /* Э��β */

    /* ���ݷ��� */
    if (endflag)
    {
        distance = 0;
        data_save.step = 0;
    }
}