#include <stdbool.h>
#include <string.h>
#include <strings.h>

#include "clif.h"

inline
const char *clif_token_str(const unsigned int i)
{
	return clif_token_get(i);
}

const char *clif_token_str_default(const unsigned int i, const char *def_val)
{
	const char *result;
	result = clif_token_get(i);
	if (strlen(result) == 0)
	{
		return def_val;
	}
	return result;
}

char clif_token_char_default(const unsigned int i, char def_val)
{
	char result;
	if (sscanf(clif_token_get(i), "%c", &result) == 1)
		return result;
	else
		return def_val;
}

int clif_token_int_default(const unsigned int i, int def_val)
{
	int result;
	if (sscanf(clif_token_get(i), "%d", &result) == 1)
		return result;
	else
		return def_val;
}

unsigned int clif_token_uint_default(const unsigned int i, unsigned int def_val)
{
	unsigned int result;
	if (sscanf(clif_token_get(i), "%u", &result) == 1)
		return result;
	else
		return def_val;
}

long int clif_token_lint_default(const unsigned int i, long int def_val)
{
	long int result;
	if (sscanf(clif_token_get(i), "%ld", &result) == 1)
		return result;
	else
		return def_val;
}

unsigned long int clif_token_ulint_default(const unsigned int i, unsigned long int def_val)
{
	unsigned long int result;
	if (sscanf(clif_token_get(i), "%lu", &result) == 1)
		return result;
	else
		return def_val;
}

unsigned int clif_token_hex_default(const unsigned int i, unsigned int def_val)
{
	unsigned int result;
	if (sscanf(clif_token_get(i), "%x", &result) == 1)
		return result;
	else
		return def_val;
}

unsigned int clif_token_oct_default(const unsigned int i, unsigned int def_val)
{
	unsigned int result;
	if (sscanf(clif_token_get(i), "%o", &result) == 1)
		return result;
	else
		return def_val;
}

int clif_token_num_default(const unsigned int i, int def_val)
{
	int result;
	if (sscanf(clif_token_get(i), "%i", &result) == 1)
		return result;
	else
		return def_val;
}

float clif_token_float_default(const unsigned int i, float def_val)
{
	float result;
	if (sscanf(clif_token_get(i), "%f", &result) == 1)
		return result;
	else
		return def_val;
}

double clif_token_double_default(const unsigned int i, double def_val)
{
	double result;
	if (sscanf(clif_token_get(i), "%lf", &result) == 1)
		return result;
	else
		return def_val;
}

bool clif_token_bool_default(const unsigned int i, bool def_val)
{
	const char *tk_value;

	tk_value = clif_token_get(i);

	if (def_val)
	{
		if (strcmp(tk_value, "0") == 0)
			return false;
		if (strcasecmp(tk_value, "false") == 0)
			return false;
		if (strcasecmp(tk_value, "no") == 0)
			return false;
		if (strcasecmp(tk_value, "n") == 0)
			return false;
		
		return true;
	}
	else
	{
		if (strcmp(tk_value, "1") == 0)
			return true;
		if (strcasecmp(tk_value, "true") == 0)
			return true;
		if (strcasecmp(tk_value, "yes") == 0)
			return true;
		if (strcasecmp(tk_value, "y") == 0)
			return true;

		return false;
	}
}

