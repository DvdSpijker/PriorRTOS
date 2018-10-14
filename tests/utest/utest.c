#include "utest.h"

uint32_t tests_run;
uint32_t tests_pass; 
uint32_t tests_fail;

ut_test_t ut_set_up = NULL;
ut_test_t ut_tear_down = NULL;

void ut_init(void)
{
	tests_run = 0;
	tests_pass = 0;
	tests_fail = 0;
}

void ut_cnt_run_inc(void)
{
	tests_run++;
}

void ut_cnt_pass_inc(void)
{
	tests_pass++;
}

void ut_cnt_fail_inc(void)
{
	tests_fail++;
}

uint32_t ut_cnt_run_get(void)
{
	return tests_run;
}

uint32_t ut_cnt_pass_get(void)
{
	return tests_pass;
}

uint32_t ut_cnt_fail_get(void)
{
	return tests_fail;
}

int ut_run_test(ut_test_t test)
{
	int res = UT_FAIL;

	if(ut_set_up) {
		ut_set_up();
	}

	res = test();
	ut_cnt_run_inc();
	
	if (res == UT_FAIL) { 
		ut_cnt_fail_inc(); 
		printf("[UTEST] FAIL\n"); 
	} else {
		 ut_cnt_pass_inc(); 
		 printf("[UTEST] PASS\n");
	}

	if(ut_tear_down) {
		ut_tear_down();
	}

	return res; 
}

int ut_run_set(ut_test_set_t *set)
{
	int res = UT_OK; 

	ut_set_up = set->set_up;
	ut_tear_down = set->tear_down;

	printf("\n[UTEST] ---Running set: %s---\n", set->name);
	for(int i = 0; i < set->n_tests; i++) {
		if(set->tests[i] != NULL) {
			res = set->tests[i]();
		}
	}

	ut_set_report;
	
	return res;
}