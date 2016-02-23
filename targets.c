/*
 * targets.c
 *
 *	This is the module that deals with our list(s) of targets. One
 *	"target" is associated with each command-line target range, and
 *	we maintain a linked list of them. They are traversed one or
 *	more times, and we will eventually add the ability to exclude
 *	those targets already seen.
 */
#include "nbtscan_common.h"
#include <stdio.h>
#include "nbtdefs.h"

static struct targetRange	Targets;
static struct targetRange	*currTarget = 0;
static int			currIndex   = 0;
static int			triesLeft   = 1;

/*
 * add_target()
 *
 *	Given a target specification, add it to the list of all those
 *	pending on this job.
 */
void add_targetRange(struct targetRange *targ)
{
struct targetRange	*t;

	assert(targ != 0);
	
	targ->next = 0;

	for (t = &Targets; t->next != 0; t = t->next )
	{
		/* NOTHING */
	}

	t->next = targ;

/*	printf("added target [%s]\n",	targ->printable);
	printf("  first: %s\n",		inet_ntoa(targ->firstaddr));
	printf("  last:  %s\n",		inet_ntoa(targ->lastaddr ));
	printf("  #host: %ld\n",	targ->nhosts);			*/

}

/*
 * set_tries()
 *
 *	Tell our system how many tries through the big loop we are to
 *	run. This is the number of TRIES, not the number of *RE*tries.
 */
void set_tries(int n)
{
	triesLeft = n;
}

/*
 * target_responded()
 *
 *	Given the IP address of the remote that has sent back a packet to us,
 *	return TRUE if we should display it or not. We only say "no" if we
 *	are sure we've seen this address before, otherwise we return TRUE
 *	so the caller will display it again.
 *
 *	We also note that the response has been received and will not
 *	send any more queries to this target on any subsequent retry.
 */
int target_responded(const struct in_addr *paddr)
{
struct targetRange *t;
unsigned long	   addr;

	assert(paddr != 0);

	addr = ntohl( paddr->s_addr );

/*	printf("Response from %s\n", inet_ntoa(*paddr)); */

	for (t = Targets.next; t != 0; t = t->next )
	{
	unsigned long	firstaddr = ntohl(t->firstaddr.s_addr),
			lastaddr  = ntohl(t->lastaddr.s_addr);

/*		printf("checking %s\n", t->printable);
		printf("  from %s ", inet_ntoa(t->firstaddr));
		printf("  to   %s\n", inet_ntoa(t->lastaddr));	*/

		if ( addr >= firstaddr   &&   addr <= lastaddr )
		{
		int	ix = (int)(addr - firstaddr);

			/* already seen this one? */

			if ( t->hostsDone[ix] ) return FALSE;

/*			printf("host %s (%d) done\n",inet_ntoa(*paddr),ix); */
			t->hostsDone[ix] = 1;
			t->hostsRemaining--;

			break;
		}
		else
		{
/*			printf("address not within range\n"); */
		}
	}

	return TRUE;
}

/*
 * next_target()
 *
 *	Give the caller a new target to process, return TRUE if we found
 *	one and FALSE if not. This takes into account "rewinding" once we
 *	hit the end of the list on multiple retries, and we fill in the
 *	IP address of the target to scan.
 *
 *	The address filled in is in network word order.
 *
 *	===NOTE: we will soon be skipping address for which we have
 *	already received a valid response:
 */
int next_target(struct in_addr *paddr)
{
int	have_address = FALSE;

	assert(paddr != 0);

	while ( ! have_address )
	{

		/* if just getting started or rewinding after the hitting
		 * the end of the first scan, set up for initial processing.
		 */
		if ( currTarget == 0 )
		{
			if ( triesLeft <= 0 )
				return FALSE;

			triesLeft--;

			currTarget = Targets.next;
			currIndex  = 0;
		}

		assert( currTarget != 0 );

		/*--------------------------------------------------------
		 * The address to scan is the base address of this target
		 * range plus the index into the range. We have to do lots
	 	 * of shuffling around of the word order first.
		 */
		if ( ! currTarget->hostsDone[ currIndex ] )
		{
			paddr->s_addr = ntohl(
				htonl(currTarget->firstaddr.s_addr) + currIndex
				);

			have_address = TRUE;
		}

		/*--------------------------------------------------------
		 * If we have processed all the hosts in this block, skip
		 * to the next one for the next loop.
		 */
		if ( ++currIndex >= currTarget->nhosts )
		{
			currTarget = currTarget->next;
			currIndex  = 0;
		}
	}

	return have_address;
}
