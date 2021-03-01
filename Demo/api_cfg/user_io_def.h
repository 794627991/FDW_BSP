#ifndef __USER_IO_DEF_H__
#define __USER_IO_DEF_H__

/*功能IO宏定义*/

#define LED1_GPIO GPIOC
#define LED1_PIN GPIO_Pin_12
#define LED1OFF GPIO_SetBits(LED1_GPIO, LED1_PIN)
#define LED1ON GPIO_ResetBits(LED1_GPIO, LED1_PIN)
#define LED1CPL GPIO_ToggleBits(LED1_GPIO, LED1_PIN)

#define LED2_GPIO GPIOC
#define LED2_PIN GPIO_Pin_13
#define LED2OFF GPIO_SetBits(LED2_GPIO, LED2_PIN)
#define LED2ON GPIO_ResetBits(LED2_GPIO, LED2_PIN)
#define LED2CPL GPIO_ToggleBits(LED2_GPIO, LED2_PIN)

#define LED3_GPIO GPIOC
#define LED3_PIN GPIO_Pin_14
#define LED3OFF GPIO_SetBits(LED3_GPIO, LED3_PIN)
#define LED3ON GPIO_ResetBits(LED3_GPIO, LED3_PIN)
#define LED3CPL GPIO_ToggleBits(LED3_GPIO, LED3_PIN)

#define LED4_GPIO GPIOD
#define LED4_PIN GPIO_Pin_7
#define LED4OFF GPIO_SetBits(LED4_GPIO, LED4_PIN)
#define LED4ON GPIO_ResetBits(LED4_GPIO, LED4_PIN)
#define LED4CPL GPIO_ToggleBits(LED4_GPIO, LED4_PIN)

#define KEY1_GPIO GPIOC
#define KEY1_PIN GPIO_Pin_15
#define ReadKey1 GPIO_ReadInputDataBit(KEY1_GPIO, KEY1_PIN)

#define KEY2_GPIO GPIOF
#define KEY2_PIN GPIO_Pin_5
#define ReadKey2 GPIO_ReadInputDataBit(KEY2_GPIO, KEY2_PIN)

#define KEY3_GPIO GPIOA
#define KEY3_PIN GPIO_Pin_13
#define ReadKey3 GPIO_ReadInputDataBit(KEY3_GPIO, KEY3_PIN)

#define NB_EN_GPIO GPIOB
#define NB_EN_PIN GPIO_Pin_1
#define NB_ON GPIO_SetBits(NB_EN_GPIO, NB_EN_PIN)
#define NB_OFF GPIO_ResetBits(NB_EN_GPIO, NB_EN_PIN)
#define NB_CPL GPIO_ToggleBits(NB_EN_GPIO, NB_EN_PIN)

#define PWM_GPIO GPIOG
#define PWM_PIN GPIO_Pin_7
#define PWM_DIR OutputIO(PWM_GPIO, PWM_PIN, 0)
#define PWM_CPL GPIO_ToggleBits(PWM_GPIO, PWM_PIN)

/*IIC IO SET*/
#define IIC_EN_GPIO GPIOB
#define IIC_EN_PIN GPIO_Pin_12
#define POWER_2402_DIR OutputIO(IIC_EN_GPIO, IIC_EN_PIN, 0)
#define POWER_2402_ON GPIO_ResetBits(IIC_EN_GPIO, IIC_EN_PIN) //24WC02供电
#define POWER_2402_OFF GPIO_SetBits(IIC_EN_GPIO, IIC_EN_PIN)  //24WC02断电

#define MONI_IIC_SDA_GPIO GPIOA
#define MONI_IIC_SDA_PIN GPIO_Pin_1
#define MONI_IIC_SDA_DIR_OUT OutputIO(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN, 0)
#define MONI_IIC_SDA_DIR_IN InputtIO(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN, 1)
#define MONI_IIC_SDA_0 GPIO_ResetBits(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN)
#define MONI_IIC_SDA_1 GPIO_SetBits(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN)
#define MONI_IIC_SDA_READ GPIO_ReadInputDataBit(MONI_IIC_SDA_GPIO, MONI_IIC_SDA_PIN)

#define MONI_IIC_SCL_GPIO GPIOA
#define MONI_IIC_SCL_PIN GPIO_Pin_3
#define MONI_IIC_SCL_DIR_OUT OutputIO(MONI_IIC_SCL_GPIO, MONI_IIC_SCL_PIN, 0)
#define MONI_IIC_SCL_0 GPIO_ResetBits(MONI_IIC_SCL_GPIO, MONI_IIC_SCL_PIN)
#define MONI_IIC_SCL_1 GPIO_SetBits(MONI_IIC_SCL_GPIO, MONI_IIC_SCL_PIN)

#define MONI_IIC_SDA_GPIO1 GPIOA
#define MONI_IIC_SDA_PIN1 GPIO_Pin_15
#define MONI_IIC_SDA_DIR_OUT1 OutputIO(MONI_IIC_SDA_GPIO1, MONI_IIC_SDA_PIN1, 1)
#define MONI_IIC_SDA_DIR_IN1 InputtIO(MONI_IIC_SDA_GPIO1, MONI_IIC_SDA_PIN1, 0)
#define MONI_IIC_SDA_01 GPIO_ResetBits(MONI_IIC_SDA_GPIO1, MONI_IIC_SDA_PIN1)
#define MONI_IIC_SDA_11 GPIO_SetBits(MONI_IIC_SDA_GPIO1, MONI_IIC_SDA_PIN1)
#define MONI_IIC_SDA_READ1 GPIO_ReadInputDataBit(MONI_IIC_SDA_GPIO1, MONI_IIC_SDA_PIN1)

#define MONI_IIC_SCL_GPIO1 GPIOA
#define MONI_IIC_SCL_PIN1 GPIO_Pin_14
#define MONI_IIC_SCL_DIR_OUT1 OutputIO(MONI_IIC_SCL_GPIO1, MONI_IIC_SCL_PIN1, 1)
#define MONI_IIC_SCL_01 GPIO_ResetBits(MONI_IIC_SCL_GPIO1, MONI_IIC_SCL_PIN1)
#define MONI_IIC_SCL_11 GPIO_SetBits(MONI_IIC_SCL_GPIO1, MONI_IIC_SCL_PIN1)

/*SPI IO SET*/
#define MONI_SPI_NSS_GPIO GPIOF
#define MONI_SPI_NSS_PIN GPIO_Pin_14

#define MONI_SPI_SCK_GPIO GPIOF
#define MONI_SPI_SCK_PIN GPIO_Pin_12

#define MONI_SPI_MOSI_GPIO GPIOE
#define MONI_SPI_MOSI_PIN GPIO_Pin_3

#define MONI_SPI_MISO_GPIO GPIOB
#define MONI_SPI_MISO_PIN GPIO_Pin_15

#endif
