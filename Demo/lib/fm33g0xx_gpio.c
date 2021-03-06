/**
  ******************************************************************************
  * @file    FM33g0xx_gpio.c
  * @author  FM33g0xx Application Team
  * @version V0.3.00G
  * @date    08-31-2018
  * @brief   This file provides firmware functions to manage the following 
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33g0xx_gpio.h"
#include "fm33g0xx_conf.h"
/** @addtogroup fm33g0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup GPIO 
  * @brief GPIO driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/********************************
 PortX输出数据寄存器配置函数
功能: 配置PortX输出数据
输入：GPIOx_Type* GPIOx（管脚号）,  uint32_t SetValue（输出值）
输出:无
********************************/
void GPIOx_DO_Write(GPIOx_Type *GPIOx, uint32_t SetValue)
{
	GPIOx->DO = (SetValue & GPIOx_DO_DO_Msk);
}
/********************************
读PortX输出数据状态函数
功能:读PortX输出数据状态
输入：无
输出:PortX输出值
********************************/
uint32_t GPIOx_DO_Read(GPIOx_Type *GPIOx)
{
	return (GPIOx->DO & GPIOx_DO_DO_Msk);
}

/********************************
PortX输出数据置位寄存器函数
功能:PortX输出数据置位，如向PADSET写0x0000_8000，则PADO[15]置位，其余位保持不变。
输入：GPIOx_Type* GPIOx（管脚号）,  uint32_t SetValue（输出值）
输出:无
********************************/
void GPIOx_DSET_Write(GPIOx_Type *GPIOx, uint32_t SetValue)
{
	GPIOx->DSET = (SetValue & GPIOx_DSET_DSET_Msk);
}

/********************************
PortX输出数据复位寄存器函数
功能:PortX输出数据复位，如向PADRST写0x0000_8000，则PADO[15]清零，其余位保持不变。
输入：GPIOx_Type* GPIOx（管脚号）,  uint32_t SetValue（输出值）
输出:无
********************************/
void GPIOx_DRESET_Write(GPIOx_Type *GPIOx, uint32_t SetValue)
{
	GPIOx->DRESET = (SetValue & GPIOx_DRESET_DRESET_Msk);
}

/********************************
PortX输入数据寄存器函数
功能:PortX输入数据，此寄存器仅占用地址空间，无物理实现。软件读此寄存器直接返回引脚输入信号，芯片并不对引脚输入进行锁存
输入：GPIOx_Type* GPIOx（管脚号）,  uint32_t SetValue（输入值）
输出:无
********************************/
uint32_t GPIOx_DIN_Read(GPIOx_Type *GPIOx)
{
	return (GPIOx->DIN & GPIOx_DIN_DIN_Msk);
}

/********************************
 FOUT输出配置函数
功能: 配置FOUT输出
输入：输出频率选择信号
输出:无
********************************/
void GPIO_FOUTSEL_FOUTSEL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->FOUTSEL;
	tmpreg &= ~(GPIO_FOUTSEL_FOUTSEL_Msk);
	tmpreg |= (SetValue & GPIO_FOUTSEL_FOUTSEL_Msk);
	GPIO->FOUTSEL = tmpreg;
}
/********************************
读FOUT输出状态函数
功能:读FOUT输出状态
输入：无
输出:FOUT输出选择值
********************************/
uint32_t GPIO_FOUTSEL_FOUTSEL_Get(void)
{
	return (GPIO->FOUTSEL & GPIO_FOUTSEL_FOUTSEL_Msk);
}

