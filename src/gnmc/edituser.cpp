// edituser.cpp
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

#include <QMessageBox>

#include "db.h"
#include "edituser.h"
#include "globals.h"

EditUser::EditUser(QWidget *parent)
  : QDialog(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);
  setMinimumSize(sizeHint());

  //
  // Dialogs
  //
  edit_changepassword_dialog=new ChangePassword(this);

  //
  // Username
  //
  edit_username_label=new QLabel(tr("Username")+":",this);
  edit_username_label->setFont(bold_font);
  edit_username_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_username_edit=new QLineEdit(this);
  edit_username_edit->setReadOnly(true);

  //
  // Fullname
  //
  edit_fullname_label=new QLabel(tr("Full Name")+":",this);
  edit_fullname_label->setFont(bold_font);
  edit_fullname_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_fullname_edit=new QLineEdit(this);
  edit_fullname_edit->setMaxLength(255);

  //
  // Description
  //
  edit_description_label=new QLabel(tr("Description")+":",this);
  edit_description_label->setFont(bold_font);
  edit_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setMaxLength(255);

  //
  // Manage Users
  //
  edit_users_check=new QCheckBox(this);
  connect(edit_users_check,SIGNAL(toggled(bool)),
	  this,SLOT(userPrivToggled(bool)));
  edit_users_label=new QLabel(tr("Manage Users"),this);
  edit_users_label->setFont(bold_font);
  edit_users_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Manage Users
  //
  edit_sites_check=new QCheckBox(this);
  edit_sites_label=new QLabel(tr("Manage Sites"),this);
  edit_sites_label->setFont(bold_font);
  edit_sites_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Manage Feeds
  //
  edit_feeds_check=new QCheckBox(this);
  edit_feeds_label=new QLabel(tr("Manage Feeds"),this);
  edit_feeds_label->setFont(bold_font);
  edit_feeds_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Manage Events
  //
  edit_events_check=new QCheckBox(this);
  edit_events_label=new QLabel(tr("Manage Events"),this);
  edit_events_label->setFont(bold_font);
  edit_events_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Password Button
  //
  edit_password_button=new QPushButton(tr("Change")+"\n"+tr("Password"),this);
  edit_password_button->setFont(bold_font);
  connect(edit_password_button,SIGNAL(clicked()),this,SLOT(passwordData()));

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


EditUser::~EditUser()
{
}


QSize EditUser::sizeHint() const
{
  return QSize(350,242);
}
  

int EditUser::exec(int user_id)
{
  edit_user_id=user_id;

  QString sql=QString("select ")+
    "USERNAME,"+
    "FULL_NAME,"+
    "DESCRIPTION,"+
    "USER_PRIV,"+
    "SITE_PRIV,"+
    "FEED_PRIV,"+
    "EVENT_PRIV "+
    "from USERS where "+
    QString().sprintf("ID=%d",user_id);
  SqlQuery *q=new SqlQuery(sql);
  if(q->first()) {
    setWindowTitle(tr("GlassNet - Editing User")+": "+q->value(0).toString());
    edit_username_edit->setText(q->value(0).toString());
    edit_fullname_edit->setText(q->value(1).toString());
    edit_description_edit->setText(q->value(2).toString());
    edit_users_check->setChecked(q->value(3).toBool());
    edit_sites_check->setChecked(q->value(4).toBool());
    edit_feeds_check->setChecked(q->value(5).toBool());
    edit_events_check->setChecked(q->value(6).toBool());
  }
  delete q;

  return QDialog::exec();
}


void EditUser::passwordData()
{
  edit_changepassword_dialog->exec(edit_user_id);
}


void EditUser::userPrivToggled(bool state)
{
  if((!state)&&(global_user->id()==edit_user_id)) {
    QMessageBox::information(this,tr("GlassNet - Error"),
       		     tr("You cannot revoke your own Manage Users right."));
    edit_users_check->setChecked(true);
  }
}


void EditUser::okData()
{
  QString sql=QString("update USERS set ")+
    "FULL_NAME='"+SqlQuery::escape(edit_fullname_edit->text())+"',"+
    "DESCRIPTION='"+SqlQuery::escape(edit_description_edit->text())+"',"+
    QString().sprintf("USER_PRIV=%u,",edit_users_check->isChecked())+
    QString().sprintf("SITE_PRIV=%u,",edit_sites_check->isChecked())+
    QString().sprintf("FEED_PRIV=%u,",edit_feeds_check->isChecked())+
    QString().sprintf("EVENT_PRIV=%u ",edit_events_check->isChecked())+
    "where "+
    QString().sprintf("ID=%d",edit_user_id);
  SqlQuery::run(sql);
  done(true);
}


void EditUser::cancelData()
{
  done(false);
}


void EditUser::resizeEvent(QResizeEvent *e)
{
  edit_username_label->setGeometry(10,10,100,20);
  edit_username_edit->setGeometry(115,10,size().width()-140,20);

  edit_fullname_label->setGeometry(10,32,100,20);
  edit_fullname_edit->setGeometry(115,32,size().width()-140,20);

  edit_description_label->setGeometry(10,54,100,20);
  edit_description_edit->setGeometry(115,54,size().width()-140,20);

  edit_users_check->setGeometry(15,88,20,20);
  edit_users_label->setGeometry(40,86,size().width()-105,20);

  edit_sites_check->setGeometry(15,110,20,20);
  edit_sites_label->setGeometry(40,108,size().width()-105,20);

  edit_feeds_check->setGeometry(15,132,20,20);
  edit_feeds_label->setGeometry(40,130,size().width()-105,20);

  edit_events_check->setGeometry(15,154,20,20);
  edit_events_label->setGeometry(40,152,size().width()-105,20);

  edit_password_button->setGeometry(size().width()-110,86,90,50);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
