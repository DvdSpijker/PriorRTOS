/*
 * Lists.c
 *
 * Created: 10-Dec-16 23:34:42
 *  Author: User
 */


#include "../inc/List.h"
#include <Convert.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

LOG_FILE_NAME("List.c");

#define ID_BUFFER_SLOT_INVALID        0xFF
#define ID_BUFFER_SLOT_OCCUP          0x01
#define ID_BUFFER_SLOT_FREE           0x02

#define LOCK_MASK_CHECKED          0b01000000
#define LOCK_MASK_MODE             0b10000000
#define LOCK_MASK_COUNT            0b00111111
#define LOCK_COUNT_MAX_VALUE       63

#define LOCK_MODE_SET_WRITE(lock) (lock |= LOCK_MASK_MODE)
#define LOCK_MODE_SET_READ(lock) (lock &= ~(LOCK_MASK_MODE))
#define LOCK_CHECKED_BIT_SET(lock) (lock |= LOCK_MASK_CHECKED)
#define LOCK_CHECKED_BIT_GET(lock) ((lock & LOCK_MASK_CHECKED) ? true : false)
#define LOCK_CHECKED_BIT_CLEAR(lock) (lock &= ~(LOCK_MASK_CHECKED))
#define LOCK_COUNT_INC(lock) if((lock & LOCK_MASK_COUNT) < LOCK_COUNT_MAX_VALUE)++lock
#define LOCK_COUNT_DEC(lock) if((lock & LOCK_MASK_COUNT) > 0)--lock
#define LOCK_COUNT_GET(lock)(lock & LOCK_MASK_COUNT)

#define LOCK_MODE_IS_WRITE(lock)((lock & LOCK_MASK_MODE) ? true : false)

#define LOCK_MODE_IS_READ(lock)((lock & LOCK_MASK_MODE) ? true : false)


extern void OsCritSectBegin(void);
extern void OsCritSectEnd(void);

/* Internal functions. */
static void ListIdBufferInit(LinkedList_t *list, IdType_t id_type, Id_t *id_buffer, U8_t id_buffer_size);
static U8_t ListIdBufferSlotGet(Id_t *id_buffer, U8_t id_buffer_size, U8_t slot_type);
static Id_t ListIdBufferFreeIdGet(LinkedList_t *list);
static void ListIdBufferFreeIdAdd(LinkedList_t *list, Id_t id);

static OsResult_t UtilLock(volatile U8_t *lock, U8_t mode);
static OsResult_t UtilUnlock(volatile U8_t *lock, U8_t mode);

/* List Public API. */



void ListInit(LinkedList_t *list, IdType_t id_type, Id_t *id_buffer, U8_t id_buffer_size)
{
    list->head = list->tail = NULL;
    list->lock = 0;
    list->size = 0;
    list->sorted = false;
    list->id_cap_reached =  false;

#ifdef PRTOS_CONFIG_USE_SORTED_LISTS
    list->middle = NULL;
#endif

    ListIdBufferInit(list, id_type, id_buffer, id_buffer_size);
}

OsResult_t ListDestroy(LinkedList_t *list)
{
    if(list == NULL) {
        LOG_ERROR_NEWLINE("Argument is null");
        return OS_NULL_POINTER;
    }

    if(ListIsLocked(list)) {
        return OS_LOCKED;
    }

    /* Note that the list will be locked, but not unlocked.
     * The only function ignoring locks, ListInit, can unlock
     * the list. */

    ListNode_t *rm_node = NULL;
    OsResult_t result = OS_OK;
    for (ListSize_t i = 0; i <= list->size; i++) {
        rm_node = ListNodeRemoveFromHead(list);
        if(rm_node != NULL) {
            result = ListNodeDeinit(list, rm_node);
        }
    }

    ListLock(list, LIST_LOCK_MODE_WRITE);
    list->head = list->tail = NULL;
    list->last_id = 0;
    list->id_type = 0;
    list->size = 0;


    return result;
}

OsResult_t ListLock(LinkedList_t *list, U8_t mode)
{
    if(list == NULL) {
        return OS_NULL_POINTER;
    }

    OsResult_t result = UtilLock(&list->lock, mode);
    /* TODO: Catch OS_LOCKED case and wait for it to unlock. */

    if(result != OS_OK) {
        LOG_ERROR_NEWLINE("Failed to lock list %p in %s mode. Lock val: 0x%02x", list, (mode == 0 ? "READ" : "WRITE"), list->lock);
    }
    return result;
}

OsResult_t ListUnlock(LinkedList_t *list, U8_t mode)
{
    if(list == NULL) {
        return OS_NULL_POINTER;
    }
    OsResult_t result = UtilUnlock(&list->lock, mode);
    /* TODO: Catch OS_LOCKED case and wait for it to unlock. */

    if(result != OS_OK) {
        LOG_ERROR_NEWLINE("Failed to unlock list %p in %s mode. Lock val: 0x%02x", list, (mode == 0 ? "READ" : "WRITE"), list->lock);
    }

    return result;
}

