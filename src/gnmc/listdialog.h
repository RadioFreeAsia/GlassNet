// listdialog.h
//
// Base class for listing dialogs in gnmc(1)
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

#ifndef LISTDIALOG_H
#define LISTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class ListDialog : public QDialog
{
  Q_OBJECT
 public:
  ListDialog(QWidget *parent=0);
  
 public slots:
  void setServiceState(bool state);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *list_health_text_label;
  QLabel *list_health_icon_label;
  QPixmap *list_greenball_pixmap;
  QPixmap *list_redball_pixmap;
};


#endif  // LISTDIALOG_H
