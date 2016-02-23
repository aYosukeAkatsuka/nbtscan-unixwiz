/*
 * $Id: //devel/tools/main/nbtscan/printable_NETBIOS_question_type.c#1 $
 *
 *	When fooling with RFC1002 NETBIOS nameserver queries and responses,
 *	we often wish to decode the question type value in a printable form.
 *	There are only two values defined (NB and NBSTAT), but this gives
 *	us an easy way to extend it. This might find its way into other
 *	tools besides NBTSCAN.
 *
 *	If the type is known, a fixed string is returned directly, but if we
 *	don't know the type then we format a variable version into the user's
 *	buffer. The user should NOT rely on the contents of the temporary buffer
 *	after this function comes back: the return value is the exclusive
 *	feedback point.
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include <stdio.h>
#include "penlib.h"

const char * __stdcall printable_NETBIOS_question_type(char *obuf, int qtype)
{
	assert(obuf != 0);

	switch (qtype)
	{
	  case 0x0020:	return "NB";
	  case 0x0021:	return "NBSTAT";
	  default:	sprintf(obuf, "0x%04x", qtype);
			return obuf;
	}
}