bool ListIsLocked(LinkedList_t *list)
{
    return (list != NULL ? (list->lock > 0 ? true : false) : false);
}

ListSize_t ListSizeGet(LinkedList_t *list)
{
    ListSize_t size  = 0;
    if(ListLock(list, LIST_LOCK_MODE_READ) == OS_OK) {
        size = list->size;
        ListUnlock(list, LIST_LOCK_MODE_READ);
    }
    return (size);
}

OsResult_t ListMerge(LinkedList_t *list_x, LinkedList_t *list_y)
{

    if(list_x == NULL || list_y == NULL) {
        return OS_NULL_POINTER;
    }
    if(ListSizeGet(list_x) == 0) {
        return OS_FAIL;
    }

    OsResult_t result;

    result = ListLock(list_x, LIST_LOCK_MODE_WRITE);
    if(result == OS_OK) {
        result = ListLock(list_y, LIST_LOCK_MODE_WRITE);
        if(result != OS_OK) {
            ListUnlock(list_x, LIST_LOCK_MODE_WRITE);
            return result;
        }
    } else {
        return result;
    }




    if(list_y->size == 0) {
        list_y->head = list_x->head;
    } else {
        list_y->tail->next_node = list_x->head;
        list_x->head->prev_node = list_y->tail;
    }
    list_y->tail = list_x->tail;
    list_y->size += list_x->size;

    ListUnlock(list_x, LIST_LOCK_MODE_WRITE);
    ListUnlock(list_y, LIST_LOCK_MODE_WRITE);

    result = ListDestroy(list_x);

#if PRTOS_CONFIG_ENABLE_LIST_INTEGRITY_VERIFICATION==1
    ListIntegrityVerify(list_x);
    ListIntegrityVerify(list_y);
#endif


    return result;
}

ListNode_t *ListSearchLinear(LinkedList_t *list, Id_t id)
{
    if(ListLock(list, LIST_LOCK_MODE_READ) == OS_OK) {
        struct ListIterator it;

        LIST_ITERATOR_BEGIN(&it, list, LIST_ITERATOR_DIRECTION_FORWARD);
        {
            if (it.current_node->id == id) {
                ListUnlock(list, LIST_LOCK_MODE_READ);
                return it.current_node;
            }
        }
        LIST_ITERATOR_END(&it);
        ListUnlock(list, LIST_LOCK_MODE_READ);
    }
    // LOG_DEBUG_NEWLINE("Search it: %u", cnt);
    return NULL;
}

ListNode_t *ListSearch(LinkedList_t *list, Id_t id)
{
    if(ListLock(list, LIST_LOCK_MODE_READ) == OS_OK) {
        struct ListIterator it;
        U8_t search_dir = LIST_ITERATOR_DIRECTION_FORWARD; /* Default it direction. */

#ifdef PRTOS_CONFIG_USE_SORTED_LISTS
        /* If the list is sorted and has an assigned middle,
         * check if the to-search ID lies in the second half
         * of the list. If this is true, reverse the search direction. */
        if(list->sorted == true && list->middle != NULL) {
            if(id >= list->middle->id) {
                search_dir = LIST_ITERATOR_DIRECTION_REVERSE;
            }
        }
#endif
        LIST_ITERATOR_BEGIN(&it, list, search_dir) {
            if (it.current_node->id == id) {
                ListUnlock(list, LIST_LOCK_MODE_READ);
                return it.current_node;
            }
#ifdef PRTOS_CONFIG_USE_SORTED_LISTS
            /* When the list has a middle, it is easy
             * to determine when the to-search ID is not present in the
             * list. */
            if(list->sorted == true && list->middle != NULL && it.next_node != NULL) {
                if((id < it.next_node->id) && search_dir == LIST_ITERATOR_DIRECTION_FORWARD) {
                    break;
                } else if((id > it.next_node->id) && search_dir == LIST_ITERATOR_DIRECTION_REVERSE) {
                    break;
                }
            }
#endif
        }
        LIST_ITERATOR_END(&it);
        ListUnlock(list, LIST_LOCK_MODE_READ);
    }
    return NULL;
}

OsResult_t ListNodeInit(ListNode_t *node, void *child)
{

    if(node == NULL) {
        LOG_ERROR_NEWLINE("Could not initialize node.");
        return OS_NULL_POINTER;
    }

    node->next_node = node->prev_node = NULL;
    node->child = child;
    node->lock = 0;
    node->id = INVALID_ID;
    return OS_OK;
}

OsResult_t ListNodeDeinit(LinkedList_t *list, ListNode_t *node)
{
    ListNode_t *rm_node = node;

    ListIdBufferFreeIdAdd(list, rm_node->id);

    if((rm_node->next_node != NULL) || (rm_node->prev_node != NULL)) {
        rm_node = ListNodeRemove(list, rm_node);
    }
    rm_node->id = 0;
    rm_node->child = NULL;
    rm_node->next_node = rm_node->prev_node = NULL;
    return OS_OK;
}

