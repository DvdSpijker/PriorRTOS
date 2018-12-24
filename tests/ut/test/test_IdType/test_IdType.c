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
 
/***** Test KIdRequest *****/

void test_KIdRequest_invalid_group(void)
{
	Id_t id = KIdRequest(ID_GROUP_NUM + 1);
	TEST_ASSERT_EQUAL(id, ID_INVALID);

	id = KIdRequest(-1);
	TEST_ASSERT_EQUAL(id, ID_INVALID);
}

void test_KIdRequest_valid_group_single(void)
{
	const Id_t exp_seq = 0;
	
	Id_t id = KIdRequest(ID_GROUP_MAILBOX);
	TEST_ASSERT_NOT_EQUAL(id, ID_INVALID);
	TEST_ASSERT_EQUAL((id & ID_MASK_GROUP), ((Id_t)(ID_GROUP_MAILBOX << ID_SHIFT_AMOUNT_GROUP) & ID_MASK_GROUP));
	TEST_ASSERT_EQUAL((id & ID_MASK_SEQ_NUM), (exp_seq & ID_MASK_SEQ_NUM));
}

void test_KIdRequest_valid_group_all(void)
{
	Id_t exp_seq = 0;
	Id_t id = ID_INVALID;
	
	for(int i = 0; i < ID_GROUP_NUM; i++) {
		id = KIdRequest(i);
		TEST_ASSERT_NOT_EQUAL(id, ID_INVALID);
		TEST_ASSERT_EQUAL((id & ID_MASK_GROUP), ((Id_t)(i << ID_SHIFT_AMOUNT_GROUP) & ID_MASK_GROUP));
		TEST_ASSERT_EQUAL((id & ID_MASK_SEQ_NUM), (exp_seq & ID_MASK_SEQ_NUM));
	}
}

void test_KIdRequest_all(void)
{
	uint32_t id_cnt = 0;
	Id_t id = ID_INVALID;
	
	for(int i = 0; i < ID_GROUP_NUM; i++) {
		for(uint32_t j = 0; j < (ID_INVALID & ID_MASK_SEQ_NUM); j++) {
			id_cnt++;
			id = KIdRequest(i);
			TEST_ASSERT_NOT_EQUAL(id, ID_INVALID);
			TEST_ASSERT_EQUAL((id & ID_MASK_GROUP), ((Id_t)(i << ID_SHIFT_AMOUNT_GROUP) & ID_MASK_GROUP));
			TEST_ASSERT_EQUAL((id & ID_MASK_SEQ_NUM), (j & ID_MASK_SEQ_NUM));
		}
	}
	TEST_ASSERT_EQUAL(id_cnt, ID_SEQ_NUM_MAX * ID_GROUP_NUM);
	printf("Number of IDs: %u\n", id_cnt);
}

void test_KIdRequest_all_plus_one(void)
{
	uint32_t id_cnt = 0;
	Id_t id = ID_INVALID;
	
	for(int i = 0; i < ID_GROUP_NUM; i++) {
		for(uint32_t j = 0; j < (ID_INVALID & ID_MASK_SEQ_NUM); j++) {
			id_cnt++;
			id = KIdRequest(i);
			TEST_ASSERT_NOT_EQUAL(id, ID_INVALID);
			TEST_ASSERT_EQUAL((id & ID_MASK_GROUP), ((Id_t)(i << ID_SHIFT_AMOUNT_GROUP) & ID_MASK_GROUP));
			TEST_ASSERT_EQUAL((id & ID_MASK_SEQ_NUM), (j & ID_MASK_SEQ_NUM));
		}
		id_cnt++;
		id = KIdRequest(i);
		TEST_ASSERT_EQUAL(id, ID_INVALID);
	}
	TEST_ASSERT_EQUAL(id_cnt, (ID_SEQ_NUM_MAX + 1) * ID_GROUP_NUM);
	printf("Number of IDs: %u\n", id_cnt);
}

/***** Test IdSequenceNumberGet *****/

void test_IdSequenceNumberGet_valid(void)
{
	const Id_t id = ID_GROUP_MESSAGE_QUEUE | 0x00002000;
	
	Id_t id_seq = IdSequenceNumberGet(id);
	TEST_ASSERT_EQUAL(id_seq, (id & ID_MASK_SEQ_NUM));
}

void test_IdSequenceNumberGet_invalid_id(void)
{
	const Id_t id = ID_INVALID;
	
	Id_t id_seq = IdSequenceNumberGet(id);
	TEST_ASSERT_EQUAL(id_seq, ID_INVALID);
}

void test_IdSequenceNumberGet_invalid_group(void)
{
	Id_t id = ID_GROUP_INVALID | 0x00001000;
	Id_t id_seq = IdSequenceNumberGet(id);
	TEST_ASSERT_EQUAL(id_seq, ID_INVALID);
	
	id = (ID_GROUP_NUM + 1) << ID_SHIFT_AMOUNT_GROUP | 0x00001000;
	id_seq = IdSequenceNumberGet(id);
	TEST_ASSERT_EQUAL(id_seq, ID_INVALID);
}

/***** Test IdGroupGet *****/

void test_IdGroupGet_valid(void)
{
	const Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << ID_SHIFT_AMOUNT_GROUP) | 0x00002000;
	IdGroup_t id_group = IdGroupGet(id);
	TEST_ASSERT_EQUAL(id_group, ID_GROUP_MESSAGE_QUEUE);	
}

