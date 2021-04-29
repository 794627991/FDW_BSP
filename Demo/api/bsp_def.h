/*
*********************************************************************************************************
*
*	ģ������ : ��BSP����(FOR����΢G045/G048)
*	�ļ����� : bsp_def.h
*	��    �� : V1.0.0
*	˵    �� : ȫ���ؼ����壬�������ͺ궨��ȣ�����Ӳ���ײ�������������ļ�
*              ÿ��h�ļ����� #include "bsp_def.h" ��������Ӧ������ϵ
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����  ��ʽ����
*
*	Copyright (C), 2020-2030, ����˼��-����
*
*********************************************************************************************************
*/
#ifndef __BSP_DEF_H__
#define __BSP_DEF_H__

#if defined(__CC_ARM)
#pragma anon_unions /* �������������� */
/* Suppress warning message: extended constant initialiser used*/
#pragma diag_suppress 1296 /* ����1296���� (���ں����̻����µı���)*/
#pragma diag_suppress 1293 /* ����1293���� (while�ڸ�ֵ)*/
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#endif

#define __weak __attribute__((weak))	  /* ���������� */
#define __minalgt __attribute__((packed)) /* ��С�ֽڶ��붨�� */

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
#endif

#define STRUCT(type)                 \
	typedef struct _tag_##type type; \
	struct _tag_##type

#ifdef __DEBUG
#define debug(format, ...) printf(format, ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif

/* �������Ͷ��� */
typedef union
{
	unsigned char B08;
	struct
	{
		unsigned char bit0 : 1;
		unsigned char bit1 : 1;
		unsigned char bit2 : 1;
		unsigned char bit3 : 1;
		unsigned char bit4 : 1;
		unsigned char bit5 : 1;
		unsigned char bit6 : 1;
		unsigned char bit7 : 1;
	} Bit;
} B08_Bit;

#ifndef uint08
#define uint08 uint8_t
#endif

#ifndef uint16
#define uint16 uint16_t
#endif

#ifndef uint32
#define uint32 uint32_t
#endif

#ifndef int08
#define int08 int8_t
#endif

#ifndef int16
#define int16 int16_t
#endif

#ifndef int32
#define int32 int32_t
#endif

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned short int
#endif

#ifndef ulint
#define ulint unsigned int
#endif

#ifndef ullint
#define ullint unsigned long int
#endif

#define LOWBYTE(bt) (bt & 0x0F)
#define HIGHBYTE(bt) ((bt & 0xF0) >> 4)
#define LOWWORD(wd) (wd & 0xFF)
#define HIGHWORD(wd) ((wd & 0xFF00) >> 8)
#define LOWLONGINT(lt) (lt & 0xFF)
#define MID1LONGINT(lt) ((lt & 0xFF00) >> 8)
#define MID2LONGINT(lt) ((lt & 0xFF0000) >> 16)
#define HIGHLONGINT(lt) ((lt & 0xFF000000) >> 24)

#define EA_OFF ((__get_PRIMASK() & 0x00000001) == 0x00000001)
#define EA_ON ((__get_PRIMASK() & 0x00000001) == 0x00000000)

/* ================================================================================ */
/* ================             Peripheral include                 ================ */
/* ================================================================================ */

/* ��׼�� */
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "bintohex.h"

/* �û����� */
#include "API_CFG.h"
#include "user_io_def.h"

#if UseuCOS > 0 && ThisBoot == 0
#define Use_uCOS 1
#endif

#if Use_uCOS == 0 && UseEvent > 0
#define Use_Event 1
#endif

#if UseuCGUI > 0 && ThisBoot == 0
#include "GUI.h"
extern void Refresh(void); /* GUI��memģʽˢ����ʾ */
#endif

#if ThisBoot == 1
#undef RCHFCLKCFG
#endif

#ifndef RCHFCLKCFG
#define RCHFCLKCFG 36 /* ���δ����ϵͳʱ��Ƶ�ʣ�RCHFĬ��Ϊ8M */
#endif

#if (RCHFCLKCFG == 8)
#define clkmode 1
#define SYSCLKdef RCC_RCHFCON_FSEL_8MHZ /* RCHF����Ƶ��8MHz */
#elif (RCHFCLKCFG == 16)
#define clkmode 2
#define SYSCLKdef RCC_RCHFCON_FSEL_16MHZ /* RCHF����Ƶ��16MHz */
#elif (RCHFCLKCFG == 24)
#define clkmode 3
#define SYSCLKdef RCC_RCHFCON_FSEL_24MHZ /* RCHF����Ƶ��24MHz */
#elif (RCHFCLKCFG == 36)
#define clkmode 4
#define SYSCLKdef RCC_RCHFCON_FSEL_36MHZ /* RCHF����Ƶ��36MHz */
#endif

#define __SYSTEM_CLOCK (8000000 * clkmode)
#define DELAY_US (__SYSTEM_CLOCK / 1000000)
#define DELAY_MS (__SYSTEM_CLOCK / 1000)

