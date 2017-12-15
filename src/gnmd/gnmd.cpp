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

#include <signal.h>
#include <syslog.h>

#include <QCoreApplication>

#include "cmdswitch.h"
#include "event.h"
#include "gnmd.h"
#include "db.h"
#include "paths.h"
#include "receiver.h"

//
// Globals
//
bool global_is_exiting=false;

void SigHandler(int signo)
{
  switch(signo) {
  case SIGINT:
  case SIGTERM:
    global_is_exiting=true;
    break;
  }
}


MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString err_msg;
  CmdSwitch *cmd=
    new CmdSwitch(qApp->argc(),qApp->argv(),"gnmd",VERSION,GNMD_USAGE);
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
  DbHeartbeat(this);

  //
  // Initialize Receiver Status
  //
  InitReceivers();

  //
  // Command Server
  //
  QTcpServer *server=new QTcpServer(this);
  server->listen(QHostAddress::Any,gnmd_config->receiverCallbackPort());
  std::map<int,QString> cmds;
  std::map<int,int> upper_limits;
  std::map<int,int> lower_limits;

  cmds[MainObject::Exit]="EXIT";
  upper_limits[MainObject::Exit]=0;
  lower_limits[MainObject::Exit]=0;

  cmds[MainObject::Addr]="ADDR";
  upper_limits[MainObject::Addr]=3;
  lower_limits[MainObject::Addr]=3;

  cmds[MainObject::Set]="SET";
  upper_limits[MainObject::Set]=11;
  lower_limits[MainObject::Set]=11;

  cmds[MainObject::Delete]="DELETE";
  upper_limits[MainObject::Delete]=1;
  lower_limits[MainObject::Delete]=1;

  cmds[MainObject::Clear]="CLEAR";
  upper_limits[MainObject::Clear]=0;
  lower_limits[MainObject::Clear]=0;

  cmds[MainObject::Update]="UPDATE";
  upper_limits[MainObject::Update]=0;
  lower_limits[MainObject::Update]=0;

  cmds[MainObject::Playstart]="PLAYSTART";
  upper_limits[MainObject::Playstart]=1;
  lower_limits[MainObject::Playstart]=1;

  cmds[MainObject::Playstop]="PLAYSTOP";
  upper_limits[MainObject::Playstop]=0;
  lower_limits[MainObject::Playstop]=0;

  gnmd_cmd_server=
    new StreamCmdServer(cmds,upper_limits,lower_limits,server,this);
  connect(gnmd_cmd_server,SIGNAL(commandReceived(int,int,const QStringList &)),
	  this,SLOT(commandReceivedData(int,int,const QStringList &)));
  connect(gnmd_cmd_server,SIGNAL(disconnected(int)),
	  this,SLOT(receiverDisconnectedData(int)));

  //
  // Timers
  //
  gnmd_post_timer=new QTimer(this);
  connect(gnmd_post_timer,SIGNAL(timeout()),this,SLOT(postData()));
  gnmd_post_timer->start(GLASSNET_MANAGEMENT_POST_INTERVAL);

  gnmd_pending_command_timer=new QTimer(this);
  connect(gnmd_pending_command_timer,SIGNAL(timeout()),
	  this,SLOT(pendingCommandData()));
  gnmd_pending_command_timer->
    start(GLASSNET_MANAGEMENT_PENDING_COMMAND_INTERVAL);

  gnmd_timestamp_timer=new QTimer(this);
  connect(gnmd_timestamp_timer,SIGNAL(timeout()),this,SLOT(timestampData()));
  gnmd_timestamp_timer->start(GLASSNET_GNMD_TIMESTAMP_INTERVAL);

  gnmd_exit_timer=new QTimer(this);
  connect(gnmd_exit_timer,SIGNAL(timeout()),this,SLOT(exitData()));
  gnmd_exit_timer->start(100);

  //
  // Signals
  //
  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
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

  case MainObject::Addr:
    if(!ProcessAddr(id,args)) {
      CloseReceiverConnection(id);
    }
    break;

  case MainObject::Playstart:
    ProcessPlaystart(id,args);
    break;

  case MainObject::Playstop:
    ProcessPlaystop(id,args);
    break;

  case MainObject::Clear:
  case MainObject::Delete:
  case MainObject::Set:
  case MainObject::Update:
    break;
  }
}


