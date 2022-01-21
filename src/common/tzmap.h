// tzmap.h
//
// Map a timezone definition file from the TZ database.
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef TZMAP_H
#define TZMAP_H

#include <stdint.h>

#include <QDateTime>
#include <QString>
#include <QStringList>

#define TZMAP_ZONEINFO_DIR QString("/usr/share/zoneinfo")
#define TZMAP_LOCALTIME_LINK QString("/etc/localtime")

class TzType
{
 public:
  TzType(const QDateTime &start_datetime);
  QDateTime startDateTime() const;
  int64_t offset() const;
  void setOffset(int64_t msec);
  QString name() const;
  void setName(const QString &str);
  QString dump() const;

 private:
  QDateTime tzt_start_datetime;
  int64_t tzt_offset;
  QString tzt_name;
};




class TzMap
{
 public:
  TzMap();
  QString name() const;
  QDateTime convert(const QDateTime &utc,QString *tz_name=NULL) const;
  bool load(const QString &filename,bool dump=false);
  static QString localTzid(bool *ok=NULL);

 private:
  void ReadVersion0Block(int fd,bool dump);
  void ReadVersion2Block(int fd,bool dump);
  QDateTime DateTimeFromTimeT(int64_t time) const;
  uint32_t ReadUnsigned(int fd) const;
  int32_t ReadSigned(int fd) const;
  int64_t ReadLongSigned(int fd) const;
  uint8_t ReadByte(int fd) const;
  QStringList ReadStrings(int fd);
  uint64_t ntohll(uint64_t lword) const;
  QList<TzType *> tzh_types;
  QString tzh_name;
};


#endif  // TZMAP_H
