// user.cpp
//
// Abstract a GlassNet user.
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

#include "crypto.h"
#include "user.h"

User::User(int id)
{
  user_id=id;
}


User::User(const QString &username)
{
  QString sql=QString("select ID from USERS where ")+
    "USERNAME='"+SqlQuery::escape(username)+"'";
  SqlQuery *q=new SqlQuery(sql);
  if(q->first()) {
    user_id=q->value(0).toInt();
  }
  else {
    user_id=-1;
  }
  delete q;
}


int User::id() const
{
  return user_id;
}


QString User::userName() const
{
  return getRow("USERNAME").toString();
}


bool User::exists() const
{
  return User::exists(user_id);
}


bool User::passwordIsValid(const QString &passwd) const
{
  return ValidatePassword(passwd,getRow("PASSWORD").toString());
}


void User::setPassword(const QString &passwd) const
{
  setRow("PASSWORD",MakePasswordHash(passwd));
}


QString User::fullName() const
{
  return getRow("FULL_NAME").toString();
}


void User::setFullName(const QString &str) const
{
  setRow("FULL_NAME",str);
}


QString User::description() const
{
  return getRow("DESCRIPTION").toString();
}


void User::setDescription(const QString &str) const
{
  setRow("DESCRIPTION",str);
}


bool User::userPriv() const
{
  return getRow("USER_PRIV").toBool();
}


void User::setUserPriv(bool state) const
{
  setRow("USER_PRIV",state);
}


bool User::receiverPriv() const
{
  return getRow("RECEIVER_PRIV").toBool();
}


void User::setReceiverPriv(bool state) const
{
  setRow("RECEIVER_PRIV",state);
}


bool User::eventPriv() const
{
  return getRow("EVENT_PRIV").toBool();
}


void User::setEventPriv(bool state) const
{
  setRow("EVENT_PRIV",state);
}


int User::create(const QString &username)
{
  if(User::exists(username)) {
    return -1;
  }
  QString sql=QString("insert into USERS set ")+
    "USERNAME='"+SqlQuery::escape(username)+"',"+
    "PASSWORD='"+MakePasswordHash("")+"'";
  return SqlQuery::run(sql).toInt();
}


void User::remove(const QString &username)
{
  QString sql=QString("delete from USERS where ")+
    "USERNAME='"+SqlQuery::escape(username)+"'";
  SqlQuery::run(sql);
}


void User::remove(int user_id)
{
  QString sql=QString("delete from USERS where ")+
    QString().sprintf("ID=%d",user_id);
  SqlQuery::run(sql);
}


bool User::exists(const QString &username)
{
  QString sql=QString("select ID from USERS where ")+
    "USERNAME='"+SqlQuery::escape(username)+"'";
  return SqlQuery::rows(sql)>0;
}


bool User::exists(int user_id)
{
  QString sql=QString("select ID from USERS where ")+
    QString().sprintf("ID=%d",user_id);

  return SqlQuery::rows(sql)>0;
}


QString User::tableName() const
{
  return QString("USERS");
}


QString User::whereClause() const
{
  return QString().sprintf("ID=%d",user_id);
}
