// accessor.h
//
// Abstract base class for a DB object accessor.
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

#ifndef ACCESSOR_H
#define ACCESSOR_H

#include <QDateTime>
#include <QString>
#include <QVariant>

class Accessor
{
 public:
  Accessor();
  virtual ~Accessor();

 protected:
  virtual QString tableName() const=0;
  virtual QString whereClause() const=0;
  virtual QVariant getRow(const QString &column) const;
  virtual void setRow(const QString &column,const QString &value) const;
  virtual void setRow(const QString &column,int value) const;
  virtual void setRow(const QString &column,unsigned value) const;
  virtual void setRow(const QString &column,bool value) const;
  virtual void setRow(const QString &column,const QTime &time) const;
  virtual void setRow(const QString &column,const QDate &date) const;
  virtual void setRow(const QString &column,const QDateTime &dt) const;
};


#endif  // ACCESSOR_H
