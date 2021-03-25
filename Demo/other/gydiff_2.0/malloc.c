/*
*********************************************************************************************************
*
*	ģ������ : �ڴ�ض�̬�ڴ����
*	�ļ����� : malloc.c
*	��    �� : V1.0.0
*	˵    �� : ��̬�ڴ���䣬�ͷţ�������ڴ������״̬
*	�޸ļ�¼ :
*
*		�汾��     ����      ����    ˵��
*		V1.0.0  2021-1-5    ����    ��ʽ����
*		V1.0.1  2021-1-7    ����    ����malloc�������ƣ����ӷǱ����ڴ�����û���
*
*	Copyright (C), 2020-2030,  ����˼��-����
*
*********************************************************************************************************
*/
#include "malloc.h"
#include "stdio.h"

/* �ڴ��(4�ֽڶ���) */
/*__align(4)*/ u8 membase[MEM_MAX_SIZE]; /* �ڲ�SRAM�ڴ�� */
/* �ڴ����� */
u16 memmapbase[MEM_ALLOC_TABLE_SIZE]; /* �ڲ�SRAM�ڴ��MAP */
/* �ڴ������� */
const u32 memtblsize = MEM_ALLOC_TABLE_SIZE; /* �ڴ���С */
const u32 memblksize = MEM_BLOCK_SIZE;       /* �ڴ�ֿ��С */
const u32 memsize = MEM_MAX_SIZE;            /* �ڴ��ܴ�С */

