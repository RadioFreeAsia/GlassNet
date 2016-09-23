## automake.am
##
## Makefile for the gnmc(1) management client for GlassNet
##
## (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing @QT4GUI_CFLAGS@ @WEBHOST_CFLAGS@ @OPENSSL_CFLAGS@
MOC = @QT4_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@MOC@ $< -o $@


bin_PROGRAMS = gnmc

dist_gnmc_SOURCES = adduser.cpp adduser.h\
                    changepassword.cpp changepassword.h\
                    crypto.cpp crypto.h\
                    edituser.cpp edituser.h\
                    gnmc.cpp gnmc.h\
                    listusers.cpp listusers.h\
                    login.cpp login.h\
                    mysql.cpp\
                    sqltablemodel.cpp sqltablemodel.h\
                    tableview.cpp tableview.h

nodist_gnmc_SOURCES = db.cpp db.h\
                      mysqlconfig.cpp mysqlconfig.h\
                      moc_adduser.cpp\
                      moc_changepassword.cpp\
                      moc_edituser.cpp\
                      moc_gnmc.cpp\
                      moc_listusers.cpp\
                      moc_login.cpp\
                      moc_sqltablemodel.cpp\
                      moc_tableview.cpp

gnmc_LDADD = @QT4GUI_LIBS@ @WEBHOST_LIBS@ @OPENSSL_LIBS@

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk
DISTCLEANFILES = db.cpp db.h\
                 mysqlconfig.cpp mysqlconfig.h\
                 streamcmdserver.cpp streamcmdserver.h

MAINTAINERCLEANFILES = *~\
                       Makefile.in
