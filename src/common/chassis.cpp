// chassis.cpp
//
// Abstract a GlassNet chassis.
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

#include "chassis.h"

Chassis::Chassis(int id)
  : Accessor()
{
  chassis_id=id;
}


Chassis::Chassis(int site_id,int slot)
{
  chassis_id=-1;

  QString sql=QString("select ID from CHASSIS where ")+
    QString().sprintf("SITE_ID=%d and SLOT=%d",site_id,slot);
  SqlQuery *q=new SqlQuery(sql);
  if(q->first()) {
    chassis_id=q->value(0).toInt();
  }
  delete q;
}


int Chassis::id() const
{
  return chassis_id;
}


bool Chassis::exists() const
{
  return Chassis::exists(chassis_id);
}


int Chassis::siteId() const
{
  return getRow("SITE_ID").toInt();
}


void Chassis::setSiteId(int id) const
{
  setRow("SITE_ID",id);
}


int Chassis::slot() const
{
  return getRow("SLOT").toInt();
}


void Chassis::setSlot(int slot) const
{
  setRow("SLOT",slot);
}


Chassis::Type Chassis::type() const
{
  return (Chassis::Type)getRow("TYPE").toInt();
}


void Chassis::setType(Chassis::Type type) const
{
  setRow("TYPE",(int)type);
}


QString Chassis::serialNumber() const
{
  return getRow("SERIAL_NUMBER").toString();
}


void Chassis::setSerialNumber(const QString &str) const
{
  setRow("SERIAL_NUMBER",str);
}


QString Chassis::description() const
{
  if(type()==Chassis::TypeNone) {
    return QString("[none]");
  }
  return Chassis::typeString(type())+" ["+serialNumber()+"]";
}


QString Chassis::typeString(Type type)
{
  QString ret="Unknown";

  switch(type) {
  case Chassis::TypeNone:
    ret="[none]";
    break;

  case Chassis::TypeSolo:
    ret="Solo";
    break;

  case Chassis::TypeProto1:
    ret="Prototype 1";
    break;

  case Chassis::TypeSoloXlr:
    ret="Solo XLR";
    break;

  case Chassis::TypeLast:
    break;
  }

  return ret;
}


int Chassis::slotQuantity(Type type)
{
  int ret=0;

  switch(type) {
  case Chassis::TypeNone:
    ret=0;
    break;

  case Chassis::TypeSolo:
  case Chassis::TypeSoloXlr:
    ret=1;
    break;

  case Chassis::TypeProto1:
    ret=4;
    break;

  case Chassis::TypeLast:
    break;
  }

  return ret;
}


int Chassis::create(Chassis::Type type,const QString &serial)
{
  QString sql=QString("insert into CHASSIS set ")+
    QString().sprintf("TYPE=%d,",type)+
    "SERIAL_NUMBER='"+SqlQuery::escape(serial)+"'";
  return SqlQuery::run(sql).toInt();
}


void Chassis::remove(int chassis_id)
{
  QString sql=QString("delete from CHASSIS where ")+
    QString().sprintf("ID=%d",chassis_id);
  SqlQuery::run(sql);
}


bool Chassis::exists(int chassis_id)
{
  QString sql=QString("select ID from CHASSIS where ")+
    QString().sprintf("ID=%d",chassis_id);
  return SqlQuery::rows(sql)>0;
}


bool Chassis::exists(int site_id,int slot)
{
  QString sql=QString("select ID from CHASSIS where ")+
    QString().sprintf("SITE_ID=%d and SLOT=%d",site_id,slot);
  return SqlQuery::rows(sql)>0;
}


QString Chassis::tableName() const
{
  return QString("CHASSIS");
}


QString Chassis::whereClause() const
{
  return QString().sprintf("ID=%d",chassis_id);
}
