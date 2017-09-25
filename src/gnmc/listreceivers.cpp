// listreceivers.cpp
//
// List GlassNet Receivers
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

#include "chassis.h"
#include "globals.h"
#include "listreceivers.h"
#include "receiver.h"
#include "site.h"
#include "user.h"

ListReceivers::ListReceivers(QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle(tr("GlassNet - List Receivers"));
  setMinimumSize(sizeHint());

  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Dialogs
  //
  list_editreceiver_dialog=new EditReceiver(this);

  list_model=new SqlTableModel(this);
  QString sql=QString("select ")+
    "RECEIVERS.ID,"+
    "RECEIVERS.ONLINE,"+
    "SITES.NAME,"+
    "CHASSIS.SLOT,"+
    "RECEIVERS.SLOT,"+
    "RECEIVERS.TYPE,"+
    "RECEIVERS.MAC_ADDRESS,"+
    "RECEIVERS.LAST_SEEN,"+
    "RECEIVERS.PUBLIC_ADDRESS,"+
    "RECEIVERS.INTERFACE_ADDRESS,"+
    "RECEIVERS.FIRMWARE_VERSION,"+
    "RECEIVERS.UPDATE_FIRMWARE "+
    "from RECEIVERS left join CHASSIS "+
    "on RECEIVERS.CHASSIS_ID=CHASSIS.ID left join SITES "+
    "on CHASSIS.SITE_ID=SITES.ID "+
    "order by SITES.NAME,RECEIVERS.CHASSIS_ID,RECEIVERS.SLOT,"+
    "RECEIVERS.MAC_ADDRESS";
  list_model->setQuery(sql);
  list_model->setHeaderData(0,Qt::Horizontal,tr("Rcvr ID"));
  list_model->setHeaderData(1,Qt::Horizontal,"");
  list_model->setFieldType(1,SqlTableModel::TriStateType);
  list_model->setHeaderData(2,Qt::Horizontal,tr("Site"));
  list_model->setFieldType(2,SqlTableModel::DefaultType);
  list_model->setHeaderData(3,Qt::Horizontal,tr("Chassis"));
  list_model->setFieldType(3,SqlTableModel::NumericType);
  list_model->setHeaderData(4,Qt::Horizontal,tr("Slot"));
  list_model->setFieldType(4,SqlTableModel::NumericType);
  list_model->setHeaderData(5,Qt::Horizontal,tr("Type"));
  list_model->setFieldType(5,SqlTableModel::ReceiverType);
  list_model->setHeaderData(6,Qt::Horizontal,tr("MAC Addr"));
  list_model->setHeaderData(7,Qt::Horizontal,tr("Last Seen"));
  list_model->setNullText(7,tr("[never]"));
  list_model->setHeaderData(8,Qt::Horizontal,tr("Public Addr"));
  list_model->setHeaderData(9,Qt::Horizontal,tr("Iface Addr"));
  list_model->setHeaderData(10,Qt::Horizontal,tr("Firmware"));
  list_model->setHeaderData(11,Qt::Horizontal,tr("Update Pending"));
  list_model->setFieldType(11,SqlTableModel::BooleanType);
  list_view=new TableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(0);
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

  list_update_button=new QPushButton(tr("Update")+"\n"+tr("Firmware"),this);
  list_update_button->setFont(bold_font);
  connect(list_update_button,SIGNAL(clicked()),this,SLOT(updateData()));

  list_close_button=new QPushButton(tr("Close"),this);
  list_close_button->setFont(bold_font);
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Update Timer
  //
  list_update_timer=new QTimer(this);
  connect(list_update_timer,SIGNAL(timeout()),list_model,SLOT(update()));
}


ListReceivers::~ListReceivers()
{
}


QSize ListReceivers::sizeHint() const
{
  return QSize(850,600);
}


int ListReceivers::exec()
{
  QString sql=QString("select ")+
    "RECEIVERS.ID,"+
    "RECEIVERS.ONLINE,"+
    "SITES.NAME,"+
    "CHASSIS.SLOT,"+
    "RECEIVERS.SLOT,"+
    "RECEIVERS.TYPE,"+
    "RECEIVERS.MAC_ADDRESS,"+
    "RECEIVERS.LAST_SEEN,"+
    "RECEIVERS.PUBLIC_ADDRESS,"+
    "RECEIVERS.INTERFACE_ADDRESS,"+
    "RECEIVERS.FIRMWARE_VERSION,"+
    "RECEIVERS.UPDATE_FIRMWARE "+
    "from RECEIVERS left join CHASSIS "+
    "on RECEIVERS.CHASSIS_ID=CHASSIS.ID left join SITES "+
    "on CHASSIS.SITE_ID=SITES.ID "+
    "order by SITES.NAME,RECEIVERS.CHASSIS_ID,RECEIVERS.SLOT,"+
    "RECEIVERS.MAC_ADDRESS";
  list_model->setQuery(sql);
  list_view->resizeColumnsToContents();
  list_update_timer->start(5000);
  return QDialog::exec();
}


