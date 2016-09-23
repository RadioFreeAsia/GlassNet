#!/bin/sh

# link_common.sh
#
#  Link common sources for GlassNet
#
#   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

DESTDIR=$1

rm -f src/$DESTDIR/db.cpp
ln -s ../../src/common/db.cpp src/$DESTDIR/db.cpp
rm -f src/$DESTDIR/db.h
ln -s ../../src/common/db.h src/$DESTDIR/db.h

rm -f src/$DESTDIR/mysqlconfig.cpp
ln -s ../../src/common/mysqlconfig.cpp src/$DESTDIR/mysqlconfig.cpp
rm -f src/$DESTDIR/mysqlconfig.h
ln -s ../../src/common/mysqlconfig.h src/$DESTDIR/mysqlconfig.h

rm -f src/$DESTDIR/streamcmdserver.cpp
ln -s ../../src/common/streamcmdserver.cpp src/$DESTDIR/streamcmdserver.cpp
rm -f src/$DESTDIR/streamcmdserver.h
ln -s ../../src/common/streamcmdserver.h src/$DESTDIR/streamcmdserver.h
