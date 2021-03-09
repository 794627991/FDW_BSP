#ifndef __API_IPORTCONVERT_H__
#define __API_IPORTCONVERT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

#define BigLittleSwap16(A) ((((uint16)(A)&0xff00) >> 8) | \
                            (((uint16)(A)&0x00ff) << 8)) /* 16位数据大小端转换 */

#define BigLittleSwap32(A) ((((uint32)(A)&0xff000000) >> 24) | \
                            (((uint32)(A)&0x00ff0000) >> 8) |  \
                            (((uint32)(A)&0x0000ff00) << 8) |  \
                            (((uint32)(A)&0x000000ff) << 24)) /* 32位数据大小端转换 */

    /**************************函数指针定义**************************/

    /****************************函数声明****************************/
    size_t mystrlen(const char *s, uint16_t len);                                      /* 带有长度限制的strlen */
    char *strnstr(const char *src, const char *substr, uint8_t len, uint8_t limt_len); /* 带有长度限制的字符串比较 */
    uint8_t HEXtoBCD(uint8_t hex_data);                                                /* HEX转为BCD子程序 */
    uint8_t BCDtoHEX(uint8_t bcd_data);                                                /* BCD转为HEX子程序 */
    char BinToChar(uint8_t _bcd);                                                      /* 16进制数0-15转成ascii的'0'-'9'和'a'-'f' */
    void HexToAscll(char *p1, uint8_t *p2, uint16_t length);                           /* 16进制格式的ASCII字符串 */
    void AsciiToHex(uint8_t *code, uint8_t *decode, uint16_t len);                     /* ASCII字符串转成16进制 */
    uint8_t CharToHex(char ch);                                                        /* ASCII的'0'-'9' 'a'-'f' 'A'-'F' 转成 0-15 */
    void CharToBety(uint8_t *iput, uint8_t *oput, uint8_t len);                        /* 将一传纯数字的ASCII码转成16进制数 */
    uint16_t MathCrc16(uint8_t *ptr, uint16_t len);
    uint16_t calcCheckCrc(uint8_t *pdata, uint16_t length);
    uint8_t CheckSum(uint8_t *parray, uint32_t size, uint8_t op);
    uint8_t camper(uint8_t *a, uint8_t *b, uint8_t len);
    void IPReduceSpotAddZero(char *ip1, char *ip2);
    void PortAddZero(char *port, char *tbf);
    void IPReduceZero(char *IP);
    void PortReduceZero_New(char *Port);
    void HexIPToAscii(char *ip, char *changeip);
    void HexPortToAscii(uint16_t port, char *changeport);
    void IPAddSpot(char *a, char *b);
    void PortReduceZero(char *a, char *b);
    uint8_t TheIPFormatIsCorrect(char *IPort, uint8_t type);
    uint8_t CalCheckSum(uint8_t *buf, uint32_t size, uint8_t op);

#ifdef __cplusplus
}
#endif

#endif
