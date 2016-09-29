// config.cpp
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

#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <QSettings>

#include "config.h"

Config::Config()
{
}


QString Config::alsaDevice() const
{
  return config_alsa_device;
}


void Config::setAlsaDevice(const QString &str)
{
  config_alsa_device=str;
}


QString Config::audioDevice() const
{
  return config_audio_device;
}


void Config::setAudioDevice(const QString &str)
{
  config_audio_device=str;
}


QString Config::dbName() const
{
  return config_db_name;
}


void Config::setDbName(const QString &str)
{
  config_db_name=str;
}


unsigned Config::commandPort() const
{
  return config_command_port;
}


void Config::setCommandPort(unsigned port)
{
  config_command_port=port;
}


QString Config::callbackHostname() const
{
  return config_callback_hostname;
}


void Config::setCallbackHostname(const QString &str)
{
  config_callback_hostname=str;
}


unsigned Config::callbackPort() const
{
  return config_callback_port;
}


void Config::setCallbackPort(unsigned port)
{
  config_callback_port=port;
}


QString Config::networkInterface() const
{
  return config_network_interface;
}


void Config::setNetworkInterface(const QString &str)
{
  config_network_interface=str;
}


void Config::load()
{
  QSettings s(GNCD_CONF_FILE,QSettings::IniFormat);

  config_alsa_device=s.value("AlsaDevice",GNCD_DEFAULT_ALSA_DEVICE).toString();
  config_audio_device=
    s.value("AudioDevice",GNCD_DEFAULT_AUDIO_DEVICE).toString();
  config_db_name=s.value("DbName",GNCD_DEFAULT_DB_NAME).toString();
  config_network_interface=
    s.value("NetworkInterface",GNCD_DEFAULT_INTERFACE).toString();
  config_command_port=
    s.value("CommandPort",GLASSNET_RECEIVER_COMMAND_PORT).toUInt();
  config_callback_hostname=
    s.value("CallbackHostname",GLASSNET_RECEIVER_CALLBACK_HOSTNAME).toString();
  config_callback_port=
    s.value("CallbackPort",GLASSNET_RECEIVER_CALLBACK_PORT).toUInt();
}


void Config::save() const
{
  QSettings s(GNCD_CONF_FILE,QSettings::IniFormat);
  s.setValue("AlsaDevice",config_alsa_device);
  s.setValue("AudioDevice",config_audio_device);
  s.setValue("DbName",config_db_name);
  s.setValue("CommandPort",config_command_port);
  s.setValue("NetworkInterface",config_network_interface);
}
