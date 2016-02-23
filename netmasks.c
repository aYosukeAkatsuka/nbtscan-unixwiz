/*
 * $Id: //devel/tools/main/nbtscan/netmasks.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	Index this table with the number of bits, and it returns
 *	the netmask for it.
 *
 *	BE CAREFUL about indexing out of range!
 */

#ifndef COMMONFILE
# define COMMONFILE "libcommon.h"
#endif
#include COMMONFILE

#include "penlib.h"

const unsigned long netmasks[] = {      0x00000000,		/*       0 */

	0x80000000, 0xC0000000, 0xE0000000, 0xF0000000,		/*  1 -  4 */
	0xF8000000, 0xFC000000, 0xFE000000, 0xFF000000,		/*  5 -  8 */
	0xFF800000, 0xFFC00000, 0xFFE00000, 0xFFF00000,		/*  9 - 12 */
	0xFFF80000, 0xFFFC0000, 0xFFFE0000, 0xFFFF0000,		/* 13 - 16 */
	0xFFFF8000, 0xFFFFC000, 0xFFFFE000, 0xFFFFF000,		/* 17 - 20 */
	0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00,		/* 21 - 24 */
	0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 0xFFFFFFF0,		/* 25 - 28 */
	0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE, 0xFFFFFFFF,		/* 29 - 32 */
 };
