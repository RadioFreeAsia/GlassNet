// gnmc.h
//
// gnmc(1) Management client for GlassNet
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

#ifndef GNMC_H
#define GNMC_H

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>

#include "listsites.h"
#include "listusers.h"
#include "login.h"
#include "mysqlconfig.h"

class MainWidget : public QMainWindow
{
 Q_OBJECT;
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
  void usersData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  bool CheckDb(QString *err_msg);
  bool CreateDb();
  bool CheckSchema();
  Login *gnmc_login_dialog;
  ListSites *gnmc_listsites_dialog;
  ListUsers *gnmc_listusers_dialog;
  QLabel *gnmc_name_label;
  QLabel *gnmc_description_label;
  QPushButton *gnmc_users_button;
  QPushButton *gnmc_sites_button;
  QPushButton *gnmc_events_button;
  QPushButton *gnmc_close_button;
  int gnmc_user_id;
  Config *gnmc_config;
};


#endif  // GNMC_H