OsResult_t ListNodeLock(ListNode_t *node, U8_t mode)
{
    if(node == NULL) {
        return OS_NULL_POINTER;
    }
    OsResult_t result = UtilLock(&node->lock, mode);
    /* TODO: Catch OS_LOCKED case and wait for it to unlock. */
    if(result == OS_LOCKED) {
        LOG_ERROR_NEWLINE("Access to node %04x denied;locked", node->id);
    }
    return result;
}

OsResult_t ListNodeUnlock(ListNode_t *node, U8_t mode)
{
    if(node == NULL) {
        return OS_NULL_POINTER;
    }
    /* TODO: Catch OS_LOCKED case and wait for it to unlock. */
    return (UtilUnlock(&node->lock, mode));
}




OsResult_t ListNodeAddAtPosition(LinkedList_t *list, ListNode_t *node, U8_t position)
{

    if(ListSizeGet(list) == LIST_SIZE_MAX) {
        LOG_ERROR_NEWLINE("List (%p) has reached its max capacity.", list);
        return OS_FAIL;
    }

    if(node == NULL || list == NULL) {
        LOG_ERROR_NEWLINE("List or node is NULL!");
        while(1);
    }

    OsResult_t result;
    if(ListLock(list, LIST_LOCK_MODE_WRITE) == OS_OK) {
        if(node->id == INVALID_ID) {
            node->id = ListIdBufferFreeIdGet(list);
            if(node->id == INVALID_ID) {
                LOG_ERROR_NEWLINE("No free ID found for node (%p) in list (%p).", node, list);
                result = OS_INVALID_ID;
                goto unlock;
            }
        }

        if(list->size == 0) { /* In case the size 0, the newly added node will head and tail. */
            list->head = list->tail = node;
            node->prev_node = NULL;
            node->next_node = NULL;
        } else if(position == LIST_POSITION_HEAD) {
            list->head->prev_node = node;
            node->next_node = list->head;
            list->head = node;
            node->prev_node = NULL;
        } else if(position == LIST_POSITION_TAIL) {
            list->tail->next_node = node;
            node->prev_node = list->tail;
            list->tail = node;
            node->next_node = NULL;
        }


        list->size++;
        result = OS_OK;
        list->sorted = false;

unlock:
        ListUnlock(list, LIST_LOCK_MODE_WRITE);
    }

#if PRTOS_CONFIG_ENABLE_LIST_INTEGRITY_VERIFICATION==1
    ListIntegrityVerify(list);
#endif

    return result;
}

OsResult_t ListNodeAddAtNode(LinkedList_t *list, ListNode_t *node_y, ListNode_t *node_x, U8_t before_after)
{
    if(ListSizeGet(list) == LIST_SIZE_MAX) {
        LOG_ERROR_NEWLINE("List (%p) has reached its max capacity.", list);
        return OS_FAIL;
    }
    OsResult_t result;
    if(ListLock(list, LIST_LOCK_MODE_WRITE) == OS_OK) {
        if(node_y->id == INVALID_ID) {
            node_y->id = ListIdBufferFreeIdGet(list);
            if(node_y->id == INVALID_ID) {
                LOG_ERROR_NEWLINE("No free ID found for node (%p) in list (%p).", node_y, list);
                result = OS_INVALID_ID;
                goto unlock;
            }
        }

        ListNode_t *node_z;
        if(node_x == NULL) {
            if(list->size == 0) {
                goto insert_first;
            } else {
                goto insert_head;
            }
        } else if(node_x == list->head && before_after == LIST_ADD_BEFORE) {
            goto insert_head;
        } else if(node_x == list->tail && before_after == LIST_ADD_AFTER) {
            goto insert_tail;
        }

        if(list->size == 0 ) {
insert_first:
            list->head = list->tail = node_y;
            node_y->prev_node = NULL;
            node_y->next_node = NULL;
        } else if(node_x == list->head) {
insert_head:
            if(before_after == LIST_ADD_BEFORE) {
                list->head->prev_node = node_y;
                node_y->next_node = list->head;
                list->head = node_y;
                node_y->prev_node = NULL;
            } else {
                goto insert_after;
            }
        } else if (node_x == list->tail) {
insert_tail:
            if(before_after == LIST_ADD_BEFORE) {
                goto insert_before;
            } else {
                node_x->next_node = node_y;
                node_y->next_node = NULL;
                node_y->prev_node = node_x;
                list->tail = node_y;
            }

        } else {
            if(before_after == LIST_ADD_BEFORE) {
insert_before:
                node_z = node_x->prev_node;

                node_x->prev_node = node_y;

                if(node_z != NULL) {
                    node_z->next_node = node_y;
                }

                node_y->next_node = node_x;
                node_y->prev_node = node_z;


            } else {
insert_after:
                node_z = node_x->next_node;
                node_x->next_node = node_y;
                if(node_z != NULL) {
                    node_z->prev_node = node_y;
                }

                node_y->next_node = node_z;
                node_y->prev_node = node_x;
            }

        }

        list->size++;
        result = OS_OK;
        list->sorted = false;
unlock:
        ListUnlock(list, LIST_LOCK_MODE_WRITE);
    }

#if PRTOS_CONFIG_ENABLE_LIST_INTEGRITY_VERIFICATION==1
    ListIntegrityVerify(list);
#endif
    return result;
}


