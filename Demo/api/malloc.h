/*
*********************************************************************************************************
*
*	模块名称 : 内存池动态内存分配定义
*	文件名称 : lptim_api.h
*	版    本 : V1.0.0
*	说    明 : 声明和定义
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹    正式发布
*		V1.0.1  2021-1-7    高屹    增加malloc保护机制的宏定义
*                                   修改myfree的临界条件，改为全程临界，以避免内存未清
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __MALLOC_H
#define __MALLOC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "isthisboot.h"

#define MEM_BLOCK_SIZE 32								   /* 内存块大小为32字节 */
#if ThisBoot ==0
#define MEM_MAX_SIZE 9 * 1024							   /* 最大管理内存 nK */
#else
#define MEM_MAX_SIZE 15 * 1024
#endif
#define MEM_ALLOC_TABLE_SIZE MEM_MAX_SIZE / MEM_BLOCK_SIZE /* 内存表大小 */

#if defined(__CC_ARM)
#define disable_irq __disable_irq()
#define enable_irq __enable_irq()
#else
#define disable_irq
#define enable_irq
#endif

#define Protect 0x8000		  /* 对内存池map加保护，在set时判断非 Protect 值的，清0 */
#define ReProtect Protect - 1 /* 该大小需要和 memmap 的数据类型同步，这里memmap类型是u16 */

	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned int u32;

	//内存管理控制器
	struct _m_mallco_dev
	{
		void (*init)(void);	 /* 初始化 */
		u8 (*perused)(void); /* 内存使用率 */
		u8 *membase;		 /* 内存池 */
		u16 *memmap;		 /* 内存管理状态表 */
		u8 memrdy;			 /* 内存管理是否就绪 */
	};

	extern struct _m_mallco_dev mallco_dev; /* 在mallco.c里面定义 */

	void mymemset(void *s, u8 c, u32 count);	/* 设置内存 */
	void mymemcpy(void *des, void *src, u32 n); /* 复制内存 */
	void mem_init(void);						/* 内存管理初始化函数 */
	u32 mem_malloc(u32 size, u8 needprotect);	/* 内存分配 */
	u8 mem_free(u32 offset);					/* 内存释放 */
	u8 mem_perused(void);						/* 获得内存使用率 */

	void __myfree(void *ptr);
#define myfree(x)    \
	{                \
		disable_irq; \
		__myfree(x); \
		x = 0;       \
		enable_irq;  \
	}
	void *__mymalloc(u32 size);
	void *__mymallocpro(u32 size);
#define mymalloc(x) __mymalloc(x)
#define mymallocpro(x) __mymallocpro(x)
	void ram_set(void);
	void *__myrealloc(void *ptr, u32 size, u8 needprotect);
#define myrealloc(ptr, size) __myrealloc(ptr, size, 0)
#define myreallocpro(ptr, size) __myrealloc(ptr, size, 1)

#ifdef __cplusplus
}
#endif

#endif
