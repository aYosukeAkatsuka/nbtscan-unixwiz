/*
 * $Id: //devel/tools/main/nbtscan/process_response.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	This runs through the barely-processed NMB response object built
 *	from the packet received from the other end and performs a bit of
 *	summary data on it. We try to determine the computer and domain
 *	(or workgroup) name plus whether sharing is enabled on the node
 *	or not.
 */
#include "nbtscan_common.h"
#include <string.h>
#include "nbtdefs.h"

void process_response(struct NMB_query_response *rsp)
{
int	i;

	assert(rsp != 0);

	rsp->computer[0] = '\0';
	rsp->domain  [0] = '\0';
	rsp->user    [0] = '\0';
	rsp->has_RAS     = FALSE;
	rsp->is_dc       = FALSE;
	rsp->sharing     = FALSE;
	rsp->has_unknown = FALSE;

	for (i = 0; i < rsp->nnodes; i++ )
	{
	struct nodeinfo	   *ni = &rsp->nodes[i];
	int                 isgroup = NODE_FLAGS_GROUP(ni);
	int                 t = ni->type;

		/*--------------------------------------------------------
		 * Look up the printable NETBIOS resource name and stick
		 * it into the local node buffer. This is NULL if not
		 * known, and we mark us as having some unknown ones: this
		 * might help us research the new stuff.
		 */
		if ( (ni->svcname = NETBIOS_name(ni)) == 0 ) rsp->has_unknown++;

		/*--------------------------------------------------------
		 * A GROUP node <00> is the domain name, and this is not
		 * always found if this is a workgroup environment with
		 * no domain controller.
		 */
		if ( rsp->domain[0] == '\0' )
		{
			if ( isgroup  &&  (t == 0x00) )
			{
				strcpy(rsp->domain, ni->name);
			}
		}

		/*--------------------------------------------------------
		 * Look for the computer name. This is always a UNIQUE name,
		 * and we think it's always first.
		 */
		if ( rsp->computer[0] == '\0'  &&  ! isgroup )
		{
			switch ( t )
			{
			/*------------------------------------------------
			 * Unique type <00> is either "IIS" or "Workstation
			 * Service" depending on whether we have the IS~
			 * part at the beginning.
			 */
			  case 0x00:
				if ( strncmp(ni->name, "IS~", 3) != 0 )
					strcpy(rsp->computer, ni->name);
				break;

			  case 0x06:	/* RAS Client Service		*/
			  case 0x01:	/* Messenger Service (uncommon)	*/
			  case 0x1F:	/* NetDDE service		*/
			  case 0x20:	/* File sharing service		*/
			  case 0x2B:	/* Lotus Notes Server Service	*/
				strcpy(rsp->computer, ni->name);
				break;

			  default:
				/*nothing*/
				break;
			}
		
		}

                /*--------------------------------------------------------
                 * Sharing is on if the File Server Service is published,
                 * and this is noticed with a unique type of <20>.
                 */
		if ( ! isgroup  &&  (t == 0x20) )
			rsp->sharing = TRUE;

		/*--------------------------------------------------------
		 * UNIQUE<06> seems to be RAS, which indicates modems?
		 */
		if ( ! isgroup  &&  (t == 0x06) )
		{
			rsp->has_RAS = TRUE;
		}

		/*--------------------------------------------------------
		 * It seems that being a domain controller and running IIS
		 * are pretty similar. If the token is <1C> and the name
		 * matches the domain name, it's a domain controller.
		 */
		if ( isgroup && (t == 0x1C) )
		{
			if ( strcmp(ni->name, "INet~Services") == 0 )
				rsp->has_IIS = TRUE;
			else if ( strcmp(ni->name, rsp->domain) == 0 )
				rsp->is_dc = TRUE;
		}

		/*--------------------------------------------------------
		 * We've observed that UNIQUE<87> and UNIQUE<6A> are MS
		 * Exchange, but we don't remember how we got that.
		 */
		if ( ! isgroup && (t == 0x87 || t == 0x6A) )
		{
			rsp->has_Exchange = TRUE;
		}

		if ( ! isgroup && (t == 0x2B) )
		{
			rsp->has_Notes = TRUE;
		}

		/*--------------------------------------------------------
		 * If this is messenger service for something other than
		 * the computer name, this is probably a user.
		 */
		if ( ! isgroup && (t == 0x03) )
		{
			if ( strcmp(ni->name, rsp->computer) != 0 )
				strcpy(rsp->user, ni->name);
		}
	}

	NETBIOS_fixname(rsp->domain);
	NETBIOS_fixname(rsp->computer);
}