/********************************
 PG6强驱动使能函数
功能: 配置PG6强驱动
输入：ENABLE  使能强驱动模式
      DISABLE 关闭强驱动模式
输出:无
********************************/
void GPIO_HDSEL_PG6HDEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		GPIO->HDSEL |= (GPIO_HDSEL_PG6HDEN_Msk);
	}
	else
	{
		GPIO->HDSEL &= ~(GPIO_HDSEL_PG6HDEN_Msk);
	}
}
/********************************
读PG6强驱动状态函数
功能:读PG6强驱动状态
输入：无
输出: ENABLE  强驱动模式已使能
      DISABLE 强驱动模式已关闭
********************************/
FunState GPIO_HDSEL_PG6HDEN_Getable(void)
{
	if (GPIO->HDSEL & (GPIO_HDSEL_PG6HDEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
 PE2强驱动使能函数
功能: 配置PE2强驱动
输入：ENABLE  使能强驱动模式
      DISABLE 关闭强驱动模式
输出:无
********************************/
void GPIO_HDSEL_PE2HDEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		GPIO->HDSEL |= (GPIO_HDSEL_PE2HDEN_Msk);
	}
	else
	{
		GPIO->HDSEL &= ~(GPIO_HDSEL_PE2HDEN_Msk);
	}
}

/********************************
读PE2强驱动状态函数
功能:读PE2强驱动状态
输入：无
输出: ENABLE  强驱动模式已使能
      DISABLE 强驱动模式已关闭
********************************/
FunState GPIO_HDSEL_PE2HDEN_Getable(void)
{
	if (GPIO->HDSEL & (GPIO_HDSEL_PE2HDEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
 PE4模拟功能选择函数
功能: 配置PE4模拟功能选择
输入：GPIO_ANASEL_PE4ANS_SEG19：选择作为SEG19
      GPIO_ANASEL_PE4ANS_ACMP1_INP1：选择作为ACMP1_INP1
输出:无
********************************/
void GPIO_ANASEL_PE4ANS_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->ANASEL;
	tmpreg &= ~(GPIO_ANASEL_PE4ANS_Msk);
	tmpreg |= (SetValue & GPIO_ANASEL_PE4ANS_Msk);
	GPIO->ANASEL = tmpreg;
}
/********************************
读PE4模拟功能选择状态函数
功能:读PE4模拟功能选择状态
输入：无
输出: GPIO_ANASEL_PE4ANS_SEG19：选择SEG19
      GPIO_ANASEL_PE4ANS_ACMP1_INP1：选择ACMP1_INP1
********************************/
uint32_t GPIO_ANASEL_PE4ANS_Get(void)
{
	return (GPIO->ANASEL & GPIO_ANASEL_PE4ANS_Msk);
}

/********************************
 PE3模拟功能选择函数
功能: 配置PE3模拟功能选择
输入：GPIO_ANASEL_PE3ANS_SEG18：选择作为SEG18
      GPIO_ANASEL_PE3ANS_ACMP1_INN1：选择作为ACMP1_INN1
输出:无
********************************/
void GPIO_ANASEL_PE3ANS_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->ANASEL;
	tmpreg &= ~(GPIO_ANASEL_PE3ANS_Msk);
	tmpreg |= (SetValue & GPIO_ANASEL_PE3ANS_Msk);
	GPIO->ANASEL = tmpreg;
}
/********************************
读PE3模拟功能选择状态函数
功能:读PE3模拟功能选择状态
输入：无
输出: GPIO_ANASEL_PE3ANS_SEG18：选择SEG18
      GPIO_ANASEL_PE3ANS_ACMP1_INN1：选择ACMP1_INN1
********************************/
uint32_t GPIO_ANASEL_PE3ANS_Get(void)
{
	return (GPIO->ANASEL & GPIO_ANASEL_PE3ANS_Msk);
}

/********************************
 PC15模拟功能选择函数
功能: 配置PC15模拟功能选择
输入：GPIO_ANASEL_PC15ANS_SEG39：选择作为SEG39
      GPIO_ANASEL_PC15ANS_ACMP2_INP0_ADC_IN6：选择作为ACMP2_INP0或ADC_IN6
输出:无
********************************/
void GPIO_ANASEL_PC15ANS_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->ANASEL;
	tmpreg &= ~(GPIO_ANASEL_PC15ANS_Msk);
	tmpreg |= (SetValue & GPIO_ANASEL_PC15ANS_Msk);
	GPIO->ANASEL = tmpreg;
}
/********************************
读PC15模拟功能选择状态函数
功能:读PC15模拟功能选择状态
输入：无
输出: GPIO_ANASEL_PC15ANS_SEG39：选择SEG39
      GPIO_ANASEL_PC15ANS_ACMP2_INP0_ADC_IN6：选择ACMP2_INP0或ADC_IN6
********************************/
uint32_t GPIO_ANASEL_PC15ANS_Get(void)
{
	return (GPIO->ANASEL & GPIO_ANASEL_PC15ANS_Msk);
}

/********************************
 PC14模拟功能选择函数
功能: 配置PC14模拟功能选择
输入：GPIO_ANASEL_PC14ANS_SEG38：选择作为SEG38
      GPIO_ANASEL_PC14ANS_ACMP1_INN0：选择作为ACMP1_INN0
输出:无
********************************/
void GPIO_ANASEL_PC14ANS_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->ANASEL;
	tmpreg &= ~(GPIO_ANASEL_PC14ANS_Msk);
	tmpreg |= (SetValue & GPIO_ANASEL_PC14ANS_Msk);
	GPIO->ANASEL = tmpreg;
}
/********************************
读PC14模拟功能选择状态函数
功能:读PC14模拟功能选择状态
输入：无
输出: GPIO_ANASEL_PC14ANS_SEG38：选择SEG38
      GPIO_ANASEL_PC14ANS_ACMP1_INN0：选择ACMP1_INN0
********************************/
uint32_t GPIO_ANASEL_PC14ANS_Get(void)
{
	return (GPIO->ANASEL & GPIO_ANASEL_PC14ANS_Msk);
}

