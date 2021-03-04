/* Includes ------------------------------------------------------------------*/
#ifndef __HT16C23_H__
#define __HT16C23_H__

#include "api_define_all.h"

#define LCDSDA_GPIO_PORT GPIOA
#define LCDSDA_GPIO_PIN GPIO_Pin_15

#define LCDSCL_GPIO_PORT GPIOA
#define LCDSCL_GPIO_PIN GPIO_Pin_14

#define TT 0x24
#define W_DEVICE_ADR_LCD 0x7c
#define SDA_LCD_OUT OutputIO(LCDSDA_GPIO_PORT, LCDSDA_GPIO_PIN, OUT_PUSHPULL);
#define SDA_LCD_IN InputtIO(LCDSDA_GPIO_PORT, LCDSDA_GPIO_PIN, IN_NORMAL)
#define SDA_LCD_1 GPIO_SetBits(LCDSDA_GPIO_PORT, LCDSDA_GPIO_PIN)
#define SDA_LCD_0 GPIO_ResetBits(LCDSDA_GPIO_PORT, LCDSDA_GPIO_PIN)
#define READ_LCD_SDA GPIO_ReadOutputDataBit(LCDSDA_GPIO_PORT, LCDSDA_GPIO_PIN)

#define SCL_LCD_OUT OutputIO(LCDSCL_GPIO_PORT, LCDSDA_GPIO_PIN, OUT_PUSHPULL);
#define SCL_LCD_1 GPIO_SetBits(LCDSCL_GPIO_PORT, LCDSCL_GPIO_PIN)
#define SCL_LCD_0 GPIO_ResetBits(LCDSCL_GPIO_PORT, LCDSCL_GPIO_PIN)
uint8_t I2cSendCommand(uint8_t com, uint8_t com1);
uint8_t I2cSendData(uint8_t WordAddress, uint8_t WordAddress1, uint8_t *s, uint8_t length);
void LCD_Init_HT16C23(void);
void LCD_SHOW_HT16C23(void);

#endif
