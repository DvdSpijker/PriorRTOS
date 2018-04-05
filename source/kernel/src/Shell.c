#include <Shell.h>
#include <Types.h>
#include <KernelTask.h>
#include <Task.h>
#include <TaskDef.h>
#include <Ringbuffer.h>
#include <Memory.h>

#include "shellcmd/ShellCommandHelp.h"
#include "shellcmd/ShellCommandRun.h"
//#include "shellcmd/ShellCommandHelp.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define SHELL_MAX_COMMAND_COUNT     10  /* Maximum number of commands. */
#define SHELL_MAX_LINE_LENGTH       200 /* Maximum length of a single line. */
#define SHELL_MAX_TOKEN_COUNT       10  /* Maximum number of arguments per command. Max. 127. */
#define SHELL_MAX_TOKEN_LENGTH      10  /* Maximum length of each argument. */
#define SHELL_SPLIT_CHAR_COUNT      1   /* Number of split characters. Split characters are defined by SplitChars array. */
#define SHELL_MSG_START_LENGTH      5   /* Length of each message prefix. The message itself is defined by ShellMessageStart. */
#define SHELL_IGNORE_CHAR_COUNT     1

const char ShellMessageStart[SHELL_MSG_START_LENGTH+1] = {'\n', 'p', 's', 'h', '>', '\0'};
const char SplitChars[SHELL_SPLIT_CHAR_COUNT] = {' '};
const char IgnoreChars[SHELL_IGNORE_CHAR_COUNT] = {'='};
const char LineTerminatorChar = '\n';

//char TokenBuffer[SHELL_MAX_TOKEN_COUNT][SHELL_MAX_TOKEN_LENGTH];

Id_t ShellRxRingbuf;
Id_t ShellTxRingbuf;
Id_t ShellPool = OS_ID_INVALID;
struct ShellCommand ShellCommandSet[SHELL_MAX_COMMAND_COUNT];
U8_t TotalShellCommands;


/* Private API. */
static void IShellReplyInvalidNumberArgs(char *command, uint8_t n_args);
static void IShellReplyInvalidCommand(char *command);
static U8_t IShellSplitLine(char *line, char **args);

S8_t IShellCommandTokensContainArgument(char **tokens, U8_t n_tokens, const char *argument);

/* Shell Tasks. */
void TaskShellReadParse(const void *p_arg, U32_t v_arg);
Id_t TidShellReadParse;

void TaskShellExecute(const void *p_arg, U32_t v_arg);


/* Built-in shell commands. */
OsResult_t ShellCommmandHelpConfig(void);
OsResult_t ShellCommandExecuteConfig(char **tokens, U8_t n_tokens);
struct ShellCommand ShellCommandShellConfig = {
    .cmd = "cfg",
    .callback_init = NULL,
    .callback_help = ShellCommmandHelpConfig,
    .callback_execute = ShellCommandExecuteConfig,
    .min_tokens = 1,
    .max_tokens = 4
};


RingbufBase_t ShellRxBuffer[SHELL_MAX_LINE_LENGTH];

OsResult_t KShellInit(void)
{
    OsResult_t result = OS_RES_OK;

    ShellRxRingbuf = RingbufCreate(ShellRxBuffer, SHELL_MAX_LINE_LENGTH * 2);
    ShellTxRingbuf = RingbufCreate(NULL, 50);
    ShellPool = MemPoolCreate(SHELL_MAX_TOKEN_COUNT * SHELL_MAX_TOKEN_COUNT * 2);
    if((ShellRxRingbuf == OS_ID_INVALID) || (ShellTxRingbuf == OS_ID_INVALID)) {
        RingbufDelete(&ShellRxRingbuf);
        RingbufDelete(&ShellTxRingbuf);
        result = OS_RES_ERROR;
    }

    if(result == OS_RES_OK) {
        TotalShellCommands = 0;
        ShellCommandRegister(&ShellCommandShellConfig);
        ShellCommandRegister(&ShellCommandHelp);
        ShellCommandRegister(&ShellCommandRun);

        if(result == OS_RES_OK) {
            TidShellReadParse = KernelTaskCreate(TaskShellReadParse, 3, TASK_PARAM_NONE, 0, NULL, 0);
            if(TidShellReadParse == OS_ID_INVALID) {
                result = OS_RES_ERROR;
            }
            if(result == OS_RES_OK) {
                result = TaskResumeWithVarg(TidShellReadParse, 0);
            }
        }
    }

    return (result);
}

