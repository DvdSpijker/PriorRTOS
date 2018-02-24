/******************************************************************************************************************************************
 *  File: Timer.h
 *  Description: Timer API.

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


#ifndef TIMER_H_
#define TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Types.h>
#include <EventDef.h>

/* Timer States */
typedef enum {
    TIMER_STATE_STOPPED   =0x00,
    TIMER_STATE_RUNNING   =0x01,
    TIMER_STATE_WAITING   =0x02,
    TIMER_STATE_INVALID   =0xFF,
} TmrState_t;


typedef void (*TimerOverflowCallback_t)(Id_t timer_id);

/* Timer Parameter macros */
#define TIMER_PARAMETER_ON 0x01
#define TIMER_PARAMETER_PERIODIC  0x02
#define TIMER_PARAMETER_AR 0x04
#define TIMER_PARAMETER_ITR_SET(itr) (((U8_t)itr) << 3)
#define TIMER_PARAMETER_ITR_GET(param) ((param & 0xF8) >> 3)

/* Illegal interval value. */
#define TIMER_INTERVAL_ILLEGAL 0xFFFFFFFF

/* Timer Events. */
#define TIMER_EVENT_CREATE   EVENT_TYPE_CREATE
#define TIMER_EVENT_DELETE   EVENT_TYPE_DELETE

#ifdef PRTOS_CONFIG_USE_EVENT_TIMER_OVERFLOW
#define TIMER_EVENT_OVERFLOW (EVENT_TYPE_STATE_CHANGE | 0x00000001)
#endif

#ifdef PRTOS_CONFIG_USE_EVENT_TIMER_START_STOP_RESET
#define TIMER_EVENT_START    (EVENT_TYPE_STATE_CHANGE | 0x00000002)
#define TIMER_EVENT_STOP     (EVENT_TYPE_STATE_CHANGE | 0x00000003)
#define TIMER_EVENT_RESET    (EVENT_TYPE_STATE_CHANGE | 0x00000004)
#endif

/******************************************************************************
 * @func: Id_t TimerCreate(U32_t interval, U8_t parameter, 
 *                         TimerOverflowCallback_t overflow_callback)
 *
 * @desc: Creates a timer that will overflow after the specified
 * interval. If the TIMER_PARAMETER_ON flag is set, the timer will start
 * after creation. The number of iterations before auto-delete can be
 * specified through the use of the parameter.
 * Upon overflowing the timer will do the following:
 * - If PRTOS_CONFIG_USE_TIMER_EVENT_OVERFLOW: Emit a timer overflow event.
 * - If overflow_callback != NULL: Call the overflow callback.
 * - If TIMER_PARAMETER_ON = 1: Restart the timer.
 * - If TIMER_PARAMETER_AR = 1: Automatically reset.
 * - If TIMER_PARAMETER_P = 0: Decrement the number of iterations left.
 * - If iterations left = 0: Delete the timer.
 * Timer parameter macros
 * TIMER_PARAMETER_AR (auto-reset)
 * TIMER_PARAMETER_ON 
 * TIMER_PARAMETER_PERIODIC
 * TIMER_PARAMETER_ITR_SET(itr) (sets iterations for parameter)
 * TIMER_PARAMETER_ITR_GET(param) (gets iterations from parameter)
 *
 * Arguments:
 * @argin: (U32_t) interval_us; Timer interval in microseconds(us), 0xFFFFFFFF is illegal.
 * @argin: (U8_t) parameter; Timer parameter.
 * @argin: (TimerOverflowCallback_t) overflow_callback; Called when the timer overflows.
 *
 * @rettype:  (Id_t) Timer ID
 * @retval:   INVALID_ID; if creation failed.
 * @retval:   Other; if the timer was created.
 ******************************************************************************/
Id_t TimerCreate(U32_t interval_us, U8_t parameter, TimerOverflowCallback_t overflow_callback);


