#include "test_IdType.h"

#include "IdType.h"
#include "IdTypeDef.h"

int test_KIdRequest_invalid_group(void);
int test_KIdRequest_valid_group_single(void);
int test_KIdRequest_valid_group_all(void);
int test_KIdRequest_all(void);
int test_KIdRequest_all_plus_one(void);

int test_IdSequenceNumberGet_valid(void);
int test_IdSequenceNumberGet_invalid_id(void);
int test_IdSequenceNumberGet_invalid_group(void);

int test_IdGroupGet_invalid_group(void);
int test_IdGroupGet_invalid_id(void);
int test_IdGroupGet_valid(void);

ut_test_t test_set_IdType[] = {
	test_KIdRequest,
	test_IdSequenceNumberGet,
	test_IdGroupGet,
};

int ut_set_up(void)
{
	KIdInit();
	return UT_OK;
}

int ut_tear_down(void) 
{
	return UT_OK;
}

int test_KIdRequest(void)
{
	ut_run_test(test_KIdRequest_invalid_group);
	ut_run_test(test_KIdRequest_valid_group_single);
	ut_run_test(test_KIdRequest_valid_group_all);
	ut_run_test(test_KIdRequest_all);
	ut_run_test(test_KIdRequest_all_plus_one);
	
	ut_end;
}

int test_KIdRequest_invalid_group(void)
{
	ut_start;
	
	Id_t id = KIdRequest(ID_GROUP_NUM + 1);
	ut_assert_eq(id, ID_INVALID, "ID returned on invalid group.");
	
	id = KIdRequest(-1);
	ut_assert_eq(id, ID_INVALID, "ID returned on invalid group.");
	
	ut_end;
}

int test_KIdRequest_valid_group_single(void)
{
	ut_start;
	
	const Id_t exp_seq = 0;
	
	Id_t id = KIdRequest(ID_GROUP_MAILBOX);
	ut_assert_neq(id, ID_INVALID, "Invalid ID returned on valid group.");
	ut_assert_eq((id & ID_MASK_GROUP), ((Id_t)(ID_GROUP_MAILBOX << ID_SHIFT_AMOUNT_GROUP) & ID_MASK_GROUP), "Invalid group.");
	ut_assert_eq((id & ID_MASK_SEQ_NUM), (exp_seq & ID_MASK_SEQ_NUM), "Invalid seq nr.");
	
	ut_end;
}

int test_KIdRequest_valid_group_all(void)
{
	ut_start;
	
	Id_t exp_seq = 0;
	Id_t id = ID_INVALID;
	
	for(int i = 0; i < ID_GROUP_NUM; i++) {
		id = KIdRequest(i);
		ut_assert_neq(id, ID_INVALID, "Invalid ID returned on valid group.");
		ut_assert_eq((id & ID_MASK_GROUP), ((Id_t)(i << ID_SHIFT_AMOUNT_GROUP) & ID_MASK_GROUP), "Invalid group.");
		ut_assert_eq((id & ID_MASK_SEQ_NUM), (exp_seq & ID_MASK_SEQ_NUM), "Invalid seq nr.");
	}
	
	ut_end;
}

int test_KIdRequest_all(void)
{
	ut_start;
	
	uint32_t id_cnt = 0;
	Id_t id = ID_INVALID;
	
	for(int i = 0; i < ID_GROUP_NUM; i++) {
		for(uint32_t j = 0; j < (ID_INVALID & ID_MASK_SEQ_NUM); j++) {
			id_cnt++;
			id = KIdRequest(i);
			ut_assert_neq(id, ID_INVALID, "Invalid ID returned on valid group.");
			ut_assert_eq((id & ID_MASK_GROUP), ((Id_t)(i << ID_SHIFT_AMOUNT_GROUP) & ID_MASK_GROUP), "Invalid group.");
			ut_assert_eq((id & ID_MASK_SEQ_NUM), (j & ID_MASK_SEQ_NUM), "Invalid seq nr.");
		}
	}
	ut_assert_eq(id_cnt, ID_SEQ_NUM_MAX * ID_GROUP_NUM, "Not all IDs were requested.");
	printf("Number of IDs: %u\n", id_cnt);
	
	ut_end;
}

