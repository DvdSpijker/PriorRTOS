/*
 * AccessControl.h
 *
 *  Created on: 15 jul. 2018
 *      Author: Dorus
 */

#ifndef ACCESS_CONTROL_H_
#define ACCESS_CONTROL_H_

#include "OsTypes.h"
#include "List.h"

#define ACCESS_PARAMETER_READ	0x01
#define ACCESS_PARAMETER_WRITE	0x02
#define ACCESS_PARAMETER_MODIFY	0x04
#define ACCESS_PARAMETER_DELETE	0x08
#define ACCESS_PARAMETER_ALL	0xFF

typedef struct {
	ListNode_t node;

	Id_t task_id;
	U8_t access_parameter;
} AcaEntry_t;

/* Access Control Attributes */
typedef struct {
	ListNode_t node;
	void *resource;
	LinkedList_t list;
	/* TODO: Add recursive lock. */
} Aca_t, *pAca_t;

pAca_t AcaCreate(void *resource);

OsResult_t AcEntryCreate(pAca_t aca, U8_t access_parameter);

OsResult_t AcEntryModify(pAca_t aca, U8_t access_parameter);

OsResult_t AcEntryDelete(pAca_t aca);

void *AcRightsRequest(pAca_t aca, U8_t access_parameter);



#endif /* ACCESS_CONTROL_H_ */