#if Use_uCOS == 0 || ThisBoot > 0

#define Do_DelayStart()                   \
	{                                     \
		uint32_t LastTick = SysTick->VAL; \
		do                                \
		{

#define While_DelayMsEnd(Count)                                       \
	}                                                                 \
	while (((LastTick - SysTick->VAL) & 0xFFFFFF) < DELAY_MS * Count) \
		;                                                             \
	}

#define While_DelayUsEnd(Count)                                       \
	}                                                                 \
	while (((LastTick - SysTick->VAL) & 0xFFFFFF) < DELAY_US * Count) \
		;                                                             \
	}
#else
#if Use32BitEtim == 0
#define Do_DelayStart()                                       \
	{                                                         \
		uint32_t LastTick = ETCNT->ET4CNT;                    \
		uint32_t aa = 0;                                      \
		do                                                    \
		{                                                     \
			if (((ETCNT->ET4CNT - LastTick) & 0xFFFF) > 1000) \
			{                                                 \
				aa++;                                         \
				LastTick = ETCNT->ET4CNT;                     \
			}

#define While_DelayMsEnd(Count) \
	}                           \
	while (aa < Count)          \
		;                       \
	}
#else
#define Do_DelayStart()                                                      \
	{                                                                        \
		uint32_t LastTick = (uint32_t)(ETCNT->ET2CNT << 16) + ETCNT->ET1CNT; \
		do                                                                   \
		{
#define While_DelayMsEnd(Count)                                                                          \
	}                                                                                                    \
	while ((((uint32_t)(ETCNT->ET2CNT << 16) + ETCNT->ET1CNT - LastTick) & 0xFFFFFFFFUL) < 1000 * Count) \
		;                                                                                                \
	}
#define While_DelayUsEnd(Count)                                                                   \
	}                                                                                             \
	while ((((uint32_t)(ETCNT->ET2CNT << 16) + ETCNT->ET1CNT - LastTick) & 0xFFFFFFFFUL) < Count) \
		;                                                                                         \
	}
#endif
#endif

/* ������ */
#include "FM33G0XX.h"
#include "fm33G0xx_aes.h"
#include "fm33G0xx_anac.h"
#include "fm33G0xx_btim.h"
#include "fm33G0xx_crc.h"
#include "fm33G0xx_dma.h"
#include "fm33G0xx_etim.h"
#include "fm33G0xx_flash.h"
#include "fm33G0xx_gpio.h"
#include "fm33G0xx_i2c.h"
#include "fm33G0xx_iwdt.h"
#include "fm33G0xx_lcd.h"
#include "fm33G0xx_lptim.h"
#include "fm33G0xx_rcc.h"
#include "fm33G0xx_rtc.h"
#include "fm33G0xx_scu.h"
#include "fm33G0xx_spi.h"
#include "fm33G0xx_hspi.h"
#include "fm33G0xx_trng.h"
#include "fm33G0xx_u7816.h"
#include "fm33G0xx_uart.h"
#include "fm33G0xx_wwdt.h"
#include "fm33G0xx_pmu.h"
#include "fm33G0xx_lpuart.h"

/* bsp���Ŀ� */
#include "bsp_lib.h"
#include "bsp_core.h"

#define DelayMsStart(ms)               \
	{                                  \
		uint32_t tms;                  \
		for (tms = 0; tms < ms; tms++) \
		{
#define DelayMsEnd() \
	delay_ms(1);     \
	}                \
	}

#define DelayUsStart(us)               \
	{                                  \
		uint32_t tus;                  \
		for (tus = 0; tus < us; tus++) \
		{
#define DelayUsEnd() \
	delay_us(1);     \
	}                \
	}

#define WAITTIME 250 /* BOOT��ʹ�õĲ��� X * 4MS �ϵ��ȴ�����boot��ʱ�䣬��ʱ����ת������������ */

#ifndef BOOTENDADR
#define APPLICATION_ADDRESS 0x10000 /* ��BOOTһ��Ĭ��һ��APP��ڵ�ַ */
#else
#define APPLICATION_ADDRESS BOOTENDADR
#endif

//��ַ��Χ:PROGRAMSTA1~PROGRAMEND1~PROGRAMSTA2~PROGRAMEND2
//#define PROGRAMSTA1	0x000000	//����ռ�1��ʼ��ַ
//#ifdef __A02
//#define PROGRAMEND1	0x01FFFF	//����ռ�1������ַ
//#define PROGRAMPROZ	0x02F800	//���򱣻�����ʼ��ַ
//#endif
//#ifdef __A03
//#define PROGRAMEND1	0x02FFFF	//����ռ�1������ַ
//#define PROGRAMPROZ	0x02F800	//���򱣻�����ʼ��ַ
//#endif
//#ifdef __A04
#define PROGRAMEND1 0x03FFFF //����ռ�1������ַ
#define PROGRAMPROZ 0x03F800 //���򱣻�����ʼ��ַ
//#endif
//#ifdef __A05
//#define PROGRAMEND1	0x05FFFF	//����ռ�1������ַ
//#define PROGRAMPROZ	0x05F800	//���򱣻�����ʼ��ַ
//#endif
//#ifdef __A06
//#define PROGRAMEND1	0x07FFFF	//����ռ�1������ַ
//#define PROGRAMPROZ	0x07F800	//���򱣻�����ʼ��ַ
//#endif

