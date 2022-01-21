// listsites.cpp
//
// List GlassNet Sites
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

#include <QMessageBox>

#include "globals.h"
#include "listsites.h"
#include "site.h"
#include "user.h"

ListSites::ListSites(QWidget *parent)
  : ListDialog(false,parent)
{
  setWindowTitle(tr("GlassNet - List Sites"));
  setMinimumSize(sizeHint());

  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Dialogs
  //
  list_editsite_dialog=new EditSite(this);

  list_model=new SqlTableModel(this);
  QString sql=QString("select ")+
    "`ID`,"+
    "`NAME` "+
    "from `SITES` order by "+
    "`NAME`";
  list_model->setQuery(sql);
  list_model->setHeaderData(0,Qt::Horizontal,tr("Site ID"));
  list_model->setHeaderData(1,Qt::Horizontal,tr("Name"));
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


ListSites::~ListSites()
{
}


QSize ListSites::sizeHint() const
{
  return QSize(400,300);
}


int ListSites::exec()
{
  list_model->update();
  list_view->resizeColumnsToContents();
  return QDialog::exec();
}


void ListSites::addData()
{
  int site_id=-1;

  if(list_editsite_dialog->exec(&site_id)) {
    list_model->update();
    list_view->select(0,site_id);
    list_view->resizeColumnsToContents();
  }
  else {
    Site::remove(site_id);
  }
}


void ListSites::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int site_id=s->selectedRows()[0].data().toInt();
    if(list_editsite_dialog->exec(&site_id)) {
      list_model->update();
    }
  }
}


void ListSites::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int site_id=s->selectedRows()[0].data().toInt();
    Site *site=new Site(site_id);
    if(QMessageBox::question(this,tr("GlassNet - Delete Site"),
			     tr("Are you sure you want to delete site")+
			     " \""+site->siteName()+"\"?\n"+
			     tr("This will remove all associated receivers as well."), 
			     QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete site;
      return;
    }
    Site::remove(site_id);
    list_model->update();
    delete site;
  }
}


void ListSites::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListSites::closeData()
{
  done(1);
}


void ListSites::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,32,size().width()-20,size().height()-112);

  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);

  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);

  ListDialog::resizeEvent(e);
}