OsResult_t ListNodeAddSorted(LinkedList_t *list, ListNode_t *node)
{

    if(ListSizeGet(list) == LIST_SIZE_MAX) {
        LOG_ERROR_NEWLINE("List (%p) has reached its max capacity.", list);
        return OS_FAIL;
    }
    OsResult_t result;

#ifndef PRTOS_CONFIG_USE_SORTED_LISTS
    result = ListNodeAddAtPosition(list, node, LIST_POSITION_TAIL);
#else
    if(list->sorted == false && ListSizeGet(list) != 0) { /* We only care about sorted state if the list is not empty. */
        LOG_ERROR_NEWLINE("List %p is not in sorted state. Adding node %p at list tail.", list, node);
        ListNodeAddAtPosition(list, node, LIST_POSITION_TAIL);
        return OS_OK;
    }

    result = ListLock(list, LIST_LOCK_MODE_WRITE);
    if(result == OS_OK) {
        result = OS_LOCKED;
        struct ListIterator it;
        bool stop_loop = false;

        /* Acquire ID if necessary. */
        if(node->id == INVALID_ID) {
            node->id = ListIdBufferFreeIdGet(list);
            if(node->id == INVALID_ID) {
                LOG_ERROR_NEWLINE("No free ID found for node (%p) in list (%p).", node, list);
                result = OS_INVALID_ID;
                goto unlock;
            }
        }

        /* Iterate through list. */
        LIST_ITERATOR_BEGIN(&it, list, LIST_ITERATOR_DIRECTION_FORWARD) {
            if(it.current_node != NULL) {
                if(node->id > it.current_node->id) {
                    if(it.next_node != NULL) {
                        if(node->id < it.next_node->id) { /* Add in between two nodes? */
                            /* Insert after current node. */
                            result = ListNodeAddAtNode(list, node, it.current_node, LIST_ADD_AFTER);
                            stop_loop = true;
                        }
                    } else { /* If next node is null the end of the list has been reached. */
                        result = ListNodeAddAtNode(list, node, it.current_node, LIST_ADD_AFTER);
                        stop_loop = true;
                    }
                } else if(node->id < it.current_node->id) { /* Add before the head of the list. New node ID is now lowest ID. */
                    result = ListNodeAddAtNode(list, node, it.current_node, LIST_ADD_BEFORE);
                    stop_loop = true;
                } else if(node->id == it.current_node->id) { /* ID is the same somehow. */
                    /* TODO: Throw exception. */
                    result = OS_ERROR;
                    stop_loop = true;
                }
            } else { /* New node will likely be the first node. */
                if(it.list->size == 0) {
                    result = ListNodeAddAtPosition(list, node, LIST_POSITION_HEAD);
                    if(result == OS_OK) {
                        list->middle = node;
                    }
                } else {
                    result = OS_ERROR;
                }
                stop_loop = true;
            }

            LIST_ITERATOR_BREAK_ON_CONDITION(stop_loop);
        }
        LIST_ITERATOR_END(&it);
        if(result == OS_OK) {
            /* If added node has higher ID and the list has a middle.
             * Move the middle node pointer 1 node forward.
             * If added node has lower ID and the list has a middle.
             * Move the middle node pointer 1 node backward. */
            if(list->middle != NULL) {
                if((node->id > list->middle->id) && (ListSizeGet(list) % 2)) {
                    list->middle = list->middle->next_node;
                } else if((node->id < list->middle->id) && (ListSizeGet(list) % 2)) {
                    list->middle = list->middle->prev_node;
                }
            }
            list->sorted = true;
        }
unlock:
        ListUnlock(list, LIST_LOCK_MODE_WRITE);
    }
#endif

    return result;
}

