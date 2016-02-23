/*
 * $Id: //devel/tools/main/nbtscan/parse_target.c#3 $
 *
 *	This module takes care of parsing "target" strings provided by
 *	the user on the command line. These targets specify machines that
 *	we're trying to describe, and they can be in a host of different
 *	formats.
 *
 *	The "base" target can bet an IP address or standard hostname, and
 *	it can be followed by a /nbits notation, where /nbits is 16..32.
 *	It's technically possible to process things like /8 addresses, but
 *	these provide simply too many addresses to deal with.
 *
 *	If an IP address or hostname is given without anything else, its
 *	address is used as given, but if the /## notation is provided,
 *	it implied the entire address block that includes the "base"
 *	address, and this works even if the address given is not the start
 *	of the block. The slash is doubled if we want to include the
 *	broadcast addresses at the top and bottom of the ranges.
 *
 *	For instance 10.1.2.3//24 specifies 10.1.2.0 -> 10.1.2.255.
 *	For instance 10.1.2.3/24  specifies 10.1.2.1 -> 10.1.2.254.
 *
 *	We also permit a comma-separated list of numbers and ranges in
 *	the last octet. This last octet can be of the form:
 *
 *		1,5,10
 *		1-254
 *		1-31,200-254
 *		2,4,6,8,10
 *
 *	and so on. Technically this can all be done with all the octets,
 *	but we're not sure how to represent it, and we don't believe
 *	that it's terribly necessary.
 *
 * CALLBACK FUNCTION REQUIRED
 * --------------------------
 *
 *	Once an address (a singleton or a range) is parsed, it must be
 *	added to the user's "list" of addresses, and this is done via
 *	a callback function provided by the user. This function is called
 *	for each range seen, with the starting and ending address, along
 *	with a user-provided "hint" that is not touched by our routine.
 *	This callback does whatever is required to add the target(s) to
 *	the list of work to do.
 *
 *	Some callback functions will continue to represent the ranges
 *	given by the user, while others will always decompose down to
 *	the individual addresses without regard for the larger groupings.
 *	That's up to the callback.
 *
 *	The callback function returns TRUE for this one to keep going
 *	or FALSE to stop.
 *
 * ERROR CONTROL
 * -------------
 *
 *	This function is very strict with respect to what it will
 *	take, and the intention is that we catch /everything/ that's
 *	wrong. When an error is seen, we copy an error message to 
 *	the user's buffer and return FALSE. A TRUE return indicates
 *	that all is well.
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "penlib.h"

#define MIN_HOSTBITS	16

static int scan_octet(char **p, int *n);

int __stdcall parse_target( const char *str,
		  char *errbuf,
		  int (__stdcall *add_target)(	unsigned long first,
					unsigned long lastaddr,
					const char    *hostname,
					void          *hint ),
		  void *hint )
{
char		temp[256];
unsigned long	addr;
char		*p,
		*tok;
int		n1,
		n2,
		n3;

	assert(str    != 0);
	assert(errbuf != 0);

	strcpy(errbuf, "-no error-");

	/*----------------------------------------------------------------
	 * Is this a standalone hostname without any trailing stuff such
	 * as slashes or dots, just convert it to a regular IP address and
	 * be done with it. This is the easy case.
	 *
	 * If the name contains a slash, a comma, or a dash, we're clearly
	 * using the digit octets only and need not fool with DNS at all.
	 *
	 * ACK: this fails if the hostname contains a dash - DUH. Skip
	 * the "-" test.
	 */
	if ( strpbrk(str, "/,") == 0 )
	{
/*		printf("looking up [%s]\n", str); */

		if ( lookup_hostname(str, &addr) )
			return (*add_target)(addr, addr, str, hint);
	}

	/* copy user's string to local working area */
	strncpy(temp, str, sizeof temp)[sizeof temp - 1] = '\0';

	/*----------------------------------------------------------------
	 * If we see a slash, it means the address is followed by the number
	 * of network bits. We don't permit scanning of larger than a class
	 * B (sorry). If there are two slashes, we do NOT scan the top and
	 * bottom addresses, which are both broadcasts.
	 */
	if ( (p = strchr(temp, '/')) != 0 )
	{
	int	netbits, nhosts;
	int	enable_bcast = FALSE;

		*p++ = '\0';

		if ( *p == '/' ) { enable_bcast = TRUE; p++; }

		if ( ! all_digits(p, &netbits) )
		{
			sprintf(errbuf, "net bitcount [%s] is invalid", p);
			return FALSE;
		}

		if ( netbits <= 0  ||  netbits > 32 )
		{
			sprintf(errbuf, "net bits [%d] must be 1..32", netbits);
			return FALSE;
		}

		if ( netbits < MIN_HOSTBITS )
		{
			sprintf(errbuf, "subnet /%d is too large (%d max)",
				netbits,
				MIN_HOSTBITS);
			return FALSE;
		}

		/*--------------------------------------------------------
		 * parse the IP address itself. If this is invalid we
	 	 * have to bail on it.
		 */
		if ( ! lookup_hostname(temp, &addr) )
		{
			sprintf(errbuf, "[%s] is invalid IP address", temp);
			return FALSE;
		}

		/*--------------------------------------------------------
		 * the address given might be in the middle of the subnet
		 * of interest -- mask it off so we automatically get the
		 * whole range.
		 */
		addr &= netmasks[netbits];

		if (netbits == 32)	nhosts = 1;
		else			nhosts = (int)~netmasks[netbits] + 1;

		if (!enable_bcast && netbits < 32)
		{
			nhosts -= 2;	/* drop .0 and .255 addresses */
			addr++;		/* start at .1                */
		}

		return (*add_target)(addr, addr + nhosts - 1, 0, hint);
	}

	/*----------------------------------------------------------------
	 * Now pick apart the dotted-quad. The first three parts must
	 * be digits, then the rest is used to select ranges and the
	 * like.
	 */
	p = temp;

	if ( ! scan_octet(&p, &n1)	/* first  octet */
	 ||  ! scan_octet(&p, &n2)	/* second octet */
	 ||  ! scan_octet(&p, &n3)	/* third  octet */
	 ||  ! isdigit(*p) )
	{
		sprintf(errbuf, "[%s] is an invalid target (bad IP/hostname)", str);
		return FALSE;
	}

	addr = build_ipaddr_from_octets(n1, n2, n3, 0);

	/*----------------------------------------------------------------
	 * Now we've picked apart the first three octets, so the last one
	 * can be a combination of comma-separated ranges. This is tricky,
	 * but we'll try to get it right.
	 */
	while ( (tok = strtok(p, ",")) != 0 )
	{
	char	*pcomma;

		p = 0;

		if ( (pcomma = strchr(tok, '-')) != 0 )
		{
			*pcomma++ = '\0';

			if ( ! all_digits(tok,    &n1) || n1 < 0 || n1 > 255
			 ||  ! all_digits(pcomma, &n2) || n2 < 0 || n2 > 255
			 ||   n1 > n2 )
			{
				sprintf(errbuf, "%s has a bad octet range",str);
				return FALSE;
			}

			if ( ! add_target(addr+n1, addr+n2, 0, hint) )
			{
				sprintf(errbuf, "add_target failed");
				return FALSE;
			}
		}
		else if ( all_digits(tok, &n1)  &&  n1 >= 0  &&  n1 <=  255 )
		{
			if ( ! (*add_target)(addr + n1, addr + n1, 0, hint) )
			{
				sprintf(errbuf, "add_target failed");
				return FALSE;
			}
		}
		else
		{
			sprintf(errbuf, "invalid octet \"%s\" in range", tok);
			return FALSE;
		}
	}

	return TRUE;
}