void test_IdGroupGet_invalid_group(void)
{
	Id_t id = ID_GROUP_INVALID | 0x00001000;
	IdGroup_t id_group = IdGroupGet(id);
	TEST_ASSERT_EQUAL(id_group, ID_GROUP_INV);
	
	id = (ID_GROUP_NUM + 1) << ID_SHIFT_AMOUNT_GROUP | 0x00001000;
	id_group = IdGroupGet(id);
	TEST_ASSERT_EQUAL(id_group, ID_GROUP_INV);
}

/***** Test IdIsInGroup *****/

void test_IdIsInGroup_valid_true(void)
{
	Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << ID_SHIFT_AMOUNT_GROUP) | 0x00002000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_MESSAGE_QUEUE);
	TEST_ASSERT_EQUAL(in_group, 1);
}

void test_IdIsInGroup_valid_false(void)
{
	Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << ID_SHIFT_AMOUNT_GROUP) | 0x00002000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_EVENTGROUP);
	TEST_ASSERT_EQUAL(in_group, 0);
}

void test_IdIsInGroup_invalid_group(void)
{
	Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << ID_SHIFT_AMOUNT_GROUP) | 0x00002000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_NUM);
	TEST_ASSERT_EQUAL(in_group, 0);
}

void test_IdIsInGroup_invalid_id(void)
{
	Id_t id = ID_GROUP_INVALID | 0x00001000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_EVENTGROUP);
	TEST_ASSERT_EQUAL(in_group, 0);
}

void test_IdIsInGroup_invalid_group_and_id(void)
{
	Id_t id = ID_GROUP_INVALID | 0x00001000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_NUM);
	TEST_ASSERT_EQUAL(in_group, 0);
}

/***** Test IdListInit *****/

void test_IdListInit_valid(void)
{
	IdList_t list;
	IdListInit(&list);
	TEST_ASSERT_EQUAL(list.n, ID_LIST_EMPTY);
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		TEST_ASSERT_EQUAL(list.ids[i], ID_INVALID);
	}
}

void test_IdListInit_invalid(void)
{
	IdListInit(NULL);
}

/***** Test IdListIdAdd *****/

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

/***** Test IdListIdRemove *****/

void test_IdListIdRemove_invalid_list(void)
{
	Id_t id = IdListIdRemove(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(id, ID_INVALID, "Removed valid ID from invalid list.");
}

void test_IdListIdRemove_single(void)
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
	
	Id_t rm_id = IdListIdRemove(&list);
	TEST_ASSERT_EQUAL_MESSAGE(id, rm_id, "Removed ID not equal to added ID.");
	TEST_ASSERT_EQUAL_MESSAGE(list.n, 0, "Number of items in the list incorrect.");
}

void test_IdListIdRemove_all(void)
{	
	Id_t id_array[ID_LIST_SIZE_MAX] = {ID_INVALID};
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		id = i;
		id_array[i] = id;
		IdListIdAdd(&list, id);
		TEST_ASSERT_EQUAL_MESSAGE(list.n, i + 1, "Number of items in the ID list incorrect.");
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[list.n - 1], id, "ID in the list is incorrect.");
	}
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_SIZE_MAX, "Number of IDs in the list is incorrect");
	
	
	Id_t rm_id = ID_INVALID;
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		rm_id = IdListIdRemove(&list);
		TEST_ASSERT_EQUAL_MESSAGE(id_array[ID_LIST_SIZE_MAX - (i + 1)], rm_id, "Removed ID not equal to added ID.");
		TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_SIZE_MAX - (i + 1), "Number of items in the list incorrect.");
	}
	TEST_ASSERT_EQUAL_MESSAGE(list.n, 0, "Number of IDs in the list is not 0.");
}

void test_IdListIdRemove_all_plus_one(void)
{	
	Id_t id_array[ID_LIST_SIZE_MAX] = {ID_INVALID};
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		id = i;
		id_array[i] = id;
		IdListIdAdd(&list, id);
		TEST_ASSERT_EQUAL_MESSAGE(list.n, i + 1, "Number of items in the ID list incorrect.");
		TEST_ASSERT_EQUAL_MESSAGE(list.ids[list.n - 1], id, "ID in the list is incorrect.");
	}
	TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_SIZE_MAX, "Number of IDs in the list is incorrect");
	
	
	Id_t rm_id = ID_INVALID;
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		rm_id = IdListIdRemove(&list);
		TEST_ASSERT_EQUAL_MESSAGE(id_array[ID_LIST_SIZE_MAX - (i + 1)], rm_id, "Removed ID not equal to added ID.");
		TEST_ASSERT_EQUAL_MESSAGE(list.n, ID_LIST_SIZE_MAX - (i + 1), "Number of items in the list incorrect.");
	}
	TEST_ASSERT_EQUAL_MESSAGE(list.n, 0, "Number of IDs in the list is not 0.");
	
	rm_id = IdListIdRemove(&list);
	TEST_ASSERT_EQUAL_MESSAGE(rm_id, ID_INVALID, "ID removed from empty list.");
}

/***** Test IdListCount *****/

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


/***** Test IdListCopy *****/

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



