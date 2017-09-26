// eventtableview.cpp
//
// Table Viewer for GlassNet Events
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>

#include <QMouseEvent>

#include "db.h"
#include "event.h"
#include "eventtableview.h"
#include "receiver.h"
#include "sqltablemodel.h"

EventTableView::EventTableView(QWidget *parent)
  : TableView(parent)
{
  event_event_id=-1;
  event_receiver_id=-1;

  event_menu=new QMenu(this);
  event_receiver_action=
    event_menu->addAction(tr("View Receiver"),this,SLOT(editReceiverData()));
  event_menu->addSeparator();
  event_start_action=
    event_menu->addAction(tr("START Event Now"),this,SLOT(startEventData()));
  event_stop_action=
    event_menu->addAction(tr("STOP Event Now"),this,SLOT(stopEventData()));
  connect(event_menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowData()));
}


void EventTableView::aboutToShowData()
{
  if(event_receiver_id<0) {
    event_receiver_action->setEnabled(false);
    event_start_action->setEnabled(false);
    event_stop_action->setEnabled(false);
  }
  else {
    event_receiver_action->setEnabled(true);
    Receiver *rcvr=new Receiver(event_receiver_id);
    event_start_action->setEnabled((rcvr->activeGuid()==0)&&rcvr->isOnline());
    event_stop_action->setEnabled((rcvr->activeGuid()!=0)&&rcvr->isOnline());
    delete rcvr;
  }
}


void EventTableView::editReceiverData()
{
  if(event_receiver_id>=0) {
    emit editReceiver(event_receiver_id);
  }
}


void EventTableView::startEventData()
{
  QString sql=QString("insert into PENDING_COMMANDS set ")+
    QString().sprintf("RECEIVER_ID=%d,",event_receiver_id)+
    QString().sprintf("COMMAND=\"PLAYSTART %d\"",event_event_id);
  SqlQuery::run(sql);
}


void EventTableView::stopEventData()
{
  QString sql=QString("insert into PENDING_COMMANDS set ")+
    QString().sprintf("RECEIVER_ID=%d,",event_receiver_id)+
    "COMMAND=\"PLAYSTOP\"";
  SqlQuery::run(sql);
}


void EventTableView::mousePressEvent(QMouseEvent *e)
{
  if(e->button()==Qt::RightButton) {
    QModelIndex index=indexAt(e->pos());
    if(index.isValid()) {
      SqlTableModel *m=(SqlTableModel *)model();
      event_event_id=m->data(index.row(),0).toInt();
      event_receiver_id=Event::receiverId(m->data(index.row(),0).toInt());
      event_menu->popup(e->globalPos());
    }
    else {
      event_event_id=-1;
      event_receiver_id=-1;
    }
  }
  TableView::mousePressEvent(e);
}
