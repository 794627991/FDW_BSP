/*
*********************************************************************************************************
*
*	模块名称 : 事件处理机制声明
*	文件名称 : useevent.h
*	版    本 : V1.0.0
*	说    明 : 事件处理机制声明
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2020-1-6    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
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

    /**************************函数指针定义**************************/
    typedef void (*Event)(void);

    /****************************函数声明****************************/
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
