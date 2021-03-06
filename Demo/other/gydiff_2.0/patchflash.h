#ifndef _PATCHFLASH_H
#define _PATCHFLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "bspatch.h"

    typedef struct _patchdownload
    {
        uint16_t crc;
        uint16_t num;
        uint32_t len;
        uint32_t offset;
    } DownLoadType;

    uint8_t judgeSize(int64_t oldsize, int64_t newsize);
    uint32_t patchGetAdr(uint8_t op, uint32_t *oldlen, uint32_t *newlen);
    void patchFlashStart(void);
    void patchFlashEnd(void);
    void patchFlashWDog(uint32_t cnt);
    void patchFlashAddData(uint8_t *buf, uint8_t *data, uint8_t op);
    uint8_t patchFlashWrite(uint8_t *buf, uint8_t *data, uint32_t Len);

    uint8_t patchDownLoad(uint8_t *buf, uint16_t *len);
    void patchUpGrade(void);
    void patchDownLoadAnswer(uint8_t *buf, uint16_t *len);
    uint8_t patchAddDiff(bspatchtype *bspatch, int64_t newpos, int64_t oldpos, int64_t len);
    void patchAppWriteVer(uint16_t ver);

#ifdef __cplusplus
}
#endif

#endif
