// listevents.cpp
//
// List GlassNet Events
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

#include "chassis.h"
#include "globals.h"
#include "listevents.h"
#include "receiver.h"
#include "site.h"
#include "user.h"

ListEvents::ListEvents(QWidget *parent)
  : ListDialog(true,parent)
{
  QString sql;
  SqlQuery *q;

  setWindowTitle(tr("GlassNet - List Events"));
  setMinimumSize(sizeHint());

  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Dialogs
  //
  list_editevent_dialog=new EditEvent(this);

  //
  // Site Selector
  //
  list_site_label=new QLabel(tr("Site")+":",this);
  list_site_label->setFont(bold_font);
  list_site_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  list_site_box=new ComboBox(this);
  list_site_box->insertItem(0,tr("ALL"),-1);
  connect(list_site_box,SIGNAL(activated(int)),
	  this,SLOT(siteIdActivatedData(int)));
  sql=QString("select ")+
    "`ID`,"+    // 00
    "`NAME` "+  // 01
    "from `SITES` order by `NAME`";
  q=new SqlQuery(sql);
  while(q->next()) {
    list_site_box->insertItem(list_site_box->count(),q->value(1).toString(),
			      q->value(0).toInt());
  }
  delete q;

  list_model=new EventTableModel(this);
  UpdateModel();
  list_model->setHeaderData(0,Qt::Horizontal,tr("ID"));
  list_model->setHeaderData(1,Qt::Horizontal,"");
  list_model->setFieldType(1,SqlTableModel::BiStateType);
  list_model->setHeaderData(2,Qt::Horizontal,"Site");
  list_model->setHeaderData(3,Qt::Horizontal,tr("Chassis"));
  list_model->setFieldType(3,SqlTableModel::NumericType);
  list_model->setHeaderData(4,Qt::Horizontal,tr("Slot")); 
  list_model->setFieldType(4,SqlTableModel::NumericType);
  list_model->setHeaderData(5,Qt::Horizontal,tr("Start Time"));
  list_model->setFieldType(5,SqlTableModel::TimeType);
  list_model->setHeaderData(6,Qt::Horizontal,tr("Length"));
  list_model->setFieldType(6,SqlTableModel::LengthType);
  list_model->setHeaderData(7,Qt::Horizontal,tr("Sun"));
  list_model->setFieldType(7,SqlTableModel::BooleanType);
  list_model->setHeaderData(8,Qt::Horizontal,tr("Mon"));
  list_model->setFieldType(8,SqlTableModel::BooleanType);
  list_model->setHeaderData(9,Qt::Horizontal,tr("Tue"));
  list_model->setFieldType(9,SqlTableModel::BooleanType);
  list_model->setHeaderData(10,Qt::Horizontal,tr("Wed"));
  list_model->setFieldType(10,SqlTableModel::BooleanType);
  list_model->setHeaderData(11,Qt::Horizontal,tr("Thu"));
  list_model->setFieldType(11,SqlTableModel::BooleanType);
  list_model->setHeaderData(12,Qt::Horizontal,tr("Fri"));
  list_model->setFieldType(12,SqlTableModel::BooleanType);
  list_model->setHeaderData(13,Qt::Horizontal,tr("Sat"));
  list_model->setFieldType(13,SqlTableModel::BooleanType);
  list_model->setHeaderData(14,Qt::Horizontal,tr("Feed"));
  list_model->setHeaderData(15,Qt::Horizontal,tr("Event ID"));
  list_model->setFieldType(15,SqlTableModel::NumericType);
  list_model->setHeaderData(16,Qt::Horizontal,tr("Feed ID"));
  list_model->setFieldType(16,SqlTableModel::NumericType);
  list_model->setHeaderData(17,Qt::Horizontal,tr("Active"));
  list_model->setFieldType(17,SqlTableModel::BooleanType);
  list_view=new EventTableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(0);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(editReceiver(int)),this,SLOT(editReceiverData(int)));
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

  //
  // Update Timer
  //
  list_update_timer=new QTimer(this);
  connect(list_update_timer,SIGNAL(timeout()),list_model,SLOT(update()));
}


