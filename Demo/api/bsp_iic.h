/*
*********************************************************************************************************
*
*	ģ������ : IIC����(FOR����΢G045/G048)
*	�ļ����� : bsp_iic.h
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
#ifndef __BSP_IIC_H__
#define __BSP_IIC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

#define SSP_TBIT (20 * clkmode) /* ע�����cpu��Ƶ������ʱ�� */
#define I2CREAD 1               /* I2C������ */
#define I2CWRITE 0              /* I2Cд���� */
#define I2COPBUSY 1             /* I2C�ж��շ������� */
#define I2COPFINISH 0           /* I2C�ж��շ���� */
#define STARTBIT 0
#define RESTARTBIT 1
#define STOPBIT 2
#define ACKBIT 3

#ifndef I2C_Read
#define I2C_Read(Addr, Buf, Len) API_EEPROM_Read(DEVICE_EE256, Addr, ADDRLEN_EE256, Buf, Len)
#endif
#ifndef I2C_Write
#define I2C_Write(Addr, Buf, Len) API_EEPROM_Write(DEVICE_EE256, Addr, ADDRLEN_EE256, Buf, Len)
#endif

    typedef struct /* ʹ�ýṹ��������ʱʹ�� */
    {
        uint32_t IICBaseAdr;
        uint32_t IICStartAdr;
    } IICBase;
    extern IICBase IICDATA;

#define I2CBaseAdr IICDATA.IICBaseAdr
#define I2CStartAdr IICDATA.IICStartAdr

#define GetLen(len) sizeof(len) /* ��ȡ�ṹ���Ա���� */
#define GetAdr (uint8_t *)&     /* ��ȡ�ṹ���Ա��Ե�ַ */

#ifndef _API_IIC_SAVE_TO /* ��buf�����ݴ���ṹ���Աname���ڵĵ�ַ */
#define _API_IIC_SAVE_TO(name, buf) API_I2CSave(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, buf, GetLen(name))
#endif
#ifndef _API_IIC_READ_TO /* ���ṹ���Աname���ڵĵ�ַ���ݶ�ȡ��buf */
#define _API_IIC_READ_TO(name, buf) API_ReadData(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, buf, GetLen(name))
#endif

#ifndef _API_IIC_SAVE /* ʹ�ýṹ��洢����ʱ������ֱ��ͨ���ṹ���Ա�����д洢 */
#define _API_IIC_SAVE(name) API_I2CSave(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, GetAdr(name), GetLen(name))
#endif
#ifndef _API_IIC_READ /* ʹ�ýṹ���ȡ����ʱ������ֱ��ͨ���ṹ���Ա�����ж�ȡ */
#define _API_IIC_READ(name) API_ReadData(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, GetAdr(name), GetLen(name))
#endif

#ifndef IIC_SPEED
#define IIC_SPEED 100 /* ��λk  ��100k���ٶ� */
#endif

#define SIMLT_IIC_TT 5 /* ģ��IICʹ�� */

#ifndef MONI_IIC_SDA_GPIO
#define MONI_IIC_SDA_GPIO GPIOA
#endif
#ifndef MONI_IIC_SDA_PIN
#define MONI_IIC_SDA_PIN GPIO_Pin_15
#endif
#ifndef MONI_IIC_SDA_DIR_OUT
#define MONI_IIC_SDA_DIR_OUT OutputIO(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN, 0)
#endif
#ifndef MONI_IIC_SDA_DIR_IN
#define MONI_IIC_SDA_DIR_IN InputtIO(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN, 1)
#endif
#ifndef MONI_IIC_SDA_0
#define MONI_IIC_SDA_0 GPIO_ResetBits(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN)
#endif
#ifndef MONI_IIC_SDA_1
#define MONI_IIC_SDA_1 GPIO_SetBits(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN)
#endif
#ifndef MONI_IIC_SDA_READ
#define MONI_IIC_SDA_READ GPIO_ReadInputDataBit(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN)
#endif
#ifndef MONI_IIC_SCL_GPIO
#define MONI_IIC_SCL_GPIO GPIOA
#endif
#ifndef MONI_IIC_SCL_PIN
#define MONI_IIC_SCL_PIN GPIO_Pin_14
#endif
#ifndef MONI_IIC_SCL_DIR_OUT
#define MONI_IIC_SCL_DIR_OUT OutputIO(MONI_IIC_SCL_GPIO, MONI_IIC_SCL_PIN, 0)
#endif
#ifndef MONI_IIC_SCL_0
#define MONI_IIC_SCL_0 GPIO_ResetBits(MONI_IIC_SCL_GPIO, MONI_IIC_SCL_PIN)
#endif
#ifndef MONI_IIC_SCL_1
#define MONI_IIC_SCL_1 GPIO_SetBits(MONI_IIC_SCL_GPIO, MONI_IIC_SCL_PIN)
#endif

    typedef uint8_t (*iic_op)(uint8_t op); /* ģ��IICʹ�� */
    typedef struct
    {
        iic_op sda_set;
        iic_op sda_out;
        iic_op sda_in;
        iic_op scl;
    } iic_op_type; /* ģ��IICʹ�� */
    extern iic_op_type SIMLT_IIC;

    /**************************����ָ�붨��**************************/
    typedef void (*gy_iicinit)(void);
    typedef uint8_t (*gy_iicopcom)(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);
    typedef uint8_t (*gy_iicop)(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);

    /****************************��������****************************/
    void API_I2C_Init(void);
    void API_I2CSave(uint32_t base, uint8_t *Buf, uint32_t len);
    void API_I2CRead(uint32_t base, uint8_t *Buf, uint32_t len);
    uint8_t API_EEPROM_Read(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);
    uint8_t API_EEPROM_Write(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);
    uint8_t API_I2C_Read_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);
    uint8_t API_I2C_Write_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);

    void SIMLT_I2C_OP_INIT(void);
    uint8_t API_SIMLT_I2C_Write_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);
    uint8_t API_SIMLT_I2C_Read_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);
    uint8_t iic_op_sda_out(uint8_t op);
    uint8_t iic_op_sda_in(uint8_t op);
    uint8_t iic_op_scl(uint8_t op);
    uint8_t iic_op_sda_set(uint8_t op);

#ifdef __cplusplus
}
#endif

#endif
