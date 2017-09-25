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
#include "eventtableview.h"
#include "sqltablemodel.h"

EventTableView::EventTableView(QWidget *parent)
  : TableView(parent)
{
  event_menu=new QMenu(this);
  event_menu->addAction(tr("View Receiver"),this,SLOT(editReceiverData()));
}


void EventTableView::editReceiverData()
{
  emit editReceiver(event_selected_mac);
}


void EventTableView::mousePressEvent(QMouseEvent *e)
{
  QString sql;
  SqlQuery *q;

  if(e->button()==Qt::RightButton) {
    QModelIndex index=indexAt(e->pos());
    if(index.isValid()) {
      SqlTableModel *m=(SqlTableModel *)model();
      sql=QString("select RECEIVERS.MAC_ADDRESS from RECEIVERS ")+
	"left join CHASSIS on RECEIVERS.CHASSIS_ID=CHASSIS.ID "+
	"left join SITES on CHASSIS.SITE_ID=SITES.ID where "+
	"(SITES.NAME=\""+SqlQuery::escape(m->data(index.row(),2).toString())+
	"\")&&"+
	QString().sprintf("(CHASSIS.SLOT=%d)&&",m->data(index.row(),3).toInt())+
	QString().sprintf("(RECEIVERS.SLOT=%d)",m->data(index.row(),4).toInt());
      q=new SqlQuery(sql);
      if(q->first()) {
	event_selected_mac=q->value(0).toString();
	//	emit editReceiver(q->value(0).toString());
	event_menu->popup(e->globalPos());
      }
      delete q;
    }
  }
  TableView::mousePressEvent(e);
}
