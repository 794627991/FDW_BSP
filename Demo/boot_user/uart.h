#ifndef __UART_H__
#define __UART_H__

#include "boot_define_all.h"

//定义通信超时时间常数
#define txtimeout_max 125 //500ms，串口字符间最大延时
#define framedelay_mx 15  //60ms
//通信缓冲区最大长度定义
#define urx_buf_size 1024 //通信缓冲区最大长度

struct COMMUNICATION_FRM
{
    uint08 Buf[urx_buf_size]; //接收缓冲(645&698)/发送缓冲(645)
    uint16 Total_Len;		  //接收总长度/发送总长度
    uint08 Step;			  //接收步骤
    uint08 Flag;			  //通讯标志：b0-645广播地址; b1-645缩位地址; b2-645接收完毕; b3-645启动发送; b5:b4-00单地址、01通配地址、10∽组地址、11-广播地址; b6-698接收完毕; b7-698启动发送
    uint08 Response_Timer;	  //通讯响应定时器
    uint16 Overtime_Timer;	  //通讯接收或者发生超时定时器
    uint16 Len;				  //接收长度字节/发送长度
};

extern struct COMMUNICATION_FRM Uart[UseHowMany];
extern void Uart_Proc(uint08 Ch);
extern void Veri_Etmr3(void);
extern uint08 Judge_RX_Frame(uint08 Ch, uint08 RcvData);
extern void Tx_Frame(uint08 Ch);
extern void UART485_Send_String(uint08 *buffer);

#endif
