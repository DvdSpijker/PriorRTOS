#include "include/Shell.h"

#include "kernel/inc/KernelTask.h"
#include "include/Task.h"
#include "kernel/inc/TaskDef.h"
#include "include/Ringbuffer.h"
#include "include/Memory.h"
#include "include/OsTypes.h"

#include "kernel/inc/shellcmd/ShellCommandHelp.h"
#include "kernel/inc/shellcmd/ShellCommandRun.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

//#define SHELL_CONFIG_USE_TX_RINGBUF

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
#ifdef SHELL_CONFIG_USE_TX_RINGBUF
Id_t ShellTxRingbuf;
#endif
Id_t ShellPool = ID_INVALID;
struct ShellCommand ShellCommandSet[SHELL_MAX_COMMAND_COUNT];
U8_t TotalShellCommands;


/* Private API. */
static void IShellReplyInvalidNumberArgs(char *command, uint8_t n_args);
static void IShellReplyInvalidCommand(char *command);
static U8_t IShellSplitLine(char *line, char **args);

S8_t IShellCommandTokensContainArgument(char **tokens, U8_t n_tokens, const char *argument);

/* Shell Tasks. */
void TaskShellParse(const void *p_arg, U32_t v_arg);
Id_t TidShellParse;

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
#ifdef SHELL_CONFIG_USE_TX_RINGBUF
    ShellTxRingbuf = RingbufCreate(NULL, 50);
#endif
    ShellPool = MemPoolCreate(SHELL_MAX_TOKEN_COUNT * SHELL_MAX_TOKEN_LENGTH * 2);

    if((ShellRxRingbuf == ID_INVALID) || (ShellPool == ID_INVALID)) {
        RingbufDelete(&ShellRxRingbuf);
        MemPoolDelete(ShellPool);
        result = OS_RES_ERROR;
    }

#ifdef SHELL_CONFIG_USE_TX_RINGBUF
    if(ShellTxRingbuf == ID_INVALID) {
        RingbufDelete(&ShellRxRingbuf);
        MemPoolDelete(ShellPool);
    	RingbufDelete(&ShellTxRingbuf);
    	result = OS_RES_ERROR;
    }
#endif

    if(result == OS_RES_OK) {
        TotalShellCommands = 0;
        ShellCommandRegister(&ShellCommandShellConfig);
        ShellCommandRegister(&ShellCommandHelp);
        ShellCommandRegister(&ShellCommandRun);

        if(result == OS_RES_OK) {
        	TidShellParse = KernelTaskCreate(TaskShellParse, 3, TASK_PARAMETER_NONE, 0, NULL, 0);
            if(TidShellParse == ID_INVALID) {
                result = OS_RES_ERROR;
            }
            if(result == OS_RES_OK) {
                result = TaskNotify(TidShellParse, 0);
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
	U32_t act_size;
    va_list args;
    va_start(args, message);

#ifdef SHELL_CONFIG_USE_TX_RINGBUF
    U32_t est_size = 30;
    char *msg_buffer = malloc(est_size);

    act_size = vsnprintf(msg_buffer, est_size, message, args);
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
        free(msg_buffer);
#else
	act_size = vprintf(message, args);
#endif
	va_end(args);
	return act_size;
}

U16_t ShellPutRawNewline(char *message, ...)
{
	U32_t act_size;
    va_list args;
    va_start(args, message);

#ifdef SHELL_CONFIG_USE_TX_RINGBUF
    U32_t est_size = 30;
    char *msg_buffer = malloc(est_size);
    msg_buffer[0] = '\n';
    act_size = vsnprintf(&msg_buffer[1], (est_size - 1), message, args);
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
        free(msg_buffer);
#else
	printf("\n");
	act_size = vprintf(message, args);
#endif
	va_end(args);
	return act_size;
}

U16_t ShellPut(char *message, ...)
{
	U32_t act_size;
    va_list args;
    va_start(args, message);

#ifdef SHELL_CONFIG_USE_TX_RINGBUF
    U32_t est_size = 30;
    U32_t offset = 0;
    char *msg_buffer = malloc(est_size);
    for (; offset < SHELL_MSG_START_LENGTH; offset++) {
        msg_buffer[offset] = ShellMessageStart[offset];
    }
    act_size = vsnprintf(&msg_buffer[offset], (est_size - offset), message, args);
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
    free(msg_buffer);
#else
    printf("%s", ShellMessageStart);
    act_size = vprintf(message, args);
#endif
    va_end(args);
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

    while(line[i] != '\0' && i < SHELL_MAX_LINE_LENGTH) {
        for(U8_t j = 0; j < SHELL_SPLIT_CHAR_COUNT; j++) {
            if(line[i] == SplitChars[j]) {
                split = 1;
                break;
            }
        }
        if(split) {
            args[token_cnt][token_w_index] = '\0';
            if(++token_cnt > SHELL_MAX_TOKEN_LENGTH) {
            	token_cnt--;
            	break;
            }
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
                if(++token_w_index > SHELL_MAX_TOKEN_COUNT) {
                	token_w_index--;
                	break;
                }
            }
        }
        i++;
    }

    args[token_cnt][token_w_index] = '\0';

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

void TaskShellParse(const void* p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);

    static U8_t line_write_index = 0;
    static char LineBuffer[SHELL_MAX_LINE_LENGTH] = {0};

    U32_t read_amount = 1;
	OsResult_t result = OS_RES_ERROR;
	

    TASK_INIT_BEGIN() {

    }
    TASK_INIT_END();

    result = TaskPoll(ShellRxRingbuf, RINGBUF_EVENT_DATA_IN, OS_TIMEOUT_INFINITE, true);
	if( (result == OS_RES_EVENT) || (result == OS_RES_POLL) ) {
		read_amount = RingbufDataCountGet(ShellRxRingbuf);
		RingbufRead(ShellRxRingbuf, (RingbufBase_t *)&LineBuffer[line_write_index], &read_amount, OS_TIMEOUT_INFINITE);
		if(read_amount) {
			line_write_index+=(read_amount-1);
			if(LineBuffer[line_write_index] == LineTerminatorChar) {
				LineBuffer[line_write_index] = '\0';
				char **tokens = (char **)MemAlloc(ShellPool, SHELL_MAX_TOKEN_COUNT);
				for(U32_t i = 0; i < SHELL_MAX_TOKEN_COUNT; i++) {
					tokens[i] = (char *)MemAlloc(ShellPool, SHELL_MAX_TOKEN_LENGTH);
				}
				if(tokens == NULL) {
					ShellPut("No resources available to execute command.");
				} else {
					U8_t n_tokens = IShellSplitLine(LineBuffer, tokens);
					if(TaskCreate(TaskShellExecute, TASK_CAT_LOW, 5, TASK_PARAMETER_START, 0, (void *)tokens, n_tokens) == ID_INVALID) {
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

void PortDebugCallbackReadChars(char *chars, U32_t n)
{
	RingbufWrite(ShellRxRingbuf, (RingbufBase_t *)chars, &n, OS_TIMEOUT_NONE);
}

/* Config command. */

OsResult_t ShellCommandExecuteConfig(char **tokens, U8_t n_tokens)
{
    return OS_RES_OK;
}

OsResult_t ShellCommmandHelpConfig(void)
{
    ShellPut("Help for command 'cfg'");
    return OS_RES_OK;
}
