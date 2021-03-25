#ifndef MYLZO_H
#define MYLZO_H

#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "minilzo.h"
#include "malloc.h"
#include "myprintf.h"

#if _WIN64 || _WIN32
#include "stdio.h"
typedef struct _myLzoWrite
{
    FILE *target;   //目标文件
    FILE *tfile;    //临时文件
    uint32_t level; //压缩等级
} lzoWrite;

lzoWrite *myLzoWriteOpen(int *err, FILE *tar, int level);
void myLzoWrite(int *err, lzoWrite *lzo, const void *buffer, size_t size);
uint32_t myLzoWriteClose(int *err, lzoWrite *lzo);

#endif

typedef struct _myLzoRead
{
    uint32_t pos;   /* 区块计算地址 */
    uint32_t lzlen; /* 解压后的数据长度 */
    uint32_t loc;   /* 计算到的工作区位置 */
    uint32_t level; /* 压缩等级 */
    uint8_t *block; /* patch 数据块指针地址 */
    uint8_t *work;  /* 工作区指针地址 */
} lzoRead;

#define LZOLEVEL 9000 /* 压缩等级，也就是按照多大的内存工作去进行压缩，该数值越大压缩效果越好，但是需要下位机ram足够 */
#define MYLZO_OK 'g'
#define LZOWORKLEN(x, y) (((x / y) + 1) * (y + 6) + 1) /* 用于申请工作区，动态计算申请区大小 */
#define OUT_LEN(IN_LEN) (IN_LEN + IN_LEN / 16 + 64 + 3)

uint16_t LzoCRC(uint8_t *ptr, int64_t len);
lzo_uint minilzo_compress(uint8_t *inbuf, uint32_t inlen);
uint32_t mylzo_c(uint8_t *inbuf, uint8_t *outbuf, uint16_t slen, uint32_t mlen);
uint32_t mylzo_d(uint8_t *inbuf, uint8_t *outbuf, uint32_t *dlen, uint32_t limit);

lzoRead *myLzoReadOpen(int *err, int level);
void myLzoReadClose(lzoRead *lzo);
uint32_t myLzoRead(int *err, lzoRead *lzo, void *buf, uint32_t len);

#endif
