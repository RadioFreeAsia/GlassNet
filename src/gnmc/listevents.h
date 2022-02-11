// listevents.h
//
// List GlassNet Events
//
//   (C) Copyright 2016-2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LISTEVENTS_H
#define LISTEVENTS_H

#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>

#include "combobox.h"
#include "editevent.h"
#include "editreceiver.h"
#include "eventtablemodel.h"
#include "eventtableview.h"
#include "listdialog.h"

class ListEvents : public ListDialog
{
  Q_OBJECT
 public:
  ListEvents(QWidget *parent=0);
  ~ListEvents();
  QSize sizeHint() const;
  
 signals:
  void editReceiver(int receiver_id);

 public slots:
  int exec();

 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(const QModelIndex &index);
  void editReceiverData(int receiver_id);
  void siteIdActivatedData(int n);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void UpdateModel();
  EditEvent *list_editevent_dialog;
  QLabel *list_site_label;
  ComboBox *list_site_box;
  EventTableModel *list_model;
  EventTableView *list_view;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
  QTimer *list_update_timer;
};


#endif  // LISTEVENTS_H
