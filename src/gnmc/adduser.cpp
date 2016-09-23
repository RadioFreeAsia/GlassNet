// adduser.cpp
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

#include <QMessageBox>

#include "adduser.h"
#include "user.h"

AddUser::AddUser(QWidget *parent)
  : QDialog(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);
  setMinimumSize(sizeHint());
  setWindowTitle(tr("GlassNet - Add User"));

  //
  // Username
  //
  add_username_label=new QLabel(tr("Username")+":",this);
  add_username_label->setFont(bold_font);
  add_username_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  add_username_edit=new QLineEdit(this);
  add_username_edit->setMaxLength(255);

  //
  // Ok Button
  //
  add_ok_button=new QPushButton(tr("OK"),this);
  add_ok_button->setFont(bold_font);
  connect(add_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  add_cancel_button=new QPushButton(tr("Cancel"),this);
  add_cancel_button->setFont(bold_font);
  connect(add_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddUser::~AddUser()
{
}


QSize AddUser::sizeHint() const
{
  return QSize(350,110);
}


int AddUser::exec(int *user_id)
{
  add_user_id=user_id;

  return QDialog::exec();
}


void AddUser::okData()
{
  if((*add_user_id=User::create(add_username_edit->text()))<0) {
    QMessageBox::warning(this,tr("GlassNet - Error"),
			 tr("User")+" \""+add_username_edit->text()+"\" "+
			 tr("already exists."));
    return;
  }
  done(true);
}


void AddUser::cancelData()
{
  done(false);
}


void AddUser::resizeEvent(QResizeEvent *e)
{
  add_username_label->setGeometry(10,10,100,20);
  add_username_edit->setGeometry(115,10,size().width()-140,20);

  add_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  add_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
