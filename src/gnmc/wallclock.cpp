// wallclock.cpp
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

#include "db.h"
#include "managementconfig.h"
#include "wallclock.h"

WallClock::WallClock(QWidget *parent)
  : QLabel(parent)
{
  setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  //
  // System Timezone Data
  //
  QString tzid="UTC";
  QString sql=QString("select `SYSTEM_TZID` from `VERSION`");
  SqlQuery *q=new SqlQuery(sql);
  if(q->first()) {
    tzid=q->value(0).toString();
  }
  delete q;
  d_tz_map=new TzMap();
  d_tz_map->load(TZMAP_ZONEINFO_DIR+"/"+tzid);

  d_timer=new QTimer(this);
  connect(d_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));
  d_timer->start(100);
}


WallClock::~WallClock()
{
  delete d_timer;
  delete d_tz_map;
}


void WallClock::timeoutData()
{
  QDateTime now=QDateTime::currentDateTimeUtc();
  QDateTime now_rounded(now.date(),
			QTime(now.time().hour(),
			      now.time().minute(),
			      now.time().second()));
  QString tz_name="";

  if(now_rounded!=d_current_datetime) {
    QDateTime dt=d_tz_map->convert(now_rounded,&tz_name);
    setText(dt.toString("ddd, d MMM yyyy - hh:mm:ss")+" "+tz_name);

    d_current_datetime=now_rounded;
  }
}