int test_KIdRequest_all_plus_one(void)
{
	ut_start;
	
	uint32_t id_cnt = 0;
	Id_t id = ID_INVALID;
	
	for(int i = 0; i < ID_GROUP_NUM; i++) {
		for(uint32_t j = 0; j < (ID_INVALID & ID_MASK_SEQ_NUM); j++) {
			id_cnt++;
			id = KIdRequest(i);
			ut_assert_neq(id, ID_INVALID, "Invalid ID returned on valid group.");
			ut_assert_eq((id & ID_MASK_GROUP), ((Id_t)(i << ID_SHIFT_AMOUNT_GROUP) & ID_MASK_GROUP), "Invalid group.");
			ut_assert_eq((id & ID_MASK_SEQ_NUM), (j & ID_MASK_SEQ_NUM), "Invalid seq nr.");
		}
		id_cnt++;
		id = KIdRequest(i);
		ut_assert_eq(id, ID_INVALID, "Valid ID returned on invalid request.");
	}
	ut_assert_eq(id_cnt, (ID_SEQ_NUM_MAX + 1) * ID_GROUP_NUM, "Not all IDs were requested.");
	printf("Number of IDs: %u\n", id_cnt);
	
	ut_end;
}


int test_IdSequenceNumberGet(void)
{
	ut_run_test(test_IdSequenceNumberGet_valid);
	ut_run_test(test_IdSequenceNumberGet_invalid_id);
	ut_run_test(test_IdSequenceNumberGet_invalid_group);

	ut_end;
}

int test_IdSequenceNumberGet_valid(void)
{
	ut_start;
	
	const Id_t id = ID_GROUP_MESSAGE_QUEUE | 0x00002000;
	Id_t id_seq = IdSequenceNumberGet(id);
	ut_assert_eq(id_seq, (id & ID_MASK_SEQ_NUM), "Incorrect seq nr.");
	
	ut_end;
}

int test_IdSequenceNumberGet_invalid_id(void)
{
	ut_start;
	
	const Id_t id = ID_INVALID;
	Id_t id_seq = IdSequenceNumberGet(id);
	ut_assert_eq(id_seq, ID_INVALID, "Valid ID seq returned on invalid ID.");
	
	ut_end;
}

int test_IdSequenceNumberGet_invalid_group(void)
{
	ut_start;
	
	Id_t id = ID_GROUP_INVALID | 0x00001000;
	Id_t id_seq = IdSequenceNumberGet(id);
	ut_assert_eq(id_seq, ID_INVALID, "Valid ID seq returned on invalid ID group.");
	
	id = (ID_GROUP_NUM + 1) | 0x00001000;
	id_seq = IdSequenceNumberGet(id);
	ut_assert_eq(id_seq, ID_INVALID, "Valid ID seq returned on invalid ID group.");
	
	ut_end;
}

int test_IdGroupGet(void)
{
	ut_run_test(test_IdGroupGet_valid);
	ut_run_test(test_IdGroupGet_invalid_id);
	ut_run_test(test_IdGroupGet_invalid_group);

	ut_end;
}

int test_IdGroupGet_valid(void)
{
	ut_start;
	
	const Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << ID_SHIFT_AMOUNT_GROUP) | 0x00002000;
	IdGroup_t id_group = IdGroupGet(id);
	ut_assert_eq(id_group, ID_GROUP_MESSAGE_QUEUE, "Incorrect group.");
	
	ut_end;
}

int test_IdGroupGet_invalid_id(void)
{
	ut_start;
	
	const Id_t id = ID_INVALID;
	IdGroup_t id_group = IdGroupGet(id);
	ut_assert_eq(id_group, ID_GROUP_INV, "Valid ID group returned on invalid ID.");
	
	ut_end;
}

int test_IdGroupGet_invalid_group(void)
{
	ut_start;
	
	Id_t id = ID_GROUP_INVALID | 0x00001000;
	IdGroup_t id_group = IdSequenceNumberGet(id);
	ut_assert_eq(id_group, ID_GROUP_INV, "Valid ID group returned on invalid ID group.");
	
	id = (ID_GROUP_NUM + 1) | 0x00001000;
	id_group = IdSequenceNumberGet(id);
	ut_assert_eq(id_group, ID_GROUP_INV, "Valid ID group returned on invalid ID group.");
	
	ut_end;
}