void MainObject::receiverDisconnectedData(int id)
{
  for(std::map<int,ReceiverConnection *>::iterator it=
	gnmd_rcvr_connections.begin();
      it!=gnmd_rcvr_connections.end();it++) {
    if(it->first==id) {
      QString sql=QString("update RECEIVERS set ")+
	"ONLINE=0,"+
	"INTERFACE_ADDRESS=null,"+
	"ACTIVE_GUID=null,"+
	"PUBLIC_ADDRESS=null where "+
	"MAC_ADDRESS='"+it->second->macAddress()+"'";
      SqlQuery::run(sql);

      Receiver *rcvr=new Receiver(it->second->macAddress());
      sql=QString("delete from PENDING_COMMANDS where ")+
	QString().sprintf("RECEIVER_ID=%d",rcvr->id());
      SqlQuery::run(sql);
      delete rcvr;

      delete it->second;
      gnmd_rcvr_connections.erase(it);
      break;
    }
  }
}


void MainObject::postData()
{
  QString sql;
  SqlQuery *q=NULL;
  int receiver_id;

  //
  // Purge Deleted Events
  //
  sql=QString("select ")+
    "DELETED_EVENTS.ID,"+            // 00
    "DELETED_EVENTS.SITE_ID,"+       // 01
    "DELETED_EVENTS.CHASSIS_SLOT,"+  // 02
    "DELETED_EVENTS.EVENT_ID,"+      // 03
    "RECEIVERS.MAC_ADDRESS "+        // 04
    "from DELETED_EVENTS left join SITES on DELETED_EVENTS.SITE_ID=SITES.ID "+
    "left join CHASSIS on SITES.ID=CHASSIS.SITE_ID "+
    "left join RECEIVERS on CHASSIS.ID=RECEIVERS.CHASSIS_ID";
  q=new SqlQuery(sql);
  while(q->next()) {
    ReceiverConnection *conn=GetReceiverConnection(q->value(4).toString());
    if(conn!=NULL) {
      QStringList args;
      args.push_back(QString().sprintf("%d",q->value(3).toInt()));
      gnmd_cmd_server->sendCommand(conn->id(),MainObject::Delete,args);
      sql=QString("delete from DELETED_EVENTS where ")+
	QString().sprintf("ID=%d",q->value(0).toInt());
      SqlQuery::run(sql);
      syslog(LOG_DEBUG,"purged event %d from receiver %s",q->value(3).toInt(),
	     (const char *)q->value(4).toString().toUtf8());
    }
  }
  delete q;

  //
  // Post New Events
  //
  sql=QString("select ")+
    "EVENTS.ID,"+              // 00
    "EVENTS.START_TIME,"+      // 01
    "EVENTS.LENGTH,"+          // 02
    "EVENTS.SUN,"+             // 03
    "EVENTS.MON,"+             // 04
    "EVENTS.TUE,"+             // 05
    "EVENTS.WED,"+             // 06
    "EVENTS.THU,"+             // 07
    "EVENTS.FRI,"+             // 08
    "EVENTS.SAT,"+             // 09
    "FEEDS.URL "+              // 10
    "from EVENTS left join SITES on EVENTS.SITE_ID=SITES.ID "+
    "left join FEEDS on EVENTS.FEED_ID=FEEDS.ID "+
    "where EVENTS.POSTED=0";
  q=new SqlQuery(sql);
  while(q->next()) {
    if((receiver_id=Event::receiverId(q->value(0).toInt()))>=0) {
      Receiver *rcvr=new Receiver(receiver_id);
      ReceiverConnection *conn=GetReceiverConnection(rcvr->macAddress());
      if(conn!=NULL) {
	QStringList args;
	args.push_back(QString().sprintf("%d",q->value(0).toInt()));
	args.push_back(q->value(1).toTime().toString("hh:mm:ss"));
	args.push_back(QString().sprintf("%d",q->value(2).toInt()/1000));
	args.push_back(QString().sprintf("%d",q->value(3).toInt()));
	args.push_back(QString().sprintf("%d",q->value(4).toInt()));
	args.push_back(QString().sprintf("%d",q->value(5).toInt()));
	args.push_back(QString().sprintf("%d",q->value(6).toInt()));
	args.push_back(QString().sprintf("%d",q->value(7).toInt()));
	args.push_back(QString().sprintf("%d",q->value(8).toInt()));
	args.push_back(QString().sprintf("%d",q->value(9).toInt()));
	args.push_back(q->value(10).toString());
	gnmd_cmd_server->sendCommand(conn->id(),MainObject::Set,args);
	Event *event=new Event(q->value(0).toInt());
	event->setPosted(true);
	delete event;
	syslog(LOG_DEBUG,"posted event %d to receiver %s",q->value(0).toInt(),
	       (const char *)rcvr->macAddress().toUtf8());
      }
      delete rcvr;
    }
  }
  delete q;

  //
  // Timeout AWOL Receivers
  //
  sql=QString("select ID,MAC_ADDRESS from RECEIVERS where ")+
    "(ONLINE!=0)&&"+
    "(LAST_SEEN<\""+
    QDateTime::currentDateTime().addMSecs(-3*GLASSNET_RECEIVER_PING_INTERVAL).
    toString("yyyy-MM-dd hh:mm:ss")+"\")";
  q=new SqlQuery(sql);
  while(q->next()) {
    sql=QString("update RECEIVERS set ")+
      "ONLINE=0,"+
      "INTERFACE_ADDRESS=null,"+
      "PUBLIC_ADDRESS=null where "+
      QString().sprintf("ID=%d",q->value(0).toInt());
    SqlQuery::run(sql);
    for(std::map<int,ReceiverConnection *>::iterator it=
	  gnmd_rcvr_connections.begin();it!=gnmd_rcvr_connections.end();it++) {
      if(it->second->macAddress()==q->value(1).toString()) {
	delete it->second;
	gnmd_rcvr_connections.erase(it);
	break;
      }
    }
  }
  delete q;
}


