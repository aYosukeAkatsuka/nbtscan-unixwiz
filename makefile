#
# $Id: //devel/tools/main/nbtscan/makefile#5 $
#
#	This is the makefile for the NBTSCAN program, which compiles and
#	runs under both NT and many flavors of UNIX. This requires GNU
#	make (especially under NT, sorry). If you are building under NT
#	and *don't* have GNU make, use:
#
#		nmake /f makefile.nt all
#

#-------------------------------------------------------------------
# Operating system detection
#
# See if we're running NT or UNIX so the rest of the Makefile is
# keyed to actual operating system type and not to funky
# environment variables.
#
ifdef PROCESSOR_ARCHITECTURE
  OS=NT
else
  OS=unix
endif

#-------------------------------------------------------------------
# NT is lots different than UNIX, so we take care of the CFLAGS and
# the like right here. These are the flags of interest:
#
#	/nologo		suppress that damn copyright message
#
#	/MD		link with the shared MSVCRT.DLL library. This
#			reduces the footprint of the .EXE. All NT
#			systems have this DLL installed in the WINNT
#			directories.
#
#	/GF		enable readonly string pooling. Safer!
#
#	/G5		optimize for Pentium
#
#	/W3		enable lots of warnings
#

ifeq ($(OS),NT)

  O	 = obj
  L	 = lib
  E	 = .exe
  CC	 = cl
  CFLAGS = /nologo -DENABLE_PERL
  LIBS   = wsock32.lib kernel32.lib advapi32.lib

else

  O	 = o
  L	 = a
  E	 =
  CFLAGS = -DENABLE_PERL
  LIBS	 = $(NETLIBS)

endif

CFLAGS		+= $(PENCFLAGS)
#PENLIB		= ../lib/libpen.$L
#CINCLUDES	= -I../lib

all : nbtscan$E $(PENLIB)

OBJS	= nbtscan.$O parse_target_cb.$O \
	  dump_packet.$O \
	  byteswap_nodestats.$O netbios_fixname.$O \
	  process_response.$O netbios_name.$O \
	  display_nbtstat.$O parse_nbtstat.$O \
	  packetio.$O errors.$O hostname.$O \
	  version.$O targets.$O gen_perl.$O

CFLAGS += -DCOMMONFILE=\"nbtscan_common.h\"

OBJS  += all_digitsA.$O \
	die.$O \
	lookup_hostname.$O \
	netbios_pack.$O \
	netbios_unpack.$O \
	netmasks.$O \
	nstrcpyA.$O \
	parse_inaddr.$O \
	parse_target.$O \
	printable_NETBIOS_question_class.$O \
	printable_NETBIOS_question_type.$O \
	sleep_msecs.$O \
	stripA.$O \
	timeval_set_secs.$O \
	winsock.$O

$(OBJS) : nbtdefs.h

CLEAN	= *.$O nbtscan$E nbtscan-source.tgz *.pch

ifeq ($(OS),NT)

LINKDEBUG=/debug /debugtype:both

nbtscan.exe : $(OBJS) $(PENLIB)
	$(strip link /nologo /out:$@ $(OBJS) $(PENLIB) $(LIBS))

else

nbtscan : $(OBJS) $(PENLIB)
	$(strip $(CC) -o $@ $(OBJS) $(PENLIB) $(LIBS))

endif

ifdef COPY
deliver : nbtscan$E
	$(COPY) $^ $(DELIVER)
endif

clean :
	-rm -f $(CLEAN)

#------------------------------------------------------------------------
# Set rules for building objects from sources
#

ifeq ($(OS), NT)

%.obj : %.c
	cl /YXnbtscan_common.h $(CINCLUDES) $(CFLAGS) -c $*.c

else

%.o : %.c
	$(CC) $(CINCLUDES) $(CFLAGS) -c $*.c

endif

#------------------------------------------------------------------------
# Create a tarball
#

SOURCES	 = makefile README
SOURCES += $(OBJS:.o=.c)
SOURCES += nbtdefs.h nbtscan_common.h penlib.h win_sock.h

tarball : nbtscan-source.tgz nbtscan-source.zip

nbtscan-source.tgz : $(SOURCES)
	gtar -czvf $@ $(SOURCES)

nbtscan-source.zip : $(SOURCES)
	zip $@ $(SOURCES)

# ------------------------------------------------------------------------
# PC-Lint stuff
#
ifdef PCLINTDIR

CSRC = $(OBJS:.$O=.c)

lint : $(CSRC)
	lint-nt -i../lib -u local.lnt $(CSRC) > lint.out

endif

ifdef INSTDIR
ifdef PROCESSOR_REVISION

install : nbtscan.exe
	xcopy /Y nbtscan.exe $(INSTDIR)\nbtscan.exe

else 

install : nbtscan
	cp nbtscan $(INSTDIR); chmod a+x $(INSTDIR)/nbtscan

endif
endif
