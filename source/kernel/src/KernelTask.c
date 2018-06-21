/**********************************************************************************************************************************************
 *  File: KernelTask.c
 *
 *  Description: Kernel Task API.
 *
 *  OS Version: V0.4
 *  Date: 13/03/2015
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *
 *  Copyright© 2017    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific prior written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************************************************************************************/
#include <PriorRTOS.h>
#include <KernelTask.h>

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>




Id_t KernelTaskCreate(Task_t handler, Prio_t priority, U8_t add_param, U32_t stack_size, const void *p_arg, U32_t v_arg)
{
    Id_t task_id = ID_INVALID;
    KCoreKernelModeEnter(); /* Has to be in kernel mode in order to create CAT_OS tasks. */
    /* All kernel tasks are not pre-empted and instantly woken by default. */
    task_id = TaskCreate(handler, TASK_CAT_OS, priority, (TASK_PARAMETER_NO_PREEM | TASK_PARAMETER_START | add_param),
                         stack_size, p_arg, v_arg);
    KCoreKernelModeExit();
    return task_id;
}



