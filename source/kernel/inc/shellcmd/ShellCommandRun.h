/*
 * ShellCommandRun.h
 *
 * Created: 4-3-2018 1:15:23
 *  Author: Dorus
 */ 


#ifndef SHELL_COMMAND_RUN_H_
#define SHELL_COMMAND_RUN_H_

#include <OsTypes.h>
#include <Shell.h>
#include <stdlib.h>

OsResult_t ShellCommandExecuteRun(char **tokens, U8_t n_tokens);
OsResult_t ShellCommandHelpRun(void);

extern struct ShellCommand ShellCommandRun;

#endif /* SHELL_COMMAND_RUN_H_ */