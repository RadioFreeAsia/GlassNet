// gnmd.cpp
//
// gncd(1) management daemon for GlassNet
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

#include <syslog.h>

#include <QCoreApplication>

#include <wh/whcmdswitch.h>

#include "gnmd.h"
#include "db.h"
#include "paths.h"
#include "receiver.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString err_msg;
  WHCmdSwitch *cmd=
    new WHCmdSwitch(qApp->argc(),qApp->argv(),"gnmd",VERSION,
		    GNMD_USAGE);
  for(unsigned i=0;i<(cmd->keys());i++) {
    if(!cmd->processed(i)) {
      fprintf(stderr,"gnmd: unknown option\n");
      exit(256);
    }
  }

  //
  // Open Syslog
  //
  openlog("gnmd",LOG_PERROR,LOG_DAEMON);

  //
  // Open Database
  //
  gnmd_config=new Config();
  if(!gnmd_config->openDb(&err_msg,true)) {
    syslog(LOG_ERR,"unable to open database [%s]",
	   (const char *)err_msg.toUtf8());
    exit(256);
  }

  //
  // Command Server
  //
  QTcpServer *server=new QTcpServer(this);
  server->listen(QHostAddress::Any,gnmd_config->receiverCommandPort());
  std::map<int,QString> cmds;
  std::map<int,int> upper_limits;
  std::map<int,int> lower_limits;

  cmds[MainObject::Exit]="EXIT";
  upper_limits[MainObject::Exit]=0;
  lower_limits[MainObject::Exit]=0;

  cmds[MainObject::Mac]="MAC";
  upper_limits[MainObject::Mac]=1;
  lower_limits[MainObject::Mac]=1;

  gnmd_cmd_server=
    new StreamCmdServer(cmds,upper_limits,lower_limits,server,this);
  connect(gnmd_cmd_server,SIGNAL(commandReceived(int,int,const QStringList &)),
	  this,SLOT(commandReceivedData(int,int,const QStringList &)));
  connect(gnmd_cmd_server,SIGNAL(disconnected(int)),
	  this,SLOT(receiverDisconnectedData(int)));
}


void MainObject::commandReceivedData(int id,int cmd,const QStringList &args)
{
  /*
  printf("CMD: %d\n",cmd);
  for(int i=0;i<args.size();i++) {
    printf("[%d]: %s\n",i,(const char *)args[i].toUtf8());
  }
  printf("\n");
  */
  switch((MainObject::ReceiverCommands)cmd) {
  case MainObject::Exit:
    CloseReceiverConnection(id);
    break;

  case MainObject::Mac:
    if(!ProcessMac(id,args)) {
      CloseReceiverConnection(id);
    }
    break;
  }
}


void MainObject::receiverDisconnectedData(int id)
{
  for(std::map<int,ReceiverConnection *>::iterator it=
	gnmd_rcvr_connections.begin();
      it!=gnmd_rcvr_connections.end();it++) {
    if(it->first==id) {
      delete it->second;
      gnmd_rcvr_connections.erase(it);
      break;
    }
  }
}


bool MainObject::ProcessMac(int id,const QStringList &args)
{
  ReceiverConnection *conn=NULL;

  if(!Receiver::isMacAddress(args[0])) {
    return false;
  }
  QString sql=QString("select ID from RECEIVERS where ")+
    "MAC_ADDRESS='"+SqlQuery::escape(args[0])+"'";
  if(SqlQuery::rows(sql)==0) {
    return false;
  }
  conn=GetReceiverConnection(id,args[0]);
  sql=QString("update RECEIVERS set ")+
    "LAST_SEEN=now() where "+
    "MAC_ADDRESS='"+conn->macAddress()+"'";
  SqlQuery::run(sql);

  return true;
}


ReceiverConnection *MainObject::GetReceiverConnection(int id,const QString &mac)
{
  ReceiverConnection *conn=NULL;

  try {
    conn=gnmd_rcvr_connections.at(id);
  }
  catch(...) {
    if(!mac.isEmpty()) {
      conn=new ReceiverConnection(mac);
      gnmd_rcvr_connections[id]=conn;
    }
  }

  return conn;
}


void MainObject::CloseReceiverConnection(int id)
{
  gnmd_cmd_server->closeConnection(id);
  receiverDisconnectedData(id);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
