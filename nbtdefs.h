/*
 * $Id: //devel/tools/main/nbtscan/nbtdefs.h#1 $
 *
 * written by :	Stephen J. Friedl
 *		Software Consultant
 *		steve@unixwiz.net
 *
 *	Common definitions for the nbtscan program. We put all of
 *	our prototypes and #defines in here, and we require that ALL
 *	external declarations collide with their definitions so we
 *	can find errors quickly.
 */

/*------------------------------------------------------------------------
 * Stuff for PC-Lint to keep it generally quiet
 */
/*lint -esym(534, NETBIOS_fixname)	// return value ignored		*/

#ifndef NBTDEFS_H
# define NBTDEFS_H

# include <sys/types.h>
# ifdef unix
#   include <unistd.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#
# else
#   define WIN32_LEAN_AND_MEAN
# endif

# include "penlib.h"

/*------------------------------------------------------------------------
 * The user can specify a target as a single host, a class C network, or
 * a range of nodes in the last part of the IP address. These objects
 * support the parsing of targets.
 *
 * The struct in_addr values are always in *network* word order.
 */
struct targetRange {
	char		*printable;	/* printable name		*/
	struct in_addr	firstaddr;	/* first address to scan	*/
	struct in_addr	lastaddr;	/* last address to scan		*/
	long		nhosts;		/* # of total hosts		*/

	char		*hostsDone;
	int		 hostsRemaining;

	struct targetRange *next;	/* next target in line		*/
};

extern int parse_target_range(const char *str, char *errbuf);

/*------------------------------------------------------------------------
 * When we get a NBTSTAT response, the tail end contains a big block of
 * statistics. These are all in network word order, and we shuffle them
 * around before storing them in the user space.
 *
 * The definitions of the fields are taken from RFC1002.
 *
 * ===NOTE: the size of this struct must be 46 bytes and does NOT
 *	include the C padding that is normally expected. Be careful!
 */

#define NODE_STATS_SIZE	46

/*768 = struct member not referenced */
/*lint -esym(768, NODE_statistics::jumpers, NODE_statistics::test_result) */

struct NODE_statistics {
	unsigned char		uniqueid[6];	/* Ethernet address */

	unsigned char		jumpers;
	unsigned char		test_result;

	unsigned short		version_number;
	unsigned short		period_of_statistics;
	unsigned short		number_of_crcs;
	unsigned short		number_alignment_errors;
	unsigned short		number_of_collisions;
	unsigned short		number_send_aborts;
	unsigned long		number_good_sends;
	unsigned long		number_good_receives;
	unsigned short		number_retransmits;
	unsigned short		number_no_resource_conditions;
	unsigned short		number_free_command_blocks;
	unsigned short		total_number_command_blocks;
	unsigned short		max_total_number_command_blocks;
	unsigned short		number_pending_sessions;
	unsigned short		max_number_pending_sessions;
	unsigned short		max_total_sessions_possible;
	unsigned short		session_data_packet_size;
};

extern void byteswap_nodestats(struct NODE_statistics *p);

/*------------------------------------------------------------------------
 * When talking to the other end, we maintain this information about the
 * NETBIOS information.
 */
struct NMB_query_response {

	struct sockaddr_in	remote;			/* IP address 	*/

	char			domain   [15+1];	/* printable	*/
	char			computer [15+1];	/* printable	*/
	char			ether    [20];		/* printable	*/
	char			user     [32];          /* printable    */

	int			sharing;		/* sharing on?	*/
	int			has_IIS;		/* MS IIS?	*/
	int			has_Exchange;		/* MS Exchange	*/
	int			has_Notes;		/* Lotus notes	*/
	int			has_RAS;		/* Rmt Access	*/
	int			is_dc;                  /* domain ctlr? */

	int			has_unknown;            /* any unknown? */

	struct NODE_statistics	nodestats;		/* full info	*/

	/*----------------------------------------------------------------
	 * This is information about all the nodes that we can gather
	 * from the other end. These are taken directly from the NODE_NAME
	 * array, but >these< ones are formatted for easy printing.
	 */
	struct nodeinfo {
		char		name[15+1];	/* NUL-terminated!	*/
		char		type;		/* type code		*/
		unsigned short	flags;		/* host byte order	*/
		const char     *svcname;        /* long name            */
	}			nodes[100];

	int			nnodes;
	int			nametrunc;
};

extern void process_response(struct NMB_query_response *rsp);
#ifdef EOF
extern void display_nbtstat(FILE *ofp, const struct NMB_query_response *rsp,
				int full);
