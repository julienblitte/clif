#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "clif.h"
#include "clif_private.h"
#include "debug.h"

static char g_prompt[MAX_INVITE_LEN];
char g_input[MAX_INPUT_LEN];

const char *clif_prompt_set(const char *s)
{
	if (s == NULL)
	{
		g_prompt[0] = INVITE_DEFAULT;
		g_prompt[1] = '\0';
	}
	else
	{
		strncpy(g_prompt, s, sizeof(g_prompt)-1);
		g_prompt[sizeof(g_prompt)-1] = '\0';
	}

	return g_prompt;
}

const char *clif_prompt_get()
{
	if (g_prompt[0] == '\0')
	{
		clif_prompt_set(NULL);
	}

	return g_prompt;
}

int clif_gets(clif_grammar g)
{
	if (feof(stdin))
		return CLIF_PARSE_EOF;
	
	printf("%s", clif_prompt_get());
	fgets(g_input, sizeof(g_input), stdin);
	clif_tokenize(g_input);

	return clif_grammar_parse(g);
}

int clif_fgets(clif_grammar g, FILE *f)
{
	if (feof(f))
		return CLIF_PARSE_EOF;

	fgets(g_input, sizeof(g_input), f);
	clif_tokenize(g_input);

	return clif_grammar_parse(g);
}

int clif_sgets(clif_grammar g, const char *s)
{
	strncpy(g_input, s, sizeof(g_input)-1);
	g_input[sizeof(g_input)-1] = '\0';

	clif_tokenize(g_input);

	return clif_grammar_parse(g);
}

int clif_buffered(clif_grammar g, const char *s, size_t size, void(*parse_result)(int))
{
	int i;
	int res;
	static char buffer[MAX_INPUT_LEN];
	static int index;     // default 0
	static bool truncate; // default false

	for(i=0; (i < size) && (s[i] != '\0'); i++)
	{
		if ((s[i] == '\r') || (s[i] == '\n'))
		{
			buffer[index] = '\0';
			if (index > 0)
			{
				res = clif_sgets(g, buffer);
				index = 0;
				if (parse_result) parse_result(res);
			}

			truncate = false;
		}
		else if (index == MAX_INPUT_LEN-1)
		{
			buffer[index] = '\0';
			res = clif_sgets(g, buffer);
			index = 0;
			if (parse_result) parse_result(res);

			truncate = true;
		}
		else if (truncate == false)
		{
			buffer[index] = s[i];
			index++;
		}
	}

	return clif_sgets(g, buffer);
}
