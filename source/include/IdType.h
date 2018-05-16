/*
 * Id.h
 *
 *  Created on: 11 mei 2018
 *      Author: Dorus
 */

#ifndef ID_TYPE_H_
#define ID_TYPE_H_

#include <StdTypes.h>

#define ID_LIST_SIZE_MAX 5

#define ID_INVALID           0xFFFFFFFF /* Invalid ID definition. */

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

	ID_GROUP_NUM, /* Must be the last member of this enum. */
} IdGroup_t;

struct IdList {
	Id_t ids[ID_LIST_SIZE_MAX];
	U8_t n;
};

#define ID_LIST_INIT(p_id_list) {				\
p_id_list->n = 0;								\
for(U8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {	\
	p_id_list->ids[i] = OS_ID_INVALID;			\
}}												\

#define ID_LIST_ID_ADD(p_id_list, id) { \
if(p_id_list->n < ID_LIST_SIZE_MAX) {	\
	p_id_list->ids[n] = id;				\
	p_id_list->n++;						\
}}										\

#define ID_LIST_ID_REMOVE(p_id_list, p_id) {\
*p_id = OS_ID_INVALID;						\
if(p_id_list->n) {							\
	*p_id = p_id_list->ids[n];				\
	p_id_list->n--;							\
}}											\

/******************************************************************************
 * @func: Id_t IdSequenceNumberGet(Id_t id)
 *
 * @desc: Returns the sequence number part of the ID.
 *
 * @argin: (Id_t) id; ID.
 *
 * @rettype:  (Id_t) Sequence number
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
 * @rettype:  (IdGroup_t) ID group
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
 * @rettype:  (U8_t) Check result.
 * @retval:   0; ID is not a member of the group.
 * @retval:   1; ID is a member of the group.
 ******************************************************************************/
U8_t IdIsInGroup(Id_t id, IdGroup_t group);

#endif /* ID_TYPE_H_ */
