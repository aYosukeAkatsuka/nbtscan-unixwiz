/*
 * $Id: //devel/tools/main/nbtscan/timeval_set_secs.c#1 $
 *
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include "penlib.h"

struct timeval * __stdcall timeval_set_secs(struct timeval *tv, int secs)
{
	assert(tv != 0);

	tv->tv_sec  = secs;
	tv->tv_usec = 0;

	return tv;
}
