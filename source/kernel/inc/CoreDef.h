#ifndef CORE_DEF_H_
#define CORE_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "include/PriorRTOSConfig.h"
#include "include/OsTypes.h"

#define CORE_OS_TIMER_TICKS_TO_US(ticks) (U32_t)(((float)ticks / PRTOS_CONFIG_F_OS_TIMER_HZ) * 1e6)
#define USEC_PER_HOUR	0xD693A400 /* 3600.000.000 us. */

typedef enum {
    CORE_FLAG_ALL        =0xFFFF,
    CORE_FLAG_TICK       =0x8000,
    CORE_FLAG_SCHEDULER  =0x4000,
    CORE_FLAG_IDLE       =0x1000,
    CORE_FLAG_DISPATCH   =0x0800,
    CORE_FLAG_STACK_OVF  =0x0400,
    CORE_FLAG_WDT_IRQ    =0x0200,
    CORE_FLAG_WAIT       =0x0100,
    CORE_FLAG_KERNEL_MODE=0x0080,
    CORE_FLAG_OS_IRQ_EN  =0x0040,
    CORE_FLAG_EXCEPTION  =0x0020,
} CoreFlags_t;

void KCoreFlagSet(CoreFlags_t flag);
void KCoreFlagClear(CoreFlags_t flag);
U16_t KCoreFlagGet(CoreFlags_t flag);

U8_t KCoreKernelModeEnter(void);
U8_t KCoreKernelModeExit(void);

Id_t KCoreTaskRunningGet(void);

#ifdef __cplusplus
}
#endif
#endif