/********************************
 PC13模拟功能选择函数
功能: 配置PC13模拟功能选择
输入：GPIO_ANASEL_PC13ANS_SEG37：选择作为SEG37
      GPIO_ANASEL_PC13ANS_ADC_IN2：选择作为ADC_IN2
输出:无
********************************/
void GPIO_ANASEL_PC13ANS_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->ANASEL;
	tmpreg &= ~(GPIO_ANASEL_PC13ANS_Msk);
	tmpreg |= (SetValue & GPIO_ANASEL_PC13ANS_Msk);
	GPIO->ANASEL = tmpreg;
}
/********************************
读PC13模拟功能选择状态函数
功能:读PC13模拟功能选择状态
输入：无
输出: GPIO_ANASEL_PC13ANS_SEG37：选择SEG37
      GPIO_ANASEL_PC13ANS_ADC_IN2：选择ADC_IN2
********************************/
uint32_t GPIO_ANASEL_PC13ANS_Get(void)
{
	return (GPIO->ANASEL & GPIO_ANASEL_PC13ANS_Msk);
}
/********************************
 PC12模拟功能选择函数
功能: 配置PC12模拟功能选择
输入：GPIO_ANASEL_PC12ANS_SEG36：选择作为SEG36
      GPIO_ANASEL_PC12ANS_ADC_IN1：选择作为ADC_IN1
输出:无
********************************/
void GPIO_ANASEL_PC12ANS_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->ANASEL;
	tmpreg &= ~(GPIO_ANASEL_PC12ANS_Msk);
	tmpreg |= (SetValue & GPIO_ANASEL_PC12ANS_Msk);
	GPIO->ANASEL = tmpreg;
}
/********************************
读PC12模拟功能选择状态函数
功能:读PC12模拟功能选择状态
输入：无
输出: GPIO_ANASEL_PC12ANS_SEG36：选择SEG36
      GPIO_ANASEL_PC12ANS_ADC_IN1：选择ADC_IN1
********************************/
uint32_t GPIO_ANASEL_PC12ANS_Get(void)
{
	return (GPIO->ANASEL & GPIO_ANASEL_PC12ANS_Msk);
}

void GPIOx_Deinit(GPIOx_Type *GPIOx)
{
	GPIOx->INEN = 0x00000000;
	GPIOx->PUEN = 0x00000000;
	GPIOx->ODEN = 0x00000000;
	GPIOx->FCR = 0x00000000;
	GPIOx->DO = 0x00000000;
	//GPIOx->DSET = ;
	//GPIOx->DRESET = ;
	//GPIOx->DIN = ;
}

void GPIO_Deinit(void)
{
	GPIO->EXTI0_SEL = 0xFFFF0000;
	GPIO->EXTI1_SEL = 0xFFFF0000;
	GPIO->EXTI2_SEL = 0xFFFF0000;
	//GPIO->EXTI0IF = ;
	//GPIO->EXTI1IF = ;
	//GPIO->EXTI2IF = ;
	GPIO->FOUTSEL = 0x00000000;
	GPIO->HDSEL = 0x00000000;
	GPIO->ANASEL = 0x00000000;
	GPIO->IODF = 0x00000000;
	GPIO->PINWKEN = 0x00000000;
	GPIO->PF4AFSEL = 0x00000000;
}
//Code_End

void GPIO_ALL_Deinit(void)
{
	GPIO_Deinit();
	GPIOx_Deinit(GPIOA);
	GPIOx_Deinit(GPIOB);
	GPIOx_Deinit(GPIOC);
	GPIOx_Deinit(GPIOD);
	GPIOx_Deinit(GPIOE);
	GPIOx_Deinit(GPIOF);
	GPIOx_Deinit(GPIOG);
}

/********************************
GPIO初始化配置函数
功能:GPIO初始化配置
输入：GPIO端口  GPIO_PIN引脚
输出: 无
********************************/
void GPIO_Init(GPIOx_Type *GPIOx, GPIO_InitTypeDef *para)
{
	uint32_t pinpos = 0x00, pinbit = 0x00, currentpin = 0x00;
	uint32_t tmpreg;

	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

	for (pinpos = 0; pinpos < 16; pinpos++)
	{
		pinbit = ((uint32_t)0x01) << pinpos;
		currentpin = (para->Pin) & pinbit;

		if (currentpin == pinbit)
		{
			tmpreg = GPIOx->INEN;
			tmpreg &= ~pinbit;
			tmpreg |= (para->PxINEN) << pinpos;
			GPIOx->INEN = tmpreg;

			tmpreg = GPIOx->PUEN;
			tmpreg &= ~pinbit;
			tmpreg |= (para->PxPUEN) << pinpos;
			GPIOx->PUEN = tmpreg;

			tmpreg = GPIOx->ODEN;
			tmpreg &= ~pinbit;
			tmpreg |= (para->PxODEN) << pinpos;
			GPIOx->ODEN = tmpreg;

			pinbit = (((uint32_t)0x00000003)) << (pinpos * 2);
			tmpreg = GPIOx->FCR;
			tmpreg &= ~pinbit;
			tmpreg |= (para->PxFCR) << (pinpos * 2);
			GPIOx->FCR = tmpreg;
		}
	}
}

