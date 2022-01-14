// listchassis.cpp
//
// List GlassNet Chassis
//
//   (C) Copyright 2016-2022 Fred Gleason <fredg@paravelsystems.com>
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
#include "listchassis.h"
#include "receiver.h"
#include "site.h"
#include "user.h"

ListChassis::ListChassis(QWidget *parent)
  : ListDialog(parent)
{
  setWindowTitle(tr("GlassNet - List Chassis"));
  setMinimumSize(sizeHint());

  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Dialogs
  //
  list_editchassis_dialog=new EditChassis(this);

  list_model=new SqlTableModel(this);
  QString sql=QString("select ")+
    "CHASSIS.ID,"+             // 00
    "SITES.NAME,"+             // 01
    "CHASSIS.SLOT,"+           // 02
    "CHASSIS.TYPE,"+           // 03
    "CHASSIS.SERIAL_NUMBER "+  // 04
    "from CHASSIS left join SITES "
    "on CHASSIS.SITE_ID=SITES.ID "+
    "order by "+
    "SITES.NAME,CHASSIS.TYPE,CHASSIS.SERIAL_NUMBER";
  list_model->setQuery(sql);
  list_model->setHeaderData(0,Qt::Horizontal,tr("Chassis ID"));
  list_model->setHeaderData(1,Qt::Horizontal,tr("Site"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Slot"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("Type"));
  list_model->setFieldType(3,SqlTableModel::ChassisType);
  list_model->setHeaderData(4,Qt::Horizontal,tr("Serial Number"));
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


ListChassis::~ListChassis()
{
}


QSize ListChassis::sizeHint() const
{
  return QSize(400,300);
}


int ListChassis::exec()
{
  list_model->update();
  list_view->resizeColumnsToContents();
  return QDialog::exec();
}


void ListChassis::addData()
{
  int chassis_id=-1;
  if(list_editchassis_dialog->exec(&chassis_id)) {
    list_model->update();
    list_view->select(0,chassis_id);
    list_view->resizeColumnsToContents();
  }
}


void ListChassis::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int chassis_id=s->selectedRows()[0].data().toInt();
    if(list_editchassis_dialog->exec(&chassis_id)) {
      list_model->update();
    }
  }
}


void ListChassis::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int chassis_id=s->selectedRows()[0].data().toInt();
    Chassis *chassis=new Chassis(chassis_id);
    if(chassis->siteId()>0) {
      Site *site=new Site(chassis->siteId());
      QMessageBox::warning(this,tr("GlassNet - Error"),
			   tr("Chassis is in use at site")+
			   " \""+site->siteName()+"\".");
      delete site;
      return;
    }
    QString sql=QString("select ID from RECEIVERS where ")+
      QString::asprintf("CHASSIS_ID=%d",chassis_id);
    SqlQuery *q=new SqlQuery(sql);
    if(q->size()>0) {
      switch(QMessageBox::question(this,tr("GlassNet - Warning"),
				   tr("Chassis contains one or more receivers.")+"\n"+
				   tr("Delete receivers also?"),
				   QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel)) {
      case QMessageBox::Yes:
	while(q->next()) {
	  Receiver::remove(q->value(0).toInt());
	}
	break;

      case QMessageBox::No:
	sql=QString("update RECEIVERS set ")+
	  "CHASSIS_ID=null,"+
	  "SLOT=null where "+
	  QString::asprintf("CHASSIS_ID=%d",chassis_id);
	SqlQuery::run(sql);
	break;

      default:
	delete q;
	return;
      }
    }
    delete q;

    if(QMessageBox::question(this,tr("GlassNet - Delete Chassis"),
			     tr("Are you sure you want to delete chassis")+
			     " \""+Chassis::typeString(chassis->type())+
			     " ["+chassis->serialNumber()+"\"\"?\n",
			QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete chassis;
      return;
    }
    Chassis::remove(chassis_id);
    list_model->update();
    delete chassis;
  }
}


void ListChassis::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListChassis::closeData()
{
  done(1);
}


void ListChassis::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,32,size().width()-20,size().height()-112);

  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);

  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);

  ListDialog::resizeEvent(e);
}
