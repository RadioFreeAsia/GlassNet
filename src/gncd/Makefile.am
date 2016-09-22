## automake.am
##
## Makefile for the gncd(1) client daemon for GlassNet
##
## (C) Copyright 2014-2015 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing @QT4_CFLAGS@ @WEBHOST_CFLAGS@
MOC = @QT4_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@MOC@ $< -o $@


sbin_PROGRAMS = gncd

dist_gncd_SOURCES = config.cpp config.h\
                    database.cpp\
                    gncd.cpp gncd.h\
                    timeengine.cpp timeengine.h

nodist_gncd_SOURCES = db.cpp db.h\
                      moc_gncd.cpp\
                      moc_streamcmdserver.cpp\
                      moc_timeengine.cpp\
                      streamcmdserver.cpp streamcmdserver.h

gncd_LDADD = @QT4_LIBS@ @WEBHOST_LIBS@
# gncd_LDFLAGS = @SIRFLAGS@

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk
DISTCLEANFILES = db.cpp db.h\
                 streamcmdserver.cpp streamcmdserver.h
MAINTAINERCLEANFILES = *~\
                       Makefile.in
