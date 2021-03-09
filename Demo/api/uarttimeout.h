/*
*********************************************************************************************************
*
*	ģ������ : ���ڳ�ʱ����
*	�ļ����� : system.h
*	��    �� : V1.0.0
*	˵    �� : ���ڳ�ʱ����
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-1-6    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#ifndef __API_UARTTIMEOUT_H__
#define __API_UARTTIMEOUT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

/* ���ڳ�ʱ��ʱ��ѡ������ */
#if Use_uCOS > 0 && Use32BitEtim > 0
#if UseWhichTim == 3
#define UseTim ETIM3
#elif UseWhichTim == 4
#define UseTim ETIM4
#else
#define UseTim ETIM3
#endif
#elif Use_uCOS > 0 && Use32BitEtim == 0
#if UseWhichTim == 1
#define UseTim ETIM1
#elif UseWhichTim == 2
#define UseTim ETIM2
#elif UseWhichTim == 3
#define UseTim ETIM3
#else
#define UseTim ETIM3
#endif
#else
#ifndef UseWhichTim
#define UseWhichTim 3
#endif
#if UseWhichTim == 1
#define UseTim ETIM1
#elif UseWhichTim == 2
#define UseTim ETIM2
#elif UseWhichTim == 3
#define UseTim ETIM3
#elif UseWhichTim == 4
#define UseTim ETIM4
#else
#define UseTim ETIM3
#endif
#endif

#define UartTim0 BIT0
#define UartTim1 BIT1
#define UartTim2 BIT2
#define UartTim3 BIT3
#define UartTim4 BIT4
#define UartTim5 BIT5

        typedef struct
        {
                union
                {
                        uint8_t TimFlag;
                        struct
                        {
                                uint8_t TimFlag0 : 1;
                                uint8_t TimFlag1 : 1;
                                uint8_t TimFlag2 : 1;
                                uint8_t TimFlag3 : 1;
                                uint8_t TimFlag4 : 1;
                                uint8_t TimFlag5 : 1;
                        };
                };
#if TmOut_Uart0 > 0
                uint8_t TimNum0;
#endif
#if TmOut_Uart1 > 0
                uint8_t TimNum1;
#endif
#if TmOut_Uart2 > 0
                uint8_t TimNum2;
#endif
#if TmOut_Uart3 > 0
                uint8_t TimNum3;
#endif
#if TmOut_Uart4 > 0
                uint8_t TimNum4;
#endif
#if TmOut_Uart5 > 0
                uint8_t TimNum5;
#endif
        } TimStruct;

        /**************************����ָ�붨��**************************/

        /****************************��������****************************/
        void API_TimSet(uint8_t xUart, uint8_t num);
        void UartTimeOut(void);
        /****************************�ص�����****************************/
        void Uart0TimCallBack(void);
        void Uart1TimCallBack(void);
        void Uart2TimCallBack(void);
        void Uart3TimCallBack(void);
        void Uart4TimCallBack(void);
        void Uart5TimCallBack(void);

        void UART0RXSUCC(void);
        void UART1RXSUCC(void);
        void UART2RXSUCC(void);
        void UART3RXSUCC(void);
        void UART4RXSUCC(void);
        void UART5RXSUCC(void);

        void API_UART0_DO(uint8_t *buf, uint16_t len);
        void API_UART1_DO(uint8_t *buf, uint16_t len);
        void API_UART2_DO(uint8_t *buf, uint16_t len);
        void API_UART3_DO(uint8_t *buf, uint16_t len);
        void API_UART4_DO(uint8_t *buf, uint16_t len);
        void API_UART5_DO(uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
