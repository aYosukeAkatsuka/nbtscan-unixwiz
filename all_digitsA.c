/*
 * $Id: //devel/tools/main/nbtscan/all_digitsA.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	Is the given string composed entirely of digits right up
 *	to the terminating NUL byte? We define an empty string as
 *	*not* all digits.
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include <ctype.h>
#include "penlib.h"

int __stdcall all_digitsA(const char *s, int *n)
{
const char	*s_save = s;

	assert(s != 0);
	assert(n != 0);

	*n = 0;

	while ( isdigit(*s) )
		*n = (*n * 10) + (*s++ - '0');

	return (*s == 0) && (s > s_save);
}
