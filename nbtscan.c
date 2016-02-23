/*
 * $Id: //devel/tools/main/nbtscan/nbtscan.c#3 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	This is a UNIX version of NBTSTAT that queries one or more
 *	remote systems. It's sort of like "dig" for DNS but or
 *	NETBIOS instead
 *
 * COMMAND-LINE PARAMS
 * -------------------
 *
 *	-O outfile	specify output file (instead of stdout)
 *
 *	-p portno	bind to the given UDP port number. This is used
 *			as the source port for all queries, and the default
 *			is the standard 137. However, if you have a packet-
 *			filtering firewall that doesn't permit inbound
 *			traffic to port 137, set this to something else that
 *			you do permit: then the responses will return to
 *			the (secret) high-numbered port and pass through.
 *
 *	-v		verbose: turn on more debugging info.
 *
 *	-T secs		timeout when no activity is seen for the given
 *			number of seconds.
 *
 *	-t ##		# of tries for each address (default=1)
 *
 *	-b		enable broadcast. *DOESN'T WORK SO WELL*
 *
 *	-w msecs	To keep from flooding a link with queries, we can
 *			wait for the given number of milliseconds after
 *			every write to allow us to pace things a bit.
 *			Default = 10 msecs, should be turned lower if
 *			all is on the local Ethernet.
 *
 *	-f		show full NBTSTAT information, else summary only
 *
 *	-m		include MAC address (from NETBIOS packet) in listing
 *
 *	-n		don't look up inverse DNS names in full listing
 *
 *	-V		show version info
 *
 *	-H		output with (brief) HTTP headers for use in a CGI
 *
 *	-1		Use Winsock 1 only (Windows only)
 *
 *	-P		Generate Perl hashref output
 *
 *	-C		Make a CIFS query (**NOT SUPPORTED YET**)
 */
#include "nbtscan_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef unix
#  include <errno.h>
#  include <sys/select.h>
#  include <sys/time.h>
#else
#  include "getopt.i"
#endif
#include "nbtdefs.h"

#undef SUPPORT_CIFS
#undef SUPPORT_BROADCAST

static void	query_names(FILE *, SOCKET sockfd);
static void	fill_namerequest(struct NMBpacket *, int *len, short eq);

static unsigned short	bind_portno = 0;
static int 		broadcast   = 0,
			timeout_secs = 2,
			write_sleep_msecs = 10;
	
int		verbose = 0;
int		no_inverse_lookup = FALSE;
int		gen_HTTP = FALSE;
int		gen_Perl = FALSE;

static int	full_nbtstat = FALSE;
int		show_mac_address = FALSE;

static int sendpacket(int sfd, const void *pak, int len,
			const struct sockaddr_in *dst);
static int recvpacket(int sfd, void *pak, int len, struct sockaddr_in *dst);

/*lint -sem(Usage, r_no) */
static void Usage(const char *argv0);

static short     dest_portno = 137;

/*------------------------------------------------------------------------
 * Set the Usage information for the user...
 */
