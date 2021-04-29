#include "bsdiff.h"
#include "bspatch.h"
#include "myfile.h"
#include "gylzo.h"

int main(void)
{
    char mmm[15] = {"gycfsjsucc"};
    char yyy = 'y';
    uint16_t addzero;
    uint16_t datalen, crc, packnum;
    uint8_t *patchbuf, *write;
    uint64_t patchsize, offset, patchoffset;
    char argv1[200] = {"C:\\Users\\gaoyi\\Desktop\\gydiff_2.0\\test\\1.bin"};
    char argv2[200] = {"C:\\Users\\gaoyi\\Desktop\\gydiff_2.0\\test\\2.bin"};
    char argv3[200] = {"C:\\Users\\gaoyi\\Desktop\\gydiff_2.0\\test\\0.dif"};
    char argv4[200] = {"C:\\Users\\gaoyi\\Desktop\\gydiff_2.0\\test\\3.bin"};
    char argv5[200] = {"C:\\Users\\gaoyi\\Desktop\\gydiff_2.0\\test\\0.difdiv"};
    diff(argv1, argv2, argv3);
    patch(argv1, argv4, argv3);

    /*
        'g'+cmd(1b)+datalen(2b)+data(datalen)+crc(2b)+'y' 
        crc='g'+cmd+datalen+data
        cmd=0:开始下载 data: patchlen(4b)+patchcrc(2b) 
        cmd=1:数据包 data: packnum(2b)+data(max=512)
        cmd=2:下载完成 data: "gycfsjsucc"
        成功应答：cmd=3 data="gycfsjsucc"
        失败应答：cmd=4 data="gycfsjfail"
    */
    if ((patchbuf = myFileRead(argv3, &patchsize)) == NULL)
    {
        return 0;
    }
    crc = LzoCRC(patchbuf, patchsize);
    write = malloc(patchsize * 4);

    write[0] = 'g';
    write[1] = 0;
    datalen = 6;
    memcpy(write + 2, &datalen, 2);
    memcpy(write + 4, &patchsize, 4);
    memcpy(write + 8, &crc, 2);
    crc = LzoCRC(write, 4 + datalen);
    memcpy(write + 10, &crc, 2);
    memcpy(write + 12, &yyy, 1);
    memset(write + 13, 0, 3);

    offset = 16;
    packnum = 0;
    patchoffset = 0;

    while (patchsize > 0)
    {
        if (patchsize > 512)
        {
            datalen = 514;
            patchsize -= 512;

            write[0 + offset] = 'g';
            write[1 + offset] = 1;
            offset += 2;
            memcpy(write + offset, &datalen, 2);
            offset += 2;
            memcpy(write + offset, &packnum, 2);
            offset += 2;
            memcpy(write + offset, patchbuf + patchoffset, 512);
            patchoffset += 512;
            offset += 512;
            crc = LzoCRC(write + offset - 4 - datalen, 4 + datalen);
            memcpy(write + offset, &crc, 2);
            offset += 2;
            memcpy(write + offset, &yyy, 1);
            offset += 1;
        }
        else
        {
            datalen = patchsize + 2;

            write[0 + offset] = 'g';
            write[1 + offset] = 1;
            offset += 2;
            memcpy(write + offset, &datalen, 2);
            offset += 2;
            memcpy(write + offset, &packnum, 2);
            offset += 2;
            memcpy(write + offset, patchbuf + patchoffset, patchsize);
            patchoffset += patchsize;
            offset += patchsize;
            crc = LzoCRC(write + offset - 4 - datalen, 4 + datalen);
            memcpy(write + offset, &crc, 2);
            offset += 2;
            memcpy(write + offset, &yyy, 1);
            offset += 1;
            patchsize = 0;
        }
        packnum++;
        addzero = 16 - (offset % 16) + 512;
        memset(write + offset, 0, addzero);
        offset += addzero;
    }

    datalen = 10;
    write[0 + offset] = 'g';
    write[1 + offset] = 2;
    offset += 2;
    memcpy(write + offset, &datalen, 2);
    offset += 2;
    memcpy(write + offset, mmm, 10);
    offset += 10;
    crc = LzoCRC(write + offset - 4 - datalen, 4 + datalen);
    memcpy(write + offset, &crc, 2);
    offset += 2;
    memcpy(write + offset, &yyy, 1);
    offset += 1;
    myFileWrite(argv5, write, offset);

    free(write);
}
