/*
*********************************************************************************************************
*
*	模块名称 : 字符串操作\数值转换
*	文件名称 : bsp_user_lib.h
*	版    本 : V1.2
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __BSP_USER_LIB_H
#define __BSP_USER_LIB_H

#include "stdint.h"

int str_len(char *_str);
void str_cpy(char *_tar, char *_src);
int str_cmp(char *s1, char *s2);
void mem_set(char *_tar, char _data, int _len);

void int_to_str(int _iNumber, char *_pBuf, unsigned char _len);
int str_to_int(char *_pStr);

uint16_t BEBufToUint16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);

uint32_t BEBufToUint32(uint8_t *_pBuf);
uint32_t LEBufToUint32(uint8_t *_pBuf);

uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen);
int32_t CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);

unsigned char Bcd_to_Int(unsigned char i);
unsigned char Int_to_Bcd(unsigned char i);

char BcdToChar(uint8_t _bcd);
uint32_t AsciiToUint32(char *pAscii);
uint16_t Find_Max(uint16_t *buf);
uint16_t Find_Min(uint16_t *buf);
void uint_to_str(unsigned int _iNumber, char *_pBuf, unsigned char _len);

#endif
