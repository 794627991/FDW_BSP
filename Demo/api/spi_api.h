/*
*********************************************************************************************************
*
*	模块名称 : SPI定义(FOR复旦微G045/G048)
*	文件名称 : spi_api.h
*	版    本 : V1.0.0
*	说    明 : SPI相关定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-2-23    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
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

    typedef uint8_t (*spi_op)(uint8_t op); /* 模拟SPI使用 */
    typedef struct
    {
        spi_op mosi;
        spi_op miso;
        spi_op sck;
        spi_op nss;
        spi_op mode;
    } spi_op_type; /* 模拟SPI使用 */
    extern spi_op_type SIMLT_SPI;

#ifdef __cplusplus
}
#endif

#endif