/******************************************************************************
 * @func: OsResult_t TimerDelete(Id_t *timer_id)
 *
 * @desc: Deletes the specified timer and sets timer_id to INVALID_ID if the
 * operation is successful.
 *
 * Arguments:
 * @argout: (Id_t *) timer_id; ID of the timer to delete. Will be set to INVALID_ID.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the timer was successfully deleted.
 * @retval:   OS_RES_ERROR; if the timer could not be found.
 ******************************************************************************/
OsResult_t TimerDelete(Id_t *timer_id);


/* TODO: Implement OsResult_t in TimerStop. */
/******************************************************************************
 * @func: OsResult_t TimerStop(Id_t timer_id)
 *
 * @desc: Stops the specified timer, it will not start unless TimerStart
 * is called. The timer is transitioned to the stopped-state.
 * The current ticks on the timer are REMOVED, to save the ticks use TimerPause.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; ID of the timer to stop.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the timer was successfully stopped.
 * @retval:   OS_RES_ERROR; if the timer could not be found.
 ******************************************************************************/
void TimerStop(Id_t timer_id);


/* TODO: Implement OsResult_t in TimerStart. */
/******************************************************************************
 * @func: OsResult_t TimerStart(Id_t timer_id)
 *
 * @desc: Starts the specified timer.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; ID of the timer to start.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the timer was successfully started.
 * @retval:   OS_RES_ERROR; if the timer could not be found.
 ******************************************************************************/
void TimerStart(Id_t timer_id);


/* TODO: Implement OsResult_t in TimerPause. */
/******************************************************************************
 * @func: OsResult_t TimerPause(Id_t timer_id)
 *
 * @desc: Pauses the specified timer, it will not start unless TimerStart
 * is called. The timer is transitioned to the stopped-state.
 * The current ticks on the timer are SAVED, to remove ticks use TimerStop or
 * TimerReset.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; ID of the timer to pause.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the timer was successfully paused.
 * @retval:   OS_RES_ERROR; if the timer could not be found.
 ******************************************************************************/
void TimerPause(Id_t timer_id);


/******************************************************************************
 * @func: OsResult_t TimerReset(Id_t timer_id)
 *
 * @desc: Resets the timer's current ticks and transitions it to the
 * running state.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; ID of the timer to reset.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the timer was successfully reset.
 * @retval:   OS_RES_ERROR; if the timer could not be found.
 ******************************************************************************/
OsResult_t TimerReset(Id_t timer_id);




/******************************************************************************
 * @func: void TimerStartAll(void)
 *
 * @desc: Starts all timers that exist at the time of the call.
 *
 * @rettype:  N/A
 ******************************************************************************/
void TimerStartAll(void);


/******************************************************************************
 * @func: void TimerStopAll(void)
 *
 * @desc: Stops all timers that exist at the time of the call.
 *
 * @rettype:  N/A
 ******************************************************************************/
void TimerStopAll(void);


/******************************************************************************
 * @func: void TimerResetAll(void)
 *
 * @desc: Reset all timers that exist at the time of the call.
 *
 * @rettype:  N/A
 ******************************************************************************/
void TimerResetAll(void);


/******************************************************************************
 * @func: TmrState_t TimerGetState (Id_t timer_id)
 *
 * @desc: Returns the current state of the timer.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; Timer ID.
 *
 * @rettype:  (TmrState_t) current state
 * @retval:   TIMER_STATE_STOPPED;
 * @retval:   TIMER_STATE_WAITING;
 * @retval:   TIMER_STATE_RUNNING;
 * @retval:   TIMER_STATE_INVALID; if the timer could not be found.
 ******************************************************************************/
TmrState_t TimerStateGet(Id_t timer_id);


/******************************************************************************
 * @func: U32_t TimerTicksGet (Id_t timer_id)
 *
 * @desc: Returns the current value of the timer's counter.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; Timer ID.
 *
 * @rettype:  (U32_t) counter value
 * @retval:   TIMER_INTERVAL_ILLEGAL; if the timer could not be found.
 * @retval:   Other; valid counter value.
 ******************************************************************************/
