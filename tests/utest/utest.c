#include "utest.h"

uint32_t tests_run;
uint32_t tests_pass; 
uint32_t tests_fail;

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
	
	ut_set_up();
	
	res = test();
	ut_cnt_run_inc();
	
	if (res == UT_FAIL) { 
		ut_cnt_fail_inc(); 
		printf("[UTEST] FAIL\n"); 
	} else {
		 ut_cnt_pass_inc(); 
		 printf("[UTEST] PASS\n");
	}
	
	ut_tear_down(); 
	return res; 
}


#define ut_run_test(test) do { ut_set_up(); int res = test(); ut_cnt_run_inc(); \
                                if (res == UT_FAIL) { ut_cnt_fail_inc(); ut_tear_down(); printf("[UTEST] FAIL\n"); return res; } else {}  } while (0)

int ut_run_set(const char *name, ut_test_t *ut_test_set, int n_tests)
{
	int res = UT_OK; 
	
	printf("\n[UTEST] ---Running set: %s---\n", name);
	for(int i = 0; i < n_tests; i++) {
		if(ut_test_set[i] != NULL) {
			res = ut_test_set[i]();
		}
	}
	
	ut_set_report;
	
	return res;
}

__attribute__((weak))
int ut_set_up(void)
{
	return UT_OK;
}

__attribute__((weak))
int ut_tear_down(void) 
{
	return UT_OK;
}