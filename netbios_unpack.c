/*
 * $Id: //devel/tools/main/nbtscan/netbios_unpack.c#1 $
 *
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include "penlib.h"


/*
 * NETBIOS_unpack()
 *
 *	Given a pointer to a NETBIOS name in the stupid "compressed"
 *	format (which actually takes up *more* space than the original),
 *	unpack it to the given output buffer, returning the number of
 *	bytes generated.
 *
 *	The input buffer is passed by handle to pointer, so we use this
 *	to advance along the input, leaving it pointing to the first
 *	character after the end of the name. This byte should be the
 *	length of the next segment, or zero if it's the end.
 *
 *	The output name is always terminated with a NUL byte, and the
 *	return count does /not/ include that trailing NUL.
 *
 *	If the input is not well formed, the return value is <0 and
 *	the input pointer is left at an indeterminate state, but the
 *	output buffer is *always* terminated with a NUL byte.
 *
 *	NOTE: the return value actually encodes the reason for
 *	failure, but this is only for debugging. We do NOT document
 *	these return codes other than "negative means error", and it
 *	is unwise to count on them.
 */

int __stdcall NETBIOS_unpack(const char **ibuf, char *obuf, int osize)
{
int          isize;
char		*obuf_save,
            *obuf_max;
const char  *ibuf_save;

	assert(  ibuf  != 0 );
	assert( *ibuf  != 0 );
	assert(  obuf  != 0 );
	assert(  osize  > 0 );

	ibuf_save = *ibuf;
	obuf_save =  obuf;

	/*----------------------------------------------------------------
	 * The length in bytes of the "compressed" name must be even, as
	 * each final character is made of two input bytes. If the size
	 * is odd, it's just a bogus input.
	 *
	 * Then make sure the # of bytes will for sure fit in the output.
	 */
	isize = *(*ibuf)++;

	if ( (isize % 2) != 0 )
	{
		/* must be even length */
		return -1;
	}

	if ( (isize /= 2) > osize )
	{
		/* output buffer not big enough */
		return -2;
	}

	obuf_max = obuf + isize;

	while ( obuf < obuf_max )
	{
	unsigned int	c1 = (unsigned int)( *(*ibuf)++ - 'A' ),
			c2 = (unsigned int)( *(*ibuf)++ - 'A' );

		if ( c1 > 15  ||  c2 > 15 )	return -3;

		*obuf++ = (char)( (c1 << 4) | c2 );
	}

	*obuf = '\0';

	/* round up to even word boundary */
	if ( (*ibuf - ibuf_save) % 2 )
		++*ibuf;

	return (int)(obuf - obuf_save);
}

#ifdef TESTING

#include <stdio.h>

static const char	name1[] = "\040CKFDENECFDEFFCFGEFFCCACACACACACA";
static const char	name2[] = "\040CACACACACACACACACACACACACACACACA";

int main()
{
char		obuf[100];
const char	*p;

	p = name1;
	NETBIOS_unpack(&p, obuf, sizeof obuf);

	printf("name1: \"%s\" -> \"%s\"\n", name1, obuf);


	p = name2;
	NETBIOS_unpack(&p, obuf, sizeof obuf);

	printf("name2: \"%s\" -> \"%s\"\n", name2, obuf);

	return 0;
}
#endif
