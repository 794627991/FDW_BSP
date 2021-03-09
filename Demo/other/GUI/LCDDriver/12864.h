#ifndef _LCD_12864_H__
#define _LCD_12864_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

#define LCD_12864_CS_GPIO GPIOG
#define LCD_12864_CS_PIN GPIO_Pin_2
#define LCD_12864_CS_W OutputIO(LCD_12864_CS_GPIO, LCD_12864_CS_PIN, 0)
#define LCD_12864_CS_1 GPIO_SetBits(LCD_12864_CS_GPIO, LCD_12864_CS_PIN)
#define LCD_12864_CS_0 GPIO_ResetBits(LCD_12864_CS_GPIO, LCD_12864_CS_PIN)

#define LCD_12864_CLK_GPIO GPIOD
#define LCD_12864_CLK_PIN GPIO_Pin_0
#define LCD_12864_CLK_W OutputIO(LCD_12864_CLK_GPIO, LCD_12864_CLK_PIN, 0)
#define LCD_12864_CLK_1 GPIO_SetBits(LCD_12864_CLK_GPIO, LCD_12864_CLK_PIN)
#define LCD_12864_CLK_0 GPIO_ResetBits(LCD_12864_CLK_GPIO, LCD_12864_CLK_PIN)

#define LCD_12864_SID_GPIO GPIOD
#define LCD_12864_SID_PIN GPIO_Pin_6
#define LCD_12864_SID_W OutputIO(LCD_12864_SID_GPIO, LCD_12864_SID_PIN, 0)
#define LCD_12864_SID_R InputtIO(LCD_12864_SID_GPIO, LCD_12864_SID_PIN, 0)
#define LCD_12864_SID_IN GPIO_ReadInputDataBit(LCD_12864_SID_GPIO, LCD_12864_SID_PIN)
#define LCD_12864_SID_1 GPIO_SetBits(LCD_12864_SID_GPIO, LCD_12864_SID_PIN)
#define LCD_12864_SID_0 GPIO_ResetBits(LCD_12864_SID_GPIO, LCD_12864_SID_PIN)

    void LCD_12864_Init(void);
		void Send(uchar type, uchar transdata);
    void lcd_show(const unsigned char *ptr);

#ifdef __cplusplus
}
#endif

#endif
