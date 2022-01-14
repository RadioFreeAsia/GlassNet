// crypto.cpp
//
// Cryptographic routines for GlassNet
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

#include <stdlib.h>

#include <openssl/sha.h>

#include "crypto.h"

QString MakePasswordHash(const QString &passwd,const QString &salt)
{
  unsigned char md[SHA_DIGEST_LENGTH];
  QString ret=salt;

  if(ret.isEmpty()) {
    ret=QString::asprintf("%04x",0xFFFF&rand());
  }
  QString phrase=ret+passwd;
  SHA1((const unsigned char *)phrase.toUtf8().data(),
       phrase.length(),md);
  for(int i=0;i<SHA_DIGEST_LENGTH;i++) {
    ret+=QString::asprintf("%02x",0xFF&md[i]);
  }

  return ret;
}


bool ValidatePassword(const QString &passwd,const QString &hash)
{
  return hash==MakePasswordHash(passwd,hash.left(4));
}
