// user.h
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

#ifndef USER_H
#define USER_H

#include <QString>

#include "accessor.h"
#include "db.h"

class User : public Accessor
{
 public:
  User(int id);
  User(const QString &username);
  int id() const;
  QString userName() const;
  bool exists() const;
  bool passwordIsValid(const QString &passwd) const;
  void setPassword(const QString &passwd) const;
  QString fullName() const;
  void setFullName(const QString &str) const;
  QString description() const;
  void setDescription(const QString &str) const;
  bool userPriv() const;
  void setUserPriv(bool state) const;
  bool sitePriv() const;
  void setSitePriv(bool state) const;
  bool eventPriv() const;
  void setEventPriv(bool state) const;
  static int create(const QString &username);
  static void remove(const QString &username);
  static void remove(int user_id);
  static bool exists(const QString &username);
  static bool exists(int user_id);

 private:
  QString tableName() const;
  QString whereClause() const;
  int user_id;
};


#endif  // USER_H