ListNode_t *ListNodeRemove(LinkedList_t *list, ListNode_t *node)
{
    /* TODO: Fix possible bug in ListNodeRemove. */
    if(ListIsLocked(list)) {
        return NULL;
    }
    if(list == NULL || node == NULL) {
        return NULL;
    }
    ListNode_t *x = NULL, *y = NULL, *z = NULL;

    if(ListLock(list, LIST_LOCK_MODE_WRITE) == OS_OK) {

        y = node;
        x = y->prev_node;
        z = y->next_node;
        if(list->size == 2) {
            if(x != NULL) {
                list->head = list->tail = x;
                x->next_node = NULL;
                x->prev_node = NULL;
            } else {
                list->head = list->tail = z;
                z->next_node = NULL;
                z->prev_node = NULL;
            }
        } else if(list->size == 1) {
            list->head = list->tail = NULL;
        } else {
            if(y == list->head) { /* Case y = head. */
                list->head = z;
                z->prev_node = NULL;
            } else if (y == list->tail) { /* Case y = tail. */
                list->tail = x;
                x->next_node = NULL;
            } else { /* Case y is in the middle somewhere. */
                x->next_node = z;
                z->prev_node = x;
            }
        }

        list->size--;

#ifdef PRTOS_CONFIG_USE_SORTED_LISTS
        if(list->middle == y) {
            if(ListSizeGet(list) % 2) {
                list->middle = z;
            }
        }
#endif

        y->prev_node = y->next_node = NULL;

        ListUnlock(list, LIST_LOCK_MODE_WRITE);
    }

#if PRTOS_CONFIG_ENABLE_LIST_INTEGRITY_VERIFICATION==1
    ListIntegrityVerify(list);
#endif

    return y;
}

ListNode_t *ListNodeRemoveFromHead(LinkedList_t *list)
{
    return (ListNodeRemove(list, list->head));
}

ListNode_t *ListNodeRemoveFromTail(LinkedList_t *list)
{
    return (ListNodeRemove(list, list->tail));
}


OsResult_t ListNodeMove(LinkedList_t *list_x, LinkedList_t *list_y, ListNode_t *node)
{
    OsCritSectBegin();
    OsResult_t result = OS_ERROR;
    ListNode_t *mv_node = ListNodeRemove(list_x, node);
    if(mv_node != NULL) {
        result = ListNodeAddSorted(list_y, node); /* TODO: Replace with AddSorted. */
    }
    OsCritSectEnd();
    return result;
}




void *ListNodeChildFromId(LinkedList_t *list, Id_t id)
{
    ListNode_t *node = ListSearch(list, id);
    return (node != NULL ? node->child : NULL);
}


OsResult_t ListNodeSwap(LinkedList_t *list, ListNode_t *node_x, ListNode_t *node_y)
{
    if(list == NULL || node_x == NULL || node_y == NULL) {
        return OS_NULL_POINTER;
    }

    if(ListIsLocked(list)) {
        return OS_LOCKED;
    }

    if (list->size < 2) {
        return OS_FAIL;
    }


    if(ListLock(list, LIST_LOCK_MODE_WRITE) == OS_OK) {
        ListNode_t *x_p, *x, *x_n, *y_p, *y, *y_n; /* x_p = x previous, x = Node x, x_n = x next, etc. */

        x = node_x;
        y = node_y;

        x_p = x->prev_node;
        x_n = x->next_node;

        y_p = y->prev_node;
        y_n = y->next_node;

        /* Assign previous and next nodes and test for head and tail positions
         * since these also need to be swapped if true. */
        if(x_p != NULL) {
            x_p->next_node = y;
        } else {
            list->head = y;
        }

        if(x_n != NULL) {
            x_n->prev_node = y;
        } else {
            list->tail = y;
        }

        y->prev_node = x_p;
        y->next_node = x_n;


        if(y_p != NULL) {
            y_p->next_node = x;
        } else {
            list->head = x;
        }

        if(y_n != NULL) {
            y_n->prev_node = x;
        } else {
            list->tail = x;
        }

        x->prev_node = y_p;
        x->next_node = y_n;

        list->sorted = false;

        ListUnlock(list, LIST_LOCK_MODE_WRITE);
    }


#if PRTOS_CONFIG_ENABLE_LIST_INTEGRITY_VERIFICATION==1
    ListIntegrityVerify(list);
#endif

    return OS_OK;
}

ListNode_t *ListNodePeek(LinkedList_t *list, U8_t position)
{
    if(list == NULL) {
        return NULL;
    }

    if(position == LIST_POSITION_HEAD) {
        return list->head;
    } else if (position == LIST_POSITION_TAIL) {
        return list->tail;
    } else {
        return NULL;
    }
}

ListNode_t *ListNodePeekNext(ListNode_t *node)
{
    if(node != NULL) {
        return (node->next_node);
    }

    return NULL;
}

ListNode_t *ListNodePeekPrev(ListNode_t *node)
{
    if(node != NULL) {
        return (node->prev_node);
    }

    return NULL;
}

bool ListNodeHasNext(ListNode_t *node)
{
    return (node != NULL ? (node->next_node != NULL ? true : false) : false);
}

bool ListNodeHasPrev(ListNode_t *node)
{
    return (node != NULL ? (node->prev_node != NULL ? true : false) : false);
}



void *ListNodeChildGet(ListNode_t *node)
{
    return(node != NULL ? node->child : NULL);
}

OsResult_t ListNodeChildSet(ListNode_t *node, void *child)
{
    if (node == NULL || child == NULL) {
        return OS_NULL_POINTER;
    }
    if(ListNodeLock(node, LIST_LOCK_MODE_WRITE) == OS_OK) {
        node->child = child;
        ListNodeUnlock(node, LIST_LOCK_MODE_WRITE);
        return OS_OK;
    } else {
        return OS_LOCKED;
    }

}

