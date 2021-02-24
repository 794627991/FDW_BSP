#ifndef __API_CFG_H__
#define __API_CFG_H__

#include "isthisboot.h"

#ifndef ThisBoot
#define ThisBoot 0
#endif

/***********************************************************************
//---------------------------BOOT相关设置------------------------------//
//-----------------------根据实际使用串口情况配置-----------------------//
//-------------------当 UseHowMany > 0 时 U_Ux_U 有效------------------//
//------------------------U_Ux_U 是实际的串口号------------------------//
***********************************************************************/
#define UseHowMany 1 /* 根据实际情况改，用几个串口写几 */
#define U_U0_U 1     /* 取值范围0-5 一定不要重复 */
#define U_U1_U 2
#define U_U2_U 3
#define U_U3_U 4
#define U_U4_U 5
#define U_U5_U 0

/***********************************************************************
//-----------------------Flash和升级相关设置---------------------------//
//--------------------注:若使用远程升级注意好分区-----------------------//
//------------------注:BOOTENDADR必须是0x200的整数倍-------------------//
//------------------注:APP1ENDADR必须是0x200的整数倍-------------------//
***********************************************************************/
#define API_UseDataUp 0      /* 远程升级 大于0开启 */
#define FlashAllSize 0x40000 /* G045/48固定0x40000 */
#define BOOTENDADR 0x4000   /* BOOT区的大小 */
#define APP1ENDADR 0x20000   /* APP1和APP2的分界地址 */

/***********************************************************************
//---------------------------系统相关设置------------------------------//
***********************************************************************/
#define RCHFCLKCFG 36 /* 系统时钟主频配置：8, 16, 24, 36MHz*/

#define UseBootFixSys 0 /* boot固化单片机核心程序，跑系统不能用 */
#define UseBootFixFun 0 /* boot固化API程序 */

#define UseuCOS 1      /* 如果使用操作系统,需要由ETIM4提供软件延时  大于0使用系统 */
#define Use32BitEtim 1 /* 当定义后，无法使用ETM1和ETM2，但是能够使用ETM4  使用32位定时器能够提供CPU使用率扩展 */
#define UseEvent 1     /* 使用事件的方式 ，如果 Use_uCOS>0 即使定义了也无效 */
#define UseuCGUI 1     /* 大于0使用GUI ，GUI只支持在APP中使用 */

/***********************************************************************
//---------------------------IO口中断设置-----------------------------//
***********************************************************************/
#define SoftDelayTime 5 /* 用于IO口软件延时去抖 单位ms */

/***********************************************************************
//---------------------------硬件外围设置------------------------------//
***********************************************************************/
#define API_UseRTC 1 /* 硬件RTC 大于0开启 */
#define API_UseADC 1 /* ADC 大于0开启 */
#define API_UseAES 0 /* 硬件AES 大于0开启 */

/***********************************************************************
//-----------------------------串口设置-------------------------------//
//-------------------注:iot通信串口不建议使用中断发--------------------//
***********************************************************************/
#define U0Rxlen 1 /* 串口x接收的最大缓存 */

#define U1Rxlen 1

#define U2Rxlen 200

#define U3Rxlen 500

#define U4Rxlen 1

#define U5Rxlen 1

#define Uart_Output 1    /* 使用printf输出调试信息,并在下面选择要使用的串口 */
#define Use_Uartx_Send 3 /* 使用串口x打印消息  0-5 */
#define OutBaud 115200   /* 输出的波特率 */

#define UseUart0 0    /* UART0 大于0开启 */
#define U0USEIRMod 0  /* 大于0使用调制红外，注：使用调制红外固定2400波特率 */
#define U0Baud 115200 /* 需要的波特率 */
#define U0UseRxIe 1   /* 接收中断 大于0开启 */
#define U0UseTxIe 0   /* 发送中断 大于0开启 */

#define UseUart1 0    /* UART1 大于0开启 */
#define U1USEIRMod 0  /* 大于0使用调制红外 */
#define U1Baud 9600   /* 需要的波特率 */
#define U1UseRxIe 1   /* 接收中断 大于0开启 */
#define U1UseTxIe 0   /* 发送中断 大于0开启 */
#define U1UsePE3PE4 1 /* 0：Uart1使用PB0PB1  1：Uart1使用PE3PE4 */

#define UseUart2 1   /* UART2 大于0开启 */
#define U2USEIRMod 0 /* 大于0使用调制红外 */
#define U2Baud 9600  /* 需要的波特率 */
#define U2UseRxIe 1  /* 接收中断 大于0开启 */
#define U2UseTxIe 0  /* 发送中断 大于0开启 */

