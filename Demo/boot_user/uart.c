#include "boot_define_all.h"

struct COMMUNICATION_FRM Uart[UseHowMany];

unsigned char Cal_ChkSum(unsigned char *Buf, unsigned char Len)
{
    unsigned char i, csdata = 0;

    for (i = 0; i < Len; i++)
    {
        csdata += Buf[i];
    }
    return (csdata);
}

//CRC-16/CCITT x16+x12+x5+1 0x1021
//Init = 0x0000
//RefIn��RefOut
//�������ߵ��ֽڵ������
unsigned int CalCRC16_CCITT(uint16 Init, uint08 *DataIn, uint16 Len)
{
    uint16 i;
    uint08 j;
    uint16 Result = 0;

    //���ƼĴ���
    CRC->CRCCR = B0110_0010;
    //�����ʼֵ
    CRC->CRCLFSR = Init; //��ֵ�Ĵ���
    //������ֵ
    CRC->CRCXOR = 0x0000; //���������Ĵ���

    for (i = 0; i < Len; i++)
    {
        CRC->CRCDR = DataIn[i];

        j = 0;
        Do_DelayStart();
        {
            if ((CRC->CRCCR & BIT3) == 0)
            {
                j = 1;
                break;
            }
        }
        While_DelayUsEnd(200);
        if (j == 0)
            return 0xFFFF;
    }

    Result = CRC->CRCDR;
    return Result;
}

unsigned int BitChange(unsigned int us_DataIn)
{
    unsigned int us_Data = us_DataIn;
    us_Data = ((us_Data & 0xFF00) >> 8) | ((us_Data & 0x00FF) << 8);
    us_Data = ((us_Data & 0xF0F0) >> 4) | ((us_Data & 0x0F0F) << 4);
    us_Data = ((us_Data & 0xCCCC) >> 2) | ((us_Data & 0x3333) << 2);
    us_Data = ((us_Data & 0xAAAA) >> 1) | ((us_Data & 0x5555) << 1);

    return (us_Data);
}

