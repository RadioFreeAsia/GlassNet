// tzmap.cpp
//
// Map a timezone definition file from the TZ database.
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <QList>
#include <QTime>

#include "tzmap.h"

TzType::TzType(const QDateTime &start_datetime)
{
  tzt_start_datetime=start_datetime;
  tzt_offset=0;
  tzt_name="???";
}


QDateTime TzType::startDateTime() const
{
  return tzt_start_datetime;
}


int64_t TzType::offset() const
{
  return tzt_offset;
}


void TzType::setOffset(int64_t msec)
{
  tzt_offset=msec;
}


QString TzType::name() const
{
  return tzt_name;
}


void TzType::setName(const QString &str)
{
  tzt_name=str;
}


QString TzType::dump() const
{
  return startDateTime().toString("yyyy-MM-dd hh:mm:ss")+
    " name: "+name()+
    QString().sprintf(" offset: %ld",offset());
}




TzMap::TzMap()
{
}


QString TzMap::name() const
{
  return tzh_name;
}


QDateTime TzMap::convert(const QDateTime &utc,QString *tz_name) const
{
  int line=tzh_types.size()-1;

  if(tzh_types.size()==0) {
    if(tz_name!=NULL) {
      *tz_name=name();
    }
    return utc;
  }
  for(int i=0;i<tzh_types.size();i++) {
    if(utc<tzh_types.at(i)->startDateTime()) {
      line=i-1;
      if(line<0) {
	line=0;
      }
      if(tz_name!=NULL) {
	*tz_name=tzh_types.at(line)->name();
      }
      return utc.addMSecs(1000*tzh_types.at(line)->offset());
    }
  }
  if(tz_name!=NULL) {
    *tz_name=tzh_types.at(line)->name();
  }
  return utc.addMSecs(1000*tzh_types.at(line)->offset());
}


bool TzMap::load(const QString &filename,bool dump)
{
  int fd=-1;
  char accum[16];
  char version=0;

  QStringList f0=filename.split("/");
  tzh_name=f0.back();

  if(dump) {
    printf("reading zone \"%s\":\n",filename.toUtf8().constData());
  }

  //
  // Open the file
  //
  if((fd=open(filename.toUtf8(),O_RDONLY))<0) {
    fprintf(stderr,"unable to open %s [%s]\n",
	    filename.toUtf8().constData(),strerror(errno));
    return false;
  }

  //
  // Parse the header
  if(read(fd,accum,5)!=5) {
    close(fd);
    fprintf(stderr,"file truncated\n");
    return false;
  }
  if(strncmp(accum,"TZif",4)!=0) {
    close(fd);
    fprintf(stderr,"not a TZ file\n");
    return false;
  }
  version=0xFF&accum[4];
  if(version=='0') {
    ReadVersion0Block(fd,dump); 
   return true;
  }
  if(version=='2') {
    ReadVersion0Block(fd,dump);
    for(int i=0;i<tzh_types.size();i++) {
      delete tzh_types.at(i);
    }
    tzh_types.clear();
    lseek(fd,5,SEEK_CUR);
    ReadVersion2Block(fd,dump);
    return true;
  }

  fprintf(stderr,"unrecognized file version [%c]\n",version);
  return false;
}


QString TzMap::localTzid(bool *ok)
{
  QString tzid;

  if(ok!=NULL) {
    *ok=false;
  }
  char destpath[PATH_MAX];
  memset(destpath,0,PATH_MAX);
  ssize_t n=readlink(TZMAP_LOCALTIME_LINK.toUtf8(),destpath,PATH_MAX-1);
  if(n>0) {
    destpath[n]=0;
    QString raw_destpath(destpath);
    int index=raw_destpath.indexOf(TZMAP_ZONEINFO_DIR);
    if(index>=0) {
      index+=TZMAP_ZONEINFO_DIR.length();
      tzid=raw_destpath.right(raw_destpath.length()-index-1);
      if(ok!=NULL) {
	*ok=true;
      }
    }
  }

  return tzid;
}