#define UseUart3 1    /* UART3 大于0开启 */
#define U3USEIRMod 0  /* 大于0使用调制红外 */
#define U3Baud 115200 /* 需要的波特率 */
#define U3UseRxIe 1   /* 接收中断 大于0开启 */
#define U3UseTxIe 1   /* 发送中断 大于0开启 */

#define UseUart4 0   /* UART4 大于0开启 */
#define U4USEIRMod 0 /* 大于0使用调制红外 */
#define U4Baud 9600  /* 需要的波特率 */
#define U4UseRxIe 1  /* 接收中断 大于0开启 */
#define U4UseTxIe 0  /* 发送中断 大于0开启 */

#define UseUart5 0   /* UART4 大于0开启 */
#define U5USEIRMod 0 /* 大于0使用调制红外 */
#define U5Baud 9600  /* 需要的波特率 */
#define U5UseRxIe 1  /* 接收中断 大于0开启 */
#define U5UseTxIe 0  /* 发送中断 大于0开启 */

#define API_UseLPUart 0 /* LPUart 大于0开启 */
#define LPUBaud 9600    /* 需要的波特率 */

/***********************************************************************
//--------------------------定时器相关设置-----------------------------//
***********************************************************************/
#define Use_UartTimeOut 1 /* 使用串口超时功能,使用该功能会占用ETIM3 */
#define UseWhichTim 3     /* 使用定时器1-4作为串口超时计时 如果开启Use_UartTimeOut，不配置或配置错误，默认使用ETM3*/
#define TmOut_Uart0 0     /* 配置需要使用超时功能的串口 */
#define TmOut_Uart1 0
#define TmOut_Uart2 1
#define TmOut_Uart3 1
#define TmOut_Uart4 0
#define TmOut_Uart5 0

#define API_UseLPTIM 0 /* LPTIM 大于0开启 */
#define LPPRE 7        /* LPTIM分频系数  0-7  2^(pre+1)倍分频 */
#define LPCount 100    /* 计数目标值  时钟源32.768khz  具体计数时间为 : LPCount/(32768/2^(pre+1)) */

#define UseETIM1 0 /* ETIMx 大于0开启 用了哪个定时器中断必须开启，否则无法开启中断 */
#define UseETIM2 0
#define UseETIM3 0
#define UseETIM4 0

/***********************************************************************
//---------------------------硬件IIC设置------------------------------//
***********************************************************************/
#define API_UseIIC 1      /* 硬件IIC 大于0开启 */
#define IIC_ChipWL 99     /* IIC允许一次页写的最大长度,和器件有关系 */
#define DEVICE_EE256 0X7C /* I2C器件地址 */
#define ADDRLEN_EE256 1   /* I2C数据地址长度为x字节 */
#define IIC_SPEED 100     /* IIC速度，最大100k */

/***********************************************************************
//---------------------------联网相关设置------------------------------//
***********************************************************************/
#define UseNBBand 0      /* 使用的频段 0：全频段，1：电信，2：移动联通，3：只联通*/
#define TS_Uart 3        /* 调试模式的输出串口 取值：0-5 不要和 NB_Uart 重复 */
#define NB_Uart 2        /* 用于与模块通讯的串口 取值：0-5  */
#define vLinkMaxTime 300 /* 连接最大时长(从开始连接就计算)  单位为秒 */
#define NB_CLK 1         /* 自动校时功能,需和rtc同时使用  大于0开启 */
#define CoapProfile 0    /* 如果大于1记得自己写profile */

/***********************************************************************
//---------------------------LCD相关设置------------------------------//
***********************************************************************/
#define API_UseLCD 0 /* LCD 大于0开启 */
#define ShowMaxLen 6 /* 段式液晶显示的最大数字个数，不能小于4 */

#define LCD_a BIT7
#define LCD_b BIT6
#define LCD_c BIT5
#define LCD_d BIT0
#define LCD_dp BIT4
#define LCD_f BIT3
#define LCD_g BIT2
#define LCD_e BIT1

/* 以下为实际液晶需要映射的对应引脚 */
#define COM1 COM0_Seg0_31
#define COM2 COM1_Seg0_31
#define COM3 COM2_Seg0_31
#define COM4 COM3_Seg0_31

/* 引脚和单片机的段码对应   
G_LCD_SEGx  为实际液晶引脚  
SEGx为单片机段码 */
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

/* 需要和液晶真值表引脚对应 */
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

/* 需要和液晶真值表引脚对应 */
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

/* 需要和液晶真值表的com对应 */
#define L_A COM1
#define L_B COM2
#define L_C COM3
#define L_D COM4
#define L_E COM3
#define L_F COM1
#define L_G COM2

#endif
