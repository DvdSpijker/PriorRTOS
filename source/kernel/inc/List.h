/******************************************************************************************************************************************
 *  File: List.h
 *  Description: Generic Linked List API.

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


#ifndef LIST_H_
#define LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <OsTypes.h>
#include <CoreDef.h>

#define LIST_ITERATOR_DIRECTION_FORWARD 0x00
#define LIST_ITERATOR_DIRECTION_REVERSE 0x01

#define LIST_POSITION_HEAD 0x00
#define LIST_POSITION_TAIL 0x01

#define LIST_ADD_BEFORE 0x00
#define LIST_ADD_AFTER  0x01

#define LIST_LOCK_MODE_READ  0x00
#define LIST_LOCK_MODE_WRITE 0x01

#define LIST_INTEGRITY_RESULT_NO_ELEMENTS        3
#define LIST_INTEGRITY_RESULT_LOCKED             2
#define LIST_INTEGRITY_RESULT_NULL               1
#define LIST_INTEGRITY_RESULT_LIST_INTACT        0
#define LIST_INTEGRITY_RESULT_CORRUPT_NAV_FW    -1
#define LIST_INTEGRITY_RESULT_CORRUPT_NAV_BW    -2
#define LIST_INTEGRITY_RESULT_CORRUPT_NAV_BOTH  -3
#define LIST_INTEGRITY_RESULT_CORRUPT_HEAD      -4
#define LIST_INTEGRITY_RESULT_CORRUPT_TAIL      -5


//#define LIST_EVENT_LOCK         (EVENT_TYPE_STATE_CHANGE | 0x00001000)
//#define LIST_EVENT_UNLOCK       (EVENT_TYPE_STATE_CHANGE | 0x00002000)
//#define LIST_EVENT_LOCK_FAIL    (EVENT_TYPE_STATE_CHANGE | 0x00003000)
//
///* TODO: Implement these events. */
//#define LIST_EVENT_NODE_LOCK    (EVENT_TYPE_STATE_CHANGE | 0x00004000)
//#define LIST_EVENT_NODE_UNLOCK  (EVENT_TYPE_STATE_CHANGE | 0x00005000)
//#define LIST_EVENT_NODE_ADD     (EVENT_TYPE_STATE_CHANGE | 0x00006000)
//#define LIST_EVENT_NODE_REMOVE  (EVENT_TYPE_STATE_CHANGE | 0x00007000)

typedef U16_t ListSize_t;
#define LIST_SIZE_MAX 65535

typedef struct ListNode_t {
    Id_t            id;
    volatile U8_t   lock;
    void            *child;

    /* !!!DO NOT READ/WRITE TO THE FIELDS BELOW!!! */
    struct ListNode_t *next_node;
    struct ListNode_t *prev_node;
} ListNode_t;


typedef struct LinkedList_t {

    ListNode_t   *head;     /* Pointer to head of the list. */
    ListNode_t   *tail;     /* Pointer to tail of the list. */
#ifdef PRTOS_CONFIG_USE_SORTED_LISTS
    ListNode_t   *middle;   /* Pointer to middle of the list. */
#endif

    ListSize_t              size;
    volatile U8_t           lock;
    bool                    sorted;

    /* ID related fields. */
    IdGroup_t                id_type;        /* ID Type of the list. */
    Id_t                    free_id;        /* Free ID. Calculated at creation of the previous node. */
    bool                    id_rollover;	/* Incremental IDs have reached the limit. */
} LinkedList_t;

struct ListIterator {
    U8_t            direction;
    LinkedList_t    *list;
    ListNode_t   *prev_node;
    ListNode_t   *current_node;
    ListSize_t   current_position;
    ListNode_t   *next_node;
};

/* List Results, returned by List* API calls.
 * ListResult_t can be casted to OsResult_t. */
typedef enum {
    LIST_RESULT_OK      = 0,
    LIST_RESULT_FAIL    = 1,
    LIST_RESULT_ERROR   = 2,
    LIST_RESULT_NULL_POINTER = 5,
    LIST_RESULT_LOCKED  = 9
} ListResult_t;

/* V = Implemented.
 * T = Tested. */

/**** List API. ****/

/* V T Initializes the list with specified ID type. V */
void ListInit(LinkedList_t *list, IdGroup_t id_type);

/* V Destroys the specified list and all its nodes. */
OsResult_t ListDestroy(LinkedList_t *list);

/* V T Locks the list. Any request to access the list after locking
 * will be denied. */
OsResult_t ListLock(LinkedList_t *list, U8_t mode);

/* V T Unlocks the list. */
OsResult_t ListUnlock(LinkedList_t *list);

