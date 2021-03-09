/*
*********************************************************************************************************
*
*	模块名称 : IIC定义(FOR复旦微G045/G048)
*	文件名称 : bsp_iic.h
*	版    本 : V1.0.0
*	说    明 : IIC相关定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
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

#define SSP_TBIT (20 * clkmode) /* 注意根据cpu主频调整延时量 */
#define I2CREAD 1               /* I2C读操作 */
#define I2CWRITE 0              /* I2C写操作 */
#define I2COPBUSY 1             /* I2C中断收发操作中 */
#define I2COPFINISH 0           /* I2C中断收发完成 */
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

    typedef struct /* 使用结构体存读数据时使用 */
    {
        uint32_t IICBaseAdr;
        uint32_t IICStartAdr;
    } IICBase;
    extern IICBase IICDATA;

#define I2CBaseAdr IICDATA.IICBaseAdr
#define I2CStartAdr IICDATA.IICStartAdr

#define GetLen(len) sizeof(len) /* 获取结构体成员长度 */
#define GetAdr (uint8_t *)&     /* 获取结构体成员相对地址 */

#ifndef _API_IIC_SAVE_TO /* 将buf中数据存入结构体成员name所在的地址 */
#define _API_IIC_SAVE_TO(name, buf) API_I2CSave(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, buf, GetLen(name))
#endif
#ifndef _API_IIC_READ_TO /* 将结构体成员name所在的地址数据读取到buf */
#define _API_IIC_READ_TO(name, buf) API_ReadData(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, buf, GetLen(name))
#endif

#ifndef _API_IIC_SAVE /* 使用结构体存储变量时，可以直接通过结构体成员名进行存储 */
#define _API_IIC_SAVE(name) API_I2CSave(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, GetAdr(name), GetLen(name))
#endif
#ifndef _API_IIC_READ /* 使用结构体读取变量时，可以直接通过结构体成员名进行读取 */
#define _API_IIC_READ(name) API_ReadData(I2CBaseAdr + (uint32_t)GetAdr(name) - I2CStartAdr, GetAdr(name), GetLen(name))
#endif

#ifndef IIC_SPEED
#define IIC_SPEED 100 /* 单位k  即100k的速度 */
#endif

#define SIMLT_IIC_TT 5 /* 模拟IIC使用 */

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

    typedef uint8_t (*iic_op)(uint8_t op); /* 模拟IIC使用 */
    typedef struct
    {
        iic_op sda_set;
        iic_op sda_out;
        iic_op sda_in;
        iic_op scl;
    } iic_op_type; /* 模拟IIC使用 */
    extern iic_op_type SIMLT_IIC;

    /**************************函数指针定义**************************/
    typedef void (*gy_iicinit)(void);
    typedef uint8_t (*gy_iicopcom)(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);
    typedef uint8_t (*gy_iicop)(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len);

    /****************************函数声明****************************/
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
