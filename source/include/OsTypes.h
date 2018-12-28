/******************************************************************************************************************************************
 *  File: Types.h
 *  Description: OS standard types.

 *  OS Version: V0.4
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *  Copyright© 2017    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed or sold, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************************************************************************************/


#ifndef TYPES_H_
#define TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "PriorRTOSConfig.h"
#include "StdTypes.h"
#include "OsResult.h"
#include "IdType.h"


#ifdef PRTOS_CONFIG_MEM_WIDTH_8_BITS
#if (defined(PRTOS_CONFIG_MEM_WIDTH_16_BITS) || defined(PRTOS_CONFIG_MEM_WIDTH_32_BITS))
#error "PriorRTOS: Ambiguous memory width; multiple PRTOS_CONFIG_MEM_WIDTH_* definitions found."
#endif
typedef     U8_t   MemBase_t;
#endif
#ifdef PRTOS_CONFIG_MEM_WIDTH_16_BITS
#if (defined(PRTOS_CONFIG_MEM_WIDTH_8_BITS) || defined(PRTOS_CONFIG_MEM_WIDTH_32_BITS))
#error "PriorRTOS: Ambiguous memory width; multiple PRTOS_CONFIG_MEM_WIDTH_* definitions found."
#endif
typedef     U16_t   MemBase_t;
#endif
#ifdef PRTOS_CONFIG_MEM_WIDTH_32_BITS
#if (defined(PRTOS_CONFIG_MEM_WIDTH_8_BITS) || defined(PRTOS_CONFIG_MEM_WIDTH_16_BITS))
#error "PriorRTOS: Ambiguous memory width; multiple PRTOS_CONFIG_MEM_WIDTH_* definitions found."
#endif
typedef     U32_t   MemBase_t;
#endif

typedef     U8_t    Prio_t;
#define     OS_TASK_PRIO_LIMIT_HIGH 5
#define     OS_TASK_PRIO_LIMIT_LOW  1

typedef     U16_t   OsVer_t;
typedef     PRTOS_CONFIG_IRQ_PRIORITY_TYPE  IrqPriority_t;

typedef		U32_t   OsRunTime_t[2];
#define		OS_RUN_TIME_HOURS	0
#define		OS_RUN_TIME_MICROS	1
#define		OS_RUN_TIME_INIT(rt) {	\
			rt[0] = 0;				\
			rt[1] = 0;				\
}									\

/* Can be used to suppress compiler warnings regarding unused (task) arguments. */
#define OS_ARG_UNUSED(arg) ((void)(arg))

#define OS_TIMEOUT_INFINITE	0xFFFFFFFF /* Wait or Poll forever. */
#define OS_TIMEOUT_NONE		0x00000000 /* Do not Wait or Poll. */



/* Task type definition */
typedef void (*Task_t)(const void* p_arg, U32_t v_arg);

#ifdef __cplusplus
}
#endif
#endif
