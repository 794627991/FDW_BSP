/*
*********************************************************************************************************
*
*	模块名称 : 内存池动态内存分配
*	文件名称 : malloc.c
*	版    本 : V1.0.0
*	说    明 : 动态内存分配，释放，重申和内存池利用状态
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹    正式发布
*		V1.0.1  2021-1-7    高屹    增加malloc保护机制，增加非保护内存池重置机制
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "malloc.h"
#include "stdio.h"

/* 内存池(4字节对齐) */
/*__align(4)*/ u8 membase[MEM_MAX_SIZE]; /* 内部SRAM内存池 */
/* 内存管理表 */
u16 memmapbase[MEM_ALLOC_TABLE_SIZE]; /* 内部SRAM内存池MAP */
/* 内存管理参数 */
const u32 memtblsize = MEM_ALLOC_TABLE_SIZE; /* 内存表大小 */
const u32 memblksize = MEM_BLOCK_SIZE;       /* 内存分块大小 */
const u32 memsize = MEM_MAX_SIZE;            /* 内存总大小 */

/* 内存管理控制器 */
struct _m_mallco_dev mallco_dev = {
    mem_init,    /* 内存初始化 */
    mem_perused, /* 内存使用率 */
    membase,     /* 内存池 */
    memmapbase,  /* 内存管理状态表 */
    0,           /* 内存管理未就绪 */
};
/*
*********************************************************************************************************
*	函 数 名: mymemcpy
*	功能说明: 复制内存
*	形    参: *des:目的地址
*            *src:源地址
*            n:需要复制的内存长度(字节为单位)
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void mymemcpy(void *des, void *src, u32 n)
{
    u8 *xdes = des;
    u8 *xsrc = src;
    while (n--)
        *xdes++ = *xsrc++;
}
/*
*********************************************************************************************************
*	函 数 名: mymemset
*	功能说明: 设置内存
*	形    参: *s:内存首地址
*            c :要设置的值
*            count:需要设置的内存大小(字节为单位)
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void mymemset(void *s, u8 c, u32 count)
{
    u8 *xs = s;
    while (count--)
        *xs++ = c;
}
/*
*********************************************************************************************************
*	函 数 名: mem_init
*	功能说明: 内存管理初始化
*	形    参: memx:所属内存块
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void mem_init(void)
{
    mymemset(mallco_dev.memmap, 0, memtblsize * 2); /* 内存状态表数据清零 */
    mymemset(mallco_dev.membase, 0, memsize);       /* 内存池所有数据清零 */
    mallco_dev.memrdy = 1;                          /* 内存管理初始化OK */
}
/*
*********************************************************************************************************
*	函 数 名: ram_set
*	功能说明: 内存池非保护区清0
*	形    参: 无
*	返 回 值: 无
*	备    注：建议定时调用,一天清一次
*********************************************************************************************************
*/
void ram_set(void)
{
    volatile u32 i = 0;
    for (i = 0; i < memtblsize; i++)
    {
        if (!(mallco_dev.memmap[i] & Malloc_Protect))
        {
            mallco_dev.memmap[i] = 0; /* 非保护区清0 */
            mymemset(&mallco_dev.membase[i * memblksize], 0, memblksize);
        }
    }
}
/*
*********************************************************************************************************
*	函 数 名: mem_perused
*	功能说明: 获取内存使用率
*	形    参: 无
*	返 回 值: 返回值:使用率(0~100)
*	备    注：无
*********************************************************************************************************
*/
u8 mem_perused(void)
{
    u32 used = 0;
    u32 i;
    for (i = 0; i < memtblsize; i++)
    {
        if (mallco_dev.memmap[i])
            used++;
    }
    return (used * 100) / (memtblsize);
}
/*
*********************************************************************************************************
*	函 数 名: mem_malloc
*	功能说明: 内存分配(内部调用)
*	形    参: size：要申请的大小
*	返 回 值: 返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址
*	备    注：无
*********************************************************************************************************
*/
u32 mem_malloc(u32 size, u8 needprotect)
{
    signed long offset = 0;
    u16 nmemb = 0; /* 需要的内存块数 */
    u16 cmemb = 0; /* 连续空内存块数 */
    volatile u32 i = 0;
    if (!mallco_dev.memrdy)
        mallco_dev.init(); /* 未初始化,先执行初始化 */
    if (size == 0)
        return 0XFFFFFFFF; /* 不需要分配 */
    disable_irq;
    nmemb = size / MEM_BLOCK_SIZE; /* 获取需要分配的连续内存块数 */
    if (size % memblksize)
        nmemb++;

    if (memtblsize > nmemb)
    {
        offset = memtblsize - nmemb;
        for (i = 0; i + cmemb < nmemb;)
        {
            if (mallco_dev.memmap[offset + i]) /* 内存块不空 */
            {
                if (offset + i >= nmemb) /* 即offset + i一定大于等于一 */
                {
                    if (mallco_dev.memmap[offset + i - 1] == mallco_dev.memmap[offset + i])
                    {
                        /* 说明落点在连续已被占有内存块内 */
                        if (offset + i > (mallco_dev.memmap[offset + i] & Malloc_ReProtect))
                            offset = offset + i - (mallco_dev.memmap[offset + i] & Malloc_ReProtect);
                        else
                            break;
                    }
                    else
                    {
                        offset = offset + i - nmemb;
                    }
                    cmemb = i;
                    i = 0;
                }
                else
                    break; /* 剩余大小已经不够分配了 */
            }
            else
                i++;
        }

        if ((cmemb + i) == nmemb)
        {
            for (i = 0; i < nmemb; i++) /* 标注内存块非空 */
            {
                if (needprotect)
                    mallco_dev.memmap[offset + i] = (nmemb | Malloc_Protect);
                else
                {
                    mallco_dev.memmap[offset + i] = nmemb;
                }
            }
            enable_irq;
            return (offset * memblksize); /* 返回偏移地址 */
        }
    }

    enable_irq;
    return 0XFFFFFFFF; /* 未找到符合分配条件的内存块 */
}
/*
*********************************************************************************************************
*	函 数 名: mem_free
*	功能说明: 释放内存(内部调用)
*	形    参: offset:内存地址偏移
*	返 回 值: 返回值:0,释放成功;1,释放失败;
*	备    注：无
*********************************************************************************************************
*/
u8 mem_free(u32 offset)
{
    volatile int i;
    if (!mallco_dev.memrdy) /* 未初始化,先执行初始化 */
    {
        mallco_dev.init();
        return 1; /* 未初始化 */
    }
    if (offset < memsize) /* 偏移在内存池内 */
    {
        int index = offset / MEM_BLOCK_SIZE;                       /* 偏移所在内存块号码 */
        int nmemb = (mallco_dev.memmap[index] & Malloc_ReProtect); /* 内存块数量 */
        for (i = 0; i < nmemb; i++)                                /* 内存块清零 */
        {
            mallco_dev.memmap[index + i] = 0;
        }
        return 0;
    }
    else
        return 2; /* 偏移超区了 */
}
/*
*********************************************************************************************************
*	函 数 名: __myfree
*	功能说明: 释放内存(外部调用)
*	形    参: ptr:内存首地址
*	返 回 值: 无
*	备    注：无
*********************************************************************************************************
*/
void __myfree(void *ptr)
{
    u32 offset;
    if (ptr == 0)
        return;
    offset = (u32)((u8 *)ptr - mallco_dev.membase);
    mem_free(offset); /* 释放内存 */
    printf("memuse:%d\r\n", mem_perused());
}
/*
*********************************************************************************************************
*	函 数 名: __mymalloc
*	功能说明: 分配内存(外部调用)
*	形    参: size:内存大小(字节)
*	返 回 值: 分配到的内存首地址.
*	备    注：调用后务必记得free掉
*********************************************************************************************************
*/
void *__mymalloc(u32 size)
{
    u32 offset = mem_malloc(size, 0);
    printf("memuse:%d\r\n", mem_perused());
    if (offset == 0XFFFFFFFF)
        return 0;
    else
        return (void *)(mallco_dev.membase + offset);
}
/*
*********************************************************************************************************
*	函 数 名: __mymallocpro
*	功能说明: 分配内存(外部调用)
*	形    参: size:内存大小(字节)
*	返 回 值: 分配到的内存首地址.
*	备    注：调用后务必记得free掉
*********************************************************************************************************
*/
void *__mymallocpro(u32 size)
{
    u32 offset = mem_malloc(size, 1);
    if (offset == 0XFFFFFFFF)
        return 0;
    else
        return (void *)(mallco_dev.membase + offset);
}
/*
*********************************************************************************************************
*	函 数 名: myrealloc
*	功能说明: 重新分配内存(外部调用)
*	形    参: *ptr:旧内存首地址
*             size:要分配的内存大小(字节)
*	返 回 值: 新分配到的内存首地址.
*	备    注：无
*********************************************************************************************************
*/
void *__myrealloc(void *ptr, u32 size, u8 needprotect)
{
    u32 offset;
    offset = mem_malloc(size, needprotect);
    if (offset == 0XFFFFFFFF)
        return 0;
    else
    {
        mymemcpy((void *)(mallco_dev.membase + offset), ptr, size); /* 拷贝旧内存内容到新内存 */
        myfree(ptr);                                                /* 释放旧内存 */
        return (void *)(mallco_dev.membase + offset);               /* 返回新内存首地址 */
    }
}
