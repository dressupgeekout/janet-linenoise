/*
 * dressupgeekout's Janet binding for linenoise
 */

#include <stdlib.h>

#include <janet.h>

#include "linenoise.h"

/* ********** */

static Janet j_linenoise(int32_t argc, Janet *argv);
static Janet j_linenoiseHistoryAdd(int32_t argc, Janet *argv);
static Janet j_linenoiseHistorySetMaxLen(int32_t argc, Janet *argv);
static Janet j_linenoiseHistorySave(int32_t argc, Janet *argv);
static Janet j_linenoiseHistoryLoad(int32_t argc, Janet *argv);
static Janet j_linenoiseClearScreen(int32_t argc, Janet *argv);
static Janet j_linenoiseSetMultiLine(int32_t argc, Janet *argv);
static Janet j_linenoiseSetCompletionCallback(int32_t argc, Janet *argv);
static Janet j_linenoiseAddCompletion(int32_t argc, Janet *argv);

static const JanetReg functions[] = {
	{
		"linenoise",
		j_linenoise,
		"(linenoise/linenoise prompt)\n\n"
		"The high level function that is the main API of the linenoise library. "
		"This function checks if the terminal has basic capabilities, just checking "
		"for a blacklist of stupid terminals, and later either calls the line "
		"editing function or uses dummy fgets() so that you will be able to type "
		"something even in the most desperate of the conditions."
	},
	{
		"history-add",
		j_linenoiseHistoryAdd,
		"(linenoise/history-add line)\n\n"
		"This is the API call to add a new entry in the linenoise history. "
		"It uses a fixed array of char pointers that are shifted (memmoved) "
		"when the history max length is reached in order to remove the older "
		"entry and make room for the new one, so it is not exactly suitable for huge "
		"histories, but will work well for a few hundred of entries.\n\n"
		"Using a circular buffer is smarter, but a bit more complex to handle."
	},
	{
		"history-set-max-len",
		j_linenoiseHistorySetMaxLen,
		"(linenoise/history-set-max-len n)\n\n"
	},
	{
		"history-save",
		j_linenoiseHistorySave,
		"(linenoise/history-save filename)\n\n"
	},
	{
		"history-load",
		j_linenoiseHistoryLoad,
		"(linenoise/history-load filename)\n\n"
	},
	{
		"clear-screen",
		j_linenoiseClearScreen,
		"(linenoise/clear-screen)\n\n"
	},
	{
		"set-multi-line",
		j_linenoiseSetMultiLine,
		"(linenoise/set-multi-line boolean)\n\n"
		"Set if to use or not the multi line mode."
	},
	{
		"set-completion-callback",
		j_linenoiseSetCompletionCallback,
		"(linenoise/set-completion-callback fn)\n\n"
	},
	{
		"add-completion",
		j_linenoiseAddCompletion,
		"(linenoise/add-completion list completions)\n\n"
	},
	{NULL, NULL, NULL}
};

/* ********** */

static Janet
j_linenoise(int32_t argc, Janet *argv)
{
	janet_fixarity(argc, 1);
	const char *prompt = janet_getcstring(argv, 0);

	char *result = linenoise(prompt);

	if (result) {
		Janet string_value = janet_wrap_string(janet_cstring(result));
		free(result);
		return string_value;
	} else {
		return janet_wrap_nil();
	}
}


static Janet
j_linenoiseHistoryAdd(int32_t argc, Janet *argv)
{
	janet_fixarity(argc, 1);
	const char *line = janet_getcstring(argv, 0);
	int rv = linenoiseHistoryAdd(line);
	return janet_wrap_integer(rv);
}


static Janet
j_linenoiseHistorySetMaxLen(int32_t argc, Janet *argv)
{
	janet_fixarity(argc, 1);
	int len = janet_getinteger(argv, 0);
	int rv = linenoiseHistorySetMaxLen(len);
	return janet_wrap_integer(rv);
}


static Janet
j_linenoiseHistorySave(int32_t argc, Janet *argv)
{
	janet_fixarity(argc, 1);
	const char *filename = janet_getcstring(argv, 0);
	int rv = linenoiseHistorySave(filename);
	return janet_wrap_integer(rv);
}


static Janet
j_linenoiseHistoryLoad(int32_t argc, Janet *argv)
{
	janet_fixarity(argc, 1);
	const char *filename = janet_getcstring(argv, 0);
	int rv = linenoiseHistoryLoad(filename);
	return janet_wrap_integer(rv);
}


static Janet
j_linenoiseClearScreen(int32_t argc, Janet *argv)
{
	janet_fixarity(argc, 0);
	linenoiseClearScreen();
	return janet_wrap_nil();
}


static Janet
j_linenoiseSetMultiLine(int32_t argc, Janet *argv)
{
	janet_fixarity(argc, 1);
	int val = janet_getboolean(argv, 0);
	linenoiseSetMultiLine(val);
	return janet_wrap_nil();
}


/* LINENOISE-COMPLETION is supposed to call into linenoiseAddCompletion at some point. */
// XXX I guess 'core' isn't the right env after all 
static void
real_completion_cb(const char *buf, linenoiseCompletions *lc)
{
	JanetFunction *completion_cb = janet_unwrap_function(janet_resolve_core("LINENOISE-COMPLETION"));

	Janet argv[] = {
		janet_wrap_string(janet_cstring(buf)),
		janet_wrap_pointer(lc)
	};
	janet_call(completion_cb, 2, argv);
}


// XXX we get it by default, is that a problem?
//
// The problem is, I really just can't invent a C function out of thin air, can I?
static Janet
j_linenoiseSetCompletionCallback(int32_t argc, Janet *argv)
{
	linenoiseSetCompletionCallback(real_completion_cb); 
	return janet_wrap_nil();
}


/* (linenoise/add-completion lc string) */
static Janet
j_linenoiseAddCompletion(int32_t argc, Janet *argv)
{
	janet_fixarity(argc, 2);
	linenoiseCompletions *lc = janet_getpointer(argv, 0);
	const char *completion = janet_getcstring(argv, 1);
	linenoiseAddCompletion(lc, completion);
	return janet_wrap_nil();
}

/* ********** */

JANET_MODULE_ENTRY(JanetTable *env)
{
	janet_cfuns(env, "linenoise", functions);
}
