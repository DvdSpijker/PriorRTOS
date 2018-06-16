#ifndef PORT_CORE_H
#define PORT_CORE_H

#include <OsTypes.h>
#include <stdint.h>

/***** SuperVisor mode Port. *****/

/******************************************************************************
 * @func: void PortSuperVisorModeEnable(void)
 *
 * @desc: Enables supervisor mode.
 ******************************************************************************/
void PortSuperVisorModeEnable(void);

/******************************************************************************
 * @func: void PortSuperVisorModeDisable(void)
 *
 * @desc: Disables supervisor mode.
 ******************************************************************************/
void PortSuperVisorModeDisable(void);


/***** Global Interrupt Port. *****/

/******************************************************************************
 * @func: void PortGlobalIntEnable(void)
 *
 * @desc: Enables global interrupts.
 ******************************************************************************/
void PortGlobalIntEnable(void);

/******************************************************************************
 * @func: void PortGlobalIntDisable(void)
 *
 * @desc: Disables global interrupts.
 ******************************************************************************/
void PortGlobalIntDisable(void);




/***** OS Timer Port. *****/

/******************************************************************************
 * @func: void PortOsTimerInit(U16_t prescaler, U16_t ovf)
 *
 * @desc: The OS Timer must generate a periodic interrupt, the
 * OS Tick. Inside the Interrupt Service Routine the OS Tick
 * function (defined below) must be called.
 *
 * @argin: (U16_t) prescaler; Timer prescaler value.
 * @argin: (U16_t) ovf; Timer overflow value.
 ******************************************************************************/
void PortOsTimerInit(U16_t prescaler, U16_t ovf);

/******************************************************************************
 * @func: void PortOsTimerEnable(void)
 *
 * @desc: Enables the OS Timer.
 ******************************************************************************/
void PortOsTimerEnable(void);

/******************************************************************************
 * @func: void PortOsTimerDisable(void)
 *
 * @desc: Disables the OS Timer.
 ******************************************************************************/
void PortOsTimerDisable(void);

/******************************************************************************
 * @func: U32_t PortOsTimerTicksGet(void)
 *
 * @desc: Returns the current number of ticks of the OS Timer.
 *
 * @rettype: (U32_t); ticks
 * @retval: Any; valid number of ticks.
 ******************************************************************************/
U32_t PortOsTimerTicksGet(void);

/******************************************************************************
 * @func: void PortOsTimerTicksSet(U32_t ticks)
 *
 * @desc: Sets the number of ticks of the OS Timer.
 *
 * @argin: (U32_t) ticks; Number of ticks.
 ******************************************************************************/
void PortOsTimerTicksSet(U32_t ticks);

/******************************************************************************
 * @func: void PortOsTimerTicksReset(void)
 *
 * @desc: Sets the number of ticks of the OS Timer to 0.
 ******************************************************************************/
void PortOsTimerTicksReset(void);


/***** OS (Tick) Interrupt Port. *****/

/******************************************************************************
 * @func: void PortOsIntInit(IrqPriority_t os_tick_irq_prio)
 *
 * @desc: Initializes the OS Interrupt at the given IRQ priority.
 *
 * @argin: (IrqPriority_t) os_tick_irq_prio; OS Tick IRQ priority level.
 ******************************************************************************/
void PortOsIntInit(IrqPriority_t os_tick_irq_prio);

/******************************************************************************
 * @func: void PortOsIntEnable(void)
 *
 * @desc: Enables the OS Interrupt.
 ******************************************************************************/
void PortOsIntEnable(void);

/******************************************************************************
 * @func: void PortOsIntDisable(void)
 *
 * @desc: Enables the OS Interrupt.
 ******************************************************************************/
void PortOsIntDisable(void);

/******************************************************************************
 * @func: void PortOsIntFlagClear(void)
 *
 * @desc: Clears the OS Interrupt flag.
 ******************************************************************************/
void PortOsIntFlagClear(void);

/******************************************************************************
 * @func: extern void OsTick(void)
 *
 * @desc: OS Tick function. Must be called by the OS Tick Interrupt Service
 * Routine.
 ******************************************************************************/
extern void OsTick(void);


#endif