int ListReceivers::exec(const QString &mac)
{
  QString sql=QString("select ")+
    "RECEIVERS.ID,"+
    "RECEIVERS.ONLINE,"+
    "SITES.NAME,"+
    "CHASSIS.SLOT,"+
    "RECEIVERS.SLOT,"+
    "RECEIVERS.TYPE,"+
    "RECEIVERS.MAC_ADDRESS,"+
    "RECEIVERS.LAST_SEEN,"+
    "RECEIVERS.PUBLIC_ADDRESS,"+
    "RECEIVERS.INTERFACE_ADDRESS,"+
    "RECEIVERS.FIRMWARE_VERSION,"+
    "RECEIVERS.UPDATE_FIRMWARE "+
    "from RECEIVERS left join CHASSIS "+
    "on RECEIVERS.CHASSIS_ID=CHASSIS.ID left join SITES "+
    "on CHASSIS.SITE_ID=SITES.ID where "+
    "RECEIVERS.MAC_ADDRESS=\""+SqlQuery::escape(mac)+"\" "+
    "order by SITES.NAME,RECEIVERS.CHASSIS_ID,RECEIVERS.SLOT,"+
    "RECEIVERS.MAC_ADDRESS";
  list_model->setQuery(sql);
  list_view->resizeColumnsToContents();
  list_update_timer->start(5000);
  return QDialog::exec();
}


void ListReceivers::addData()
{
  int receiver_id=-1;
  if(list_editreceiver_dialog->exec(&receiver_id)) {
    list_model->update();
    list_view->resizeColumnsToContents();
    list_view->select(0,receiver_id);
  }
}


void ListReceivers::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int receiver_id=s->selectedRows()[0].data().toInt();
    if(list_editreceiver_dialog->exec(&receiver_id)) {
      list_model->update();
      list_view->resizeColumnsToContents();
    }
  }
}


void ListReceivers::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int receiver_id=s->selectedRows()[0].data().toInt();
    Receiver *rcvr=new Receiver(receiver_id);

    QString sql=QString("select ")+
      "CHASSIS.ID,"+
      "CHASSIS.SITE_ID from "+
      "CHASSIS left join RECEIVERS "+
      "on CHASSIS.ID=RECEIVERS.CHASSIS_ID where "+
      QString().sprintf("RECEIVERS.ID=%d",receiver_id);
    SqlQuery *q=new SqlQuery(sql);
    if(q->first()) {
      Chassis *chassis=new Chassis(q->value(0).toInt());
      if(Site::exists(q->value(1).toInt())) {
	Site *site=new Site(q->value(1).toInt());
	QMessageBox::warning(this,tr("GlassNet - Error"),
			     tr("Receiver is in use in chassis")+
			     " \""+chassis->description()+"\" "+
			     tr("at site")+" \""+site->siteName()+"\".");
	delete site;
      }
      else {
	QMessageBox::warning(this,tr("GlassNet - Error"),
			     tr("Receiver is in use in undeployed chassis")+
			     " \""+chassis->description()+"\".");
      }
      delete chassis;
      delete q;
      return;
    }
    delete q;
    if(QMessageBox::question(this,tr("GlassNet - Delete Receiver"),
			     tr("Are you sure you want to delete receiver")+
			     " \""+Receiver::typeString(rcvr->type())+
			     " ["+rcvr->macAddress()+"]\"?",
			     QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete rcvr;
      return;
    }
    Receiver::remove(receiver_id);
    list_model->update();
    delete rcvr;
  }
}


void ListReceivers::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListReceivers::updateData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int receiver_id=s->selectedRows()[0].data().toInt();
    Receiver *rcvr=new Receiver(receiver_id);
    if(QMessageBox::question(this,tr("GlassNet - Receiver Reboot"),
			     tr("This will cause the receiver to be rebooted.")+
			     "\n"+tr("Continue?"),
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      delete rcvr;
      return;
    }
    rcvr->setUpdateFirmware(true);
    delete rcvr;
    list_model->update();
  }
}


void ListReceivers::closeData()
{
  list_update_timer->stop();
  done(1);
}


void ListReceivers::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,10,size().width()-20,size().height()-80);

  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);

  list_update_button->setGeometry(350,size().height()-60,80,50);

  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
