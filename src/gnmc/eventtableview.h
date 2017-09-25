// eventtableview.h
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

#ifndef EVENTTABLEVIEW_H
#define EVENTTABLEVIEW_H

#include <QMenu>

#include "tableview.h"

class EventTableView : public TableView
{
  Q_OBJECT
 public:
  EventTableView(QWidget *parent=0);

 signals:
  void editReceiver(const QString &mac_addr);

 private slots:
  void editReceiverData();

 protected:
  void mousePressEvent(QMouseEvent *e);

 private:
  QMenu *event_menu;
  QString event_selected_mac;
};


#endif  //  EVENTTABLEVIEW_H
