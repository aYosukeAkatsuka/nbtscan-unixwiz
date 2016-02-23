/*
 * $Id: //devel/tools/main/nbtscan/netbios_pack.c#1 $
 *
 *	This creates a NETBIOS name and encodes it for transmission on
 *	the wire. The encoding puts a single length byte followed by
 *	two bytes per character. This is done in two levels: raw, and
 *	customary.
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include "penlib.h"

/*
 * NETBIOS_raw_pack_name()
 *
 *	Given a buffer containing a name plus a size, encode it in
 *	the usual NETBIOS way. The length is encoded as the number of
 *	output bytes (input bytes times two), and we return the total
 *	bytes placed in the output buffer.
 *
 *	We do put a NUL byte at the end of the output buffer, but
 *	this is a courtesy and it's not counted in the returned
 *	length.
 */
int __stdcall NETBIOS_raw_pack_name(const char *ibuf, int isize, char *obuf)
{
char	*obuf_save = obuf;

	assert( ibuf != 0 );
	assert( obuf != 0 );

	*obuf++ = (char)(isize * 2);

	while (isize-- > 0 )
	{
	unsigned int c = *(unsigned char *)ibuf;

		*obuf++ = (char)( 'A' + ( (c >> 4) & 0x0F ) );
		*obuf++ = (char)( 'A' + (  c       & 0x0F ) );

		ibuf++;
	}
	*obuf = '\0';

	return (int)(obuf - obuf_save);
}

int __stdcall NETBIOS_pack_name(const char *ibuf, int itype, char *obuf)
{
char	tempbuf[16+1];

	assert(ibuf != 0);
	assert(obuf != 0);

	/*----------------------------------------------------------------
	 * Preformat the name to be the format that we require for a
	 * normal NETBIOS name. The usual rule is 15 characters of 
	 * name (space padded) with a type code at the end. The special
	 * case of the name "*" is passed literally to the output
	 * buffer with NUL byte padding instead of spaces.
	 *
	 * +---------------------------------------------------------------+
	 * |*  | \0| \0| \0| \0| \0| \0| \0| \0| \0| \0| \0| \0| \0| \0| \0|
	 * +---------------------------------------------------------------+
	 *
	 * |                                                               |
	 * |<------------------------ 16 bytes --------------------------->|
	 * |                                                               |
	 *
	 * +---------------------------------------------------------------+
	 * |*  | S | M | B | S | E | R | V | E | R | sp| sp| sp| sp| sp| TT|
	 * +---------------------------------------------------------------+
	 *
	 * where "TT" is the type desired.
	 */
	if ( ibuf[0] == '*'  &&  ibuf[1] == '\0' )
	{
		memset(tempbuf, 0, sizeof tempbuf);
		tempbuf[0] = '*';
	}
	else
	{
		sprintf(tempbuf, "%-15.15s%c", ibuf, itype);
	}

	return NETBIOS_raw_pack_name(tempbuf, 16, obuf);
}

#ifdef TESTING

int main()
{
char	ibuf[128];

	while ( gets(ibuf) != 0 )
	{
	char	packbuf[256], *p = packbuf;
	int	rc;

		rc = NETBIOS_name_pack(ibuf, 32, &p);

		printf("rc = %d {%s}\n", rc, packbuf);
	}

	return 0;
}

#endif
