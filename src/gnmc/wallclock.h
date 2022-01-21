// wallclock.h
//
// Wall Clock Widget
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef WALLCLOCK_H
#define WALLCLOCK_H

#include <QDateTime>
#include <QTimer>
#include <QLabel>

#include "tzmap.h"

class WallClock : public QLabel
{
 Q_OBJECT;
 public:
  WallClock(QWidget *parent=0);
  ~WallClock();

 private slots:
  void timeoutData();

 private:
  QTimer *d_timer;
  QDateTime d_current_datetime;
  TzMap *d_tz_map;
};


#endif  // WALLCLOCK_H
