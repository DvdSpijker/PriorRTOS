#include <Shell.h>
#include <PriorRTOS.h>
#include <ShellCommandRun.h>
#include <string.h>

struct ShellCommand ShellCommandRun = {
    .cmd = "run",
    .callback_init = NULL,
    .callback_help = ShellCommandHelpRun,
    .callback_execute = ShellCommandExecuteRun,
    .min_tokens = 3,
    .max_tokens = 4
};

/* Task Run command callback. */
OsResult_t ShellCommandExecuteRun(char **tokens, U8_t n_tokens)
{
    if(strcmp(tokens[1], "t") == 0) {
        /* Run task by ID. */
        Id_t task_id = ConvertHexStringToId(tokens[2]);
        if(OsTaskExists(task_id)) {
            TaskResumeWithVarg(task_id, 0);
        } else {
            ShellPut("Specified task does not exist.");
        }
    }  else {
        /* Invalid args. */
        ShellReplyInvalidArgs("run");
    }

    return OS_RES_OK;
}

OsResult_t ShellCommandHelpRun(void)
{
    ShellPut("The 'run' command can be used to run tasks manually.\nArguments:\n\t -i=<task id integer> \n\t -x=<task id hex>");
    return OS_RES_OK;
}
