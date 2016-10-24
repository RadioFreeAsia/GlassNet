// editevent.h
//
// Edit a GlassNet Event
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

#ifndef EDITEVENT_H
#define EDITEVENT_H

#include <QDateTimeEdit>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>

#include "combobox.h"
#include "dayofweekwidget.h"
#include "event.h"

class EditEvent : public QDialog
{
  Q_OBJECT
 public:
  EditEvent(QWidget *parent=0);
  ~EditEvent();
  QSize sizeHint() const;
  
 public slots:
  int exec(int *event_id);

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void LoadSites();
  QLabel *edit_site_label;
  ComboBox *edit_site_box;
  QLabel *edit_chassis_label;
  ComboBox *edit_chassis_box;
  QLabel *edit_receiver_label;
  ComboBox *edit_receiver_box;
  DayOfWeekWidget *edit_dow_widget;
  QLabel *edit_start_label;
  QDateTimeEdit *edit_start_edit;
  QLabel *edit_length_label;
  QDateTimeEdit *edit_length_edit;
  QLabel *edit_url_label;
  QLineEdit *edit_url_edit;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  int *edit_event_id;
};


#endif  // EDITEVENT_H
