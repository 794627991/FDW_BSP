#ifndef _PATCHFLASH_H
#define _PATCHFLASH_H

#include "stdint.h"

uint8_t *patchGetAdr(uint8_t op, uint32_t *len);
void patchFlashStart(void);
void patchFlashEnd(void);
void patchFlashWDog(uint32_t cnt);
void patchFlashAddData(uint8_t *buf, uint8_t *data, uint8_t op);
uint8_t patchFlashWrite(uint8_t *buf, uint8_t *data, uint32_t Len);

#endif
