// event.cpp
//
// Abstract a GlassNet Event.
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

#include "event.h"

Event::Event(int id)
{
  event_id=id;
}


int Event::id() const
{
  return event_id;
}


bool Event::exists() const
{
  return Event::exists(event_id);
}


int Event::siteId() const
{
  return getRow("SITE_ID").toInt();
}


void Event::setSiteId(int id) const
{
  setRow("SITE_ID",id);
}


int Event::chassisSlot() const
{
  return getRow("CHASSIS_SLOT").toInt();
}


void Event::setChassisSlot(int slot) const
{
  setRow("CHASSIS_SLOT",slot);
}


int Event::receiverSlot() const
{
  return getRow("RECEIVER_SLOT").toInt();
}


void Event::setReceiverSlot(int slot) const
{
  setRow("RECEIVER_SLOT",slot);
}


bool Event::isPosted() const
{
  return getRow("POSTED").toBool();
}


void Event::setPosted(bool state) const
{
  setRow("POSTED",state);
}


QTime Event::startTime() const
{
  return getRow("START_TIME").toTime();
}


void Event::setStartTime(const QTime &time) const
{
  setRow("START_TIME",time);
}


int Event::length() const
{
  return getRow("LENGTH").toInt();
}


void Event::setLength(int msecs) const
{
  setRow("LENGTH",msecs);
}


bool Event::dowActive(int dow) const
{
  return getRow(QDate::shortDayName(dow).toUpper()).toBool();
}


void Event::setDowActive(int dow,bool state) const
{
  setRow(QDate::shortDayName(dow).toUpper(),state);
}


QString Event::url() const
{
  return getRow("URL").toString();
}


void Event::setUrl(const QString &url) const
{
  setRow("URL",url);
}


bool Event::receiverExists() const
{
  return Event::receiverExists(siteId(),chassisSlot(),receiverSlot());
}


bool Event::receiverIsOnline() const
{
  QString sql=QString("select RECEIVERS.MAC_ADDRESS from ")+
    "RECEIVERS left join CHASSIS "+
    "on RECEIVERS.CHASSIS_ID=CHASSIS.ID left join SITES "+
    "on CHASSIS.SITE_ID=SITES.ID where "+
    QString().sprintf("SITES.ID=%d && ",siteId())+
    QString().sprintf("CHASSIS.SLOT=%d && ",chassisSlot())+
    QString().sprintf("RECEIVERS.SLOT=%d && ",receiverSlot())+
    "RECEIVERS.ONLINE=1";
  return SqlQuery::rows(sql)!=0;
}


int Event::create(int site_id,int chassis_slot,int receiver_slot)
{
  QString sql=QString("insert into EVENTS set ")+
    QString().sprintf("SITE_ID=%d,",site_id)+
    QString().sprintf("CHASSIS_SLOT=%d,",chassis_slot)+
    QString().sprintf("RECEIVER_SLOT=%d,",receiver_slot)+
    "LENGTH=0";
  return SqlQuery::run(sql).toInt();
}


void Event::remove(int id)
{
  QString sql;
  SqlQuery *q=NULL;

  sql=QString("select ")+
    "SITE_ID,"+
    "CHASSIS_SLOT,"+
    "RECEIVER_SLOT "+
    "from EVENTS where "+
    QString().sprintf("ID=%d",id);
  q=new SqlQuery(sql);
  if(q->first()) {
    sql=QString("insert into DELETED_EVENTS set ")+
      QString().sprintf("SITE_ID=%d,",q->value(0).toInt())+
      QString().sprintf("CHASSIS_SLOT=%d,",q->value(1).toInt())+
      QString().sprintf("RECEIVER_SLOT=%d,",q->value(2).toInt())+
      QString().sprintf("EVENT_ID=%d",id);
    SqlQuery::run(sql);

    sql=QString("delete from EVENTS where ")+
      QString().sprintf("ID=%d",id);
    SqlQuery::run(sql);
  }
  delete q;
}


bool Event::exists(int id)
{
  QString sql=QString("select ID from EVENTS where ")+
    QString().sprintf("ID=%d",id);
  return SqlQuery::rows(sql)>0;
}


bool Event::receiverExists(int site_id,int chassis_slot,int receiver_slot)
{
  QString sql=QString("select RECEIVERS.MAC_ADDRESS from ")+
    "RECEIVERS left join CHASSIS "+
    "on RECEIVERS.CHASSIS_ID=CHASSIS.ID left join SITES "+
    "on CHASSIS.SITE_ID=SITES.ID where "+
    QString().sprintf("SITES.ID=%d && ",site_id)+
    QString().sprintf("CHASSIS.SLOT=%d && ",chassis_slot)+
    QString().sprintf("RECEIVERS.SLOT=%d",receiver_slot);
  return SqlQuery::rows(sql)!=0;
}

QString Event::tableName() const
{
  return QString("EVENTS");
}


QString Event::whereClause() const
{
  return QString().sprintf("ID=%d",event_id);
}
