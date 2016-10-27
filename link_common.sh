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

rm -f src/$DESTDIR/accessor.cpp
ln -s ../../src/common/accessor.cpp src/$DESTDIR/accessor.cpp
rm -f src/$DESTDIR/accessor.h
ln -s ../../src/common/accessor.h src/$DESTDIR/accessor.h

rm -f src/$DESTDIR/chassis.cpp
ln -s ../../src/common/chassis.cpp src/$DESTDIR/chassis.cpp
rm -f src/$DESTDIR/chassis.h
ln -s ../../src/common/chassis.h src/$DESTDIR/chassis.h

rm -f src/$DESTDIR/cmdswitch.cpp
ln -s ../../src/common/cmdswitch.cpp src/$DESTDIR/cmdswitch.cpp
rm -f src/$DESTDIR/cmdswitch.h
ln -s ../../src/common/cmdswitch.h src/$DESTDIR/cmdswitch.h

rm -f src/$DESTDIR/common.h
ln -s ../../src/common/common.h src/$DESTDIR/common.h

rm -f src/$DESTDIR/db.cpp
ln -s ../../src/common/db.cpp src/$DESTDIR/db.cpp
rm -f src/$DESTDIR/db.h
ln -s ../../src/common/db.h src/$DESTDIR/db.h

rm -f src/$DESTDIR/event.cpp
ln -s ../../src/common/event.cpp src/$DESTDIR/event.cpp
rm -f src/$DESTDIR/event.h
ln -s ../../src/common/event.h src/$DESTDIR/event.h

rm -f src/$DESTDIR/managementconfig.cpp
ln -s ../../src/common/managementconfig.cpp src/$DESTDIR/managementconfig.cpp
rm -f src/$DESTDIR/managementconfig.h
ln -s ../../src/common/managementconfig.h src/$DESTDIR/managementconfig.h

rm -f src/$DESTDIR/receiver.cpp
ln -s ../../src/common/receiver.cpp src/$DESTDIR/receiver.cpp
rm -f src/$DESTDIR/receiver.h
ln -s ../../src/common/receiver.h src/$DESTDIR/receiver.h

rm -f src/$DESTDIR/site.cpp
ln -s ../../src/common/site.cpp src/$DESTDIR/site.cpp
rm -f src/$DESTDIR/site.h
ln -s ../../src/common/site.h src/$DESTDIR/site.h

rm -f src/$DESTDIR/streamcmdserver.cpp
ln -s ../../src/common/streamcmdserver.cpp src/$DESTDIR/streamcmdserver.cpp
rm -f src/$DESTDIR/streamcmdserver.h
ln -s ../../src/common/streamcmdserver.h src/$DESTDIR/streamcmdserver.h

rm -f src/$DESTDIR/user.cpp
ln -s ../../src/common/user.cpp src/$DESTDIR/user.cpp
rm -f src/$DESTDIR/user.h
ln -s ../../src/common/user.h src/$DESTDIR/user.h
