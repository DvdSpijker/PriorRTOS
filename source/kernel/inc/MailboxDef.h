/*
 * MailboxDef.h
 *
 * Created: 2-10-2017 12:55:56
 *  Author: Dorus
 */


#ifndef MAILBOX_DEF_H_
#define MAILBOX_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <List.h>

#define MAILBOX_CONFIG_MAX_OWNERS 2

typedef struct Mailbox_t {
    ListNode_t   list_node;

    Id_t            owner_ids[MAILBOX_CONFIG_MAX_OWNERS];
    U8_t            n_owners;
    U8_t            size;
    MailboxBase_t  *buffer;
    U8_t            *pend_counters;
} Mailbox_t;

typedef struct Mailbox_t* pMailbox_t;

OsResult_t KMailboxInit(void);
pMailbox_t KMailboxFromId(Id_t mailbox_ID);

#ifdef __cplusplus
}
#endif
#endif /* MAILBOX_DEF_H_ */