U32_t TimerTicksGet(Id_t timer_id);



/* TODO: Implement OsResult_t in TimerIntervalSet. */
/******************************************************************************
 * @func: void TimerIntervalSet(Id_t timer_id, U32_t new_interval_us)
 *
 * @desc: Sets a new interval for the timer. The timer is NOT implicitly
 * reset.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; Timer ID.
 * @argin: (U32_t) new_interval_us; New Timer interval in microseconds(us),
 *                                   TIMER_INTERVAL_ILLEGAL (0xFFFFFFFF)
 *                                   is illegal.
 *
 * @rettype:  N/A
 ******************************************************************************/
void TimerIntervalSet(Id_t timer_id, U32_t new_interval_us);


/******************************************************************************
 * @func: U32_t TimerIntervalGet (Id_t timer_id)
 *
 * @desc: Returns the current interval of the timer in us.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; Timer ID.
 *
 * @rettype:  (U32_t) timer interval
 * @retval:   TIMER_INTERVAL_ILLEGAL; if the timer could not be found.
 * @retval:   Other; valid interval.
 ******************************************************************************/
U32_t TimerIntervalGet(Id_t timer_id);


/******************************************************************************
 * @func: U8_t TimerIterationsGet(Id_t timer_id)
 *
 * @desc: Returns the current amount of iterations left on the timer.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; Timer ID.
 *
 * @rettype:  (U8_t) timer iterations
 * @retval:   0; if the timer could not be found.
 * @retval:   1-31; for a valid number of iterations.
 ******************************************************************************/
U8_t TimerIterationsGet(Id_t timer_id);


/******************************************************************************
 * @func: OsResult_t TimerIterationsSet(Id_t timer_id, U8_t iterations)
 *
 * @desc: Sets the amount of iterations left on the timer. The value
 * should be between 1 and 31.
 *
 * Arguments:
 * @argin: (Id_t) timer_id; Timer ID.
 * @argin: (U8_t) iterations; Number of iterations, 1-31.
 *
 * @rettype:  (OsResult_t) Sys call result:
 * @retval:   OS_RES_OK; if the operation was successful.
 * @retval:   OS_RES_ERROR; if the timer could not be found.
 * @retval:   OS_RES_OUT_OF_BOUNDS; if the iteration value was > 31 or 0.
 ******************************************************************************/
OsResult_t TimerIterationsSet(Id_t timer_id, U8_t iterations);


/******************************************************************************
 * @func: U8_t TimerParameterGet (Id_t timer_id)
 *
 * @desc: Returns the current parameter of the timer.
 * Timer parameter bits | 7-3: iterations | 2: auto-reset | 1: periodic | 0: ON |
 * Arguments:
 *
 * @argin: (Id_t) timer_id; Timer ID.
 *
 * @rettype:  (U8_t) Sys call result:
 * @retval:   0xFF; if the timer could not be found.
 * @retval:   Other; valid parameter.
 ******************************************************************************/
U8_t TimerParameterGet(Id_t timer_id);


/* TODO: Implement OsResult_t in TimerParameterSet. */
/******************************************************************************
 * @func: U8_t TimerParameterSet (Id_t timer_id)
 *
 * @desc: Sets a new parameter for the timer.
 * Timer parameter bits | 7-3: iterations | 2: auto-reset | 1: periodic | 0: ON |
 * Arguments:
 *
 * @argin: (Id_t) timer_id; Timer ID.
 * @argin: (U8_t) parameter; New timer parameter. 0xFF is illegal.
 *
 * @rettype:  N/A
 ******************************************************************************/
void TimerParameterSet(Id_t timer_id, U8_t parameter);


#ifdef __cplusplus
}
#endif
#endif /* TIMER_H_ */