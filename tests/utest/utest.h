 #ifndef UTEST_H_
 #define UTEST_H_
 
 #include <stdio.h>
 #include <stdint.h>
 
 #define UT_OK		0
 #define UT_FAIL	-1
 
 typedef int (*ut_test_t)(void);
 
 typedef struct {
	const char *name;
	ut_test_t set_up;
	ut_test_t tear_down;
	int n_tests;
	ut_test_t tests[];
 }ut_test_set_t;

void ut_init(void);

void ut_cnt_run_inc(void);
void ut_cnt_pass_inc(void);
void ut_cnt_fail_inc(void);

uint32_t ut_cnt_sets_get(void);
uint32_t ut_cnt_run_get(void);
uint32_t ut_cnt_pass_get(void);
uint32_t ut_cnt_fail_get(void);

int ut_run_test(ut_test_t test);
int ut_run_set(ut_test_set_t *set);

#define ut_suite_report printf("\n[UTEST] ---Suite Report---\nSets:\t%u\nTotal:\t%u\nPassed:\t%u\nFailed:\t%u\n", ut_cnt_sets_get(), ut_cnt_run_get(), ut_cnt_pass_get(),  ut_cnt_fail_get());
#define ut_set_report(curr_run, curr_passed, curr_failed) printf("\n[UTEST] ---Set Report---\nTotal:\t%u\nPassed:\t%u\nFailed:\t%u\n\n", ut_cnt_run_get() - curr_run, ut_cnt_pass_get() - curr_passed,  ut_cnt_fail_get() - curr_failed);
#define ut_start printf("\n[UTEST] ---Running test: %s---\n", __FUNCTION__);
#define ut_end return UT_OK;
#define ut_assert_eq(a,b,err_msg) do { if (a != b) { printf("[assert_eq][%u] %s\n", __LINE__, err_msg); return UT_FAIL; } } while (0)
#define ut_assert_neq(a,b,err_msg) do { if (a == b) { printf("[assert_neq][%u] %s\n", __LINE__, err_msg); return UT_FAIL; } } while (0)

#define ut_test_set_size(test_set) (sizeof(test_set) / sizeof(ut_test_t))


 #endif