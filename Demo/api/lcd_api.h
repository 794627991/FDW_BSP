/*
*********************************************************************************************************
*
*	模块名称 : 段码lcd声明
*	文件名称 : lcd_api.h
*	版    本 : V1.0.0
*	说    明 : 相关声明
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0  2021-1-5    高屹  正式发布
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#ifndef __API_LCD_H__
#define __API_LCD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

/*以下为实际液晶需要映射的对应引脚*/
#ifndef LCD_a
#define LCD_a BIT7
#endif
#ifndef LCD_b
#define LCD_b BIT6
#endif
#ifndef LCD_c
#define LCD_c BIT5
#endif
#ifndef LCD_d
#define LCD_d BIT0
#endif
#ifndef LCD_dp
#define LCD_dp BIT4
#endif
#ifndef LCD_f
#define LCD_f BIT3
#endif
#ifndef LCD_g
#define LCD_g BIT2
#endif
#ifndef LCD_e
#define LCD_e BIT1
#endif

#define LcdOpen 1
#define LcdClose 0

#define COM0_Seg0_31 0
#define COM1_Seg0_31 1
#define COM2_Seg0_31 2
#define COM3_Seg0_31 3
#define COM0_Seg32_43 4
#define COM1_Seg32_43 4
#define COM2_Seg32_39 4
#define COM2_Seg40_43 5
#define COM3_Seg32_43 5

#define SEG0 BIT0
#define SEG1 BIT1
#define SEG2 BIT2
#define SEG3 BIT3
#define SEG4 BIT4
#define SEG5 BIT5
#define SEG6 BIT6
#define SEG7 BIT7
#define SEG8 BIT8
#define SEG9 BIT9
#define SEG10 BIT10
#define SEG11 BIT11
#define SEG12 BIT12
#define SEG13 BIT13
#define SEG14 BIT14
#define SEG15 BIT15
#define SEG16 BIT16
#define SEG17 BIT17
#define SEG18 BIT18
#define SEG19 BIT19
#define SEG20 BIT20
#define SEG21 BIT21
#define SEG22 BIT22
#define SEG23 BIT23
#define SEG24 BIT24
#define SEG25 BIT25
#define SEG26 BIT26
#define SEG27 BIT27
#define SEG28 BIT28
#define SEG29 BIT29
#define SEG30 BIT30
#define SEG31 BIT31

#define COM0_SEG32 BIT0
#define COM0_SEG33 BIT1
#define COM0_SEG34 BIT2
#define COM0_SEG35 BIT3
#define COM0_SEG36 BIT4
#define COM0_SEG37 BIT5
#define COM0_SEG38 BIT6
#define COM0_SEG39 BIT7
#define COM0_SEG40 BIT8
#define COM0_SEG41 BIT9
#define COM0_SEG42 BIT10
#define COM0_SEG43 BIT11

#define COM1_SEG32 BIT12
#define COM1_SEG33 BIT13
#define COM1_SEG34 BIT14
#define COM1_SEG35 BIT15
#define COM1_SEG36 BIT16
#define COM1_SEG37 BIT17
#define COM1_SEG38 BIT18
#define COM1_SEG39 BIT19
#define COM1_SEG40 BIT20
#define COM1_SEG41 BIT21
#define COM1_SEG42 BIT22
#define COM1_SEG43 BIT23

#define COM2_SEG32 BIT24
#define COM2_SEG33 BIT25
#define COM2_SEG34 BIT26
#define COM2_SEG35 BIT27
#define COM2_SEG36 BIT28
#define COM2_SEG37 BIT29
#define COM2_SEG38 BIT30
#define COM2_SEG39 BIT31
#define COM2_SEG40 BIT0
#define COM2_SEG41 BIT1
#define COM2_SEG42 BIT2
#define COM2_SEG43 BIT3

#define COM3_SEG32 BIT4
#define COM3_SEG33 BIT5
#define COM3_SEG34 BIT6
#define COM3_SEG35 BIT7
#define COM3_SEG36 BIT8
#define COM3_SEG37 BIT9
#define COM3_SEG38 BIT10
#define COM3_SEG39 BIT11
#define COM3_SEG40 BIT12
#define COM3_SEG41 BIT13
#define COM3_SEG42 BIT14
#define COM3_SEG43 BIT15

#ifndef L_COM1
#define L_COM1 LCD->DISPDATA0
#endif
#ifndef L_COM2
#define L_COM2 LCD->DISPDATA1
#endif
#ifndef L_COM3
#define L_COM3 LCD->DISPDATA2
#endif
#ifndef L_COM4
#define L_COM4 LCD->DISPDATA3
#endif

/*需要和实际液晶对应对应*/
#ifndef ShowMaxLen
#define ShowMaxLen 8 //不能小于4
#endif

/*需要和原理图的com对应*/
#ifndef COM1
#define COM1 COM0_Seg0_31
#endif
#ifndef COM2
#define COM2 COM1_Seg0_31
#endif
#ifndef COM3
#define COM3 COM2_Seg0_31
#endif
#ifndef COM4
#define COM4 COM3_Seg0_31
#endif

/*需要和液晶真值表的com对应*/
#ifndef L_A
#define L_A COM1
#endif
#ifndef L_B
#define L_B COM2
#endif
#ifndef L_C
#define L_C COM3
#endif
#ifndef L_D
#define L_D COM4
#endif
#ifndef L_E
#define L_E COM3
#endif
#ifndef L_F
#define L_F COM1
#endif
#ifndef L_G
#define L_G COM2
#endif

    typedef struct
    {
        uint8_t control;          /* 1：开,0：关 */
        uint8_t Refreshfrequency; /* 刷新频率 */
        uint8_t Brightness;       /* 灰度：0-15 */
        uint32_t Com0_31Set;
        uint32_t Com32_43Set;
    } LCD_Init_Type;

    extern uint32 DispBuf[6];
    extern const char disptable[];
    extern const char diatable1[];

    /**************************函数指针定义**************************/
    typedef void (*gy_lcdinit)(LCD_Init_Type *init);
    typedef void (*lcd_io_32_43)(void);
    /****************************函数声明****************************/
    void LCD_IO_Init(uint32_t lcd_io_0_31, lcd_io_32_43 hook);
    void Lcd_clear(void);
    void Lcd_All(void);
    void LCD_Display(uint8_t Num, uint8_t Show);
    void API_ShowEvery(char *ledbuf);
    void API_Init_LCD(LCD_Init_Type *init);

#ifdef __cplusplus
}
#endif

#endif
