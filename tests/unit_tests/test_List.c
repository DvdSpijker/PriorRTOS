#include "test_List.h"

#include "List.h"

int test_ListInit(void);
int test_ListInit_invalid_list(void);
int test_ListInit_valid_list(void);

int test_ListIsLocked(void);
int test_ListIsLocked_invalid_list(void);
int test_ListIsLocked_locked_list(void);
int test_ListIsLocked_unlocked_list(void);

int test_ListDestroy(void);
int test_ListDestroy_invalid_list(void);
int test_ListDestroy_locked_list(void);
int test_ListDestroy_empty_list(void);

static int set_up(void);
static int tear_down(void);

ut_test_set_t test_set_List = {
	.name = "List",
	.set_up = set_up,
	.tear_down = tear_down,
	.n_tests = 3,
	.tests = {
		test_ListInit,
		test_ListIsLocked,
		test_ListDestroy
	},
};

static int set_up(void)
{
	ut_end;
}

static int tear_down(void) 
{
	ut_end;
}

/***** Test ListInit *****/

int test_ListInit(void)
{
	ut_run_test(test_ListInit_invalid_list);
	ut_run_test(test_ListInit_valid_list);
	
	ut_end;
}

int test_ListInit_invalid_list(void)
{
	ut_start;
	
	OsResult_t res = ListInit(NULL, ID_GROUP_MESSAGE_QUEUE);
	ut_assert_eq(res, OS_RES_INVALID_ARGUMENT, "Wrong result code on invalid list.");
	
	ut_end;
}

int test_ListInit_valid_list(void)
{
	ut_start;
	
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;

	res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ut_assert_eq(res, OS_RES_OK, "Wrong result code on valid list.");
	ut_assert_eq(list.head, NULL, "Head has a non-initialized value.");
	ut_assert_eq(list.tail, NULL, "Tail has a non-initialized value.");
	ut_assert_eq(list.id_group, ID_GROUP_MESSAGE_QUEUE, "ID group has a non-initialized value.");
	ut_assert_eq(list.size, 0, "List size is not 0.");
	ut_assert_eq(list.lock, 0, "List is in a locked state.");

	ut_end;
}

/***** Test ListIsLocked *****/

int test_ListIsLocked(void)
{
	ut_run_test(test_ListIsLocked_invalid_list);
	ut_run_test(test_ListIsLocked_locked_list);
	ut_run_test(test_ListIsLocked_unlocked_list);

	ut_end;
}

int test_ListIsLocked_invalid_list(void)
{
	ut_start;

	bool res = ListIsLocked(NULL);
	ut_assert_eq(res, false, "Wrong result code on invalid list.");

	ut_end;
}

int test_ListIsLocked_locked_list(void)
{
	ut_start;

	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ut_assert_eq(res, OS_RES_OK, "Wrong result code on valid list.");
	
	list.lock = 1;
	
	bool locked = ListIsLocked(&list);
	ut_assert_eq(locked, true, "Wrong result code on locked list.");

	ut_end;
}

int test_ListIsLocked_unlocked_list(void)
{
	ut_start;

	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ut_assert_eq(res, OS_RES_OK, "Wrong result code on valid list.");
	
	bool locked = ListIsLocked(&list);
	ut_assert_eq(locked, false, "Wrong result code on unlocked list.");

	ut_end;
}

/***** Test ListDestroy *****/

int test_ListDestroy(void)
{
	ut_run_test(test_ListDestroy_invalid_list);
	ut_run_test(test_ListDestroy_locked_list);
	ut_run_test(test_ListDestroy_empty_list);
	
	ut_end;
}

int test_ListDestroy_invalid_list(void)
{
	ut_start;

	OsResult_t res = ListDestroy(NULL);
	ut_assert_eq(res, OS_RES_INVALID_ARGUMENT, "Wrong result code on invalid list.");

	ut_end;
}

int test_ListDestroy_locked_list(void)
{
	ut_start;
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ut_assert_eq(res, OS_RES_OK, "Wrong result code on valid list.");
	
	list.lock = 1;
	
	res = ListDestroy(&list);
	ut_assert_eq(res, OS_RES_LOCKED, "Wrong result code on locked list.");

	ut_end;
}

int test_ListDestroy_empty_list(void)
{
	ut_start;
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	ut_assert_eq(res, OS_RES_OK, "Wrong result code on valid list.");
	
	res = ListDestroy(&list);
	ut_assert_eq(res, OS_RES_OK, "Wrong result code on empty list.");
	ut_assert_eq(ListIsLocked(&list), true, "Detroyed list must be locked.");
	ut_assert_eq(list.head, NULL, "Head not deinitialized.");
	ut_assert_eq(list.tail, NULL, "Tail not deinitialized.");
	ut_assert_eq(list.size, 0, "Destroyed list must be size 0.");
	
	ut_end;
}