ListEvents::~ListEvents()
{
}


QSize ListEvents::sizeHint() const
{
  return QSize(1000,600);
}


int ListEvents::exec()
{
  list_model->update();
  list_view->resizeColumnsToContents();
  list_update_timer->start(5000);
  return QDialog::exec();
}


void ListEvents::addData()
{
  int event_id=-1;
  if(list_editevent_dialog->exec(&event_id)) {
    list_model->update();
    list_view->resizeColumnsToContents();
    list_view->select(0,event_id);
  }
  else {
    Event::remove(event_id);
    list_model->update();
  }
}


void ListEvents::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int event_id=s->selectedRows()[0].data().toInt();
    if(list_editevent_dialog->exec(&event_id)) {
      list_model->update();
      list_view->resizeColumnsToContents();
    }
  }
}


void ListEvents::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    Event *event=new Event(s->selectedRows()[0].data().toInt());
    if(QMessageBox::question(this,tr("GlassNet - Delete Event"),
			     tr("Are you sure you want to delete the")+
			     " \""+event->feedName()+"\" "+tr("event?"),
			     QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete event;
      return;
    }
    Event::remove(event->id());
    delete event;
    list_model->update();
  }
}


void ListEvents::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListEvents::editReceiverData(int receiver_id)
{
  emit editReceiver(receiver_id);
}


void ListEvents::siteIdActivatedData(int n)
{
  UpdateModel();
}


void ListEvents::closeData()
{
  list_update_timer->stop();
  done(1);
}


void ListEvents::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,32,size().width()-20,size().height()-112);

  list_site_label->setGeometry(200,10,60,20);
  list_site_box->setGeometry(265,10,200,20);

  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);

  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);

  ListDialog::resizeEvent(e);
}


void ListEvents::UpdateModel()
{
  QString sql=QString("select ")+
    "`EVENTS`.`ID`,"+             // 00
    "`EVENTS`.`POSTED`,"+         // 01
    "`SITES`.`NAME`,"+            // 02
    "`EVENTS`.`CHASSIS_SLOT`,"+   // 03
    "`EVENTS`.`RECEIVER_SLOT`,"+  // 04
    "`EVENTS`.`START_TIME`,"+     // 05
    "`EVENTS`.`LENGTH`,"+         // 06
    "`EVENTS`.`SUN`,"+            // 07
    "`EVENTS`.`MON`,"+            // 08
    "`EVENTS`.`TUE`,"+            // 09
    "`EVENTS`.`WED`,"+            // 10
    "`EVENTS`.`THU`,"+            // 11
    "`EVENTS`.`FRI`,"+            // 12
    "`EVENTS`.`SAT`,"+            // 13
    "`FEEDS`.`NAME`,"+            // 14
    "`EVENTS`.`ID`,"+             // 15
    "`EVENTS`.`FEED_ID`,"+        // 16
    "`EVENTS`.`IS_ACTIVE` "+             // 17
    "from `EVENTS` left join `SITES` "+
    "on `EVENTS`.`SITE_ID`=`SITES`.`ID` "+
    "left join `FEEDS` "+
    "on `EVENTS`.`FEED_ID`=`FEEDS`.`ID` ";
  if(list_site_box->currentItemData().toInt()>=0) {
    sql+=QString("where ")+
      QString::asprintf("`EVENTS`.`SITE_ID`=%d ",
			list_site_box->currentItemData().toInt());
  }
  sql+=QString("order by ")+
    "`EVENTS`.`START_TIME`,"+
    "`SITES`.`NAME`,"+
    "`EVENTS`.`CHASSIS_SLOT`,"+
    "`EVENTS`.`RECEIVER_SLOT`";
  list_model->setQuery(sql);
}

