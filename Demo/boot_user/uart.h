#ifndef __UART_H__
#define __UART_H__

#include "boot_define_all.h"

//����ͨ�ų�ʱʱ�䳣��
#define txtimeout_max 125 //500ms�������ַ��������ʱ
#define framedelay_mx 15  //60ms
//ͨ�Ż�������󳤶ȶ���
#define urx_buf_size 1024 //ͨ�Ż�������󳤶�

struct COMMUNICATION_FRM
{
    uint08 Buf[urx_buf_size]; //���ջ���(645&698)/���ͻ���(645)
    uint16 Total_Len;		  //�����ܳ���/�����ܳ���
    uint08 Step;			  //���ղ���
    uint08 Flag;			  //ͨѶ��־��b0-645�㲥��ַ; b1-645��λ��ַ; b2-645�������; b3-645��������; b5:b4-00����ַ��01ͨ���ַ��10�����ַ��11-�㲥��ַ; b6-698�������; b7-698��������
    uint08 Response_Timer;	  //ͨѶ��Ӧ��ʱ��
    uint16 Overtime_Timer;	  //ͨѶ���ջ��߷�����ʱ��ʱ��
    uint16 Len;				  //���ճ����ֽ�/���ͳ���
};

extern struct COMMUNICATION_FRM Uart[UseHowMany];
extern void Uart_Proc(uint08 Ch);
extern void Veri_Etmr3(void);
extern uint08 Judge_RX_Frame(uint08 Ch, uint08 RcvData);
extern void Tx_Frame(uint08 Ch);
extern void UART485_Send_String(uint08 *buffer);

#endif