int main(int argc, char **argv)
{
FILE			*ofp = 0;
const char		*outfile = 0;
SOCKET			sockfd;
int			c;
char			errbuf[256];
struct sockaddr_in	myaddr;
int			have_targets = FALSE;
#ifdef _WIN32
int			winsock_level = 2;
#endif

	/* quick hack */
	if ( argv[1]  &&  strcmp(argv[1], "--version") == 0 )
	{
		puts(Version);
		exit(EXIT_SUCCESS);
	}

	while ( (c = getopt(argc, argv, "CHT:nfp:bt:vw:mVO:1P")) != EOF )
	{
		switch (c)
		{
		  default:	Usage(argv[0]);

		  case 'P':
#ifdef ENABLE_PERL
				gen_Perl = TRUE;
#else
				die("Sorry, -P perl support not compiled");
#endif
				break;

		  case 'C':
#ifdef SUPPORT_CIFS
				dest_portno = 445;
				break;
#else
				die("Sorry, -C not supported yet");
#endif

		  case '1':
#ifdef _WIN32
				winsock_level = 1;
#endif
				break;

		  case 'H':	gen_HTTP = TRUE;
				break;
		
		  case 'O':	outfile = optarg;
				break;

		  case 't':	set_tries( atoi(optarg) );
				break;

		  case 'V':
				puts(Version);
				exit(EXIT_SUCCESS);

		  case 'm':	show_mac_address = TRUE;
				break;

		  case 'f':	full_nbtstat = TRUE;
				break;

	 	  case 'p':	bind_portno = (unsigned short)atoi(optarg);
				break;

		  case 'b':
#if SUPPORT_BROADCAST
				broadcast = TRUE;
				break;
#else
				die("Sorry, -b broadcast not yet supported");
#endif

		  case 'v':	verbose++;
				break;

		  case 'T':	timeout_secs = atoi(optarg);
				break;

		  case 'w':	write_sleep_msecs = atoi(optarg);
				break;

		  case 'n':	no_inverse_lookup = TRUE;
				break;
		}
	}

	if ( outfile != 0 )
		ofp = fopen(outfile, "w");
	if ( ofp == 0 )
		ofp = stdout;

	if ( gen_HTTP )
		printf("Content-type: text/plain\n\n");

#if _WIN32
	init_winsock(verbose, winsock_level);
#endif

	/*----------------------------------------------------------------
	 * Run through the rest of the command line parsing the various
	 * target specifications. Each one specifies one or more targets
	 * that we're to scan, and each one gets linked into the end of
	 * the current chain.
	 */
	while ( optind < argc )
	{
	const char		*arg = argv[optind++];

		if ( ! parse_target_range(arg, errbuf) )
			die("ERROR: invalid target specification: %s", errbuf);

		have_targets++;
	}

	if ( ! have_targets )
		Usage(argv[0]);

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);

	if ( ! SOCKET_IS_VALID(sockfd) )
	{
		die("ERROR: cannot create socket [%s]", NATIVE_ERROR);
	}

	/*----------------------------------------------------------------
	 * Some systems require that we specifically enable broadcast on
	 * the local network: do so if requested. It doesn't seem to work
	 * very well.
	 */
#if ENABLE_BROADCAST
	if ( broadcast )
	{
	int	b = 1;
	int	rc;

		rc = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
				(char *)&b,
				sizeof b);

		if ( rc != 0 )
		{
			die("ERROR: can't set SO_BROADCAST [%s]", NATIVE_ERROR);
		}
	}
#endif

	/*----------------------------------------------------------------
	 * Bind the local endpoint to receive our responses. If we use a
	 * zero, the system will pick one for us, or we can pick our own
	 * if we wish to make it easier to get past our firewall.
	 */
	memset(&myaddr, 0, sizeof myaddr);

	myaddr.sin_family      = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port        = htons(bind_portno);

	if ( bind_in(sockfd, &myaddr) != 0 )
#ifdef _WIN32
		die("ERROR: cannot bind to local socket [%ld]",
			WSAGetLastError());
#else
		die("ERROR: cannot bind to local socket [%s]", strerror(errno));
#endif

	if ( verbose )
	{
		fprintf(ofp, "Bound to %s.%d\n", inet_ntoa( myaddr.sin_addr),
						ntohs( myaddr.sin_port));
	}

#ifdef ENABLE_PERL
	if ( gen_Perl ) start_perl(ofp, argv);
#endif

	query_names(ofp, sockfd);

#ifdef ENABLE_PERL
	if ( gen_Perl ) end_perl(ofp);
#endif

#if _WIN32
	WSACleanup();
#endif

	return 0;
}

