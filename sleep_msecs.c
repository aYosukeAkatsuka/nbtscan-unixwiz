/*
 * $Id: //devel/tools/main/nbtscan/sleep_msecs.c#1 $
 *
 *	Sleep for the given number of milliseconds in as portable
 *	manner as possible. This is really an important function,
 *	because when probing a remote network, blasting out a full
 *	class C address of UDP packets virtually guarantees packet
 *	loss. 
 *
 *	If you really can't figure out a way to do this, at least add
 *	the real UNIX sleep(1) function. This makes for very slow
 *	scanning, but at least it will work.
 *
 *	My goal is to eliminate the use of this function entirely by
 *	doing proper select I/O with timeouts, but some of the tools
 *	have sleeps for legacy purposes. Sorry.
 *
 *	Please find a way to send back portability changes to me!
 *	-- steve@unixwiz.net
 */

#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include "penlib.h"

void __stdcall sleep_msecs(long msecs)
{
	if (msecs <= 0) return;

#ifdef _WIN32

	Sleep((unsigned long)msecs);

#elif defined(M_XENIX)

	napms(msecs);

#else

	usleep(msecs * 1000);	/* microseconds! */

#endif
}