void TzMap::ReadVersion0Block(int fd,bool dump)
{
  uint32_t tzh_ttisgmtcnt;
  uint32_t tzh_ttisstdcnt;
  uint32_t tzh_leapcnt;
  uint32_t tzh_timecnt;
  uint32_t tzh_typecnt;
  uint32_t tzh_charcnt;
  char *names=NULL;

  lseek(fd,15,SEEK_CUR);

  tzh_ttisgmtcnt=ReadUnsigned(fd);
  tzh_ttisstdcnt=ReadUnsigned(fd);
  tzh_leapcnt=ReadUnsigned(fd);
  tzh_timecnt=ReadUnsigned(fd);
  tzh_typecnt=ReadUnsigned(fd);
  tzh_charcnt=ReadUnsigned(fd);

  //
  // Transition Times
  //
  for(uint32_t i=0;i<tzh_timecnt;i++) {
    tzh_types.push_back(new TzType(DateTimeFromTimeT(ReadSigned(fd))));
  }

  QList<uint8_t> type_indices;
  for(uint32_t i=0;i<tzh_timecnt;i++) {
    type_indices.push_back(ReadByte(fd));
  }

  QList<int32_t> offsets;
  QList<uint8_t> name_indices;
  for(uint32_t i=0;i<tzh_typecnt;i++) {
    offsets.push_back(ReadSigned(fd));
    ReadByte(fd);  // tt_isdst, not needed
    name_indices.push_back(ReadByte(fd));
  }

  names=new char[tzh_charcnt];
  read(fd,names,tzh_charcnt);
  //  QStringList names=ReadStrings(fd);

  lseek(fd,tzh_leapcnt*8,SEEK_CUR);  // leap seconds, not needed
  lseek(fd,tzh_ttisstdcnt,SEEK_CUR); // standard indicators, not needed
  lseek(fd,tzh_ttisgmtcnt,SEEK_CUR); // DST indicators, not needed

  //
  // Put it all together
  //
  for(int i=0;i<tzh_types.size();i++) {
    tzh_types.at(i)->setOffset(offsets.at(type_indices.at(i)));
    tzh_types.at(i)->setName(names+(name_indices.at(type_indices.at(i))));
    //    tzh_types.at(i)->setName(names.at(type_indices.at(i)));
    if(dump) {
      printf("TYPE0[%u]: %s\n",i,tzh_types.at(i)->dump().toUtf8().constData());
    }
  }
}