/* 获取一个IO口的配置参数结构体 
	注意一次只能读取一个IO的配置
*/
/********************************
获取一个IO配置参数结构体函数
功能:获取一个IO配置参数结构体
输入：GPIO端口  GPIO_PIN引脚 引脚类型
输出: 无
********************************/
void GPIO_Get_InitPara(GPIOx_Type *GPIOx, uint32_t GPIO_Pin, GPIO_InitTypeDef *para)
{
	uint32_t pinbit = 0x00;
	uint32_t i, temp;

	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

	for (i = 0; i < 16; i++)
	{
		pinbit = GPIO_Pin & (1 << i);
		if (pinbit > 0)
			break;
	}

	if (i < 16)
	{
		pinbit = i;
		para->Pin = GPIO_Pin;

		temp = (GPIOx->INEN & GPIO_Pin) >> pinbit;
		if (temp == (uint32_t)GPIO_IN_En)
		{
			para->PxINEN = GPIO_IN_En;
		}
		else
		{
			para->PxINEN = GPIO_IN_Dis;
		}

		temp = (GPIOx->PUEN & GPIO_Pin) >> pinbit;
		if (temp == (uint32_t)GPIO_PU_En)
		{
			para->PxPUEN = GPIO_PU_En;
		}
		else
		{
			para->PxPUEN = GPIO_PU_Dis;
		}

		temp = (GPIOx->ODEN & GPIO_Pin) >> pinbit;
		if (temp == (uint32_t)GPIO_OD_En)
		{
			para->PxODEN = GPIO_OD_En;
		}
		else
		{
			para->PxODEN = GPIO_OD_Dis;
		}

		temp = (GPIOx->FCR >> (pinbit * 2)) & 0x00000003;
		if (temp == (uint32_t)GPIO_FCR_IN)
		{
			para->PxFCR = GPIO_FCR_IN;
		}
		else if (temp == (uint32_t)GPIO_FCR_OUT)
		{
			para->PxFCR = GPIO_FCR_OUT;
		}
		else if (temp == (uint32_t)GPIO_FCR_DIG)
		{
			para->PxFCR = GPIO_FCR_DIG;
		}
		else
		{
			para->PxFCR = GPIO_FCR_ANA;
		}
	}
}

/********************************
读取GPIOx输入数据寄存器函数
功能:读取GPIOx输入数据寄存器
输入：GPIOx_PIN引脚 
输出: 数据寄存器值
********************************/
uint32_t GPIO_ReadInputData(GPIOx_Type *GPIOx)
{
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

	//	return(GPIOx_DIN_Read(GPIOx));
	return (GPIOx->DIN);
}

/********************************
读取GPIOx输入数据寄存器bit函数
功能:GPIOx输入数据寄存器bit
输入：GPIO端口  GPIO_PIN引脚 
输出: bit状态
********************************/
uint8_t GPIO_ReadInputDataBit(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
	uint8_t bitstatus = 0x00;

	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	assert_param(IS_GET_GPIO_PIN(GPIO_Pin));

	if ((GPIOx->DIN & GPIO_Pin) != (uint32_t)Bit_RESET)
	{
		bitstatus = (uint8_t)Bit_SET;
	}
	else
	{
		bitstatus = (uint8_t)Bit_RESET;
	}
	return bitstatus;
}

/********************************
GPIOx输出置1函数
功能: GPIOx输出置1
输入：GPIO端口  GPIO_PIN引脚 
输出: 无
********************************/
void GPIO_SetBits(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	assert_param(IS_GPIO_PIN(GPIO_Pin));

	//	GPIOx_DSET_Write(GPIOx, Pin);
	GPIOx->DSET = GPIO_Pin;
}

/********************************
GPIOx输出置0函数
功能: GPIOx输出置0
输入：GPIO端口  GPIO_PIN引脚 
输出: 无
********************************/
void GPIO_ResetBits(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	assert_param(IS_GPIO_PIN(GPIO_Pin));

	//	GPIOx_DRESET_Write(GPIOx, Pin);
	GPIOx->DRESET = GPIO_Pin;
}

/********************************
GPIOx输出翻转函数
功能: GPIOx输出置0
输入：GPIO端口  GPIO_PIN引脚 
输出: 无
********************************/
void GPIO_ToggleBits(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	assert_param(IS_GPIO_PIN(GPIO_Pin));

	if (GPIO_Pin & GPIOx->DO)
	{
		//GPIO_ResetBits(GPIOx, GPIO_Pin);
		GPIOx->DRESET = GPIO_Pin;
	}
	else
	{
		//GPIO_SetBits(GPIOx, GPIO_Pin);
		GPIOx->DSET = GPIO_Pin;
	}
}

/********************************
读取GPIOx输出数据寄存器函数
功能:读取GPIOx输出数据寄存器
输入：GPIOx_PIN引脚 
输出: 数据寄存器值
********************************/
uint32_t GPIO_ReadOutputData(GPIOx_Type *GPIOx)
{
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

	//	return(GPIOx_DO_Read(GPIOx));
	return (GPIOx->DO);
}

