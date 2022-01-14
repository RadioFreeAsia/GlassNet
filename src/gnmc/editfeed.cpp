// editfeed.cpp
//
// Edit a GlassNet Feed
//
//   (C) Copyright 2017-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include "db.h"
#include "editfeed.h"
#include "feed.h"
#include "globals.h"

EditFeed::EditFeed(QWidget *parent)
  : QDialog(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);
  setMinimumSize(sizeHint());

  //
  // Name
  //
  edit_name_label=new QLabel(tr("Name")+":",this);
  edit_name_label->setFont(bold_font);
  edit_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_name_edit=new QLineEdit(this);
  edit_name_edit->setMaxLength(16);

  //
  // URL
  //
  edit_url_label=new QLabel(tr("URL")+":",this);
  edit_url_label->setFont(bold_font);
  edit_url_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_url_edit=new QLineEdit(this);
  edit_url_edit->setMaxLength(255);

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


EditFeed::~EditFeed()
{
}


QSize EditFeed::sizeHint() const
{
  return QSize(600,125);
}
  

int EditFeed::exec(int *feed_id)
{
  edit_feed_id=feed_id;
  if(*feed_id>0) {
    setWindowTitle(tr("GlassNet - Edit Feed")+
		   QString::asprintf(" %d",*feed_id));
    Feed *feed=new Feed(*feed_id);
    edit_name_edit->setText(feed->name());
    edit_url_edit->setText(feed->url());
    delete feed;
  }
  else {
    setWindowTitle(tr("GlassNet - Edit Feed [new]"));
    edit_name_edit->setText("[new feed]");
    edit_url_edit->setText("");
  }
  return QDialog::exec();
}


void EditFeed::okData()
{
  if(*edit_feed_id<0) {
    *edit_feed_id=Feed::create();
  }
  Feed *feed=new Feed(*edit_feed_id);
  feed->setName(edit_name_edit->text());
  feed->setUrl(edit_url_edit->text());
  delete feed;

  done(true);
}


void EditFeed::cancelData()
{
  done(false);
}


void EditFeed::resizeEvent(QResizeEvent *e)
{
  edit_name_label->setGeometry(10,10,120,20);
  edit_name_edit->setGeometry(135,10,200,20);

  edit_url_label->setGeometry(10,32,120,20);
  edit_url_edit->setGeometry(135,32,size().width()-160,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
