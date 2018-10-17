#include "test_IdType.h"

#include "IdType.h"
#include "IdTypeDef.h"

int set_up(void);
int tear_down(void);

int test_KIdRequest(void);
int test_KIdRequest_invalid_group(void);
int test_KIdRequest_valid_group_single(void);
int test_KIdRequest_valid_group_all(void);
int test_KIdRequest_all(void);
int test_KIdRequest_all_plus_one(void);

int test_IdSequenceNumberGet(void);
int test_IdSequenceNumberGet_valid(void);
int test_IdSequenceNumberGet_invalid_id(void);
int test_IdSequenceNumberGet_invalid_group(void);

int test_IdGroupGet(void);
int test_IdGroupGet_invalid_group(void);
int test_IdGroupGet_invalid_id(void);
int test_IdGroupGet_valid(void);

int test_IdIsInGroup(void);
int test_IdIsInGroup_valid_true(void);
int test_IdIsInGroup_valid_false(void);
int test_IdIsInGroup_invalid_group(void);
int test_IdIsInGroup_invalid_id(void);
int test_IdIsInGroup_invalid_group_and_id(void);

int test_IdListInit(void);
int test_IdListInit_valid(void);
int test_IdListInit_invalid(void);

int test_IdListIdAdd(void);
int test_IdListIdAdd_single(void);
int test_IdListIdAdd_all(void);
int test_IdListIdAdd_all_plus_one(void);

int test_IdListIdRemove(void);
int test_IdListIdRemove_invalid_list(void);
int test_IdListIdRemove_single(void);


ut_test_set_t test_set_IdType = {
	.name = "IdType",
	.set_up = set_up,
	.tear_down = tear_down,
	.n_tests = 7,
	.tests = {
	test_KIdRequest,
	test_IdSequenceNumberGet,
	test_IdGroupGet,
	test_IdIsInGroup,
	test_IdListInit,
	test_IdListIdAdd,
	test_IdListIdRemove
	},
};

int set_up(void)
{
	KIdInit();
	ut_end;
}

int tear_down(void) 
{
	ut_end;
}

/***** Test KIdRequest *****/

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

/***** Test IdSequenceNumberGet *****/

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

/***** Test IdGroupGet *****/

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


/***** Test IdIsInGroup *****/

int test_IdIsInGroup(void)
{
	ut_run_test(test_IdIsInGroup_valid_true);
	ut_run_test(test_IdIsInGroup_valid_false);
	ut_run_test(test_IdIsInGroup_invalid_group);
	ut_run_test(test_IdIsInGroup_invalid_id);
	ut_run_test(test_IdIsInGroup_invalid_group_and_id);
	
	ut_end;
}

int test_IdIsInGroup_valid_true(void)
{
	ut_start;
	
	Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << ID_SHIFT_AMOUNT_GROUP) | 0x00002000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_MESSAGE_QUEUE);
	ut_assert_eq(in_group, 1, "ID is in this group");
	
	ut_end;
}

int test_IdIsInGroup_valid_false(void)
{
	ut_start;
	
	Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << ID_SHIFT_AMOUNT_GROUP) | 0x00002000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_EVENTGROUP);
	ut_assert_eq(in_group, 0, "ID is not in this group");
	
	ut_end;
}

int test_IdIsInGroup_invalid_group(void)
{
	ut_start;
	
	Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << ID_SHIFT_AMOUNT_GROUP) | 0x00002000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_NUM);
	ut_assert_eq(in_group, 0, "Valid ID cannot be part of an invalid group.");
	
	ut_end;
}

int test_IdIsInGroup_invalid_id(void)
{
	ut_start;
	
	Id_t id = ID_GROUP_INVALID | 0x00001000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_EVENTGROUP);
	ut_assert_eq(in_group, 0, "Invalid ID cannot be part of a valid group.");
	
	ut_end;
}

int test_IdIsInGroup_invalid_group_and_id(void)
{
	ut_start;
	
	Id_t id = ID_GROUP_INVALID | 0x00001000;
	U8_t in_group = IdIsInGroup(id, ID_GROUP_NUM);
	ut_assert_eq(in_group, 0, "Invalid ID cannot be part of an invalid group.");
	
	ut_end;
}

