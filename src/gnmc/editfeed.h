// editfeed.h
//
// Edit a GlassNet Feed
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

#ifndef EDITFEED_H
#define EDITFEED_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QTextEdit>

class EditFeed : public QDialog
{
  Q_OBJECT
 public:
  EditFeed(QWidget *parent=0);
  ~EditFeed();
  QSize sizeHint() const;
  
 public slots:
  int exec(int *feed_id);

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_name_label;
  QLineEdit *edit_name_edit;
  QLabel *edit_url_label;
  QLabel *edit_remarks_label;
  QTextEdit *edit_remarks_textedit;
  QLineEdit *edit_url_edit;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  int *edit_feed_id;
};


#endif  // EDITFEED_H
