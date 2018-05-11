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

#define OS_ID_INVALID           0xFFFFFFFF /* Invalid ID definition. */
#define OS_ID_MASK_TYPE         0xFF000000 /* ID type mask. */
#define OS_ID_MASK_UID          0x00FFFFFF /* Unique ID (UID) mask. */

/* Any OS object ID consists of 2 parts; an ID type and a unique ID. */
typedef U32_t Id_t;

typedef enum {
    ID_TYPE_POOL        = 0x00000000,
    ID_TYPE_TASK        = 0x01000000,
    ID_TYPE_TIMER       = 0x02000000,
    ID_TYPE_EVENTGROUP  = 0x03000000,
    ID_TYPE_SEMAPHORE   = 0x04000000,
    ID_TYPE_MAILBOX     = 0x05000000,
    ID_TYPE_RINGBUF     = 0x06000000,
	ID_TYPE_OTHER       = 0xFF000000,
} IdType_t;

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

#endif /* ID_TYPE_H_ */
