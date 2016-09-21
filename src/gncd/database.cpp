// database.cpp
//
// Database routines for gncd(8).
//
// (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "gncd.h"
#include "gnsqlquery.h"

bool MainObject::OpenDb()
{
  QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
  if(!db.isValid()) {
    fprintf(stderr,"gncd: unable to add database\n");
    exit(256);
  }
  db.setDatabaseName(gncd_config->dbName());
  if(!db.open()) {
    fprintf(stderr,"gncd: unable to open database\n");
    exit(256);
  }

  QString sql=QString("select DB from VERSION");
  GNSqlQuery *q=new GNSqlQuery(sql);
  if(!q->first()) {
    if(!CreateDb()) {
      fprintf(stderr,"gncd: unable to create database\n");
      exit(256);
    }
  }
  delete q;
  if(!CheckSchema()) {
    fprintf(stderr,"gncd: invalid/unrecognized database schema\n");
    exit(256);
  }

  return true;
}


bool MainObject::CreateDb()
{
  QString sql;
  bool ok=false;

  sql=QString("create table VERSION(DB int not null)");
  GNSqlQuery::run(sql,&ok);
  if(!ok) {
    return false;
  }
  sql=QString("insert into VERSION values(0)");
  GNSqlQuery::run(sql,&ok);

  return ok;
}


bool MainObject::CheckSchema()
{
  int schema;
  bool ok=false;
  QString sql;
  GNSqlQuery *q=NULL;

  sql=QString("select DB from VERSION");
  q=new GNSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  schema=q->value(0).toInt();
  delete  q;

  if(schema<1) {
    sql=QString("create table EVENTS (")+
      "ID integer primary key autoincrement,"+
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
  GNSqlQuery::run(sql,&ok);
  if(!ok) {
    return false;
  }


  //
  // *** End of schema updates ***
  //

  sql=QString("update VERSION set ")+
    QString().sprintf("DB=%d",GNCD_SCHEMA_VERSION);
  GNSqlQuery::run(sql,&ok);

  return ok;
}