void MainObject::pendingCommandData()
{
  QString sql;
  SqlQuery *q;

  for(std::map<int,ReceiverConnection *>::iterator it=
	gnmd_rcvr_connections.begin();it!=gnmd_rcvr_connections.end();it++) {
    sql=QString("select PENDING_COMMANDS.ID,PENDING_COMMANDS.COMMAND from ")+
      "PENDING_COMMANDS "+
      "left join RECEIVERS on PENDING_COMMANDS.RECEIVER_ID=RECEIVERS.ID where "+
      "RECEIVERS.MAC_ADDRESS=\""+SqlQuery::escape(it->second->macAddress())+
      "\"";
    q=new SqlQuery(sql);
    if(q->first()) {
      gnmd_cmd_server->sendString(it->second->id(),q->value(1).toString());
      sql=QString("delete from PENDING_COMMANDS where ")+
	QString().sprintf("ID=%d",q->value(0).toInt());
      SqlQuery::run(sql);
      syslog(LOG_DEBUG,"sent command \"%s\" to receiver %s",
	     (const char *)q->value(1).toString().toUtf8(),
	     (const char *)it->second->macAddress().toUtf8());
    }
    delete q;
  }
}


void MainObject::timestampData()
{
  SqlQuery::run("update VERSION set GNMD_TIMESTAMP=now()");
}


void MainObject::exitData()
{
  if(global_is_exiting) {
    InitReceivers();
    exit(0);
  }
}


