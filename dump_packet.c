/*
 * $Id: //devel/tools/main/nbtscan/dump_packet.c#1 $
 *
 *	Given an NBT packet plus the length of same, dump it in raw format to
 *	the usual output place. We try to decode bits and pieces of this so we
 *	can decide why things are broken.
 *
 *	This is defined in RFC 1002, but much of it is duplicated here for
 *	reference. It's a >big< RFC. Note that we only talk about the response
 *	here -- the *request* is a separate animal.
 *
 *	All short/long values are in network word order, which is big-endian
 *	(Intel machines are little-endian and require byte swapping). The
 *	RFC numbers bits with the MSB as bit zero, but we've never operated
 *	that way so we number them from the low end. Sorry.
 *
 *	Section 4.2.1 defines the general format of name service packets as:
 *
 *	+------------------------------+------------------------------+
 *	| 							      |
 *	~                             header			      ~
 *	| 							      |
 *	+------------------------------+------------------------------+
 *	| 							      |
 *	~                       QUESTION ENTRIES                      ~
 *	| 							      |
 *	+------------------------------+------------------------------+
 *	| 							      |
 *	~                     ANSWER RESOURCE RECORDS                 ~
 *	| 							      |
 *	+------------------------------+------------------------------+
 *	| 							      |
 *	~                     AUTHORITY RESOURCE RECORDS              ~
 *	| 							      |
 *	+------------------------------+------------------------------+
 *	| 							      |
 *	~                    ADDITIONAL RESOURCE RECORDS              ~
 *	| 							      |
 *	+------------------------------+------------------------------+
 *
 */
#include "nbtscan_common.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "nbtdefs.h"

void dump_nbtpacket(const struct NMBpacket *pak, int paklen, FILE *ofp)
{
unsigned short	flags;
unsigned short	op;
char		flagbuf[132],
		hexbuf[132], *phex,
		chrbuf[17],  *pchr,
		*p = flagbuf;
const char	*pakdata;

	assert(pak != 0);
	assert(ofp != 0);

	/*----------------------------------------------------------------
	 * Before we display everything, decode the flags:
	 *
	 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * | R |    opcode     |AA |TC |RD |RA | 0 | 0 | B |    RCODE      |
	 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 *                 ^               ^               ^               ^
	 *
	 * R	0 = request
	 *	1 = response
	 *
	 * OP	0 = query
	 *	5 = registration
	 *	6 = release
	 *	7 = WACK
	 *	8 = refresh
	 *
	 * AA	1 if authoritative answer
	 * TC	1 if packet was truncated
	 * RD	1 if recursion desired
	 * B	1 if broadcast
	 */
	flags = ntohs(pak->flags);
	p     = flagbuf;

	if ( flags & 0x8000 )	p += sprintf(p, "RSP");		/* response */
	else			p += sprintf(p, "REQ");		/* reqeust */

	switch ( op = (flags & 0x7000) >> 11 )
	{
	  case 0:	p += sprintf(p, " op=Query");	break;
	  case 5:	p += sprintf(p, " op=Reg");	break;
	  case 6:	p += sprintf(p, " op=Release");	break;
	  case 7:	p += sprintf(p, " op=WACK");	break;
	  case 8:	p += sprintf(p, " op=Refresh");	break;
	  default:	p += sprintf(p, " op=%d", op);	break;
	}

	if ( flags & 0x0400 ) p += sprintf(p, " AA");
	if ( flags & 0x0200 ) p += sprintf(p, " TC");
	if ( flags & 0x0100 ) p += sprintf(p, " RD");
	if ( flags & 0x0080 ) p += sprintf(p, " RA");
	if ( flags & 0x0010 ) p += sprintf(p, " B");

	p += sprintf(p, " rcode=%d", flags & 0x000F);

	fprintf(ofp, "DUMP OF PACKET\n");
	fprintf(ofp, "  tranid       = %d\n",		ntohs(pak->tranid)  );
	fprintf(ofp, "  flags        = 0x%02x - %s\n",	flags,  flagbuf);

	fprintf(ofp, "  query count  = %d\n",		ntohs(pak->qdcount) );
	fprintf(ofp, "  answer count = %d\n",		ntohs(pak->ancount) );
	fprintf(ofp, "  ns count     = %d\n",		ntohs(pak->nscount) );
	fprintf(ofp, "  ar count     = %d\n",		ntohs(pak->arcount) );

	/*----------------------------------------------------------------
	 * Now decode the raw data so we can see what's what. We subtract
	 * the offset of the header from the total packet length, then start
	 * dumping a byte at a time.
	 */
	pakdata = pak->data;

	fprintf(ofp, "\n");
	fprintf(ofp, "  data bytes ... %d\n",		paklen);

	phex  = hexbuf;
	pchr  = chrbuf;

	while ( paklen-- > 0 )
	{
	unsigned char	c = *pakdata++ & 0xFF;

		phex += sprintf(phex, "%02X ", c);

		if (!isprint(c)) c = '.';

		*pchr++ = c;

		*phex = '\0';
		*pchr = '\0';

		/*--------------------------------------------------------
		 * Reached the end of the line? Dump it.
		 */
		if( (pchr - chrbuf) >= 16 )
		{
			fprintf(ofp, "\t%-48s  %s\n", hexbuf, chrbuf);
			phex = hexbuf;
			pchr = chrbuf;
		}
	}

	if ( pchr > chrbuf)
		fprintf(ofp, "\t%-48s  %s\n", hexbuf, chrbuf);

	fprintf(ofp, "\n");

	fflush(ofp);
}
