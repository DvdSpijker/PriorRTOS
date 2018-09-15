#ifndef SHELL_H_
#define SHELL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "OsTypes.h"

#define SHELL_MAX_COMMAND_LENGTH 10 /* Maximum length of the command. */

/* Available shell commands:
 * ------------------------------------------------------------------------------------ *
 * command		| arguments		| optional	| description							  	*
 * ------------------------------------------------------------------------------------ *
 * help			| <command>		| 	X		| Shows all the available commands, or		*
 * 											| shows the help msg of the specified		*
 * 											| command.									*
 * ------------------------------------------------------------------------------------ *
 * run			| <task id hex> |			| Runs the specified task if able.			*
 * ------------------------------------------------------------------------------------ *
 *
 *
 * */

typedef OsResult_t (*ShellCommandExecuteCallback_t)(char **tokens, U8_t n_tokens);
typedef OsResult_t (*ShellCommandGenericCallback_t)(void);

/* ShellCommand struct.
 * Defines the command, callbacks and token counts.
 * All fields have to be initialized. Unused callbacks
 * should be set to NULL.
 */
struct ShellCommand {
    char cmd[SHELL_MAX_COMMAND_LENGTH];             /* Command string. */
    ShellCommandGenericCallback_t callback_init;    /* Additional initialization required for the command. */
    ShellCommandGenericCallback_t callback_help;    /* Must print the help command.
                                                     * Called when help is called with the command's name.  */
    ShellCommandExecuteCallback_t callback_execute; /* Called when the command is called and the token count is within limits. */
    U8_t min_tokens;                                /* Minimum amount of tokens to parse the command as valid. */
    U8_t max_tokens;                                /* Maximum amount of tokens to parse the command as valid. */
};


/******************************************************************************
 * @func: OsResult_t ShellCommandRegister(struct ShellCommand *command)
 *
 * @desc: Registers a Shell command making available for calling using
 * the CLI.
 *
 * @argin: (struct ShellCommand *) command; Initialized ShellCommand structure
 * that defines the command, callbacks and token counts.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the command was registered.
 * @retval:   OS_RES_FAIL; if the maximum amount of registered commands has been
 * reached.
 ******************************************************************************/
OsResult_t ShellCommandRegister(struct ShellCommand *command);


/******************************************************************************
 * @func: void ShellReplyInvalidArgs(char *command)
 *
 * @desc: Should be called by the command callbacks when the contents of
 * the arguments are invalid. The user will be informed with this message:
 * "Command '<command>' has invalid arguments".
 *
 * @argin: (char *command) command; Command in string form.
 ******************************************************************************/
void ShellReplyInvalidArgs(char *command);

/******************************************************************************
 * @func: U16_t ShellPut(char *message, ...)
 *
 * @desc: Prints a message starting on a new line prefixed with 'psh>'.
 *
 * @argin: (char *) message; Message in string form.
 * @argin: (...) variable arguments; -
 *
 * @rettype:  (U16_t); Number of characters
 * @retval:   0; if no characters were printed because the buffer could not
 * process the requested amount.
 * @retval:   Other; valid number of characters.
 ******************************************************************************/
U16_t ShellPut(char *message, ...);

/******************************************************************************
 * @func: U16_t ShellPutRaw(char *message, ...)
 *
 * @desc: Prints the exact message.
 *
 * @argin: (char *) message; Message in string form.
 * @argin: (...) variable arguments; -
 *
 * @rettype:  (U16_t); Number of characters
 * @retval:   0; if no characters were printed because the buffer could not
 * process the requested amount.
 * @retval:   Other; valid number of characters.
 ******************************************************************************/
U16_t ShellPutRaw(char *message, ...);

/******************************************************************************
 * @func: U16_t ShellPutRawNewline(char *message, ...)
 *
 * @desc: Prints the exact message on a new line.
 *
 * @argin: (char *) message; Message in string form.
 * @argin: (...) variable arguments; -
 *
 * @rettype:  (U16_t); Number of characters
 * @retval:   0; if no characters were printed because the buffer could not
 * process the requested amount.
 * @retval:   Other; valid number of characters.
 ******************************************************************************/
U16_t ShellPutRawNewline(char *message, ...);

/******************************************************************************
 * @func: struct ShellCommand *ShellCommandFromName(char *name)
 *
 * @desc: Get the ShellCommand struct from the (NULL terminated) command name.
 *
 * @argin: (char *) name; NULL terminated command name.
 *
 * @rettype:  (struct ShellCommand); ShellCommand struct belonging to the name.
 * @retval:  NULL; no ShellCommand matches the name.
 * @retval:   Other; valid ShellCommand.
 ******************************************************************************/
struct ShellCommand *ShellCommandFromName(char *name);


#ifdef __cplusplus
}
#endif
#endif
