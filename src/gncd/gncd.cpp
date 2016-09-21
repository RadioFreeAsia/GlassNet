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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>
#include <QStringList>

#include <wh/whcmdswitch.h>

#include "gncd.h"
#include "paths.h"

bool global_exiting=false;
void SigHandler(int signo)
{
  switch(signo) {
  case SIGINT:
  case SIGTERM:
    global_exiting=true;
    break;
  }
}


MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  host_player_process=NULL;
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

  /*
  //
  // Timer
  //
  host_restart_timer=new QTimer(this);
  host_restart_timer->setSingleShot(true);
  connect(host_restart_timer,SIGNAL(timeout()),this,SLOT(restartData()));

  host_exit_timer=new QTimer(this);
  connect(host_exit_timer,SIGNAL(timeout()),this,SLOT(exitData()));

  host_watchdog_timer=new QTimer(this);
  host_watchdog_timer->setSingleShot(true);
  connect(host_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));

  //
  // Configure Signals
  //
  ::signal(SIGHUP,SigHandler);
  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);

  host_restart_timer->start(0);
  host_exit_timer->start(1000);
  */
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
	      (const char *)host_player_process->readAllStandardError().
	      constData());
    }
  }
  if(global_exiting) {
    exit(0);
  }
  host_restart_timer->start(GNCD_RESTART_INTERVAL);
}


void MainObject::playerErrorData(QProcess::ProcessError err)
{
  fprintf(stderr,"gncd: glassplayer process error %d\n",err);
}


void MainObject::restartData()
{
  /*
  QStringList args;

  args.push_back("--stats-out");
  args.push_back("--audio-device="+host_config->audioDevice());
  args.push_back("--alsa-device="+host_config->alsaDevice());
  args.push_back(host_config->streamUrl());
  if(host_player_process!=NULL) {
    delete host_player_process;
  }
  host_player_process=new QProcess(this);
  connect(host_player_process,SIGNAL(finished(int,QProcess::ExitStatus)),
	  this,SLOT(playerFinishedData(int,QProcess::ExitStatus)));
  connect(host_player_process,SIGNAL(error(QProcess::ProcessError)),
	  this,SLOT(playerErrorData(QProcess::ProcessError)));

  if(host_formatter_process!=NULL) {
    delete host_formatter_process;
  }
  host_formatter_process=new QProcess(this);
  host_player_process->setStandardOutputProcess(host_formatter_process);
  
  host_player_process->start("/usr/bin/glassplayer",args);
  host_formatter_process->start("/usr/bin/glassformatter");
  host_formatter_process->setProcessChannelMode(QProcess::ForwardedChannels);
  */
}


void MainObject::exitData()
{
  if(global_exiting) {
    if(host_player_process==NULL) {
      exit(0);
    }
    host_exit_timer->stop();
    host_player_process->terminate();
    host_watchdog_timer->start(GNCD_WATCHDOG_INTERVAL);
  }
}


void MainObject::watchdogData()
{
  host_player_process->kill();
  fprintf(stderr,"sent SIGKILL to glassplayer process\n");
  qApp->processEvents();
  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
