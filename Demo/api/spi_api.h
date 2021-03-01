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

#ifndef MONI_SPI_NSS_GPIO
#define MONI_SPI_NSS_GPIO GPIOF
#endif
#ifndef MONI_SPI_NSS_PIN
#define MONI_SPI_NSS_PIN GPIO_Pin_14
#endif

#ifndef MONI_SPI_SCK_GPIO
#define MONI_SPI_SCK_GPIO GPIOF
#endif
#ifndef MONI_SPI_SCK_PIN
#define MONI_SPI_SCK_PIN GPIO_Pin_12
#endif

#ifndef MONI_SPI_MOSI_GPIO
#define MONI_SPI_MOSI_GPIO GPIOE
#endif
#ifndef MONI_SPI_MOSI_PIN
#define MONI_SPI_MOSI_PIN GPIO_Pin_3
#endif

#ifndef MONI_SPI_MISO_GPIO
#define MONI_SPI_MISO_GPIO GPIOB
#endif
#ifndef MONI_SPI_MISO_PIN
#define MONI_SPI_MISO_PIN GPIO_Pin_15
#endif

#ifndef MONI_SPI_MOSI_DIR 
#define MONI_SPI_MOSI_DIR OutputIO(MONI_SPI_MOSI_GPIO, MONI_SPI_MOSI_PIN, 1)
#endif
#ifndef MONI_SPI_MOSI_1
#define MONI_SPI_MOSI_1 GPIO_SetBits(MONI_SPI_MOSI_GPIO, MONI_SPI_MOSI_PIN)
#endif
#ifndef MONI_SPI_MOSI_0
#define MONI_SPI_MOSI_0 GPIO_ResetBits(MONI_SPI_MOSI_GPIO, MONI_SPI_MOSI_PIN)
#endif

#ifndef MONI_SPI_MISO_DIR
#define MONI_SPI_MISO_DIR InputtIO(MONI_SPI_MISO_GPIO, MONI_SPI_MISO_PIN, 1)
#endif
#ifndef MONI_SPI_MISO_READ
#define MONI_SPI_MISO_READ GPIO_ReadInputDataBit(MONI_SPI_MISO_GPIO, MONI_SPI_MISO_PIN)
#endif

#ifndef MONI_SPI_SCK_DIR
#define MONI_SPI_SCK_DIR OutputIO(MONI_SPI_SCK_GPIO, MONI_SPI_SCK_PIN, 1)
#endif
#ifndef MONI_SPI_SCK_1
#define MONI_SPI_SCK_1 GPIO_SetBits(MONI_SPI_SCK_GPIO, MONI_SPI_SCK_PIN)
#endif
#ifndef MONI_SPI_SCK_0
#define MONI_SPI_SCK_0 GPIO_ResetBits(MONI_SPI_SCK_GPIO, MONI_SPI_SCK_PIN)
#endif

#ifndef MONI_SPI_NSS_DIR
#define MONI_SPI_NSS_DIR OutputIO(MONI_SPI_NSS_GPIO, MONI_SPI_NSS_PIN, 1)
#endif
#ifndef MONI_SPI_NSS_1
#define MONI_SPI_NSS_1 GPIO_SetBits(MONI_SPI_NSS_GPIO, MONI_SPI_NSS_PIN)
#endif
#ifndef MONI_SPI_NSS_0
#define MONI_SPI_NSS_0 GPIO_ResetBits(MONI_SPI_NSS_GPIO, MONI_SPI_NSS_PIN)
#endif

    typedef uint8_t (*spi_op)(uint8_t op); /* 模拟SPI使用 */
    typedef struct
    {
        spi_op mosi;
        spi_op miso;
        spi_op sck;
        spi_op nss;
        spi_op op;
    } spi_op_type; /* 模拟SPI使用 */

    spi_op_type* SIMLT_SPI_OP_INIT(uint8_t mode);
    void API_SIMLT_SPI(uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
