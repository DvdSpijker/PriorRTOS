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

#include <stdint.h>
#include <PriorRTOSConfig.h>

#define OS_ARG_UNUSED(arg) ((void)(arg))

typedef uint8_t     U8_t;
typedef int8_t      S8_t;
typedef uint16_t    U16_t;
typedef int16_t     S16_t;
typedef uint32_t    U32_t;
typedef int32_t     S32_t;
typedef uint64_t    U64_t;
typedef int64_t     S64_t;

#ifdef PRTOS_CONFIG_MEM_WIDTH_8_BITS
typedef     U8_t   MemBase_t;
#endif
#ifdef PRTOS_CONFIG_MEM_WIDTH_16_BITS
typedef     U16_t   MemBase_t;
#endif
#ifdef PRTOS_CONFIG_MEM_WIDTH_32_BITS
typedef     U32_t   MemBase_t;
#endif

typedef     U16_t               Id_t;
typedef     U8_t                Prio_t;
typedef     U16_t               OsVer_t;


typedef enum {
    /* The system call was successful. */
    OS_OK               = 0,

    /* The system call failed. The reason is
     * further specified in the API description of
     * the caller. */
    OS_FAIL             = 1,

    /* The system call has caused an error. The reason is
     * further specified in the API description of
     * the caller. */
    OS_ERROR            = 2,

    /* The system call has caused a critical error. The reason is
     * further specified in the API description of
     * the caller. */
    OS_CRIT_ERROR       = 3,

    /* One or or more arguments passed to the system call were
     * out of bounds. */
    OS_OUT_OF_BOUNDS    = 4,

    /* One or more argument passed to the system call were
     * forbidden null pointers. */
    OS_NULL_POINTER     = 5,

    /* One or more arguments passed to the system call had invalid
     * values. */
    OS_INVALID_VALUE    = 6,

    /* The system call violated access rights. */
    OS_RESTRICTED       = 7,

    /* One or more IDs passed to the system call were invalid.
     * This could either be a non-existing object or the wrong
     * ID type. */
    OS_INVALID_ID       = 8,

    /* The system call attempted to access a resource that appears
     * to be locked. */
    OS_LOCKED           = 9,

    /* The system call could not be completed, the calling task will have to
     * wait for the resource. The calling task is now subscribed to the specific
     * event that indicates that the resource is again available.  */
    OS_WAIT             = 10,


    /* The calling task has NOT received the specified event
     * within the set time span. */
    OS_TIMEOUT          = 11,

} OsResult_t;


#define INVALID_ID          0xFFFF //Invalid ID
#define ID_MASK_TYPE    0xF000
#define ID_MASK_UID     0x0FFF

typedef enum {
    ID_TYPE_POOL        = 0x0000,
    ID_TYPE_TASK        = 0x1000,
    ID_TYPE_TIMER       = 0x2000,
    ID_TYPE_EVENTGROUP  = 0x3000,
    ID_TYPE_SEMAPHORE   = 0x4000,
    ID_TYPE_MAILBOX     = 0x5000,
    ID_TYPE_RINGBUF     = 0x6000,
} IdType_t;



/* Task type definition */
typedef void (*Task_t)(const void* p_arg, U32_t v_arg);

#ifdef __cplusplus
}
#endif
#endif