/*
 * IdType.c
 *
 *  Created on: 14 mei 2018
 *      Author: Dorus
 */


#include "include/IdType.h"
#include "kernel/inc/IdTypeDef.h"

#include <stdlib.h>
#include <stdbool.h>

#define ID_MASK_GROUP	0xFF000000 /* ID group mask. */
#define ID_MASK_SEQ_NUM	0x00FFFFFF /* Sequence mask. */
#define ID_SHIFT_AMOUNT_GROUP	24

#define GROUP_FROM_ID(id) (IdGroup_t)((U8_t)((id & ID_MASK_GROUP) >> ID_SHIFT_AMOUNT_GROUP))
#define SEQ_NUM_FROM_ID(id) (id & ID_MASK_SEQ_NUM)

#define ID_LIST_EMPTY -1

struct IdPool{
	Id_t free_id;
	bool rollover;
};

struct IdPool IdPools[ID_GROUP_NUM];

void KIdInit(void)
{
	for(U8_t i = 0; i < ID_GROUP_NUM; i++) {
		IdPools[i].free_id = 0 | (U32_t)(i << ID_SHIFT_AMOUNT_GROUP);
		IdPools[i].rollover = false;
	}
}

Id_t KIdRequest(IdGroup_t type)
{
	Id_t id = ID_INVALID;
	U8_t i = 0;

	if(type < ID_GROUP_NUM) { /* If type is within bounds. */
		i = (U8_t)type; /* Map type to index. */

		/* Check if the ID count has rolled over.
		 * Assign the free ID if this is not the case and increment
		 * the new free ID. */
		if(IdPools[i].rollover == false) {
			id = IdPools[i].free_id;
			IdPools[i].free_id++;

			/* Check if the maximum ID has been reached, if true set the roll-over field true. */
			if((IdPools[i].free_id & ID_MASK_SEQ_NUM) == (ID_INVALID & ID_MASK_SEQ_NUM)) {
				IdPools[i].rollover = true;
			}
		}
	}

	return id;
}

Id_t IdSequenceNumberGet(Id_t id)
{
	return SEQ_NUM_FROM_ID(id);
}

IdGroup_t IdGroupGet(Id_t id)
{
	return GROUP_FROM_ID(id);
}

U8_t IdIsInGroup(Id_t id, IdGroup_t group)
{
    return (GROUP_FROM_ID(id) == group ? 1 : 0);
}

void IdListInit(IdList_t *list)
{
    if(list == NULL) {
        return;
    }
    
    list->n = ID_LIST_EMPTY;								
    for(U8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {	
        list->ids[i] = ID_INVALID;			
    }									
}

void IdListIdAdd(IdList_t *list, Id_t id)
{
    if(list == NULL) {
        return;
    }
    
    if(list->n < ID_LIST_SIZE_MAX) {
        list->n++;
        list->ids[list->n] = id;
    }    
}

Id_t IdListIdRemove(IdList_t *list)
{
    if(list == NULL) {
        return ID_INVALID;
    }
    
    Id_t rm_id = ID_INVALID;
    
    if(list->n != ID_LIST_EMPTY) {
        rm_id = list->ids[list->n];
        list->n--;
    }  
    
    return rm_id;
}

S8_t IdListCount(IdList_t *list)
{
    if(list == NULL) {
        return ID_LIST_EMPTY;
    }

    return list->n;
}

void IdListCopy(IdList_t *list_to, IdList_t *list_from)
{
    if(list_to == NULL || list_from == NULL) {
        return;
    }
    list_to->n = list_from->n;
    for(U8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
        list_to->ids[i] = list_from->ids[i];
    }
}