bool MainObject::ProcessAddr(int id,const QStringList &args)
{
  ReceiverConnection *conn=NULL;
  QHostAddress addr;

  //
  // Validate Arguments
  //
  if(!Receiver::isMacAddress(args[0])) {
    return false;
  }
  if(!addr.setAddress(args[1])) {
    return false;
  }
  if(args[2].split(".").size()!=3) {
    return false;
  }

  QString sql=QString("select ID from RECEIVERS where ")+
    "MAC_ADDRESS='"+SqlQuery::escape(args[0])+"'";
  if(SqlQuery::rows(sql)==0) {
    return false;
  }
  conn=GetReceiverConnection(id,args[0]);
  sql=QString("update RECEIVERS set ")+
    "ONLINE=1,"+
    "FIRMWARE_VERSION='"+args[2]+"',"+
    "INTERFACE_ADDRESS='"+args[1]+"',"+
    "PUBLIC_ADDRESS='"+gnmd_cmd_server->peerAddress(id).toString()+"',"+
    "LAST_SEEN=now() where "+
    "MAC_ADDRESS='"+conn->macAddress()+"'";
  SqlQuery::run(sql);

  //
  // Check for firmware update
  //
  sql=QString("select ID from RECEIVERS where ")+
    "UPDATE_FIRMWARE=1 && "+
    "MAC_ADDRESS='"+conn->macAddress()+"'";
  if(SqlQuery::rows(sql)>0) {
    gnmd_cmd_server->sendCommand(id,MainObject::Update);
    sql=QString("update RECEIVERS set UPDATE_FIRMWARE=0 where ")+
    "MAC_ADDRESS='"+conn->macAddress()+"'";
    SqlQuery::run(sql);
    syslog(LOG_DEBUG,"sent update command to receiver %s",
	   (const char *)conn->macAddress().toUtf8());
  }

  return true;
}


void MainObject::ProcessPlaystart(int id,const QStringList &args)
{
  ReceiverConnection *conn=NULL;
  QString sql;
  unsigned guid;
  bool ok;

  try {
    conn=gnmd_rcvr_connections.at(id);
  }
  catch(...) {
    syslog(LOG_WARNING,"received PLAYSTART from non-existent connection");
    return;
  }

  QString mac=conn->macAddress();
  if(mac.isEmpty()) {
    syslog(LOG_WARNING,"received PLAYSTART from non-registered connection");
    return;
  }
  try {
    guid=args.at(0).toUInt(&ok);
  }
  catch(...) {
    syslog(LOG_WARNING,"received PLAYSTART with no GUID argument");
    return;
  }
  if(!ok) {
    syslog(LOG_WARNING,"received mal-formatted PLAYSTART command");
    return;
  }
  sql=QString("update RECEIVERS set ")+
    QString().sprintf("ACTIVE_GUID=%d where ",guid)+
    "MAC_ADDRESS=\""+SqlQuery::escape(mac)+"\"";
  SqlQuery::run(sql);

  sql=QString("update EVENTS set IS_ACTIVE=1 where ")+
    QString().sprintf("ID=%d",guid);
  SqlQuery::run(sql);
}


void MainObject::ProcessPlaystop(int id,const QStringList &args)
{
  ReceiverConnection *conn=NULL;
  QString sql;
  SqlQuery *q;
  int guid=-1;

  try {
    conn=gnmd_rcvr_connections.at(id);
  }
  catch(...) {
    return;
  }
  QString mac=conn->macAddress();
  if(mac.isEmpty()) {
    syslog(LOG_WARNING,"received PLAYSTOP from non-registered connection");
    return;
  }
  sql=QString("select ACTIVE_GUID from RECEIVERS where ")+
    "MAC_ADDRESS=\""+SqlQuery::escape(mac)+"\"";
  q=new SqlQuery(sql);
  if(q->first()) {
    guid=q->value(0).toInt();
    sql=QString("update EVENTS set IS_ACTIVE=0 where ")+
      QString().sprintf("ID=%d",guid);
    SqlQuery::run(sql);
  }
  delete q;

  sql=QString("update RECEIVERS set ACTIVE_GUID=null where ")+
    "MAC_ADDRESS=\""+SqlQuery::escape(mac)+"\"";
  SqlQuery::run(sql);
}


ReceiverConnection *MainObject::GetReceiverConnection(int id,const QString &mac)
{
  ReceiverConnection *conn=NULL;

  try {
    conn=gnmd_rcvr_connections.at(id);
  }
  catch(...) {
    if(!mac.isEmpty()) {
      conn=new ReceiverConnection(id,mac);
      gnmd_rcvr_connections[id]=conn;
      ResetReceiver(id);
    }
  }

  return conn;
}


