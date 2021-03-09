/*
*********************************************************************************************************
*
*	ģ������ : BSP��������
*	�ļ����� : bsp_core.h
*	��    �� : V1.0.0
*	˵    �� : BSP�Ļ��������͹ؼ���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-1-6    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#ifndef __BSP_CORE_H__
#define __BSP_CORE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

#define MAX_NVIC_PRIO 3 /* �ж����ȼ����ֵ */

#define SYSCORE 1 /* �ϵ��ʼ���ĺ��ģ����Ǳ�Ҫ�� */

#define Boot_SYS_Base_Adr 0x00000950 /* boot�̻�SYS��boot����0x950 */

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

#define delay_ms(x) TicksDelayMs(x)
#define delay_us(x) TicksDelayUs(x)

/* GPIO���ú��������궨�� */
/* IO��������� */
#define IN_NORMAL 0 /* ��ͨ */
#define IN_PULLUP 1 /* ���� */

/* IO��������� */
#define OUT_PUSHPULL 0	/* ��ͨ */
#define OUT_OPENDRAIN 1 /* OD */

/* IO�������⹦�ܿ� */
#define ALTFUN_NORMAL 0			  /* ��ͨ */
#define ALTFUN_OPENDRAIN 1		  /* OD (OD���ܽ��������⹦��֧��) */
#define ALTFUN_PULLUP 2			  /* ��ͨ+���� */
#define ALTFUN_OPENDRAIN_PULLUP 3 /* OD+���� */

	typedef struct
	{
		uint8_t EXTI0IF;
		uint8_t EXTI1IF;
		uint8_t EXTI2IF;
	} IOEXTI_Type; /* ��ϵͳʱ��ȡIO���ж��� */
	extern IOEXTI_Type IOEXTI;

	/**************************����ָ�붨��*************************/
	typedef int (*ConditionHook)(void);
	typedef void (*gy_delay)(uint32_t time);
	typedef void (*gy_tyioa)(GPIOx_Type *GPIOx, uint32_t PinNum);
	typedef void (*gy_tyiob)(GPIOx_Type *GPIOx, uint32_t PinNum, uint8_t Type);
	typedef void (*gy_initsystem)(void);
	typedef void (*gy_sleep)(uint8_t sleepmode);
	typedef int32_t (*gy_rchfadjust)(ETIMx_Type *ETIMx, uint8_t clkMode);

	/****************************��������****************************/
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

	uint32_t ReadResetState(void);
	void GotoBoot(uint8_t *buf, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif
