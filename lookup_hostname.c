/*
 * $Id: //devel/tools/main/nbtscan/lookup_hostname.c#1 $
 *
 *	Given a string that might be a hostname or an IP address, do
 *	whatever is required to convert to an internal IP address.
 *	Return is TRUE if all is well and FALSE if not.
 *
 *	NOTE: this function returns only the first address associated
 *	with the name. Sorry.
 *
 *	The returned IP address is in HOST byte order.
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE

#ifndef _WIN32
#  include <memory.h>
#  include <sys/types.h>	/* SCO 3.2v4.2 */
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#endif
#include "penlib.h"

int __stdcall lookup_hostname(const char *remote, unsigned long *paddr)
{
const struct hostent *hp;

	assert(remote != 0);
	assert(paddr  != 0);

	if ( parse_inaddr(remote, paddr) != 0 )
		return TRUE;

	if ( (hp = gethostbyname(remote)) == 0 )
		return FALSE;

	memcpy(paddr, hp->h_addr, sizeof *paddr);

	/* put back to host word order */
	*paddr = ntohl(*paddr);

	return TRUE;
}
