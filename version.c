/* 
 * $Id: //devel/tools/main/nbtscan/version.c#5 $
 *
 *	This is a version indicator that we try to update when we
 *	make a change. Should help keep track of what version is
 *	what...
 *
 *	We also record the ChangeLog here...
 */

#include "nbtscan_common.h"
#include "nbtdefs.h"

const char Version[] =
"nbtscan 1.0.35 - 2008-04-08 - http://www.unixwiz.net/tools/";

/*
Tue Apr  8 16:24:16 PDT 2008 -- version 1.0.35
- added ZIP-file maker option (makefile only)

Tue Apr  8 16:06:13 PDT 2008 -- version 1.0.34
- for -P mode, now quote all the 'name' => value tags (for Ruby)

Fri Mar 16 12:21:41 PST 2007 -- version 1.0.33
- recompiled with MSVC6

Tue Aug 30 07:25:14 PST 2005 -- version 1.0.32
- disabled CIFS option; doesn't work yet

Thu Oct  2 10:30:49 PDT 2003 -- version 1.0.31
- replaced email address in version string with web URL

Thu Jun  6 17:05:36 PDT 2002 -- version 1.0.30
- DUH: CIFS is at 445/udp, not 443/udp. Thanks myNetwatchman!

Mon May 27 13:20:18 PDT 2002 -- version 1.0.29
- properly look up hostnames that contain a dash

Mon Mar 11 21:18:50 PST 2002 -- version 1.0.28
- now report Dr. Solomon AV Management NETBIOS names

Sun Mar 10 12:13:58 PST 2002 -- version 1.0.27
- now by default bind to random UDP port number

Sun Mar 10 10:01:25 PST 2002 -- version 1.0.26
- on summary line, write "?" if we don't recognize a NETBIOS resource
  type. Will give us a clue that we ought to investigate.

Fri Mar  8 13:21:35 PST 2002 -- version 1.0.25
- enabled "-P" for perl hashref generation

Thu Mar  7 12:34:28 PST 2002 -- version 1.0.24
- extended the "-v -v" raw dump format a bit

Sat Apr 21 09:37:38 PDT 2001 -- version 1.0.23
- added default values to the help message

Mon Feb 12 15:40:16 PST 2001 -- version 1.0.22
- recompiled with library that does no DNS lookups if /## is present

Fri Jan 12 18:25:21 PST 2001 -- version 1.0.21
- fixed problem with types > 0x7F

Fri Jan 12 16:47:03 PST 2001 -- version 1.0.20
- minor port fixes for SCO Open Server 5

Sun Oct 29 15:05:00 PDT 2000 -- version 1.0.18
- added -H for HTTP headers
- now put quotes around user names that have spaces in them

Tue Oct 24 13:01:22 PDT 2000 -- version 1.0.17
- recompiled after library fixes

Tue Oct 17 12:41:22 PDT 2000 -- version 1.0.16
- fixed assertion failure in the library

Fri Oct  6 12:46:28 PDT 2000 -- version 1.0.15
- now show user name in the short listing (U=)

Fri Oct  6 09:13:47 PDT 2000 -- version 1.0.14
- now attempt to recognize a domain controller in the listing (first try)
  fixed a small core-dump bug.

Thu Jun 22 21:32:44 PDT 2000 -- version 1.0.13
- figured out the no-names thing

Thu Jun 22 11:31:10 PDT 2000 -- version 1.0.12
- started fixing the parse_nbtscan() processing for the no-names error

Tue Jun 20 08:12:08 PDT 2000 -- version 1.0.11
- dumped all proxy crap, now use library "parse_target()"

Fri Jun  9 11:08:33 PDT 2000 -- version 1.0.10
- checking into new Perforce depot structure

Wed May 17 12:31:40 PDT 2000 -- version 1.0.9
- portability fixes

Thu Apr 20 20:45:43 PDT 2000 -- version 1.0.8
- added reasonable usage() message (finally!)
- lots of PC-Lint cleanups

Mon Apr 10 16:49:27 PDT 2000 -- version 1.0.7
- now only require Winsock 1.1 on NT platforms (no UNIX changes)

Mon Apr 10 14:42:01 PDT 2000 -- version 1.0.6
- added multiple-target support, started adding good retry support

Sun Apr  9 19:56:51 PDT 2000 -- version 1.0.5
- tried to implement broadcast mode (didn't work)

Sun Apr  9 08:09:51 PDT 2000 -- version 1.0.4
- minor porting fixes to Unixware
- now require sleep_msecs() function to have a real body!

Sun Apr  9 07:31:57 PDT 2000 -- version 1.0.3
- added DNS lookups for target specifications (finally!)
- show inverse DNS name along with ether address
- added '-V' for version information
- now proxy support must be specifically compiled

Sat Apr  8 09:27:03 PDT 2000
- ported to NT (easy!)

 */

