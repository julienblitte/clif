%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>		/* basename */
#include <stdbool.h>

extern FILE *yyout;

extern int yylineno;
extern int yycolumn;
extern char *yytext;

int yyerror(char *s);
int yylex();

static FILE *header;

%}

%locations

// Symbols.
%union
{
	char	*sval;
};
%token <sval> IDENTIFIER
%token <sval> STRING
%token SECTION
%token ENDSECTION
%token ASSOC
%token EOL
%token VARIABLE
%token CALLBACK

%start MENUS
%%

MENUS:
	/* empty */
	| MENUS MENU
	;

MENU:
	VARIABLE IDENTIFIER ASSOC SECTION { fprintf(yyout, "CLIF_DECLARE_MENU(&%s)\nCLIF_SECTION\n", $2); }
	ENTRIES
	ENDSECTION	{ fprintf(yyout, "CLIF_ENDSECTION\n"); }
	|
	VARIABLE IDENTIFIER ASSOC VARIABLE IDENTIFIER EOL { fprintf(yyout, "%s=%s;\n", $2, $5); }
	;

ENTRIES:
	/* empty */
	| ENTRIES ENTRY
	;

ENTRY:
	MENUITEM SECTION { fprintf(yyout, "CLIF_SECTION\n"); }
		ENTRIES
	ENDSECTION { fprintf(yyout, "CLIF_ENDSECTION\n"); }
	|
	MENUITEM EOL
	|
	VARIABLE IDENTIFIER EOL { fprintf(yyout, "CLIF_LINK(%s)\n", $2); }
	;

MENUITEM:
	STRING CALLBACK IDENTIFIER { fprintf(yyout, "#line %d\n", yylineno); fprintf(yyout, "CLIF_ENTRY(%s, %s)\n", $1, $3); if (header) fprintf(header, "int %s(clif_keyword *w, const unsigned int t);\n", $3); }
	|
	STRING { fprintf(yyout, "#line %d\n", yylineno); fprintf(yyout, "CLIF_ENTRY(%s, NULL)\n", $1); }
	;

%%

int yyerror(char *s)
{
	fprintf(stderr, "Error near line %d, col %d: \'%s\'\n", yylineno, yycolumn, s);

	exit(EXIT_FAILURE);
	return 1; /* avoid non-return compiler warning */
}

void usage(const char *p)
{
	fprintf(stderr, "Usage: %s [-h] [-o output.c] [-t header.h] <filename>\n\n", p);
}

int main(int argc, char *argv[])
{
	int opt;
	char *input, *output, *header_name, *tmp;
	int result;
	extern FILE *yyin;
	int l;

	opterr = 0;
	header_name = NULL;
	output = NULL;
	while ((opt = getopt (argc, argv, ":o:t:h?")) != -1)
	{
		switch (opt)
		{
			case 'h':
			case '?':
				usage(basename(argv[0]));
				return EXIT_SUCCESS;
			case 'o':
				output = strdup(optarg); // output has to be freed (0) 1/2
				break;
			case 't':
				header_name = strdup(optarg); // header_name has to be freed (1)
				break;
			case ':':
				if (optopt == 'o')
				{
					fprintf (stderr, "Error: option -o requires an argument.\n\n");
				}
				if (optopt == 't')
				{
					fprintf (stderr, "Error: option -t requires an argument.\n\n");
				}
				usage(basename(argv[0]));
				return EXIT_FAILURE;
		}
	}

	if (optind == argc)
	{
		fprintf (stderr, "Error: no input file.\n\n");
		usage(basename(argv[0]));
		return EXIT_FAILURE;
	}

	input = NULL;
	if (argc - optind > 0)
	{
		input = argv[optind];
		yyin = fopen(input, "rt");
		if (yyin == NULL)
		{
			fprintf(stderr, "Unable to open input file '%s'!\n", input);
			return EXIT_FAILURE;
		}
	}

	if (output == NULL)
	{
		l = strlen(input);
		// added length of 3 char, add ".c" + NULL CHAR
		output = malloc(l+3); // output has to be freed (0) 2/2
		strcpy(output, input);
		strcat(output, ".c");
		printf("Output file: %s\n", output);
	}

	if (header_name != NULL)
	{
		header = fopen(header_name, "wt");
		if (header != NULL)
		{
			fprintf(header, "#include <clif.h>\n\n/* functions bellow should return CLIF_PARSE_SUCCESS or value above CLIF_PARSE_USER */\n"
			"/* Remove duplicated entries */\n");
		}
		free(header_name); // header_name freed (1)
	}

	/* use temporary file to preserve output file in case of failure */
	l = strlen(output);
	// added length is 2, add "~" + NULL CHAR
	tmp = malloc(l+2); // tmp has to be freed (3)
	strcpy(tmp, output);
	strcat(tmp, "~");

	yyout = fopen(tmp, "wt");
	if (yyout == NULL)
	{
		fprintf(stderr, "Unable to open output file '%s'!\n", output);
		return EXIT_FAILURE;
	}
	fprintf(yyout, "#line 1 \"%s\"\n", input);

	result = yyparse();

	fclose(yyout);
	unlink(output);
	rename(tmp, output);

	free(tmp); // tmp freed (3)

	if (header != NULL)
	{
		fprintf(header, "\n"); fclose(header);
	}

	return result;
}
