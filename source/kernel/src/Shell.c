#include <Shell.h>
#include <Types.h>
#include <KernelTask.h>
#include <Task.h>
#include <TaskDef.h>
#include <Ringbuffer.h>
#include <Memory.h>

#include "shellcmd/ShellCommandHelp.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define SHELL_MAX_COMMAND_COUNT     10  /* Maximum number of commands. */
#define SHELL_MAX_LINE_LENGTH       200 /* Maximum length of a single line. */
#define SHELL_MAX_TOKEN_COUNT       10  /* Maximum number of arguments per command. Max. 127. */
#define SHELL_MAX_TOKEN_LENGTH      10  /* Maximum length of each argument. */
#define SHELL_SPLIT_CHAR_COUNT      2   /* Number of split characters. Split characters are defined by SplitChars array. */
#define SHELL_MSG_START_LENGTH      5   /* Length of each message prefix. The message itself is defined by ShellMessageStart. */
#define SHELL_IGNORE_CHAR_COUNT     2

const char ShellMessageStart[SHELL_MSG_START_LENGTH+1] = {'\n', 'p', 's', 'h', '>', '\0'};
const char SplitChars[SHELL_SPLIT_CHAR_COUNT] = {'-','='};
const char IgnoreChars[SHELL_IGNORE_CHAR_COUNT] = {' ', '%'};
const char LineTerminatorChar = '\n';

char *Line;
char **Tokens;
U8_t TokenCount;

Id_t ShellPool = OS_ID_INVALID;
struct ShellCommand ShellCommandSet[SHELL_MAX_COMMAND_COUNT];
U8_t TotalShellCommands;


/* Private API. */
static void IShellReplyInvalidNumberArgs(char *command, uint8_t n_args);
static void IShellReplyInvalidCommand(char *command);
static U8_t IShellSplitLine(char *line, char **args);

S8_t IShellCommandTokensContainArgument(char **tokens, U8_t n_tokens, const char *argument);

/* Shell Tasks. */
void OsTaskShellReadParse(const void *p_arg, U32_t v_arg);
Id_t TidShellReadParse;

void OsTaskShellExecute(const void *p_arg, U32_t v_arg);


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


OsResult_t ShellCommmandHelpLock(void);
OsResult_t ShellCommandExecuteLock(char **tokens, U8_t n_tokens);
struct ShellCommand ShellCommandLock = {
    .cmd = "lock",
    .callback_init = NULL,
    .callback_help = ShellCommmandHelpLock,
    .callback_execute = ShellCommandExecuteLock,
    .min_tokens = 3,
    .max_tokens = 3
};


RingbufBase_t ShellRxBuffer[SHELL_MAX_LINE_LENGTH];

OsResult_t KShellInit(void)
{
    OsResult_t result = OS_OK;
    TokenCount = 0;

    ShellRxRingbuf = RingbufCreate(ShellRxBuffer, SHELL_MAX_LINE_LENGTH);
    ShellTxRingbuf = RingbufCreate(NULL, 200);
    // ShellPool = MemPoolCreate(200);
    if((ShellRxRingbuf == OS_ID_INVALID) || (ShellTxRingbuf == OS_ID_INVALID)) {
        RingbufDelete(&ShellRxRingbuf);
        RingbufDelete(&ShellTxRingbuf);
        result = OS_ERROR;
    }

    Line = (char *)malloc(sizeof(char) * SHELL_MAX_LINE_LENGTH);

    Tokens = malloc(sizeof (*Tokens) * SHELL_MAX_TOKEN_COUNT);
    if (Tokens != NULL) {
        for (U8_t i = 0; i < SHELL_MAX_TOKEN_COUNT; i++) {
            Tokens[i] = malloc(sizeof(*Tokens[i]) * SHELL_MAX_TOKEN_LENGTH);
        }
    }


    if(result == OS_OK) {
        TotalShellCommands = 0;
        ShellCommandRegister(&ShellCommandShellConfig);
        ShellCommandRegister(&ShellCommandHelp);
        // ShellCommandRegister(&ShellCommandRun);

        if(result == OS_OK) {
            TidShellReadParse = KernelTaskCreate(OsTaskShellReadParse, 3, TASK_PARAM_NONE, 0, NULL, 0);
            if(TidShellExecute == OS_ID_INVALID) {
                result = OS_ERROR;
            }
            if(result == OS_OK) {
                result = TaskResumeWithVarg(TidShellReadParse, 0);
            }
        }
    }

    return (result);
}

