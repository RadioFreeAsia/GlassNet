// editchassis.h
//
// Edit a GlassNet Chassis
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

#ifndef EDITCHASSIS_H
#define EDITCHASSIS_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QTextEdit>

#include "chassis.h"
#include "combobox.h"
#include "sqltablemodel.h"
#include "tableview.h"

class EditChassis : public QDialog
{
  Q_OBJECT
 public:
  EditChassis(QWidget *parent=0);
  ~EditChassis();
  QSize sizeHint() const;
  
 public slots:
  int exec(int *chassis_id);

 private slots:
  void slotActivatedData(int n);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void BuildReceiverLists();
  QLabel *edit_type_label;
  ComboBox *edit_type_box;
  QLabel *edit_serial_label;
  QLineEdit *edit_serial_edit;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  int *edit_chassis_id;
  QLabel *edit_receiver_label[MAX_RECEIVER_SLOTS];
  ComboBox *edit_receiver_box[MAX_RECEIVER_SLOTS];
};


#endif  // EDITCHASSIS_H
