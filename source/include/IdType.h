/*
 * Id.h
 *
 *  Created on: 11 mei 2018
 *      Author: Dorus
 */

#ifndef ID_TYPE_H_
#define ID_TYPE_H_

#include "StdTypes.h"

#define ID_LIST_SIZE_MAX 5 /* Max. 127 */

#define ID_INVALID			0xFFFFFFFF /* Invalid ID definition. */
#define ID_GROUP_INVALID	0xFF000000
#define ID_SEQ_NUM_MAX		0x00FFFFFF

#define ID_MASK_GROUP		0xFF000000 /* ID group mask. */
#define ID_MASK_SEQ_NUM		0x00FFFFFF /* Sequence mask. */
#define ID_SHIFT_AMOUNT_GROUP	24

#define ID_GROUP_IS_VALID(group) ( (((Id_t)group & ID_MASK_GROUP) != ID_GROUP_INVALID) \
									&& ((((Id_t)group & ID_MASK_GROUP) >> ID_SHIFT_AMOUNT_GROUP) < ID_GROUP_NUM) ) 
#define ID_IS_VALID(id) ( id != ID_INVALID && ID_GROUP_IS_VALID(id) )

#define ID_LIST_EMPTY 0

/* All OS resources are assigned a unique ID.
 * An ID is requested using IdRequest.
 * An ID consists of 2 parts: the ID group and the sequence number. */
typedef U32_t Id_t;

typedef enum {
	ID_GROUP_POOL	= 0,
	ID_GROUP_TASK,
	ID_GROUP_TIMER,
	ID_GROUP_EVENTGROUP,
	ID_GROUP_SEMAPHORE,
	ID_GROUP_MAILBOX,
	ID_GROUP_RINGBUF,
	ID_GROUP_MESSAGE_QUEUE,
	ID_GROUP_NUM, /* Must be the second to last member of this enum. */
	ID_GROUP_INV = 0xFF, /* Must be the last member of this enum. */
} IdGroup_t;

typedef struct {
	Id_t ids[ID_LIST_SIZE_MAX];
	U8_t n;
}IdList_t;

/******************************************************************************
 * @func: Id_t IdSequenceNumberGet(Id_t id)
 *
 * @desc: Returns the sequence number part of the ID.
 *
 * @argin: (Id_t) id; ID.
 *
 * @rettype:  (Id_t); Sequence number
 * @retval:   0x00000000 - 0x00FFFFFF; if the ID is valid.
 ******************************************************************************/
Id_t IdSequenceNumberGet(Id_t id);

/******************************************************************************
 * @func: IdGroup_t IdGroupGet(Id_t id)
 *
 * @desc: Returns the ID group part of the ID.
 *
 * @argin: (Id_t) id; ID.
 *
 * @rettype:  (IdGroup_t); ID group
 * @retval:   ID_GROUP_INV; if the ID is invalid.
 * @retval:   Valid IdGroup_t; if the ID is valid.
 ******************************************************************************/
IdGroup_t IdGroupGet(Id_t id);

/******************************************************************************
 * @func: U8_t IdIsInGroup(Id_t id, IdGroup_t group)
 *
 * @desc: Check whether the ID is a member of the specified group.
 *
 * @argin: (Id_t) id; ID to check.
 * @argin: (IdGroup_t) group; Group to check.
 *
 * @rettype:  (U8_t); Check result.
 * @retval:   0; ID is not a member of the group.
 * @retval:   1; ID is a member of the group.
 ******************************************************************************/
U8_t IdIsInGroup(Id_t id, IdGroup_t group);


void IdListInit(IdList_t *list);
void IdListIdAdd(IdList_t *list, Id_t id);
Id_t IdListIdRemove(IdList_t *list);
U8_t IdListCount(IdList_t *list);
void IdListCopy(IdList_t *list_to, IdList_t *list_from);

#endif /* ID_TYPE_H_ */
