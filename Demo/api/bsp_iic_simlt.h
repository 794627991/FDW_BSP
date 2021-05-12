/*
*********************************************************************************************************
*
*	ģ������ : IIC����(FOR����΢G045/G048)
*	�ļ����� : bsp_iic_simlt.h
*	��    �� : V1.0.0
*	˵    �� : IIC��ض���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#ifndef __BSP_IIC_SIMLT_H__
#define __BSP_IIC_SIMLT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_def.h"

#ifndef SIMLT_IIC_TT
#define SIMLT_IIC_TT 4
#endif

#ifndef IIC_ChipWL
#define IIC_ChipWL 99
#endif

    typedef void (*io_op_dir)(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type);
    typedef void (*io_op_set)(GPIOx_Type *GPIOx, uint32_t GPIO_Pin);
    typedef uint8_t (*io_op_read)(GPIOx_Type *GPIOx, uint32_t GPIO_Pin);

    typedef struct
    {
        GPIOx_Type *sda_io;
        uint32_t sda_pin;
        GPIOx_Type *scl_io;
        uint32_t scl_pin;
        io_op_dir io_out;
        io_op_dir io_in;
        io_op_set io_set;
        io_op_set io_reset;
        io_op_read io_read;
    } iic_simlt_type; /* ģ��IICʹ�� */

    uint8_t API_SIMLT_IIC(iic_simlt_type *io, uint8_t op, uint8_t Device, uint16_t Addr, uint8_t *Buf, uint16_t Len);

#ifdef __cplusplus
}
#endif

#endif
