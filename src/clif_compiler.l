%{
#include "clif_compiler.tab.h"
#include <string.h>
#include <stdbool.h>

int mylineno = 1;
int yycolumn = 1;

int yyerror(char *s);

%}

%option stack
%option yylineno

%x comment_line
%x comment_block
%x error

%x c_string
%x c_char
%x clif_block

blanks          [ \t\n]+
identifier		[_a-zA-Z][_a-zA-Z0-9]*

%%

<INITIAL>{
	"//"		{ yy_push_state(comment_line); ECHO; }
	"/*"		{ yy_push_state(comment_block); ECHO; }
	"\""		{ BEGIN(c_string); ECHO; }
	"'"		{ BEGIN(c_char); ECHO; }
	"%{"		BEGIN(clif_block);
	
	"\n"		{ mylineno++; ECHO; }
	.		{ ECHO; }
}

<clif_block>{
	"%}"			BEGIN(INITIAL);

	"//"			{ yy_push_state(comment_line); ECHO; }
	"/*"			{ yy_push_state(comment_block); ECHO; }

	[ \t\r]+		{ /* ignore */ }
	\n			{ mylineno++; }

	"{"			return(SECTION);
	"}"			return(ENDSECTION);
	"="			return(ASSOC);
	";"			return(EOL);
	"$"			return(VARIABLE);
	"->"			return(CALLBACK);

	{identifier}	{yylval.sval = strdup(yytext);return(IDENTIFIER);}

	\"(\\.|[^"\\])*\"	{yylval.sval = strdup(yytext);return STRING;}

	. { yy_push_state(error); yyless(0); }
}

<comment_line>{
	\n		{ yy_pop_state(); mylineno++; ECHO; }
	[^\n]+		ECHO;
}

<comment_block>{
	"*/"		{ yy_pop_state(); ECHO; }
	"\n"		{ mylineno++; ECHO; }
	[^*]+		ECHO;
	"*"		ECHO;
}

<c_string>{
	"\\".		ECHO;
	"\\\n"		{ ECHO; mylineno++; }  // REGEX '.' does not match the \n
	"\""		{ BEGIN(INITIAL); ECHO; }
	"\n"		{ yy_push_state(error); mylineno++; ECHO; }
	.		ECHO;
}

<c_char>{
	"\\".		ECHO;
	"\\\n"		{ ECHO; mylineno++; }  // REGEX '.' does not match the \n
	"'"		{ BEGIN(INITIAL); ECHO; }
	"\n"		{ yy_push_state(error); mylineno++; ECHO; }
	.		ECHO;
}

<error>{
	[^ \t\n]+	{ ECHO; yyerror(strdup(yytext)); yy_pop_state(); }
	"\n"		{ mylineno++; ECHO; yymore(); /* continue till we found a token */ }
	[ \t]+		{ ECHO; yymore(); /* continue till we found a token */ }
}
<<EOF>> { return EOF;}
