#include <stdio.h>

#include "utest.h"

#include "test_IdType.h"
#include "test_List.h"

int main(int argc, char **argv)
{

	ut_init();
	ut_run_set(&test_set_IdType);
	ut_run_set(&test_set_List);
	ut_suite_report;

	while(1);
	return 0;
}