OsResult_t ShellCommandRegister(struct ShellCommand *command)
{
    if(TotalShellCommands >= (sizeof(ShellCommandSet) / sizeof(struct ShellCommand))) {
        return OS_FAIL;
    }

    ShellCommandSet[TotalShellCommands] = *command;
    TotalShellCommands++;

    return OS_OK;
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
    va_end(args);
    RingbufWrite(ShellTxRingbuf, (RingbufBase_t *)msg_buffer, &act_size, OS_TIMEOUT_INFINITE);

cleanup:
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
    va_end(args);
    act_size+=1;
    RingbufWrite(ShellTxRingbuf, (RingbufBase_t *)msg_buffer, &act_size, OS_TIMEOUT_INFINITE);

cleanup:
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
    va_end(args);
    act_size += offset;
    RingbufWrite(ShellTxRingbuf, (RingbufBase_t *)msg_buffer, &act_size, OS_TIMEOUT_INFINITE);

cleanup:
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

void OsTaskShellReadParse(const void* p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);

    static U8_t line_write_index = 0;
    static Id_t evt_rx_data_available = OS_ID_INVALID;

    U32_t read_amount = 1;

    TASK_INIT_BEGIN() {

    }
    TASK_INIT_END();

    if(TaskPoll(ShellRxRingbuf, RINGBUF_EVENT_DATA_IN, 0, true) == OS_EVENT) {
        read_amount = RingbufDataCountGet(ShellRxRingbuf);
        RingbufRead(ShellRxRingbuf, (RingbufBase_t *)&Line[line_write_index], &read_amount, OS_TIMEOUT_INFINITE);
        if(read_amount) {
            line_write_index+=(read_amount-1);
            if(Line[line_write_index] == LineTerminatorChar) {
                Line[line_write_index] = '\0';
                TokenCount = IShellSplitLine(Line, Tokens);
                TidShellExecute = TaskCreate(OsTaskShellExecute, TASK_CAT_LOW, 5, TASK_PARAM_NONE, 0, NULL, 0);
                TaskResumeWithVarg(TidShellExecute, TokenCount);
            } else {
                if(line_write_index < (SHELL_MAX_COMMAND_LENGTH - 1)) {
                    line_write_index+=read_amount;
                }
            }
        }
    }

}

void OsTaskShellExecute(const void* p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    U8_t n_tokens = v_arg;

    for(U8_t i = 0; i < TotalShellCommands; i++) {
        if(strcmp(Tokens[0], ShellCommandSet[i].cmd) == 0) {
            if((n_tokens <= ShellCommandSet[i].max_tokens) && (n_tokens >= ShellCommandSet[i].min_tokens)) {
                if(ShellCommandSet[i].callback_execute != NULL) {
                    ShellCommandSet[i].callback_execute(Tokens, n_tokens);
                    ShellPut("ok.");
                } else {
                    ShellPut("error: no callback."); /* No callback defined. */
                }
            } else {
                IShellReplyInvalidNumberArgs(Tokens[0], n_tokens);
            }
            goto task_exit;
        }
    }

    IShellReplyInvalidCommand(Tokens[0]);

task_exit:
    TaskDelete(&TidShellExecute);
}

/* Built-in Shell Command callbacks. */

OsResult_t ShellCommandExecuteConfig(char **tokens, U8_t n_tokens)
{
    return OS_OK;
}

OsResult_t ShellCommmandHelpConfig(void)
{
    ShellPut("Help for command 'cfg'");
    return OS_OK;
}




OsResult_t ShellCommandExecuteLock(char **tokens, U8_t n_tokens)
{
    /* Example: lock -o=100a -l */
    /* Token 0 will be the command itself. */

    /* Token 1 : -o=ID object to lock. */
    Id_t object = OS_ID_INVALID;
    S8_t object_arg_index = IShellCommandTokensContainArgument(tokens, n_tokens, "o");
    S8_t mode = -1; /* Not found = -1*/
    if(object_arg_index != -1) {

        // object = ConvertHexStringToId(tokens[object_arg_index]);

        /* Token 2 : -l for locking -u for unlocking*/
        mode = IShellCommandTokensContainArgument(tokens, n_tokens, "l");
        if(mode == -1) {
            mode = IShellCommandTokensContainArgument(tokens, n_tokens, "u");
            if(mode != -1) {
                /* Unlock. */

            } else {
                /* Invalid. */
                ShellReplyInvalidArgs("lock");
            }
        } else {
            /* Lock object. */

        }

        /* The object will always be locked in read mode to avoid blocking. */

    } else {
        ShellReplyInvalidArgs("lock");
    }

    return OS_OK;


}