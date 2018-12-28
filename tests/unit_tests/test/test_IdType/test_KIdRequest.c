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
