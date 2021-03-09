#ifndef __API_bc28_35_95_H__
#define __API_bc28_35_95_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "iot_core.h"

#ifndef MODULE
#define MODULE bc28
#endif

	typedef struct
	{
		char *ip;
		char *port;
		uint8_t Simword[10]; //NCCID
		uint8_t CGSNNum[15];
		uint8_t Ver[20];
		uint16_t rsrp;
		uint32_t cellid;
		uint16_t EARFCN;
		uint16_t PCI;
		uint8_t ECL;
		uint16_t SNR;
		uint8_t RSSI;
		uint8_t readDGRAM;
		uint8_t readData;
		uint8_t DGRAM;
	} IOTDATTYPE(MODULE);
	extern IOTDATTYPE(MODULE) IOTDAT(MODULE);

	/*外部函数声明:字典获取，数据解析，不同发送方式的发送函数*/
	extern getcmdinfo_type GETCMD(MODULE)(uint8_t linkmode);

#ifdef __cplusplus
}
#endif

#endif
