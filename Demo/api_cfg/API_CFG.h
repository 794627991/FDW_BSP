#ifndef __API_CFG_H__
#define __API_CFG_H__

#include "isthisboot.h"

#ifndef ThisBoot
#define ThisBoot 0
#endif

/***********************************************************************
//---------------------------BOOT�������------------------------------//
//-----------------------����ʵ��ʹ�ô����������-----------------------//
//-------------------�� UseHowMany > 0 ʱ U_Ux_U ��Ч------------------//
//------------------------U_Ux_U ��ʵ�ʵĴ��ں�------------------------//
***********************************************************************/
#define UseHowMany 1 /* ����ʵ������ģ��ü�������д�� */
#define U_U0_U 1     /* ȡֵ��Χ0-5 һ����Ҫ�ظ� */
#define U_U1_U 2
#define U_U2_U 3
#define U_U3_U 4
#define U_U4_U 5
#define U_U5_U 0

/***********************************************************************
//-----------------------Flash�������������---------------------------//
//--------------------ע:��ʹ��Զ������ע��÷���-----------------------//
//------------------ע:BOOTENDADR������0x200��������-------------------//
//------------------ע:APP1ENDADR������0x200��������-------------------//
***********************************************************************/
#define API_UseDataUp 0      /* Զ������ ����0���� */
#define FlashAllSize 0x40000 /* G045/48�̶�0x40000 */
#define BOOTENDADR 0x4000   /* BOOT���Ĵ�С */
#define APP1ENDADR 0x20000   /* APP1��APP2�ķֽ��ַ */

/***********************************************************************
//---------------------------ϵͳ�������------------------------------//
***********************************************************************/
#define RCHFCLKCFG 36 /* ϵͳʱ����Ƶ���ã�8, 16, 24, 36MHz*/

#define UseBootFixSys 0 /* boot�̻���Ƭ�����ĳ�����ϵͳ������ */
#define UseBootFixFun 0 /* boot�̻�API���� */

#define UseuCOS 1      /* ���ʹ�ò���ϵͳ,��Ҫ��ETIM4�ṩ�����ʱ  ����0ʹ��ϵͳ */
#define Use32BitEtim 1 /* ��������޷�ʹ��ETM1��ETM2�������ܹ�ʹ��ETM4  ʹ��32λ��ʱ���ܹ��ṩCPUʹ������չ */
#define UseEvent 1     /* ʹ���¼��ķ�ʽ ����� Use_uCOS>0 ��ʹ������Ҳ��Ч */
#define UseuCGUI 1     /* ����0ʹ��GUI ��GUIֻ֧����APP��ʹ�� */

/***********************************************************************
//---------------------------IO���ж�����-----------------------------//
***********************************************************************/
#define SoftDelayTime 5 /* ����IO�������ʱȥ�� ��λms */

/***********************************************************************
//---------------------------Ӳ����Χ����------------------------------//
***********************************************************************/
#define API_UseRTC 1 /* Ӳ��RTC ����0���� */
#define API_UseADC 1 /* ADC ����0���� */
#define API_UseAES 0 /* Ӳ��AES ����0���� */

/***********************************************************************
//-----------------------------��������-------------------------------//
//-------------------ע:iotͨ�Ŵ��ڲ�����ʹ���жϷ�--------------------//
***********************************************************************/
#define U0Rxlen 1 /* ����x���յ���󻺴� */

#define U1Rxlen 1

#define U2Rxlen 200

#define U3Rxlen 500

#define U4Rxlen 1

#define U5Rxlen 1

#define Uart_Output 1    /* ʹ��printf���������Ϣ,��������ѡ��Ҫʹ�õĴ��� */
#define Use_Uartx_Send 3 /* ʹ�ô���x��ӡ��Ϣ  0-5 */
#define OutBaud 115200   /* ����Ĳ����� */

#define UseUart0 0    /* UART0 ����0���� */
#define U0USEIRMod 0  /* ����0ʹ�õ��ƺ��⣬ע��ʹ�õ��ƺ���̶�2400������ */
#define U0Baud 115200 /* ��Ҫ�Ĳ����� */
#define U0UseRxIe 1   /* �����ж� ����0���� */
#define U0UseTxIe 0   /* �����ж� ����0���� */

