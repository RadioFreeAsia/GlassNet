// servicemonitor.cpp
//
// Heartbeat monitor for the gnmd(8) service.
//
//   (C) Copyright 2017-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>

#include <QDateTime>

#include "db.h"
#include "servicemonitor.h"

ServiceMonitor::ServiceMonitor(QObject *parent)
  : QObject(parent)
{
  monitor_state=-1;
  monitor_timer=new QTimer(this);
  connect(monitor_timer,SIGNAL(timeout()),this,SLOT(timerData()));
}


bool ServiceMonitor::isActive() const
{
  return monitor_state==1;
}


void ServiceMonitor::start()
{
  monitor_timer->start(GLASSNET_GNMD_TIMESTAMP_INTERVAL);
  timerData();
}


void ServiceMonitor::timerData()
{
  QString sql=QString("select `GNMD_TIMESTAMP` from `VERSION` where ")+
    "`GNMD_TIMESTAMP`>\""+
    SqlQuery::escape(QDateTime::currentDateTime().
		     addMSecs(-2*GLASSNET_GNMD_TIMESTAMP_INTERVAL).
		     toString("yyyy=MM-dd hh:mm:ss"))+"\"";
  SqlQuery *q=new SqlQuery(sql);
  int state=q->first();
  delete q;
  if(state!=monitor_state) {
    monitor_state=state;
    emit stateChanged((bool)monitor_state);
  }
}
