/*
*********************************************************************************************************
*
*	模块名称 : 压缩测试
*	文件名称 : CompressTest.c
*	版    本 : V1.0.0
*	说    明 : gzip,quicklz,minilzo 三种压缩测试
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0   2021.3.4   高屹     无
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "CompressTest.h"

#define uCOS_TaskCompress_PRIO 6
#define uCOS_TaskCompress_STK_SIZE 256

//#define IN_LEN (128 * 1024ul)
#define OUT_LEN(IN_LEN) (IN_LEN + IN_LEN / 16 + 64 + 3)

//static unsigned char __LZO_MMODEL in[IN_LEN];
//static unsigned char __LZO_MMODEL out[OUT_LEN];

#define HEAP_ALLOC(var, size) \
    lzo_align_t __LZO_MMODEL var[((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t)]

#define IN_LEN 450
unsigned char in[IN_LEN];

uint8_t asd[15] = {3, 6, 2, 1, 9, 5, 78, 15, 98, 10, 61, 18, 97, 15, 64};

/*
*********************************************************************************************************
*	函 数 名: jiamitest3
*	功能说明: gizp的压缩测试，该压缩算法需要40kram
*	形    参: 略
*	返 回 值: 无
*	备    注：！！！该压缩算法需要40kram
*********************************************************************************************************
*/
// void jiamitest3(uint8_t *buf, uint16_t len)
// {
//     char *out;
//     uint16_t len1, len2;
//     printf("\r\n加密测试3开始\r\n");
//     out = (char *)mymalloc(len + 100);
//     len1 = zipmem((char *)buf, len, out);
//     printf("\r\n压缩完成,压缩后数据大小为:%d\r\n", len1);
//     printf("\n压缩比为: %.2lf%c\n", (double)((len - len1) / (double)len) * 100.0, '%');
//     memset(buf, 0, len);
//     len2 = unzipmem(out, len1, (char *)buf);
//     printf("解压收的数据为：");
//     for (len1 = 0; len1 < len2; len1++)
//         printf("%x", buf[len1]);
//     printf("\r\n加密测试3结束\r\n");
// }

/*
*********************************************************************************************************
*	函 数 名: jiamitest2
*	功能说明: quicklz的压缩测试，该压缩算法需要10kram
*	形    参: 略
*	返 回 值: 无
*	备    注：！！！该压缩算法需要10kram
*********************************************************************************************************
*/
void jiamitest2(uint8_t *buf, uint16_t len)
{
    char *compressed;
    uint16_t c, d;
    printf("\r\n加密测试2开始\r\n");
    qlz_state_compress *state_compress = (qlz_state_compress *)mymalloc(sizeof(qlz_state_compress));

    // allocate "uncompressed size" + 400 bytes for the destination buffer where
    // "uncompressed size" = 10000 in worst case in this sample demo
    compressed = (char *)mymalloc(len + 100);

    // allocate and initially zero out the states. After this, make sure it is
    // preserved across calls and never modified manually
    memset(state_compress, 0, sizeof(qlz_state_compress));

    d = qlz_compress(buf, compressed, len, state_compress);
    printf("\n压缩比为: %.2f%c\n", (float)((len - d) / (float)len) * 100.0, '%');
    myfree(state_compress);

    qlz_state_decompress *state_decompress = (qlz_state_decompress *)mymalloc(sizeof(qlz_state_decompress));

    c = qlz_decompress(compressed, buf, state_decompress);

    printf("%u bytes decompressed into %u.\n", (unsigned int)d, (unsigned int)c);
    printf("解压后的数据为：");
    for (uint16_t j = 0; j < c; j++)
        printf("%x", buf[j]);
    myfree(compressed);
    myfree(state_decompress);
    printf("\r\n加密测试2结束\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: jiamitest
*	功能说明: minilzo的压缩测试，该压缩算法需要10kram
*	形    参: 略
*	返 回 值: 无
*	备    注：！！！该压缩算法需要10kram
*********************************************************************************************************
*/
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
        printf("\n压缩比为: %.2f%c\n", (float)((in_len - out_len) / (float)in_len) * 100.0, '%');
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
        printf("解压后的数据为：");
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
        printf("\n压缩比为: %.2f%c\n", (float)((in_len - out_len) / (float)in_len) * 100.0, '%');
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
                 (OS_PRIO)uCOS_TaskCompress_PRIO,
                 (CPU_STK_SIZE)uCOS_TaskCompress_STK_SIZE);
}
