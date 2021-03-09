/*
*********************************************************************************************************
*
*	模块名称 : IIC_BSP程序
*	文件名称 : bsp_iic.c
*	版    本 : V1.0.0
*	说    明 : IIC操作
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-4    高屹    正式发布
*		V1.0.1  2021-1-25   高屹    增加模拟IIC操作
*		V1.0.3  2021-2-1    高屹    API_EPROM操作支持到24V10，地址可大于0xffff
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "bsp_iic.h"

#if API_UseIIC > 0

uint8_t IICLOCK = 0; /* iic锁，伪进程安全保护 */
IICBase IICDATA;

/*
*********************************************************************************************************
*	函 数 名: API_I2CSave
*	功能说明: 硬件IIC存储
*	形    参: base：地址
*             *Buf：数据缓存
*             len：长度
*	返 回 值: 无
*	备    注：适配EEPROM，详情看API_EEPROM_Write的说明
*********************************************************************************************************
*/
void API_I2CSave(uint32_t base, uint8_t *Buf, uint32_t len)
{
    if (IICLOCK == 0)
    {
        IICLOCK = 1;
        API_I2C_Init();
        /* __disable_irq(); */
        I2C_Write(base, Buf, len);
        // if (base != I2CBaseAdr)
        // {
        //     I2C_Write(base, Buf, len);
        // }
        // else
        // {
        //     I2C_Write(base + (uint32_t)Buf - I2CStartAdr, Buf, len);
        // }
        /* __enable_irq(); */
        IICLOCK = 0;
        CloseeIO(GPIOA, GPIO_Pin_14);
        CloseeIO(GPIOA, GPIO_Pin_15);
    }
}
/*
*********************************************************************************************************
*	函 数 名: API_I2CRead
*	功能说明: 硬件IIC读取
*	形    参: base：地址
*             *Buf：数据缓存
*             len：长度
*	返 回 值: 无
*	备    注：适配EEPROM，详情看API_EEPROM_Read的说明
*********************************************************************************************************
*/
void API_I2CRead(uint32_t base, uint8_t *Buf, uint32_t len)
{
    if (IICLOCK == 0)
    {
        IICLOCK = 1;
        API_I2C_Init();
        /* __disable_irq(); */
        I2C_Read(base, Buf, len);
        // if (base != I2CBaseAdr)
        // {
        //     I2C_Read(base, Buf, len);
        // }
        // else
        // {
        //     I2C_Read(base + (uint32_t)Buf - I2CStartAdr, Buf, len);
        // }
        /* __enable_irq(); */
        IICLOCK = 0;
        CloseeIO(GPIOA, GPIO_Pin_14);
        CloseeIO(GPIOA, GPIO_Pin_15);
    }
}
/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 硬件IIC操作
*	形    参: 
*	返 回 值: 
*	备    注：库函数，说明略
*********************************************************************************************************
*/
uint8_t G_I2C_Send_Bit(uint8_t BIT_def)
{
    uint32_t i;

    I2C_I2CCTRL_I2CEN_Setable(ENABLE); //使能I2C电路

    switch (BIT_def)
    {
    case STARTBIT:
        I2C_SEND_STARTBIT();
        break;

    case RESTARTBIT:
        I2C_SEND_RESTARTBIT();
        break;

    case STOPBIT:
        I2C_SEND_STOPBIT();
        break;

    case ACKBIT:
        I2C_SEND_ACKBIT();
        break;

    default:
        break;
    }

    for (i = 0; i < 2 * SSP_TBIT; i++)
    {
        if (SET == I2C_I2CIR_I2CIF_Chk())
            break;
    }

    if (i < 2 * SSP_TBIT)
    {
        I2C_I2CIR_I2CIF_Clr();   //clr int flag
        i = I2C_I2CSTA_RW_Chk(); //clr flag through reading SSPSTA.

        return 0; //ok
    }
    else
    {
        return 1; //failure.
    }
}
/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 硬件IIC操作
*	形    参: 
*	返 回 值: 
*	备    注：库函数，说明略
*********************************************************************************************************
*/
uint8_t G_I2C_Send_Byte(uint8_t x_byte)
{
    uint16_t i;

    I2C_I2CBUF_Write(x_byte); //写发送缓冲寄存器

    for (i = 0; i < 18 * SSP_TBIT; i++)
    {
        if (SET == I2C_I2CIR_I2CIF_Chk())
            break; //中断标志置位时跳出循环
    }

    if (i < 18 * SSP_TBIT)
    {
        I2C_I2CIR_I2CIF_Clr(); //clr int flag

        if (SET == I2C_I2CSTA_ACKSTA_Chk()) //从机无应答
        {
            return 2; //失败
        }
        else
        {
            return 0; //成功
        }
    }
    else
    {
        return 1; //failure.
    }
}
/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 硬件IIC操作
*	形    参: 
*	返 回 值: 
*	备    注：库函数，说明略
*********************************************************************************************************
*/
uint8_t G_I2C_Receive_Byte(uint8_t *x_byte)
{
    uint16_t i;

    //i2c en, rcen
    I2C_I2CCTRL_RCEN_Setable(ENABLE);

    for (i = 0; i < 18 * SSP_TBIT; i++)
    {
        if (SET == I2C_I2CIR_I2CIF_Chk())
            break; //中断标志置位, 跳出循环
    }

    if (i < 18 * SSP_TBIT)
    {
        *x_byte = I2C_I2CBUF_Read();

        I2C_I2CIR_I2CIF_Clr(); //clr int flag

        return 0;
    }
    else
    {
        return 1; //failure.
    }
}
/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 硬件IIC操作
*	形    参: 
*	返 回 值: 
*	备    注：库函数，说明略
*********************************************************************************************************
*/
uint8_t G_Sendaddr(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t Opt)
{
    uint8_t result, Devi_Addr;

    Devi_Addr = Device;

    //-------------- reset i2c -------------
    I2C_ResetI2C(); //复位I2C

    //-------------- start bit -------------
    result = G_I2C_Send_Bit(STARTBIT); //发送起始位
    if (result != 0)
        return 1; //failure.

    //-------------- disable read -------------
    I2C_I2CCTRL_RCEN_Setable(DISABLE);

    //-------------- device addr -------------
    result = G_I2C_Send_Byte(Devi_Addr); //发送器件地址
    if (result != 0)
        return 2; //failure.

    //--------------- data addr --------------
    if (AddrLen == 2)
    {
        result = G_I2C_Send_Byte(Addr >> 8); //发送数据地址高8位
        if (result != 0)
            return 3; //failure.
    }
    result = G_I2C_Send_Byte(Addr >> 0); //发送数据地址低8位
    if (result != 0)
        return 3; //failure.

    if (Opt == I2CREAD) //读操作
    {
        result = G_I2C_Send_Bit(RESTARTBIT); //发送重起始位
        if (result != 0)
            return 5; //failure.

        result = G_I2C_Send_Byte(Devi_Addr | 1); //发送器件地址,读取
        if (result != 0)
            return 5; //failure.
    }

    return 0; //ok
}
/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 硬件IIC操作
*	形    参: 
*	返 回 值: 
*	备    注：库函数，说明略
*********************************************************************************************************
*/
uint8_t G_Wait_for_end(uint8_t Device)
{
    uint8_t result, Devi_Addr;

    Devi_Addr = Device;

    Do_DelayStart();
    {
        G_I2C_Send_Bit(STARTBIT); //发送起始位

        result = G_I2C_Send_Byte(Devi_Addr); //发送器件地址

        G_I2C_Send_Bit(STOPBIT); //发送停止位

        if (result == 0)
            return 0; //设置地址成功, 写周期结束
    }
    While_DelayMsEnd(8 * clkmode);

    return 1; //设置地址失败
}
/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 硬件IIC操作
*	形    参: 
*	返 回 值: 
*	备    注：库函数，说明略
*********************************************************************************************************
*/
uint8_t G_I2C_Write_Bottom(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint8_t k = 0, status = 0;
    uint32_t n = 0;
    for (n = 0; n < Len; n++)
    {
        for (k = 0; k < 3; k++) //每遍最多写3次
        {
            status = 0;
            if (G_Sendaddr(Device, Addr, AddrLen, I2CWRITE))
                status = 1; //写入失败
            else
            {
                //for( n=0; n<Len; n++ )
                {
                    //发送一个字节
                    if (G_I2C_Send_Byte(Buf[n]))
                    {
                        status = 1;
                        break;
                    } //写入失败
                }
            }
            //发送停止位
            if (G_I2C_Send_Bit(STOPBIT))
                status = 1;

            if (status == 0)
            {
                G_Wait_for_end(Device);
                break;
            } //写正确
        }
        Addr++;
    }

    return (status);
}
/*
*********************************************************************************************************
*	函 数 名: API_I2C_Write_Com
*	功能说明: 硬件IIC通用写操作
*	形    参: Device：设备地址
*             Addr：要写的数据地址
*             AddrLen：地址的长度
*             *Buf：数据缓存
*             Len：数据长度
*	返 回 值: 0：成功；1：失败；2：超过一次写入的最大上限      
*	备    注：IIC_ChipWL 需要在APP_CFG.h中手动配置
*********************************************************************************************************
*/
__weak uint8_t API_I2C_Write_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    /*这里的len不可能大于IIC_ChipWL*/
    uint8_t k = 0, status = 0;
    uint32_t n = 0;
    if (Len > IIC_ChipWL)
        return 2;

    for (k = 0; k < 3; k++) //每遍最多写3次
    {
        status = 0;
        if (G_Sendaddr(Device, Addr, AddrLen, I2CWRITE))
            status = 1; //写入失败
        else
        {
            for (n = 0; n < Len; n++)
            {
                //发送一个字节
                if (G_I2C_Send_Byte(Buf[n]))
                {
                    status = 1;
                    break;
                } //写入失败
            }
        }
        //发送停止位
        if (G_I2C_Send_Bit(STOPBIT))
            status = 1;

        if (status == 0)
        {
            G_Wait_for_end(Device);
            break;
        } //写正确
    }
    return (status);
}
/*
*********************************************************************************************************
*	函 数 名: API_I2C_Read_Com
*	功能说明: 硬件IIC通用读操作
*	形    参: Device：设备地址
*             Addr：要写的数据地址
*             AddrLen：地址的长度
*             *Buf：数据缓存
*             Len：数据长度
*	返 回 值: 0：成功；1：失败  
*	备    注：无
*********************************************************************************************************
*/
__weak uint8_t API_I2C_Read_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint8_t k = 0, status = 0;
    uint32_t n = 0;

    for (k = 0; k < 3; k++) //每遍最多读3次
    {
        status = 0;

        if (G_Sendaddr(Device, Addr, AddrLen, I2CREAD))
            status = 1; //写入失败
        else
        {
            for (n = 0; n < Len; n++)
            {
                //接收一个字节
                if (G_I2C_Receive_Byte(Buf + n))
                {
                    status = 1;
                    break;
                }

                if (n < (Len - 1))
                    I2C_SEND_ACK_0(); //主机将给从机的回应为0
                else
                    I2C_SEND_ACK_1(); //主机将给从机的回应为1

                //发送 ACK
                if (G_I2C_Send_Bit(ACKBIT)) //失败
                {
                    I2C_SEND_ACK_0();
                    status = 1;
                    break;
                }
                I2C_SEND_ACK_0();
            }
        }
        //发送停止位
        if (G_I2C_Send_Bit(STOPBIT))
            status = 1; //失败

        if (status == 0)
            break; //读正确
    }

    return (status);
}
/*
*********************************************************************************************************
*	函 数 名: API_EEPROM_Write
*	功能说明: 硬件IIC适配EEPROM专用
*	形    参: Device：设备地址
*             Addr：要写的数据地址
*             AddrLen：地址的长度
*             *Buf：数据缓存
*             Len：数据长度
*	返 回 值: 0：成功；1：失败；2：超过一次写入的最大上限 
*	备    注：对24C02/04及其以上进行专门适配，根据器件地址长度自行判断使用的是C02/04还是以上
*            可以连续写操作
*********************************************************************************************************
*/
__weak uint8_t API_EEPROM_Write(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint32_t tAdr = Addr, sendlen = 0;
    uint8_t status = 0;
    uint16_t needlen = 0; /* 可以发送的长度，已经发送的长度 */

    for (;;)
    {
        if ((AddrLen == 1) && (tAdr >= 0x100))
        {
            Device |= 0x02;
            tAdr = tAdr & 0xff;
        }
        else if ((AddrLen == 2) && (tAdr >= 0x10000))
        {
            Device |= 0x02;
            tAdr = tAdr & 0xffff;
        }

        needlen = IIC_ChipWL - (tAdr % IIC_ChipWL); /* 计算剩余可发送的长度 */
        if (sendlen + needlen >= Len)
        {
            needlen = Len - sendlen;
        }

        status = API_I2C_Write_Com(Device, tAdr, AddrLen, &Buf[sendlen], needlen);
        if (status != 0)
            return status;

        tAdr += needlen;
        sendlen += needlen;
        if (sendlen >= Len)
        {
            break;
        }
    }
    return status;
}
/*
*********************************************************************************************************
*	函 数 名: API_EEPROM_Read
*	功能说明: 硬件IIC适配EEPROM专用
*	形    参: Device：设备地址
*             Addr：要写的数据地址
*             AddrLen：地址的长度
*             *Buf：数据缓存
*             Len：数据长度
*	返 回 值: 0：成功；1：失败
*	备    注：对24C02/04及其以上进行专门适配，根据器件地址长度自行判断使用的是C02/04还是以上
*            可以连续读操作
*********************************************************************************************************
*/
__weak uint8_t API_EEPROM_Read(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint8_t status = 0;
    uint16_t rlen = 0;
    uint32_t tAdr = Addr;

    if ((AddrLen == 1) && (tAdr >= 0x100))
    {
        Device |= 0x02;
        tAdr = tAdr & 0xff;
        status = API_I2C_Read_Com(Device, Addr, AddrLen, Buf, Len);
    }
    else if ((AddrLen == 2) && (tAdr >= 0x10000))
    {
        Device |= 0x02;
        tAdr = tAdr & 0xffff;
        status = API_I2C_Read_Com(Device, Addr, AddrLen, Buf, Len);
    }
    else
    {
        if ((AddrLen == 1) && ((tAdr + Len) >= 0x100))
        {
            status = API_I2C_Read_Com(Device, Addr, AddrLen, Buf, 0x100 - Addr);
            Device |= 2;
            rlen = Addr + Len - 0x100;
            status = API_I2C_Read_Com(Device, 0, AddrLen, Buf + 0x100 - Addr, rlen);
        }
        else if ((AddrLen == 2) && (tAdr + Len >= 0x10000))
        {
            status = API_I2C_Read_Com(Device, Addr, AddrLen, Buf, 0x10000 - Addr);
            Device |= 2;
            rlen = Addr + Len - 0x10000;
            status = API_I2C_Read_Com(Device, 0, AddrLen, Buf + 0x10000 - Addr, rlen);
        }
        else
        {
            status = API_I2C_Read_Com(Device, Addr, AddrLen, Buf, Len);
        }
    }
    return status;
}
/*
*********************************************************************************************************
*	函 数 名: API_I2C_Init
*	功能说明: 硬件IIC初始化
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void API_I2C_Init(void)
{
    RCC_PERCLK_SetableEx(I2CCLK, ENABLE); /* I2C时钟使能 */

    AltFunIO(GPIOA, GPIO_Pin_14, ALTFUN_OPENDRAIN); /* PA14 SCL */
    AltFunIO(GPIOA, GPIO_Pin_15, ALTFUN_OPENDRAIN); /* PA15 SDA */

    I2C_I2CBRG_Write(I2C_BaudREG_Calc(IIC_SPEED * 1000, 8000000 * clkmode)); /* 100k@8M */
    I2C_I2CIR_I2CIE_Setable(DISABLE);                                        /* 关闭I2C中断 */
    I2C_I2CERR_ERRIE_Setable(DISABLE);                                       /* 关闭I2C错误中断 */

    NVIC_DisableIRQ(I2C_IRQn);
}

