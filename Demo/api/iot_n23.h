#ifndef __API_n23_H__
#define __API_n23_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "iot_core.h"

#undef MODULE
#define MODULE n23

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
    } IOTDATTYPE(MODULE);
    extern IOTDATTYPE(MODULE) IOTDAT(MODULE);

    /*�ⲿ��������:�ֵ��ȡ�����ݽ�������ͬ���ͷ�ʽ�ķ��ͺ���*/
    extern getcmdinfo_type GETCMD(MODULE)(uint8_t linkmode);

#ifdef __cplusplus
}
#endif

#endif
