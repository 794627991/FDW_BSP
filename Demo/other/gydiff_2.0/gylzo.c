#include "gylzo.h"
#include "patchflash.h"

/*
*********************************************************************************************************
*	函 数 名: LzoCRC
*	功能说明: CRC16
*	形    参:
*	返 回 值:
*	备    注：无
*********************************************************************************************************
*/
uint16_t LzoCRC(uint8_t *ptr, int64_t len)
{
    uint32_t LEN = (uint32_t)len;
    uint16_t crc_val = 0xffff;
    uint16_t tmp = 0;
    uint16_t i = 0;
    while (LEN > 0)
    {
        crc_val ^= *ptr;
        ptr++;
        for (i = 0; i < 8; i++)
        {
            tmp = crc_val;
            crc_val >>= 1; /* CRC 校验字节的右移，最低位到 C，最高位补 0 */
            if ((tmp & 0x01) == 1)
                crc_val ^= 0xA001; /* 采用的多项式是： 0xA001 */
        }
        LEN--;
    }
    return (crc_val);
}

lzo_uint minilzo_decompress(uint8_t *inbuf, uint32_t inlen, uint8_t *outbuf)
{
    int r;
    lzo_uint new_len = (lzo_uint)(-1); //加密后
    if (lzo_init() != LZO_E_OK)
    {
        debug("\r\n初始化失败\r\n");
    }
    else
    {
        r = lzo1x_decompress_safe(inbuf, inlen, outbuf, &new_len, NULL);
        if (r == LZO_E_OK)
        {
            debug("\r\n解压后的长度为：%d\r\n", new_len);
        }
    }

    return new_len;
}

/*返回-1意味着压缩失败*/
lzo_uint minilzo_compress(uint8_t *inbuf, uint32_t inlen)
{
    int r;
    lzo_uint out_len = (lzo_uint)(-1);
    unsigned char *out = NULL;
    lzo_align_t *wrkmem = NULL;
    out = malloc(OUT_LEN(inlen));
    wrkmem = malloc((lzo_uint32_t)((1 << D_BITS) * lzo_sizeof_dict_t));
    if (wrkmem == NULL || out == NULL)
    {
        /*有一个申请失败就不加密了*/
        debug("mem fail\r\n");
    }
    else
    {
        if (lzo_init() != LZO_E_OK)
        {
            debug("初始化失败\r\n");
        }
        else
        {
            r = lzo1x_1_compress(inbuf, inlen, out, &out_len, wrkmem);
            if (r == LZO_E_OK)
            {
                // debug("\r\n压缩后的数据长度为：%d\r\n", out_len);
                // debug("\r\n压缩比为: %.2lf%c\r\n", (float)(((int)inlen - (int)out_len) / (float)inlen) * 100.0, '%');
                if (out_len > inlen)
                {
                    /*意味着负压缩,保持原样*/
                    out_len = (lzo_uint)(-1);
                    // debug("\r\n压缩后数据长度比压缩前大\r\n");
                }
                else
                {
                    memset(inbuf, 0, inlen);
                    memcpy(inbuf, out, out_len);
                }
            }
        }
    }
    free(out);
    free(wrkmem);
    return out_len;
}

/* 分包压缩，slen：每包长度 mlen：总长度 */
uint32_t mylzo_c(uint8_t *inbuf, uint8_t *outbuf, uint16_t slen, uint32_t mlen)
{
    uint16_t crc = 0;
    uint32_t ll = 0, ss = 0, mm = 0, tll = 0;
    const char g = 'g', y = 'y', G = 'G', Y = 'Y';
    while (mm < mlen)
    {
        if (mlen >= slen + mm)
        {
            ss = slen;
        }
        else
        {
            ss = mlen - mm;
        }
        crc = LzoCRC(inbuf + mm, ss);
        ll = minilzo_compress(inbuf + mm, ss);
        if (ll == (uint32_t)-1)
        {
            /* 意味着曾经压缩失败 */
            ll = ss;
            memcpy(outbuf + tll, &G, 1);
            memcpy(outbuf + tll + 5 + ll, &Y, 1);
        }
        else
        {
            memcpy(outbuf + tll, &g, 1);
            memcpy(outbuf + tll + 5 + ll, &y, 1);
        }

        /* 文件格式: g+压缩后长度+dat+压缩前数据crc+y */
        memcpy(outbuf + tll + 1, &ll, 2);
        memcpy(outbuf + tll + 3, inbuf + mm, ll);
        memcpy(outbuf + tll + 3 + ll, &crc, 2);

        mm += ss;
        tll += (ll + 6);
    }
    return tll;
}