/********************************
读取GPIOx输出数据寄存器bit函数
功能:GPIOx输出数据寄存器bit
输入：GPIO端口  GPIO_PIN引脚 
输出: bit状态
********************************/
uint8_t GPIO_ReadOutputDataBit(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
	uint8_t bitstatus = 0x00;

	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	assert_param(IS_GET_GPIO_PIN(GPIO_Pin));

	if (((GPIOx->DO) & GPIO_Pin) != (uint32_t)Bit_RESET)
	{
		bitstatus = (uint8_t)Bit_SET;
	}
	else
	{
		bitstatus = (uint8_t)Bit_RESET;
	}
	return bitstatus;
}

/********************************
GPIO输入数字滤波配置函数
功能:输入数字滤波配置
输入：数字滤波GPIO_PIN引脚        
      ENABLE 使能数字滤波
      DISABLE 关闭数字滤波
输出: 无
********************************/
void GPIO_IODF_SetableEx(uint32_t DFPinDef, FunState NewState)
{
	if (NewState == ENABLE)
	{
		GPIO->IODF |= ((uint32_t)0x01) << DFPinDef;
	}
	else
	{
		GPIO->IODF &= ~(((uint32_t)0x01) << DFPinDef);
	}
}

/********************************
WKUP引脚配置函数
功能：WKUP引脚配置
输入：WKUP GPIO_PIN引脚        
      ENABLE 使能WKUP功能
      DISABLE 关闭WKUP功能
输出: 无
********************************/
void GPIO_PINWKEN_SetableEx(uint32_t NWKPinDef, FunState NewState)
{
	if (NewState == ENABLE)
	{
		GPIO->PINWKEN |= NWKPinDef;
	}
	else
	{
		GPIO->PINWKEN &= ~NWKPinDef;
	}
}

/********************************
WKUP边沿选择函数
功能?WWKUP边沿选择
输入：WKUP GPIO_PIN引脚        
      ENABLE 上升沿
      DISABLE 下降沿
输出: 无
********************************/
void GPIO_PINWKSEL_SetableEx(uint32_t NWKPinDef, FunState NewState)
{
	if (NewState == ENABLE)
	{
		GPIO->PINWKEN |= (NWKPinDef << 8);
	}
	else
	{
		GPIO->PINWKEN &= ~(NWKPinDef << 8);
	}
}

/********************************
WKUP中断入口选择函数
功能?WWKUP中断入口选择
输入：GPIO_PINWKEN_WKISEL_NMI: NMI中断      
      GPIO_PINWKEN_WKISEL_38：#38中断
输出: 无
********************************/
void GPIO_PINWKEN_WKISEL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->PINWKEN;
	tmpreg &= ~(GPIO_PINWKEN_WKISEL_Msk);
	tmpreg |= (SetValue & GPIO_PINWKEN_WKISEL_Msk);
	GPIO->PINWKEN = tmpreg;
}
/********************************
读取WKUP中断入口选择函数
功能：读取WKUP中断入口选择
输入：无     
输出: GPIO_PINWKEN_WKISEL_NMI: NMI中断 
      GPIO_PINWKEN_WKISEL_38：#38中断
********************************/
uint32_t GPIO_PINWKEN_WKISEL_Get(void)
{
	return (GPIO->PINWKEN & GPIO_PINWKEN_WKISEL_Msk);
}

/********************************
PF4复用功能选择函数
功能：PF4复用功能选择
输入：GPIO_PF4AFSEL_PF4AFS_UART0_TX：UART_TX
      GPIO_PF4AFSEL_PF4AFS_LPUART_TX: LPUART_TX
      GPIO_PF4AFSEL_PF4AFS_TFO:TFO
输出:无
********************************/
void GPIO_PF4AFSEL_PF4AFS_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->PF4AFSEL;
	tmpreg &= ~(GPIO_PF4AFSEL_PF4AFS_Msk);
	tmpreg |= (SetValue & GPIO_PF4AFSEL_PF4AFS_Msk);
	GPIO->PF4AFSEL = tmpreg;
}
/********************************
读取PF4复用功能选择函数
功能：读取PF4复用功能选择
输入：无
输出：GPIO_PF4AFSEL_PF4AFS_UART0_TX：UART_TX
      GPIO_PF4AFSEL_PF4AFS_LPUART_TX: LPUART_TX
      GPIO_PF4AFSEL_PF4AFS_TFO:TFO
********************************/
uint32_t GPIO_PF4AFSEL_PF4AFS_Get(void)
{
	return (GPIO->PF4AFSEL & GPIO_PF4AFSEL_PF4AFS_Msk);
}

