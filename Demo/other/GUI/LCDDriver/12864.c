#include "12864.h"

void LCD_12864_Init(void)
{
    LCD_12864_CS_W;
    LCD_12864_CLK_W;
    LCD_12864_SID_W;
    Send(0, 0x30); /*功能设置:一次送8位数据,基本指令集*/
    delay_us(720);
    Send(0, 0x02); /*DDRAM地址归位*/
    delay_us(720);
    Send(0, 0x0c); /*显示设定:开显示,不显示光标,不做当前显示位反白闪动*/
    delay_us(720);
    Send(0, 0x01); /*清屏，将DDRAM的位址计数器调整为“00H”*/
    delay_us(720);
    Send(0, 0x06); /*功能设置，点设定:显示字符/光标从左到右移位,DDRAM地址加1*/
    delay_us(720);
    Send(0, 0x34); //打开扩展指令集
}

/*******************************************
函数名称：Send
功    能：MCU向液晶模块发送1一个字节的数据
参    数：type--数据类型，0--控制命令，1--显示数据
transdata--发送的数据
返回值  ：无
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
        Send(0, 0x36); //打开绘图显示
    }

    for (y = 0; y < 32; y++)
    {
        Send(0, 0x80 + y);
        Send(0, 0x88);
        for (x = 0; x < 16; x++)
        {
            Send(1, *ptr++);
        }
        Send(0, 0x36); //打开绘图显示
    }
}
