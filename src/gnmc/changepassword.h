// changepassword.h
//
// Password Set dialog for GlassNet
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

#ifndef CHANGEPASSWORD_H
#define CHANGEPASSWORD_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>

class ChangePassword : public QDialog
{
  Q_OBJECT
 public:
  ChangePassword(QWidget *parent=0);
  ~ChangePassword();
  QSize sizeHint() const;
  
 public slots:
  int exec(int user_id);

 private slots:
  void textChangedData(const QString &str);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_password_label[2];
  QLineEdit *edit_password_edit[2];
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  int edit_user_id;
};


#endif  // CHANGEPASSWORD_H
