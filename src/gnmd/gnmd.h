// gnmd.h
//
// gncd(8) management daemon for GlassNet
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

#ifndef GNMD_H
#define GNMD_H

#include <map>

#include <QObject>
#include <QProcess>
#include <QTimer>

#include "managementconfig.h"
#include "receiverconnection.h"
#include "streamcmdserver.h"

#define GNMD_USAGE "[options]\n"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
 enum ReceiverCommands {Exit=0,Addr=1,Set=2,Delete=3,Clear=4,Update=5,
			Playstart=6,Playstop=7};
  MainObject(QObject *parent=0);

 private slots:
  void commandReceivedData(int id,int cmd,const QStringList &args);
  void receiverDisconnectedData(int id);
  void postData();
  void pendingCommandData();
  void exitData();

 private:
  bool ProcessAddr(int id,const QStringList &args);
  void ProcessPlaystart(int id,const QStringList &args);
  void ProcessPlaystop(int id,const QStringList &args);
  void InitReceivers() const;
  void ResetReceiver(int id);
  QTimer *gnmd_post_timer;
  QTimer *gnmd_pending_command_timer;
  QTimer *gnmd_exit_timer;
  ReceiverConnection *GetReceiverConnection(int id,const QString &mac="");
  ReceiverConnection *GetReceiverConnection(const QString &mac);
  void CloseReceiverConnection(int id);
  StreamCmdServer *gnmd_cmd_server;
  std::map<int,ReceiverConnection *> gnmd_rcvr_connections;
  Config *gnmd_config;
};


#endif  // GNMD_H
