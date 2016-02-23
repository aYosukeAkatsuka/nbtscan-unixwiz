/*
 * $Id: //devel/tools/main/nbtscan/gen_perl.c#2 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	This generates our perl output in hashref format. At the start of
 *	the program, we generate the "header", which should be common to
 *	most of our perl hashrefs.
 *
 *	There is a header part that includes
 *
 *		$ref->{DATE}     - date the scan was ruin
 *		$ref->{CMDLINE}  - array of argv
 *
 *	Then followed by the body:
 *
 *		$ref->{NBTSCAN} = {
 *
 *	Which itself is a hash to each of the nbtscan outputs. In each
 *	case, the key is the IP address, followed by all the params.
 *
 *	This is used with
 *
 *		my $ref = do perlfile;
 *
 *	NOTE: as of 1.0.34, we now quote all the 'NAME' => ... values so
 *	that Ruby can use them.
 *
 */
#include "nbtscan_common.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "nbtdefs.h"

#ifdef ENABLE_PERL

#define	fpr	fprintf		/* shorthand */

/*
 * nbq()
 *
 *	This quotes a NETBIOS value for perl. We always wrap the thing in
 *	single quotes and return a pointer to the temporary buffer that
 *	holds the converted value.
 *
 *	NOTE: this uses a static buffer whose contents are overwritten
 *	upon each call.
 */
static char *nbq(const char *ibuf)
{
static char	tmpbuf[1024];
char            *obuf = tmpbuf;

	assert(ibuf != 0);

	*obuf++ = '\'';

	for ( ; *ibuf; ibuf++ )
	{
		if ( ! isprint(*ibuf) )
			obuf += sprintf(obuf, "\\x%02X", *ibuf & 0xFF);

		else
		{
			if ( *ibuf == '\\'  ||  *ibuf == '\'' )
				*obuf++ = '\\';

			*obuf++ = *ibuf;
		}
	}

	*obuf++ = '\'';
	*obuf   = '\0';

	return tmpbuf;
}

/*
 * start_perl()
 *
 *	Start generating the perl output: this is header only, and there is
 *	no per-target information here.
 */

void start_perl(FILE *ofp, char **argv)
{
time_t	now;

	assert(ofp  != 0);
	assert(argv != 0);

	time(&now);

	fpr(ofp, "# perl hashref output\n");
	fpr(ofp, "# use as  'my $ref = do filename;'\n");

	fpr(ofp, "{\n");
	fpr(ofp, "    'DATE'    => %s,\n", nbq( strip(ctime(&now))));

	/*----------------------------------------------------------------
	 * Generate the command line one word at a time, but by doing it
	 * this way we avoid an intermediate buffer of unknown size. Note
	 * that it's possible for command lines to be *really* long if we
	 * are using a long list of networks.
	 */
	fpr(ofp, "    'CMDLINE' => [\n");

	for ( ; *argv; argv++ )
	{
		fpr(ofp, "\t%s,\n", nbq(*argv) );
	}

	fpr(ofp, "    ],\n");

	fpr(ofp, "    'NBTSCAN' => {\n");
}

void end_perl(FILE *ofp)
{
	assert(ofp != 0);

	fpr(ofp, "    }\n");
	fpr(ofp, "}\n");
}

void generate_perl(FILE *ofp, const struct NMB_query_response *rsp)
{
char	dnsbuf[1024];
char	*ipaddr;
int	i;

	assert(ofp  != 0);
	assert(rsp  != 0);

	dnsbuf[0] = '\0';

	/*----------------------------------------------------------------
	 * Look up the DNS name if requested.
	 */
	if ( no_inverse_lookup
	  || ip_to_name(rsp->remote.sin_addr.s_addr, dnsbuf, sizeof dnsbuf) == 0 )
	{
		dnsbuf[0] = '\0';
	}

	/*----------------------------------------------------------------
	 *
	 */
	ipaddr = inet_ntoa(rsp->remote.sin_addr);

	fpr(ofp, "\t'%s' => {\n", ipaddr );

	fpr(ofp, "\t    'rDNS'         => %s,\n", nbq(dnsbuf)        );
	fpr(ofp, "\t    'IPAddress'    => %s,\n", nbq(ipaddr)        );
	fpr(ofp, "\t    'MACAddress'   => %s,\n", nbq(rsp->ether)    );
	fpr(ofp, "\t    'DomainName'   => %s,\n", nbq(rsp->domain)   );
	fpr(ofp, "\t    'ComputerName' => %s,\n", nbq(rsp->computer) );
	fpr(ofp, "\t    'Sharing'      => %d,\n", rsp->sharing       );
	fpr(ofp, "\t    'DC'           => %d,\n", rsp->is_dc         );
	fpr(ofp, "\t    'Exchange'     => %d,\n", rsp->has_Exchange  );
	fpr(ofp, "\t    'IIS'          => %d,\n", rsp->has_IIS       );
	fpr(ofp, "\t    'LotusNotes'   => %d,\n", rsp->has_Notes     );
	fpr(ofp, "\t    'Nodes'        => [\n");

	for (i = 0; i < rsp->nnodes; i++ )
	{
	const struct nodeinfo	*ni = &rsp->nodes[i];
	int			isgroup = NODE_FLAGS_GROUP(ni);
	const char              *svcname = ni->svcname;

		if ( svcname == 0 ) svcname = "-unknown-";

		fpr(ofp, "\t\t[ ");
		fpr(ofp, "%-16s, ",  nbq(ni->name) );
		fpr(ofp, "0x%02X, ", ni->type & 0xFF);
		fpr(ofp, "'%-6s', ", isgroup ? "GROUP" : "UNIQUE");
		fpr(ofp, "%s ],\n",  nbq( svcname ));
	}

	fpr(ofp, "\t    ],\n");
	fpr(ofp, "\t},\n\n");

	fflush(ofp);
}

#endif
