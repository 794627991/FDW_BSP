/*
*********************************************************************************************************
*
*	ģ������ : �¼������������
*	�ļ����� : useevent.h
*	��    �� : V1.0.0
*	˵    �� : �¼������������
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2020-1-6    ����  ��ʽ����
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#ifndef __API_EVENT_H__
#define __API_EVENT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    /**************************����ָ�붨��**************************/
    typedef void (*Event)(void);

    /****************************��������****************************/
    void SendMessage(Event SendEvent);
    void DispatchMessage(void);
    uint8_t GetQempty(void);

#define ExecuteEvent         \
    while (GetQempty() != 1) \
    {                        \
        DispatchMessage();   \
    }

#ifdef __cplusplus
}
#endif

#endif
