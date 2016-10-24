// dayofweekwidget.h
//
// Widget for selecting days of the week
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

#ifndef DAYOFWEEKWIDGET_H
#define DAYOFWEEKWIDGET_H

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class DayOfWeekWidget : public QWidget
{
  Q_OBJECT
 public:
  DayOfWeekWidget(QWidget *parent=0);
  ~DayOfWeekWidget();
  QSizePolicy sizePolicy() const;
  QSize sizeHint() const;
  bool isActive(int dow) const;
  void setActive(int dow,bool state);

 private slots:
  void setAllData();
  void clearAllData();

 protected:
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *dow_labels[7];
  QCheckBox *dow_checks[7];
  QPushButton *dow_set_button;
  QPushButton *dow_clear_button;
};


#endif  // DAYOFWEEKWIDGET_H
