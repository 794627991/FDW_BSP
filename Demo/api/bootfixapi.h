#ifndef __API_BOOTFIXAPI_H__
#define __API_BOOTFIXAPI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "api_define_all.h"

#define Boot_API_Base_Adr 0x00001000 //boot固化FUN到boot区的0x1000

#define API_ADC_Loc 0
#define API_ETIM_Loc 1
#define API_I2C_Init_Loc 2
#define API_I2C_Read_Loc 3
#define API_I2C_Write_Loc 4
#define API_I2C_Read_Com_Loc 5
#define API_I2C_Write_Com_Loc 6
#define API_LPTIM_Loc 7
#define API_RTC_Init_Loc 8
#define API_GetTime_Loc 9
#define API_SetTIME_Loc 10
#define API_Uart_Init_Loc 11
#define API_Uart_Send_Loc 12
#define API_LPUart_Init_Loc 13
#define API_LPUart_Send_Loc 14
#define API_UpData_Loc 15
#define API_NB_Init_Loc 16
#define API_NB_Link_Loc 17
#define API_NB_Connect_Loc 18
#define API_NB_UartRx_Loc 19
#define API_NB_UartTx_Loc 20
#define API_NB_Clock_Loc 21
#define API_NB_End_Loc 22
#define API_NB_RXDATA_Loc 23
#define API_LCD_Loc 24
#define API_AES_Loc 25
    /**************************函数指针定义**************************/

    /****************************函数声明****************************/

#ifdef __cplusplus
}
#endif

#endif