/* V T Return true if locked, false if not locked. */
bool ListIsLocked(LinkedList_t *list);

/* Sorts the list according to the specified sorting type. */
OsResult_t ListSort(LinkedList_t *list, U8_t sort_type);

/* V T Merges two lists. Head of List x will be attached to the
 * tail of List y. */
OsResult_t ListMerge(LinkedList_t *list_x, LinkedList_t *list_y);

/* V T Searches for the list for the ID. NULL
 * is returned if no node was found.
 * ListSearch takes advantage of sorted lists.
 * Worst case unsorted: O(N)
 * Worst case sorted:   O(N/2)
 */
ListNode_t *ListSearch(LinkedList_t *list, Id_t id);

/* V T Searches the list in a exclusively linear fashion for the
 * ID. NULL is returned if no node was found.
 * ListSearch does NOT take advantage of sorted lists.
 * Worst case: O(N)
 */
ListNode_t *ListSearchLinear(LinkedList_t *list, Id_t id);

/* V T Returns the amount of nodes in the list. */
ListSize_t ListSizeGet(LinkedList_t *list);



/* V T Verifies the integrity of the list by checking next and previous pointers.
 * Note that this function will locked the list indefinitely if corruptions are
 * found. Only ListIntegrityRestore can access the list and unlock it if restoration
 * succeeds.
 * Returns:
 * [WARNING] LIST_INTEGRITY_RESULT_NO_ELEMENTS (3) if the list has no nodes (size = 0).
 * [WARNING] LIST_INTEGRITY_RESULT_LOCKED (2) if the list is locked.
 * [WARNING] LIST_INTEGRITY_RESULT_NULL (1) if the list is a null pointer.
 * [OK]      LIST_INTEGRITY_RESULT_LIST_INTACT (0) if the list is intact.
 * [ERROR]   LIST_INTEGRITY_RESULT_CORRUPT_NAV_FW (-1) if forward navigation is corrupted.
 * [ERROR]   LIST_INTEGRITY_RESULT_CORRUPT_NAV_BW (-2) if backward navigation is corrupted.
 * [ERROR]   LIST_INTEGRITY_RESULT_CORRUPT_NAV_BOTH (-3) if both navigation directions are corrupted.
 * [ERROR]   LIST_INTEGRITY_RESULT_CORRUPT_HEAD (-4) if list head is corrupt.
 * [ERROR]   LIST_INTEGRITY_RESULT_CORRUPT_TAIL (-5) if list tail is corrupt.
 */
S8_t ListIntegrityVerify(LinkedList_t *list);

/* Attempts to restore the integrity of the list. Note that the order of
 * the nodes will likely be changed. */
OsResult_t ListIntegrityRestore(LinkedList_t *list, S8_t list_verify_result); /* TODO: Implement ListIntegrityRestore. */


/* Allocates a format buffer and inserts the following list characteristics
 * in the allocated buffer in the specified order.
 * - size
 * - lock
 * - ID type
 * - all node IDs, masked with ID_MASK_UID.
 * formatted_Listsize will be equal to the format buffer size, the list
 * is returned.
 * NOTE: The user is responsible for freeing the format list after usage! */
char *ListPrintToBuffer(LinkedList_t *list,  U32_t *buffer_size);


/**** ListNode API. ****/

/* V T Initializes the node with specified child. */
OsResult_t ListNodeInit(ListNode_t *node, void *child);

/* V T De-initializes the node. If the node was still in a list, it is removed.
 * If the node is already removed from its list, pass NULL for the list. */
OsResult_t ListNodeDeinit(LinkedList_t *list, ListNode_t *node);

/* V T Set the ID of a node. */
OsResult_t ListNodeIdSet(ListNode_t *node, Id_t id);

/* V T Lock the node in either read or write mode.
 * Read locks are recursive. Write lock exclusive. */
OsResult_t ListNodeLock(ListNode_t *node, U8_t mode);

/* V T Unlocks the node from either read or write mode.
 * Read locks are recursive. Write lock exclusive. */
OsResult_t ListNodeUnlock(ListNode_t *node);

/* V T Adds an initialized node to the list at the specified position.
 * position options: LIST_POSITION_HEAD or LIST_POSITION_TAIL. */
OsResult_t ListNodeAddAtPosition(LinkedList_t *list, ListNode_t *node, U8_t position);

/* V T Adds an initialized node_y to the list before or after node_x.
 * position options: LIST_INSERT_BEFORE or LIST_INSERT_AFTER. */
OsResult_t ListNodeAddAtNode(LinkedList_t *list, ListNode_t *node_y, ListNode_t *node_x, U8_t before_after);

