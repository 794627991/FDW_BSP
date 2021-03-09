/*
*********************************************************************************************************
*
*	ģ������ : ADC_BSP����(FOR����΢G045/G048)
*	�ļ����� : bsp_adc.h
*	��    �� : V1.0.0
*	˵    �� : ADC��ض���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-12-31  ����  ��ʽ����
*
*	Copyright (C), 2020-2030, ����˼��-����
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

#define SmpSpd_Fast 0 /* AD�����ٶ� */
#define SmpSpd_Medium 1
#define SmpSpd_Slow 2

#define SmpLve_3V 0 /* AD����ֵ��С��Χ,С��3Vѡ�� SmpLve_3V,����3С��5ѡ�� SmpLve_5V */
#define SmpLve_5V 1

#define SampleMax 16 /* ADC������������ */

	typedef struct
	{
		uint16_t Bufu16[SampleMax]; /* ���ݻ�������ַ */
		uint8_t SampleNeed;			/* ��Ҫ���������ݸ��� */
		uint8_t SampleCount;		/* �Ѿ����������ݸ��� */
	} ADC_Buf_Type;

	typedef struct
	{
		uint8_t Nvicpry;		 /* �ж����ȼ� */
		uint8_t ChSel_def;		 /* �ɼ�ͨ��ѡ�� */
		uint8_t TrimV;			 /* �ο���ѹѡ��  0,1,2   ��ֵԽ��ɼ���Ҫ��ʱ��Խ�� */
		uint8_t SampleNeed;		 /* ��������ѡ��  ������С��2 */
		uint8_t SampleLevel;	 /* �����ȼ�  0��������ѹС��3V  1��������ѹС��5V  ע��ֻ�ܲɼ�5V���µĵ�ѹ */
		ADC_Buf_Type ADC_Struct; /* ADC�������� */
	} ADC_INIT_Type;
	extern ADC_INIT_Type ADC_init;

	/**************************����ָ�붨��**************************/
	typedef uint32_t (*gy_adcfun)(ADC_INIT_Type *init);
	/****************************��������****************************/
	void ADC_IRQ_CallBack(void);
	uint32_t API_ADC(ADC_INIT_Type *init);
	float API_Temperature(ADC_INIT_Type *init);

#ifdef __cplusplus
}
#endif

#endif
