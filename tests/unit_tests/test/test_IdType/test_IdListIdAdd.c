/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
 
/* Test framework */
#include "unity.h"
 
/* UUT */
#include "IdType.h"
#include "IdTypeDef.h"

/* Dependencies */

/* Other */
#include <stdio.h>
 
/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/
 
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
	KIdInit();
}
 
void tearDown(void)
{
}
 
/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_IdListIdAdd_single(void)
{
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	IdListIdAdd(&list, id);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, 1, "Expected 1 item in the ID list.");
	TEST_ASSERT_EQUAL_MESSAGE(list.ids[list.n - 1], id, "ID in the list is not correct.");
}

void test_IdListIdAdd_all(void)
{
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	for(uint8_t j = 0; j < ID_LIST_SIZE_MAX; j++) {
		id = j;
		IdListIdAdd(&list, id);
		TEST_ASSERT_EQUAL_MESSAGE(list.n, j + 1, "Number of items in the ID list incorrect.");
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[list.n - 1], id, "ID in the list is incorrect.");
	}
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_SIZE_MAX, "Number of IDs in the list is incorrect");
}

void test_IdListIdAdd_all_plus_one(void)
{
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	for(uint8_t j = 0; j < ID_LIST_SIZE_MAX; j++) {
		id = j;
		IdListIdAdd(&list, id);
		TEST_ASSERT_EQUAL_MESSAGE(list.n, j + 1, "Expected number of items in the ID list incorrect.");
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[list.n - 1], id, "ID in the list is incorrect.");
	}
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_SIZE_MAX, "Number of IDs in the list is incorrect");
	id++;
	IdListIdAdd(&list, id);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_SIZE_MAX, "Added an ID beyond the list capacity.");
}

