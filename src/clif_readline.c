// http://gnuwin32.sourceforge.net/packages/readline.htm
// (alternative https://sourceforge.net/projects/mingweditline/ )

// sudo apt-get install libreadline-dev
// gcc test.c -lreadline

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "clif.h"
#include "clif_private.h"
#include "debug.h"

static clif_grammar rl_grammar_context;

char **clif_completion(const char *text, int start, int end);

int rl_clear()
{
	//rl_delete_text(0, rl_end);
	rl_kill_text(0, rl_end);
	rl_redisplay();
	return 0;
}

int clif_readline(clif_grammar g)
{
        char *r_input;
	int result;

	rl_bind_key('`', rl_clear);

	rl_grammar_context = g, rl_attempted_completion_function = clif_completion;

        r_input = readline(clif_prompt_get());
	if (r_input == NULL)
	{
		return CLIF_PARSE_EOF;
	} 

	add_history(r_input);

	clif_tokenize(r_input);
	result = clif_grammar_parse(g);

	free(r_input);

	return result;
}

char *clif_completion_iterator(const char *text, int state)
{
	static char **result;
	static int index;


	dbgprintf(DINFO, "%sclif_completion_iterator(%d)\n", (state?"":"<TAB>\n"),state);

	if (!state)
	{
		char *current_line;

		current_line = strdup(rl_line_buffer);
		clif_tokenize(current_line);

		dbgprintf(DINFO, "\tinitialize for '%s' with %d tokens\n", text, clif_token_count());

		result = (char **)clif_grammar_autocomplete(rl_grammar_context, text[0] != '\0');
		index = 0;
	}

	if (result == NULL)
	{
		dbgprintf(DDEBUG, "\tempty\n");
		return NULL;
	}
	else if (result[index] == NULL)
	{
		dbgprintf(DDEBUG, "\tend\n");
		free(result);
		result = NULL;
		return NULL;
	}
	else
	{
		dbgprintf(DDEBUG, "\t%s\n", result[index]);
		return result[index++];
	}

        return NULL; /* never reached, to avoid warning */
}

char **clif_completion(const char *text, int start, int end)
{
	rl_attempted_completion_over = 1;

	return rl_completion_matches(text, clif_completion_iterator);
}

