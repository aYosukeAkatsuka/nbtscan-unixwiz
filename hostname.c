/*
 * $Id: //devel/tools/main/nbtscan/hostname.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	Utilities for dealing with hostnames and the DNS.
 */
#include "nbtscan_common.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifndef _WIN32
#  include <netdb.h>	/* found in <winsock2.h> */
#endif
#include "nbtdefs.h"

/*
 * might_be_hostname()
 *
 *	Given a target address specification, see if this might be
 *	a hostname or not. We require this test because some versions
 *	of the DNS resolver do a simple-minded test for dotted-quads
 *	early and return a single address, and this would subvert our
 *	notation of
 *
 *		192.168.0.1-254
 *	or
 *		192.168.0.0/24
 *
 *	notation. By only trying the hostname lookup if it really looks
 *	like a hostname, we stand a better chance of not making this
 *	mistake.
 */
int might_be_hostname(const char *s)
{
int	ndots = 0;

	assert(s != 0);

	for ( ; *s; s++ )
	{
		if ( isalpha(*s) )
			return TRUE;
		if ( *s == '.' )
			ndots++;
	}

	return ndots < 4;
}

/*
 * name_to_ip()
 *
 *	Given what purports to be a hostname, look it up in the DNS
 *	and return the IP address associated with it. If the name is
 *	not valid/not known, we return INADDR_NONE.
 *
 *	===NOTE: we only return *one* of the names, sorry.
 */
unsigned long name_to_ip(const char *name)
{
struct hostent	*hp;
struct in_addr	addr;

	assert(name != 0);

	if ( (hp = gethostbyname(name)) == 0 )
		return INADDR_NONE;
	else
	{
		memcpy(&addr, hp->h_addr, sizeof addr);

		return addr.s_addr;
	}
}

/*
 * ip_to_name()
 *
 *	Given an IP address for a remote, look up its inverse name
 *	via the DNS. Return is the number of bytes in the looked-up
 *	name, or 0 if not found.
 */
int ip_to_name(unsigned long ipaddr, char *obuf, int osize)
{
struct in_addr	addr;
struct hostent	*hp;

	assert(obuf != 0);
	assert(osize > 1);

	addr.s_addr = ipaddr;

	--osize;		/* allow room for terminating NUL */

	if ( (hp = gethostbyaddr((char *)&addr, sizeof addr, AF_INET)) == 0 )
		return 0;

	if ( hp->h_name == 0 )
		return 0;

	strncpy(obuf, hp->h_name, (unsigned int)osize)[osize] = '\0';

	return (int) strlen(obuf);
}
