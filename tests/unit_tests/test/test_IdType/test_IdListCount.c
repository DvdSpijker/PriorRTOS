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
 
void test_IdListCount_invalid_list(void)
{
	U8_t cnt = IdListCount(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(cnt, ID_LIST_EMPTY, "Valid count returned for an invalid list.");	
}

void test_IdListCount_single(void)
{	
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	U8_t cnt = IdListCount(&list);
	TEST_ASSERT_EQUAL_MESSAGE(cnt, ID_LIST_EMPTY, "Non-empty count returned for an empty list.");
	
	IdListIdAdd(&list, id);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, 1, "Expected 1 item in the ID list.");
	TEST_ASSERT_EQUAL_MESSAGE(list.ids[list.n - 1], id, "ID in the list is not correct.");
	
	cnt = IdListCount(&list);
	TEST_ASSERT_EQUAL_MESSAGE(cnt, 1, "List count incorrect.");
	
	Id_t rm_id = IdListIdRemove(&list);
	TEST_ASSERT_EQUAL_MESSAGE(id, rm_id, "Removed ID not equal to added ID.");
	TEST_ASSERT_EQUAL_MESSAGE(list.n, 0, "Number of items in the list incorrect.");
	
	cnt = IdListCount(&list);
	TEST_ASSERT_EQUAL_MESSAGE(cnt, ID_LIST_EMPTY, "Non-empty count returned for an empty list.");
	
	
}

void test_IdListCount_all(void)
{
	Id_t id_array[ID_LIST_SIZE_MAX] = {ID_INVALID};
	Id_t id = 1;
	IdList_t list;
	U8_t cnt = ID_LIST_EMPTY;
	IdListInit(&list);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	cnt = IdListCount(&list);
	TEST_ASSERT_EQUAL_MESSAGE(cnt, ID_LIST_EMPTY, "Non-empty count returned for an empty list.");
	
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		id = i;
		id_array[i] = id;
		IdListIdAdd(&list, id);
		TEST_ASSERT_EQUAL_MESSAGE(list.n, i + 1, "Number of items in the ID list incorrect.");
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[list.n - 1], id, "ID in the list is incorrect.");
		cnt = IdListCount(&list);
		TEST_ASSERT_EQUAL_MESSAGE(cnt, i+1, "List count incorrect.");
	}
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_SIZE_MAX, "Number of IDs in the list is incorrect");
	cnt = IdListCount(&list);
	TEST_ASSERT_EQUAL_MESSAGE(cnt, ID_LIST_SIZE_MAX, "List count incorrect.");
	
	Id_t rm_id = ID_INVALID;
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		rm_id = IdListIdRemove(&list);
		TEST_ASSERT_EQUAL_MESSAGE(id_array[ID_LIST_SIZE_MAX - (i + 1)], rm_id, "Removed ID not equal to added ID.");
		TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_SIZE_MAX - (i + 1), "Number of items in the list incorrect.");
		cnt = IdListCount(&list);
		TEST_ASSERT_EQUAL_MESSAGE(cnt, ID_LIST_SIZE_MAX - (i + 1), "List count incorrect.");
	}
	TEST_ASSERT_EQUAL_MESSAGE(list.n, 0, "Number of IDs in the list is not 0.");
	cnt = IdListCount(&list);
	TEST_ASSERT_EQUAL_MESSAGE(cnt, ID_LIST_EMPTY, "Non-empty count returned for an empty list.");
	
	
}