/*
 * $Id: //devel/tools/main/nbtscan/nstrcpyA.c#2 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	This is just like strcpy() but it returns the number of bytes
 *	copied (but not including the NUL byte which is always copied).
 *	This is useful when building up lines a bit at a time and you
 *	don't care to incur the overhead of sprintf():
 *
 *		p += nstrcpy(p, "foo");
 *	-vs-
 *		p += nsprintf(p, "foo");
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE

#include <stddef.h>
#include "penlib.h"

size_t __stdcall nstrcpyA(char *dst, const char *src)
{
const char	*dst_save = dst;

	assert(dst != 0);
	assert(src != 0);

	while ( (*dst = *src++) != 0 )
		dst++;

	return (size_t)(dst - dst_save);
}
