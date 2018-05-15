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


/* Request an ID of a specific type. */
Id_t IdRequest(IdGroup_t type);

Id_t IdSequenceNumberGet(Id_t id);

IdGroup_t IdGroupGet(Id_t id);

#endif /* ID_TYPE_H_ */
