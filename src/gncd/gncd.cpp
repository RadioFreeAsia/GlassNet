// gncd.cpp
//
// gncd(1) client daemon for GlassNet
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

#include <map>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>
#include <QDateTime>
#include <QStringList>
#include <QTcpServer>
#include <QUrl>

#include <wh/whcmdswitch.h>

#include "gncd.h"
#include "db.h"
#include "paths.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  gncd_player_process=NULL;
  WHCmdSwitch *cmd=
    new WHCmdSwitch(qApp->argc(),qApp->argv(),"gncd",VERSION,
		    GNCD_USAGE);
  for(unsigned i=0;i<(cmd->keys());i++) {
    if(!cmd->processed(i)) {
      fprintf(stderr,"gncd: unknown option\n");
      exit(256);
    }
  }

  gncd_config=new Config();
  gncd_config->load();

  OpenDb();

  //
  // Command Server
  //
  QTcpServer *server=new QTcpServer(this);
  server->listen(QHostAddress::Any,gncd_config->commandPort());
  std::map<int,QString> cmds;
  std::map<int,int> upper_limits;
  std::map<int,int> lower_limits;

  cmds[MainObject::Delete]="DELETE";
  upper_limits[MainObject::Delete]=1;
  lower_limits[MainObject::Delete]=1;

  cmds[MainObject::Exit]="EXIT";
  upper_limits[MainObject::Exit]=0;
  lower_limits[MainObject::Exit]=0;

  cmds[MainObject::List]="LIST";
  upper_limits[MainObject::List]=1;
  lower_limits[MainObject::List]=0;

  cmds[MainObject::Set]="SET";
  upper_limits[MainObject::Set]=11;
  lower_limits[MainObject::Set]=11;

  cmds[MainObject::Event]="EVENT";
  upper_limits[MainObject::Set]=11;
  lower_limits[MainObject::Set]=11;

  gncd_cmd_server=
    new StreamCmdServer(cmds,upper_limits,lower_limits,server,this);
  connect(gncd_cmd_server,SIGNAL(commandReceived(int,int,const QStringList &)),
	  this,SLOT(commandReceivedData(int,int,const QStringList &)));

  //
  // Time Engine
  //
  gncd_time_engine=new TimeEngine(this);
  connect(gncd_time_engine,SIGNAL(eventTriggered(unsigned)),
	  this,SLOT(eventTriggeredData(unsigned)));

  gncd_time_engine->reload();

  //
  // Timers
  //
  gncd_stop_timer=new QTimer(this);
  gncd_stop_timer->setSingleShot(true);
  connect(gncd_stop_timer,SIGNAL(timeout()),this,SLOT(stopData()));
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
  switch((MainObject::Commands)cmd) {
  case MainObject::Delete:
    ProcessDelete(id,args);
    break;

  case MainObject::Exit:
    gncd_cmd_server->closeConnection(id);
    break;

  case MainObject::List:
    ProcessList(id,args);
    break;

  case MainObject::Set:
    ProcessSet(id,args);
    break;

  case MainObject::Event:
    break;
  }
}


void MainObject::eventTriggeredData(unsigned guid)
{
  //  printf("eventTriggeredData(%u)\n",guid);

  QString sql;
  SqlQuery *q=NULL;

  sql=QString("select ")+
    "URL,"+
    "LENGTH "+
    "from EVENTS where "+
    QString().sprintf("GUID=%u",guid);
  q=new SqlQuery(sql);
  if(q->first()) {
    QStringList args;

    args.push_back("--stats-out");
    args.push_back("--audio-device="+gncd_config->audioDevice());
    args.push_back("--alsa-device="+gncd_config->alsaDevice());
    args.push_back(q->value(0).toString());
    if(gncd_player_process!=NULL) {
      delete gncd_player_process;
    }
    gncd_player_process=new QProcess(this);
    connect(gncd_player_process,SIGNAL(finished(int,QProcess::ExitStatus)),
	    this,SLOT(playerFinishedData(int,QProcess::ExitStatus)));
    connect(gncd_player_process,SIGNAL(error(QProcess::ProcessError)),
	    this,SLOT(playerErrorData(QProcess::ProcessError)));

    gncd_player_process->start("/usr/bin/glassplayer",args);
    gncd_stop_timer->start(q->value(1).toInt());
  }
  delete q;
}


void MainObject::playerFinishedData(int exit_code,QProcess::ExitStatus status)
{
  if(status!=QProcess::NormalExit) {
    fprintf(stderr,"gncd: glassplayer process crashed\n");
  }
  else {
    if(exit_code!=0) {
      fprintf(stderr,
    "gncd: glassplayer process returned non-zero exit code %d [%s]",
	      exit_code,
	      (const char *)gncd_player_process->readAllStandardError().
	      constData());
    }
  }
  gncd_stop_timer->stop();
}


void MainObject::playerErrorData(QProcess::ProcessError err)
{
  fprintf(stderr,"gncd: glassplayer process error %d\n",err);
}