#endif

/*------------------------------------------------------------------------
 * The overall packets sent and received from the other end are all of
 * the form like this. Unfortunately the "data" part of the packet is
 * variable and that takes the most work to get right. The header is
 * more or less fixed though...
 */
struct NMBpacket {
	/* HEADER */

	unsigned short	tranid;			/* transaction ID */
	unsigned short	flags;			/* various flags */
	unsigned short	qdcount;		/* query count */
	unsigned short	ancount;		/* answer count */
	unsigned short	nscount;
	unsigned short	arcount;

	char		data[1024];
};

extern int parse_nbtstat(const struct NMBpacket *pak, int paklen, struct NMB_query_response *, char *errbuf);
#ifdef EOF
extern void dump_nbtpacket(const struct NMBpacket *pak, int paklen, FILE *ofp);
#endif

/*------------------------------------------------------------------------
 * When processing a NETBIOS node status response, we receive an array of
 * name structures of this form. The name is up to 15 chars, and is sadly
 * not NUL-byte terminated -- sorry.
 *
 * NOTE: the size of the record is exactly the size of the struct members,
 * and does NOT include any padding that C provides for us automatically.
 * It is important to use the NODE_RECORD_SIZE macro when stepping through
 * the array.
 */
struct node_name_record {
	char		name[15];
	char		type;
	unsigned short	flags;		/* in host byte order */
};

#define		NODE_FLAGS_GROUP(np)	 ((np)->flags & 0x8000)
/* #define	NODE_FLAGS_OWNTYPE(np)	(((np)->flags & 0x6000) >> 13)	*/
/* #define	NODE_FLAGS_DEREG(np)	 ((np)->flags & 0x1000)		*/
/* #define	NODE_FLAGS_CONFLICT(np)	 ((np)->flags & 0x0800)		*/
/* #define	NODE_FLAGS_ACTIVE(np)	 ((np)->flags & 0x0400)		*/
/* #define	NODE_FLAGS_PERM(np)	 ((np)->flags & 0x0200)		*/

#define		NODE_RECORD_SIZE	18

extern const char *NETBIOS_name(const struct nodeinfo *np);

/*lint -sem(NETBIOS_fixname, 1p) */
extern char *NETBIOS_fixname(char *buf);

extern int			show_mac_address;

/* packetio.c */
extern int	sendpacket_direct( SOCKET ofd, const void *buf,
				   int len, const struct sockaddr_in * );

extern int	recvpacket_direct( SOCKET ifd, void *buf,
				   int len, struct sockaddr_in * );

extern int			verbose;
extern int			no_inverse_lookup;

/*------------------------------------------------------------------------
 * errors.c
 *
 * These translate errors to printable messages. The name in front tells
 * us essentially whether we should be using "errno" or "GetLastError()".
 * Since we so commonly use just these args, we use UNIX_ERROR and WIN32_ERROR
 * to do them, plus NATIVE_ERROR which is platform-specific.
 *
 * NOTE: we really need to make a serious pass through this program on the
 * Win32 platform to find out which error numbers we get from the various
 * networking calls. We suspect that Winsock errors need to be recognized
 * but we're not sure which calls return which error numbers.
 */
extern const char		*unix_errorstr(int error_no);
#if _WIN32
extern const char		*win32_errorstr(DWORD error_no);
#endif

#if _WIN32
#define WIN32_ERROR		( win32_errorstr(GetLastError()) )
#endif
#define	UNIX_ERROR		( unix_errorstr(errno)           )

#if _WIN32
#  define NATIVE_ERROR		WIN32_ERROR
#else
#  define NATIVE_ERROR		UNIX_ERROR
#endif


/* hostname.c */
extern unsigned long name_to_ip(const char *hostname);
extern int  	     ip_to_name(unsigned long ip, char *obuf, int osize);
extern int           might_be_hostname(const char *hostname);

/* targets.c */
extern void	add_targetRange(struct targetRange *targ);
extern int	next_target(struct in_addr *);
extern void	set_tries(int n);
extern int	target_responded(const struct in_addr *);

extern const char		Version[];


/* rawdump.c */
extern void     rawdump(const char *buf, int n);

#ifdef ENABLE_PERL
extern int      gen_Perl;
#ifdef EOF
extern void     start_perl(FILE *ofp, char **argv);
extern void     generate_perl(FILE *ofp, const struct NMB_query_response *rsp);
extern void     end_perl(FILE *ofp);
#endif
#endif

#endif /* DEFS_H */

