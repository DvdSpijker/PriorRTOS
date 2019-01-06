/******************************************************************************************************************************************
 *  File: PriorRTOS.h
 *  Description: Includes all enabled modules.

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
#ifndef PRIOR_RTOS_H
#define PRIOR_RTOS_H

#ifdef __cplusplus
extern "C" {
#endif

#define PRIOR_RTOS
#define OS_VERSION 0x0041 /* 0.4.1 */

#include "PriorRTOSConfig.h"
#include "OsTypes.h"

/* Required includes */
#include "Os.h"
#include "Memory.h"
#include "Task.h"
#include "Logger.h"

/* Utility API includes */
#ifdef PRTOS_CONFIG_USE_CONVERT_LIB_IN_APP
#include "Convert.h"
#endif

/* Optional includes */

#if PRTOS_CONFIG_ENABLE_SOFTWARE_TIMERS==1
#include "Timer.h"
#endif

#if PRTOS_CONFIG_ENABLE_EVENTGROUPS==1
#include "Eventgroup.h"
#endif

#if PRTOS_CONFIG_ENABLE_MAILBOXES==1
#include "Mailbox.h"
#endif

#if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
#include "Ringbuffer.h"
#endif

#if PRTOS_CONFIG_ENABLE_SEMAPHORES==1
#include "Semaphore.h"
#endif

#if PRTOS_CONFIG_ENABLE_MESSAGES==1
#include "Message.h"
#endif


#ifdef __cplusplus
}
#endif
#endif
