/*
 * $Id: //devel/tools/main/nbtscan/netbios_fixname.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	NETBIOS names sometimes contain funny characters (especially the
 *	__MSBROWSE__ thing) so this fixes them up into a printable format.
 *	All non-printable characters are converted to dots, which seems
 *	to be the same algorithm used by Windows NBTSTAT.
 *
 *	But if we're generating perl, we elide this whole process because
 *	we want to get all the magic binary values.
 */
#include "nbtscan_common.h"
#include <ctype.h>
#include "nbtdefs.h"

char *NETBIOS_fixname(char *buf)
{
char	*buf_save = buf;

#ifdef ENABLE_PERL
	if ( gen_Perl ) return buf;
#endif

	assert(buf != 0);

	for ( ; *buf; buf++ )
	{
		if ( ! isprint(*buf) )
			*buf = '.';
	}

	return strip(buf_save);
}
