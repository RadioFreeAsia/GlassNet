// listusers.cpp
//
// List GlassNet Users
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

#include "globals.h"
#include "listusers.h"
#include "user.h"

ListUsers::ListUsers(int user_id,QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle(tr("GlassNet - List Users"));

  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Dialogs
  //
  list_adduser_dialog=new AddUser(this);
  list_edituser_dialog=new EditUser(this);

  list_model=new SqlTableModel(this);
  QString sql=QString("select ")+
    "ID,"+
    "USERNAME,"+
    "FULL_NAME,"+
    "USER_PRIV,"+
    "SITE_PRIV,"+
    "EVENT_PRIV "+
    "from USERS order by "+
    "USERNAME";
  list_model->setQuery(sql);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Username"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Full Name"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("Manage Users"));
  list_model->setFieldType(3,SqlTableModel::BooleanType);
  list_model->setHeaderData(4,Qt::Horizontal,tr("Manage Sites"));
  list_model->setFieldType(4,SqlTableModel::BooleanType);
  list_model->setHeaderData(5,Qt::Horizontal,tr("Manage Events"));
  list_model->setFieldType(5,SqlTableModel::BooleanType);
  list_view=new TableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(0);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  list_add_button=new QPushButton(tr("Add"),this);
  list_add_button->setFont(bold_font);
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  list_edit_button=new QPushButton(tr("Edit"),this);
  list_edit_button->setFont(bold_font);
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  list_delete_button=new QPushButton(tr("Delete"),this);
  list_delete_button->setFont(bold_font);
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  list_close_button=new QPushButton(tr("Close"),this);
  list_close_button->setFont(bold_font);
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListUsers::~ListUsers()
{
}


QSize ListUsers::sizeHint() const
{
  return QSize(650,400);
}


int ListUsers::exec()
{
  list_model->update();
  return QDialog::exec();
}


void ListUsers::addData()
{
  int user_id=0;

  if(list_adduser_dialog->exec(&user_id)) {
    if(list_edituser_dialog->exec(user_id)) {
      list_model->update();
    }
    else {
      User::remove(user_id);
    }
  }
}


void ListUsers::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    if(list_edituser_dialog->exec(s->selectedRows()[0].data().toInt())) {
      list_model->update();
    }
  }
}


void ListUsers::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int user_id=s->selectedRows()[0].data().toInt();
    if(user_id==global_user->id()) {
      QMessageBox::information(this,tr("GlassNet - Error"),
			       tr("You cannot delete yourself."));
      return;
    }
    User *user=new User(user_id);
    if(QMessageBox::question(this,tr("GlassNet - Delete User"),
			     tr("Are you sure you want to delete user")+
			     " \""+user->userName()+"\"?",
			     QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete user;
      return;
    }
    User::remove(user_id);
    list_model->update();
    delete user;
  }
}


void ListUsers::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListUsers::closeData()
{
  done(1);
}


void ListUsers::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,10,size().width()-20,size().height()-80);

  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);

  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
