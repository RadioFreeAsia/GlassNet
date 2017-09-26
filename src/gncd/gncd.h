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
#include "timeengine.h"

#define GNCD_RESTART_INTERVAL 1000
#define GNCD_WATCHDOG_INTERVAL 1000
#define GNCD_USAGE "[options]\n"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  enum Commands {Exit=0,List=1,Set=2,Event=3,Delete=4,Addr=5,Clear=6,
		 Update=7,Playstart=8,Playstop=9};
  MainObject(QObject *parent=0);

 private slots:
  void connectedData(int id);
  void commandReceivedData(int id,int cmd,const QStringList &args);
  void eventTriggeredData(unsigned guid);
  void playerStartedData();
  void playerFinishedData(int exit_code,QProcess::ExitStatus status);
  void playerErrorData(QProcess::ProcessError err);
  void updateFinishedData(int exit_code,QProcess::ExitStatus status);
  void updateErrorData(QProcess::ProcessError err);
  void stopData();
  void pingData();

 private:
  bool ProcessDelete(int id,const QStringList &args);
  void ProcessList(int id,const QStringList &args);
  bool ProcessSet(int id,const QStringList &args);
  void ProcessClear(int id);
  void ProcessPlaystart(int id,const QStringList &args);
  void ProcessUpdate(int id);
  void ReadInterface();
  bool OpenDb();
  bool CreateDb();
  bool CheckSchema();
  QProcess *gncd_player_process;
  QTimer *gncd_stop_timer;
  TimeEngine *gncd_time_engine;
  StreamCmdServer *gncd_cmd_server;
  Config *gncd_config;
  QString gncd_mac_address;
  QHostAddress gncd_ipv4_address;
  QHostAddress gncd_ipv4_netmask;
  QTimer *gncd_ping_timer;
  int gncd_active_guid;
  int gncd_update_pass;
};


#endif  // GNCD_H
