// gncd.h
//
// gncd(8) client daemon for GlassNet
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef GNCD_H
#define GNCD_H

#include <QObject>
#include <QProcess>
#include <QTimer>

#include "config.h"
#include "streamcmdserver.h"

#define GNCD_RESTART_INTERVAL 1000
#define GNCD_WATCHDOG_INTERVAL 1000
#define GNCD_SCHEMA_VERSION 1
#define GNCD_USAGE "[options]\n"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
 enum Commands {Exit=0,List=1,Set=2,Event=3,Delete=4};
  MainObject(QObject *parent=0);

 private slots:
  void commandReceivedData(int id,int cmd,const QStringList &args);
  void playerFinishedData(int exit_code,QProcess::ExitStatus status);
  void playerErrorData(QProcess::ProcessError err);
  void restartData();
  void exitData();
  void watchdogData();

 private:
  bool ProcessDelete(int id,const QStringList &args);
  void ProcessList(int id,const QStringList &args);
  bool ProcessSet(int id,const QStringList &args);
  bool OpenDb();
  bool CreateDb();
  bool CheckSchema();
  QProcess *host_player_process;
  QTimer *host_restart_timer;
  QTimer *host_exit_timer;
  QTimer *host_watchdog_timer;
  StreamCmdServer *gncd_cmd_server;
  Config *gncd_config;
};


#endif  // GNCD_H