OsResult_t ShellCommandRegister(struct ShellCommand *command)
{
    if(TotalShellCommands >= (sizeof(ShellCommandSet) / sizeof(struct ShellCommand))) {
        return OS_RES_FAIL;
    }

    ShellCommandSet[TotalShellCommands] = *command;
    TotalShellCommands++;

    return OS_RES_OK;
}

S8_t IShellCommandTokensContainArgument(char **tokens, U8_t n_tokens, const char *argument)
{
    S8_t index = -1;
    for (U8_t i = 0; i < n_tokens; i++) {
        if(strcmp((const char *)tokens[i], argument) == 0) {
            index = i;
            break;
        }
    }

    return index;
}

U16_t ShellPutRaw(char *message, ...)
{
    va_list args;
    va_start(args, message);
    U32_t est_size = 30;
    char *msg_buffer = malloc(est_size);

    U32_t act_size = vsnprintf(msg_buffer, est_size, message, args);
    if(act_size > est_size) {
        msg_buffer = realloc(msg_buffer, act_size);
        if(msg_buffer == NULL) {
            act_size = 0;
            goto cleanup;
        }
        vsnprintf(msg_buffer, act_size, message, args);
    }
    RingbufWrite(ShellTxRingbuf, (RingbufBase_t *)msg_buffer, &act_size, OS_TIMEOUT_INFINITE);

cleanup:
	va_end(args);
    free(msg_buffer);
    return act_size;
}

U16_t ShellPutRawNewline(char *message, ...)
{
    va_list args;
    va_start(args, message);
    U32_t est_size = 30;
    char *msg_buffer = malloc(est_size);
    msg_buffer[0] = '\n';
    U32_t act_size = vsnprintf(&msg_buffer[1], (est_size - 1), message, args);
    if(act_size > (est_size - 1)) {
        msg_buffer = realloc(msg_buffer, (act_size+1));
        if(msg_buffer == NULL) {
            act_size = 0;
            goto cleanup;
        }
        vsnprintf(msg_buffer, act_size, message, args);
        vsnprintf(&msg_buffer[1], (act_size-1), message, args);
    }
    act_size+=1;
    RingbufWrite(ShellTxRingbuf, (RingbufBase_t *)msg_buffer, &act_size, OS_TIMEOUT_INFINITE);

cleanup:
	va_end(args);
    free(msg_buffer);
    return act_size;
}

U16_t ShellPut(char *message, ...)
{
    va_list args;
    va_start(args, message);
    U32_t est_size = 30;
    U32_t offset = 0;
    char *msg_buffer = malloc(est_size);
    for (; offset < SHELL_MSG_START_LENGTH; offset++) {
        msg_buffer[offset] = ShellMessageStart[offset];
    }
    U32_t act_size = vsnprintf(&msg_buffer[offset], (est_size - offset), message, args);
    if(act_size > (est_size - offset)) {
        msg_buffer = realloc(msg_buffer, (act_size + offset));
        if(msg_buffer == NULL) {
            act_size = 0;
            goto cleanup;
        }
        vsnprintf(&msg_buffer[offset], (act_size-offset), message, args);
    }
    act_size += offset;
    RingbufWrite(ShellTxRingbuf, (RingbufBase_t *)msg_buffer, &act_size, OS_TIMEOUT_INFINITE);

cleanup:
    va_end(args);
    free(msg_buffer);
    return act_size;
}

static void IShellReplyInvalidNumberArgs(char *command, uint8_t n_args)
{
    ShellPut("Command '%s' has an invalid number (%u) of arguments.", command, n_args);
}

void ShellReplyInvalidArgs(char *command)
{
    ShellPut("Command '%s' has invalid arguments.", command);
}

static void IShellReplyInvalidCommand(char *command)
{
    ShellPut("Invalid command '%s'.", command);
}


/* Internal functions. */

