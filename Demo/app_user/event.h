#ifndef __Event_H__
#define __Event_H__

#include "define_all.h"

#define Uart_Sent(buff, len) API_Uart_Send(UART1, buff, len)

typedef struct __minalgt
{
    uchar vI2cStart;
    uchar vSystemStat;
    uint32_t vTotalValue;
    uint32_t vZ_TotalValue;
    uint32_t vF_TotalValue;
    uchar vMeter_address[4];
} BaseType;

typedef struct
{
    BaseType Base;
} SaveBase;
extern SaveBase Save;

#define IIC_All_Data Save
#define BaseAdr Save.Base

#define I2cStart BaseAdr.vI2cStart
#define SystemStat BaseAdr.vSystemStat
#define TotalValue BaseAdr.vTotalValue
#define Z_TotalValue BaseAdr.vZ_TotalValue
#define F_TotalValue BaseAdr.vF_TotalValue
#define meter_address BaseAdr.vMeter_address

typedef struct
{
    uint8_t Year;   /*!<RTC 年*/
    uint8_t Month;  /*!<RTC 月*/
    uint8_t Date;   /*!<RTC 日*/
    uint8_t Hour;   /*!<RTC 时*/
    uint8_t Minute; /*!<RTC 分*/
    uint8_t Second; /*!<RTC 秒*/
    uint8_t Week;   /*!<RTC 周*/
} TimeType;

void SaveData(uint base, uchar *Adr, uint len);
void ReadData(uint base, uchar *Adr, uint len);

#define UDPIP "60.21.136.130"
#define UDPPORT "4325"

#define COAPPIP "117.60.157.137"
#define COAPPORT "5683"

extern OS_TCB TaskLEDTCB;
extern OS_TCB TaskNBTCB;

// void datainit(void);
// uint8_t RXDataSetCmd(uint8_t *buf, uint16_t len, bool uartset);
// void showins(bool op, char *buf, uint16_t len);
// void ShowChar(char *buf, uchar len);
// void ShowNum(uchar Num, uchar show);
// void DisPlay(uchar *disbuf);
// uchar DisPlaySwitch(uchar Num, uchar show);
// void DisPlayClear(void);
// void DisPlayFull(void);
// void lcd_insert(char *buf, uint16_t len);
// void rollshow(void);
// void lcd_send(int x, int y);
// void lcd_re_send(int x, int y);

void uCOS_NBTaskDel(void);
void uCOS_NBTaskCreate(void);
void uCOS_NBInitTaskCreate(void);
void uCOS_LEDCreate(void);

#endif
