// editreceiver.h
//
// Edit a GlassNet Receiver
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

#ifndef EDITRECEIVER_H
#define EDITRECEIVER_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QTextEdit>

#include "combobox.h"

class EditReceiver : public QDialog
{
  Q_OBJECT
 public:
  EditReceiver(QWidget *parent=0);
  ~EditReceiver();
  QSize sizeHint() const;
  
 public slots:
  int exec(int *receiver_id);

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_type_label;
  ComboBox *edit_type_box;
  QLabel *edit_mac_label;
  QLineEdit *edit_mac_edit;
  QLabel *edit_remarks_label;
  QTextEdit *edit_remarks_textedit;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  int *edit_receiver_id;
};


#endif  // EDITRECEIVER_H