void MainObject::stopData()
{
  gncd_player_process->terminate();
}


bool MainObject::ProcessDelete(int id,const QStringList &args)
{
  bool ok=false;
  unsigned guid=args[0].toUInt(&ok);

  if(!ok) {
    return false;
  }
  QString sql=QString("delete from EVENTS where ")+
    QString().sprintf("GUID=%u",guid);
  SqlQuery::run(sql);
  gncd_time_engine->reload();

  return true;
}


void MainObject::ProcessList(int id,const QStringList &args)
{
  QString sql=QString("select ")+
    "GUID,"+        // 00
    "START_TIME,"+  // 01
    "LENGTH,"+      // 02
    "SUN,"+         // 03
    "MON,"+         // 04
    "TUE,"+         // 05
    "WED,"+         // 06
    "THU,"+         // 07
    "FRI,"+         // 08
    "SAT,"+         // 09
    "URL "+         // 10
    "from EVENTS";
  if(args.size()>0) {
    sql+=" where GUID="+args[0];
  }
  SqlQuery *q=new SqlQuery(sql);
  while(q->next()) {
    QStringList resp;
    resp.push_back(QString().sprintf("%d",q->value(0).toInt()));
    resp.push_back(QTime().addSecs(q->value(1).toInt()).toString());
    resp.push_back(QString().sprintf("%d",q->value(2).toInt()/1000));
    resp.push_back(QString().sprintf("%d",q->value(3).toInt()));
    resp.push_back(QString().sprintf("%d",q->value(4).toInt()));
    resp.push_back(QString().sprintf("%d",q->value(5).toInt()));
    resp.push_back(QString().sprintf("%d",q->value(6).toInt()));
    resp.push_back(QString().sprintf("%d",q->value(7).toInt()));
    resp.push_back(QString().sprintf("%d",q->value(8).toInt()));
    resp.push_back(QString().sprintf("%d",q->value(9).toInt()));
    resp.push_back(q->value(10).toString());
    gncd_cmd_server->sendCommand(id,(int)MainObject::Event,resp);
  }
  delete q;
}


bool MainObject::ProcessSet(int id,const QStringList &args)
{
  QString sql;
  SqlQuery *q=NULL;
  bool ok=false;

  //
  // Parse Command
  //
  unsigned guid=args[0].toInt(&ok);
  if(!ok) {
    return false;
  }
  QTime start_time=QTime::fromString(args[1],"hh:mm:ss");
  if(!start_time.isValid()) {
    return false;
  }
  unsigned length=args[2].toUInt(&ok);
  if(!ok) {
    return false;
  }
  bool dow[7];
  for(unsigned i=0;i<7;i++) {
    dow[i]=args[3+i]!="0";
  }
  QUrl url(args[10]);
  if(!url.isValid()) {
    return false;
  }

  //
  // Process
  //
  sql=QString("select GUID from EVENTS where ")+
    QString().sprintf("GUID=%u",guid);
  q=new SqlQuery(sql);
  if(q->first()) {
    sql=QString("update EVENTS set ")+
      QString().sprintf("START_TIME=%u,",QTime().secsTo(start_time))+
      QString().sprintf("LENGTH=%u,",1000*length)+
      QString().sprintf("SUN=%u,",dow[0])+
      QString().sprintf("MON=%u,",dow[1])+
      QString().sprintf("TUE=%u,",dow[2])+
      QString().sprintf("WED=%u,",dow[3])+
      QString().sprintf("THU=%u,",dow[4])+
      QString().sprintf("FRI=%u,",dow[5])+
      QString().sprintf("SAT=%u,",dow[6])+
      "URL='"+SqlQuery::escape(url.toString())+"' where "+
      QString().sprintf("GUID=%u",guid);
  }
  else {
    sql=QString("insert into EVENTS (")+
      "GUID,"+        // 00
      "START_TIME,"+  // 01
      "LENGTH,"+      // 02
      "SUN,"+         // 03
      "MON,"+         // 04
      "TUE,"+         // 05
      "WED,"+         // 06
      "THU,"+         // 07
      "FRI,"+         // 08
      "SAT,"+         // 09
      "URL) "+         // 10
      "VALUES("+
      QString().sprintf("%u,",guid)+                        // 00
      QString().sprintf("%u,",QTime().secsTo(start_time))+  // 01
      QString().sprintf("%u,",1000*length)+                 // 02
      QString().sprintf("%d,",dow[0])+                      // 03
      QString().sprintf("%d,",dow[1])+                      // 04
      QString().sprintf("%d,",dow[2])+                      // 05
      QString().sprintf("%d,",dow[3])+                      // 06
      QString().sprintf("%d,",dow[4])+                      // 07
      QString().sprintf("%d,",dow[5])+                      // 08
      QString().sprintf("%d,",dow[6])+                      // 09
      "'"+SqlQuery::escape(url.toString())+"'"+             // 10
      ")";
  }
  delete q;
  SqlQuery::run(sql);
  gncd_time_engine->reload();

  return true;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
