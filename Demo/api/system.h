/*
*********************************************************************************************************
*
*	模块名称 : BSP基础声明
*	文件名称 : system.h
*	版    本 : V1.0.0
*	说    明 : BSP的基础声明和关键宏
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-1-6    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

#define MAX_NVIC_PRIO 3 /* 中断优先级最大值 */

#define SYSCORE 1 /* 上电初始化的核心，这是必要的 */

#define Boot_SYS_Base_Adr 0x00000950 /* boot固化SYS到boot区的0x950 */

#define SYS_DelayMs_Loc 0
#define SYS_DelayUs_Loc 1
#define SYS_AnalogIO_Loc 2
#define SYS_InputtIO_Loc 3
#define SYS_OutputIO_Loc 4
#define SYS_AltfunIO_Loc 5
#define SYS_CloseeIO_Loc 6
#define SYS_SysmInit_Loc 7
#define SYS_Sleep_Loc 8
#define SYS_RchfadJust_Loc 9

#ifndef RCHFCLKCFG
#define RCHFCLKCFG 8 /* 如果未定义系统时钟频率，RCHF默认为8M */
#endif

#if (RCHFCLKCFG == 8)
#define clkmode 1
#define SYSCLKdef RCC_RCHFCON_FSEL_8MHZ /* RCHF中心频率8MHz */
#elif (RCHFCLKCFG == 16)
#define clkmode 2
#define SYSCLKdef RCC_RCHFCON_FSEL_16MHZ /* RCHF中心频率16MHz */
#elif (RCHFCLKCFG == 24)
#define clkmode 3
#define SYSCLKdef RCC_RCHFCON_FSEL_24MHZ /* RCHF中心频率24MHz */
#elif (RCHFCLKCFG == 36)
#define clkmode 4
#define SYSCLKdef RCC_RCHFCON_FSEL_36MHZ /* RCHF中心频率36MHz */
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

#define delay_ms(x) TicksDelayMs(x)
#define delay_us(x) TicksDelayUs(x)

/* GPIO配置函数参数宏定义 */
/* IO输入口配置 */
#define IN_NORMAL 0 /* 普通 */
#define IN_PULLUP 1 /* 上拉 */

/* IO输出口配置 */
#define OUT_PUSHPULL 0	/* 普通 */
#define OUT_OPENDRAIN 1 /* OD */

/* IO数字特殊功能口 */
#define ALTFUN_NORMAL 0			  /* 普通 */
#define ALTFUN_OPENDRAIN 1		  /* OD (OD功能仅部分特殊功能支持) */
#define ALTFUN_PULLUP 2			  /* 普通+上拉 */
#define ALTFUN_OPENDRAIN_PULLUP 3 /* OD+上拉 */

	typedef struct
	{
		uint8_t EXTI0IF;
		uint8_t EXTI1IF;
		uint8_t EXTI2IF;
	} IOEXTI_Type; /* 跑系统时读取IO口中断用 */
	extern IOEXTI_Type IOEXTI;

	/**************************函数指针定义*************************/
	typedef int (*ConditionHook)(void);
	typedef void (*gy_delay)(uint32_t time);
	typedef void (*gy_tyioa)(GPIOx_Type *GPIOx, uint32_t PinNum);
	typedef void (*gy_tyiob)(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type);
	typedef void (*gy_initsystem)(void);
	typedef void (*gy_sleep)(uint8_t sleepmode);
	typedef int32_t (*gy_rchfadjust)(ETIMx_Type *ETIMx, uint8_t clkMode);

	/****************************函数声明****************************/
	char API_GPIO_Judge(GPIOx_Type *GPIOx, uint32_t GPIO_Pin);

	void TicksDelayMs(uint32_t ms);
	void TicksDelayUs(uint32_t us);
	void TicksDelay(uint32_t ClkNum);

	void CloseeIO(GPIOx_Type *GPIOx, uint32_t PinNum);
	void AnalogIO(GPIOx_Type *GPIOx, uint32_t PinNum);
	void InputtIO(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type);
	void OutputIO(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type);
	void AltFunIO(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type);

	void Init_System(void);
	void Sleep(uint8_t sleepmode);
	void NVIC_SET(IRQn_Type IRQn, uint32_t priority);
	int32_t RchfAdjust(ETIMx_Type *ETIMx, uint8_t clkMode);

#ifdef __cplusplus
}
#endif

#endif
