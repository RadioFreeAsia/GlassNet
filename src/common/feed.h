// feed.h
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

#ifndef FEED_H
#define FEED_H

#include <stdint.h>

#include <QString>

#include "accessor.h"
#include "db.h"

class Feed : public Accessor
{
 public:
  Feed(int id);
  int id() const;
  bool exists() const;
  QString name() const;
  void setName(const QString &name);
  QString url() const;
  void setUrl(const QString &str);
  QString remarks() const;
  void setRemarks(const QString &str) const;
  static int create();
  static void remove(int feed_id);
  static bool exists(int feed_id);

 private:
  QString tableName() const;
  QString whereClause() const;
  int feed_id;
};


#endif  // FEED_H