/********************************
PF4测试频率输出信号选择函数
功能：PF4测试频率输出信号选择函
输入：
输出：无
********************************/
void GPIO_PF4AFSEL_TFOSEL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = GPIO->PF4AFSEL;
	tmpreg &= ~(GPIO_PF4AFSEL_TFOSEL_Msk);
	tmpreg |= (SetValue & GPIO_PF4AFSEL_TFOSEL_Msk);
	GPIO->PF4AFSEL = tmpreg;
}
/********************************
读取PF4测试频率输出信号选择函数
功能：读取PF4测试频率输出信号选择函
输入：无
输出：
********************************/
uint32_t GPIO_PF4AFSEL_TFOSEL_Get(void)
{
	return (GPIO->PF4AFSEL & GPIO_PF4AFSEL_TFOSEL_Msk);
}
/*************************************************************************
 函数名称：GPIO_EXTI_Select_Pin
 功能说明：GPIO EXTI 外部引脚选择
 输入参数：GPIOx 端口 GPIO_Pinpin 端口内引脚 
 输出参数：无
 返回参数?w无
 *************************************************************************/
void GPIO_EXTI_Select_Pin(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
	uint32_t pin_num = 0, i;
	uint32_t pin_shift;
	uint32_t tmpreg;

	for (i = 0; i < 16; i++)
	{
		if (GPIO_Pin & (1 << i))
		{
			pin_num = i;
			break;
		}
	}

	switch ((uint32_t)GPIOx)
	{
	case (uint32_t)GPIOA:
	case (uint32_t)GPIOB:
		pin_shift = (pin_num % 8) * 2;
		tmpreg = GPIO->EXTI0_SEL;
		tmpreg &= (~(0x00000003 << pin_shift));
		if (GPIOx == GPIOA)
		{
			tmpreg |= ((pin_num / 8) << pin_shift);
		}
		else
		{
			tmpreg |= (((pin_num / 8) + 2) << pin_shift);
		}
		GPIO->EXTI0_SEL = tmpreg;
		break;

	case (uint32_t)GPIOC:
		pin_shift = (pin_num % 8) * 2;
		tmpreg = GPIO->EXTI1_SEL;
		tmpreg &= (~(0x00000003 << pin_shift));
		tmpreg |= ((pin_num / 8) << pin_shift);
		GPIO->EXTI1_SEL = tmpreg;
		break;

	case (uint32_t)GPIOD: //0~10
		if (pin_num <= 10)
		{
			pin_shift = (pin_num % 8) * 2;
			tmpreg = GPIO->EXTI1_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= (((pin_num / 8) + 2) << pin_shift);
			GPIO->EXTI1_SEL = tmpreg;
		}
		break;

	case (uint32_t)GPIOE:					  //2~9(-2)
		if ((pin_num >= 2) && (pin_num <= 5)) //1bit
		{
			pin_shift = pin_num - 2;
			tmpreg = GPIO->EXTI2_SEL;
			tmpreg &= (~(0x00000001 << pin_shift));
			GPIO->EXTI2_SEL = tmpreg;
		}
		else if ((pin_num >= 6) && (pin_num <= 9)) //2bit
		{
			pin_shift = 4 + (pin_num - 6) * 2;
			tmpreg = GPIO->EXTI2_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			GPIO->EXTI2_SEL = tmpreg;
		}
		break;

	case (uint32_t)GPIOF:					  //1~7   8~11(-4)
		if ((pin_num >= 1) && (pin_num <= 3)) //1bit
		{
			pin_shift = pin_num;
			tmpreg = GPIO->EXTI2_SEL;
			tmpreg &= (~(0x00000001 << pin_shift));
			tmpreg |= (0x00000001 << pin_shift);
			GPIO->EXTI2_SEL = tmpreg;
		}
		else if ((pin_num >= 4) && (pin_num <= 7)) //2bit-a
		{
			pin_shift = 4 + (pin_num - 4) * 2;
			tmpreg = GPIO->EXTI2_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= (0x00000001 << pin_shift);
			GPIO->EXTI2_SEL = tmpreg;
		}
		else if ((pin_num >= 8) && (pin_num <= 11)) //2bit-b
		{
			pin_shift = 4 + (pin_num - 8) * 2;
			tmpreg = GPIO->EXTI2_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= (0x00000002 << pin_shift);
			GPIO->EXTI2_SEL = tmpreg;
		}
		break;

	case (uint32_t)GPIOG: //2(+1) 5~8(-1)
		if (pin_num == 2)
		{
			pin_shift = 6;
			tmpreg = GPIO->EXTI1_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= (0x00000003 << pin_shift);
			GPIO->EXTI1_SEL = tmpreg;
		}
		else if ((pin_num >= 5) && (pin_num <= 8))
		{
			pin_shift = (pin_num - 1) * 2;
			tmpreg = GPIO->EXTI1_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= (0x00000003 << pin_shift);
			GPIO->EXTI1_SEL = tmpreg;
		}
		break;

	default:
		break;
	}
}

