// adduser.h
//
// Add a GlassNet User
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

#ifndef ADDUSER_H
#define ADDUSER_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>

class AddUser : public QDialog
{
  Q_OBJECT
 public:
  AddUser(QWidget *parent=0);
  ~AddUser();
  QSize sizeHint() const;
  
 public slots:
  int exec(int *user_id);

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *add_username_label;
  QLineEdit *add_username_edit;
  QPushButton *add_ok_button;
  QPushButton *add_cancel_button;
  int *add_user_id;
};


#endif  // ADDUSER_H
