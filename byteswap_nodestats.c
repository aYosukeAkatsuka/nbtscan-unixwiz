/*
 * $Id: //devel/tools/main/nbtscan/byteswap_nodestats.c#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	Given a NODE_statistics struct, byte swap it to the local word
 *	order. This is meant only for processing buffers we have just
 *	read: there is no corresponding local->net version (sorry).
 */
#include "nbtscan_common.h"
#include "nbtdefs.h"

#define	FIXSHORT(x)		( (x) = ntohs(x) )
#define	FIXLONG(x)		( (x) = ntohl(x) )

void byteswap_nodestats(struct NODE_statistics *p)
{
	assert(p != 0);

	FIXSHORT(p->version_number);
	FIXSHORT(p->period_of_statistics);
	FIXSHORT(p->number_of_crcs);
	FIXSHORT(p->number_alignment_errors);
	FIXSHORT(p->number_of_collisions);
	FIXSHORT(p->number_send_aborts);
	FIXLONG (p->number_good_sends);
	FIXLONG (p->number_good_receives);
	FIXSHORT(p->number_retransmits);
	FIXSHORT(p->number_no_resource_conditions);
	FIXSHORT(p->number_free_command_blocks);
	FIXSHORT(p->total_number_command_blocks);
	FIXSHORT(p->max_total_number_command_blocks);
	FIXSHORT(p->number_pending_sessions);
	FIXSHORT(p->max_number_pending_sessions);
	FIXSHORT(p->max_total_sessions_possible);
	FIXSHORT(p->session_data_packet_size);
}
