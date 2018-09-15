/*
 * ShellCommandRun.h
 *
 * Created: 4-3-2018 1:15:23
 *  Author: Dorus
 */ 


#ifndef SHELL_COMMAND_RUN_H_
#define SHELL_COMMAND_RUN_H_

#include "include/OsTypes.h"
#include "include/Shell.h"

OsResult_t ShellCommandExecuteRun(char **tokens, U8_t n_tokens);
OsResult_t ShellCommandHelpRun(void);

extern struct ShellCommand ShellCommandRun;

#endif /* SHELL_COMMAND_RUN_H_ */
