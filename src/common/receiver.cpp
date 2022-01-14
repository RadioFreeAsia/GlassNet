// receiver.cpp
//
// Abstract a GlassNet receiver.
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

#include <QStringList>

#include "receiver.h"

Receiver::Receiver(int id)
{
  receiver_id=id;
}


Receiver::Receiver(int chassis_id,int slot)
{
  receiver_id=-1;

  QString sql=QString("select ID from RECEIVERS where ")+
    QString::asprintf("CHASSIS_ID=%d and SLOT=%d",chassis_id,slot);
  SqlQuery *q=new SqlQuery(sql);
  if(q->first()) {
    receiver_id=q->value(0).toInt();
  }
  delete q;
}


Receiver::Receiver(const QString &mac)
{
  receiver_id=-1;

  QString sql=QString("select ID from RECEIVERS where ")+
    "MAC_ADDRESS='"+mac+"'";
  SqlQuery *q=new SqlQuery(sql);
  if(q->first()) {
    receiver_id=q->value(0).toInt();
  }
  delete q;
}


int Receiver::id() const
{
  return receiver_id;
}


bool Receiver::exists() const
{
  return Receiver::exists(receiver_id);
}


int Receiver::chassisId() const
{
  return getRow("CHASSIS_ID").toInt();
}


void Receiver::setChassisId(int id) const
{
  setRow("CHASSIS_ID",id);
}


int Receiver::slot() const
{
  return getRow("SLOT").toInt();
}


void Receiver::setSlot(int slot) const
{
  setRow("SLOT",slot);
}


Receiver::Type Receiver::type() const
{
  return (Receiver::Type)getRow("TYPE").toInt();
}


void Receiver::setType(Receiver::Type type) const
{
  setRow("TYPE",(int)type);
}


bool Receiver::isOnline() const
{
  return getRow("ONLINE").toBool();
}


void Receiver::setOnline(bool state) const
{
  setRow("ONLINE",state);
}


QString Receiver::macAddress() const
{
  return getRow("MAC_ADDRESS").toString();
}


void Receiver::setMacAddress(const QString &mac) const
{
  setRow("MAC_ADDRESS",mac.toUpper());
}


int Receiver::activeGuid() const
{
  return getRow("ACTIVE_GUID").toInt();
}


void Receiver::setActiveGuid(int guid) const
{
  setRow("ACTIVE_GUID",guid);
}


bool Receiver::updateFirmware() const
{
  return getRow("UPDATE_FIRMWARE").toBool();
}


void Receiver::setUpdateFirmware(bool state) const
{
  setRow("UPDATE_FIRMWARE",state);
}


QString Receiver::description() const
{
  if(type()==Receiver::TypeNone) {
    return Receiver::typeString(type());
  }
  return Receiver::typeString(type())+" ["+macAddress()+"]";
}


bool Receiver::isMacAddress(const QString &mac)
{
  bool ok=false;
  QString addr=mac;
  addr.replace(":","");
  if(addr.length()!=12) {
    return false;
  }
  addr.toLongLong(&ok,16);
  return ok;
}


QString Receiver::typeString(Type type)
{
  QString ret="Unknown";

  switch(type) {
  case Receiver::TypeNone:
    ret="[empty]";
    break;

  case Receiver::TypeRaspPi2:
    ret="Raspberry Pi 2";
    break;

  case Receiver::TypeRaspPi3:
    ret="Raspberry Pi 3";
    break;

  case Receiver::TypeRaspPi3Plus:
    ret="Raspberry Pi 3+";
    break;

  case Receiver::TypeDevPlatform:
    ret="Development Platform";
    break;

  case Receiver::TypeLast:
    break;
  }

  return ret;
}


int Receiver::create(Type type,const QString &mac)
{
  QString sql=QString("insert into RECEIVERS set ")+
    QString::asprintf("TYPE=%d,",type)+
    "MAC_ADDRESS='"+mac+"'";
  return SqlQuery::run(sql).toInt();
}


void Receiver::remove(int receiver_id)
{
  QString sql=QString("delete from RECEIVERS where ")+
    QString::asprintf("ID=%d",receiver_id);
  SqlQuery::run(sql);
}


bool Receiver::exists(int receiver_id)
{
  QString sql=QString("select ID from RECEIVERS where ")+
    QString::asprintf("ID=%d",receiver_id);
  return SqlQuery::rows(sql)>0;
}


QString Receiver::tableName() const
{
  return QString("RECEIVERS");
}


QString Receiver::whereClause() const
{
  return QString::asprintf("ID=%d",receiver_id);
}
