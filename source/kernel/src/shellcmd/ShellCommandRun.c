
#include "kernel/inc/shellcmd/ShellCommandRun.h"
#include "include/Shell.h"
#include "include/PriorRTOS.h"

#include <string.h>

struct ShellCommand ShellCommandRun = {
    .cmd = "run",
    .callback_init = NULL,
    .callback_help = ShellCommandHelpRun,
    .callback_execute = ShellCommandExecuteRun,
    .min_tokens = 2,
    .max_tokens = 2
};

/* Task Run command callback. */
OsResult_t ShellCommandExecuteRun(char **tokens, U8_t n_tokens)
{
	/* Run task by ID. */
	Id_t task_id = ConvertHexStringToId(tokens[1]);
	if(OsTaskExists(task_id)) {
		TaskNotify(task_id, 0);
	} else {
		ShellPut("Specified task does not exist.");
	}

	return OS_RES_OK;
}

OsResult_t ShellCommandHelpRun(void)
{
    ShellPut("The 'run' command can be used to run tasks manually.\nArguments:\n\t <task id hex>");
    return OS_RES_OK;
}
