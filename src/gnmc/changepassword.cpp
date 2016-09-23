// changepassword.cpp
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

#include "changepassword.h"
#include "crypto.h"
#include "db.h"

ChangePassword::ChangePassword(QWidget *parent)
  : QDialog(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());
  setWindowTitle(tr("GlassNet - Set Password"));

  //
  // Password
  //
  for(int i=0;i<2;i++) {
    edit_password_label[i]=new QLabel(this);
    edit_password_label[i]->setFont(bold_font);
    edit_password_label[i]->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    edit_password_edit[i]=new QLineEdit(this);
    edit_password_edit[i]->setMaxLength(255);
    edit_password_edit[i]->setEchoMode(QLineEdit::Password);
    connect(edit_password_edit[i],SIGNAL(textChanged(const QString &)),
	    this,SLOT(textChangedData(const QString &)));
  }
  edit_password_label[0]->setText(tr("New Password")+":");
  edit_password_label[1]->setText(tr("Repeat Password")+":");

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


ChangePassword::~ChangePassword()
{
}


QSize ChangePassword::sizeHint() const
{
  return QSize(300,130);
}


int ChangePassword::exec(int user_id)
{
  edit_user_id=user_id;

  edit_password_edit[0]->setText("");
  edit_password_edit[1]->setText("");
  textChangedData("");

  return QDialog::exec();
}		      


void ChangePassword::textChangedData(const QString &str)
{
  edit_ok_button->
    setEnabled(edit_password_edit[0]->text()==edit_password_edit[1]->text());
}


void ChangePassword::okData()
{
  QString sql=QString("update USERS set ")+
    "PASSWORD='"+
    SqlQuery::escape(MakePasswordHash(edit_password_edit[0]->text(),"8e8e"))+
    "' where "+
    QString().sprintf("ID=%d",edit_user_id);
  SqlQuery::run(sql);

  done(true);
}


void ChangePassword::cancelData()
{
  done(false);
}


void ChangePassword::resizeEvent(QResizeEvent *e)
{
  edit_password_label[0]->setGeometry(10,10,135,20);
  edit_password_edit[0]->setGeometry(150,10,size().width()-145,20);

  edit_password_label[1]->setGeometry(10,32,135,20);
  edit_password_edit[1]->setGeometry(150,32,size().width()-145,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
