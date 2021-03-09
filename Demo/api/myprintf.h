#ifndef __MYPRINFT_H__
#define __MYPRINFT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "fm33G0xx_uart.h"
#include "API_CFG.h"

#define _PRINTF_UART(x) UART##x
#define PRINTF_UART(x) _PRINTF_UART(x)

#ifdef __cplusplus
}
#endif

#endif
