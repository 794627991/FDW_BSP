#include "12864.h"

uint8_t *membuf = NULL;
spi_op_type *SoftSPI = NULL;

void LCD_12864_Init(void)
{
    // LCD_12864_CS_W;
    // LCD_12864_CLK_W;
    // LCD_12864_SID_W;
    // Send(0, 0x30); /*��������:һ����8λ����,����ָ�*/
    // delay_us(720);
    // Send(0, 0x02); /*DDRAM��ַ��λ*/
    // delay_us(720);
    // Send(0, 0x0c); /*��ʾ�趨:����ʾ,����ʾ���,������ǰ��ʾλ��������*/
    // delay_us(720);
    // Send(0, 0x01); /*��������DDRAM��λַ����������Ϊ��00H��*/
    // delay_us(720);
    // Send(0, 0x06); /*�������ã����趨:��ʾ�ַ�/����������λ,DDRAM��ַ��1*/
    // delay_us(720);
    // Send(0, 0x34); //����չָ�

    uint8_t cmd[10];
    membuf = mymalloc(1024);
    SoftSPI = SIMLT_SPI_OP_INIT(0);
    cmd[0] = 0xfd;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = 0xdd;
    cmd[5] = 0xcc;
    cmd[6] = 0xbb;
    cmd[7] = 0xaa;
    API_SIMLT_SPI(cmd, 10);
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

void qumozhuanhuan(uint8_t *buf, const unsigned char *ptr)
{
    uint8_t i, j, k, q;
    uint16_t n = 0;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 128; j++)
        {
            q = 1 << (7 - (j % 8));
            for (k = 0; k < 8; k++)
            {
                buf[n] <<= 1;
                if (ptr[i * 128 + 16 * (8 - k) + (j >> 3)] & q)
                {
                    buf[n] += 1;
                }
            }
            n++;
        }
}

void lcd_show(const unsigned char *ptr)
{
    // uint8_t y, x;
    // for (y = 0; y < 32; y++)
    // {
    //     Send(0, 0x80 + y);
    //     Send(0, 0x80);
    //     for (x = 0; x < 16; x++)
    //     {
    //         Send(1, *ptr++);
    //     }
    //     Send(0, 0x36); //�򿪻�ͼ��ʾ
    // }

    // for (y = 0; y < 32; y++)
    // {
    //     Send(0, 0x80 + y);
    //     Send(0, 0x88);
    //     for (x = 0; x < 16; x++)
    //     {
    //         Send(1, *ptr++);
    //     }
    //     Send(0, 0x36); //�򿪻�ͼ��ʾ
    // }

    //fd 34 00 08 00 00 00 00 00 80 00 40 ��ͼƬ���ݣ�dd cc bb aa

    uint8_t cmd[15];
    uint16_t i = 0;
    qumozhuanhuan(membuf, ptr);
    cmd[0] = 0xfd;
    cmd[1] = 0x34;
    cmd[2] = 0;
    cmd[3] = 0x08;

    cmd[4] = 0;
    cmd[5] = 0; //x

    cmd[6] = 0;
    cmd[7] = 0; //y

    cmd[8] = 0;
    cmd[9] = 0x80; //Width

    cmd[10] = 0;
    cmd[11] = 0x40; //Hight

    SoftSPI->nss(0);
    for (i = 0; i < 12; i++)
    {
        SoftSPI->op(cmd[i]);
    }
    for (i = 0; i < 1024; i++)
    {
        SoftSPI->op(membuf[i]);
    }
    cmd[0] = 0xdd;
    cmd[1] = 0xcc;
    cmd[2] = 0xbb;
    cmd[3] = 0xaa;
    for (i = 0; i < 4; i++)
    {
        SoftSPI->op(cmd[i]);
    }
    SoftSPI->nss(1);
}