static U8_t IShellSplitLine(char *line, char **args)
{
    U8_t token_cnt = 0;
    U8_t token_w_index = 0;
    U8_t i = 0;
    U8_t split = 0;

    while(line[i] != '\0') {
        for(U8_t j = 0; j < SHELL_SPLIT_CHAR_COUNT; j++) {
            if(line[i] == SplitChars[j]) {
                split = 1;
                break;
            }
        }
        if(split) {
            args[token_cnt][token_w_index] = '\0';
            token_cnt++;
            token_w_index = 0;
            split = 0;
        } else {
            U8_t store = 1;
            for(U8_t j = 0; j < SHELL_IGNORE_CHAR_COUNT; j++) {
                if(line[i] == IgnoreChars[j]) {
                    store = 0;
                    break;
                }
            }
            if(store) {
                args[token_cnt][token_w_index] = line[i];
                token_w_index++;
            }
        }
        i++;
    }

    return (token_cnt+1);
}

struct ShellCommand *ShellCommandFromName(char *name)
{
    struct ShellCommand *cmd = NULL;
    for(U8_t i = 0; i < TotalShellCommands; i++) {
        if(strcmp(ShellCommandSet[i].cmd, name) == 0) {
            cmd = &ShellCommandSet[i];
        }
    }

    return cmd;
}

/* Shell Tasks. */

void TaskShellReadParse(const void* p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);

    static U8_t line_write_index = 0;
    static Id_t evt_rx_data_available = OS_ID_INVALID;

    U32_t read_amount = 1;
	
	static char LineBuffer[SHELL_MAX_LINE_LENGTH] = {0};

    TASK_INIT_BEGIN() {

    }
    TASK_INIT_END();

    if(TaskPoll(ShellRxRingbuf, RINGBUF_EVENT_DATA_IN, 0, true) == OS_RES_EVENT) {
        read_amount = RingbufDataCountGet(ShellRxRingbuf);
        RingbufRead(ShellRxRingbuf, (RingbufBase_t *)&LineBuffer[line_write_index], &read_amount, OS_TIMEOUT_INFINITE);
        if(read_amount) {
            line_write_index+=(read_amount-1);
            if(LineBuffer[line_write_index] == LineTerminatorChar) {
                LineBuffer[line_write_index] = '\0';
				char **tokens = (char **)MemAlloc(ShellPool, SHELL_MAX_TOKEN_COUNT * SHELL_MAX_TOKEN_LENGTH);
				if(tokens == NULL) {
					ShellPut("No resources available to execute command.");
				} else {
					U8_t n_tokens = IShellSplitLine(LineBuffer, tokens);
					if(TaskCreate(TaskShellExecute, TASK_CAT_LOW, 5, TASK_PARAM_START, 0, (void *)tokens, n_tokens) == OS_ID_INVALID) {
						ShellPut("Failed to create task to execute command.");	
					}
				}
				memset(LineBuffer, 0, SHELL_MAX_LINE_LENGTH);
            } else {
                if(line_write_index < (SHELL_MAX_COMMAND_LENGTH - 1)) {
                    line_write_index+=read_amount;
                } else {
					ShellPut("Invalid command.");
					memset(LineBuffer, 0, SHELL_MAX_LINE_LENGTH);
				}
            }
        }
    }

}

void TaskShellExecute(const void* p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    U8_t n_tokens = v_arg;
	char **tokens = (char **)p_arg;
	Id_t tid = TaskIdGet();
	
    for(U8_t i = 0; i < TotalShellCommands; i++) { /* Loop through all available shell commands. */
        if(strcmp(tokens[0], ShellCommandSet[i].cmd) == 0) { /* tokens[0] always contains the command name. */
            if((n_tokens <= ShellCommandSet[i].max_tokens) && (n_tokens >= ShellCommandSet[i].min_tokens)) { /* Check token bounds. */
                if(ShellCommandSet[i].callback_execute != NULL) {
                    ShellCommandSet[i].callback_execute(tokens, n_tokens);
                    ShellPut("ok.");
                } else {
                    ShellPut("error: no callback."); /* No callback defined. */
                }
            } else {
                IShellReplyInvalidNumberArgs(tokens[0], n_tokens);
            }
            goto task_exit;
        }
    }
	
    IShellReplyInvalidCommand(tokens[0]); /* Only called if no command was found. */

task_exit:
	MemFree((void **)tokens);
    TaskDelete(&tid);
}

OsResult_t ShellCommandExecuteConfig(char **tokens, U8_t n_tokens)
{
    return OS_RES_OK;
}

OsResult_t ShellCommmandHelpConfig(void)
{
    ShellPut("Help for command 'cfg'");
    return OS_RES_OK;
}
