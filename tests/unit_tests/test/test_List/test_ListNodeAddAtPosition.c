/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

/* Test framework */
#include "unity.h"
 
/* UUT */
#include "List.h"

/* Dependencies */
#include "mock_IdType.h"
#include "mock_IdTypeDef.h"

/* Other */
#include <stdio.h>
 
/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/
#ifdef LIST_SIZE_MAX
#undef LIST_SIZE_MAX
#define LIST_SIZE_MAX 256
#endif

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/
 
/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/
 
 
/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
 
 
/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/
 
void setUp(void)
{
}
 
void tearDown(void)
{
}
 
/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_ListNodeAddAtPosition_full_list(void)
{
	printf("IGNORED: test_ListNodeAddAtPosition_full_list\n");
	return;
	
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;
	ListNode_t node;
	
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ListNodeInit(&node, NULL);
	
	list.size = LIST_SIZE_MAX;
	
	res = ListNodeAddAtPosition(&list, &node, LIST_POSITION_HEAD);
	TEST_ASSERT_EQUAL(res, OS_RES_FAIL);	
}

void test_ListNodeAddAtPosition_invalid_list(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	res = ListNodeAddAtPosition(NULL, &node, LIST_POSITION_HEAD);
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);	
}

void test_ListNodeAddAtPosition_invalid_node(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;
	
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);

	res = ListNodeAddAtPosition(&list, NULL, LIST_POSITION_HEAD);
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);	
}

void test_ListNodeAddAtPosition_locked_list(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;
	ListNode_t node;
	
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ListNodeInit(&node, NULL);
	
	ListLock(&list, LIST_LOCK_MODE_WRITE);
	
	res = ListNodeAddAtPosition(&list, &node, LIST_POSITION_HEAD);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);	
}

void test_ListNodeAddAtPosition_invalid_id_request(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;
	ListNode_t node;
	
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ListNodeInit(&node, NULL);
	
	KIdRequest_ExpectAndReturn(ID_GROUP_MESSAGE_QUEUE, ID_INVALID);
	
	res = ListNodeAddAtPosition(&list, &node, LIST_POSITION_HEAD);
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ID);	
}

void test_ListNodeAddAtPosition_add_one_head(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;
	ListNode_t node;
	
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ListNodeInit(&node, NULL);
	
	KIdRequest_ExpectAndReturn(ID_GROUP_MESSAGE_QUEUE, 1);
	
	res = ListNodeAddAtPosition(&list, &node, LIST_POSITION_HEAD);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(list.head, &node);
	TEST_ASSERT_EQUAL(list.tail, &node);
	TEST_ASSERT_EQUAL(list.size, 1);
	TEST_ASSERT_EQUAL(node.id, 1);
	TEST_ASSERT_EQUAL(node.next_node, NULL);
	TEST_ASSERT_EQUAL(node.prev_node, NULL);
}

void test_ListNodeAddAtPosition_add_two_head(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;
	ListNode_t node;
	ListNode_t node_two;
	Id_t id = 0;
	
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ListNodeInit(&node, NULL);
	ListNodeInit(&node_two, NULL);
	
	id++;
	KIdRequest_ExpectAndReturn(ID_GROUP_MESSAGE_QUEUE, id);
	
	res = ListNodeAddAtPosition(&list, &node, LIST_POSITION_HEAD);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(list.head, &node);
	TEST_ASSERT_EQUAL(list.tail, &node);
	TEST_ASSERT_EQUAL(list.size, 1);
	TEST_ASSERT_EQUAL(node.id, id);
	TEST_ASSERT_EQUAL(node.next_node, NULL);
	TEST_ASSERT_EQUAL(node.prev_node, NULL);
	
	id++;
	KIdRequest_ExpectAndReturn(ID_GROUP_MESSAGE_QUEUE, id);
	
	res = ListNodeAddAtPosition(&list, &node_two, LIST_POSITION_HEAD);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(list.head, &node_two);
	TEST_ASSERT_EQUAL(list.tail, &node);
	TEST_ASSERT_EQUAL(list.size, 2);
	TEST_ASSERT_EQUAL(node_two.id, id);
	TEST_ASSERT_EQUAL(node.next_node, NULL);
	TEST_ASSERT_EQUAL(node.prev_node, &node_two);
	TEST_ASSERT_EQUAL(node_two.next_node, &node);
	TEST_ASSERT_EQUAL(node_two.prev_node, NULL);
}

void test_ListNodeAddAtPosition_add_one_head_one_tail(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;
	ListNode_t node;
	ListNode_t node_two;
	Id_t id = 0;
	
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ListNodeInit(&node, NULL);
	ListNodeInit(&node_two, NULL);
	
	id++;
	KIdRequest_ExpectAndReturn(ID_GROUP_MESSAGE_QUEUE, id);
	
	res = ListNodeAddAtPosition(&list, &node, LIST_POSITION_HEAD);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(list.head, &node);
	TEST_ASSERT_EQUAL(list.tail, &node);
	TEST_ASSERT_EQUAL(list.size, 1);
	
	TEST_ASSERT_EQUAL(node.id, id);
	TEST_ASSERT_EQUAL(node.next_node, NULL);
	TEST_ASSERT_EQUAL(node.prev_node, NULL);
	
	id++;
	KIdRequest_ExpectAndReturn(ID_GROUP_MESSAGE_QUEUE, id);
	
	res = ListNodeAddAtPosition(&list, &node_two, LIST_POSITION_TAIL);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(list.head, &node);
	TEST_ASSERT_EQUAL(list.tail, &node_two);
	TEST_ASSERT_EQUAL(list.size, 2);
	TEST_ASSERT_EQUAL(node_two.id, id);
	TEST_ASSERT_EQUAL(node.next_node, &node_two);
	TEST_ASSERT_EQUAL(node.prev_node, NULL);
	TEST_ASSERT_EQUAL(node_two.next_node, NULL);
	TEST_ASSERT_EQUAL(node_two.prev_node, &node);
}

void test_ListNodeAddAtPosition_add_head_max(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;
	ListNode_t nodes[LIST_SIZE_MAX];
	Id_t id = 0;
	ListSize_t size = 0;
	int i = 0;
	
	printf("Max list size: %u\n", LIST_SIZE_MAX);
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);

	for(i = 0;i < LIST_SIZE_MAX; i++) {
		size++;

		KIdRequest_IgnoreAndReturn(id);
		//printf("ID: %u\n", id);
		
		ListNodeInit(&nodes[i], NULL);
		res = ListNodeAddAtPosition(&list, &nodes[i], LIST_POSITION_HEAD);	
		TEST_ASSERT_EQUAL(res, OS_RES_OK);
		TEST_ASSERT_EQUAL(list.head, &nodes[i]);
		TEST_ASSERT_EQUAL(list.tail, &nodes[0]);
		TEST_ASSERT_EQUAL(list.size, size);
		TEST_ASSERT_EQUAL(nodes[i].id, id);
		
		/* If this is the first node, the next and prev pointers must be NULL. */
		if(i == 0) {
			TEST_ASSERT_EQUAL(nodes[i].next_node, NULL);
			TEST_ASSERT_EQUAL(nodes[i].prev_node, NULL);				
		} else {
			TEST_ASSERT_EQUAL(nodes[i].next_node, &nodes[i - 1]);
			TEST_ASSERT_EQUAL(nodes[i].prev_node, NULL);		
		}
		
		id++;
	}
}


