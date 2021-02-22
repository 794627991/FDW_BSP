#include "12864.h"

void LCD_12864_Init(void)
{
    LCD_12864_CS_W;
    LCD_12864_CLK_W;
    LCD_12864_SID_W;
    Send(0, 0x30); /*��������:һ����8λ����,����ָ�*/
    delay_us(720);
    Send(0, 0x02); /*DDRAM��ַ��λ*/
    delay_us(720);
    Send(0, 0x0c); /*��ʾ�趨:����ʾ,����ʾ���,������ǰ��ʾλ��������*/
    delay_us(720);
    Send(0, 0x01); /*��������DDRAM��λַ����������Ϊ��00H��*/
    delay_us(720);
    Send(0, 0x06); /*�������ã����趨:��ʾ�ַ�/����������λ,DDRAM��ַ��1*/
    delay_us(720);
    Send(0, 0x34); //����չָ�
}

/*******************************************
�������ƣ�Send
��    �ܣ�MCU��Һ��ģ�鷢��1һ���ֽڵ�����
��    ����type--�������ͣ�0--�������1--��ʾ����
transdata--���͵�����
����ֵ  ����
********************************************/
void Send(uchar type, uchar transdata)
{
    uchar firstbyte = 0xf8;
    uchar temp;
    uchar i, j = 3;
    LCD_12864_SID_W;
    if (type)
        firstbyte |= 0x02;

    LCD_12864_CS_1;
    LCD_12864_CLK_0;
    while (j > 0)
    {
        if (j == 3)
            temp = firstbyte;
        else if (j == 2)
            temp = transdata & 0xf0;
        else
            temp = (transdata << 4) & 0xf0;

        for (i = 8; i > 0; i--)
        {
            if (temp & 0x80)
                LCD_12864_SID_1;
            else
                LCD_12864_SID_0;
            LCD_12864_CLK_1;
            temp <<= 1;
            LCD_12864_CLK_0;
        }
        j--;
    }

    LCD_12864_SID_0;
    LCD_12864_CS_0;
}

void lcd_show(const unsigned char *ptr)
{
    uchar y, x;
    for (y = 0; y < 32; y++)
    {
        Send(0, 0x80 + y);
        Send(0, 0x80);
        for (x = 0; x < 16; x++)
        {
            Send(1, *ptr++);
        }
        Send(0, 0x36); //�򿪻�ͼ��ʾ
    }

    for (y = 0; y < 32; y++)
    {
        Send(0, 0x80 + y);
        Send(0, 0x88);
        for (x = 0; x < 16; x++)
        {
            Send(1, *ptr++);
        }
        Send(0, 0x36); //�򿪻�ͼ��ʾ
    }
}