#endif

iic_op_type SIMLT_IIC;
/*
*********************************************************************************************************
*	函 数 名: iic_op_sda_out
*	功能说明: IIC_SDA输出
*	形    参: op：规定：非0为设置输出高 否则设置输出低
*	返 回 值: 随意
*	备    注：无
*********************************************************************************************************
*/
__weak uint8_t iic_op_sda_out(uint8_t op)
{
    (op) ? (MONI_IIC_SDA_1) : (MONI_IIC_SDA_0);
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: iic_op_sda_in
*	功能说明: IIC_SDA输入
*	形    参: op：随意
*	返 回 值: SDA读入的值
*	备    注：无
*********************************************************************************************************
*/
__weak uint8_t iic_op_sda_in(uint8_t op)
{
    return MONI_IIC_SDA_READ;
}
/*
*********************************************************************************************************
*	函 数 名: iic_op_scl
*	功能说明: IIC_SCL输出
*	形    参: op：规定：非0为设置输出高 否则设置输出低
*	返 回 值: 随意
*	备    注：无
*********************************************************************************************************
*/
__weak uint8_t iic_op_scl(uint8_t op)
{
    (op) ? (MONI_IIC_SCL_1) : (MONI_IIC_SCL_0);
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: iic_op_sda_set
*	功能说明: 设置IIC_SDA输入输出
*	形    参: op：规定：非0为设置输出 否则设置输入
*	返 回 值: 随意
*	备    注：无
*********************************************************************************************************
*/
__weak uint8_t iic_op_sda_set(uint8_t op)
{
    (op) ? (MONI_IIC_SDA_DIR_OUT) : (MONI_IIC_SDA_DIR_IN);
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: SIMLT_I2C_OP_INIT
*	功能说明: 使用模拟IIC前必须调用该函数
*	形    参: 无
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
__weak void SIMLT_I2C_OP_INIT(void)
{
    MONI_IIC_SDA_DIR_OUT;
    MONI_IIC_SCL_DIR_OUT;
    SIMLT_IIC.sda_set = iic_op_sda_set;
    SIMLT_IIC.sda_out = iic_op_sda_out;
    SIMLT_IIC.sda_in = iic_op_sda_in;
    SIMLT_IIC.scl = iic_op_scl;
}
/*
*********************************************************************************************************
*	函 数 名: Start_I2c
*	功能说明: 起始信号
*	形    参: 无
*	返 回 值: 无
*	备    注：当时钟线和数据线都为高电平时，IIC总线上的所有从设备都处于空闲状态。
*            当时钟线和数据线都为高电平时，数据线从高电平到低电平的跳动，被定义为起始信号。
*********************************************************************************************************
*/
void Start_I2c(void)
{
    SIMLT_IIC.sda_set(1);   /* 设置SDA为输出 */
    SIMLT_IIC.sda_out(1);   /* 发送起始条件的数据信号 */
    SIMLT_IIC.scl(1);       /* 发送起始条件的时钟信号 */
    delay_us(SIMLT_IIC_TT); /* 起始条件建立时间大于4us */
    SIMLT_IIC.sda_out(0);   /* 发送起始信号 */
    delay_us(SIMLT_IIC_TT); /* 起始条件锁定时间大于4μs */
    SIMLT_IIC.scl(0);       /* 钳住I2C总线，准备发送或接收数据 */
}
/*
*********************************************************************************************************
*	函 数 名: Stop_I2c
*	功能说明: 停止信号
*	形    参: 无
*	返 回 值: 无
*	备    注：当时钟线为高电平，数据线为低电平时，数据线从低到高的跳动，被定义为停止信号。
*********************************************************************************************************
*/
void Stop_I2c(void)
{
    SIMLT_IIC.sda_set(1);   /* 设置SDA为输出 */
    SIMLT_IIC.sda_out(0);   /* 发送结束条件的数据信号 */
    SIMLT_IIC.scl(0);       /* 发送结束条件的时钟信号 */
    delay_us(SIMLT_IIC_TT); /* 结束条件建立时间大于4μs */
    SIMLT_IIC.scl(1);
    delay_us(SIMLT_IIC_TT); /* 释放总线时间大于4μs */
    SIMLT_IIC.sda_out(1);   /* 发送I2C总线结束信号 */
}
/*
*********************************************************************************************************
*	函 数 名: Ack_I2c
*	功能说明: 主机发送ACK应答
*	形    参: a:应答类型，0表示应答信号，非0表示非应答信号。
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void Ack_I2c(uint8_t a)
{
    SIMLT_IIC.sda_set(1);                                       /* 设置SDA为输出 */
    (a == 0) ? (SIMLT_IIC.sda_out(0)) : (SIMLT_IIC.sda_out(1)); /*在此发出应答或非应答信号 */
    delay_us(1);
    SIMLT_IIC.scl(1);
    delay_us(SIMLT_IIC_TT); /*时钟低电平周期大于4μs*/
    SIMLT_IIC.scl(0);       /*清时钟线，钳住I2C总线以便继续接收*/
}
/*
*********************************************************************************************************
*	函 数 名: TestAck
*	功能说明: 等待应答信号到来
*	形    参: 无
*	返 回 值: 0表示正常应答，1表示不正确应答
*	备    注：无
*********************************************************************************************************
*/
uint8_t TestAck(void)
{
    uint8_t outTime = 0;
    SIMLT_IIC.sda_set(1); /* 设置SDA为输出 */
    SIMLT_IIC.sda_out(1);
    delay_us(1); /* 此条后SDA电平已经无变化 */
    SIMLT_IIC.scl(1);
    delay_us(1);
    SIMLT_IIC.sda_set(0); /* SDA改为输入 */
    delay_us(SIMLT_IIC_TT);
    while (SIMLT_IIC.sda_in(0))
    {
        outTime++;
        delay_us(1);
        if (outTime > 10)
        {
            Stop_I2c();
            return 1;
        }
    }
    SIMLT_IIC.scl(0);
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: SendByte
*	功能说明: 主机发送数据信号
*	形    参: c:要发送的一字节数据
*	返 回 值: 0表示正常应答，1表示不正确应答
*	备    注：时钟线高电平和数据线为低电平时，当时钟线拉低之后，IIC从设备会收到一个数据0；
*             时钟线高电平和数据线为高电平时，当时钟线拉低之后，IIC从设备会收到一个数据1。
*********************************************************************************************************
*/
uint8_t SendByte(uint8_t c)
{
    volatile uint8_t Bit;
    SIMLT_IIC.sda_set(1);         /* 设置SDA为输出 */
    for (Bit = 0; Bit < 8; Bit++) /* 要传送的数据长度为8位 */
    {
        ((c << Bit) & 0x80) ? (SIMLT_IIC.sda_out(1)) : (SIMLT_IIC.sda_out(0)); /* 判断发送位 */
        delay_us(1);
        SIMLT_IIC.scl(1);       /* 置时钟线为高，通知被控器开始接收数据位 */
        delay_us(SIMLT_IIC_TT); /* 保证时钟高电平周期大于4μs */
        SIMLT_IIC.scl(0);       /* 置时钟线为低，被控器接收数据位 */
    }
    return TestAck();
}
/*
*********************************************************************************************************
*	函 数 名: SendByte
*	功能说明: 主机发送数据信号
*	形    参: ack=1时，发送ACK，ack=0，发送nACK
*	返 回 值: 无
*	备    注：时钟线高电平和数据线为低电平时，当时钟线拉低之后，读到一个数据0；
*             时钟线高电平和数据线为高电平时，当时钟线拉低之后，读到一个数据1。
*********************************************************************************************************
*/
uint8_t RcvByte(uint8_t ack)
{
    uint8_t ch, BitCnt;
    ch = 0;
    SIMLT_IIC.sda_set(0); /* 置数据线为输入方式 */
    for (BitCnt = 0; BitCnt < 8; BitCnt++)
    {
        SIMLT_IIC.scl(1);
        delay_us(1);
        ch <<= 1;
        if (SIMLT_IIC.sda_in(0))
            ch++; /* 读数据位,接收的数据位放入retc中 */
        SIMLT_IIC.scl(0);
        delay_us(SIMLT_IIC_TT); /* 时钟低电平周期大于4μs */
    }
    (ack) ? (Ack_I2c(0)) : (Ack_I2c(1));

    return (ch);
}
/*
*********************************************************************************************************
*	函 数 名: SIMLT_I2C_OP
*	功能说明: 模拟IIC通用操作
*	形    参: Device：设备地址
*             Addr：要写的数据地址
*             AddrLen：地址的长度
*             op：I2CREAD I2C读操作 I2CWRITE I2C写操作
*	返 回 值: 1：失败 0：成功   
*	备    注：无
*********************************************************************************************************
*/
uint8_t SIMLT_I2C_OP(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t op)
{
    uint8_t Devi_Addr = Device; /* 由于模拟发送会将数据移位清空，所以要另起变量 */
    uint16_t t_Addr = Addr;     /* 由于模拟发送会将数据移位清空，所以要另起变量 */

    Start_I2c();          /* 启动总线 */
    if (SendByte(Device)) /* 发送器件地址 */
        return 1;
    if (AddrLen > 1)
    {
        if (SendByte(HIGHWORD(t_Addr))) /* 发送器件高子地址 */
            return 1;
    }
    if (SendByte(LOWWORD(t_Addr))) /* 发送器件低子地址 */
        return 1;

    if (op == I2CREAD) /* 读操作 */
    {
        Start_I2c();                 /* 发送重起始位 */
        if (SendByte(Devi_Addr + 1)) /* 发送器件地址,读取 */
            return 1;                /* 判断是否接收到应答信号 */
    }
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: SIMLT_Wait_for_end
*	功能说明: 模拟IIC通用操作
*	形    参: Device：设备地址
*	返 回 值: 1：失败 0：成功   
*	备    注：无
*********************************************************************************************************
*/
uint8_t SIMLT_Wait_for_end(uint8_t Device)
{
    uint8_t result, Devi_Addr;

    Devi_Addr = Device;

    Do_DelayStart();
    {
        Start_I2c(); /* 发送起始位 */

        result = SendByte(Devi_Addr); /* 发送器件地址 */

        Stop_I2c(); /* 发送停止位 */

        if (result == 0)
            return 0; /* 设置地址成功, 写周期结束 */
    }
    While_DelayMsEnd(8 * clkmode);

    return 1; /* 设置地址失败 */
}
/*
*********************************************************************************************************
*	函 数 名: API_I2C_Write_Com
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
uint8_t API_SIMLT_I2C_Write_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    /*这里的len不可能大于IIC_ChipWL*/
    uint8_t k = 0, status = 0;
    uint32_t n = 0;
    if (Len > IIC_ChipWL)
        return 2;
    for (k = 0; k < 3; k++) //每遍最多写3次
    {
        status = 0;

        if (SIMLT_I2C_OP(Device, Addr, AddrLen, I2CWRITE))
            status = 1; //写入失败
        else
        {
            for (n = 0; n < Len; n++)
            {
                //发送一个字节
                if (SendByte(Buf[n]))
                {
                    status = 1; //写入失败
                    break;
                }
            }
        }
        //发送停止位
        Stop_I2c();
        if (status == 0)
        {
            SIMLT_Wait_for_end(Device);
            break;
        } //写正确
    }
    return (status);
}
/*
*********************************************************************************************************
*	函 数 名: API_I2C_Read_Com
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
uint8_t API_SIMLT_I2C_Read_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint8_t k = 0, status = 0;
    uint32_t n = 0;

    for (k = 0; k < 3; k++) //每遍最多读3次
    {
        status = 0;

        if (SIMLT_I2C_OP(Device, Addr, AddrLen, I2CREAD))
            status = 1; //写入失败
        else
        {
            for (n = 0; n < Len; n++)
            {
                //接收一个字节
                (n < (Len - 1)) ? (Buf[n] = RcvByte(1)) : (Buf[n] = RcvByte(0));
            }
        }
        //发送停止位
        Stop_I2c();

        if (status == 0)
            break; //读正确
    }
    return (status);
}