#define UseUart1 0    /* UART1 ����0���� */
#define U1USEIRMod 0  /* ����0ʹ�õ��ƺ��� */
#define U1Baud 9600   /* ��Ҫ�Ĳ����� */
#define U1UseRxIe 1   /* �����ж� ����0���� */
#define U1UseTxIe 0   /* �����ж� ����0���� */
#define U1UsePE3PE4 1 /* 0��Uart1ʹ��PB0PB1  1��Uart1ʹ��PE3PE4 */

#define UseUart2 1   /* UART2 ����0���� */
#define U2USEIRMod 0 /* ����0ʹ�õ��ƺ��� */
#define U2Baud 9600  /* ��Ҫ�Ĳ����� */
#define U2UseRxIe 1  /* �����ж� ����0���� */
#define U2UseTxIe 0  /* �����ж� ����0���� */

#define UseUart3 1    /* UART3 ����0���� */
#define U3USEIRMod 0  /* ����0ʹ�õ��ƺ��� */
#define U3Baud 115200 /* ��Ҫ�Ĳ����� */
#define U3UseRxIe 1   /* �����ж� ����0���� */
#define U3UseTxIe 1   /* �����ж� ����0���� */

#define UseUart4 0   /* UART4 ����0���� */
#define U4USEIRMod 0 /* ����0ʹ�õ��ƺ��� */
#define U4Baud 9600  /* ��Ҫ�Ĳ����� */
#define U4UseRxIe 1  /* �����ж� ����0���� */
#define U4UseTxIe 0  /* �����ж� ����0���� */

#define UseUart5 0   /* UART4 ����0���� */
#define U5USEIRMod 0 /* ����0ʹ�õ��ƺ��� */
#define U5Baud 9600  /* ��Ҫ�Ĳ����� */
#define U5UseRxIe 1  /* �����ж� ����0���� */
#define U5UseTxIe 0  /* �����ж� ����0���� */

#define API_UseLPUart 0 /* LPUart ����0���� */
#define LPUBaud 9600    /* ��Ҫ�Ĳ����� */

/***********************************************************************
//--------------------------��ʱ���������-----------------------------//
***********************************************************************/
#define Use_UartTimeOut 1 /* ʹ�ô��ڳ�ʱ����,ʹ�øù��ܻ�ռ��ETIM3 */
#define UseWhichTim 3     /* ʹ�ö�ʱ��1-4��Ϊ���ڳ�ʱ��ʱ �������Use_UartTimeOut�������û����ô���Ĭ��ʹ��ETM3*/
#define TmOut_Uart0 0     /* ������Ҫʹ�ó�ʱ���ܵĴ��� */
#define TmOut_Uart1 0
#define TmOut_Uart2 1
#define TmOut_Uart3 1
#define TmOut_Uart4 0
#define TmOut_Uart5 0

#define API_UseLPTIM 0 /* LPTIM ����0���� */
#define LPPRE 7        /* LPTIM��Ƶϵ��  0-7  2^(pre+1)����Ƶ */
#define LPCount 100    /* ����Ŀ��ֵ  ʱ��Դ32.768khz  �������ʱ��Ϊ : LPCount/(32768/2^(pre+1)) */

#define UseETIM1 0 /* ETIMx ����0���� �����ĸ���ʱ���жϱ��뿪���������޷������ж� */
#define UseETIM2 0
#define UseETIM3 0
#define UseETIM4 0

/***********************************************************************
//---------------------------Ӳ��IIC����------------------------------//
***********************************************************************/
#define API_UseIIC 1      /* Ӳ��IIC ����0���� */
#define IIC_ChipWL 99     /* IIC����һ��ҳд����󳤶�,�������й�ϵ */
#define DEVICE_EE256 0X7C /* I2C������ַ */
#define ADDRLEN_EE256 1   /* I2C���ݵ�ַ����Ϊx�ֽ� */
#define IIC_SPEED 100     /* IIC�ٶȣ����100k */