/***** Test IdListInit *****/

int test_IdListInit(void)
{
	ut_run_test(test_IdListInit_valid);
	ut_run_test(test_IdListInit_invalid);
	
	ut_end;
}

int test_IdListInit_valid(void)
{
	ut_start;
	
	IdList_t list;
	IdListInit(&list);
	ut_assert_eq(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		ut_assert_eq(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	ut_end;
}

int test_IdListInit_invalid(void)
{
	ut_start;
	
	IdListInit(NULL);
	
	ut_end;
}


/***** Test IdListIdAdd *****/

int test_IdListIdAdd(void)
{
	ut_run_test(test_IdListIdAdd_single);
	ut_run_test(test_IdListIdAdd_all);
	ut_run_test(test_IdListIdAdd_all_plus_one);
	
	ut_end;
}

int test_IdListIdAdd_single(void)
{
	ut_start;
	
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	ut_assert_eq(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		ut_assert_eq(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	IdListIdAdd(&list, id);
	ut_assert_eq(list.n, 1, "Expected 1 item in the ID list.");
	ut_assert_eq(list.ids[list.n - 1], id, "ID in the list is not correct.");
	
	ut_end;
}

int test_IdListIdAdd_all(void)
{
	ut_start;
	
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	ut_assert_eq(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		ut_assert_eq(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	for(uint8_t j = 0; j < ID_LIST_SIZE_MAX; j++) {
		id = j;
		IdListIdAdd(&list, id);
		ut_assert_eq(list.n, j + 1, "Expected number of items in the ID list incorrect.");
		ut_assert_eq(list.ids[list.n - 1], id, "ID in the list is incorrect.");
	}
	ut_assert_eq(list.n, ID_LIST_SIZE_MAX, "Number of IDs in the list is incorrect");
	
	ut_end;
}

int test_IdListIdAdd_all_plus_one(void)
{
	ut_start;
	
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	ut_assert_eq(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		ut_assert_eq(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	for(uint8_t j = 0; j < ID_LIST_SIZE_MAX; j++) {
		id = j;
		IdListIdAdd(&list, id);
		ut_assert_eq(list.n, j + 1, "Expected number of items in the ID list incorrect.");
		ut_assert_eq(list.ids[list.n - 1], id, "ID in the list is incorrect.");
	}
	ut_assert_eq(list.n, ID_LIST_SIZE_MAX, "Number of IDs in the list is incorrect");
	id++;
	IdListIdAdd(&list, id);
	ut_assert_eq(list.n, ID_LIST_SIZE_MAX, "Added an ID beyond the list capacity.");
	
	ut_end;
}

/***** Test IdListIdRemove *****/

int test_IdListIdRemove(void)
{
	ut_run_test(test_IdListIdRemove_invalid_list);
	ut_run_test(test_IdListIdRemove_single);
	//ut_run_test(test_IdListIdAdd_all);
	//ut_run_test(test_IdListIdAdd_all_plus_one);
	
	ut_end;
}

int test_IdListIdRemove_invalid_list(void)
{
	ut_start;
	
	Id_t id = IdListIdRemove(NULL);
	ut_assert_eq(id, ID_INVALID, "Removed valid ID from invalid list.");
	
	ut_end;
}

int test_IdListIdRemove_single(void)
{
	ut_start;
	
	Id_t id = 1;
	IdList_t list;
	IdListInit(&list);
	ut_assert_eq(list.n, ID_LIST_EMPTY, "List count was not initialized to ID_LIST_EMPTY.");
	for(uint8_t i = 0; i < ID_LIST_SIZE_MAX; i++) {
		ut_assert_eq(list.ids[i], ID_INVALID, "List IDs were not all initialized to ID_INVALID.");
	}
	
	IdListIdAdd(&list, id);
	ut_assert_eq(list.n, 1, "Expected 1 item in the ID list.");
	ut_assert_eq(list.ids[list.n - 1], id, "ID in the list is not correct.");
	
	Id_t rm_id = IdListIdRemove(&list);
	ut_assert_eq(id, rm_id, "Removed ID not equal to added ID.");
	ut_assert_eq(list.n, 0, "Number of items in the list incorrect.");
	
	ut_end;
}