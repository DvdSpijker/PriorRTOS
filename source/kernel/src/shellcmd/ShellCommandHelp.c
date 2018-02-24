/*
 * ShellCommandHelp.c
 *
 * Created: 16-10-2017 20:57:08
 *  Author: Dorus
 */ 

#include "ShellCommandHelp.h"

struct ShellCommand ShellCommandHelp = {
    .cmd = "help",
    .callback_init = NULL,
    .callback_help = NULL,
    .callback_execute = ShellCommandExecuteHelp,
    .min_tokens = 1,
    .max_tokens = 3
};

OsResult_t ShellCommandExecuteHelp(char **tokens, U8_t n_tokens)
{
    //if(n_tokens == 3) { /* Calling help of a specific command, not general Shell help. */
        //struct ShellCommand *cmd = ShellCommandFromName(tokens[2]);
        //if(cmd != NULL) {
            //if(cmd->callback_help != NULL) {
                //cmd->callback_help();
            //}
        //}
        //} else {
        //ShellPut("\n");
        //ShellPutRaw("---Available Shell Commands:%u---", TotalShellCommands);
        //for(U8_t i = 0; i < TotalShellCommands; i++) {
            //ShellPutRawNewline("  -%s", ShellCommandSet[i].cmd);
        //}
        //ShellPutRaw("\n--------------------------------");
    //}
    return OS_RES_OK;
}