/***********************************************************************
//---------------------------�����������------------------------------//
***********************************************************************/
#define UseNBBand 0      /* ʹ�õ�Ƶ�� 0��ȫƵ�Σ�1�����ţ�2���ƶ���ͨ��3��ֻ��ͨ*/
#define TS_Uart 3        /* ����ģʽ��������� ȡֵ��0-5 ��Ҫ�� NB_Uart �ظ� */
#define NB_Uart 2        /* ������ģ��ͨѶ�Ĵ��� ȡֵ��0-5  */
#define vLinkMaxTime 300 /* �������ʱ��(�ӿ�ʼ���Ӿͼ���)  ��λΪ�� */
#define NB_CLK 1         /* �Զ�Уʱ����,���rtcͬʱʹ��  ����0���� */
#define CoapProfile 0    /* �������1�ǵ��Լ�дprofile */

/***********************************************************************
//---------------------------LCD�������------------------------------//
***********************************************************************/
#define API_UseLCD 0 /* LCD ����0���� */
#define ShowMaxLen 6 /* ��ʽҺ����ʾ��������ָ���������С��4 */

#define LCD_a BIT7
#define LCD_b BIT6
#define LCD_c BIT5
#define LCD_d BIT0
#define LCD_dp BIT4
#define LCD_f BIT3
#define LCD_g BIT2
#define LCD_e BIT1

/* ����Ϊʵ��Һ����Ҫӳ��Ķ�Ӧ���� */
#define COM1 COM0_Seg0_31
#define COM2 COM1_Seg0_31
#define COM3 COM2_Seg0_31
#define COM4 COM3_Seg0_31

/* ���ź͵�Ƭ���Ķ����Ӧ   
G_LCD_SEGx  Ϊʵ��Һ������  
SEGxΪ��Ƭ������ */
#define G_LCD_SEG2 SEG0
#define G_LCD_SEG3 SEG1
#define G_LCD_SEG4 SEG2
#define G_LCD_SEG5 SEG3

#define G_LCD_SEG6 SEG6
#define G_LCD_SEG7 SEG7
#define G_LCD_SEG8 SEG8
#define G_LCD_SEG9 SEG9

#define G_LCD_SEG12 SEG17
#define G_LCD_SEG13 SEG16
#define G_LCD_SEG14 SEG15
#define G_LCD_SEG15 SEG14
#define G_LCD_SEG16 SEG13
#define G_LCD_SEG17 SEG12
#define G_LCD_SEG18 SEG11
#define G_LCD_SEG19 SEG10

/* ��Ҫ��Һ����ֵ�����Ŷ�Ӧ */
// #define LCD_1_EFGDP G_LCD_SEG3
// #define LCD_1_ABCD G_LCD_SEG2

// #define LCD_2_EFGDP G_LCD_SEG3
// #define LCD_2_ABCD G_LCD_SEG4

// #define LCD_3_EFGDP G_LCD_SEG5
// #define LCD_3_ABCD G_LCD_SEG7

// #define LCD_4_EFGDP G_LCD_SEG8
// #define LCD_4_ABCD G_LCD_SEG9

// #define LCD_5_EFGDP G_LCD_SEG19
// #define LCD_5_ABCD G_LCD_SEG20

// #define LCD_6_EFGDP G_LCD_SEG10
// #define LCD_6_ABCD G_LCD_SEG11

// #define LCD_7_EFGDP G_LCD_SEG22
// #define LCD_7_ABCD G_LCD_SEG12

// #define LCD_8_EFGDP G_LCD_SEG13
// #define LCD_8_ABCD G_LCD_SEG26

/* ��Ҫ��Һ����ֵ�����Ŷ�Ӧ */
#define LCD_1_EFGD G_LCD_SEG3
#define LCD_1_ABCDP G_LCD_SEG19

#define LCD_2_EFGD G_LCD_SEG4
#define LCD_2_ABCDP G_LCD_SEG18

#define LCD_3_EFGD G_LCD_SEG5
#define LCD_3_ABCDP G_LCD_SEG16

#define LCD_4_EFGD G_LCD_SEG6
#define LCD_4_ABCDP G_LCD_SEG15

#define LCD_5_EFGD G_LCD_SEG7
#define LCD_5_ABCDP G_LCD_SEG14

#define LCD_6_EFGD G_LCD_SEG8
#define LCD_6_ABCDP G_LCD_SEG13

/* ��Ҫ��Һ����ֵ���com��Ӧ */
#define L_A COM1
#define L_B COM2
#define L_C COM3
#define L_D COM4
#define L_E COM3
#define L_F COM1
#define L_G COM2

#endif
