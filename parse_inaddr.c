/*
 * $Id: //devel/tools/main/nbtscan/parse_inaddr.c#1 $
 *
 *	This is a replacment for the useful but not [yet] portable
 *	inet_aton() function. The user passes in a pointer to what
 *	purports to be a dots-and-digits IP address, and this function
 *	cracks it apart, storing it into the pointed-to unsigned long.
 *
 *	Return value is 0 on error and nonzero if all is well. This
 *	function is a better interface than inet_addr() becauser it is
 *	able to recognize all the possible addresses, including
 *	255.255.255.255. This is an error return for inet_addr().
 *
 *	This function /strictly/ implements dots-and-digits, so it's
 *	not possible to fool it with the usual very large decimal
 *	number tricks. It also does not permit any deviation from
 *	the exact requirements of dotted-quads.
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include <ctype.h>
#include "penlib.h"

/*
 * scan_digits()
 *
 *	Advance along the string scanning the digits seen, returning
 *	TRUE if all is well and FALSE if not. We return failure if there
 *	are no digits found or if the scanned number is out of range of
 *	an IP octet (0..255).
 *
 *	We also pass the character that should be seen immediately
 *	after the digit sequence and return failure if this is not found.
 */
static int scan_digits(const char **pstring, int *pdigit, char after)
{
	assert( pstring != 0);
	assert(*pstring != 0);
	assert( pdigit  != 0);

#undef  P
#define	P	(*pstring)

#undef N
#define N	(*pdigit)

	N = 0;

	if ( ! isdigit(*P) )	return FALSE;

	while ( isdigit(*P) )
		N = (N * 10) + (*P++ - '0');

	if ( N >= 256 )		return FALSE;

	if ( *P++ != after )	return FALSE;

	return TRUE;
#undef P
#undef N
}

int parse_inaddr(const char *cp, unsigned long *inp)
{
int	a = 0,
	b = 0,
	c = 0,
	d = 0;

	assert(cp  != 0);
	assert(inp != 0);

	*inp = (unsigned long) -1; /* same as INADDR_NONE */

	if ( ! scan_digits(&cp, &a, '.' ) )	return FALSE;
	if ( ! scan_digits(&cp, &b, '.' ) )	return FALSE;
	if ( ! scan_digits(&cp, &c, '.' ) )	return FALSE;
	if ( ! scan_digits(&cp, &d, '\0') )	return FALSE;

	*inp = build_ipaddr_from_octets(a, b, c, d);

	return TRUE;
}

/*------------------------------------------------------------------------
 * TEST HARNESS
 *
 *	This function will be in the center of our little universe, so
 *	we really want to make sure that it works correctly. This is a
 *	test harness that uses a table of known values and compares them
 *	with the result of the function.
 * 
 *	As failures are found, add them to this table for testing!
 */
#ifdef TESTING

static const struct tester {
	const char	*remote;
	int		expect_rc;
	unsigned long	expect_addr;	/* HOST word order!	*/
} table[] = {

	/* all legal addresses */
	{ "209.60.37.2",		TRUE,	0xFFFFFFFF	},
	{ "209.60.37.255",		TRUE,	0xFFFFFFFF	},
	{ "127.0.0.1",			TRUE,	0x7F000001	},

	/* odd values, but are legal */
	{ "0.0.0.0",			TRUE,	0x00000000	},
	{ "255.255.255.255",		TRUE,	0xFFFFFFFF	},

	/* all have out-of-range values */
	{ "256.255.255.255",		FALSE,	0xFFFFFFFF	},
	{ "255.256.256.255",		FALSE,	0xFFFFFFFF	},
	{ "255.255.256.255",		FALSE,	0xFFFFFFFF	},
	{ "255.255.255.256",		FALSE,	0xFFFFFFFF	},
	{ "127.0.0.2000",		FALSE,	0xFFFFFFFF	},
	{ "127.0.0.2000",		FALSE,	0xFFFFFFFF	},

	{ "-1.0.0.0",			FALSE,	0xFFFFFFFF	},

	{ 0 }
};

#define	SF(x)	((x) ? "success" : "failure")

int main()
{
const struct tester	*ptest;

	printf("parse_inaddr() test driver\n");


	for ( ptest = table; ptest->remote != 0; ptest++ )
	{
	int		rc;
	unsigned long	result_addr,
			should_be;

		should_be = ptest->expect_addr);
		rc = parse_inaddr(ptest->remote, &result_addr) != 0;

/*
		printf("string %s, rc = %d, expected = %s\n",
			ptest->remote,
			rc,
			SF(ptest->expect_rc));
 */

		/* rc == 0 -> failure */
		/* rc != 0 -> successd */

		if ( rc != ptest->expect_rc )
		{
			printf("ERROR: for string \"%s\"\n", ptest->remote);
			printf("  expected %s, got %s\n",
				SF(ptest->expect_rc), SF(rc));
		}
		else
		{
			printf("%s OK for string \"%s\"\n",
				SF(rc),
				ptest->remote);
		}
	}
}

#endif	/* TESTING */