/*************************************************************************
 函数名称：GPIO_EXTI_Select_Edge
 功能说明：GPIO EXTI_Select_Edge 外部中断边沿选择
 输入参数：GPIOx 端口 ，GPIO_Pin 端口内引脚 ,edge_select 触发沿
 输出参数：无
 返回参数?w无
 *************************************************************************/
void GPIO_EXTI_Select_Edge(GPIOx_Type *GPIOx, uint32_t GPIO_Pin, GPIOExtiEdge edge_select)
{
	uint32_t pin_num = 0, i;
	uint32_t pin_shift;
	uint32_t tmpreg;

	for (i = 0; i < 16; i++)
	{
		if (GPIO_Pin & (1 << i))
		{
			pin_num = i;
			break;
		}
	}

	switch ((uint32_t)GPIOx)
	{
	case (uint32_t)GPIOA:
	case (uint32_t)GPIOB:
		pin_shift = (pin_num % 8) * 2 + 16;
		tmpreg = GPIO->EXTI0_SEL;
		tmpreg &= (~(0x00000003 << pin_shift));
		tmpreg |= edge_select << pin_shift;
		GPIO->EXTI0_SEL = tmpreg;
		break;

	case (uint32_t)GPIOC:
		pin_shift = (pin_num % 8) * 2 + 16;
		tmpreg = GPIO->EXTI1_SEL;
		tmpreg &= (~(0x00000003 << pin_shift));
		tmpreg |= edge_select << pin_shift;
		GPIO->EXTI1_SEL = tmpreg;
		break;

	case (uint32_t)GPIOD: //0~10
		if (pin_num <= 10)
		{
			pin_shift = (pin_num % 8) * 2 + 16;
			tmpreg = GPIO->EXTI1_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= edge_select << pin_shift;
			GPIO->EXTI1_SEL = tmpreg;
		}
		break;

	case (uint32_t)GPIOE: //2~9(-2)
		if ((pin_num >= 2) && (pin_num <= 9))
		{
			pin_shift = (pin_num - 2) * 2 + 16;
			tmpreg = GPIO->EXTI2_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= edge_select << pin_shift;
			GPIO->EXTI2_SEL = tmpreg;
		}
		break;

	case (uint32_t)GPIOF: //1~7   8~11(-4)
		if ((pin_num >= 1) && (pin_num <= 7))
		{
			pin_shift = pin_num * 2 + 16;
			tmpreg = GPIO->EXTI2_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= edge_select << pin_shift;
			GPIO->EXTI2_SEL = tmpreg;
		}
		else if ((pin_num >= 8) && (pin_num <= 11))
		{
			pin_shift = ((pin_num - 8) + 4) * 2 + 16;
			tmpreg = GPIO->EXTI2_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= edge_select << pin_shift;
			GPIO->EXTI2_SEL = tmpreg;
		}
		break;

	case (uint32_t)GPIOG: //2(+1) 5~8(-1)
		if (pin_num == 2)
		{
			pin_shift = (pin_num + 1) * 2 + 16;
			tmpreg = GPIO->EXTI1_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= edge_select << pin_shift;
			GPIO->EXTI1_SEL = tmpreg;
		}
		else if ((pin_num >= 5) && (pin_num <= 8))
		{
			pin_shift = (pin_num - 1) * 2 + 16;
			tmpreg = GPIO->EXTI1_SEL;
			tmpreg &= (~(0x00000003 << pin_shift));
			tmpreg |= edge_select << pin_shift;
			GPIO->EXTI1_SEL = tmpreg;
		}
		break;

	default:
		break;
	}
}

/*************************************************************************
 函数名称：GPIO_EXTI_EXTIxIF_ClrEx
 功能说明：GPIO_EXTI_EXTIxIF_ClrEx 清除中断标志
 输入参数：GPIOx 端口 ，GPIO_Pin 端口内引脚 
 输出参数：无
 返回参数?w无
 *************************************************************************/
void GPIO_EXTI_EXTIxIF_ClrEx(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
	uint32_t pin_num = 0, i;

	for (i = 0; i < 16; i++)
	{
		if (GPIO_Pin & (1 << i))
		{
			pin_num = i;
			break;
		}
	}

	switch ((uint32_t)GPIOx)
	{
	case (uint32_t)GPIOA:
	case (uint32_t)GPIOB:
		GPIO->EXTI0IF = (0x00000001 << (pin_num & 0x07));
		break;

	case (uint32_t)GPIOC:
		GPIO->EXTI1IF = (0x00000001 << (pin_num & 0x07));
		break;

	case (uint32_t)GPIOD: //0~10
		if (pin_num <= 10)
		{
			GPIO->EXTI1IF = (0x00000001 << (pin_num & 0x07));
		}
		break;

	case (uint32_t)GPIOE: //2~9(-2)
		if ((pin_num >= 2) && (pin_num <= 9))
		{
			GPIO->EXTI2IF = (0x00000001 << (pin_num - 2));
		}
		break;

	case (uint32_t)GPIOF: //1~7   8~11(-4)
		if ((pin_num >= 1) && (pin_num <= 7))
		{
			GPIO->EXTI2IF = (0x00000001 << (pin_num & 0x07));
		}
		else if ((pin_num >= 8) && (pin_num <= 11))
		{
			GPIO->EXTI2IF = (0x00000001 << (pin_num - 4));
		}
		break;

	case (uint32_t)GPIOG: //2(+1) 5~8(-1)
		if (pin_num == 2)
		{
			GPIO->EXTI1IF = (0x00000001 << (pin_num + 1));
		}
		else if ((pin_num >= 5) && (pin_num <= 8))
		{
			GPIO->EXTI1IF = (0x00000001 << (pin_num - 1));
		}
		break;

	default:
		break;
	}
}

