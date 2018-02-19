#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#define MAX_INVITE_LEN	1024
#define MAX_INPUT_LEN	4096
#define INVITE_DEFAULT	'#'

/* TODO: make the lists bellow modifiable dynamically: */
#define COMMENT_SYMBOLS		"#!"
#define INPUT_DELIMITERS	" \t\n\r"
/* TODO: add also operatordefinition here */

int clif_tokenize(char *);
int clif_grammar_parse(clif_grammar l);
char **clif_grammar_autocomplete(clif_grammar l, bool composing);

int init_signal_handler();
int signal_register_pid(int pid);

#endif // PARSER_H
