/*
 * $Id: //devel/tools/main/nbtscan/printable_NETBIOS_question_class.c#1 $
 *
 *	When fooling with RFC1002 NETBIOS nameserver queries and responses, we
 *	often wish to decode the question class value in a printable form. We
 *	only have *one* possible value -- "IN" (internet) class -- but to be
 *	consistent we do it this way. Then if we ever have to add more later
 *	everybody gets the benefit.
 */
#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE
#include <stdio.h>
#include "penlib.h"

const char * __stdcall printable_NETBIOS_question_class(char *obuf, int qtype)
{
	assert(obuf != 0);

	switch (qtype)
	{
	  case 0x0001:	return "IN";

	  default:	sprintf(obuf, "0x%04x", qtype);
			return obuf;
	}
}
