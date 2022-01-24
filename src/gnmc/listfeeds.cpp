// listfeeds.cpp
//
// List GlassNet Sites
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

#include "globals.h"
#include "listfeeds.h"
#include "feed.h"
#include "user.h"

ListFeeds::ListFeeds(QWidget *parent)
  : ListDialog(true,parent)
{
  setWindowTitle(tr("GlassNet - List Sites"));
  setMinimumSize(sizeHint());

  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Dialogs
  //
  list_editfeed_dialog=new EditFeed(this);

  //
  // Remarks Checkbox
  //
  list_remarks_check=new QCheckBox(this);
  list_remarks_label=new QLabel(tr("Show Note Bubbles"),this);
  list_model=new SqlTableModel(this);
  connect(list_remarks_check,SIGNAL(toggled(bool)),
	  list_model,SLOT(setShowRemarks(bool)));
  QString sql=QString("select ")+
    "`ID`,"+       // 00
    "`NAME`,"+     // 01
    "`URL`,"+      // 02
    "`REMARKS` "+  // 03
    "from `FEEDS` order by "+
    "`NAME`";
  list_model->setQuery(sql,3);
  list_model->setHeaderData(0,Qt::Horizontal,tr("Feed ID"));
  list_model->setFieldType(0,SqlTableModel::NumericType);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Name"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("URL"));
  list_view=new TableView(this);
  list_view->setModel(list_model);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  list_add_button=new QPushButton(tr("Add"),this);
  list_add_button->setFont(bold_font);
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  list_edit_button=new QPushButton(tr("Edit"),this);
  list_edit_button->setFont(bold_font);
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  list_delete_button=new QPushButton(tr("Delete"),this);
  list_delete_button->setFont(bold_font);
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  list_close_button=new QPushButton(tr("Close"),this);
  list_close_button->setFont(bold_font);
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListFeeds::~ListFeeds()
{
}


QSize ListFeeds::sizeHint() const
{
  return QSize(640,480);
}


int ListFeeds::exec()
{
  list_model->update();
  list_view->resizeColumnsToContents();
  return QDialog::exec();
}


void ListFeeds::addData()
{
  int feed_id=-1;

  if(list_editfeed_dialog->exec(&feed_id)) {
    list_model->update();
    list_view->select(0,feed_id);
    list_view->resizeColumnsToContents();
  }
  else {
    Feed::remove(feed_id);
  }
}


void ListFeeds::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int feed_id=s->selectedRows()[0].data().toInt();
    if(list_editfeed_dialog->exec(&feed_id)) {
      list_model->update();
      QString sql=QString("update EVENTS set POSTED=0 where ")+
	QString::asprintf("FEED_ID=%d",feed_id);
      SqlQuery::run(sql);
    }
  }
}


void ListFeeds::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int feed_id=s->selectedRows()[0].data().toInt();
    Feed *feed=new Feed(feed_id);
    if(QMessageBox::question(this,tr("GlassNet - Delete Feed"),
			     tr("Are you sure you want to delete feed")+
			     " \""+feed->name()+"\"?"+ListEvents(feed_id),
			     QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete feed;
      return;
    }
    Feed::remove(feed_id);
    list_model->update();
    delete feed;
  }
}


void ListFeeds::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListFeeds::closeData()
{
  done(1);
}


void ListFeeds::resizeEvent(QResizeEvent *e)
{
  list_remarks_check->setGeometry(35,32,20,20);
  list_remarks_label->setGeometry(55,32,200,20);

  list_view->setGeometry(10,32+24,size().width()-20,size().height()-112-24);

  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);

  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);

  ListDialog::resizeEvent(e);
}


QString ListFeeds::ListEvents(int feed_id) const
{
  int count=0;
  QString ret="";

  QString sql=QString("select ID from EVENTS where ")+
    QString::asprintf("FEED_ID=%d",feed_id);
  QSqlQuery *q=new QSqlQuery(sql);
  while(q->next()) {
    count++;
  }
  delete q;
  if(count>0) {
    ret=QString::asprintf("\n\n%d ",count)+
      tr("events that use this feed will also be deleted.");
  }
  return ret;
}
