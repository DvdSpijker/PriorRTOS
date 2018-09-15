/*
 * EventgroupDef.h
 *
 * Created: 2-10-2017 12:56:30
 *  Author: Dorus
 */


#ifndef EVENTGROUP_DEF_H_
#define EVENTGROUP_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel/inc/List.h"

typedef struct EventGrp_t {
    ListNode_t list_node;

    Id_t         id;
    U8_t         event_reg;
} EventGrp_t;

typedef struct EventGrp_t * pEventGrp_t;

LinkedList_t EventGroupList;

OsResult_t KEventgroupInit(void);


#ifdef __cplusplus
}
#endif
#endif /* EVENTGROUP_DEF_H_ */
