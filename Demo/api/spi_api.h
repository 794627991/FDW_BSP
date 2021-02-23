/*
*********************************************************************************************************
*
*	ģ������ : SPI����(FOR����΢G045/G048)
*	�ļ����� : spi_api.h
*	��    �� : V1.0.0
*	˵    �� : SPI��ض���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-2-23    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#ifndef __API_SPI_H__
#define __API_SPI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

#ifndef MONI_SPI_MOSI_DIR
#define MONI_SPI_MOSI_DIR
#endif
#ifndef MONI_SPI_MOSI_1
#define MONI_SPI_MOSI_1 1
#endif
#ifndef MONI_SPI_MOSI_0
#define MONI_SPI_MOSI_0 0
#endif

#ifndef MONI_SPI_MISO_DIR
#define MONI_SPI_MISO_DIR
#endif
#ifndef MONI_SPI_MISO_READ
#define MONI_SPI_MISO_READ 0
#endif

#ifndef MONI_SPI_SCK_DIR
#define MONI_SPI_SCK_DIR
#endif
#ifndef MONI_SPI_SCK_1
#define MONI_SPI_SCK_1 1
#endif
#ifndef MONI_SPI_SCK_0
#define MONI_SPI_SCK_0 0
#endif

#ifndef MONI_SPI_NSS_DIR
#define MONI_SPI_NSS_DIR
#endif
#ifndef MONI_SPI_NSS_1
#define MONI_SPI_NSS_1 1
#endif
#ifndef MONI_SPI_NSS_0
#define MONI_SPI_NSS_0 0
#endif

    typedef uint8_t (*spi_op)(uint8_t op); /* ģ��SPIʹ�� */
    typedef struct
    {
        spi_op mosi;
        spi_op miso;
        spi_op sck;
        spi_op nss;
        spi_op mode;
    } spi_op_type; /* ģ��SPIʹ�� */
    extern spi_op_type SIMLT_SPI;

#ifdef __cplusplus
}
#endif

#endif
