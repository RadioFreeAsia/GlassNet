// config.h
//
// Class for gncd(8) configuration.
//
// (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
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

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

#include "common.h"

#define GNCD_CONF_FILE QString("/etc/gncd.conf")
#define GNCD_DEFAULT_ALSA_DEVICE "hw:0"
#define GNCD_DEFAULT_AUDIO_DEVICE "ALSA"
#define GNCD_DEFAULT_DB_NAME "/var/gncd/gncd.db"
#define GNCD_DEFAULT_INTERFACE "eth0"

class Config
{
 public:
  Config();
  QString alsaDevice() const;
  void setAlsaDevice(const QString &str);
  QString audioDevice() const;
  void setAudioDevice(const QString &str);
  QString dbName() const;
  void setDbName(const QString &str);
  unsigned commandPort() const;
  void setCommandPort(unsigned port);
  QString callbackHostname() const;
  void setCallbackHostname(const QString &str);
  unsigned callbackPort() const;
  void setCallbackPort(unsigned port);
  QString networkInterface() const;
  void setNetworkInterface(const QString &str);
  void load();
  void save() const;

 private:
  QString config_alsa_device;
  QString config_audio_device;
  QString config_db_name;
  QString config_network_interface;
  unsigned config_command_port;
  QString config_callback_hostname;
  unsigned config_callback_port;
};


#endif  // CONFIG_H
