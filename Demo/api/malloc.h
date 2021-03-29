/*
*********************************************************************************************************
*
*	ģ������ : �ڴ�ض�̬�ڴ���䶨��
*	�ļ����� : lptim_api.h
*	��    �� : V1.0.0
*	˵    �� : �����Ͷ���
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����    ��ʽ����
*		V1.0.1  2021-1-7    ����    ����malloc�������Ƶĺ궨��
*                                   �޸�myfree���ٽ���������Ϊȫ���ٽ磬�Ա����ڴ�δ��
*
*	Copyright (C), 2020-2030,  ����˼��-����
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

#define MEM_BLOCK_SIZE 32								   /* �ڴ���СΪ32�ֽ� */
#if ThisBoot ==0
#define MEM_MAX_SIZE 9 * 1024							   /* �������ڴ� nK */
#else
#define MEM_MAX_SIZE 15 * 1024
#endif
#define MEM_ALLOC_TABLE_SIZE MEM_MAX_SIZE / MEM_BLOCK_SIZE /* �ڴ���С */

#if defined(__CC_ARM)
#define disable_irq __disable_irq()
#define enable_irq __enable_irq()
#else
#define disable_irq
#define enable_irq
#endif

#define Protect 0x8000		  /* ���ڴ��map�ӱ�������setʱ�жϷ� Protect ֵ�ģ���0 */
#define ReProtect Protect - 1 /* �ô�С��Ҫ�� memmap ����������ͬ��������memmap������u16 */

	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned int u32;

	//�ڴ���������
	struct _m_mallco_dev
	{
		void (*init)(void);	 /* ��ʼ�� */
		u8 (*perused)(void); /* �ڴ�ʹ���� */
		u8 *membase;		 /* �ڴ�� */
		u16 *memmap;		 /* �ڴ����״̬�� */
		u8 memrdy;			 /* �ڴ�����Ƿ���� */
	};

	extern struct _m_mallco_dev mallco_dev; /* ��mallco.c���涨�� */

	void mymemset(void *s, u8 c, u32 count);	/* �����ڴ� */
	void mymemcpy(void *des, void *src, u32 n); /* �����ڴ� */
	void mem_init(void);						/* �ڴ�����ʼ������ */
	u32 mem_malloc(u32 size, u8 needprotect);	/* �ڴ���� */
	u8 mem_free(u32 offset);					/* �ڴ��ͷ� */
	u8 mem_perused(void);						/* ����ڴ�ʹ���� */

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