FlagStatus Uart_RxErr_ChkEx(UARTx_Type *UARTx)
{
    if ((SET == UARTx_RXSTA_PERR_Chk(UARTx)) || (SET == UARTx_RXSTA_FERR_Chk(UARTx)) || (SET == UARTx_RXSTA_OERR_Chk(UARTx)))
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

void Uart_RxErr_ClrEx(UARTx_Type *UARTx)
{
    UARTx_RXSTA_PERR_Clr(UARTx);
    UARTx_RXSTA_FERR_Clr(UARTx);
    UARTx_RXSTA_OERR_Clr(UARTx);
}

uint08 Uart_RXErr_Proc(uint08 Ch)
{
    uint08 status = 0;

#if UseHowMany > 0
    if (Ch == U_U0)
    {
        if (SET == Uart_RxErr_ChkEx(U_U0_Uart))
        {
            status = 0xaa;
            Uart_RxErr_ClrEx(U_U0_Uart);
        }
    }
#endif
#if UseHowMany > 1
    if (Ch == U_U1)
    {
        if (SET == Uart_RxErr_ChkEx(U_U1_Uart))
        {
            status = 0xaa;
            Uart_RxErr_ClrEx(U_U1_Uart);
        }
    }
#endif
#if UseHowMany > 2
    if (Ch == U_U2)
    {
        if (SET == Uart_RxErr_ChkEx(U_U2_Uart))
        {
            status = 0xaa;
            Uart_RxErr_ClrEx(U_U2_Uart);
        }
    }
#endif
#if UseHowMany > 3
    if (Ch == U_U3)
    {
        if (SET == Uart_RxErr_ChkEx(U_U3_Uart))
        {
            status = 0xaa;
            Uart_RxErr_ClrEx(U_U3_Uart);
        }
    }
#endif
#if UseHowMany > 4
    if (Ch == U_U4)
    {
        if (SET == Uart_RxErr_ChkEx(U_U4_Uart))
        {
            status = 0xaa;
            Uart_RxErr_ClrEx(U_U4_Uart);
        }
    }
#endif
#if UseHowMany > 5
    if (Ch == U_U5)
    {
        if (SET == Uart_RxErr_ChkEx(U_U5_Uart))
        {
            status = 0xaa;
            Uart_RxErr_ClrEx(U_U5_Uart);
        }
    }
#endif
    return status;
}

uint08 Judge_RX_Frame(uint08 Ch, uint08 RcvData) ////���մ���
{
    unsigned char status = 0;

    Uart[Ch].Overtime_Timer = txtimeout_max; //���ճ�ʱ

    switch (Uart[Ch].Step)
    {
    case 0x00: //�ȴ�����0x68
        if (RcvData == 0x68)
        {
            Uart[Ch].Flag &= B1111_0000; //ͨѶ��־����
            Uart[Ch].Step = 0x01;
            Uart[Ch].Len = 0;
        }
        else
        {
            status = 0xaa; //��ʾ���մ���
        }
        break;

    case 0x01: //�ȴ�����2�ֽ����ݳ���
        if ((Uart[Ch].Len) >= 2)
        {
            Uart[Ch].Total_Len = Uart[Ch].Buf[1] * 256 + RcvData; //ֻ�ϵڶ����ֽڣ����ֽڲ�����
            Uart[Ch].Step = 0x02;                                 //�ȴ����յڶ���0x68
        }
        break;

    case 0x02: //�ȴ����յڶ���0x68
        if (RcvData == 0x68)
        {
            Uart[Ch].Flag = 0x00; //ͨѶ��־����
            Uart[Ch].Step = 0x05; //�ȴ�����DATA
        }
        else
        {
            status = 0xaa; //��ʾ���մ���
        }
        break;

    case 0x05:                                        //�ȴ�����DATA
        if (Uart[Ch].Len == (Uart[Ch].Total_Len + 3)) //���ݽ�����ȫ
        {
            Uart[Ch].Step = 0x06; //�ȴ�����cs.
        }

        if (Uart[Ch].Len > (Uart[Ch].Total_Len + 3)) //֡���������
        {
            status = 0xaa; //��ʾ���մ���
        }
        break;

    case 0x06:                                        //�ȴ�����cs.
        if (Uart[Ch].Len == (Uart[Ch].Total_Len + 5)) //CRCУ�������ȫ
        {
            Uart[Ch].Step = 0x07; //�ȴ�����0x16
        }
        if (Uart[Ch].Len > (Uart[Ch].Total_Len + 5)) //У�����
        {
            status = 0xaa; //��ʾ���մ���
        }
        break;

    case 0x07: //�ȴ�����0x16
        if (RcvData == 0x16)
        {
            Uart[Ch].Step = 0x08;                    //�ֽڽ�����ȷ
            Uart[Ch].Overtime_Timer = 0x00;          //����ճ�ʱ
            Uart[Ch].Response_Timer = framedelay_mx; //����Ӧ��ʱ
            Uart[Ch].Flag |= B0000_0100;             //����֡���ݽ�����ɱ�־
        }
        else
        {
            status = 0xaa; //��ʾ���մ���
        }
        break;

    default:
        status = 0xaa; //��ʾ���մ���
        break;
    }

    if (status == 0x00) //�ֽڽ�����ȷ
    {
        Uart[Ch].Buf[Uart[Ch].Len] = RcvData;
        Uart[Ch].Len++;
    }

    if (Uart_RXErr_Proc(Ch))
    {
        status = 0xaa;
    }

    if (status == 0xaa) //���մ���
    {
        Uart[Ch].Step = 0x00;
        Uart[Ch].Flag &= B1111_1100; //���645�е���λ��㲥��ַ
    }

    return Uart[Ch].Step;
}

void Tx_Frame(uint08 Ch) //using 1	//���ݷ���
{
    uint08 Temp08;

    if (Uart[Ch].Len >= Uart[Ch].Total_Len)
    {
        Uart[Ch].Overtime_Timer = 2; //ͨ����ʱ���Ƹ�λͨѶ
    }
    else
    {
        Temp08 = Uart[Ch].Buf[Uart[Ch].Len];
        if (Ch == U_U0)
        {
            UARTx_TXREG_Write(U_U0_Uart, Temp08);
            U_U0_Uart->TXREG = Temp08;
        }
#if UseHowMany > 1
        else if (Ch == U_U1)
        {
            UARTx_TXREG_Write(U_U1_Uart, Temp08);
            U_U1_Uart->TXREG = Temp08;
        }
#endif
#if UseHowMany > 2
        else if (Ch == U_U2)
        {
            UARTx_TXREG_Write(U_U2_Uart, Temp08);
            U_U2_Uart->TXREG = Temp08;
        }
#endif
#if UseHowMany > 3
        else if (Ch == U_U3)
        {
            UARTx_TXREG_Write(U_U3_Uart, Temp08);
            U_U3_Uart->TXREG = Temp08;
        }
#endif
#if UseHowMany > 4
        else if (Ch == U_U4)
        {
            UARTx_TXREG_Write(U_U4_Uart, Temp08);
            U_U4_Uart->TXREG = Temp08;
        }
#endif
#if UseHowMany > 5
        else if (Ch == U_U5)
        {
            UARTx_TXREG_Write(U_U5_Uart, Temp08);
            U_U5_Uart->TXREG = Temp08;
        }
#endif

        Uart[Ch].Len++;

        Uart[Ch].Overtime_Timer = txtimeout_max; //���ͳ�ʱ
    }
}

void Exchange(unsigned char *BufA, unsigned char *BufB, unsigned char Len)
{
    unsigned char n;
    for (n = 0; n < Len; n++)
    {
        BufA[n] = BufB[Len - 1 - n]; //��������
    }
}

void Uart_Proc(unsigned char Ch)
{
    uint16 i;
    uint16 crc16;
    uint08 command[2];
    uint08 result;
    uint16 Temp16;
    uint16 TxDataLen;
    uint32 Temp32;
    uint32 check_len;
    uint32 Temp32_len;
    uint08 RamBuf[512];
    static uint08 firstpack = 0;

    Uart[Ch].Flag &= ~(B0000_0100);

    Temp16 = Uart[Ch].Buf[1] * 256 + Uart[Ch].Buf[2] + 4;
    crc16 = CalCRC16_CCITT(0x0000, Uart[Ch].Buf, Temp16);

    if (Uart[Ch].Buf[Temp16] * 256 + Uart[Ch].Buf[Temp16 + 1] != crc16)
    {
        return;
    }
    //�����
    command[0] = Uart[Ch].Buf[4];
    command[1] = Uart[Ch].Buf[5];
    Temp32 = (Uart[Ch].Buf[6] << 24) | (Uart[Ch].Buf[7] << 16) | (Uart[Ch].Buf[8] << 8) | (Uart[Ch].Buf[9]);

    TxDataLen = 0;
    switch (command[0])
    {
    case 0x15:
        switch (command[1])
        {
        //flash erase all
        case 0x01:
            //�洢��ee����flash�У�����flashǰ��ȡ��ֵ��FlashOpID
            FlashOpID = 0x12ABF00F; //FLASHOPKEY

            for (i = 0; i < (PROGRAMEND1 + 1 - BOOTENDADR) / 512; i++)
            {
                IWDT_Clr();
                Flash_Erase_Sector(BOOTSIZE + i, FlashOpID);
            }

            API_Erase_Sector(FlashStartAdr); //������������boot�洢��ַ

            FlashOpID = 0;
            break;

        //flash erase sector
        case 0x02:
            //�洢��ee����flash�У�����flashǰ��ȡ��ֵ��FlashOpID
            FlashOpID = 0x12ABF00F; //FLASHOPKEY
            Flash_Erase_Sector(Temp32 / 512, FlashOpID);
            FlashOpID = 0;
            break;

        //flash read sector
        case 0x04:
            TxDataLen = 512;
            memcpy(Uart[Ch].Buf + 10, (unsigned char *)Temp32, 512);
            break;

        ////flash write sector
        case 0x08:
            FlashOpID = 0x12ABF00F; //FLASHOPKEY
            result = Flsah_Write_String(Temp32, Uart[Ch].Buf + 10, 512, FlashOpID);
            if (firstpack == 0)
            {
                firstpack = 1; //д�װ���ʼ��ַ
                Flsah_Write_String(FlashStartAdr, (unsigned char *)&Temp32, 4, FLASHOPKEY);
            }
            FlashOpID = 0;
            if (result)
                Uart[Ch].Buf[4] |= 0x40;
            break;

        case 0xFD: //���CRC
            memset(RamBuf, 0x00, 512);
            TxDataLen = 6;
            check_len = (Uart[Ch].Buf[10] << 24) | (Uart[Ch].Buf[11] << 16) | (Uart[Ch].Buf[12] << 8) | (Uart[Ch].Buf[13]);
            crc16 = 0x0000;
            for (Temp32_len = 0; Temp32_len < check_len; Temp32_len += 512)
            {
                crc16 = BitChange(crc16);
                memcpy(RamBuf, (unsigned char *)(Temp32 + Temp32_len), 512);
                crc16 = CalCRC16_CCITT(crc16, RamBuf, 512);
            }
            Uart[Ch].Buf[14] = crc16 >> 8;
            Uart[Ch].Buf[15] = crc16;
            break;

        //��ת����������
        case 0xFE:
            BootEnable = 0x00;
            break;

        //����bootģʽ
        case 0xFF:
            BootEnable = 0x55;
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }

    Uart[Ch].Buf[4] |= 0x80;
    TxDataLen = TxDataLen + 6;
    Uart[Ch].Buf[1] = (TxDataLen >> 8) & 0x00FF;
    Uart[Ch].Buf[2] = TxDataLen & 0x00FF;
    Temp16 = Uart[Ch].Buf[1] * 256 + Uart[Ch].Buf[2] + 4;
    crc16 = CalCRC16_CCITT(0x0000, Uart[Ch].Buf, Temp16);
    Uart[Ch].Buf[Temp16] = crc16 >> 8;
    Uart[Ch].Buf[Temp16 + 1] = crc16;
    Uart[Ch].Buf[Temp16 + 2] = 0x16;
    //���ʹ���
    Uart[Ch].Flag |= B0000_1000; //��Լ�������׼���������ݱ�־
}