/* �ڴ��������� */
struct _m_mallco_dev mallco_dev = {
    mem_init,    /* �ڴ��ʼ�� */
    mem_perused, /* �ڴ�ʹ���� */
    membase,     /* �ڴ�� */
    memmapbase,  /* �ڴ����״̬�� */
    0,           /* �ڴ����δ���� */
};
/*
*********************************************************************************************************
*	�� �� ��: mymemcpy
*	����˵��: �����ڴ�
*	��    ��: *des:Ŀ�ĵ�ַ
*            *src:Դ��ַ
*            n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
*	�� �� ֵ: ��
*	��    ע����
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
*	�� �� ��: mymemset
*	����˵��: �����ڴ�
*	��    ��: *s:�ڴ��׵�ַ
*            c :Ҫ���õ�ֵ
*            count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
*	�� �� ֵ: ��
*	��    ע����
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
*	�� �� ��: mem_init
*	����˵��: �ڴ�����ʼ��
*	��    ��: memx:�����ڴ��
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void mem_init(void)
{
    mymemset(mallco_dev.memmap, 0, memtblsize * 2); /* �ڴ�״̬���������� */
    mymemset(mallco_dev.membase, 0, memsize);       /* �ڴ�������������� */
    mallco_dev.memrdy = 1;                          /* �ڴ�����ʼ��OK */
}
/*
*********************************************************************************************************
*	�� �� ��: ram_set
*	����˵��: �ڴ�طǱ�������0
*	��    ��: ��
*	�� �� ֵ: ��
*	��    ע�����鶨ʱ����,һ����һ��
*********************************************************************************************************
*/
void ram_set(void)
{
    volatile u32 i = 0;
    for (i = 0; i < memtblsize; i++)
    {
        if (!(mallco_dev.memmap[i] & Malloc_Protect))
        {
            mallco_dev.memmap[i] = 0; /* �Ǳ�������0 */
            mymemset(&mallco_dev.membase[i * memblksize], 0, memblksize);
        }
    }
}
/*
*********************************************************************************************************
*	�� �� ��: mem_perused
*	����˵��: ��ȡ�ڴ�ʹ����
*	��    ��: ��
*	�� �� ֵ: ����ֵ:ʹ����(0~100)
*	��    ע����
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
*	�� �� ��: mem_malloc
*	����˵��: �ڴ����(�ڲ�����)
*	��    ��: size��Ҫ����Ĵ�С
*	�� �� ֵ: ����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ
*	��    ע����
*********************************************************************************************************
*/
u32 mem_malloc(u32 size, u8 needprotect)
{
    signed long offset = 0;
    u16 nmemb = 0; /* ��Ҫ���ڴ���� */
    u16 cmemb = 0; /* �������ڴ���� */
    volatile u32 i = 0;
    if (!mallco_dev.memrdy)
        mallco_dev.init(); /* δ��ʼ��,��ִ�г�ʼ�� */
    if (size == 0)
        return 0XFFFFFFFF; /* ����Ҫ���� */
    disable_irq;
    nmemb = size / MEM_BLOCK_SIZE; /* ��ȡ��Ҫ����������ڴ���� */
    if (size % memblksize)
        nmemb++;

    if (memtblsize > nmemb)
    {
        offset = memtblsize - nmemb;
        for (i = 0; i + cmemb < nmemb;)
        {
            if (mallco_dev.memmap[offset + i]) /* �ڴ�鲻�� */
            {
                if (offset + i >= nmemb) /* ��offset + iһ�����ڵ���һ */
                {
                    if (mallco_dev.memmap[offset + i - 1] == mallco_dev.memmap[offset + i])
                    {
                        /* ˵������������ѱ�ռ���ڴ���� */
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
                    break; /* ʣ���С�Ѿ����������� */
            }
            else
                i++;
        }

        if ((cmemb + i) == nmemb)
        {
            for (i = 0; i < nmemb; i++) /* ��ע�ڴ��ǿ� */
            {
                if (needprotect)
                    mallco_dev.memmap[offset + i] = (nmemb | Malloc_Protect);
                else
                {
                    mallco_dev.memmap[offset + i] = nmemb;
                }
            }
            enable_irq;
            return (offset * memblksize); /* ����ƫ�Ƶ�ַ */
        }
    }

    enable_irq;
    return 0XFFFFFFFF; /* δ�ҵ����Ϸ����������ڴ�� */
}
/*
*********************************************************************************************************
*	�� �� ��: mem_free
*	����˵��: �ͷ��ڴ�(�ڲ�����)
*	��    ��: offset:�ڴ��ַƫ��
*	�� �� ֵ: ����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;
*	��    ע����
*********************************************************************************************************
*/
u8 mem_free(u32 offset)
{
    volatile int i;
    if (!mallco_dev.memrdy) /* δ��ʼ��,��ִ�г�ʼ�� */
    {
        mallco_dev.init();
        return 1; /* δ��ʼ�� */
    }
    if (offset < memsize) /* ƫ�����ڴ���� */
    {
        int index = offset / MEM_BLOCK_SIZE;                       /* ƫ�������ڴ����� */
        int nmemb = (mallco_dev.memmap[index] & Malloc_ReProtect); /* �ڴ������ */
        for (i = 0; i < nmemb; i++)                                /* �ڴ������ */
        {
            mallco_dev.memmap[index + i] = 0;
        }
        return 0;
    }
    else
        return 2; /* ƫ�Ƴ����� */
}
/*
*********************************************************************************************************
*	�� �� ��: __myfree
*	����˵��: �ͷ��ڴ�(�ⲿ����)
*	��    ��: ptr:�ڴ��׵�ַ
*	�� �� ֵ: ��
*	��    ע����
*********************************************************************************************************
*/
void __myfree(void *ptr)
{
    u32 offset;
    if (ptr == 0)
        return;
    offset = (u32)((u8 *)ptr - mallco_dev.membase);
    mem_free(offset); /* �ͷ��ڴ� */
    printf("memuse:%d\r\n", mem_perused());
}
/*
*********************************************************************************************************
*	�� �� ��: __mymalloc
*	����˵��: �����ڴ�(�ⲿ����)
*	��    ��: size:�ڴ��С(�ֽ�)
*	�� �� ֵ: ���䵽���ڴ��׵�ַ.
*	��    ע�����ú���ؼǵ�free��
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
*	�� �� ��: __mymallocpro
*	����˵��: �����ڴ�(�ⲿ����)
*	��    ��: size:�ڴ��С(�ֽ�)
*	�� �� ֵ: ���䵽���ڴ��׵�ַ.
*	��    ע�����ú���ؼǵ�free��
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
*	�� �� ��: myrealloc
*	����˵��: ���·����ڴ�(�ⲿ����)
*	��    ��: *ptr:���ڴ��׵�ַ
*             size:Ҫ������ڴ��С(�ֽ�)
*	�� �� ֵ: �·��䵽���ڴ��׵�ַ.
*	��    ע����
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
        mymemcpy((void *)(mallco_dev.membase + offset), ptr, size); /* �������ڴ����ݵ����ڴ� */
        myfree(ptr);                                                /* �ͷž��ڴ� */
        return (void *)(mallco_dev.membase + offset);               /* �������ڴ��׵�ַ */
    }
}