static void query_names(FILE *ofp, SOCKET sockfd)
{
short            seq = 1000;
int              npending = 0;
struct in_addr   next_addr;
int              have_next_addr = FALSE;
char             errbuf[256];

	assert( ofp != 0 );
	assert( SOCKET_IS_VALID(sockfd) );

	/*----------------------------------------------------------------
	 * Figure out our starting and ending addresses to be scanning.
	 * These are treated as simple long integers that are incremented
	 * on each loop, and we must have at least one loop to be valid.
	 */

	while ( have_next_addr
	    || ((have_next_addr = next_target(&next_addr)) != 0)
	    || (npending > 0) )
	{
	fd_set		 rfds,		/* list of read descriptors	*/
			 wfds,		/* list of write descriptors	*/
			 *pwfds = 0;
	int		n;
	struct timeval	tv;

		/*--------------------------------------------------------
		 * Our select is just a bit tricky. We always are waiting
		 * on the read channel, but we only want to wait on the
		 * write channel if there are any more addresses in our
		 * list to process. After we've sent all the packets to
		 * the other end, we stop writing and do only reading.
		 */
		FD_ZERO(&rfds);
		FD_SET(sockfd, &rfds);

		timeval_set_secs(&tv, timeout_secs);

		if ( have_next_addr )
		{
			wfds  = rfds;
			pwfds = &wfds;
		}

		if ( (n = select(sockfd+1, &rfds, pwfds, 0, &tv)) == 0 )
		{
			fprintf(stderr, "*timeout (normal end of scan)\n");
			fflush(ofp);
			break;
		}
		else if ( n < 0)
		{
			printf("ERROR [%s]\n", strerror(errno));
			break;
		}

		/*--------------------------------------------------------
		 * Has the read descriptor fired?
		 */
		if ( n > 0  &&  FD_ISSET(sockfd, &rfds) )
		{
		int			paklen;
		struct sockaddr_in	src;
		struct NMBpacket	pak;
		struct NMB_query_response rsp;

			memset(&src, 0, sizeof src);
			memset(&rsp, 0, sizeof rsp);

			paklen = (int)recvpacket(sockfd, &pak, sizeof pak,&src);

			if ( verbose )
			{
				if ( paklen < 0 )
				{
					fprintf(ofp, "Error on read: %s\n",
						strerror(errno));
				}
				else
				{
					fprintf(ofp, "Got %d bytes from %s\n",
						paklen,
						inet_ntoa(src.sin_addr) );

					if ( verbose > 1 )
						dump_nbtpacket(&pak, paklen, stdout);
				}
			}

			/*------------------------------------------------
			 * If we actually got something from the other end,
			 * parse the response, plug in the remote's IP addr,
			 * and display it.
			 */
			if ( paklen <= 0 ) continue;

			npending--;

			if ( parse_nbtstat(&pak, paklen, &rsp, errbuf) )
			{
				rsp.remote = src;

				if ( target_responded(&rsp.remote.sin_addr) )
				{
#ifdef ENABLE_PERL
					if ( gen_Perl )
					    generate_perl(ofp, &rsp);
					else
#endif
					    display_nbtstat(ofp,&rsp,full_nbtstat);
				}
			}
			else
			{
				fprintf(ofp, "ERROR: no parse for %s -- %s\n",
					inet_ntoa(src.sin_addr),
					errbuf);
			}
		}

		/*--------------------------------------------------------
		 * If we have room to write one packet, do so here. Note
		 * that we make not notice whether the write succeeds or
		 * not: we don't care.
		 */
		if ( n > 0  &&  pwfds  && FD_ISSET(sockfd, pwfds) )
		{
		struct sockaddr_in	dst;
		struct NMBpacket	pak;
		int			sendlen;

			memset(&dst, 0, sizeof dst);

			dst.sin_family      = AF_INET;
			dst.sin_addr.s_addr = next_addr.s_addr;
			dst.sin_port        = htons(dest_portno);

			have_next_addr = FALSE;

			fill_namerequest(&pak, &sendlen, seq++);

			if ( verbose )
			{
				fprintf(ofp, "sending to %s\n",
					inet_ntoa(dst.sin_addr));
			}

			/* yes, ignore response! */
			(void) sendpacket(sockfd, &pak, sendlen, &dst);

			if ( write_sleep_msecs > 0 )
				sleep_msecs(write_sleep_msecs);

			npending++;

			continue;
		}
	}

}

