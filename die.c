/*
 * $Id: //devel/tools/main/nbtscan/die.c#1 $
 *
 * written by :	Stephen J. Friedl
 *              Software Consultant
 *              steve@unixwiz.net
 *
 *	Given a printf-style variadic argument list, format it to the
 *	standard error stream, append a newline, and exit with error
 *	status.
 */
#ifndef COMMONFILE
#  define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "penlib.h"

void __cdecl dieA(const char *format, ...)
{
va_list	args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	putc('\n', stderr);

	exit(EXIT_FAILURE);
}
