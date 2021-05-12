/*
*********************************************************************************************************
*
*	模块名称 : IIC_BSP程序
*	文件名称 : bsp_iic_simlt.c
*	版    本 : V1.0.2
*	说    明 : IIC操作
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-5-12   高屹    正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "bsp_iic_simlt.h"

// iic_simlt_type iic_exp = {
//     MONI_IIC_SDA_GPIO,
//     MONI_IIC_SDA_PIN,
//     MONI_IIC_SCL_GPIO,
//     MONI_IIC_SCL_PIN,
//     OutputIO,
//     InputtIO,
//     GPIO_SetBits,
//     GPIO_ResetBits,
//     GPIO_ReadInputDataBit};
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_op_sda_out
*	功能说明: IIC_SDA输出
*	形    参: op：规定：非0为设置输出高 否则设置输出低
*	返 回 值: 随意
*	备    注：无
*********************************************************************************************************
*/
void simlt_iic_op_sda_out(iic_simlt_type *io, uint8_t op)
{
    (op) ? (io->io_set(io->sda_io, io->sda_pin)) : (io->io_reset(io->sda_io, io->sda_pin));
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_op_sda_in
*	功能说明: IIC_SDA输入
*	形    参: op：随意
*	返 回 值: SDA读入的值
*	备    注：无
*********************************************************************************************************
*/
uint8_t simlt_iic_op_sda_in(iic_simlt_type *io)
{
    return io->io_read(io->sda_io, io->sda_pin);
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_op_scl_out
*	功能说明: IIC_SCL输出
*	形    参: op：规定：非0为设置输出高 否则设置输出低
*	返 回 值: 随意
*	备    注：无
*********************************************************************************************************
*/
void simlt_iic_op_scl_out(iic_simlt_type *io, uint8_t op)
{
    (op) ? (io->io_set(io->scl_io, io->scl_pin)) : (io->io_reset(io->scl_io, io->scl_pin));
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_op_sda_set
*	功能说明: 设置IIC_SDA输入输出
*	形    参: op：规定：非0为设置输出 否则设置输入
*	返 回 值: 随意
*	备    注：无
*********************************************************************************************************
*/
void simlt_iic_op_sda_set(iic_simlt_type *io, uint8_t op)
{
    (op) ? (io->io_out(io->sda_io, io->sda_pin, 0)) : (io->io_in(io->sda_io, io->sda_pin, 1));
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_op_init
*	功能说明: 使用模拟IIC前必须调用该函数
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void simlt_iic_op_init(iic_simlt_type *io)
{
    io->io_out(io->sda_io, io->sda_pin, 0);
    io->io_out(io->scl_io, io->scl_pin, 0);
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_start
*	功能说明: 起始信号
*	形    参: 无
*	返 回 值: 无
*	备    注：当时钟线和数据线都为高电平时，IIC总线上的所有从设备都处于空闲状态。
*            当时钟线和数据线都为高电平时，数据线从高电平到低电平的跳动，被定义为起始信号。
*********************************************************************************************************
*/
void simlt_iic_start(iic_simlt_type *io)
{
    simlt_iic_op_sda_set(io, 1); /* 设置SDA为输出 */
    simlt_iic_op_sda_out(io, 1); /* 发送起始条件的数据信号 */
    simlt_iic_op_scl_out(io, 1); /* 发送起始条件的时钟信号 */
    delay_us(SIMLT_IIC_TT);      /* 起始条件建立时间大于4us */
    simlt_iic_op_sda_out(io, 0); /* 发送起始信号 */
    delay_us(SIMLT_IIC_TT);      /* 起始条件锁定时间大于4μs */
    simlt_iic_op_scl_out(io, 0); /* 钳住I2C总线，准备发送或接收数据 */
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_stop
*	功能说明: 停止信号
*	形    参: 无
*	返 回 值: 无
*	备    注：当时钟线为高电平，数据线为低电平时，数据线从低到高的跳动，被定义为停止信号。
*********************************************************************************************************
*/
void simlt_iic_stop(iic_simlt_type *io)
{
    simlt_iic_op_sda_set(io, 1); /* 设置SDA为输出 */
    simlt_iic_op_sda_out(io, 0); /* 发送结束条件的数据信号 */
    simlt_iic_op_scl_out(io, 0); /* 发送结束条件的时钟信号 */
    delay_us(SIMLT_IIC_TT);      /* 结束条件建立时间大于4μs */
    simlt_iic_op_scl_out(io, 1);
    delay_us(SIMLT_IIC_TT);      /* 释放总线时间大于4μs */
    simlt_iic_op_sda_out(io, 1); /* 发送I2C总线结束信号 */
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_ack
*	功能说明: 主机发送ACK应答
*	形    参: a:应答类型，0表示应答信号，非0表示非应答信号。
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void simlt_iic_ack(iic_simlt_type *io, uint8_t a)
{
    simlt_iic_op_sda_set(io, 1);                                              /* 设置SDA为输出 */
    (a == 0) ? (simlt_iic_op_sda_out(io, 0)) : (simlt_iic_op_sda_out(io, 1)); /*在此发出应答或非应答信号 */
    delay_us(1);
    simlt_iic_op_scl_out(io, 1);
    delay_us(SIMLT_IIC_TT);      /*时钟低电平周期大于4μs*/
    simlt_iic_op_scl_out(io, 0); /*清时钟线，钳住I2C总线以便继续接收*/
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_test_ack
*	功能说明: 等待应答信号到来
*	形    参: 无
*	返 回 值: 0表示正常应答，1表示不正确应答
*	备    注：无
*********************************************************************************************************
*/
uint8_t simlt_iic_test_ack(iic_simlt_type *io)
{
    uint8_t outTime = 0;
    simlt_iic_op_sda_set(io, 1); /* 设置SDA为输出 */
    simlt_iic_op_sda_out(io, 1);
    delay_us(1); /* 此条后SDA电平已经无变化 */
    simlt_iic_op_scl_out(io, 1);
    delay_us(1);
    simlt_iic_op_sda_set(io, 0); /* SDA改为输入 */
    delay_us(SIMLT_IIC_TT);
    while (simlt_iic_op_sda_in(io))
    {
        outTime++;
        delay_us(1);
        if (outTime > 10)
        {
            simlt_iic_stop(io);
            return 1;
        }
    }
    simlt_iic_op_scl_out(io, 0);
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_send_byte
*	功能说明: 主机发送数据信号
*	形    参: c:要发送的一字节数据
*	返 回 值: 0表示正常应答，1表示不正确应答
*	备    注：时钟线高电平和数据线为低电平时，当时钟线拉低之后，IIC从设备会收到一个数据0；
*             时钟线高电平和数据线为高电平时，当时钟线拉低之后，IIC从设备会收到一个数据1。
*********************************************************************************************************
*/
uint8_t simlt_iic_send_byte(iic_simlt_type *io, uint8_t c)
{
    volatile uint8_t Bit;
    simlt_iic_op_sda_set(io, 1);  /* 设置SDA为输出 */
    for (Bit = 0; Bit < 8; Bit++) /* 要传送的数据长度为8位 */
    {
        ((c << Bit) & 0x80) ? (simlt_iic_op_sda_out(io, 1)) : (simlt_iic_op_sda_out(io, 0)); /* 判断发送位 */
        delay_us(1);
        simlt_iic_op_scl_out(io, 1); /* 置时钟线为高，通知被控器开始接收数据位 */
        delay_us(SIMLT_IIC_TT);      /* 保证时钟高电平周期大于4μs */
        simlt_iic_op_scl_out(io, 0); /* 置时钟线为低，被控器接收数据位 */
    }
    return simlt_iic_test_ack(io);
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_rcv_byte
*	功能说明: 主机发送数据信号
*	形    参: ack=1时，发送ACK，ack=0，发送nACK
*	返 回 值: 无
*	备    注：时钟线高电平和数据线为低电平时，当时钟线拉低之后，读到一个数据0；
*             时钟线高电平和数据线为高电平时，当时钟线拉低之后，读到一个数据1。
*********************************************************************************************************
*/
uint8_t simlt_iic_rcv_byte(iic_simlt_type *io, uint8_t ack)
{
    uint8_t ch, BitCnt;
    ch = 0;
    simlt_iic_op_sda_set(io, 0); /* 置数据线为输入方式 */
    for (BitCnt = 0; BitCnt < 8; BitCnt++)
    {
        simlt_iic_op_scl_out(io, 1);
        delay_us(1);
        ch <<= 1;
        if (simlt_iic_op_sda_in(io))
            ch++; /* 读数据位,接收的数据位放入retc中 */
        simlt_iic_op_scl_out(io, 0);
        delay_us(SIMLT_IIC_TT); /* 时钟低电平周期大于4μs */
    }
    (ack) ? (simlt_iic_ack(io, 0)) : (simlt_iic_ack(io, 1));
    return (ch);
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_op
*	功能说明: 模拟IIC通用操作
*	形    参: Device：设备地址
*             Addr：要写的数据地址
*             AddrLen：地址的长度
*             op：1 I2C读操作 else I2C写操作
*	返 回 值: 1：失败 0：成功   
*	备    注：无
*********************************************************************************************************
*/
uint8_t simlt_iic_op(iic_simlt_type *io, uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t op)
{
    uint8_t Devi_Addr = Device; /* 由于模拟发送会将数据移位清空，所以要另起变量 */
    uint16_t t_Addr = Addr;     /* 由于模拟发送会将数据移位清空，所以要另起变量 */

    simlt_iic_start(io);                 /* 启动总线 */
    if (simlt_iic_send_byte(io, Device)) /* 发送器件地址 */
        return 1;
    if (AddrLen > 1)
    {
        if (simlt_iic_send_byte(io, HIGHWORD(t_Addr))) /* 发送器件高子地址 */
            return 1;
    }
    if (simlt_iic_send_byte(io, LOWWORD(t_Addr))) /* 发送器件低子地址 */
        return 1;

    if (op == 1) /* 读操作 */
    {
        simlt_iic_start(io);                        /* 发送重起始位 */
        if (simlt_iic_send_byte(io, Devi_Addr + 1)) /* 发送器件地址,读取 */
            return 1;                               /* 判断是否接收到应答信号 */
    }
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_wait_end
*	功能说明: 模拟IIC通用操作
*	形    参: Device：设备地址
*	返 回 值: 1：失败 0：成功   
*	备    注：无
*********************************************************************************************************
*/
uint8_t simlt_iic_wait_end(iic_simlt_type *io, uint8_t Device)
{
    uint8_t result, Devi_Addr;

    Devi_Addr = Device;

    Do_DelayStart();
    {
        simlt_iic_start(io);                         /* 发送起始位 */
        result = simlt_iic_send_byte(io, Devi_Addr); /* 发送器件地址 */
        simlt_iic_stop(io);                          /* 发送停止位 */
        if (result == 0)
            return 0; /* 设置地址成功, 写周期结束 */
    }
    While_DelayMsEnd(8 * clkmode);

    return 1; /* 设置地址失败 */
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_write
*	功能说明: 模拟IIC通用写操作
*	形    参: Device：设备地址
*             Addr：要写的数据地址
*             AddrLen：地址的长度
*             *Buf：数据缓存
*             Len：数据长度
*	返 回 值: 0：成功；1：失败；2：超过一次写入的最大上限      
*	备    注：IIC_ChipWL 需要在APP_CFG.h中手动配置
*********************************************************************************************************
*/
uint8_t simlt_iic_write(iic_simlt_type *io, uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    /*这里的len不可能大于IIC_ChipWL*/
    uint8_t k = 0, status = 0;
    uint32_t n = 0;

    if (Len > IIC_ChipWL)
        return 2;
    simlt_iic_op_init(io);
    for (k = 0; k < 3; k++) //每遍最多写3次
    {
        status = 0;

        if (simlt_iic_op(io, Device, Addr, AddrLen, 0))
            status = 1; //写入失败
        else
        {
            for (n = 0; n < Len; n++)
            {
                //发送一个字节
                if (simlt_iic_send_byte(io, Buf[n]))
                {
                    status = 1; //写入失败
                    break;
                }
            }
        }
        //发送停止位
        simlt_iic_stop(io);
        if (status == 0)
        {
            simlt_iic_wait_end(io, Device);
            break;
        } //写正确
    }
    return (status);
}
/*
*********************************************************************************************************
*	函 数 名: simlt_iic_read
*	功能说明: 模拟IIC通用读操作
*	形    参: Device：设备地址
*             Addr：要写的数据地址
*             AddrLen：地址的长度
*             *Buf：数据缓存
*             Len：数据长度
*	返 回 值: 0：成功；1：失败  
*	备    注：无
*********************************************************************************************************
*/
uint8_t simlt_iic_read(iic_simlt_type *io, uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint8_t k = 0, status = 0;
    uint32_t n = 0;
    simlt_iic_op_init(io);
    for (k = 0; k < 3; k++) //每遍最多读3次
    {
        status = 0;

        if (simlt_iic_op(io, Device, Addr, AddrLen, 1))
            status = 1; //写入失败
        else
        {
            for (n = 0; n < Len; n++)
            {
                //接收一个字节
                (n < (Len - 1)) ? (Buf[n] = simlt_iic_rcv_byte(io, 1)) : (Buf[n] = simlt_iic_rcv_byte(io, 0));
            }
        }
        //发送停止位
        simlt_iic_stop(io);

        if (status == 0)
            break; //读正确
    }
    return (status);
}
/*
*********************************************************************************************************
*	函 数 名: API_SIMLT_IIC
*	功能说明: 模拟IIC操作
*	形    参: op:1:读 else：写
*             Device：设备地址
*             Addr：要写的数据地址
*             AddrLen：地址的长度
*             *Buf：数据缓存
*             Len：数据长度
*	返 回 值: 0：成功；1：失败  
*	备    注：无
*********************************************************************************************************
*/
uint8_t API_SIMLT_IIC(iic_simlt_type *io, uint8_t op, uint8_t Device, uint16_t Addr, uint8_t *Buf, uint16_t Len)
{
    uint8_t AddrLen = 1, status = 0;
    if (Addr > 0xff)
        AddrLen = 2;
    if (op)
    {
        status = simlt_iic_read(io, Device, Addr, AddrLen, Buf, Len);
    }
    else
    {
        status = simlt_iic_write(io, Device, Addr, AddrLen, Buf, Len);
    }
    return status;
}
