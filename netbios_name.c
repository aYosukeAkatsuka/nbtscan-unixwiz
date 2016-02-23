/*
 * $Id: //devel/tools/main/nbtscan/netbios_name.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	Return a string that indicates the WINS resource record type. These
 *	are taken from the paper 
 *
 *		http://neohapsis.com/resources/wins.htm
 *
 *	The meaning of many names depends on whether this is a group or not,
 *	and in some cases the name itself plays a part in the recognition
 *	of the service description.
 *
 */
#include "nbtscan_common.h"
#include <stdio.h>
#include <string.h>
#include "nbtdefs.h"

#define		UNIQUE		0x000
#define		XGROUP		0x100

const char *NETBIOS_name(const struct nodeinfo *np)
{
int	unique;
int	swvalue;

	assert(np != 0);

	unique = !!NODE_FLAGS_GROUP(np);

	swvalue = (unique << 8) | (0xFF & np->type);

	switch ( swvalue )
	{
	  case UNIQUE | 0x01:	return "Messenger Service<1>";
	  case UNIQUE | 0x03:	return "Messenger Service<3>";
	  case UNIQUE | 0x06:	return "RAS Server Service";
	  case UNIQUE | 0x1F:	return "NetDDE Service";
	  case UNIQUE | 0x1B:	return "Domain Master Browser";
	  case UNIQUE | 0x1D:	return "Master Browser";
	  case UNIQUE | 0x20:	return "File Server Service";
	  case UNIQUE | 0x21:	return "RAS Client Service";
	  case UNIQUE | 0x22:	return "MS Exchange Interchange";
	  case UNIQUE | 0x23:	return "MS Exchange Store";
	  case UNIQUE | 0x24:	return "MS Exchange Directory";
	  case UNIQUE | 0x87:	return "MS Exchange MTA";
	  case UNIQUE | 0x6A:	return "MS Exchange IMC";
	  case UNIQUE | 0xBE:	return "Network Monitor Agent";
	  case UNIQUE | 0xBF:	return "Network Monitor Application";
	  case UNIQUE | 0x30:	return "Modem Sharing Server Service";
	  case UNIQUE | 0x31:	return "Modem Sharing Client Service";
	  case UNIQUE | 0x43:	return "SMS Clients Remote Control";
	  case UNIQUE | 0x44:	return "SMS Admin Remote Control Tool";
	  case UNIQUE | 0x45:	return "SMS Clients Remote Chat";
	  case UNIQUE | 0x46:	return "SMS Clients Remote Transfer";
	  case UNIQUE | 0x52:	return "DEC Pathworks TCP svc";

	  case XGROUP | 0x00:	return "Domain Name";
	  case XGROUP | 0x01:	return "Master Browser";
	  case XGROUP | 0x1E:	return "Browser Service Elections";


	  case XGROUP | 0x42:
		if ( strcmp(np->name, "MLI_GROUP_BRAD") == 0)
			return "Dr. Solomon AV Management";
		break;

	  case UNIQUE | 0x42:
		if ( strncmp(np->name, "MLI", 3) == 0 )
			return "Dr. Solomon AV Management";
		break;

	  case XGROUP | 0x1C:
		if ( strcmp(np->name, "INet~Services") == 0 )
			return "IIS";
		else
			return "Domain Controller";

	  case UNIQUE | 0x00:
		if ( strncmp(np->name, "IS~", 3) == 0 )
			return "IIS";
		else
			return "Workstation Service";

	  default:
		return 0;
	}

	return 0;
}
