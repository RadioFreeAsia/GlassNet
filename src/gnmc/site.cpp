// site.cpp
//
// Abstract a GlassNet site.
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

#include <QObject>

#include "db.h"
#include "site.h"

Site::Site(int id)
{
  site_id=id;
}


int Site::id() const
{
  return site_id;
}


bool Site::exists() const
{
  return Site::exists(site_id);
}


QString Site::siteName() const
{
  return getRow("NAME").toString();
}


void Site::setSiteName(const QString &str) const
{
  setRow("NAME",str);
}


int Site::create()
{
  QString sql=QString("insert into SITES set ")+
    "NAME='"+SqlQuery::escape(QObject::tr("[new site]"))+"'";
  return SqlQuery::run(sql).toInt();
}


void Site::remove(int site_id)
{
  QString sql=QString("delete from SITES where ")+
    QString().sprintf("ID=%d",site_id);
  SqlQuery::run(sql);
}


bool Site::exists(int site_id)
{
  QString sql=QString("select ID from SITES where ")+
    QString().sprintf("ID=%d",site_id);
  return SqlQuery::rows(sql)>0;
}


QString Site::tableName() const
{
  return QString("SITES");
}


QString Site::whereClause() const
{
  return QString().sprintf("ID=%d",site_id);
}
