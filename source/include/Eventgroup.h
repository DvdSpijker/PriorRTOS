/******************************************************************************************************************************************
 *  File: Eventgroup.h
 *  Description: Eventgroup API.

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


#ifndef EVENTGROUP_H_
#define EVENTGROUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "include/OsTypes.h"
#include "include/PriorRTOSConfig.h"
#include "include/EventDef.h"

/* Eventgroup flag macros. */
#define EVENTGROUP_FLAG_MASK_0 0x01
#define EVENTGROUP_FLAG_MASK_1 0x02
#define EVENTGROUP_FLAG_MASK_2 0x04
#define EVENTGROUP_FLAG_MASK_3 0x08
#define EVENTGROUP_FLAG_MASK_4 0x10
#define EVENTGROUP_FLAG_MASK_5 0x20
#define EVENTGROUP_FLAG_MASK_6 0x40
#define EVENTGROUP_FLAG_MASK_7 0x80

/* Eventgroup Event macros */
#define EVENTGROUP_EVENT_CREATE         EVENT_TYPE_CREATE
#define EVENTGROUP_EVENT_DELETE         EVENT_TYPE_DELETE

#ifdef PRTOS_CONFIG_USE_EVENTGROUP_EVENT_FLAG_SET
#define EVENTGROUP_EVENT_FLAG_SET(flag_mask)    (EVENT_TYPE_STATE_CHANGE | 0x00001000 | ((U8_t)flag_mask))
#endif

#ifdef PRTOS_CONFIG_USE_EVENTGROUP_EVENT_FLAG_CLEAR
#define EVENTGROUP_EVENT_FLAG_CLEAR(flag_mask)  (EVENT_TYPE_STATE_CHANGE | 0x00002000 | ((U8_t)flag_mask))
#endif

/******************************************************************************
 * @func: Id_t EventgroupCreate(void)
 *
 * @desc: Creates a new eventgroup. Each eventgroup register contains
 * 8 flags that can individually set or cleared.
 *
 * @rettype:  (Id_t); Eventgroup ID
 * @retval:   ID_INVALID; if an error occurred during creation.
 * @retval:   Other; if the eventgroup was successfully created.
 ******************************************************************************/
Id_t EventgroupCreate(void);


/******************************************************************************
 * @func: Id_t EventgroupDelete(Id_t *eventgroup_id)
 *
 * @desc: Deletes an existing eventgroup. The evengroup_id will be set
 * to ID_INVALID if the operation is successful.
 *
 * @argin: (Id_t *) eventgroup_id; Eventgroup ID.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the eventgroup was successfully deleted.
 * @retval:   OS_RES_ERROR; if the eventgroup could not be found.
 ******************************************************************************/
OsResult_t EventgroupDelete(Id_t *eventgroup_id);



/* TODO: Implementation OsResult_t in EventgroupFlagsSet. */
/******************************************************************************
 * @func: void EventgroupFlagsSet(Id_t eventgroup_id, U8_t mask)
 *
 * @desc: Sets the masked flags in the eventgroup register.
 * E.g. if mask = (EVENTGROUP_FLAG_MASK_3 |  EVENTGROUP_FLAG_MASK_7),
 * bit 3 and 7 will be set.
 *
 * @argin: (Id_t) eventgroup_id; Eventgroup ID.
 * @argin: (U8_t) mask; Event flag mask.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the eventgroup was successfully deleted.
 * @retval:   OS_RES_ERROR; if the eventgroup could not be found.
 ******************************************************************************/
void EventgroupFlagsSet(Id_t eventgroup_id, U8_t mask);

/* TODO: Implementation OsResult_t in EventgroupFlagsClear. */
/******************************************************************************
 * @func: OsResult_t EventgroupFlagsClear(Id_t evengroup_id, U8_t mask)
 *
 * @desc: Clears the masked flags in the eventgroup register.
 * E.g. if mask = (EVENTGROUP_FLAG_MASK_0 |  EVENTGROUP_FLAG_MASK_4),
 * bit 0 and 4 will be cleared.
 *
 * @argin: (Id_t) eventgroup_id; Eventgroup ID.
 * @argin: (U8_t) mask; Event flag mask.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the eventgroup was successfully deleted.
 * @retval:   OS_RES_ERROR; if the eventgroup could not be found.
 ******************************************************************************/
OsResult_t EventgroupFlagsClear(Id_t eventgroup_id, U8_t mask);


/******************************************************************************
 * @func: U8_t EventgroupFlagsGet(Id_t evengroup_id, U8_t mask)
 *
 * @desc: Returns the eventgroup flags specified in the mask.
 *
 * @argin: (Id_t) eventgroup_id; Eventgroup ID.
 * @argin: (U8_t) mask; Event flag mask.
 *
 * @rettype:  (U8_t); Eventgroup flags.
 ******************************************************************************/
U8_t EventgroupFlagsGet(Id_t eventgroup_id, U8_t mask);

/******************************************************************************
 * @func: U8_t EventgroupFlagsAreSet(Id_t evengroup_id, U8_t mask)
 *
 * @desc: Returns 1 if the flags specified in the mask are set.
 *
 * @argin: (Id_t) eventgroup_id; Eventgroup ID.
 * @argin: (U8_t) mask; Event flag mask.
 *
 * @rettype:  (U8_t); state
 * @retval:   1; flags are set.
 * @retval:   0; flags are not (all) set.
 ******************************************************************************/
U8_t EventgroupFlagsAreSet(Id_t eventgroup_id, U8_t mask);

OsResult_t EventgroupFlagsRequireCleared(Id_t eventgroup_id, U8_t mask, U32_t timeout);

OsResult_t EventgroupFlagsRequireSet(Id_t eventgroup_id, U8_t mask, U32_t timeout);

#ifdef __cplusplus
}
#endif
#endif /* EVENTGROUP_H_ */