static int sendpacket(int sfd, const void *pak,
			int len, const struct sockaddr_in *dst)
{
	return sendpacket_direct(sfd, pak, len, dst);
}

static int recvpacket(int sfd, void *pak, int len, struct sockaddr_in *dst)
{
	return recvpacket_direct(sfd, pak, len, dst);
}

/*
 * fill_namerequest()
 *
 *	HACK: this creates a hand-crafter NMB packet that requests
 *	the NBTSTAT information. This was learned by sniffing a
 *	real transactions, and though we've learned what most of this
 *	means, we've not yet gone back to generalize it properly.
 *	We probably will.
 */
static void fill_namerequest(struct NMBpacket *pak, int *len, short seq)
{
char	*pbuf;

	assert(pak != 0);
	assert(len != 0);

	*len = 50;

	memset(pak, 0, *len);

	/* POPULATE THE HEADER */

	pak->tranid  = htons(seq);	/* transaction ID */
	pak->flags   = 0;
	pak->qdcount = htons(1);	/* query count */
	pak->ancount = 0;
	pak->nscount = 0;
	pak->arcount = 0;

	if ( broadcast )
		pak->flags |= htons(0x0010);	/* broadcast */

	/*----------------------------------------------------------------
	 * Encode the NETBIOS name, which is really just a "*" that's
	 * fully padded out. Then add the status and name class at the
	 * end.
	 */
	pbuf = pak->data;

	pbuf += NETBIOS_pack_name("*", 0, pbuf);
	*pbuf++ = 0x00;	/* length of next segment */

	*pbuf++ = 0x00;	/* NODE STATUS */
	*pbuf++ = 0x21;

	*pbuf++ = 0x00;	/* IN */
	*pbuf++ = 0x01;
}

/*
 * Usage()
 *
 *	Show the user how to run this program. Originally we had a large
 *	array of static text strings, but this didn't allow us to show
 *	the default values. So now we decide each string at runtime and
 *	include the defaults for values that we know about.
 *
 *	The "epr()" function is like printf(), but it sends its info to
 *	the standard error stream instead of standard output, and it adds
 *	a newline automatically.
 */
static void epr(const char *format, ...)
{
va_list	args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	putc('\n', stderr);
}

static void Usage(const char *argv0)
{
	epr("%s\n", Version);

	epr("usage: %s [options] target [targets...]", argv0);
	epr("");
	epr("   Targets are lists of IP addresses, DNS names, or address");
	epr("   ranges. Ranges can be in /nbits notation (\"192.168.12.0/24\")");
	epr("   or with a range in the last octet (\"192.168.12.64-97\")");
	epr("");
	epr("   -V        show Version information");
	epr("   -f        show Full NBT resource record responses (recommended)");
	epr("   -H        generate HTTP headers");
	epr("   -v        turn on more Verbose debugging");
	epr("   -n        No looking up inverse names of IP addresses responding");
	epr("   -p <n>    bind to UDP Port <n> (default=%d)", bind_portno);
#if SUPPORT_BROADCAST
	epr("   -b        enable local Broadcasts (not yet supported)");
#endif
	epr("   -m        include MAC address in response (implied by '-f')");
#if SUPPORT_CIFS
	epr("   -C        scan via CIFS port 445/ucp (default = 135/udp)");
#endif
	epr("   -T <n>    Timeout the no-responses in <n> seconds (default=%d secs)",
				timeout_secs);
	epr("   -w <n>    Wait <n> msecs after each write (default=%d ms)",
				write_sleep_msecs);
	epr("   -t <n>    Try each address <n> tries (default=1)");
#ifdef _WIN32
	epr("   -1        Use Winsock 1 only");
#endif
#ifdef ENABLE_PERL
	epr("   -P        generate results in perl hashref format");
#endif

	exit(EXIT_FAILURE);
}
