#include "HT16C23_IIC.h"
void LCD_Init_HT16C23(void)
{
    //  I2cSendCommand(0x82,0x00);//HT9B92�����λ
    //  I2cSendCommand(0x86,0x00);//LCD��
    //  I2cSendCommand(0x88,0x00);
    I2cSendCommand(0x84, 0x00);
    I2cSendCommand(0x84, 0x03);
}
////////////////////////////////////////////////////////////////////////
void LCD_SHOW_HT16C23(void)
{
    uchar Dis_Buf[13] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    I2cSendData(0x80, 0x00, Dis_Buf, 13); //ˢ����ʾ
}
/*************************************************/
//����IIC����
/*************************************************/
void Start(void)
{
    SDA_LCD_1;
    TicksDelayUs(TT);
    SCL_LCD_1;
    TicksDelayUs(TT);
    SDA_LCD_0;
    TicksDelayUs(TT);
    SCL_LCD_0; //Taa<=3.5us
}
/************************************************/
//ֹͣ�������ͷ�IIC����
/************************************************/
void Stop(void)
{
    SDA_LCD_OUT;
    SCL_LCD_0;
    TicksDelayUs(TT);
    SDA_LCD_0;
    TicksDelayUs(TT);
    SCL_LCD_1;
    TicksDelayUs(TT);
    SDA_LCD_1;
    TicksDelayUs(TT);
}

/************************************************/
//IIC����Ӧ��
/************************************************/
void Ack(void)
{
    SDA_LCD_OUT;
    SDA_LCD_0;
    TicksDelayUs(TT);
    SCL_LCD_1;
    TicksDelayUs(TT);
    SCL_LCD_0;
    TicksDelayUs(TT);
    SDA_LCD_1;
}
/************************************************/
//IIC������Ӧ��
/************************************************/
void NoAck(void)
{
    SDA_LCD_OUT;
    SDA_LCD_1;
    TicksDelayUs(TT);
    SCL_LCD_1;
    TicksDelayUs(TT);
    SCL_LCD_0;
    TicksDelayUs(TT);
}
/************************************************/
//IIC���߼���Ӧ��
//����ֵ��IICӦ��λ��ֵ��0��Ӧ��1��Ӧ��
/************************************************/
uint8_t TestAck1()
{
    static uint8_t ack;
    SDA_LCD_IN;
    TicksDelayUs(TT);
    SCL_LCD_1;
    TicksDelayUs(TT);
    ack = READ_LCD_SDA;
    SCL_LCD_0;
    return (ack);
}
/************************************************/
//IIC����д8λ����
/************************************************/
void Write8bit(uint8_t input)
{
    uint8_t temp;
    SDA_LCD_OUT;
    for (temp = 0; temp < 8; temp++)
    {
        if (input & 0x80)
            SDA_LCD_1;
        else
            SDA_LCD_0;
        TicksDelayUs(TT);
        SCL_LCD_1;
        TicksDelayUs(TT);
        SCL_LCD_0;
        input <<= 1;
    }
}
//�������ݡ�������Һ��
uint8_t I2cSendData(uint8_t WordAddress, uint8_t WordAddress1, uint8_t *s, uint8_t length)
{
    uint8_t i;
    SDA_LCD_OUT;
    SCL_LCD_OUT;

    Start();
    Write8bit(W_DEVICE_ADR_LCD);
    if (TestAck1())
    {
        Stop();
        return 1;
    }
    Write8bit(WordAddress);
    if (TestAck1())
    {
        Stop();
        return 1;
    }
    Write8bit(WordAddress1);
    if (TestAck1())
    {
        Stop();
        return 1;
    }
    for (i = 0; i < length - 1; i++)
    {
        Write8bit(*s);
        if (TestAck1())
        {
            Stop();
            return 1;
        }
        s++;
    }
    Write8bit(*s);
    if (TestAck1())
    {
        Stop();
        return 1;
    }
    Stop();
    return 0;
}
//�������������Һ��
uint8_t I2cSendCommand(uint8_t com, uint8_t com1)
{
    SDA_LCD_OUT;
    SCL_LCD_OUT;
    Start();
    Write8bit(W_DEVICE_ADR_LCD);
    if (TestAck1())
    {
        Stop();
        return 1;
    }
    Write8bit(com);
    if (TestAck1())
    {
        Stop();
        return 1;
    }
    Write8bit(com1);
    if (TestAck1())
    {
        Stop();
        return 1;
    }
    Stop();
    return 0;
}
