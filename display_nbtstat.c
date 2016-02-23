/*
 * $Id: //devel/tools/main/nbtscan/display_nbtstat.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	This displays a single NETBIOS NMB query to the user, either in
 *	"limited" or "full" format. There is always a line of the form
 *
 *	123.123.123.123 DOMAIN\COMPUTER    11:22:33:44:55:66   info
 * 	^-- ip addr     ^-- name           ^-- mac address     ^--- other stuff
 *
 *	These lines are good for a quick summary, but if "full" is turned on,
 *	everything reported from the other end is dumped. This makes it a
 *	really helpful when knowing what is on the other end.
 */
#include "nbtscan_common.h"
#include <stdio.h>
#include <string.h>
#include "nbtdefs.h"

void display_nbtstat(FILE *ofp, const struct NMB_query_response *rsp, int full)
{
char	reportbuf[256],
	*p = reportbuf;
char	computername[32];

	assert(ofp != 0);
	assert(rsp != 0);

	/*----------------------------------------------------------------
	 * The full name is DOMAIN\MACHINE, but some systems have no names
	 * at all (don't know why), so we display them in a special format.
	 * Not sure what this means...
	 */
	if (rsp->domain[0] == '\0'  &&  rsp->computer[0] == '\0' )
		sprintf(computername, "-no name-");
	else
		sprintf(computername, "%s\\%s",
			rsp->domain,
			rsp->computer );

	p += sprintf(p, "%-15s %-31s",
		inet_ntoa(rsp->remote.sin_addr),	/* IP address		*/
		computername);				/* DOMAIN\COMPUTER	*/

	if ( show_mac_address && ! full )
	{
		*p++ = ' ';
		p += nstrcpy(p, rsp->ether);
	}


	if ( rsp->sharing )	p += nstrcpy(p, " SHARING"  );
	if ( rsp->is_dc)	p += nstrcpy(p, " DC"       );
	if ( rsp->has_IIS )	p += nstrcpy(p, " IIS"      );
	if ( rsp->has_Exchange)	p += nstrcpy(p, " EXCHANGE" );
	if ( rsp->has_Notes)	p += nstrcpy(p, " NOTES"    );
	if ( rsp->has_RAS )     p += nstrcpy(p, " RAS"      );
	if ( rsp->has_unknown)	p += nstrcpy(p, " ?"        );

	/*----------------------------------------------------------------
	 * If we have a user, display it after a U= token. But we put quotes
	 * around it if the user name contains any spaces. This is kind of
	 * a crock.
	 */
	if ( rsp->user[0] )
	{
		const char *quote = (strchr(rsp->user, ' ') == 0)
		                  ? ""
		                  : "\"";

		p += sprintf(p, " U=%s%s%s", quote, rsp->user, quote);
	}

	*p++ = '\n';
	*p = '\0';

	fputs(reportbuf, ofp);

	if ( full )
	{
	int	i;
	char	dnsbuf[132];
	char	dispbuf[256];

		for (i = 0; i < rsp->nnodes; i++ )
		{
		const struct nodeinfo	*ni = &rsp->nodes[i];
		int			isgroup = NODE_FLAGS_GROUP(ni);
		char			namebuf[16];
		const char              *svcname = ni->svcname;

			if ( svcname == 0 ) svcname = "-unknown-";

			NETBIOS_fixname( strcpy(namebuf, ni->name) );

			fprintf(ofp, "  %-15s<%02x> %s %s\n",
				namebuf,
				0xFF & ni->type,
				isgroup ? "GROUP " : "UNIQUE",
				svcname );
		}

		if ( no_inverse_lookup
		 || ip_to_name(rsp->remote.sin_addr.s_addr,
				dnsbuf, sizeof dnsbuf) == 0 )
		{
			dnsbuf[0] = '\0';
		}

		// strip trailing white from this line :-(
		sprintf(dispbuf, "  %s   ETHER  %s", rsp->ether, dnsbuf);

		strip(dispbuf);

		fprintf(ofp, "%s\n\n", dispbuf);
	}

	fflush(ofp);
}
