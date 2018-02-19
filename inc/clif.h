#ifndef CLIF_H
#define CLIF_H

#include <stdio.h>
#include <stdbool.h>

/* return error code */
#define CLIF_PARSE_EMISSINGTOKEN	-3
#define CLIF_PARSE_EUNRECOGNIZEDTOKEN	-2
#define CLIF_PARSE_ETOOMANYTOKEN		-1
#define CLIF_PARSE_EOF			0
#define CLIF_PARSE_SUCCESS		1
#define CLIF_PARSE_WEMPTY		2
#define CLIF_PARSE_WNOCALLBACK	3
#define CLIF_PARSE_USER		128		/* this value and above: for private use */

typedef struct _clif_keyword
{
	const char *command;
	unsigned int child_c;
	struct _clif_keyword **child_v;
	int (*callback)(struct _clif_keyword*, const unsigned int matching);
	struct _clif_keyword *parent;
} clif_keyword;

typedef clif_keyword *clif_grammar;

/* common variables used to build a grammar */
#define MAX_INPUT_DEPTH	512
extern clif_keyword *clif_build_stack[MAX_INPUT_DEPTH];
extern unsigned int clif_build_stack_index;

/* macros used to build a grammar */
#define CLIF_DECLARE_MENU(A)		{clif_build_stack_index=0; clif_build_stack[0] = clif_grammar_init(A);}
#define CLIF_SECTION		{if(++clif_build_stack_index>=MAX_INPUT_DEPTH){fprintf(stderr, "clif: Stack overflow!\n"); exit(1);}}
#define CLIF_ENTRY(A,B)	{clif_build_stack[clif_build_stack_index] = clif_grammar_add(clif_build_stack[clif_build_stack_index-1], A, B);}
#define CLIF_LINK(A)		{clif_build_stack[clif_build_stack_index] = clif_grammar_link(clif_build_stack[clif_build_stack_index-1], A);}
#define CLIF_ENDSECTION		{if(clif_build_stack_index!=0){clif_build_stack_index--;}else{fprintf(stderr, "clif: Error, no section opened!\n");}}

#define EXPORT extern

/* parser public functions */
EXPORT clif_keyword *clif_grammar_init(clif_grammar *l);
EXPORT clif_keyword *clif_grammar_add(clif_keyword *w, const char *s, int (*callback)(clif_keyword *, const unsigned int));
EXPORT clif_keyword *clif_grammar_link(clif_keyword *w, clif_keyword *s);
EXPORT const char *clif_parse_error(const int code);
EXPORT unsigned int clif_token_count();
EXPORT const char *clif_token_get(const unsigned int);

/* invite public functions */
EXPORT const char *clif_prompt_set(const char *);
EXPORT const char *clif_prompt_get();
EXPORT int clif_gets(clif_grammar); // keyboard
EXPORT int clif_fgets(clif_grammar, FILE*); // stream - file
EXPORT int clif_sgets(clif_grammar, const char*); // string
EXPORT int clif_buffered(clif_grammar, const char *, size_t, void(*parse_result)(int)); // network, serial, etc.

/* readline public function */
EXPORT int clif_readline(clif_grammar g);

/* signal public function */
EXPORT int init_signal_handler();

/* debugging function */
#ifdef NDEBUG
    #define clif_grammar_graph(w)
#else
	EXPORT void clif_grammar_graph(clif_keyword *);
#endif

/* user-friendly parsing function */
EXPORT const char *clif_token_str(const unsigned int);
EXPORT const char *clif_token_str_default(const unsigned int, const char *);
EXPORT char clif_token_char_default(const unsigned int, char);
#define clif_token_char(i) clif_token_char_default(i, '\0')
EXPORT int clif_token_int_default(const unsigned int, int);
#define clif_token_int(i) clif_token_int_default(i, 0)
EXPORT unsigned int clif_token_uint_default(const unsigned int, unsigned int);
#define clif_token_uint(i) clif_token_uint_default(i, 0)
EXPORT long int clif_token_lint_default(const unsigned int, long int);
#define clif_token_lint(i) clif_token_lint_default(i, 0l)
EXPORT unsigned long int clif_token_ulint_default(const unsigned int, unsigned long int);
#define clif_token_ulint(i) clif_token_ulint_default(i, 0l)
EXPORT unsigned int clif_token_hex_default(const unsigned int, unsigned int);
#define clif_token_hex(i) clif_token_hex_default(i, 0)
EXPORT unsigned int clif_token_oct_default(const unsigned int, unsigned int);
#define clif_token_oct(i) clif_token_oct_default(i, 0)
EXPORT int clif_token_num_default(const unsigned int, int);
#define clif_token_num(i) clif_token_num_default(i, 0)
EXPORT float clif_token_float_default(const unsigned int, float);
#define clif_token_float(i) clif_token_float_default(i, 0.0)
EXPORT double clif_token_double_default(const unsigned int, double);
#define clif_token_double(i) clif_token_double_default(i, 0.0l)
EXPORT bool clif_token_bool_default(const unsigned int, bool);
#define clif_token_bool(i) clif_token_bool_default(i, false)



#endif /* CLIF_H */

