#include "gylzo.h"
#include "patchflash.h"

/*
*********************************************************************************************************
*	�� �� ��: LzoCRC
*	����˵��: CRC16
*	��    ��:
*	�� �� ֵ:
*	��    ע����
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
            crc_val >>= 1; /* CRC У���ֽڵ����ƣ����λ�� C�����λ�� 0 */
            if ((tmp & 0x01) == 1)
                crc_val ^= 0xA001; /* ���õĶ���ʽ�ǣ� 0xA001 */
        }
        LEN--;
    }
    return (crc_val);
}

lzo_uint minilzo_decompress(uint8_t *inbuf, uint32_t inlen, uint8_t *outbuf)
{
    int r;
    lzo_uint new_len = (lzo_uint)(-1); //���ܺ�
    if (lzo_init() != LZO_E_OK)
    {
        debug("\r\n��ʼ��ʧ��\r\n");
    }
    else
    {
        r = lzo1x_decompress_safe(inbuf, inlen, outbuf, &new_len, NULL);
        if (r == LZO_E_OK)
        {
            debug("\r\n��ѹ��ĳ���Ϊ��%d\r\n", new_len);
        }
    }

    return new_len;
}

/*����-1��ζ��ѹ��ʧ��*/
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
        /*��һ������ʧ�ܾͲ�������*/
        debug("mem fail\r\n");
    }
    else
    {
        if (lzo_init() != LZO_E_OK)
        {
            debug("��ʼ��ʧ��\r\n");
        }
        else
        {
            r = lzo1x_1_compress(inbuf, inlen, out, &out_len, wrkmem);
            if (r == LZO_E_OK)
            {
                // debug("\r\nѹ��������ݳ���Ϊ��%d\r\n", out_len);
                // debug("\r\nѹ����Ϊ: %.2lf%c\r\n", (float)(((int)inlen - (int)out_len) / (float)inlen) * 100.0, '%');
                if (out_len > inlen)
                {
                    /*��ζ�Ÿ�ѹ��,����ԭ��*/
                    out_len = (lzo_uint)(-1);
                    // debug("\r\nѹ�������ݳ��ȱ�ѹ��ǰ��\r\n");
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

/* �ְ�ѹ����slen��ÿ������ mlen���ܳ��� */
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
            /* ��ζ������ѹ��ʧ�� */
            ll = ss;
            memcpy(outbuf + tll, &G, 1);
            memcpy(outbuf + tll + 5 + ll, &Y, 1);
        }
        else
        {
            memcpy(outbuf + tll, &g, 1);
            memcpy(outbuf + tll + 5 + ll, &y, 1);
        }

        /* �ļ���ʽ: g+ѹ���󳤶�+dat+ѹ��ǰ����crc+y */
        memcpy(outbuf + tll + 1, &ll, 2);
        memcpy(outbuf + tll + 3, inbuf + mm, ll);
        memcpy(outbuf + tll + 3 + ll, &crc, 2);

        mm += ss;
        tll += (ll + 6);
    }
    return tll;
}

/* ��ѹ�� dlen:��ѹ��ĳ��ȣ����ص���ѹ�������� */
uint32_t mylzo_d(uint8_t *inbuf, uint8_t *outbuf, uint32_t *dlen, uint32_t limit)
{
    uint32_t len = 0;
    uint16_t crc1 = 0, crc2 = 0;
    uint8_t *mbuf;
    *dlen = 0;
    if (inbuf[0] == 'g' || inbuf[0] == 'G')
    {
        /* len��ѹ����Ĵ�С ������˵��len��ԶԶС�� limit ������ level ��ѹ���ռ����ѹ����Ĵ�С */
        memcpy(&len, inbuf + 1, 2);
        if (len > limit)
            return 0;
        if (inbuf[5 + len] == 'y' || inbuf[5 + len] == 'Y')
        {
            if (inbuf[0] == 'g') /* �������ܳɹ��� */
            {
                memset(outbuf, 0, limit);
                /* ���ﻹ��Ҫ����һ�� len+1 �Ŀռ� ����Ϊ��ѹ�����ݱ���ʹ�� ram��
                   �� inbuf ������ʵ�����Ǵ洢��flash �� patch �ļ����������ԭ��
                   Ԥ���Ļ����С������ level ��2�����ϣ����� */
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
    /* ���빤���� ��С�� level+1 */
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
        /* ������ֻҪ���� xloc ���� ONEPACKNUM һ���ǳ��������� */
        if (lzo->loc > lzo->level)
            return 0;

        /* Read diff string */
        if (((lzo->loc == 0) && (lzo->lzlen == 0)) ||
            ((lzo->loc == lzo->lzlen) && (lzo->lzlen == lzo->level)))
        {
            /* ����ѹ�󳤶Ȳ��ڵ��� lzo->level ˵���Ѿ���ѹ�����һ���� */
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