Id_t ListNodeIdGet(ListNode_t *node)
{
    if(node != NULL) {
        return node->id;
    }

    return INVALID_ID;
}

bool ListNodeIsInList(LinkedList_t *list, ListNode_t *node)
{
    return ((node->next_node != NULL || node->prev_node != NULL) || (list->size == 1 && list->head == node));
}

/* List Integrity API */

S8_t ListIntegrityVerify(LinkedList_t *list)
{
    if(list == NULL) {
        return LIST_INTEGRITY_RESULT_NULL;
    }

    /* Note that this function will locked the list indefinitely if corruptions are
     * found. Only ListIntegrityRestore can access the list and unlock it if restoration
     * succeeds. */
    if(ListIsLocked(list)) {
        return LIST_INTEGRITY_RESULT_LOCKED;
    }

    if(ListLock(list, LIST_LOCK_MODE_READ) != OS_OK) {
        return LIST_INTEGRITY_RESULT_LOCKED;
    }
    S8_t result = LIST_INTEGRITY_RESULT_LIST_INTACT;

    ListSize_t list_size = ListSizeGet(list);
    if(list_size == 0) {
        result = LIST_INTEGRITY_RESULT_NO_ELEMENTS;
        goto unlock;
    }


    /* Check head and tail for correct boundary pointers. */
    if(list->head->prev_node != NULL) {
        result = LIST_INTEGRITY_RESULT_CORRUPT_HEAD;
        goto ret;
    } else if(list->head->next_node == NULL && list->size > 1) {
        result = LIST_INTEGRITY_RESULT_CORRUPT_HEAD;
        goto ret;
    } else if(list->tail->next_node != NULL) {
        result = LIST_INTEGRITY_RESULT_CORRUPT_TAIL;
        goto ret;
    } else if(list->tail->prev_node == NULL && list->size > 1) {
        result = LIST_INTEGRITY_RESULT_CORRUPT_TAIL;
        goto ret;
    }

    /* Navigate through the list from head to tail. */
    ListNode_t *node = ListNodePeek(list, LIST_POSITION_HEAD);
    if(node != NULL) {
        for (U8_t i = 1; i < (list_size); i++) {
            node = ListNodePeekNext(node);
            if(node == NULL) {
                result = LIST_INTEGRITY_RESULT_CORRUPT_NAV_FW;
                break;
            }
        }
    } else {
        if(list->size != 0) {
            result = LIST_INTEGRITY_RESULT_CORRUPT_NAV_FW;
        }
    }


    node = ListNodePeek(list, LIST_POSITION_TAIL);
    for (U8_t i = 1; i < (list_size); i++) {
        node = ListNodePeekPrev(node);
        if(node == NULL) {
            if(result == LIST_INTEGRITY_RESULT_CORRUPT_NAV_FW) {
                result = LIST_INTEGRITY_RESULT_CORRUPT_NAV_BOTH;
            } else {
                result = LIST_INTEGRITY_RESULT_CORRUPT_NAV_BW;
            }
            break;
        }
    }

    if(result >= LIST_INTEGRITY_RESULT_LIST_INTACT) {
        goto unlock;
    } else {
        goto ret;
    }

unlock:
    ListUnlock(list, LIST_LOCK_MODE_READ);

ret:
    if(result < LIST_INTEGRITY_RESULT_LIST_INTACT) {
        LOG_ERROR_APPEND( "\tError in list %p : %d", list, result);
    }
    return result;
}




/**** List Iterator API ****/


OsResult_t ListIteratorInit(struct ListIterator *list_it, LinkedList_t *list, U8_t it_direction)
{
    if(list_it == NULL || list == NULL) {
        return OS_NULL_POINTER;
    }
    if(it_direction != LIST_ITERATOR_DIRECTION_FORWARD && it_direction != LIST_ITERATOR_DIRECTION_REVERSE) {
        return OS_INVALID_VALUE;
    }

    if(ListLock(list, LIST_LOCK_MODE_READ) == OS_OK) {
        list_it->list = list;
        list_it->direction = it_direction;
        if(it_direction == LIST_ITERATOR_DIRECTION_FORWARD) {
            list_it->current_node = list->head;
            if(list_it->current_node != NULL) {
                list_it->next_node = list_it->current_node->next_node;
                list_it->prev_node = list_it->current_node->prev_node;
            }
            list_it->current_position = 0;
        } else {
            list_it->current_node = list->tail;
            if(list_it->current_node != NULL) {
                list_it->next_node = list_it->current_node->prev_node;
                list_it->prev_node = list_it->current_node->next_node;
            }
            list_it->current_position = (list->size - 1);
        }

        ListUnlock(list, LIST_LOCK_MODE_READ);
        return OS_OK;
    }

    return OS_LOCKED;
}