/* V T Add an initialized node to the list in a sorted manner if
 * the list is in a sorted state. */
OsResult_t ListNodeAddSorted(LinkedList_t *list, ListNode_t *node);

/* V T Removes the node from the list. */
ListNode_t *ListNodeRemove(LinkedList_t *list, ListNode_t *node);

/* V T Removes the head node from the list. */
ListNode_t *ListNodeRemoveFromHead(LinkedList_t *list);

/* V T Removes the tail node from the list. */
ListNode_t *ListNodeRemoveFromTail(LinkedList_t *list);

/* V T Peeks at the specified position in the list.
 * position options: LIST_POSITION_HEAD or LIST_POSITION_TAIL.
 * The node is not removed. */
ListNode_t *ListNodePeek(LinkedList_t *list, U8_t position);

/* V T Peeks at the next node of the specified node.
 * The node is not removed. */
ListNode_t *ListNodePeekNext(ListNode_t *node);

/* V T Peeks at the previous node of the specified node.
 * The node is not removed. */
ListNode_t *ListNodePeekPrev(ListNode_t *node);

bool ListNodeHasNext(ListNode_t *node);

bool ListNodeHasPrev(ListNode_t *node);

/* V T Moves the specified node from list x to list y either sorted or at the tail position. */
OsResult_t ListNodeMove(LinkedList_t *list_x, LinkedList_t *list_y, ListNode_t *node);

/* V T Swaps node x with node y in the specified list. */
OsResult_t ListNodeSwap(LinkedList_t *list, ListNode_t *node_x, ListNode_t *node_y);

/* V T Returns the child pointer from the node ID in the list. */
void *ListNodeChildFromId(LinkedList_t *list, Id_t id);

/* V T Returns the child pointer of the node. */
void *ListNodeChildGet(ListNode_t *node);

/* V T Sets the child of the node. */
OsResult_t ListNodeChildSet(ListNode_t *node, void *child);

/* V T Returns the node ID. */
Id_t ListNodeIdGet(ListNode_t *node);

/* V T Returns true if the node is in a list, false if not. */
bool ListNodeIsInList(LinkedList_t *list, ListNode_t *node);



/* Indicates the start of an Access block.
 * Searches for the node in the list belonging to the ID.
 * An attempt to lock the node is done.
 * If successful, the block between _BEGIN and _END is executed. */
#define LIST_NODE_ACCESS_READ_BEGIN(list, id)                    \
static ListNode_t *node = NULL;                               \
node = ListSearch(list, id);                                     \
if(node != NULL) {                                               \
    if(ListNodeLock(node, LIST_LOCK_MODE_READ) == OS_RES_OK) {    \

/* Indicates the start of Access block.
 * Searches for the node in the list belonging to the ID.
 * An attempt to lock the node is done.
 * If successful, the block between _BEGIN and _END is executed. */
#define LIST_NODE_ACCESS_WRITE_BEGIN(list, id)                   \
static ListNode_t *node = NULL;                               \
node = ListSearch(list, id);                                     \
if(node != NULL) {                                               \
    if(ListNodeLock(node, LIST_LOCK_MODE_WRITE) == OS_RES_OK) {   \



/* Indicates the end of an Access block. */
#define LIST_NODE_ACCESS_END()              \
 ListNodeUnlock(node);                      \
 }}                                         \



OsResult_t ListIteratorInit(struct ListIterator *list_it, LinkedList_t *list, U8_t it_direction);

ListNode_t *ListIteratorNext(struct ListIterator *list_it);

ListNode_t *ListIteratorPrev(struct ListIterator *list_it);

bool ListIteratorEnd(struct ListIterator *list_it);

/* Beginning of a List Iterator block.
 * All code contained by the _BEGIN & _END
 * macros is executed at every iteration of the
 * loop. */
#define LIST_ITERATOR_BEGIN(p_it, p_list, it_direction)         \
if(ListIteratorInit(p_it, p_list, it_direction) == OS_RES_OK) { \
do {                                                            \

#define LIST_ITERATOR_BREAK_ON_CONDITION(cond)  \
if(cond) break;                                 \

#define LIST_ITERATOR_GOTO_NEXT(p_it)   \
goto p_it##_next;                       \

#define LIST_ITERATOR_GOTO_NEXT_ON_CONDITION(p_it, cond)  

#define LIST_ITERATOR_END(p_it)     \
if(ListIteratorNext(p_it) == NULL)  \
    break;                          \
} while (!ListIteratorEnd(p_it));	\
}                                   \


#ifdef __cplusplus
}
#endif
#endif /* LIST_H_ */
