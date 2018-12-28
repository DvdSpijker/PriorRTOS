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
