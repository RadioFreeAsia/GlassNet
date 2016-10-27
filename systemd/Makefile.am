## automake.am
##
## systemd/ Makefile.am for GlassNet
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

install-exec-local:	
	mkdir -p $(DESTDIR)/usr/lib/systemd/system
	cp gncd.service $(DESTDIR)/usr/lib/systemd/system/gncd.service
	cp gnmd.service $(DESTDIR)/usr/lib/systemd/system/gnmd.service

uninstall-local:	
	rm -f $(DESTDIR)/usr/lib/systemd/system/gncd.service
	rm -f $(DESTDIR)/usr/lib/systemd/system/gnmd.service

EXTRA_DIST = gncd.service.in\
             gnmd.service.in

CLEANFILES = *~\
             *.deb\
             *.mo\
             *.rpm\
             *.tar.gz\
             *.tgz\
             *.zip

DISTCLEANFILES = *.service

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       config.guess\
                       config.status\
                       config.sub\
                       configure\
                       depcomp\
                       install-sh\
                       libtool\
                       ltmain.sh\
                       Makefile.in\
                       missing\
                       mkinstalldirs
