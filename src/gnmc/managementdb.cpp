// mysql.cpp
//
// MySQL Database routines for gnmc(1).
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

#include <QMessageBox>

#include "crypto.h"
#include "db.h"
#include "gnmc.h"

bool MainWidget::CheckDb(QString *err_msg)
{
  QString sql=QString("select DB from VERSION");
  SqlQuery *q=new SqlQuery(sql);
  if(!q->first()) {
    if(!CreateDb()) {
      *err_msg=tr("unable to create database");
      return false;
    }
  }
  delete q;
  if(!CheckSchema()) {
    *err_msg=tr("invalid/unrecognized database schema");
    return false;
  }

  return true;
}


bool MainWidget::CreateDb()
{
  QString sql;
  bool ok=false;

  sql=QString("create table VERSION(DB int not null)")+
    gnmc_config->createTablePostfix();
  SqlQuery::run(sql,&ok);
  if(!ok) {
    return false;
  }
  sql=QString("insert into VERSION values(0)");
  SqlQuery::run(sql,&ok);

  return ok;
}


bool MainWidget::CheckSchema()
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

  if(schema>GLASSNET_SCHEMA_VERSION) {
    QMessageBox::warning(this,tr("GlassNet - DB Schema Skew"),
	   tr("This version of GlassNet is incompatible with schema version")+
			 QString::asprintf(" %d.",schema));
    exit(256);
  }

  if(schema<GLASSNET_SCHEMA_VERSION) {
    if(QMessageBox::question(this,tr("GlassNet - DB Update"),
			     tr("The DB schema needs to be updated.")+"\n"+
			     tr("Proceed?"),
			     QMessageBox::No,QMessageBox::Yes)!=
       QMessageBox::Yes) {
      exit(0);
    }
  }

  if(schema<1) {
    sql=QString("create table USERS (")+
      "ID integer primary key auto_increment,"+
      "USERNAME char(255) unique not null,"+
      "PASSWORD char(44) not null,"+
      "FULL_NAME text,"+
      "DESCRIPTION text,"+
      "USER_PRIV int not null default 0,"+
      "SITE_PRIV int not null default 0,"+
      "EVENT_PRIV int not null default 0)";
      gnmc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }

    sql=QString("insert into USERS set ")+
      "USERNAME='"+SqlQuery::escape("admin")+"',"+
      "FULL_NAME='"+SqlQuery::escape(tr("GlassNet Administrator"))+"',"+
      "PASSWORD='"+SqlQuery::escape(MakePasswordHash(""))+"',"+
      "USER_PRIV=1,"+
      "SITE_PRIV=1,"+
      "EVENT_PRIV=1";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }

    sql=QString("create table SITES (")+
      "ID integer primary key auto_increment,"+
      "NAME char(255) not null,"+
      "REMARKS text,"+
      "index NAME_IDX(NAME))"+
      gnmc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<2) {
    sql=QString("create table CHASSIS (")+
      "ID integer primary key auto_increment,"+
      "SITE_ID int default null,"+
      "SLOT int default null,"+
      "TYPE int not null,"+
      "SERIAL_NUMBER char(32) not null,"+
      "index SITE_ID_IDX(SITE_ID,SLOT))"+
      gnmc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<3) {
    sql=QString("create table RECEIVERS (")+
      "ID integer primary key auto_increment,"+
      "CHASSIS_ID int default null,"+
      "SLOT int default null,"+
      "TYPE int not null,"+
      "MAC_ADDRESS char(18) unique not null,"+
      "index CHASSIS_ID_IDX(CHASSIS_ID,SLOT))"+
      gnmc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<4) {
    sql=QString("alter table RECEIVERS ")+
      "add column LAST_SEEN datetime default null "+
      "after MAC_ADDRESS";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<5) {
    sql=QString("alter table RECEIVERS ")+
      "add column INTERFACE_ADDRESS char(15) default null "+
      "after MAC_ADDRESS";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
    sql=QString("alter table RECEIVERS ")+
      "add column PUBLIC_ADDRESS char(15) default null "+
      "after INTERFACE_ADDRESS";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<6) {
    sql=QString("alter table RECEIVERS ")+
      "add column ONLINE int not null default 0 "+
      "after TYPE";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
    sql=QString("alter table RECEIVERS ")+
      "add column FIRMWARE_VERSION char(64) default null "+
      "after MAC_ADDRESS";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<7) {
    sql=QString("create table if not exists EVENTS (")+
      "ID int not null primary key auto_increment,"+
      "SITE_ID int not null,"+
      "CHASSIS_SLOT int not null,"+
      "RECEIVER_SLOT int not null,"+
      "START_TIME time not null,"+
      "LENGTH int not null,"+
      "SUN int not null default 0,"+
      "MON int not null default 0,"+
      "TUE int not null default 0,"+
      "WED int not null default 0,"+
      "THU int not null default 0,"+
      "FRI int not null default 0,"+
      "SAT int not null default 0,"+
      "URL text not null,"+
      "index RECEIVER_IDX(SITE_ID,CHASSIS_SLOT,RECEIVER_SLOT))"+
      gnmc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<8) {
    sql=QString("alter table EVENTS ")+
      "add column POSTED int not null default 0 after RECEIVER_SLOT";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }

    sql=QString("create index POSTED_IDX on EVENTS (POSTED)");
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<9) {
    sql=QString("create table if not exists DELETED_EVENTS (")+
      "ID int not null primary key auto_increment,"+
      "SITE_ID int not null,"+
      "CHASSIS_SLOT int not null,"+
      "RECEIVER_SLOT int not null,"+
      "EVENT_ID int not null)"+
      gnmc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<10) {
    sql=QString("alter table RECEIVERS ")+
      "add column UPDATE_FIRMWARE int not null default 0 after LAST_SEEN";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }

    sql=QString("create index UPDATE_FIRMWARE_IDX on ")+
      "RECEIVERS (UPDATE_FIRMWARE)";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<11) {
    sql=QString("create table if not exists FEEDS (")+
      "ID int not null primary key auto_increment,"+
      "NAME char(16) not null,"+
      "URL text,"+
      "unique index NAME_IDX(NAME))"+
      gnmc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }

    sql=QString("alter table USERS ")+
      "add column FEED_PRIV int not null default 0 after EVENT_PRIV";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }

    sql=QString("alter table EVENTS drop column URL");
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
    sql=QString("alter table EVENTS add column FEED_ID int not null after SAT");
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<12) {
    sql=QString("create index FEED_ID_IDX on EVENTS(FEED_ID)");
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<13) {
    sql=QString("alter table RECEIVERS add column ACTIVE_GUID int ")+
      "default NULL after LAST_SEEN";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<14) {
    sql=QString("alter table EVENTS add column IS_ACTIVE int not null ")+
      "default 0 after FEED_ID";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<15) {
    sql=QString("create table if not exists PENDING_COMMANDS (")+
      "ID int auto_increment not null primary key,"+
      "RECEIVER_ID int not null,"+
      "COMMAND char(32) not null,"+
      "index RECEIVER_ID_IDX(ID,RECEIVER_ID))"+
      gnmc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<16) {
    sql=QString("alter table VERSION add column ")+
      "GNMD_TIMESTAMP datetime after DB";
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }



  //
  // *** End of schema updates ***
  //

  sql=QString("update VERSION set ")+
    QString::asprintf("DB=%d",GLASSNET_SCHEMA_VERSION);
  SqlQuery::run(sql,&ok);

  return ok;
}
