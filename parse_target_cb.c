/*
 * $Id: //devel/tools/main/nbtscan/parse_target_cb.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	This routine parses a "target" host (or subnet) specification and
 *	produces the struct containing the full list of all hosts included
 *	in the range. The specification patterns were loosely suggested
 *	by Fyodor's "nmap" but our code is all original. We also don't
 *	permit anywhere near the flexibility that nmap does, but this is
 *	plenty for us now.
 */
#include "nbtscan_common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nbtdefs.h"

static int __stdcall add_target_callback(unsigned long addr1,
					 unsigned long addr2,
					 const char *hostname,
					 void *hint )
{
struct targetRange	*targ;
long			nhosts = (addr2 - addr1) + 1;

	assert(addr1 <= addr2);

	UNUSED_PARAMETER(hint);

	targ   = (struct targetRange *)malloc(sizeof *targ);

	if ( hostname == 0 ) hostname = "-range-";

	targ->printable		= strdup(hostname);
	targ->firstaddr.s_addr	= htonl(addr1);
	targ->lastaddr.s_addr	= htonl(addr2);
	targ->nhosts		= nhosts;
	targ->next		= 0;
	targ->hostsDone		= calloc(nhosts, 1);
	targ->hostsRemaining	= targ->nhosts;

	memset(targ->hostsDone, 0, nhosts);

	add_targetRange(targ);

	return TRUE;
}

int parse_target_range(const char *str, char *errbuf)
{
	return parse_target(str, errbuf, add_target_callback, 0);
}

