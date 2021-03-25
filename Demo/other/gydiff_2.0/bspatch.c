/*-
 * Copyright 2003-2005 Colin Percival
 * Copyright 2012 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions 
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <limits.h>
#include "bspatch.h"
#include "gylzo.h"
#include "patchflash.h"

static int64_t offtin(uint8_t *buf)
{
    int64_t y;

    y = buf[7] & 0x7F;
    y = y * 256;
    y += buf[6];
    y = y * 256;
    y += buf[5];
    y = y * 256;
    y += buf[4];
    y = y * 256;
    y += buf[3];
    y = y * 256;
    y += buf[2];
    y = y * 256;
    y += buf[1];
    y = y * 256;
    y += buf[0];

    if (buf[7] & 0x80)
        y = -y;

    return y;
}

int bspatch(const uint8_t *old, int64_t oldsize, uint8_t *_new, int64_t newsize, struct bspatch_stream *stream)
{
    uint8_t buf[8];
    int64_t oldpos, newpos;
    int64_t ctrl[3];
    int64_t i;

    oldpos = 0;
    newpos = 0;
    while (newpos < newsize)
    {
        /* Read control data */
        for (i = 0; i <= 2; i++)
        {
            if (stream->read(stream, buf, 8))
                return -1;
            ctrl[i] = offtin(buf);
        };

        /* Sanity-check */
        if (ctrl[0] < 0 || ctrl[0] > INT_MAX ||
            ctrl[1] < 0 || ctrl[1] > INT_MAX ||
            newpos + ctrl[0] > newsize)
            return -1;

        /* Read diff string */
        if (stream->read(stream, _new + newpos, ctrl[0]))
            return -1;

        /* Add old data to diff string */
        patchFlashStart();
        for (i = 0; i < ctrl[0]; i++)
            if ((oldpos + i >= 0) && (oldpos + i < oldsize))
            {
                patchFlashWDog(i);
                // _new[newpos + i] += old[oldpos + i];
                patchFlashAddData(&_new[newpos + i], (uint8_t*)&old[oldpos + i], 0);
            }
        patchFlashEnd();

        /* Adjust pointers */
        newpos += ctrl[0];
        oldpos += ctrl[0];

        /* Sanity-check */
        if (newpos + ctrl[1] > newsize)
            return -1;

        /* Read extra string */
        if (stream->read(stream, _new + newpos, ctrl[1]))
            return -1;

        /* Adjust pointers */
        newpos += ctrl[1];
        oldpos += ctrl[2];
    };

    return 0;
}

static int lzo_read(const struct bspatch_stream *stream, void *buffer, int length)
{
    int n, err;
    lzoRead *lzo;
    lzo = (lzoRead *)stream->opaque;
    n = myLzoRead(&err, lzo, buffer, length);
    if (n != length)
        return -1;
    return 0;
}

/* argv3:差分文件  argv1：原文件 argv2：生成后的文件 */
int patch(void)
{
    int lzoerr;
    uint8_t *old, *_new, *patch;
    lzoRead *lzo;
    struct bspatch_stream stream;
    uint16_t crc1, crc2, level;
    int64_t oldsize = 0, newsize = 0;
    uint32_t tsize = 0;

    /* 打开 patch 文件 ,如果在单片机里，这里需要改成一个地址指针 直接指向 patch */
    patch = patchGetAdr(2, &tsize);

    /* 打开 old 文件 ,如果在单片机里，这里需要改成一个地址指针 直接指向 OLD APP */
    old = patchGetAdr(0, (uint32_t *)&oldsize);

    /* Check for appropriate magic */
    if (memcmp(patch, "CFSJ", 4) != 0)
    {
        printf("Check for appropriate magic error\r\n");
        return 0;
    }
    memcpy(&level, patch + 4, 2);
    memcpy(&crc1, patch + 6, 2);
    newsize = offtin(patch + 8);
    if (newsize < 0)
    {
        printf("Read lengths from header error\r\n");
        return 0;
    }

    /* 模拟 FLASH 的缓存 */
    _new = patchGetAdr(1, &tsize);

    if ((NULL == (lzo = myLzoReadOpen(&lzoerr, level))) || (lzoerr != MYLZO_OK))
    {
        printf("myLzoReadOpen fail\r\n");
        return 0;
    }

    stream.read = lzo_read;
    lzo->block = patch + 16;
    stream.opaque = lzo;
    if (bspatch(old, oldsize, _new, newsize, &stream))
    {
        printf("bspatch fail\r\n");
        return 0;
    }

    /* Clean up the lzo reads */
    myLzoReadClose(lzo);

    crc2 = LzoCRC(_new, newsize);
    if (crc1 != crc2)
    {
        printf("crc err\r\n");
        return 0;
    }
    return 1;
}