ListNode_t *ListIteratorNext(struct ListIterator *list_it)
{
    ListNode_t *node = NULL;
    if(list_it != NULL) {
        if(list_it->current_node != NULL) {
            if(list_it->direction == LIST_ITERATOR_DIRECTION_FORWARD) { /* Move forward i.e. towards tail. */
                list_it->prev_node = list_it->current_node;
                list_it->current_node = list_it->next_node;
                list_it->next_node = list_it->current_node->next_node;
                node = list_it->current_node;
                list_it->current_position++;
            } else {
                list_it->prev_node = list_it->current_node;
                list_it->current_node = list_it->next_node;
                list_it->next_node = list_it->current_node->prev_node;
                node = list_it->current_node;
                list_it->current_position--;
            }
        }
    }
    return node;
}

ListNode_t *ListIteratorPrev(struct ListIterator *list_it)
{
    ListNode_t *node = NULL;
    if(list_it != NULL) {
        if(list_it->current_node != NULL) {
            if(list_it->direction == LIST_ITERATOR_DIRECTION_FORWARD) { /* Move forward i.e. towards head. */
                list_it->next_node = list_it->current_node;
                list_it->current_node = list_it->prev_node;
                list_it->prev_node = list_it->current_node->prev_node;
                node = list_it->current_node;
                list_it->current_position--;
            } else {
                list_it->next_node = list_it->current_node;
                list_it->current_node = list_it->next_node;
                list_it->prev_node = list_it->current_node->next_node;
                node = list_it->current_node;
                list_it->current_position++;
            }

        }
    }
    return node;
}

bool ListIteratorEnd(struct ListIterator *list_it)
{
    return ((list_it != NULL) ? ((list_it->current_node == NULL) ? true : false) : false);
}



/**** ID buffering API ****/


static void ListIdBufferInit(LinkedList_t *list, IdType_t id_type, Id_t *id_buffer, U8_t id_buffer_size)
{
    list->id_type = id_type;
    list->id_buf = id_buffer;
    list->id_cap_reached = false;
    list->id_buf_size = id_buffer_size;
    list->last_id = (id_type | 0x0000);
    if(list->id_buf != NULL) {
        for (U8_t i = 0; i < id_buffer_size; i++) {
            id_buffer[i] = INVALID_ID;
        }
    }
    while(ListIdBufferFillCycle(list) == OS_OK); /* Fill buffer with IDs. */
}


static U8_t ListIdBufferSlotGet(Id_t *id_buffer, U8_t id_buffer_size, U8_t slot_type)
{
    U8_t found = 0;
    U8_t i = 0;
    for (; i < id_buffer_size; i++) {
        if(id_buffer[i] == INVALID_ID && slot_type == ID_BUFFER_SLOT_FREE) {
            found = 1;
            break;
        } else if (id_buffer[i] != INVALID_ID && slot_type == ID_BUFFER_SLOT_OCCUP) {
            found = 1;
            break;
        }

    }

    if(found == 1) {
        return i;
    } else {
        return ID_BUFFER_SLOT_INVALID;
    }
}

static void ListIdBufferFreeIdAdd(LinkedList_t *list, Id_t id)
{
    // ListLock(list);

    Id_t *id_buffer = list->id_buf;

    if(list->n_id_avail == list->id_buf_size) { /* Buffer is full. */
        goto unlock;
    }


    U8_t slot = ListIdBufferSlotGet(id_buffer, list->id_buf_size, ID_BUFFER_SLOT_FREE);
    if(slot != ID_BUFFER_SLOT_INVALID) {
        id_buffer[slot] = id;
        list->n_id_avail++;
    }

unlock:
    //ListUnlock(list);

    return;
}


static Id_t ListIdBufferFreeIdGet(LinkedList_t *list)
{
    //ListLock(list);

    Id_t free_id = INVALID_ID;

    if (list->n_id_avail == 0) {
        if(ListIdBufferFillCycle(list) != OS_OK) {
            goto unlock;
        }
    }

    U8_t slot = ListIdBufferSlotGet(list->id_buf, list->id_buf_size, ID_BUFFER_SLOT_OCCUP);
    if(slot != ID_BUFFER_SLOT_INVALID) {
        free_id = list->id_buf[slot];
        list->id_buf[slot] = INVALID_ID;
        list->n_id_avail--;
    }

unlock:
    // ListUnlock(list);


    return free_id;

}

OsResult_t ListIdBufferFillCycle(LinkedList_t *list)
{
    OsResult_t status = OS_FAIL;
    // ListLock(list);

    Id_t *id_buffer = list->id_buf;

    if(id_buffer != NULL) {
        if(list->n_id_avail == list->id_buf_size) { /* Buffer is full. */
            goto unlock;
        }

        U8_t slot = ListIdBufferSlotGet(id_buffer, list->id_buf_size, ID_BUFFER_SLOT_FREE);
        if(slot != ID_BUFFER_SLOT_INVALID) { /* If a slot if available. */
            if(list->id_cap_reached == false) { /* Check for ID cap. */
                list->last_id++;
                if((list->last_id & ID_MASK_UID) == ID_MASK_UID) {
                    list->id_cap_reached = true;
                }
            } else { /* ID cap was reached. */
                /* TODO: Add search for ID function. */
            }
            id_buffer[slot] = list->last_id;
            list->n_id_avail++;
            status = OS_OK;
        } else {
            status = OS_ERROR;
        }
    }



unlock:
    // ListUnlock(list);

    return status;
}

