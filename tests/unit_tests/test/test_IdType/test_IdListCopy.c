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

void test_IdListCopy_valid_lists(void)
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
	
	IdList_t list_cpy;
	IdListInit(&list_cpy);
	
	IdListCopy(&list_cpy, &list);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, list_cpy.n, "Item count is not equal after copy.");
	for(uint8_t k = 0; k < ID_LIST_SIZE_MAX; k++) {
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[k], list_cpy.ids[k], "IDs are not equal after copy.");
	}
	
}


