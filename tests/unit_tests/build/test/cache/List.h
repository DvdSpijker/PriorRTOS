#include "include/OsTypes.h"


typedef U16_t ListSize_t;





typedef struct ListNode_t {

    Id_t id;

    volatile U8_t lock;

    void *child;





    struct ListNode_t *next_node;

    struct ListNode_t *prev_node;

} ListNode_t;





typedef struct LinkedList_t {



    ListNode_t *head;

    ListNode_t *tail;



    ListNode_t *middle;





    ListSize_t size;

    volatile U8_t lock;

    

   _Bool 

                           sorted;





    IdGroup_t id_group;

} LinkedList_t;



struct ListIterator {

    U8_t direction;

    LinkedList_t *list;

    ListNode_t *prev_node;

    ListNode_t *current_node;

    ListSize_t current_position;

    ListNode_t *next_node;

};







typedef enum {

    LIST_RESULT_OK = 0,

    LIST_RESULT_FAIL = 1,

    LIST_RESULT_ERROR = 2,

    LIST_RESULT_NULL_POINTER = 5,

    LIST_RESULT_LOCKED = 9

} ListResult_t;















void ListInit(LinkedList_t *list, IdGroup_t id_type);





OsResult_t ListDestroy(LinkedList_t *list);







OsResult_t ListLock(LinkedList_t *list, U8_t mode);





OsResult_t ListUnlock(LinkedList_t *list);







_Bool 

    ListIsLocked(LinkedList_t *list);





OsResult_t ListSort(LinkedList_t *list, U8_t sort_type);







OsResult_t ListMerge(LinkedList_t *list_x, LinkedList_t *list_y);















ListNode_t *ListSearch(LinkedList_t *list, Id_t id);













ListNode_t *ListSearchLinear(LinkedList_t *list, Id_t id);





ListSize_t ListSizeGet(LinkedList_t *list);

S8_t ListIntegrityVerify(LinkedList_t *list);







OsResult_t ListIntegrityRestore(LinkedList_t *list, S8_t list_verify_result);

char *ListPrintToBuffer(LinkedList_t *list, U32_t *buffer_size);











OsResult_t ListNodeInit(ListNode_t *node, void *child);







OsResult_t ListNodeDeinit(LinkedList_t *list, ListNode_t *node);





OsResult_t ListNodeIdSet(ListNode_t *node, Id_t id);







OsResult_t ListNodeLock(ListNode_t *node, U8_t mode);







OsResult_t ListNodeUnlock(ListNode_t *node);







OsResult_t ListNodeAddAtPosition(LinkedList_t *list, ListNode_t *node, U8_t position);







OsResult_t ListNodeAddAtNode(LinkedList_t *list, ListNode_t *node_y, ListNode_t *node_x, U8_t before_after);







OsResult_t ListNodeAddSorted(LinkedList_t *list, ListNode_t *node);





ListNode_t *ListNodeRemove(LinkedList_t *list, ListNode_t *node);





ListNode_t *ListNodeRemoveFromHead(LinkedList_t *list);





ListNode_t *ListNodeRemoveFromTail(LinkedList_t *list);









ListNode_t *ListNodePeek(LinkedList_t *list, U8_t position);







ListNode_t *ListNodePeekNext(ListNode_t *node);







ListNode_t *ListNodePeekPrev(ListNode_t *node);





_Bool 

    ListNodeHasNext(ListNode_t *node);





_Bool 

    ListNodeHasPrev(ListNode_t *node);





OsResult_t ListNodeMove(LinkedList_t *list_x, LinkedList_t *list_y, ListNode_t *node);





OsResult_t ListNodeSwap(LinkedList_t *list, ListNode_t *node_x, ListNode_t *node_y);





void *ListNodeChildFromId(LinkedList_t *list, Id_t id);





void *ListNodeChildGet(ListNode_t *node);





OsResult_t ListNodeChildSet(ListNode_t *node, void *child);





Id_t ListNodeIdGet(ListNode_t *node);







_Bool 

    ListNodeIsInList(LinkedList_t *list, ListNode_t *node);

OsResult_t ListIteratorInit(struct ListIterator *list_it, LinkedList_t *list, U8_t it_direction);



ListNode_t *ListIteratorNext(struct ListIterator *list_it);



ListNode_t *ListIteratorPrev(struct ListIterator *list_it);





_Bool 

    ListIteratorEnd(struct ListIterator *list_it);