/*
 * scan_octet()
 *
 *	Given a pointer to the start of digits in an IP address, scan the next
 *	octet and store it at "*n", then consume the dot that follows. Return
 *	TRUE if all is well and FALSE if not.
 *
 *	===NOTE: we treat leading zeros as *decimal*, not octal. This is helpful
 *	when pasting IP addresses from WS_Ping Pro Pack that displays with three
 *	digits in each octet.
 */
static int scan_octet(char **p, int *n)
{
	assert( p != 0);
	assert(*p != 0);
	assert( n != 0);

	*n = 0;

	if ( ! isdigit(**p) )
		return FALSE;

	while ( isdigit(**p) && *n < 256 )
		*n = (*n * 10) + (*(*p)++ - '0');

	if ( *(*p) != '.' ) return FALSE;
	++*p;

	return (*n >= 0) && (*n < 256);
}

#ifdef TESTING

#ifdef _WIN32
#  include "win_sock.h"
#else
#  include <sys/types.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#endif

static int add_target(	unsigned long addr1, unsigned long addr2,
			const char *host, void *hint)
{
struct in_addr	a1, a2;

	a1.s_addr = htonl(addr1);
	a2.s_addr = htonl(addr2);

	printf(" ... %s", inet_ntoa(a1));

	if ( addr1 != addr2 )
		printf(" -> %s\n", inet_ntoa(a2));
	else
		printf("\n");

	return TRUE;
}

int main(int argc, char **argv)
{
int	i;

	for (i = 1; i < argc; i++ )
	{
	const char	*arg = argv[i];
	char		errbuf[128];

		printf("--> parse_target(%s)\n", arg);

		if ( ! parse_target(arg, errbuf, add_target, 0) )
		{
			printf("ERROR: %s\n", errbuf);
			return 1;
		}
	}

	return 0;
}

#endif /* TESTING */
