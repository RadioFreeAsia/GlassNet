// sqlite.cpp
//
// SQLite Database routines for gncd(8).
//
// (C) Copyright 2016-2022 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
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
#include <stdlib.h>

#include <QFileInfo>
#include <QProcess>
#include <QSqlError>
#include <QStringList>

#include "db.h"
#include "gncd.h"

void MainObject::OpenDb()
{
  //
  // Check that DB exists
  //
  QFileInfo *dbfile=new QFileInfo(gncd_config->dbName());
  if(dbfile->size()==0) {
    CreateDb(gncd_config->dbName());
  }
  delete dbfile;

  //
  // Load plug-in
  //
  QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
  if(!db.isValid()) {
    fprintf(stderr,"gncd: DB plug-in \"QSQLITE\" initialization failed\n");
    exit(256);
  }

  //
  // Open the DB
  //
  db.setDatabaseName(gncd_config->dbName());
  if(!db.open()) {
    fprintf(stderr,"gncd: unable to open database at \"%s\" [%s]\n",
	    gncd_config->dbName().toUtf8().constData(),
	    db.lastError().text().toUtf8().constData());
    exit(256);
  }

  QString sql=QString("select DB from VERSION");
  SqlQuery *q=new SqlQuery(sql);
  if(!q->first()) {
    fprintf(stderr,"gncd: data at \"%s\" appears corrupt\n",
	    gncd_config->dbName().toUtf8().constData());
    exit(256);
  }
  delete q;
  if(!CheckSchema()) {
    fprintf(stderr,"gncd: invalid/unrecognized schema in database at \"%s\"\n",
	    gncd_config->dbName().toUtf8().constData());
    exit(256);
  }
}


void MainObject::CreateDb(const QString dbfile_name)
{
  QStringList args;

  args.push_back("-batch");
  args.push_back(dbfile_name);
  args.push_back("create table VERSION(DB int not null);insert into VERSION values(0)");
  QProcess *proc=new QProcess(this);
  proc->start("sqlite3",args);
  proc->waitForFinished();
  if(proc->exitStatus()!=QProcess::NormalExit) {
    fprintf(stderr,
	    "gncd: sqlite3(1) crashed attempting to create database at \"%s\"\n",
	    dbfile_name.toUtf8().constData());
    exit(256);
  }
  if(proc->exitCode()!=0) {
    fprintf(stderr,
	    "gncd: sqlite3(1) returned exit code \"%d\" attempting to create database at \"%s\"\n",
	    proc->exitCode(),dbfile_name.toUtf8().constData());
    exit(255);
  }
}


bool MainObject::CheckSchema()
{
  int schema;
  bool ok=false;
  QString sql;
  SqlQuery *q=NULL;

  sql=QString("select DB from VERSION");
  q=new SqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  schema=q->value(0).toInt();
  delete  q;

  if(schema<1) {
    sql=QString("create table EVENTS (")+
      "ID integer primary key autoincrement,"+
      "GUID int unique,"+
      "START_TIME int,"+
      "LENGTH int,"+
      "SUN int,"+
      "MON int,"+
      "TUE int,"+
      "WED int,"+
      "THU int,"+
      "FRI int,"+
      "SAT int,"+
      "URL text)";
  }
  SqlQuery::run(sql,&ok);
  if(!ok) {
    return false;
  }



  //
  // *** End of schema updates ***
  //

  sql=QString("update VERSION set ")+
    QString::asprintf("DB=%d",GNCD_SCHEMA_VERSION);
  SqlQuery::run(sql,&ok);

  return ok;
}
