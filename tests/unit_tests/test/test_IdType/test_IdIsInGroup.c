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
