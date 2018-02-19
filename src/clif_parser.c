#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "clif.h"
#include "clif_private.h"
#include "debug.h"
#include "strcmpf.h"

char *g_token_v[MAX_INPUT_DEPTH];
unsigned int g_token_c;

clif_keyword *clif_build_stack[MAX_INPUT_DEPTH];
unsigned int clif_build_stack_index;

bool clif_is_comment(char *s)
{
	int i;

	for(i=0; s[i] != '\0'; i++)
	{
		if (s[i] == ' ')
			continue;

		if (strchr(COMMENT_SYMBOLS, s[i]) != NULL)
		{
			dbgprintf(DINFO, "clif_is_comment: ignoring commented line\n");
			return true;
		}
		return false;
	}
	return true; // empty line
}

int clif_tokenize(char *s)
{
	char *t;

	// check for comment char
	if (clif_is_comment(s))
	{
		g_token_c = 0;
		return 0;
	}

	// tokenize
	/* TODO: be able to split also around an operator (e.g. pipe '|' or semicolumn ';') */
	t = strtok(s, INPUT_DELIMITERS);
	g_token_c = 0;
	while (t != NULL)
	{
		g_token_v[g_token_c] = t;
		g_token_c++;

		t = strtok(NULL, INPUT_DELIMITERS);
	}

	return g_token_c;
}

unsigned int clif_token_count()
{
	return g_token_c;
}

const char *clif_token_get(const unsigned int i)
{
	if (i >= g_token_c)
		return "";

	return g_token_v[i];
}

clif_keyword *clif_grammar_init(clif_grammar *l)
{
	*l = (clif_keyword *)malloc(sizeof(clif_keyword));
	assert(*l != NULL);

	dbgprintf(DDEBUG, "clif_gammar_init: new structure @%p\n", *l);

	// command = NULL means that it's the root cell
	memset(*l, 0, sizeof(clif_keyword));

	return *l;
}

clif_keyword *clif_grammar_link(clif_keyword *w, clif_keyword *s)
{
	int child_c;
	int i, j;

	assert(w != NULL);
	assert(s != NULL);

	dbgprintf(DDEBUG, "clif_gammar_link: @%p to @%p\n", s, w);

	child_c = w->child_c + s->child_c;
	w->child_c = child_c;
	w->child_v = realloc(w->child_v, child_c*sizeof(void*));

	for(i=0, j=child_c-s->child_c; i < s->child_c; i++)
	{
		dbgprintf(DDEBUG, "\tlinked: +[@%p: '%s']\n", s->child_v[i], s->child_v[i]->command);
		w->child_v[j+i] = s->child_v[i];
	}

	return w;
}

clif_keyword *clif_grammar_add(clif_keyword *w, const char *s, int (*callback)(clif_keyword *, const unsigned int))
{
	clif_keyword *new_keyword;
	int child_c;

	assert(w != NULL);
	assert(s != NULL);

	new_keyword = (clif_keyword *)malloc(sizeof(clif_keyword));
	assert(new_keyword != NULL);
	new_keyword->command = strdup(s);
	new_keyword->child_c = 0;
	new_keyword->child_v = NULL;
	new_keyword->callback = callback;
	new_keyword->parent = w;

	dbgprintf(DDEBUG, "clif_gammar_add: +[@%p: '%s'] to @%p\n", new_keyword, s, w);
	child_c = w->child_c + 1;
	w->child_c = child_c;
	w->child_v = realloc(w->child_v, child_c*sizeof(void*));
	w->child_v[child_c-1] = new_keyword;

	return new_keyword;
}

int clif_grammar_parse(clif_grammar l)
{
	unsigned int c, wc, i;
	clif_keyword *w, *matching;
	const char *token;

	assert(l != NULL);

	w = l;

	c = clif_token_count();
	dbgprintf(DDEBUG, "clif_gammar_parse: %u tokens\n", c);

	if (c == 0)
	{
		dbgprintf(DINFO, "\tempty line\n");
		return CLIF_PARSE_WEMPTY;
	}

	wc = 0;
	while(wc < c)
	{
		token = clif_token_get(wc);

		// too many tokens
		if (w->child_c == 0)
		{
			dbgprintf(DWARNING, "\t! parsing finished at item '%s' but %d token(s) remains to parse, too many tokens\n", token, c-wc);
			return CLIF_PARSE_ETOOMANYTOKEN;
		}
		else
		{
			// parse each commands
			matching = NULL;
			for(i=0; i < w->child_c && matching == NULL; i++)
			{
				if (strcmpf(w->child_v[i]->command, token))
				{
					matching = w->child_v[i];
					dbgprintf(DDEBUG, "\ttoken '%s' match entry \"%s\"\n", token, matching->command);
				}
			}
			// no command found
			if (matching == NULL)
			{
				dbgprintf(DWARNING, "\t! no item match for token \"%s\"\n", token);
				return CLIF_PARSE_EUNRECOGNIZEDTOKEN;
			}
			else
			{
				w = matching;
			}
		}
		wc++;
	}

	if (w->callback != NULL)
	{
		dbgprintf(DINFO, "\tcallback @%p called\n", w->callback);
		return w->callback(w, wc-1);
	}
	// command not complete
	else if (w->child_c > 0)
	{
		dbgprintf(DWARNING, "\t! unexpected end of command at '%s'\n", w->command);
		return CLIF_PARSE_EMISSINGTOKEN;
	}
	// no action assigned to the command
	else
	{
		dbgprintf(DINFO, "\tend of parsing at token '%s' but no callback defined\n", w->command);
		return CLIF_PARSE_WNOCALLBACK;
	}
}

