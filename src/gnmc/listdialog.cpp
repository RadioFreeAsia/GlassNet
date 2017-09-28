// listdialog.cpp
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

#include <stdio.h>

#include "listdialog.h"

#include "../../icons/greenball.xpm"
#include "../../icons/redball.xpm"

ListDialog::ListDialog(QWidget *parent)
  : QDialog(parent)
{
  //
  // Fonts
  //
  QFont label_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Icons
  //
  list_greenball_pixmap=new QPixmap(greenball_xpm);
  list_redball_pixmap=new QPixmap(redball_xpm);

  list_health_icon_label=new QLabel(this);
  list_health_icon_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

  list_health_text_label=new QLabel(tr("Service Health"),this);
  list_health_text_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  list_health_text_label->setFont(label_font);
}


void ListDialog::setServiceState(bool state)
{
  if(state) {
    list_health_icon_label->setPixmap(*list_greenball_pixmap);
  }
  else {
    list_health_icon_label->setPixmap(*list_redball_pixmap);
  }
}


void ListDialog::resizeEvent(QResizeEvent *e)
{
  list_health_icon_label->setGeometry(15,10,20,20);
  list_health_text_label->setGeometry(35,10,200,20);
}
