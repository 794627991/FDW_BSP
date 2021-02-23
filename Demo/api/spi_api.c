/*
*********************************************************************************************************
*
*	模块名称 : SPI_BSP程序
*	文件名称 : spi_api.c
*	版    本 : V1.0.0
*	说    明 : SPI操作
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-2-23   高屹    正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/

#include "spi_api.h"

spi_op_type SIMLT_SPI;
/*
*********************************************************************************************************
*	函 数 名: spi_op_mosi
*	功能说明: SPI MOSI 输出电平设置
*	形    参: op：规定：非0为设置输出高 否则设置输出低
*	返 回 值: 随意
*	备    注：无
*********************************************************************************************************
*/
__weak uint8_t spi_op_mosi(uint8_t op)
{
    (op) ? (MONI_SPI_MOSI_1) : (MONI_SPI_MOSI_0);
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: spi_op_miso
*	功能说明: SPI MISO读入
*	形    参: op：随意
*	返 回 值: MISO读入的值
*	备    注：无
*********************************************************************************************************
*/
__weak uint8_t spi_op_miso(uint8_t op)
{
    return MONI_SPI_MISO_READ;
}
/*
*********************************************************************************************************
*	函 数 名: spi_op_sck
*	功能说明: SPI SCK 输出
*	形    参: op：规定：非0为设置输出高 否则设置输出低
*	返 回 值: 随意
*	备    注：无
*********************************************************************************************************
*/
__weak uint8_t spi_op_sck(uint8_t op)
{
    (op) ? (MONI_SPI_SCK_1) : (MONI_SPI_SCK_0);
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: spi_op_nss
*	功能说明: SPI NSS 输出
*	形    参: op：规定：非0为设置输出 否则设置输出低
*	返 回 值: 随意
*	备    注：无
*********************************************************************************************************
*/
__weak uint8_t spi_op_nss(uint8_t op)
{
    (op) ? (MONI_SPI_NSS_1) : (MONI_SPI_NSS_0);
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: SOFT_SPI_RW_MODE0
*	功能说明: SPI MODE0 
*	形    参: write_dat：写入的数据
*	返 回 值: 读取到的数据
*	备    注：CPOL = 0, CPHA = 0, MSB first
*********************************************************************************************************
*/
uint8_t SOFT_SPI_RW_MODE0(uint8_t write_dat)
{
    uint8_t i, read_dat;
    for (i = 0; i < 8; i++)
    {
        if (write_dat & 0x80)
            SIMLT_SPI.mosi(1);
        else
            SIMLT_SPI.mosi(0);
        write_dat <<= 1;
        delay_us(1);
        SIMLT_SPI.sck(1);
        read_dat <<= 1;
        if (SIMLT_SPI.mosi(0))
            read_dat++;
        delay_us(1);
        SIMLT_SPI.sck(0);
        __nop();
    }

    return read_dat;
}
/*
*********************************************************************************************************
*	函 数 名: SOFT_SPI_RW_MODE1
*	功能说明: SPI MODE1 
*	形    参: write_dat：写入的数据
*	返 回 值: 读取到的数据
*	备    注：CPOL=0，CPHA=1, MSB first
*********************************************************************************************************
*/
uint8_t SOFT_SPI_RW_MODE1(uint8_t write_dat)
{
    uint8_t i, read_dat = 0;

    for (i = 0; i < 8; i++) // 循环8次
    {
        SIMLT_SPI.sck(1); //拉高时钟
        if (write_dat & 0x80)
        {
            SIMLT_SPI.mosi(1); //若最到位为高，则输出高
        }
        else
        {
            SIMLT_SPI.mosi(0); //若最到位为低，则输出低
        }
        write_dat <<= 1; // 低一位移位到最高位
        delay_us(1);
        SIMLT_SPI.sck(0); //拉低时钟
        read_dat <<= 1;   //数据左移

        if (SIMLT_SPI.mosi(0))
            read_dat++; //若从从机接收到高电平，数据自加一
        delay_us(1);
    }
    return read_dat; //返回数据
}
/*
*********************************************************************************************************
*	函 数 名: SOFT_SPI_RW_MODE2
*	功能说明: SPI MODE2 
*	形    参: write_dat：写入的数据
*	返 回 值: 读取到的数据
*	备    注：CPOL=1，CPHA=0, MSB first
*********************************************************************************************************
*/
uint8_t SOFT_SPI_RW_MODE2(uint8_t write_dat)
{
    uint8_t i, read_dat = 0;

    for (i = 0; i < 8; i++) // 循环8次
    {
        if (write_dat & 0x80)
        {
            SIMLT_SPI.mosi(1); //若最到位为高，则输出高
        }
        else
        {
            SIMLT_SPI.mosi(0); //若最到位为低，则输出低
        }
        write_dat <<= 1; // 低一位移位到最高位
        delay_us(1);
        SIMLT_SPI.sck(0); //拉低时钟
        read_dat <<= 1;   //数据左移

        if (SIMLT_SPI.mosi(0))
            read_dat++; //若从从机接收到高电平，数据自加一
        delay_us(1);
        SIMLT_SPI.sck(1); //拉高时钟
    }
    return read_dat; //返回数据
}
/*
*********************************************************************************************************
*	函 数 名: SOFT_SPI_RW_MODE3
*	功能说明: SPI MODE3 
*	形    参: write_dat：写入的数据
*	返 回 值: 读取到的数据
*	备    注：CPOL = 1, CPHA = 1, MSB first
*********************************************************************************************************
*/
uint8_t SOFT_SPI_RW_MODE3(uint8_t write_dat)
{
    uint8_t i, read_dat;
    for (i = 0; i < 8; i++)
    {
        SIMLT_SPI.sck(0);
        if (write_dat & 0x80)
            SIMLT_SPI.mosi(1);
        else
            SIMLT_SPI.mosi(0);
        write_dat <<= 1;
        delay_us(1);
        SIMLT_SPI.sck(1);
        read_dat <<= 1;
        if (SIMLT_SPI.mosi(0))
            read_dat++;
        delay_us(1);
        __nop();
    }
    return read_dat;
}

/*
*********************************************************************************************************
*	函 数 名: SIMLT_SPI_OP_INIT
*	功能说明: 使用模拟SPI前必须调用该函数
*	形    参: mode:0-3
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void SIMLT_SPI_OP_INIT(uint8_t mode)
{
    MONI_SPI_MOSI_DIR;
    MONI_SPI_MISO_DIR;
    MONI_SPI_SCK_DIR;
    MONI_SPI_NSS_DIR;
    SIMLT_SPI.mosi = spi_op_mosi;
    SIMLT_SPI.miso = spi_op_miso;
    SIMLT_SPI.sck = spi_op_sck;
    SIMLT_SPI.nss = spi_op_nss;
    if (1 == mode)
        SIMLT_SPI.mode = SOFT_SPI_RW_MODE1;
    else if (2 == mode)
        SIMLT_SPI.mode = SOFT_SPI_RW_MODE2;
    else if (3 == mode)
        SIMLT_SPI.mode = SOFT_SPI_RW_MODE3;
    else
        SIMLT_SPI.mode = SOFT_SPI_RW_MODE0;
}
/*
*********************************************************************************************************
*	函 数 名: API_SIMLT_I2C
*	功能说明: 模拟SPI
*	形    参: *buf：读写数据缓存
*             len：数据长度
*	返 回 值: 无 
*	备    注：IIC_ChipWL 需要在APP_CFG.h中手动配置
*********************************************************************************************************
*/
void API_SIMLT_I2C(uint8_t *buf, uint32_t len)
{
    uint64_t i;
    SIMLT_SPI.nss(0);
    for (i = 0; i < len; i++)
    {
        buf[i] = SIMLT_SPI.mode(&buf[i]);
    }
    SIMLT_SPI.nss(1);
}
