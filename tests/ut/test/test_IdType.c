/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
 
//-- unity: unit test framework
#include "unity.h"
 
//-- module being tested
#include "IdType.h"
#include "IdTypeDef.h"

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

