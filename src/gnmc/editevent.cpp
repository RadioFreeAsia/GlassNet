// editevent.cpp
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

#include <QMessageBox>
#include <QUrl>

#include "chassis.h"
#include "db.h"
#include "editevent.h"
#include "event.h"
#include "globals.h"
#include "site.h"

EditEvent::EditEvent(QWidget *parent)
  : QDialog(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);
  setMinimumSize(sizeHint());

  //
  // Sites
  //
  edit_site_label=new QLabel(tr("Site")+":",this);
  edit_site_label->setFont(bold_font);
  edit_site_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_site_box=new ComboBox(this);

  //
  // Chassis
  //
  edit_chassis_label=new QLabel(tr("Chassis")+":",this);
  edit_chassis_label->setFont(bold_font);
  edit_chassis_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_chassis_box=new ComboBox(this);
  for(int i=0;i<MAX_CHASSIS_SLOTS;i++) {
    edit_chassis_box->insertItem(-1,QString().sprintf("%d",i),i);
  }

  //
  // Receivers
  //
  edit_receiver_label=new QLabel(tr("Receiver")+":",this);
  edit_receiver_label->setFont(bold_font);
  edit_receiver_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_receiver_box=new ComboBox(this);
  for(int i=0;i<MAX_RECEIVER_SLOTS;i++) {
    edit_receiver_box->insertItem(-1,QString().sprintf("%d",i),i);
  }

  //
  // Day of Week Widget
  //
  edit_dow_widget=new DayOfWeekWidget(this);

  //
  // Start Time
  //
  edit_start_label=new QLabel(tr("Start Time")+":",this);
  edit_start_label->setFont(bold_font);
  edit_start_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_start_edit=new QDateTimeEdit(this);
  edit_start_edit->setDisplayFormat("hh:mm:ss");

  //
  // Length
  //
  edit_length_label=new QLabel(tr("Length")+":",this);
  edit_length_label->setFont(bold_font);
  edit_length_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_length_edit=new QDateTimeEdit(this);
  edit_length_edit->setDisplayFormat("hh:mm:ss");

  //
  // Feeds
  //
  edit_feed_label=new QLabel(tr("Feed")+":",this);
  edit_feed_label->setFont(bold_font);
  edit_feed_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_feed_box=new ComboBox(this);

  //
  // Ok Button
  //
  edit_ok_button=new QPushButton(tr("OK"),this);
  edit_ok_button->setFont(bold_font);
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  edit_cancel_button=new QPushButton(tr("Cancel"),this);
  edit_cancel_button->setFont(bold_font);
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


EditEvent::~EditEvent()
{
}


QSize EditEvent::sizeHint() const
{
  return QSize(650,235);
}
  

int EditEvent::exec(int *event_id)
{
  edit_event_id=event_id;
  LoadSites();
  LoadEvents();
  if(*event_id>0) {
    setWindowTitle(tr("GlassNet - Edit Event")+
		   QString().sprintf(" %d",*event_id));
    Event *event=new Event(*event_id);
    edit_site_box->setCurrentItemData(event->siteId());
    edit_chassis_box->setCurrentItemData(event->chassisSlot());
    edit_receiver_box->setCurrentItemData(event->receiverSlot());
    for(int i=0;i<7;i++) {
      edit_dow_widget->setActive(i+1,event->dowActive(i+1));
    }
    edit_start_edit->setTime(event->startTime());
    edit_length_edit->setTime(QTime().addMSecs(event->length()));
    edit_feed_box->setCurrentItemData(event->feedId());
    delete event;
  }
  else {
    setWindowTitle(tr("GlassNet - Edit Event [new]"));
    *event_id=Event::create(edit_site_box->currentItemData().toInt(),0,0);
    edit_chassis_box->setCurrentItemData(0);
    edit_receiver_box->setCurrentItemData(0);
    for(int i=0;i<7;i++) {
      edit_dow_widget->setActive(i+1,false);
    }
    edit_start_edit->setTime(QTime(0,0,0,0));
    edit_length_edit->setTime(QTime());
    edit_feed_box->setCurrentItemData(-1);
  }
  return QDialog::exec();
}


void EditEvent::okData()
{
  if(!Event::receiverExists(edit_site_box->currentItemData().toInt(),
			    edit_chassis_box->currentItemData().toInt(),
			    edit_receiver_box->currentItemData().toInt())) {
    if(QMessageBox::question(this,tr("GlassNet - Inactive Receiver"),
			     tr("There is no receiver installed in the specified location.")+
			     "\n"+
			     tr("To you still want to save?"),
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
  }
  Event *event=new Event(*edit_event_id);
  event->setSiteId(edit_site_box->currentItemData().toInt());
  event->setChassisSlot(edit_chassis_box->currentItemData().toInt());
  event->setReceiverSlot(edit_receiver_box->currentItemData().toInt());
  for(int i=0;i<7;i++) {
    event->setDowActive(i+1,edit_dow_widget->isActive(i+1));
  }
  event->setStartTime(edit_start_edit->time());
  event->setLength(QTime().msecsTo(edit_length_edit->time()));
  event->setFeedId(edit_feed_box->currentItemData().toInt());
  event->setPosted(false);
  delete event;
  done(true);
}


void EditEvent::cancelData()
{
  done(false);
}


void EditEvent::resizeEvent(QResizeEvent *e)
{
  edit_site_label->setGeometry(10,10,50,20);
  edit_site_box->setGeometry(65,10,200,20);

  edit_chassis_label->setGeometry(280,10,90,20);
  edit_chassis_box->setGeometry(375,10,50,20);

  edit_receiver_label->setGeometry(440,10,90,20);
  edit_receiver_box->setGeometry(535,10,50,20);

  edit_dow_widget->setGeometry(10,45,size().width()-20,60);

  edit_start_label->setGeometry(10,115,90,20);
  edit_start_edit->setGeometry(105,115,100,20);

  edit_length_label->setGeometry(230,115,100,20);
  edit_length_edit->setGeometry(335,115,100,20);

  edit_feed_label->setGeometry(10,142,40,20);
  edit_feed_box->setGeometry(55,142,size().width()-65,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditEvent::LoadSites()
{
  edit_site_box->clear();

  QString sql=QString("select ID,NAME from SITES order by NAME");
  SqlQuery *q=new SqlQuery(sql);
  while(q->next()) {
    edit_site_box->insertItem(-1,q->value(1).toString(),q->value(0));
  }
  delete q;
}


void EditEvent::LoadEvents()
{
  edit_feed_box->clear();
  edit_feed_box->insertItem(0,tr("[none]"),-1);

  QString sql=QString("select ID,NAME from FEEDS order by NAME");
  QSqlQuery *q=new QSqlQuery(sql);
  while(q->next()) {
    edit_feed_box->insertItem(edit_feed_box->count(),q->value(1).toString(),
			      q->value(0));
  }
  delete q;
}
