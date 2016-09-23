// login.cpp
//
// Login dialog for GlassNet
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

#include "db.h"
#include "login.h"

Login::Login(QWidget *parent)
  : QDialog(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());
  setWindowTitle(tr("GlassNet - Login"));

  //
  // Username
  //
  edit_username_label=new QLabel(tr("Username")+":",this);
  edit_username_label->setFont(bold_font);
  edit_username_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_username_edit=new QLineEdit(this);
  edit_username_edit->setMaxLength(255);

  //
  // Password
  //
  edit_password_label=new QLabel(tr("Password")+":",this);
  edit_password_label->setFont(bold_font);
  edit_password_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_password_edit=new QLineEdit(this);
  edit_password_edit->setMaxLength(255);
  edit_password_edit->setEchoMode(QLineEdit::Password);

  //
  // Ok Button
  //
  edit_ok_button=new QPushButton(tr("OK"),this);
  edit_ok_button->setFont(bold_font);
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  edit_cancel_button=new QPushButton(tr("Cancel"),this);
  edit_cancel_button->setFont(bold_font);
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


Login::~Login()
{
}


QSize Login::sizeHint() const
{
  return QSize(250,125);
}


int Login::exec(int *user_id)
{
  edit_user_id=user_id;
  return QDialog::exec();
}


void Login::okData()
{
  bool ok=false;

  QString sql=QString("select ID from USERS where ")+
    "USERNAME='"+SqlQuery::escape(edit_username_edit->text())+"' && "+
    "PASSWORD='"+SqlQuery::escape(edit_password_edit->text())+"'";
  SqlQuery *q=new SqlQuery(sql);
  if((ok=q->first())) {
    *edit_user_id=q->value(0).toInt();
  }
  delete q;
  if(ok) {
    done(true);
  }
  else {
    QMessageBox::information(this,tr("GlassNet - Login"),
			     tr("Invalid username or password."));
  }
}


void Login::cancelData()
{
  done(false);
}


void Login::resizeEvent(QResizeEvent *e)
{
  edit_username_label->setGeometry(10,10,100,20);
  edit_username_edit->setGeometry(115,10,size().width()-140,20);

  edit_password_label->setGeometry(10,32,100,20);
  edit_password_edit->setGeometry(115,32,size().width()-140,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
