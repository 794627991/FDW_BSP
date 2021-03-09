#ifndef __API_bc260_H__
#define __API_bc260_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "iot_core.h"

#ifndef MODULE
#define MODULE bc260
#endif

    typedef struct
    {
        uint8_t Simword[10]; //NCCID
        uint8_t CGSNNum[15];
        uint16_t rsrp;
        uint32_t cellid;
        uint16_t EARFCN;
        uint16_t PCI;
        uint8_t ECL;
        uint16_t SNR;
        uint8_t RSSI;
        uint8_t DGRAM;
        uint8_t Band;
        uint8_t oberve[6];
        uint8_t msgid1[20];
        uint8_t msgid2[20];
    } IOTDATTYPE(MODULE);
	extern IOTDATTYPE(MODULE) IOTDAT(MODULE);

    /*�ⲿ��������:�ֵ��ȡ�����ݽ�������ͬ���ͷ�ʽ�ķ��ͺ���*/
    extern getcmdinfo_type GETCMD(MODULE)(uint8_t linkmode);

#ifdef __cplusplus
}
#endif

#endif
