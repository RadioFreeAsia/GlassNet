// servicemonitor.h
//
// Heartbeat monitor for the gnmd(8) service.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SERVICEMONITOR_H
#define SERVICEMONITOR_H

#include <QObject>
#include <QTimer>

class ServiceMonitor : public QObject
{
  Q_OBJECT
 public:
  ServiceMonitor(QObject *parent=0);
  bool isActive() const;
  void start();

 signals:
  void stateChanged(bool state);

 private slots:
  void timerData();

 private:
  int monitor_state;
  QTimer *monitor_timer;
};


#endif  //  SERVICEMONITOR_H
