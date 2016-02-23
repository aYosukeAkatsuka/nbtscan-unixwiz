/*
 * $Id: //devel/tools/main/nbtscan/winsock.c#1 $
 *
 *	This module is responsible for initailizing the Winsock modules
 *	under Win32. We would rather use Winsock 2, but this is not
 *	universally available so we live with 1.1.
 *
 *	We're a bit fuzzy on the version number business and on whether
 *	we really need to call WSAStartup() more than once. Ugh. We'll
 *	add more when we find it.
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include <stdio.h>
#include "penlib.h"

#if _WIN32
const char * __stdcall printable_wserror(DWORD dwErr, char *obuf, size_t osize)
{
	UNUSED_PARAMETER(osize);

	switch ( dwErr )
	{
	  case WSASYSNOTREADY:          return "Network not ready";
	  case WSAVERNOTSUPPORTED:      return "Version not supported";
	  case WSAEINPROGRESS:          return "WS1.1 op in progress";
	  case WSAEPROCLIM:             return "Too many tasks";
	  case WSAEFAULT:               return "Data fault";
	  case WSAECONNREFUSED:         return "Connection refused";
	  case WSAEWOULDBLOCK:          return "Operation would block";
	  default:			sprintf(obuf, "WS Error#%ld", dwErr);
					return obuf;
	}
}
#endif

#ifdef _WIN32
void __stdcall init_winsock(int verbose, int level)
{
WORD	wVersion = MAKEWORD(level,level);
WSADATA	wsaData;
DWORD	err;

	/*---------------------------------------------------------------
	 * First try to initialize Winsock with version 2.2, then fall
	 * back to 1.1
	 */
	if ( (err = (DWORD)WSAStartup(wVersion, &wsaData)) != 0 )
	{
	char	errbuf[256];

		die("ERROR: initializing Winsock [%s]",
			printable_wserror(err, errbuf, sizeof errbuf));
	}

	if ( verbose )
	{
		printf("Using Winsock %d.%d",
			LOBYTE(wsaData.wVersion),
			HIBYTE(wsaData.wVersion) );

		if ( wsaData.wVersion != wsaData.wHighVersion )
		{
			printf(" (%d.%d available)",
				LOBYTE(wsaData.wHighVersion),
				HIBYTE(wsaData.wHighVersion) );
		}
		printf("\n");
		fflush(stdout);
	}
}
#endif /* _WIN32 */
