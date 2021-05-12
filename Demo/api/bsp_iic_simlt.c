/*
*********************************************************************************************************
*
*	ģ������ : IIC_BSP����
*	�ļ����� : bsp_iic_simlt.c
*	��    �� : V1.0.2
*	˵    �� : IIC����
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-5-12   ����    ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
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
*	�� �� ��: simlt_iic_op_sda_out
*	����˵��: IIC_SDA���
*	��    ��: op���涨����0Ϊ��������� �������������
*	�� �� ֵ: ����
*	��    ע����
*********************************************************************************************************
*/
void simlt_iic_op_sda_out(iic_simlt_type *io, uint8_t op)
{
    (op) ? (io->io_set(io->sda_io, io->sda_pin)) : (io->io_reset(io->sda_io, io->sda_pin));
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_op_sda_in
*	����˵��: IIC_SDA����
*	��    ��: op������
*	�� �� ֵ: SDA�����ֵ
*	��    ע����
*********************************************************************************************************
*/
uint8_t simlt_iic_op_sda_in(iic_simlt_type *io)
{
    return io->io_read(io->sda_io, io->sda_pin);
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_op_scl_out
*	����˵��: IIC_SCL���
*	��    ��: op���涨����0Ϊ��������� �������������
*	�� �� ֵ: ����
*	��    ע����
*********************************************************************************************************
*/
void simlt_iic_op_scl_out(iic_simlt_type *io, uint8_t op)
{
    (op) ? (io->io_set(io->scl_io, io->scl_pin)) : (io->io_reset(io->scl_io, io->scl_pin));
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_op_sda_set
*	����˵��: ����IIC_SDA�������
*	��    ��: op���涨����0Ϊ������� ������������
*	�� �� ֵ: ����
*	��    ע����
*********************************************************************************************************
*/
void simlt_iic_op_sda_set(iic_simlt_type *io, uint8_t op)
{
    (op) ? (io->io_out(io->sda_io, io->sda_pin, 0)) : (io->io_in(io->sda_io, io->sda_pin, 1));
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_op_init
*	����˵��: ʹ��ģ��IICǰ������øú���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void simlt_iic_op_init(iic_simlt_type *io)
{
    io->io_out(io->sda_io, io->sda_pin, 0);
    io->io_out(io->scl_io, io->scl_pin, 0);
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_start
*	����˵��: ��ʼ�ź�
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����ʱ���ߺ������߶�Ϊ�ߵ�ƽʱ��IIC�����ϵ����д��豸�����ڿ���״̬��
*            ��ʱ���ߺ������߶�Ϊ�ߵ�ƽʱ�������ߴӸߵ�ƽ���͵�ƽ��������������Ϊ��ʼ�źš�
*********************************************************************************************************
*/
void simlt_iic_start(iic_simlt_type *io)
{
    simlt_iic_op_sda_set(io, 1); /* ����SDAΪ��� */
    simlt_iic_op_sda_out(io, 1); /* ������ʼ�����������ź� */
    simlt_iic_op_scl_out(io, 1); /* ������ʼ������ʱ���ź� */
    delay_us(SIMLT_IIC_TT);      /* ��ʼ��������ʱ�����4us */
    simlt_iic_op_sda_out(io, 0); /* ������ʼ�ź� */
    delay_us(SIMLT_IIC_TT);      /* ��ʼ��������ʱ�����4��s */
    simlt_iic_op_scl_out(io, 0); /* ǯסI2C���ߣ�׼�����ͻ�������� */
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_stop
*	����˵��: ֹͣ�ź�
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����ʱ����Ϊ�ߵ�ƽ��������Ϊ�͵�ƽʱ�������ߴӵ͵��ߵ�������������Ϊֹͣ�źš�
*********************************************************************************************************
*/
void simlt_iic_stop(iic_simlt_type *io)
{
    simlt_iic_op_sda_set(io, 1); /* ����SDAΪ��� */
    simlt_iic_op_sda_out(io, 0); /* ���ͽ��������������ź� */
    simlt_iic_op_scl_out(io, 0); /* ���ͽ���������ʱ���ź� */
    delay_us(SIMLT_IIC_TT);      /* ������������ʱ�����4��s */
    simlt_iic_op_scl_out(io, 1);
    delay_us(SIMLT_IIC_TT);      /* �ͷ�����ʱ�����4��s */
    simlt_iic_op_sda_out(io, 1); /* ����I2C���߽����ź� */
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_ack
*	����˵��: ��������ACKӦ��
*	��    ��: a:Ӧ�����ͣ�0��ʾӦ���źţ���0��ʾ��Ӧ���źš�
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void simlt_iic_ack(iic_simlt_type *io, uint8_t a)
{
    simlt_iic_op_sda_set(io, 1);                                              /* ����SDAΪ��� */
    (a == 0) ? (simlt_iic_op_sda_out(io, 0)) : (simlt_iic_op_sda_out(io, 1)); /*�ڴ˷���Ӧ����Ӧ���ź� */
    delay_us(1);
    simlt_iic_op_scl_out(io, 1);
    delay_us(SIMLT_IIC_TT);      /*ʱ�ӵ͵�ƽ���ڴ���4��s*/
    simlt_iic_op_scl_out(io, 0); /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_test_ack
*	����˵��: �ȴ�Ӧ���źŵ���
*	��    ��: ��
*	�� �� ֵ: 0��ʾ����Ӧ��1��ʾ����ȷӦ��
*	��    ע����
*********************************************************************************************************
*/
uint8_t simlt_iic_test_ack(iic_simlt_type *io)
{
    uint8_t outTime = 0;
    simlt_iic_op_sda_set(io, 1); /* ����SDAΪ��� */
    simlt_iic_op_sda_out(io, 1);
    delay_us(1); /* ������SDA��ƽ�Ѿ��ޱ仯 */
    simlt_iic_op_scl_out(io, 1);
    delay_us(1);
    simlt_iic_op_sda_set(io, 0); /* SDA��Ϊ���� */
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
*	�� �� ��: simlt_iic_send_byte
*	����˵��: �������������ź�
*	��    ��: c:Ҫ���͵�һ�ֽ�����
*	�� �� ֵ: 0��ʾ����Ӧ��1��ʾ����ȷӦ��
*	��    ע��ʱ���߸ߵ�ƽ��������Ϊ�͵�ƽʱ����ʱ��������֮��IIC���豸���յ�һ������0��
*             ʱ���߸ߵ�ƽ��������Ϊ�ߵ�ƽʱ����ʱ��������֮��IIC���豸���յ�һ������1��
*********************************************************************************************************
*/
uint8_t simlt_iic_send_byte(iic_simlt_type *io, uint8_t c)
{
    volatile uint8_t Bit;
    simlt_iic_op_sda_set(io, 1);  /* ����SDAΪ��� */
    for (Bit = 0; Bit < 8; Bit++) /* Ҫ���͵����ݳ���Ϊ8λ */
    {
        ((c << Bit) & 0x80) ? (simlt_iic_op_sda_out(io, 1)) : (simlt_iic_op_sda_out(io, 0)); /* �жϷ���λ */
        delay_us(1);
        simlt_iic_op_scl_out(io, 1); /* ��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ */
        delay_us(SIMLT_IIC_TT);      /* ��֤ʱ�Ӹߵ�ƽ���ڴ���4��s */
        simlt_iic_op_scl_out(io, 0); /* ��ʱ����Ϊ�ͣ���������������λ */
    }
    return simlt_iic_test_ack(io);
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_rcv_byte
*	����˵��: �������������ź�
*	��    ��: ack=1ʱ������ACK��ack=0������nACK
*	�� �� ֵ: ��
*	��    ע��ʱ���߸ߵ�ƽ��������Ϊ�͵�ƽʱ����ʱ��������֮�󣬶���һ������0��
*             ʱ���߸ߵ�ƽ��������Ϊ�ߵ�ƽʱ����ʱ��������֮�󣬶���һ������1��
*********************************************************************************************************
*/
uint8_t simlt_iic_rcv_byte(iic_simlt_type *io, uint8_t ack)
{
    uint8_t ch, BitCnt;
    ch = 0;
    simlt_iic_op_sda_set(io, 0); /* ��������Ϊ���뷽ʽ */
    for (BitCnt = 0; BitCnt < 8; BitCnt++)
    {
        simlt_iic_op_scl_out(io, 1);
        delay_us(1);
        ch <<= 1;
        if (simlt_iic_op_sda_in(io))
            ch++; /* ������λ,���յ�����λ����retc�� */
        simlt_iic_op_scl_out(io, 0);
        delay_us(SIMLT_IIC_TT); /* ʱ�ӵ͵�ƽ���ڴ���4��s */
    }
    (ack) ? (simlt_iic_ack(io, 0)) : (simlt_iic_ack(io, 1));
    return (ch);
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_op
*	����˵��: ģ��IICͨ�ò���
*	��    ��: Device���豸��ַ
*             Addr��Ҫд�����ݵ�ַ
*             AddrLen����ַ�ĳ���
*             op��1 I2C������ else I2Cд����
*	�� �� ֵ: 1��ʧ�� 0���ɹ�   
*	��    ע����
*********************************************************************************************************
*/
uint8_t simlt_iic_op(iic_simlt_type *io, uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t op)
{
    uint8_t Devi_Addr = Device; /* ����ģ�ⷢ�ͻὫ������λ��գ�����Ҫ������� */
    uint16_t t_Addr = Addr;     /* ����ģ�ⷢ�ͻὫ������λ��գ�����Ҫ������� */

    simlt_iic_start(io);                 /* �������� */
    if (simlt_iic_send_byte(io, Device)) /* ����������ַ */
        return 1;
    if (AddrLen > 1)
    {
        if (simlt_iic_send_byte(io, HIGHWORD(t_Addr))) /* �����������ӵ�ַ */
            return 1;
    }
    if (simlt_iic_send_byte(io, LOWWORD(t_Addr))) /* �����������ӵ�ַ */
        return 1;

    if (op == 1) /* ������ */
    {
        simlt_iic_start(io);                        /* ��������ʼλ */
        if (simlt_iic_send_byte(io, Devi_Addr + 1)) /* ����������ַ,��ȡ */
            return 1;                               /* �ж��Ƿ���յ�Ӧ���ź� */
    }
    return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_wait_end
*	����˵��: ģ��IICͨ�ò���
*	��    ��: Device���豸��ַ
*	�� �� ֵ: 1��ʧ�� 0���ɹ�   
*	��    ע����
*********************************************************************************************************
*/
uint8_t simlt_iic_wait_end(iic_simlt_type *io, uint8_t Device)
{
    uint8_t result, Devi_Addr;

    Devi_Addr = Device;

    Do_DelayStart();
    {
        simlt_iic_start(io);                         /* ������ʼλ */
        result = simlt_iic_send_byte(io, Devi_Addr); /* ����������ַ */
        simlt_iic_stop(io);                          /* ����ֹͣλ */
        if (result == 0)
            return 0; /* ���õ�ַ�ɹ�, д���ڽ��� */
    }
    While_DelayMsEnd(8 * clkmode);

    return 1; /* ���õ�ַʧ�� */
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_write
*	����˵��: ģ��IICͨ��д����
*	��    ��: Device���豸��ַ
*             Addr��Ҫд�����ݵ�ַ
*             AddrLen����ַ�ĳ���
*             *Buf�����ݻ���
*             Len�����ݳ���
*	�� �� ֵ: 0���ɹ���1��ʧ�ܣ�2������һ��д����������      
*	��    ע��IIC_ChipWL ��Ҫ��APP_CFG.h���ֶ�����
*********************************************************************************************************
*/
uint8_t simlt_iic_write(iic_simlt_type *io, uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    /*�����len�����ܴ���IIC_ChipWL*/
    uint8_t k = 0, status = 0;
    uint32_t n = 0;

    if (Len > IIC_ChipWL)
        return 2;
    simlt_iic_op_init(io);
    for (k = 0; k < 3; k++) //ÿ�����д3��
    {
        status = 0;

        if (simlt_iic_op(io, Device, Addr, AddrLen, 0))
            status = 1; //д��ʧ��
        else
        {
            for (n = 0; n < Len; n++)
            {
                //����һ���ֽ�
                if (simlt_iic_send_byte(io, Buf[n]))
                {
                    status = 1; //д��ʧ��
                    break;
                }
            }
        }
        //����ֹͣλ
        simlt_iic_stop(io);
        if (status == 0)
        {
            simlt_iic_wait_end(io, Device);
            break;
        } //д��ȷ
    }
    return (status);
}
/*
*********************************************************************************************************
*	�� �� ��: simlt_iic_read
*	����˵��: ģ��IICͨ�ö�����
*	��    ��: Device���豸��ַ
*             Addr��Ҫд�����ݵ�ַ
*             AddrLen����ַ�ĳ���
*             *Buf�����ݻ���
*             Len�����ݳ���
*	�� �� ֵ: 0���ɹ���1��ʧ��  
*	��    ע����
*********************************************************************************************************
*/
uint8_t simlt_iic_read(iic_simlt_type *io, uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint8_t k = 0, status = 0;
    uint32_t n = 0;
    simlt_iic_op_init(io);
    for (k = 0; k < 3; k++) //ÿ������3��
    {
        status = 0;

        if (simlt_iic_op(io, Device, Addr, AddrLen, 1))
            status = 1; //д��ʧ��
        else
        {
            for (n = 0; n < Len; n++)
            {
                //����һ���ֽ�
                (n < (Len - 1)) ? (Buf[n] = simlt_iic_rcv_byte(io, 1)) : (Buf[n] = simlt_iic_rcv_byte(io, 0));
            }
        }
        //����ֹͣλ
        simlt_iic_stop(io);

        if (status == 0)
            break; //����ȷ
    }
    return (status);
}
/*
*********************************************************************************************************
*	�� �� ��: API_SIMLT_IIC
*	����˵��: ģ��IIC����
*	��    ��: op:1:�� else��д
*             Device���豸��ַ
*             Addr��Ҫд�����ݵ�ַ
*             AddrLen����ַ�ĳ���
*             *Buf�����ݻ���
*             Len�����ݳ���
*	�� �� ֵ: 0���ɹ���1��ʧ��  
*	��    ע����
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
