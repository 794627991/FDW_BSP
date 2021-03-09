/*
*********************************************************************************************************
*
*	ģ������ : IIC_BSP����
*	�ļ����� : bsp_iic.c
*	��    �� : V1.0.0
*	˵    �� : IIC����
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-4    ����    ��ʽ����
*		V1.0.1  2021-1-25   ����    ����ģ��IIC����
*		V1.0.3  2021-2-1    ����    API_EPROM����֧�ֵ�24V10����ַ�ɴ���0xffff
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "bsp_iic.h"

#if API_UseIIC > 0

uint8_t IICLOCK = 0; /* iic����α���̰�ȫ���� */
IICBase IICDATA;

/*
*********************************************************************************************************
*	�� �� ��: API_I2CSave
*	����˵��: Ӳ��IIC�洢
*	��    ��: base����ַ
*             *Buf�����ݻ���
*             len������
*	�� �� ֵ: ��
*	��    ע������EEPROM�����鿴API_EEPROM_Write��˵��
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
*	�� �� ��: API_I2CRead
*	����˵��: Ӳ��IIC��ȡ
*	��    ��: base����ַ
*             *Buf�����ݻ���
*             len������
*	�� �� ֵ: ��
*	��    ע������EEPROM�����鿴API_EEPROM_Read��˵��
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
*	�� �� ��: 
*	����˵��: Ӳ��IIC����
*	��    ��: 
*	�� �� ֵ: 
*	��    ע���⺯����˵����
*********************************************************************************************************
*/
uint8_t G_I2C_Send_Bit(uint8_t BIT_def)
{
    uint32_t i;

    I2C_I2CCTRL_I2CEN_Setable(ENABLE); //ʹ��I2C��·

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
*	�� �� ��: 
*	����˵��: Ӳ��IIC����
*	��    ��: 
*	�� �� ֵ: 
*	��    ע���⺯����˵����
*********************************************************************************************************
*/
uint8_t G_I2C_Send_Byte(uint8_t x_byte)
{
    uint16_t i;

    I2C_I2CBUF_Write(x_byte); //д���ͻ���Ĵ���

    for (i = 0; i < 18 * SSP_TBIT; i++)
    {
        if (SET == I2C_I2CIR_I2CIF_Chk())
            break; //�жϱ�־��λʱ����ѭ��
    }

    if (i < 18 * SSP_TBIT)
    {
        I2C_I2CIR_I2CIF_Clr(); //clr int flag

        if (SET == I2C_I2CSTA_ACKSTA_Chk()) //�ӻ���Ӧ��
        {
            return 2; //ʧ��
        }
        else
        {
            return 0; //�ɹ�
        }
    }
    else
    {
        return 1; //failure.
    }
}
/*
*********************************************************************************************************
*	�� �� ��: 
*	����˵��: Ӳ��IIC����
*	��    ��: 
*	�� �� ֵ: 
*	��    ע���⺯����˵����
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
            break; //�жϱ�־��λ, ����ѭ��
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
*	�� �� ��: 
*	����˵��: Ӳ��IIC����
*	��    ��: 
*	�� �� ֵ: 
*	��    ע���⺯����˵����
*********************************************************************************************************
*/
uint8_t G_Sendaddr(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t Opt)
{
    uint8_t result, Devi_Addr;

    Devi_Addr = Device;

    //-------------- reset i2c -------------
    I2C_ResetI2C(); //��λI2C

    //-------------- start bit -------------
    result = G_I2C_Send_Bit(STARTBIT); //������ʼλ
    if (result != 0)
        return 1; //failure.

    //-------------- disable read -------------
    I2C_I2CCTRL_RCEN_Setable(DISABLE);

    //-------------- device addr -------------
    result = G_I2C_Send_Byte(Devi_Addr); //����������ַ
    if (result != 0)
        return 2; //failure.

    //--------------- data addr --------------
    if (AddrLen == 2)
    {
        result = G_I2C_Send_Byte(Addr >> 8); //�������ݵ�ַ��8λ
        if (result != 0)
            return 3; //failure.
    }
    result = G_I2C_Send_Byte(Addr >> 0); //�������ݵ�ַ��8λ
    if (result != 0)
        return 3; //failure.

    if (Opt == I2CREAD) //������
    {
        result = G_I2C_Send_Bit(RESTARTBIT); //��������ʼλ
        if (result != 0)
            return 5; //failure.

        result = G_I2C_Send_Byte(Devi_Addr | 1); //����������ַ,��ȡ
        if (result != 0)
            return 5; //failure.
    }

    return 0; //ok
}
/*
*********************************************************************************************************
*	�� �� ��: 
*	����˵��: Ӳ��IIC����
*	��    ��: 
*	�� �� ֵ: 
*	��    ע���⺯����˵����
*********************************************************************************************************
*/
uint8_t G_Wait_for_end(uint8_t Device)
{
    uint8_t result, Devi_Addr;

    Devi_Addr = Device;

    Do_DelayStart();
    {
        G_I2C_Send_Bit(STARTBIT); //������ʼλ

        result = G_I2C_Send_Byte(Devi_Addr); //����������ַ

        G_I2C_Send_Bit(STOPBIT); //����ֹͣλ

        if (result == 0)
            return 0; //���õ�ַ�ɹ�, д���ڽ���
    }
    While_DelayMsEnd(8 * clkmode);

    return 1; //���õ�ַʧ��
}
/*
*********************************************************************************************************
*	�� �� ��: 
*	����˵��: Ӳ��IIC����
*	��    ��: 
*	�� �� ֵ: 
*	��    ע���⺯����˵����
*********************************************************************************************************
*/
uint8_t G_I2C_Write_Bottom(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint8_t k = 0, status = 0;
    uint32_t n = 0;
    for (n = 0; n < Len; n++)
    {
        for (k = 0; k < 3; k++) //ÿ�����д3��
        {
            status = 0;
            if (G_Sendaddr(Device, Addr, AddrLen, I2CWRITE))
                status = 1; //д��ʧ��
            else
            {
                //for( n=0; n<Len; n++ )
                {
                    //����һ���ֽ�
                    if (G_I2C_Send_Byte(Buf[n]))
                    {
                        status = 1;
                        break;
                    } //д��ʧ��
                }
            }
            //����ֹͣλ
            if (G_I2C_Send_Bit(STOPBIT))
                status = 1;

            if (status == 0)
            {
                G_Wait_for_end(Device);
                break;
            } //д��ȷ
        }
        Addr++;
    }

    return (status);
}
/*
*********************************************************************************************************
*	�� �� ��: API_I2C_Write_Com
*	����˵��: Ӳ��IICͨ��д����
*	��    ��: Device���豸��ַ
*             Addr��Ҫд�����ݵ�ַ
*             AddrLen����ַ�ĳ���
*             *Buf�����ݻ���
*             Len�����ݳ���
*	�� �� ֵ: 0���ɹ���1��ʧ�ܣ�2������һ��д����������      
*	��    ע��IIC_ChipWL ��Ҫ��APP_CFG.h���ֶ�����
*********************************************************************************************************
*/
__weak uint8_t API_I2C_Write_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    /*�����len�����ܴ���IIC_ChipWL*/
    uint8_t k = 0, status = 0;
    uint32_t n = 0;
    if (Len > IIC_ChipWL)
        return 2;

    for (k = 0; k < 3; k++) //ÿ�����д3��
    {
        status = 0;
        if (G_Sendaddr(Device, Addr, AddrLen, I2CWRITE))
            status = 1; //д��ʧ��
        else
        {
            for (n = 0; n < Len; n++)
            {
                //����һ���ֽ�
                if (G_I2C_Send_Byte(Buf[n]))
                {
                    status = 1;
                    break;
                } //д��ʧ��
            }
        }
        //����ֹͣλ
        if (G_I2C_Send_Bit(STOPBIT))
            status = 1;

        if (status == 0)
        {
            G_Wait_for_end(Device);
            break;
        } //д��ȷ
    }
    return (status);
}
/*
*********************************************************************************************************
*	�� �� ��: API_I2C_Read_Com
*	����˵��: Ӳ��IICͨ�ö�����
*	��    ��: Device���豸��ַ
*             Addr��Ҫд�����ݵ�ַ
*             AddrLen����ַ�ĳ���
*             *Buf�����ݻ���
*             Len�����ݳ���
*	�� �� ֵ: 0���ɹ���1��ʧ��  
*	��    ע����
*********************************************************************************************************
*/
__weak uint8_t API_I2C_Read_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint8_t k = 0, status = 0;
    uint32_t n = 0;

    for (k = 0; k < 3; k++) //ÿ������3��
    {
        status = 0;

        if (G_Sendaddr(Device, Addr, AddrLen, I2CREAD))
            status = 1; //д��ʧ��
        else
        {
            for (n = 0; n < Len; n++)
            {
                //����һ���ֽ�
                if (G_I2C_Receive_Byte(Buf + n))
                {
                    status = 1;
                    break;
                }

                if (n < (Len - 1))
                    I2C_SEND_ACK_0(); //���������ӻ��Ļ�ӦΪ0
                else
                    I2C_SEND_ACK_1(); //���������ӻ��Ļ�ӦΪ1

                //���� ACK
                if (G_I2C_Send_Bit(ACKBIT)) //ʧ��
                {
                    I2C_SEND_ACK_0();
                    status = 1;
                    break;
                }
                I2C_SEND_ACK_0();
            }
        }
        //����ֹͣλ
        if (G_I2C_Send_Bit(STOPBIT))
            status = 1; //ʧ��

        if (status == 0)
            break; //����ȷ
    }

    return (status);
}
/*
*********************************************************************************************************
*	�� �� ��: API_EEPROM_Write
*	����˵��: Ӳ��IIC����EEPROMר��
*	��    ��: Device���豸��ַ
*             Addr��Ҫд�����ݵ�ַ
*             AddrLen����ַ�ĳ���
*             *Buf�����ݻ���
*             Len�����ݳ���
*	�� �� ֵ: 0���ɹ���1��ʧ�ܣ�2������һ��д���������� 
*	��    ע����24C02/04�������Ͻ���ר�����䣬����������ַ���������ж�ʹ�õ���C02/04��������
*            ��������д����
*********************************************************************************************************
*/
__weak uint8_t API_EEPROM_Write(uint8_t Device, uint32_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint32_t tAdr = Addr, sendlen = 0;
    uint8_t status = 0;
    uint16_t needlen = 0; /* ���Է��͵ĳ��ȣ��Ѿ����͵ĳ��� */

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

        needlen = IIC_ChipWL - (tAdr % IIC_ChipWL); /* ����ʣ��ɷ��͵ĳ��� */
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
*	�� �� ��: API_EEPROM_Read
*	����˵��: Ӳ��IIC����EEPROMר��
*	��    ��: Device���豸��ַ
*             Addr��Ҫд�����ݵ�ַ
*             AddrLen����ַ�ĳ���
*             *Buf�����ݻ���
*             Len�����ݳ���
*	�� �� ֵ: 0���ɹ���1��ʧ��
*	��    ע����24C02/04�������Ͻ���ר�����䣬����������ַ���������ж�ʹ�õ���C02/04��������
*            ��������������
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
*	�� �� ��: API_I2C_Init
*	����˵��: Ӳ��IIC��ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
__weak void API_I2C_Init(void)
{
    RCC_PERCLK_SetableEx(I2CCLK, ENABLE); /* I2Cʱ��ʹ�� */

    AltFunIO(GPIOA, GPIO_Pin_14, ALTFUN_OPENDRAIN); /* PA14 SCL */
    AltFunIO(GPIOA, GPIO_Pin_15, ALTFUN_OPENDRAIN); /* PA15 SDA */

    I2C_I2CBRG_Write(I2C_BaudREG_Calc(IIC_SPEED * 1000, 8000000 * clkmode)); /* 100k@8M */
    I2C_I2CIR_I2CIE_Setable(DISABLE);                                        /* �ر�I2C�ж� */
    I2C_I2CERR_ERRIE_Setable(DISABLE);                                       /* �ر�I2C�����ж� */

    NVIC_DisableIRQ(I2C_IRQn);
}

#endif

iic_op_type SIMLT_IIC;
/*
*********************************************************************************************************
*	�� �� ��: iic_op_sda_out
*	����˵��: IIC_SDA���
*	��    ��: op���涨����0Ϊ��������� �������������
*	�� �� ֵ: ����
*	��    ע����
*********************************************************************************************************
*/
__weak uint8_t iic_op_sda_out(uint8_t op)
{
    (op) ? (MONI_IIC_SDA_1) : (MONI_IIC_SDA_0);
    return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: iic_op_sda_in
*	����˵��: IIC_SDA����
*	��    ��: op������
*	�� �� ֵ: SDA�����ֵ
*	��    ע����
*********************************************************************************************************
*/
__weak uint8_t iic_op_sda_in(uint8_t op)
{
    return MONI_IIC_SDA_READ;
}
/*
*********************************************************************************************************
*	�� �� ��: iic_op_scl
*	����˵��: IIC_SCL���
*	��    ��: op���涨����0Ϊ��������� �������������
*	�� �� ֵ: ����
*	��    ע����
*********************************************************************************************************
*/
__weak uint8_t iic_op_scl(uint8_t op)
{
    (op) ? (MONI_IIC_SCL_1) : (MONI_IIC_SCL_0);
    return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: iic_op_sda_set
*	����˵��: ����IIC_SDA�������
*	��    ��: op���涨����0Ϊ������� ������������
*	�� �� ֵ: ����
*	��    ע����
*********************************************************************************************************
*/
__weak uint8_t iic_op_sda_set(uint8_t op)
{
    (op) ? (MONI_IIC_SDA_DIR_OUT) : (MONI_IIC_SDA_DIR_IN);
    return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: SIMLT_I2C_OP_INIT
*	����˵��: ʹ��ģ��IICǰ������øú���
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����
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
*	�� �� ��: Start_I2c
*	����˵��: ��ʼ�ź�
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����ʱ���ߺ������߶�Ϊ�ߵ�ƽʱ��IIC�����ϵ����д��豸�����ڿ���״̬��
*            ��ʱ���ߺ������߶�Ϊ�ߵ�ƽʱ�������ߴӸߵ�ƽ���͵�ƽ��������������Ϊ��ʼ�źš�
*********************************************************************************************************
*/
void Start_I2c(void)
{
    SIMLT_IIC.sda_set(1);   /* ����SDAΪ��� */
    SIMLT_IIC.sda_out(1);   /* ������ʼ�����������ź� */
    SIMLT_IIC.scl(1);       /* ������ʼ������ʱ���ź� */
    delay_us(SIMLT_IIC_TT); /* ��ʼ��������ʱ�����4us */
    SIMLT_IIC.sda_out(0);   /* ������ʼ�ź� */
    delay_us(SIMLT_IIC_TT); /* ��ʼ��������ʱ�����4��s */
    SIMLT_IIC.scl(0);       /* ǯסI2C���ߣ�׼�����ͻ�������� */
}
/*
*********************************************************************************************************
*	�� �� ��: Stop_I2c
*	����˵��: ֹͣ�ź�
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע����ʱ����Ϊ�ߵ�ƽ��������Ϊ�͵�ƽʱ�������ߴӵ͵��ߵ�������������Ϊֹͣ�źš�
*********************************************************************************************************
*/
void Stop_I2c(void)
{
    SIMLT_IIC.sda_set(1);   /* ����SDAΪ��� */
    SIMLT_IIC.sda_out(0);   /* ���ͽ��������������ź� */
    SIMLT_IIC.scl(0);       /* ���ͽ���������ʱ���ź� */
    delay_us(SIMLT_IIC_TT); /* ������������ʱ�����4��s */
    SIMLT_IIC.scl(1);
    delay_us(SIMLT_IIC_TT); /* �ͷ�����ʱ�����4��s */
    SIMLT_IIC.sda_out(1);   /* ����I2C���߽����ź� */
}
/*
*********************************************************************************************************
*	�� �� ��: Ack_I2c
*	����˵��: ��������ACKӦ��
*	��    ��: a:Ӧ�����ͣ�0��ʾӦ���źţ���0��ʾ��Ӧ���źš�
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void Ack_I2c(uint8_t a)
{
    SIMLT_IIC.sda_set(1);                                       /* ����SDAΪ��� */
    (a == 0) ? (SIMLT_IIC.sda_out(0)) : (SIMLT_IIC.sda_out(1)); /*�ڴ˷���Ӧ����Ӧ���ź� */
    delay_us(1);
    SIMLT_IIC.scl(1);
    delay_us(SIMLT_IIC_TT); /*ʱ�ӵ͵�ƽ���ڴ���4��s*/
    SIMLT_IIC.scl(0);       /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
}
/*
*********************************************************************************************************
*	�� �� ��: TestAck
*	����˵��: �ȴ�Ӧ���źŵ���
*	��    ��: ��
*	�� �� ֵ: 0��ʾ����Ӧ��1��ʾ����ȷӦ��
*	��    ע����
*********************************************************************************************************
*/
uint8_t TestAck(void)
{
    uint8_t outTime = 0;
    SIMLT_IIC.sda_set(1); /* ����SDAΪ��� */
    SIMLT_IIC.sda_out(1);
    delay_us(1); /* ������SDA��ƽ�Ѿ��ޱ仯 */
    SIMLT_IIC.scl(1);
    delay_us(1);
    SIMLT_IIC.sda_set(0); /* SDA��Ϊ���� */
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
*	�� �� ��: SendByte
*	����˵��: �������������ź�
*	��    ��: c:Ҫ���͵�һ�ֽ�����
*	�� �� ֵ: 0��ʾ����Ӧ��1��ʾ����ȷӦ��
*	��    ע��ʱ���߸ߵ�ƽ��������Ϊ�͵�ƽʱ����ʱ��������֮��IIC���豸���յ�һ������0��
*             ʱ���߸ߵ�ƽ��������Ϊ�ߵ�ƽʱ����ʱ��������֮��IIC���豸���յ�һ������1��
*********************************************************************************************************
*/
uint8_t SendByte(uint8_t c)
{
    volatile uint8_t Bit;
    SIMLT_IIC.sda_set(1);         /* ����SDAΪ��� */
    for (Bit = 0; Bit < 8; Bit++) /* Ҫ���͵����ݳ���Ϊ8λ */
    {
        ((c << Bit) & 0x80) ? (SIMLT_IIC.sda_out(1)) : (SIMLT_IIC.sda_out(0)); /* �жϷ���λ */
        delay_us(1);
        SIMLT_IIC.scl(1);       /* ��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ */
        delay_us(SIMLT_IIC_TT); /* ��֤ʱ�Ӹߵ�ƽ���ڴ���4��s */
        SIMLT_IIC.scl(0);       /* ��ʱ����Ϊ�ͣ���������������λ */
    }
    return TestAck();
}
/*
*********************************************************************************************************
*	�� �� ��: SendByte
*	����˵��: �������������ź�
*	��    ��: ack=1ʱ������ACK��ack=0������nACK
*	�� �� ֵ: ��
*	��    ע��ʱ���߸ߵ�ƽ��������Ϊ�͵�ƽʱ����ʱ��������֮�󣬶���һ������0��
*             ʱ���߸ߵ�ƽ��������Ϊ�ߵ�ƽʱ����ʱ��������֮�󣬶���һ������1��
*********************************************************************************************************
*/
uint8_t RcvByte(uint8_t ack)
{
    uint8_t ch, BitCnt;
    ch = 0;
    SIMLT_IIC.sda_set(0); /* ��������Ϊ���뷽ʽ */
    for (BitCnt = 0; BitCnt < 8; BitCnt++)
    {
        SIMLT_IIC.scl(1);
        delay_us(1);
        ch <<= 1;
        if (SIMLT_IIC.sda_in(0))
            ch++; /* ������λ,���յ�����λ����retc�� */
        SIMLT_IIC.scl(0);
        delay_us(SIMLT_IIC_TT); /* ʱ�ӵ͵�ƽ���ڴ���4��s */
    }
    (ack) ? (Ack_I2c(0)) : (Ack_I2c(1));

    return (ch);
}
/*
*********************************************************************************************************
*	�� �� ��: SIMLT_I2C_OP
*	����˵��: ģ��IICͨ�ò���
*	��    ��: Device���豸��ַ
*             Addr��Ҫд�����ݵ�ַ
*             AddrLen����ַ�ĳ���
*             op��I2CREAD I2C������ I2CWRITE I2Cд����
*	�� �� ֵ: 1��ʧ�� 0���ɹ�   
*	��    ע����
*********************************************************************************************************
*/
uint8_t SIMLT_I2C_OP(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t op)
{
    uint8_t Devi_Addr = Device; /* ����ģ�ⷢ�ͻὫ������λ��գ�����Ҫ������� */
    uint16_t t_Addr = Addr;     /* ����ģ�ⷢ�ͻὫ������λ��գ�����Ҫ������� */

    Start_I2c();          /* �������� */
    if (SendByte(Device)) /* ����������ַ */
        return 1;
    if (AddrLen > 1)
    {
        if (SendByte(HIGHWORD(t_Addr))) /* �����������ӵ�ַ */
            return 1;
    }
    if (SendByte(LOWWORD(t_Addr))) /* �����������ӵ�ַ */
        return 1;

    if (op == I2CREAD) /* ������ */
    {
        Start_I2c();                 /* ��������ʼλ */
        if (SendByte(Devi_Addr + 1)) /* ����������ַ,��ȡ */
            return 1;                /* �ж��Ƿ���յ�Ӧ���ź� */
    }
    return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: SIMLT_Wait_for_end
*	����˵��: ģ��IICͨ�ò���
*	��    ��: Device���豸��ַ
*	�� �� ֵ: 1��ʧ�� 0���ɹ�   
*	��    ע����
*********************************************************************************************************
*/
uint8_t SIMLT_Wait_for_end(uint8_t Device)
{
    uint8_t result, Devi_Addr;

    Devi_Addr = Device;

    Do_DelayStart();
    {
        Start_I2c(); /* ������ʼλ */

        result = SendByte(Devi_Addr); /* ����������ַ */

        Stop_I2c(); /* ����ֹͣλ */

        if (result == 0)
            return 0; /* ���õ�ַ�ɹ�, д���ڽ��� */
    }
    While_DelayMsEnd(8 * clkmode);

    return 1; /* ���õ�ַʧ�� */
}
/*
*********************************************************************************************************
*	�� �� ��: API_I2C_Write_Com
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
uint8_t API_SIMLT_I2C_Write_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    /*�����len�����ܴ���IIC_ChipWL*/
    uint8_t k = 0, status = 0;
    uint32_t n = 0;
    if (Len > IIC_ChipWL)
        return 2;
    for (k = 0; k < 3; k++) //ÿ�����д3��
    {
        status = 0;

        if (SIMLT_I2C_OP(Device, Addr, AddrLen, I2CWRITE))
            status = 1; //д��ʧ��
        else
        {
            for (n = 0; n < Len; n++)
            {
                //����һ���ֽ�
                if (SendByte(Buf[n]))
                {
                    status = 1; //д��ʧ��
                    break;
                }
            }
        }
        //����ֹͣλ
        Stop_I2c();
        if (status == 0)
        {
            SIMLT_Wait_for_end(Device);
            break;
        } //д��ȷ
    }
    return (status);
}
/*
*********************************************************************************************************
*	�� �� ��: API_I2C_Read_Com
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
uint8_t API_SIMLT_I2C_Read_Com(uint8_t Device, uint16_t Addr, uint8_t AddrLen, uint8_t *Buf, uint16_t Len)
{
    uint8_t k = 0, status = 0;
    uint32_t n = 0;

    for (k = 0; k < 3; k++) //ÿ������3��
    {
        status = 0;

        if (SIMLT_I2C_OP(Device, Addr, AddrLen, I2CREAD))
            status = 1; //д��ʧ��
        else
        {
            for (n = 0; n < Len; n++)
            {
                //����һ���ֽ�
                (n < (Len - 1)) ? (Buf[n] = RcvByte(1)) : (Buf[n] = RcvByte(0));
            }
        }
        //����ֹͣλ
        Stop_I2c();

        if (status == 0)
            break; //����ȷ
    }
    return (status);
}
