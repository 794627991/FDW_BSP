#ifndef __Event_H__
#define __Event_H__

#include "define_all.h"

#define Uart_Sent(buff, len) API_Uart_Send(UART1, buff, len)

#define SaveDataAdr 0x40000 - 0x200 //以程序的结尾预留的512为存储空间

typedef union
{
    uint32_t State;
    struct
    {
        uint32_t RDY_done : 1;
        uint32_t ATE0_done : 1;
        uint32_t AT_done : 1;
        uint32_t CPIN_done : 1;
        uint32_t QICSGP_done : 1;
        uint32_t QIACT_done : 1;
        uint32_t QIOPEN_done : 1;
    };
} RStype; //运行状态标志

typedef union
{
    uint32_t SYS_Stat;
    struct
    {
        uint32_t socket0Online : 1;
        uint32_t CanSend : 1;
        uint32_t lcd_succ :1;
    };
} NetStat;
extern NetStat Net;

typedef struct
{
    char ShowDataFlag;
    uint32_t ShowDataLen;
    char ledexbuf[1050];
} ShowMemType;
extern ShowMemType ShowMem;

typedef struct
{
    char csq[8];
    char band[9];
    char plmn[13];
    char cellid[17];
    char pcid[11];
    char earfcn[15];
    char tac[11];
    char rsrp[11];
    char rsrq[11];
    char rssi[11];
    char sinr[11];
    char ip[21];
    char port[13];
    char clk[21];
} ShowBufType;
extern ShowBufType ShowBuf;

typedef struct
{
    char savestate;
    char Ip[16];
    char Port[6];
    char Apn[20];
    char JZQID[4];
} JZQSaveType;
extern JZQSaveType JZQSave;

typedef struct
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
    uchar vWork_time[2];
    uchar vWorkTimeMin;
    uint16_t vGprsUpInterval;
    uchar vGprsStartworkDay;
    uchar vGprsEndworkDay;
    uchar vGprsWorkDayinterval;
    uchar vLinkMode;
} NetCoetype;

typedef struct
{
    char vIP1[15];
    char vPORT1[5];
    char vAPN[32];
} IP_Type;

typedef struct
{
    uchar vStaDay;
    uchar vCountDay;
    uchar vTotalSumNo3Day[3][4];
    uchar vMonthSumN0[4];
    uchar vMonthSumN1[4];
    uchar vMonth12Value[48];
    uchar vDay30Value[120];
} HistoryType;

typedef struct
{
    BaseType Base;	   /*基本参数*/
    NetCoetype NetCoe; /*联网相关参数*/
    IP_Type SaveIp;
    HistoryType History; /*历史用量或数据*/
} DataBase;

typedef struct
{
    uint Adr;
    DataBase Data;
} SaveBase;
extern SaveBase Save;

#ifndef IIC_Save
#define IIC_Save(name) API_SaveData(I2CBaseAdr, GetAdr(name), GetLen(name))
#endif
#ifndef IIC_Read
#define IIC_Read(name) API_ReadData(I2CBaseAdr, GetAdr(name), GetLen(name))
#endif

#define BaseAdr Save.Adr
#define IIC_All_Data Save.Data

#define I2cStart Save.Data.Base.vI2cStart
#define SystemStat Save.Data.Base.vSystemStat
#define TotalValue Save.Data.Base.vTotalValue
#define Z_TotalValue Save.Data.Base.vZ_TotalValue
#define F_TotalValue Save.Data.Base.vF_TotalValue
#define meter_address Save.Data.Base.vMeter_address

#define NetData Save.Data.NetCoe
#define Work_time Save.Data.NetCoe.vWork_time
#define WorkTimeMin Save.Data.NetCoe.vWorkTimeMin
#define GprsUpInterval Save.Data.NetCoe.vGprsUpInterval
#define GprsStartworkDay Save.Data.NetCoe.vGprsStartworkDay
#define GprsEndworkDay Save.Data.NetCoe.vGprsEndworkDay
#define GprsWorkDayinterval Save.Data.NetCoe.vGprsWorkDayinterval
#define vLinkMode Save.Data.NetCoe.vLinkMode

#define HistoryData Save.Data.History
#define StaDay Save.Data.History.vStaDay
#define TotalSumNo3Day Save.Data.History.vTotalSumNo3Day
#define MonthSumN0 Save.Data.History.vMonthSumN0
#define MonthSumN1 Save.Data.History.vMonthSumN1
#define Month12Value Save.Data.History.vMonth12Value
#define Day30Value Save.Data.History.vDay30Value
#define CountDay Save.Data.History.vCountDay

#define IPPORTData Save.Data.SaveIp
#define IP1 Save.Data.SaveIp.vIP1
#define PORT1 Save.Data.SaveIp.vPORT1
#define APN Save.Data.SaveIp.vAPN

typedef struct
{
    uint8_t Year;	/*!<RTC 年*/
    uint8_t Month;	/*!<RTC 月*/
    uint8_t Date;	/*!<RTC 日*/
    uint8_t Hour;	/*!<RTC 时*/
    uint8_t Minute; /*!<RTC 分*/
    uint8_t Second; /*!<RTC 秒*/
    uint8_t Week;	/*!<RTC 周*/
} TimeType;

void SaveData(uint base, uchar *Adr, uint len);
void ReadData(uint base, uchar *Adr, uint len);

#define UDPIP "60.21.136.130"
#define UDPPORT "4325"

#define COAPPIP "117.60.157.137"
#define COAPPORT "5683"

extern OS_TCB TaskLEDTCB;
extern OS_TCB TaskNBTCB;
extern OS_TCB TaskNBSendTCB;
extern OS_Q NBBOX;

void datainit(void);
uint8_t RXDataSetCmd(uint8_t *buf, uint16_t len, bool uartset);
void showins(bool op, char *buf, uint16_t len);
void ShowChar(char *buf, uchar len);
void ShowNum(uchar Num, uchar show);
void DisPlay(uchar *disbuf);
uchar DisPlaySwitch(uchar Num, uchar show);
void DisPlayClear(void);
void DisPlayFull(void);
void lcd_insert(char *buf, uint16_t len);
void rollshow(void);
void lcd_send(int x,int y);
void lcd_re_send(int x,int y);

void uCOS_NBTaskDel(void);
void uCOS_NBTaskCreate(void);
void uCOS_NBInitTaskCreate(void);
void uCOS_LEDCreate(void);
#endif
