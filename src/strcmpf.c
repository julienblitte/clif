#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "debug.h"

#ifndef isoctal
int isoctal(int v)
{
	return (v >= '0' && v <= '7');
}
#endif

bool strcmpf(const char *format, const char *s)
{
	assert(format != NULL);
	assert(s != NULL);

	dbgprintf(DVERBOSE, "strcmpf: called (\"%s\", \"%s\")\n", format, s);

	while(1)
	{
		dbgprintf(DPEDANTIC, "\tanalyzing: [%s]\n", s);
		while(isblank((int)*s))
			s++;

		if (*format == '%')
		{
			format++;
			switch(*format)
			{
				case '\0':
					dbgprintf(DPEDANTIC, "\t! lonely '%%' caracter\n");
					return false;
				case 'l':	// support for long parameters
					format++;
					dbgprintf(DPEDANTIC, "\t? format '%%l%c'\n", *format);
					break;
				default:
					dbgprintf(DPEDANTIC, "\t? format '%%%c'\n", *format);
					break;
			}
			switch(*format)
			{
				case '%':
					if (*s != '%')
					{
						dbgprintf(DPEDANTIC, "\t! magic char '%%' not found\n");
						return false;
					}
					s++;
					break;
				case 's': //[^ ]+
					if (*s == '\0')	// at least one character
					{
						dbgprintf(DPEDANTIC, "\t! no character found for '%%s'\n");
						return false;
					}
					while(*s != '\0' && !isspace((int)*s))
					{
						s++;
					}
					break;
				case 'd': //-?[0-9]+
				case 'i':
					if (*s == '-' || *s == '+')
					{
						s++;
					}
					if (!isdigit((int)*s))	// at least one digit
					{
						dbgprintf(DPEDANTIC, "\t! no integer value found for '%%d'\n");
						return false;
					}
					s++;
					while(isdigit((int)*s))
						s++;
					break;
				case 'f': //[-+]?([0-9](.[0-9]*)?)|(.[0-9]+)
					if (*s == '-' || *s == '+') // may start with a sign
					{
						s++;
					}
					if (isdigit((int)*s))	// at least one digit
					{
						s++;
						while(isdigit((int)*s))
							s++;
						if (*s == '.')	// followed by point and number
						{
							s++;
							while(isdigit((int)*s))
								s++;
						}
					}
					else if (*s == '.') // or start with a dot
					{
						s++;
						if (!isdigit((int)*s))	// at least one digit
						{
							dbgprintf(DPEDANTIC, "\t! no float value found for '%%f'\n");
							return false;
						}
						s++;
						while(isdigit((int)*s))
							s++;
					}
					else
					{
						dbgprintf(DPEDANTIC, "\t! no float value found for '%%f'\n");
						return false;
					}
					break;
				case 'u': //[0-9]+
					if (!isdigit((int)*s))	// at least one digit
					{
						dbgprintf(DPEDANTIC, "\t! no unsigned integer found value for '%%u'\n");
						return false;
					}
					s++;
					while(!isdigit((int)*s))
						s++;
					break;
				case 'c': //.
					if (*s == '\0')	// at least one character
					{
						dbgprintf(DPEDANTIC, "\t! no character found for '%%c'\n");
						return false;
					}
					s++;
					break;
				case 'x': //[0-9a-fA-F]+
					if (!isxdigit((int)*s))	// at least one hexdigit
					{
						dbgprintf(DPEDANTIC, "\t! no hexadecimal value found for '%%x'\n");
						return false;
					}
					s++;
					while(isxdigit((int)*s))
						s++;
					break;
				case 'o': //[0-7]+
					if (!isoctal((int)*s))	// at least one hexdigit
					{
						dbgprintf(DPEDANTIC, "\t! no octal value found for '%%o'\n");
						return false;
					}
					s++;
					while(isoctal((int)*s))
						s++;
					break;
				case 'e': //[0-9]+.[0-9]+e[0-9]+
					if (*s == '-' || *s == '+') // may start with a sign
					{
						s++;
					}
					if (isdigit((int)*s))	// at least one digit
					{
						s++;
						while(isdigit((int)*s))
							s++;
						if (*s == '.')	// followed by point and number
						{
							s++;
							while(isdigit((int)*s))
								s++;
						}
					}
					else if (*s == '.') // or start with a dot
					{
						s++;
						if (!isdigit((int)*s))	// at least one digit
						{
							dbgprintf(DPEDANTIC, "\t! no float value found for '%%e'\n");
							return false;
						}
						s++;
						while(isdigit((int)*s))
							s++;
					}
					else
					{
						dbgprintf(DPEDANTIC, "\t! no float value found for '%%e'\n");
						return false;
					}
					if (*s == 'e')	// may be ended by e + digits
					{
						s++;
						if (!isdigit((int)*s))	// at least one digit
						{
							dbgprintf(DPEDANTIC, "\t! no float value found for '%%e'\n");
							return false;
						}
						while(isdigit((int)*s))
							s++;
					}
					break;
				case '\0':
				default:
					dbgprintf(DPEDANTIC, "\t! unrecognized format specifier '%%%c'\n", *format);
					return false;
			}
		}
		else if (isblank((int)*format))
		{
			// ignore blank format
		}
		else if (*format == '\0')
		{
			while(isblank((int)*s) && *s != '\0')
				s++;

			if (*s != 0) dbgprintf(DPEDANTIC, "\t! end of format string reached but data remaining to parse: '%s'\n", s);
			return (*s == '\0');	// end of $format should not be reached unless we finished to parse $s
		}
		else
		{
			dbgprintf(DPEDANTIC, "\t? matching '%c'\n", *s);
			if (*format != *s)
			{
				dbgprintf(DPEDANTIC, "\t! mismatch at char '%c'\n", *format);
				return false;
			}
			s++;
		}

		format++;
	}

	return true;
}
