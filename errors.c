/*
 * $Id: //devel/tools/main/nbtscan/errors.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	When printing error messages, we must often do so via the UNIX
 *	errno and via the Win32 GetLastError() method. This unifies the
 *	two.
 */
#include "nbtscan_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nbtdefs.h"

const char *unix_errorstr(int error_num)
{
	return strerror(error_num);
}

#ifdef _WIN32
const char *win32_errorstr(DWORD error_num)
{
static char errorbuf[20];

	sprintf(errorbuf, "#%ld", error_num);

	return errorbuf;
}
#endif
