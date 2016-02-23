/*
 * $Id: //devel/tools/main/nbtscan/penlib.h#1 $
 *
 *	Common library definitions for our penetration tools.
 */

#ifndef _WIN32
#include <wchar.h>
#endif

/*lint -emacro(717, FD_SET) // do...while() */

#ifdef __cplusplus
extern "C" {
#endif

/*lint -emacro(605, assert)	// increase in ptr capabilityh		*/
/*lint -emacro(717, FD_SET)	// use of do {...} while		*/
/*lint -esym(534, WSACleanup)	// return value ignored			*/
/*lint -esym(534, stripA, stripW, vfprintf)	// ... ditto			*/

/*------------------------------------------------------------------------
 * The old DEC Alpha compiler takes prototypes but is not really ANSI C:
 * these are required to make the compiler happy. Yuck.
 */
#ifdef ultrix
#  undef const
#  define const 	/*nothing*/

#  undef volatile
#  define volatile	/*nothing*/
#endif

#ifndef TRUE
#  define TRUE  1
#  define FALSE 0
#endif

#ifdef _WIN32
#  define	SOCKET_IS_VALID(s)	((s) != INVALID_SOCKET)
#  define	INVALIDATE_SOCKET(s)	((s)  = INVALID_SOCKET)
#else
#  define	SOCKET			int
#  define	SOCKET_IS_VALID(s)	((s) >= 0)
#  define	INVALIDATE_SOCKET(s)	((s) = -1)

#  define	__stdcall		/*nothing*/
#  define	__cdecl			/*nothing*/
#endif


#ifdef _WIN32
#  define snprintf  _snprintf
#  define vsnprintf _vsnprintf
#endif

/*------------------------------------------------------------------------
 * We're a huge fan of GCC's __attribute__ feature, which allows us to
 * enable better compiler checking of our work, but we need to disable it
 * quietly if using another compiler.
 */
#ifndef __GNUC__
# define __attribute__(x)	/*nothing*/
#endif

/*------------------------------------------------------------------------
 * For making 
 */
#define UNUSED_PARAMETER(p)	(void)(p)	/* shut up compiler warnings */

#define	TBLSIZE(t)		(int) (sizeof(t) / sizeof((t)[0]))

/*------------------------------------------------------------------------
 * This is missing on Solaris 2.7 - ugh
 */
#if defined(INADDR_BROADCAST) && ! defined(INADDR_NONE)
#  define INADDR_NONE INADDR_BROADCAST
#endif

/*----------------------------------------------------------------------
 * die by exiting with error status after printing msg to stderr
 */
/*lint -sem(dieA, r_no) */
/*lint -sem(dieW, r_no) */
/*lint -printf(1, dieA) */
extern void	__cdecl dieA(const char *format, ...)
                                 __attribute__((format(printf,1,2)))
                                 __attribute__((noreturn));
extern void __cdecl dieW(const wchar_t *format, ...)
                                 __attribute__((noreturn));

#ifdef _UNICODE
#  define die   dieW
#  define strip stripW
#else
#  define die    dieA
#  define strip stripA
#endif

/*lint -sem(stripA, 1p) */
/*lint -sem(stripW, 1p) */
extern char	   * __stdcall stripA(char    *);
extern wchar_t * __stdcall stripW(wchar_t *);

/*------------------------------------------------------------------------
 * we're always doing these operations, but only on *internet* types.
 */

/*lint -emacro(740, connect_in, sendto_in, recvfrom_in, bind_in) */
/*             740 unusual pointer cast                          */

#define	connect_in(sfd, rmt)	\
	connect((sfd), (struct sockaddr *)(rmt), sizeof *(rmt))

#define	sendto_in(fd, buf, l, fl, rmt)	\
	sendto((fd), (buf), (l), (fl), (struct sockaddr *)(rmt), sizeof *(rmt))

#define	recvfrom_in(fd, buf, l, fl, rmt, psz)	\
	recvfrom((fd),(buf),(l),(fl),(struct sockaddr *)(rmt), (psz))

#define bind_in(sfd, rmt)	\
	bind((sfd), (struct sockaddr *)(rmt), sizeof *(rmt))

/* for getsockopt() */
#if ! defined(linux)
#  define socklen_t int
#endif


/*------------------------------------------------------------------------
 * netmasks.c -- various netmasks in *host* order
 */
extern const unsigned long	netmasks[32+1];

/*lint -sem(nstrcpyA, 1p, 2p) */
/*lint -sem(nstrcpyW, 1p, 2p) */
extern size_t __stdcall nstrcpyA(char *dst,    const char *src);
extern size_t __stdcall nstrcpyW(wchar_t *dst, const wchar_t *src);

#ifdef _UNICODE
#  define nstrcpy	nstrcpyW
#else
#  define nstrcpy	nstrcpyA
#endif


extern void	__stdcall sleep_msecs(long);

/*lint -sem(all_digitsA, 1p) */
/*lint -sem(all_digitsW, 1p) */
extern int __stdcall	all_digitsA(const char    *s, int *n);
extern int __stdcall	all_digitsW(const wchar_t *s, int *n);

#ifdef _UNICODE
#  define all_digits all_digitsW
#else
#  define all_digits all_digitsA
#endif


extern int __stdcall	netmask_bits(unsigned long mask)
				__attribute__((const));

/*------------------------------------------------------------------------
 * darray.c - dynamic arrays
 */

/*lint -emacro(1773, ARRSIZE) // attempt to cast away const (or volatile) */

#define	ARRSIZE(arr)        arrsize((const void ***)(arr))
#define	SETSIZE(arr, sz)    setsize((void ***)(arr), (sz))
#define	CLRARR(arr)         clrarr((void ***)(arr))
#define	FREEARR(arr)        freearr((void ***)(arr))
#define	STOREARR(arr, item) storearr((void ***)(arr), (item))
#define ARRXDEL(arr, ix)    arrxdel((void ***)(arr), (ix))

extern void** __stdcall mkarray(size_t);
extern size_t __stdcall arrsize  ( const void **const *const );
extern void   __stdcall setsize  (       void ***, size_t );
extern void   __stdcall clrarr   (       void *** );
extern void   __stdcall freearr  (       void *** );
extern void   __stdcall storearr (       void ***, const void * );
extern void   __stdcall arrxdel  (       void ***, size_t ix);

/*------------------------------------------------------------------------
 * eprintf.c - just like printf but to stderr
 */

/*lint -printf(1, eprintf) */
extern int __cdecl eprintfA(const char *, ...)
			__attribute__((format(printf, 1, 2)));

extern int __cdecl eprintfW(const wchar_t *, ...)
			/* __attribute__((format(wprintf, 1, 2)))*/ ;

#ifdef _UNICODE
#  define	eprintf	eprintfW
#else
#  define	eprintf	eprintfA
#endif

/*------------------------------------------------------------------------
 * circbuf.c
 *
 * Circular buffer management: allocate, save buffer, save string
 */
extern void * __stdcall	circbufA(size_t n);
extern void * __stdcall	circsaveA(const void *buf, size_t n);
extern char * __stdcall	circdupA(const char *s);

extern void    * __stdcall circbufW(size_t n);
extern void    * __stdcall circsaveW(const void *buf, size_t n);
extern wchar_t * __stdcall circdupW(const wchar_t *s);

#if defined(UNICODE) || defined(_UNICODE)
#  define circbuf   circbufW
#  define circsave  circsaveW
#  define circdup   circdupW
#else
#  define circbuf   circbufA
#  define circsave  circsaveA
#  define circdup   circdupA
#endif

/*------------------------------------------------------------------------
 * lookup_hostname.c
 *
 */
extern int __stdcall lookup_hostname(const char *, unsigned long *paddr);

/*-----------------------------------------------------------------------
 * winsock.c
 */
#ifdef _WIN32
extern void         __stdcall init_winsock(int verbose, int level);
extern const char * __stdcall printable_wserror( DWORD dwErr,
                                                 char *obuf, size_t osize);
#endif

/*-----------------------------------------------------------------------
 * netbios_unpack.c
 */
extern int __stdcall NETBIOS_unpack(const char **ibuf, char *obuf, int osize);

/*--------------------------------------------------------------------------
 * printable_NETBIOS_question_type.c
 * printable_NETBIOS_question_class.c
 *
 *	NETBIOS debugging routines.
 */
/*lint -sem(printable_NETBIOS_question_type,  1p) */
/*lint -sem(printable_NETBIOS_question_class, 1p) */
extern const char * __stdcall printable_NETBIOS_question_type (char *obuf, int qtype);
extern const char * __stdcall printable_NETBIOS_question_class(char *obuf, int qclass);


/*------------------------------------------------------------------------
 * set_nonblocking.c
 */
#if defined(AF_INET)	/* proxy for defined(SOCKET) */
extern int	set_nonblocking(SOCKET sfd, int blocking_flag);
#endif

/*------------------------------------------------------------------------
 * netbios_pack.c
 */
extern int __stdcall NETBIOS_raw_pack_name(const char *ibuf, int isize, char *obuf);
extern int __stdcall NETBIOS_pack_name    (const char *ibuf, int itype, char *obuf);

/* required by SCO UNIX */
#ifdef M_XENIX
extern int                      optind;
extern char                     *optarg;
#endif

extern long	get_etime_msecs(void);

/* icmptype.c */
/*lint -sem(printable_ICMP_type, 1p) */
extern const char * __stdcall printable_ICMP_type(char *obuf, int osize,
					int tytpe, int code);

/* in_cksum.c */
/*lint -sem(calculate_cksum, 1p) */
extern unsigned short __stdcall calculate_cksum(const unsigned short *addr, int len);

/* parse_inaddr.c */
/*lint -sem(parse_inaddr, 1p, 2p) */
extern int parse_inaddr(const char *str, unsigned long *addr);

/*lint -sem(parse_target, 1p, 2p, 3p) */
extern int __stdcall parse_target(const char *str, char *errbuf,
		int (__stdcall *pfunc)( unsigned long addr1,
					unsigned long addr2,
					const char *hostname,
					void *hint ), void *hint);

#define build_ipaddr_from_octets(b1,b2,b3,b4)	\
	( ((unsigned long)b1 << 24)	\
	| ((unsigned long)b2 << 16)	\
	| ((unsigned long)b3 <<  8)	\
	| ((unsigned long)b4      ) )
			

/*------------------------------------------------------------------------
 *
 *	Normally UNIX systems maintain the time in one-second intervals via
 *	the time() syscall, but select() and gettimeofday() can also show
 *	the time in microsecond intervals. This kind of high resolution is
 *	vital for managing timeouts.
 *
 *	We use "struct timeval", and this is typically defined as :
 *
 *		struct timeval {
 *			long	tv_sec;
 *			long	tv_usec;	|* 0 .. 999999 *|
 *		};
 *
 *	Sometimes long is a time_t, but it's ultimately usually a long.
 *
 *	We maintain the concept of "normalized" timevals, where the sign
 *	of the two fields are the same and microseconds is always less than
 *	one second's worth.
 */

#ifndef _WIN32
# include <sys/time.h>
#endif

/*lint -sem(timeval_zero, 1p) */
/*lint -sem(timeval_neg, 1p) */
/*lint -sem(timeval_set, 1p) */
/*lint -sem(timeval_normalize, 1p) */
/*lint -sem(timeval_add, 1p, 2p, 3p) */
/*lint -sem(timeval_sub, 1p, 2p, 3p) */
/*lint -sem(timeval_compare, 1p, 2p) */
/*lint -sem(timeval_add_secs, 1p) */
/*lint -sem(timeval_add_msecs, 1p) */
/*lint -sem(timeval_add_usecs, 1p) */
extern struct timeval * __stdcall timeval_zero(struct timeval *);
extern struct timeval * __stdcall timeval_normalize(struct timeval *);
extern struct timeval * __stdcall timeval_add(struct timeval *, const struct timeval *, const struct timeval *);
extern struct timeval * __stdcall timeval_sub(struct timeval *, const struct timeval *, const struct timeval *);
extern struct timeval * __stdcall timeval_neg(struct timeval *);
extern struct timeval * __stdcall timeval_set(struct timeval *, long secs, long usecs);
extern struct timeval * __stdcall timeval_add_secs(struct timeval *tv, int secs);
extern struct timeval * __stdcall timeval_add_msecs(struct timeval *tv, long milliseconds);
extern struct timeval * __stdcall timeval_add_usecs(struct timeval *tv, long microseconds);
extern struct timeval * __stdcall timeval_set_secs(struct timeval *tv, int secs);
extern struct timeval * __stdcall timeval_set_msecs(struct timeval *tv, long msecs);
extern struct timeval * __stdcall timeval_set_usecs(struct timeval *tv, long usecs);
extern int              __stdcall timeval_compare(const struct timeval *, const struct timeval *);


/*--------------------------------------------------------------------------
 * odprintf.c
 */
#ifdef _WIN32
/*lint -printf(1, odprintf) */
extern void __cdecl   odprintf(const char *format, ... );
extern void __stdcall odprintf_output(int ok);
#endif

/*--------------------------------------------------------------------------
 *
 *	This is a module that manages linked lists. We really hate to do
 *	yet another linked-list library, but we have too many places that
 *	need to do this kind of thing. Mainly it's used in the I/O code
 *	to deal with waiting-for-work and waiting-for-response lists, but
 *	surely we'll find more uses than this.
 *
 *	This linked-list library assumes the all nodes have a "next" pointer
 *	as their first element (we wish we could use C++ inheritance for 
 *	this -- sigh).  Internally we use the "linknode" type for this,
 *	which is a dummy, though the user never sees it.
 *
 *	Note that each linked-list object contains a full *node* object
 *	inside, not just a pointer.  This looks a bit more complex but
 *	ends up making the code much easier and makes the list head and
 *	list tail less of special cases. Trust us on this.
 */

struct linknode {
	struct linknode	*next;
};

struct linklist {
	struct linknode	head;
	struct linknode	tail;
	int		count;	/* how many nodes on the list?		*/
	char		*name;	/* name of this list for debugging	*/
};

typedef struct linklist	LINKLIST;

/*lint -sem(llist_is_empty, 1p) */
/*lint -sem(llist_unlink_head, 1p) */
/*lint -sem(llist_add_tail, 1p, 2p) */
/*lint -sem(llist_check_consistent, 1p) */
/*lint -sem(llist_init, 1p, 2p) */
/*lint -sem(llist_count, 1p) */
extern int	 llist_is_empty(const LINKLIST *llist);
extern void	*llist_unlink_head(LINKLIST *llist);
extern void	 llist_add_tail(LINKLIST *llist, void *item);
extern void	 llist_check_consistent(const LINKLIST *llist);
extern void	*llist_search(LINKLIST *llist, void *data, 
			int (*cmp)(void *, void *) );
extern void	*llist_search_unlink(LINKLIST *llist, void *data,
			int (*cmp)(void *, void *) );

extern struct linknode *llist_visit(LINKLIST *llist, void *data,
			int (*visitor)(void *, void *));

extern void	llist_init(LINKLIST *list, const char *name);
extern int	llist_count(const LINKLIST *list);

/* show_startup.c */
#ifdef EOF
extern void __stdcall show_startup(FILE *ofp, int argc, char **argv);
#endif

/*------------------------------------------------------------------------
 * strupper.c & strlower.c
 */
extern size_t __stdcall strupperA(char    *s);
extern size_t __stdcall strupperW(wchar_t *s);

extern size_t __stdcall strlowerA(char    *s);
extern size_t __stdcall strlowerW(wchar_t *s);

#ifdef _UNICODE
#  define strupper	strupperW
#  define strlower	strlowerW
#else
#  define strupper	strupperA
#  define strlower	strlowerA
#endif


/*------------------------------------------------------------------------
 * ntreg_value_type.c
 */
#ifdef _WIN32
extern const char    * __stdcall ntreg_value_typeA(DWORD dw);
extern const wchar_t * __stdcall ntreg_value_typeW(DWORD dw);

#  ifdef _UNICODE
#    define ntreg_value_type	ntreg_value_typeW
#  else
#    define ntreg_value_type	ntreg_value_typeA
#  endif

#endif

/*------------------------------------------------------------------------
 * perlquoteA.c / perlquoteW.c
 * htmlquoteA.c / htmlquoteW.c
 */
extern char    * __stdcall perlquoteA(char    *obuf, const char    *ibuf);
extern wchar_t * __stdcall perlquoteW(wchar_t *obuf, const wchar_t *ibuf);

extern char    * __stdcall perlquoteMB(char  *obuf, const wchar_t *ibuf, int);

extern char    * __stdcall htmlquoteA(char    *obuf, const char    *ibuf);
extern wchar_t * __stdcall htmlquoteW(wchar_t *obuf, const wchar_t *ibuf);

#ifdef _UNICODE
#  define perlquote    perlquoteW
#  define htmlquote    htmlquoteW
#else
#  define perlquote    perlquoteA
#  define htmlquote    htmlquoteA
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif


/*-------------------------------------------------------------------
 * C++ tools
*/
#ifdef __cplusplus

/*---------------------------------------------------------------------
 * string conversions. Sometimes we need to go from wide->narrow and
 * vice versa, and this seems like a pretty dumb way to do it considering
 * there is surely a set of libraries for it
 *
 * NOTE: the "size" parameters are in bytes, not characters (for convenience)
 */
inline const char    *copystring(char *, size_t, const char *ibuf)
{
        return ibuf;
}

inline const wchar_t *copystring(wchar_t *, size_t, const wchar_t *ibuf)
{
        return ibuf;
}

extern const char    *__stdcall copystring( char    *obuf,
                                            size_t osize,
                                            const wchar_t *ibuf );

extern const wchar_t *__stdcall copystring( wchar_t *obuf,
                                            size_t osize,
                                            const char    *ibuf );

extern char    * __stdcall strdup_newA(const char    *s);
extern wchar_t * __stdcall strdup_newW(const wchar_t *s);


/*lint -printf(1, tprintf) */
extern void __cdecl tprintfW(const char *format, ...);

#define tprintfA printf

#ifdef _UNICODE
#  define tprintf               tprintfW
#  define strdup_new            strdup_newW
#else
#  define tprintf               tprintfA
#  define strdup_new            strdup_newA
#endif


#endif /* __cplusplus */