/*************************************************************************
 函数名称：GPIO_EXTI_EXTIxIF_ChkEx
 功能说明：GPIO_EXTI_EXTIxIF_ChkEx 读取中断标志状态
 输入参数：GPIOx 端口 ，GPIO_Pin 端口内引脚 
 输出参数：无
 返回参数?wFlagStatus 中断标志状态
 *************************************************************************/
FlagStatus GPIO_EXTI_EXTIxIF_ChkEx(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
	uint32_t pin_num = 0, i;
	FlagStatus Result = RESET;

	for (i = 0; i < 16; i++)
	{
		if (GPIO_Pin & (1 << i))
		{
			pin_num = i;
			break;
		}
	}

	Result = RESET;
	switch ((uint32_t)GPIOx)
	{
	case (uint32_t)GPIOA:
	case (uint32_t)GPIOB:
		if (GPIO->EXTI0IF & (0x00000001 << (pin_num & 0x07)))
			Result = SET;
		break;

	case (uint32_t)GPIOC:
		if (GPIO->EXTI1IF & (0x00000001 << (pin_num & 0x07)))
			Result = SET;
		break;

	case (uint32_t)GPIOD: //0~10
		if (pin_num <= 10)
		{
			if (GPIO->EXTI1IF & (0x00000001 << (pin_num & 0x07)))
				Result = SET;
		}
		break;

	case (uint32_t)GPIOE: //2~9(-2)
		if ((pin_num >= 2) && (pin_num <= 9))
		{
			if (GPIO->EXTI2IF & (0x00000001 << (pin_num - 2)))
				Result = SET;
		}
		break;

	case (uint32_t)GPIOF: //1~7   8~11(-4)
		if ((pin_num >= 1) && (pin_num <= 7))
		{
			if (GPIO->EXTI2IF & (0x00000001 << (pin_num & 0x07)))
				Result = SET;
		}
		else if ((pin_num >= 8) && (pin_num <= 11))
		{
			if (GPIO->EXTI2IF & (0x00000001 << (pin_num - 4)))
				Result = SET;
		}
		break;

	case (uint32_t)GPIOG: //2(+1) 5~8(-1)
		if (pin_num == 2)
		{
			if (GPIO->EXTI1IF & (0x00000001 << (pin_num + 1)))
				Result = SET;
		}
		else if ((pin_num >= 5) && (pin_num <= 8))
		{
			if (GPIO->EXTI1IF & (0x00000001 << (pin_num - 1)))
				Result = SET;
		}
		break;

	default:
		break;
	}

	return Result;
}

/*************************************************************************
 函数名称：GPIO_EXTI_Init
 功能说明：GPIO_EXTI_Init 外部中断初始化
 输入参数：port 端口 ，pin 端口内引脚 ,edge 触发沿
 输出参数：无
 返回参数?w无
 *************************************************************************/
//FM33G0XX的7组GPIO（A~G）最多可以产生24个外部引脚中断，部分IO不能同时开启中断功能
void GPIO_EXTI_Init(GPIOx_Type *GPIOx, uint32_t GPIO_Pin, GPIOExtiEdge edge_select)
{
	GPIO_EXTI_Select_Edge(GPIOx, GPIO_Pin, EXTI_DISABLE); //关闭边沿触发
	GPIO_EXTI_Select_Pin(GPIOx, GPIO_Pin);				  //外部引脚选择
	GPIO_EXTI_Select_Edge(GPIOx, GPIO_Pin, edge_select);  //修改边沿选择
	GPIO_EXTI_EXTIxIF_ClrEx(GPIOx, GPIO_Pin);			  //清除中断标志
}

/*************************************************************************
 函数名称：GPIO_EXTI_Close
 功能说明：GPIO_EXTI_Close 外部中断关闭
 输入参数：port 端口 ，pin 端口内引脚 
 输出参数：无
 返回参数?w无
 *************************************************************************/
void GPIO_EXTI_Close(GPIOx_Type *GPIOx, uint32_t GPIO_Pin)
{
	GPIO_EXTI_Select_Edge(GPIOx, GPIO_Pin, EXTI_DISABLE); //关闭边沿触发
}

/******END OF FILE****/
