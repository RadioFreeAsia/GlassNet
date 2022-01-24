// feed.cpp
//
// Abstract a GlassNet feed.
//
//   (C) Copyright 2016-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QStringList>

#include "event.h"
#include "feed.h"

Feed::Feed(int id)
{
  feed_id=id;
}


int Feed::id() const
{
  return feed_id;
}


bool Feed::exists() const
{
  return Feed::exists(feed_id);
}


QString Feed::name() const
{
  return getRow("NAME").toString();
}


void Feed::setName(const QString &name)
{
  setRow("NAME",name);
}


QString Feed::url() const
{
  return getRow("URL").toString();
}


void Feed::setUrl(const QString &str)
{
  setRow("URL",str);
}


QString Feed::remarks() const
{
  return getRow("REMARKS").toString();
}


void Feed::setRemarks(const QString &str) const
{
  setRow("REMARKS",str);
}


int Feed::create()
{
  QString sql=QString("insert into `FEEDS` set ")+
    "`NAME`=\"[new feed]\"";
  return SqlQuery::run(sql).toInt();
}


void Feed::remove(int feed_id)
{
  QString sql=QString("select `ID` from `EVENTS` where ")+
    QString::asprintf("`FEED_ID`=%d",feed_id);
  QSqlQuery *q=new QSqlQuery(sql);
  while(q->next()) {
    Event::remove(q->value(0).toInt());
  }

  sql=QString("delete from `FEEDS` where ")+
    QString::asprintf("`ID`=%d",feed_id);
  SqlQuery::run(sql);
}


bool Feed::exists(int feed_id)
{
  QString sql=QString("select `ID` from `FEEDS` where ")+
    QString::asprintf("`ID`=%d",feed_id);
  return SqlQuery::rows(sql)>0;
}


QString Feed::tableName() const
{
  return QString("FEEDS");
}


QString Feed::whereClause() const
{
  return QString::asprintf("`ID`=%d",feed_id);
}
