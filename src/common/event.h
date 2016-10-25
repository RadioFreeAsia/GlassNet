// event.h
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

#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

#include <QString>

#include "accessor.h"
#include "db.h"

class Event : public Accessor
{
 public:
  Event(int id);
  int id() const;
  bool exists() const;
  int siteId() const;
  void setSiteId(int id) const;
  int chassisSlot() const;
  void setChassisSlot(int slot) const;
  int receiverSlot() const;
  void setReceiverSlot(int slot) const;
  bool isPosted() const;
  void setPosted(bool state) const;
  QTime startTime() const;
  void setStartTime(const QTime &time) const;
  int length() const;
  void setLength(int msecs) const;
  bool dowActive(int dow) const;
  void setDowActive(int dow,bool state) const;
  QString url() const;
  void setUrl(const QString &url) const;
  bool receiverExists() const;
  bool receiverIsOnline() const;
  static int create(int site_id,int chassis_slot,int receiver_slot);
  static void remove(int id);
  static bool exists(int id);
  static bool receiverExists(int site_id,int chassis_slot,int receiver_slot);

 private:
  QString tableName() const;
  QString whereClause() const;
  int event_id;
};


#endif  // EVENT_H
