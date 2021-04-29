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

int bspatch(bspatchtype *bspatch, struct bspatch_stream *stream)
{
    uint8_t *_new;
    uint8_t buf[8];
    int64_t oldpos, newpos;
    int64_t ctrl[3];
    int64_t i;

    /* old = (uint8_t *)oldadr;*/
    _new = (uint8_t *)bspatch->NewAdr;

    oldpos = 0;
    newpos = 0;
    while (newpos < bspatch->NewSize)
    {
        /* Read control data */
        for (i = 0; i <= 2; i++)
        {
            if (stream->read(stream, buf, 8, 1))
                return -1;
            ctrl[i] = offtin(buf);
        };

        /* Sanity-check */
        if (ctrl[0] < 0 || ctrl[0] > INT_MAX ||
            ctrl[1] < 0 || ctrl[1] > INT_MAX ||
            newpos + ctrl[0] > bspatch->NewSize)
            return -1;

        /* Read diff string */
        if (stream->read(stream, _new + newpos, ctrl[0], 0))
            return -1;

        /* Add old data to diff string */
        // for (i = 0; i < ctrl[0]; i++)
        //     if ((oldpos + i >= 0) && (oldpos + i < oldsize))
        //     {
        //         _new[newpos + i] += old[oldpos + i];
        //     }
        patchAddDiff(bspatch, newpos, oldpos, ctrl[0]);

        /* Adjust pointers */
        newpos += ctrl[0];
        oldpos += ctrl[0];

        /* Sanity-check */
        if (newpos + ctrl[1] > bspatch->NewSize)
            return -1;

        /* Read extra string */
        if (stream->read(stream, _new + newpos, ctrl[1], 0))
            return -1;

        /* Adjust pointers */
        newpos += ctrl[1];
        oldpos += ctrl[2];
    };

    return 0;
}

static int lzo_read(const struct bspatch_stream *stream, void *buffer, int length, uint8_t op)
{
    int n, err;
    lzoRead *lzo;
    lzo = (lzoRead *)stream->opaque;
    n = myLzoRead(&err, lzo, buffer, length, op);
    if (n != length)
        return -1;
    return 0;
}

/* argv3:差分文件  argv1：原文件 argv2：生成后的文件 */
int patch(void)
{
    int lzoerr;
    uint8_t *old, *_new, *_patch;
    lzoRead *lzo;
    struct bspatch_stream stream;
    bspatchtype Spatch;
    uint16_t crc1, crc2, level;
    uint16_t oldcrc, newcrc;
    uint32_t patchadr;

    /* 打开 patch 文件 ,如果在单片机里，这里需要改成一个地址指针 直接指向 patch */
    patchadr = patchGetAdr(2, NULL, NULL);
    _patch = (uint8_t *)patchadr;

    /* 打开 old 文件 ,如果在单片机里，这里需要改成一个地址指针 直接指向 OLD APP */
    Spatch.OldAdr = patchGetAdr(0, NULL, NULL);
    old = (uint8_t *)Spatch.OldAdr;

    /* Check for appropriate magic */
    if (memcmp(_patch, "GYCFSJ", 6) != 0)
    {
        debug("Check for appropriate magic error\r\n");
        return 0;
    }
    memcpy(&level, _patch + 6, 2);
    memcpy(&crc1, _patch + 8, 2);  /* oldcrc */
    memcpy(&crc2, _patch + 10, 2); /* newcrc */
    Spatch.OldSize = offtin(_patch + 16);
    Spatch.NewSize = offtin(_patch + 24);
    if (judgeSize(Spatch.OldSize, Spatch.NewSize) == 0)
    {
        debug("file size error\r\n");
        return 0;
    }

    oldcrc = LzoCRC(old, Spatch.OldSize);
    if (oldcrc != crc1)
    {
        debug("oldcrc err\r\n");
        return 0;
    }

    if ((Spatch.NewAdr = patchGetAdr(1, (uint32_t *)&Spatch.OldSize, (uint32_t *)&Spatch.NewSize)) == 0)
    {
        debug("get newadr err\r\n");
        return 0;
    }
    _new = (uint8_t *)Spatch.NewAdr;

    if ((NULL == (lzo = myLzoReadOpen(&lzoerr, level))) || (lzoerr != MYLZO_OK))
    {
        debug("myLzoReadOpen fail\r\n");
        return 0;
    }

    stream.read = lzo_read;
    lzo->block = _patch + 32;
    stream.opaque = lzo;

    if (bspatch(&Spatch, &stream))
    {
        myLzoReadClose(lzo);
        debug("bspatch fail\r\n");
        return 0;
    }

    /* Clean up the lzo reads */
    myLzoReadClose(lzo);

    newcrc = LzoCRC(_new, Spatch.NewSize);
    if (newcrc != crc2)
    {
        debug("crc err\r\n");
        return 0;
    }
    return 1;
}
