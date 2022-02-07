// dayofweekwidget.cpp
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

#include <QDateTime>
#include <QPainter>

#include "dayofweekwidget.h"

DayOfWeekWidget::DayOfWeekWidget(QWidget *parent)
  : QWidget(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);

  for(int i=0;i<7;i++) {
    dow_labels[i]=new QLabel(QDate::longDayName(i+1),this);
    dow_labels[i]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    dow_labels[i]->setFont(bold_font);
    dow_checks[i]=new QCheckBox(this);
  }
  dow_set_button=new QPushButton(tr("Set All"),this);
  dow_set_button->setFont(bold_font);
  connect(dow_set_button,SIGNAL(clicked()),this,SLOT(setAllData()));

  dow_clear_button=new QPushButton(tr("Clear All"),this);
  dow_clear_button->setFont(bold_font);
  connect(dow_clear_button,SIGNAL(clicked()),this,SLOT(clearAllData()));
}


DayOfWeekWidget::~DayOfWeekWidget()
{
  for(int i=0;i<7;i++) {
    delete dow_labels[i];
    delete dow_checks[i];
  }
  delete dow_set_button;
  delete dow_clear_button;
}


QSizePolicy DayOfWeekWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


QSize DayOfWeekWidget::sizeHint() const
{
  return QSize(630,60);
}


bool DayOfWeekWidget::isActive(int dow) const
{
  return dow_checks[dow-1]->isChecked();
}


void DayOfWeekWidget::setActive(int dow,bool state)
{
  dow_checks[dow-1]->setChecked(state);
}


void DayOfWeekWidget::setAllData()
{
  for(int i=0;i<7;i++) {
    dow_checks[i]->setChecked(true);
  }
}


void DayOfWeekWidget::clearAllData()
{
  for(int i=0;i<7;i++) {
    dow_checks[i]->setChecked(false);
  }
}


void DayOfWeekWidget::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->drawRect(0,0,size().width()-1,size().height()-1);

  delete p;
}


void DayOfWeekWidget::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();
  int w1=2*w/11;

  //
  // Weekdays
  //
  for(int i=0;i<5;i++) {
    dow_checks[i]->setGeometry(10+i*w1,5,20,20);
    dow_labels[i]->setGeometry(10+i*w1+20,5,w1-25,20);
  }

  //
  // Saturday
  //
  dow_checks[5]->setGeometry(10+1*w1,h/2+5,20,20);
  dow_labels[5]->setGeometry(10+1*w1+20,h/2+5,w1-25,20);

  //
  // Sunday
  //
  dow_checks[6]->setGeometry(10+3*w1,h/2+5,20,20);
  dow_labels[6]->setGeometry(10+3*w1+20,h/2+5,w1-25,20);

  //
  // Buttons
  //
  dow_set_button->setGeometry(10*w/11-20,5,w/11+15,h/2-5);
  dow_clear_button->setGeometry(10*w/11-20,h/2,w/11+15,h/2-5);

}