void TzMap::ReadVersion2Block(int fd,bool dump)
{
  uint32_t tzh_ttisgmtcnt;
  uint32_t tzh_ttisstdcnt;
  uint32_t tzh_leapcnt;
  uint32_t tzh_timecnt;
  uint32_t tzh_typecnt;
  uint32_t tzh_charcnt;
  char *names=NULL;

  lseek(fd,15,SEEK_CUR);

  tzh_ttisgmtcnt=ReadUnsigned(fd);
  tzh_ttisstdcnt=ReadUnsigned(fd);
  tzh_leapcnt=ReadUnsigned(fd);
  tzh_timecnt=ReadUnsigned(fd);
  //  printf("tzh_timecnt: %u\n",tzh_timecnt);
  tzh_typecnt=ReadUnsigned(fd);
  //  printf("tzh_typecnt: %u\n",tzh_typecnt);
  tzh_charcnt=ReadUnsigned(fd);
  //lseek(fd,4,SEEK_CUR);

  //
  // Transition Times (tzh_timecnt)
  //
  //  printf("POS1: 0x%08X\n",lseek(fd,0,SEEK_CUR));
  for(uint32_t i=0;i<tzh_timecnt;i++) {
    tzh_types.push_back(new TzType(DateTimeFromTimeT(ReadLongSigned(fd))));
  }

  //
  // Local Time Types Index (tzh_timecnt)
  //
  //  printf("POS2: 0x%08X\n",lseek(fd,0,SEEK_CUR));
  QList<uint8_t> type_indices;
  for(uint32_t i=0;i<tzh_timecnt;i++) {
    type_indices.push_back(ReadByte(fd));
  }

  //
  // Time Types (tzh_typecnt)
  //
  //  printf("POS3: 0x%08X\n",lseek(fd,0,SEEK_CUR));
  QList<int32_t> offsets;
  QList<uint8_t> name_indices;
  for(uint32_t i=0;i<tzh_typecnt;i++) {
    offsets.push_back(ReadSigned(fd));
    ReadByte(fd);  // tt_isdst, not needed
    name_indices.push_back(ReadByte(fd));
    //    printf("[%d] offset: %lu  name: %u\n",i,offsets.back(),name_indices.back());
  }

  //  printf("POS4: 0x%08X\n",lseek(fd,0,SEEK_CUR));
  
  names=new char[tzh_charcnt];
  read(fd,names,tzh_charcnt);
  //  QStringList names=ReadStrings(fd);
  //  printf("tzh_charcnt: %u  strings: %d\n",tzh_charcnt,names.size());

  //  printf("POS5: 0x%08X\n",lseek(fd,0,SEEK_CUR));
  lseek(fd,tzh_leapcnt*16,SEEK_CUR);  // leap seconds, not needed
  lseek(fd,tzh_ttisstdcnt,SEEK_CUR); // standard indicators, not needed
  lseek(fd,tzh_ttisgmtcnt,SEEK_CUR); // DST indicators, not needed

  //
  // Put it all together
  //
  for(int i=0;i<tzh_types.size();i++) {
    tzh_types.at(i)->setOffset(offsets.at(type_indices.at(i)));
    tzh_types.at(i)->setName(names+(name_indices.at(type_indices.at(i))));

    if(dump) {
      printf("TYPE2[%u]: %s\n",i,tzh_types.at(i)->dump().toUtf8().constData());
    }
  }
}


QDateTime TzMap::DateTimeFromTimeT(int64_t time) const
{
  return QDateTime(QDate(1970,1,1)).addMSecs(1000*time);
}


uint32_t TzMap::ReadUnsigned(int fd) const
{
  uint32_t accum;

  read(fd,(char *)(&accum),4);

  return htonl(accum);
}


int32_t TzMap::ReadSigned(int fd) const
{
  int32_t accum;

  read(fd,(char *)(&accum),4);

  return htonl(accum);
}


int64_t TzMap::ReadLongSigned(int fd) const
{
  int64_t accum;

  read(fd,(char *)(&accum),8);

  return ntohll(accum);
}


uint8_t TzMap::ReadByte(int fd) const
{
  uint8_t accum;

  read(fd,(char *)(&accum),1);

  return accum;
}


QStringList TzMap::ReadStrings(int fd)
{
  char data[2];
  QString accum="";
  QStringList ret;

  while(read(fd,data,1)>0) {
    if((0xFF&data[0])==0) {
      if(accum.isEmpty()) {
	return ret;
      }
      ret.push_back(accum);
      accum="";
    }
    else {
      accum+=0xff&data[0];
    }
  }
  fprintf(stderr,"ReadString() ran off the end of the file\n");
  return ret;
}


uint64_t TzMap::ntohll(uint64_t lword) const
{
  uint64_t ret=lword;
  uint16_t u16=12345;

  if(u16!=htons(u16)) {
    ret=((0x00000000000000ff&lword)<<56)+
      ((0x000000000000ff00&lword)<<40)+
      ((0x0000000000ff0000&lword)<<24)+
      ((0x00000000ff000000&lword)<<8)+
      ((0x000000ff00000000&lword)>>8)+
      ((0x0000ff0000000000&lword)>>24)+
      ((0x00ff000000000000&lword)>>40)+
      ((0xff00000000000000&lword)>>56);
  }
  return ret;
}
