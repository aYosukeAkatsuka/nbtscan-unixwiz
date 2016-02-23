/*
 * $Id: //devel/tools/main/nbtscan/stripA.c#1 $
 *
 *	Remove trailing whitespace from the given string and return the
 *	newly-shortened string to the caller. We consider "whitespace" to
 *	be anything for which "isspace()" is true.
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include <ctype.h>
#include "penlib.h"

char * __stdcall stripA(char *str)
{
char	*old = str;	/* save ptr to original string          */
char	*lnsp = 0; 	/* ptr to last non-space in string      */

	assert(str != 0);

	for ( ; *str; str++)
		if (!isspace(*str))
			lnsp = str;
	if ( lnsp )
		lnsp[1] = '\0';
	else
		*old = '\0';

	return old;
}