const char *PrintToBufferText[] = {
    "\nList:\t",
    "Size:\t",
    "Lock:\t",
    "Nodes:",
    "\n\tID:",
};


char *ListPrintToBuffer(LinkedList_t *list,  U32_t *buffer_size)
{
    if(list == NULL || buffer_size == NULL) {
        return NULL;
    }
    if(ListLock(list, LIST_LOCK_MODE_READ) != OS_OK) {
        *buffer_size = 0;
        return NULL;
    }

    struct ListIterator it;
    U32_t buffer_write_offset = 0;
    *buffer_size = sizeof(PrintToBufferText[0] + sizeof(list))
                   + sizeof(PrintToBufferText[1] + sizeof(list->size))
                   + sizeof(PrintToBufferText[2] + sizeof(list->lock))
                   + sizeof(PrintToBufferText[3])
                   + (sizeof(PrintToBufferText[4] + sizeof(Id_t))  * list->size);
    *buffer_size *= 3;
    char *format_buffer = (char*)malloc(*buffer_size);
    if(format_buffer != NULL) {
        buffer_write_offset += sprintf(&format_buffer[buffer_write_offset], "%s%p\n", PrintToBufferText[0], list);
        buffer_write_offset += sprintf(&format_buffer[buffer_write_offset], "%s%u\n", PrintToBufferText[1], list->size);
        buffer_write_offset += sprintf(&format_buffer[buffer_write_offset], "%s%02x\n", PrintToBufferText[2], list->lock);
        buffer_write_offset += sprintf(&format_buffer[buffer_write_offset], "%s", PrintToBufferText[3]);
        LIST_ITERATOR_BEGIN(&it, list, LIST_ITERATOR_DIRECTION_FORWARD) {
            if(it.current_node != NULL) {
                buffer_write_offset += sprintf(&format_buffer[buffer_write_offset], "%s%04x", PrintToBufferText[4], it.current_node->id);
            }
        }
        LIST_ITERATOR_END(&it);
    }


    return format_buffer;
}



static OsResult_t UtilLock(volatile U8_t *lock, U8_t mode)
{
    OsCritSectBegin();
    OsResult_t result = OS_ERROR;

    if(mode == LIST_LOCK_MODE_READ) {

        /* If not locked, check value of the lock counter.
         * If the counter has not reached its max. value
         * increment, otherwise return. */
        if(LOCK_COUNT_GET(*lock) == LOCK_COUNT_MAX_VALUE) {
            result = OS_LOCKED;
            goto exit;
        } else {
            LOCK_COUNT_INC(*lock);
            result = OS_OK;
            goto exit;
        }
    } else if(mode == LIST_LOCK_MODE_WRITE) {

        if(LOCK_CHECKED_BIT_GET(*lock)) {
            if(LOCK_COUNT_GET(*lock) == LOCK_COUNT_MAX_VALUE) {
                result = OS_LOCKED;
                goto exit;
            } else {
                LOCK_COUNT_INC(*lock);
                result = OS_OK;
                goto exit;
            }
        } else {
            if(LOCK_COUNT_GET(*lock) == LOCK_COUNT_MAX_VALUE) {
                result = OS_LOCKED;
                goto exit;
            } else {
                /* Locking in write mode is not allowed when
                 * the lock is in read mode and the counter != 0,
                 * since there are active readers which are not
                 * protected by a critical section. */
                if(LOCK_MODE_IS_READ(*lock) && LOCK_COUNT_GET(*lock) != 0) {
                    result = OS_LOCKED;
                    goto exit;
                }
                LOCK_MODE_SET_WRITE(*lock);
                LOCK_CHECKED_BIT_SET(*lock);
                LOCK_COUNT_INC(*lock);
                OsCritSectBegin();
                result = OS_OK;
                goto exit;
            }
        }
    } else { /* Invalid mode. */
        result = OS_ERROR;
    }

exit:
    OsCritSectEnd();
    return result;
}


static OsResult_t UtilUnlock(volatile U8_t *lock, U8_t mode)
{
    OsCritSectBegin();
    OsResult_t result = OS_ERROR;

    if(mode == LIST_LOCK_MODE_READ || mode == LIST_LOCK_MODE_WRITE) {
        LOCK_COUNT_DEC(*lock);
        if(LOCK_COUNT_GET(*lock) == 0) {
            LOCK_CHECKED_BIT_CLEAR(*lock);
            LOCK_MODE_SET_READ(*lock);
            OsCritSectEnd();
        }
        result = OS_OK;
    } else { /* Invalid mode. */
        result = OS_ERROR;
    }

    OsCritSectEnd();
    return result;
}