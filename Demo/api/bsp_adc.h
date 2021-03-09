/*
*********************************************************************************************************
*
*	模块名称 : ADC_BSP定义(FOR复旦微G045/G048)
*	文件名称 : bsp_adc.h
*	版    本 : V1.0.0
*	说    明 : ADC相关定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-12-31  高屹  正式发布
*
*	Copyright (C), 2020-2030, 辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_def.h"

#define SmpSpd_Fast 0 /* AD采样速度 */
#define SmpSpd_Medium 1
#define SmpSpd_Slow 2

#define SmpLve_3V 0 /* AD采样值大小范围,小于3V选择 SmpLve_3V,大于3小于5选择 SmpLve_5V */
#define SmpLve_5V 1

#define SampleMax 16 /* ADC采样的最大次数 */

	typedef struct
	{
		uint16_t Bufu16[SampleMax]; /* 数据缓冲区地址 */
		uint8_t SampleNeed;			/* 需要采样的数据个数 */
		uint8_t SampleCount;		/* 已经采样的数据个数 */
	} ADC_Buf_Type;

	typedef struct
	{
		uint8_t Nvicpry;		 /* 中断优先级 */
		uint8_t ChSel_def;		 /* 采集通道选择 */
		uint8_t TrimV;			 /* 参考电压选择  0,1,2   数值越大采集需要的时间越长 */
		uint8_t SampleNeed;		 /* 采样次数选择  不可以小于2 */
		uint8_t SampleLevel;	 /* 采样等级  0：采样电压小于3V  1：采样电压小于5V  注：只能采集5V以下的电压 */
		ADC_Buf_Type ADC_Struct; /* ADC采样缓存 */
	} ADC_INIT_Type;
	extern ADC_INIT_Type ADC_init;

	/**************************函数指针定义**************************/
	typedef uint32_t (*gy_adcfun)(ADC_INIT_Type *init);
	/****************************函数声明****************************/
	void ADC_IRQ_CallBack(void);
	uint32_t API_ADC(ADC_INIT_Type *init);
	float API_Temperature(ADC_INIT_Type *init);

#ifdef __cplusplus
}
#endif

#endif