/* BOOT��ʹ�õ� uart ������˳���� */
#define U_U0 0
#define U_U1 1
#define U_U2 2
#define U_U3 3
#define U_U4 4
#define U_U5 5

#if UseHowMany > 0
#if U_U0_U == 0
#define U_U0_Uart UART0 /* ʵ��ʹ�õ� */
#define IRQ_U0 U_U0
#elif U_U0_U == 1
#define U_U0_Uart UART1
#define IRQ_U1 U_U0
#elif U_U0_U == 2
#define U_U0_Uart UART2
#define IRQ_U2 U_U0
#elif U_U0_U == 3
#define U_U0_Uart UART3
#define IRQ_U3 U_U0
#elif U_U0_U == 4
#define U_U0_Uart UART4
#define IRQ_U4 U_U0
#elif U_U0_U == 5
#define U_U0_Uart UART5
#define IRQ_U5 U_U0
#endif
#endif

#if UseHowMany > 1
#if U_U1_U == 0
#define U_U1_Uart UART0
#define IRQ_U0 U_U1
#elif U_U1_U == 1
#define U_U1_Uart UART1
#define IRQ_U1 U_U1
#elif U_U1_U == 2
#define U_U1_Uart UART2
#define IRQ_U2 U_U1
#elif U_U1_U == 3
#define U_U1_Uart UART3
#define IRQ_U3 U_U1
#elif U_U1_U == 4
#define U_U1_Uart UART4
#define IRQ_U4 U_U1
#elif U_U1_U == 5
#define U_U1_Uart UART5
#define IRQ_U5 U_U1
#endif
#endif

#if UseHowMany > 2
#if U_U2_U == 0
#define U_U2_Uart UART0
#define IRQ_U0 U_U2
#elif U_U2_U == 1
#define U_U2_Uart UART1
#define IRQ_U1 U_U2
#elif U_U2_U == 2
#define U_U2_Uart UART2
#define IRQ_U2 U_U2
#elif U_U2_U == 3
#define U_U2_Uart UART3
#define IRQ_U3 U_U2
#elif U_U2_U == 4
#define U_U2_Uart UART4
#define IRQ_U4 U_U2
#elif U_U2_U == 5
#define U_U2_Uart UART5
#define IRQ_U5 U_U2
#endif
#endif

#if UseHowMany > 3
#if U_U3_U == 0
#define U_U3_Uart UART0
#define IRQ_U0 U_U3
#elif U_U3_U == 1
#define U_U3_Uart UART1
#define IRQ_U1 U_U3
#elif U_U3_U == 2
#define U_U3_Uart UART2
#define IRQ_U2 U_U3
#elif U_U3_U == 3
#define U_U3Uart UART3
#define IRQ_U3 U_U3
#elif U_U3_U == 4
#define U_U3_Uart UART4
#define IRQ_U4 U_U3
#elif U_U3_U == 5
#define U_U3_Uart UART5
#define IRQ_U5 U_U3
#endif
#endif

#if UseHowMany > 4
#if U_U4_U == 0
#define U_U4_Uart UART0
#define IRQ_U0 U_U4
#elif U_U4_U == 1
#define U_U4_Uart UART1
#define IRQ_U1 U_U4
#elif U_U4_U == 2
#define U_U4_Uart UART2
#define IRQ_U2 U_U4
#elif U_U4_U == 3
#define U_U4Uart UART3
#define IRQ_U3 U_U4
#elif U_U4_U == 4
#define U_U4_Uart UART4
#define IRQ_U4 U_U4
#elif U_U4_U == 5
#define U_U4_Uart UART5
#define IRQ_U5 U_U4
#endif
#endif

#if UseHowMany > 5
#if U_U5_U == 0
#define U_U5_Uart UART0
#define IRQ_U0 U_U5
#elif U_U5_U == 1
#define U_U5_Uart UART1
#define IRQ_U1 U_U5
#elif U_U5_U == 2
#define U_U5_Uart UART2
#define IRQ_U2 U_U5
#elif U_U5_U == 3
#define U_U5_Uart UART3
#define IRQ_U3 U_U5
#elif U_U5_U == 4
#define U_U5_Uart UART4
#define IRQ_U4 U_U5
#elif U_U5_U == 5
#define U_U5_Uart UART5
#define IRQ_U5 U_U5
#endif
#endif

#endif
