// listreceivers.h
//
// List GlassNet Receivers
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

#ifndef LISTRECEIVERS_H
#define LISTRECEIVERS_H

#include <QDialog>
#include <QPushButton>
#include <QResizeEvent>

#include "editreceiver.h"
#include "sqltablemodel.h"
#include "tableview.h"

class ListReceivers : public QDialog
{
  Q_OBJECT
 public:
  ListReceivers(QWidget *parent=0);
  ~ListReceivers();
  QSize sizeHint() const;
  
 public slots:
  int exec();

 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  EditReceiver *list_editreceiver_dialog;
  SqlTableModel *list_model;
  TableView *list_view;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
};


#endif  // LISTRECEIVERS_H