#undef clif_grammar_graph		/* remove previous definition from clif.h as it's only for library user */
void clif_grammar_graph(clif_keyword *l)
{
	int i;
	static int indent;

	assert(l != NULL);

	if (l->command != NULL)
	{
		for (i=0; i < indent; i++) printf("  ");
		printf("%s\n", l->command);
	}
	else
	{
		printf("clif_gammar_graph: tree graphical view is\n");
		indent = 0;
	}

	for(i=0; i < l->child_c; i++)
	{
		indent++;
		clif_grammar_graph(l->child_v[i]);
	}
	indent--;
}

const char *clif_parse_error(const int code)
{
	static char user_error[32];

	if (code >= CLIF_PARSE_USER)
	{
		snprintf(user_error, sizeof(user_error), "User defined error: #%d.", code-CLIF_PARSE_USER);
		return user_error;
	}
	switch(code)
	{
		case CLIF_PARSE_EMISSINGTOKEN:
			return "Command not complete";
		case CLIF_PARSE_EUNRECOGNIZEDTOKEN:
			return "Unrecognized command";
		case CLIF_PARSE_ETOOMANYTOKEN:
			return "Unexpected keyword on command line";
		case CLIF_PARSE_WEMPTY:
			return "Empty command line";
		case CLIF_PARSE_WNOCALLBACK:
			return "No action associated to this command";
		case CLIF_PARSE_SUCCESS:
			return "Command executed successfully";
	}
	return "Unknown error";
}

char **clif_grammar_autocomplete(clif_grammar l, bool composing)
{
	unsigned int c, wc, i;
	clif_keyword *w, *matching;
	const char *token;

	char **token_completion;

	assert(l != NULL);

	w = l;

	c = clif_token_count();
	dbgprintf(DDEBUG, "clif_grammar_autocomplete: %u tokens, composing=%s\n", c, (composing?"true":"false"));

	wc = 0;
	// for each items on current line
	while(wc < c)
	{
		token = clif_token_get(wc);

		// too many tokens
		if (w->child_c == 0)
		{
			dbgprintf(DWARNING, "\t! parsing finished at item '%s' but %d token(s) remains to parse, too many tokens\n", token, c-wc);
			dbgprintf(DWARNING, "\treturning null completion list\n");
			return NULL;
		}
		else
		{
			// locate command
			matching = NULL;
			for(i=0; i < w->child_c && matching == NULL; i++)
			{
				if (strcmpf(w->child_v[i]->command, token))
				{
					matching = w->child_v[i];
					dbgprintf(DDEBUG, "\ttoken '%s' match entry \"%s\"\n", token, matching->command);
				}
			}
			// not found
			if (matching == NULL)
			{
				// this is last item and user is composing it, list matching completion by first characters entered
				if (wc == c-1 && composing)
				{
					int n, j;
					n = strlen(token);

					dbgprintf(DINFO, "\ttry to find command starting by '%s'\n", token);
					token_completion = (char **)calloc(w->child_c + 1, sizeof(char*));
					j = 0;
					for(i=0; i < w->child_c; i++)
					{
						dbgprintf(DDEBUG, "\n%s,%s,(%d)\n", w->child_v[i]->command, token, n);
						if (strncmp(w->child_v[i]->command, token, n) == 0)
						{
							dbgprintf(DDEBUG, "\tmatch entry \"%s\"\n", w->child_v[i]->command);

							token_completion[j] = strdup(w->child_v[i]->command);
							dbgprintf(DDEBUG, "\t+ [%s]\n", token_completion[j]);
							j++;
						}
					}
					token_completion[j] = NULL;
					return token_completion;
				}
				// not last item, command error
				else
				{
					dbgprintf(DWARNING, "\t! no item match for token \"%s\"\n", token);

					dbgprintf(DWARNING, "\treturning null completion list\n");
					return NULL;
				}
			}
			else
			{
				w = matching;
			}
		}
		wc++;
	}

	dbgprintf(DWARNING, "\tend of parsing at token '%s'\n", w->command);

	// command is complete
	if (w->child_c == 0)
	{
		dbgprintf(DWARNING, "\t! syntax correct and command already complete\n");
		dbgprintf(DWARNING, "\treturning null completion list\n");
		return NULL; 
	}

	// command not complete, return list of next commands
	dbgprintf(DINFO, "\tsyntax correct but command not complete\n");

	// composing, return current token
	if (composing)
	{
		token_completion = (char **)calloc(2, sizeof(char*));
		token_completion[0] = strdup(token);//strdup(w->command);
		dbgprintf(DDEBUG, "\tComposing, return current token '%s'\n", token_completion[0]);
		//token_completion[1] = NULL; // no need thanks of calloc
		return token_completion;
	}
	else
	// return next tokens
	{
		token_completion = (char **)calloc(w->child_c + 1, sizeof(char*));
		int j;
		for(i=0, j=0; i < w->child_c; i++)
		{
			// if contains format char, discard completion
			if (strchr(w->child_v[i]->command, '%') != NULL)
				continue;

			token_completion[j] = strdup(w->child_v[i]->command);
			dbgprintf(DDEBUG, "\t+ [%s]\n", token_completion[j]);
			j++;
		}
		//token_completion[j] = NULL; // no need thanks of calloc
		return token_completion;
	}

	dbgprintf(DWARNING, "\treturning null completion list\n");
	return NULL; 
}
