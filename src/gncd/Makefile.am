## automake.am
##
## Makefile for the gncd(1) client daemon for GlassNet
##
## (C) Copyright 2014-2022 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing -std=c++11 -fPIC @QT5CLI_CFLAGS@
MOC = @QT5_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@MOC@ $< -o $@


sbin_PROGRAMS = gncd

dist_gncd_SOURCES = config.cpp config.h\
                    gncd.cpp gncd.h\
                    clientdb.cpp\
                    timeengine.cpp timeengine.h

nodist_gncd_SOURCES = cmdswitch.cpp cmdswitch.h\
                      common.h\
                      db.cpp db.h\
                      moc_db.cpp\
                      moc_gncd.cpp\
                      moc_streamcmdserver.cpp\
                      moc_timeengine.cpp\
                      streamcmdserver.cpp streamcmdserver.h\
                      tzmap.cpp tzmap.h

gncd_LDADD = @QT5CLI_LIBS@

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk

DISTCLEANFILES = accessor.cpp accessor.h\
                 chassis.cpp chassis.h\
                 cmdswitch.cpp cmdswitch.h\
                 common.h\
                 db.cpp db.h\
                 event.h event.cpp\
                 feed.h feed.cpp\
                 receiver.cpp receiver.h\
                 site.cpp site.h\
                 user.cpp user.h\
                 managementconfig.cpp managementconfig.h\
                 streamcmdserver.cpp streamcmdserver.h\
                 tzmap.cpp tzmap.h

MAINTAINERCLEANFILES = *~\
                       Makefile.in
