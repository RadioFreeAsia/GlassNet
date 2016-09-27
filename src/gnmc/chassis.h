// chassis.h
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

#ifndef CHASSIS_H
#define CHASSIS_H

#include <QString>

#include "accessor.h"
#include "db.h"

#define MAX_CHASSIS_SLOTS 4

class Chassis : public Accessor
{
 public:
  enum Type {TypeNone=0,TypeSolo=1,TypeProto1=2,TypeLast=3};
  Chassis(int id);
  Chassis(int site_id,int slot);
  int id() const;
  bool exists() const;
  int siteId() const;
  void setSiteId(int id) const;
  int slot() const;
  void setSlot(int slot) const;
  Type type() const;
  void setType(Type type) const;
  QString serialNumber() const;
  void setSerialNumber(const QString &str) const;
  QString description() const;
  static QString typeString(Type type);
  static int slotQuantity(Type type);
  static int create(Type type,const QString &serial);
  static void remove(int chassis_id);
  static bool exists(int chassis_id);
  static bool exists(int site_id,int slot);

 private:
  QString tableName() const;
  QString whereClause() const;
  int chassis_id;
};


#endif  // CHASSIS_H
