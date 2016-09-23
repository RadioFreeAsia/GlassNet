// edituser.h
//
// Edit a GlassNet User
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

#ifndef EDITUSER_H
#define EDITUSER_H

#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>

#include "changepassword.h"

class EditUser : public QDialog
{
  Q_OBJECT
 public:
  EditUser(QWidget *parent=0);
  ~EditUser();
  QSize sizeHint() const;
  
 public slots:
  int exec(int user_id);

 private slots:
  void passwordData();
  void userPrivToggled(bool state);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_username_label;
  QLineEdit *edit_username_edit;
  QLabel *edit_fullname_label;
  QLineEdit *edit_fullname_edit;
  QLabel *edit_description_label;
  QLineEdit *edit_description_edit;
  QCheckBox *edit_users_check;
  QLabel *edit_users_label;
  QCheckBox *edit_sites_check;
  QLabel *edit_sites_label;
  QCheckBox *edit_events_check;
  QLabel *edit_events_label;
  QPushButton *edit_password_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  ChangePassword *edit_changepassword_dialog;
  int edit_user_id;
};


#endif  // EDITUSER_H