ReceiverConnection *MainObject::GetReceiverConnection(const QString &mac)
{
  for(std::map<int,ReceiverConnection *>::const_iterator it=gnmd_rcvr_connections.begin();it!=gnmd_rcvr_connections.end();it++) {
    if(it->second->macAddress()==mac) {
      return it->second;
    }
  }
  return NULL;
}


void MainObject::CloseReceiverConnection(int id)
{
  gnmd_cmd_server->closeConnection(id);
  receiverDisconnectedData(id);
}


void MainObject::InitReceivers() const
{
  QString sql=QString("update RECEIVERS set ")+
    "ONLINE=0,"+
    "INTERFACE_ADDRESS=null,"+
    "PUBLIC_ADDRESS=null,"+
    "ACTIVE_GUID=null";
  SqlQuery::run(sql);

  sql=QString("delete from PENDING_COMMANDS");
  SqlQuery::run(sql);
}


void MainObject::ResetReceiver(int id)
{
  QString sql;
  SqlQuery *q=NULL;
  SqlQuery *q1;
  SqlQuery *q2;
  QStringList args;
  ReceiverConnection *conn=gnmd_rcvr_connections.at(id);

  //
  // Clear Events
  //
  args.clear();
  gnmd_cmd_server->sendCommand(id,MainObject::Clear,args);
  syslog(LOG_DEBUG,"cleared events list on receiver %s",
	 (const char *)conn->macAddress().toUtf8());

  //
  // Send Current Events Schedule
  //
  sql=QString("select CHASSIS_ID,SLOT from RECEIVERS where ")+
    "MAC_ADDRESS=\""+SqlQuery::escape(conn->macAddress())+"\"";
  q=new SqlQuery(sql);
  if(q->first()) {
    sql=QString("select SITE_ID,SLOT from CHASSIS where ")+
      QString().sprintf("ID=%d",q->value(0).toInt());
    q1=new SqlQuery(sql);
    if(q1->first()) {
      sql=QString("select ")+
	"EVENTS.ID,"+              // 00
	"EVENTS.START_TIME,"+      // 01
	"EVENTS.LENGTH,"+          // 02
	"EVENTS.SUN,"+             // 03
	"EVENTS.MON,"+             // 04
	"EVENTS.TUE,"+             // 05
	"EVENTS.WED,"+             // 06
	"EVENTS.THU,"+             // 07
	"EVENTS.FRI,"+             // 08
	"EVENTS.SAT,"+             // 09
	"FEEDS.URL "+              // 10
	"from EVENTS left join FEEDS on EVENTS.FEED_ID=FEEDS.ID where "+
	QString().sprintf("(SITE_ID=%d)&&",q1->value(0).toInt())+
	QString().sprintf("(CHASSIS_SLOT=%d)&&",q1->value(1).toInt())+
	QString().sprintf("(RECEIVER_SLOT=%d)",q->value(1).toInt());
      q2=new SqlQuery(sql);
      while(q2->next()) {
	QStringList args;
	args.push_back(QString().sprintf("%d",q2->value(0).toInt()));
	args.push_back(q2->value(1).toTime().toString("hh:mm:ss"));
	args.push_back(QString().sprintf("%d",q2->value(2).toInt()/1000));
	args.push_back(QString().sprintf("%d",q2->value(3).toInt()));
	args.push_back(QString().sprintf("%d",q2->value(4).toInt()));
	args.push_back(QString().sprintf("%d",q2->value(5).toInt()));
	args.push_back(QString().sprintf("%d",q2->value(6).toInt()));
	args.push_back(QString().sprintf("%d",q2->value(7).toInt()));
	args.push_back(QString().sprintf("%d",q2->value(8).toInt()));
	args.push_back(QString().sprintf("%d",q2->value(9).toInt()));
	args.push_back(q2->value(10).toString());
	gnmd_cmd_server->sendCommand(conn->id(),MainObject::Set,args);
	Event *event=new Event(q2->value(0).toInt());
	event->setPosted(true);
	delete event;
	syslog(LOG_DEBUG,"posted event %d to receiver %s",q2->value(0).toInt(),
	       (const char *)conn->macAddress().toUtf8());
      }
      delete q2;
    }
    delete q1;
  }
  delete q;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