/* 解压缩 dlen:解压后的长度，返回单个压缩包长度 */
uint32_t mylzo_d(uint8_t *inbuf, uint8_t *outbuf, uint32_t *dlen, uint32_t limit)
{
    uint32_t len = 0;
    uint16_t crc1 = 0, crc2 = 0;
    uint8_t *mbuf;
    *dlen = 0;
    if (inbuf[0] == 'g' || inbuf[0] == 'G')
    {
        /* len是压缩后的大小 正常来说，len会远远小于 limit 即按照 level 的压缩空间进行压缩后的大小 */
        memcpy(&len, inbuf + 1, 2);
        if (len > limit)
            return 0;
        if (inbuf[5 + len] == 'y' || inbuf[5 + len] == 'Y')
        {
            if (inbuf[0] == 'g') /* 曾经加密成功的 */
            {
                memset(outbuf, 0, limit);
                /* 这里还需要申请一个 len+1 的空间 是因为解压缩数据必须使用 ram，
                   而 inbuf 的数据实际上是存储在flash 的 patch 文件，由于这个原因，
                   预留的缓存大小必须是 level 的2倍以上！！！ */
                if ((mbuf = (uint8_t *)mymalloc(len + 1)) == NULL)
                    return 0;
                memcpy(mbuf, inbuf + 3, len);
                *dlen = minilzo_decompress(mbuf, len, outbuf);
                myfree(mbuf);
            }
            else
            {
                memcpy(outbuf, inbuf + 3, len);
                *dlen = len;
            }
            crc1 = LzoCRC(outbuf, *dlen);
            memcpy(&crc2, inbuf + 3 + len, 2);
            if (crc1 == crc2)
            {
                return len + 6;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

#if _WIN64 || _WIN32
lzoWrite *myLzoWriteOpen(int *err, FILE *tar, int level)
{
    lzoWrite *flie = NULL;
    FILE *tpf;
    *err = 0;
    if (((tpf = tmpfile()) == NULL) || (level < 256) ||
        ((flie = (lzoWrite *)malloc(sizeof(lzoWrite))) == NULL))
    {
        return flie;
    }
    flie->target = tar;
    flie->tfile = tpf;
    flie->level = level;
    *err = MYLZO_OK;
    return flie;
}

void myLzoWrite(int *err, lzoWrite *lzo, const void *buffer, size_t size)
{
    *err = MYLZO_OK;
    if (fwrite(buffer, size, 1, lzo->tfile) != 1)
    {
        *err = 0;
    }
}

uint32_t myLzoWriteClose(int *err, lzoWrite *lzo)
{
    size_t re = 0;
    long len = 0;
    uint32_t lzclen = 0;
    uint8_t *tbuf = NULL, *buffer = NULL;
    *err = 0;
    if ((fseek(lzo->tfile, 0, SEEK_END) != 0) || ((len = ftell(lzo->tfile)) == 0) ||
        ((tbuf = (uint8_t *)malloc(LZOWORKLEN(len, lzo->level))) == NULL) ||
        ((buffer = (uint8_t *)malloc(len + 1)) == NULL) ||
        (fseek(lzo->tfile, 0, SEEK_SET) != 0) ||
        (fread(buffer, 1, len, lzo->tfile) != len))
    {
        return 0;
    }
    lzclen = mylzo_c((uint8_t *)buffer, tbuf, lzo->level, len);
    re = fwrite(tbuf, lzclen, 1, lzo->target);

    free(tbuf);
    free(buffer);
    fclose(lzo->tfile);
    free(lzo);
    if (re != 1)
    {
        return 0;
    }
    *err = MYLZO_OK;
    return lzclen;
}
#endif
lzoRead lzo;
lzoRead *myLzoReadOpen(int *err, int level)
{
    // lzoRead *t = &lzo;
    *err = 0;
    // if ((lzo = (lzoRead *)mymalloc(sizeof(lzoRead))) == NULL)
    // {
    //     return NULL;
    // }
    // lzo->block = 0;
    // lzo->level = 0;
    // lzo->loc = 0;
    // lzo->lzlen = 0;
    // lzo->pos = 0;
    // lzo->work = 0;
    memset(&lzo, 0, sizeof(lzoRead));
    /* 申请工作区 大小是 level+1 */
    if (((lzo.level = level) < 256) || (lzo.work = (uint8_t *)mymalloc(lzo.level + 1)) == NULL)
    {
        myfree(lzo.work);
        return NULL;
    }
    *err = MYLZO_OK;
    return &lzo;
}

uint32_t myLzoRead(int *err, lzoRead *lzo, void *buf, uint32_t len, uint8_t op)
{
    uint32_t offset = 0;
    lzo->pos = 0;
    *err = 0;

    while (lzo->pos < len)
    {
        /* 过程中只要出现 xloc 大于 ONEPACKNUM 一定是出现问题了 */
        if (lzo->loc > lzo->level)
            return 0;

        /* Read diff string */
        if (((lzo->loc == 0) && (lzo->lzlen == 0)) ||
            ((lzo->loc == lzo->lzlen) && (lzo->lzlen == lzo->level)))
        {
            /* 若解压后长度不在等于 lzo->level 说明已经解压到最后一包了 */
            if ((offset = mylzo_d(lzo->block, lzo->work, (uint32_t *)&lzo->lzlen, lzo->level)) == 0)
            {
                *err = 0;
                debug("mylzo_d err\r\n");
                return 0;
            }
            lzo->block += offset;
            lzo->loc = 0;
        }

        if ((len + lzo->loc) <= (lzo->lzlen + lzo->pos))
        {
            if (op)
            {
                memcpy((uint8_t *)buf + lzo->pos, lzo->work + lzo->loc, len - lzo->pos);
            }
            else
            {
                if (patchFlashWrite((uint8_t *)buf + lzo->pos, lzo->work + lzo->loc, len - lzo->pos) == 0)
                    return 0;
            }
            lzo->loc += (len - lzo->pos);
            lzo->pos = len;
        }
        else
        {
            if (op)
            {
                memcpy((uint8_t *)buf + lzo->pos, lzo->work + lzo->loc, lzo->lzlen - lzo->loc);
            }
            else
            {
                if (patchFlashWrite((uint8_t *)buf + lzo->pos, lzo->work + lzo->loc, lzo->lzlen - lzo->loc) == 0)
                    return 0;
            }
            lzo->pos += (lzo->lzlen - lzo->loc);
            lzo->loc = lzo->lzlen;
        }
    }
    *err = MYLZO_OK;
    return lzo->pos;
}

void myLzoReadClose(lzoRead *lzo)
{
    myfree(lzo->work);
    myfree(lzo);
}
