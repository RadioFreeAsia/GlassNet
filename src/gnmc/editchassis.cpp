// editchassis.cpp
//
// Edit a GlassNet Chassis
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

#include <vector>

#include "editchassis.h"
#include "receiver.h"

EditChassis::EditChassis(QWidget *parent)
  : QDialog(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);
  setMinimumSize(sizeHint());

  //
  // Type
  //
  edit_type_label=new QLabel(tr("Type")+":",this);
  edit_type_label->setFont(bold_font);
  edit_type_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_type_box=new ComboBox(this);
  for(int i=0;i<Chassis::TypeLast;i++) {
    edit_type_box->insertItem(-1,Chassis::typeString((Chassis::Type)i),
			      (Chassis::Type)i);
  }

  //
  // Serial Number
  //
  edit_serial_label=new QLabel(tr("Serial Number")+":",this);
  edit_serial_label->setFont(bold_font);
  edit_serial_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_serial_edit=new QLineEdit(this);
  edit_serial_edit->setMaxLength(32);

  //
  // Slot List
  //
  for(int i=0;i<MAX_RECEIVER_SLOTS;i++) {
    edit_receiver_label[i]=new QLabel(QString().sprintf("%d",i+1),this);
    edit_receiver_label[i]->setFont(bold_font);
    edit_receiver_label[i]->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    edit_receiver_box[i]=new ComboBox(this);
    connect(edit_receiver_box[i],SIGNAL(activated(int)),
	    this,SLOT(slotActivatedData(int)));
  }

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


EditChassis::~EditChassis()
{
}


QSize EditChassis::sizeHint() const
{
  return QSize(400,220);
}
  

int EditChassis::exec(int *chassis_id)
{
  edit_chassis_id=chassis_id;
  if(*chassis_id<0) {
    setWindowTitle(tr("GlassNet - Edit Chassis")+" "+tr("[new]"));
    edit_serial_edit->setText("");
  }
  else {
    Chassis *chassis=new Chassis(*chassis_id);
    setWindowTitle(tr("GlassNet - Edit Chassis")+
		   QString().sprintf(" %d",*chassis_id));
    edit_type_box->setCurrentItemData(chassis->type());
    edit_serial_edit->setText(chassis->serialNumber());
    for(int i=0;i<MAX_RECEIVER_SLOTS;i++) {
      if(i<Chassis::slotQuantity(chassis->type())) {
	Receiver *rcvr=new Receiver(*edit_chassis_id,i);
	edit_receiver_box[i]->clear();
	edit_receiver_box[i]->insertItem(-1,rcvr->description(),rcvr->id());
	edit_receiver_label[i]->setEnabled(true);
	edit_receiver_box[i]->setEnabled(true);
	delete rcvr;
      }
      else {
	edit_receiver_label[i]->setDisabled(true);
	edit_receiver_box[i]->setCurrentItemData(Receiver::TypeNone);
	edit_receiver_box[i]->setDisabled(true);
      }
    }
    delete chassis;
  }
  BuildReceiverLists();

  return QDialog::exec();
}


void EditChassis::slotActivatedData(int n)
{
  BuildReceiverLists();
}


void EditChassis::okData()
{
  Chassis *chassis=NULL;

  if(*edit_chassis_id<0) {
    *edit_chassis_id=Chassis::create((Chassis::Type)edit_type_box->currentItemData().toInt(),edit_serial_edit->text());
    chassis=new Chassis(*edit_chassis_id);
  }
  else {
    chassis=new Chassis(*edit_chassis_id);
    chassis->setType((Chassis::Type)edit_type_box->currentItemData().toInt());
    chassis->setSerialNumber(edit_serial_edit->text());
  }

  QString sql=QString("update RECEIVERS set ")+
    "CHASSIS_ID=null,"+
    "SLOT=null where "+
    QString().sprintf("CHASSIS_ID=%d",*edit_chassis_id);
  SqlQuery::run(sql);

  for(int i=0;i<Chassis::slotQuantity(chassis->type());i++) {
    sql=QString("update RECEIVERS set ")+
      QString().sprintf("CHASSIS_ID=%d,",*edit_chassis_id)+
      QString().sprintf("SLOT=%d where ",i)+
      QString().sprintf("ID=%d",
			edit_receiver_box[i]->currentItemData().toInt());
    SqlQuery::run(sql);
  }
  delete chassis;

  done(true);
}


void EditChassis::cancelData()
{
  done(false);
}


void EditChassis::resizeEvent(QResizeEvent *e)
{
  edit_type_label->setGeometry(10,10,115,20);
  edit_type_box->setGeometry(130,10,200,20);

  edit_serial_label->setGeometry(10,32,115,20);
  edit_serial_edit->setGeometry(130,32,size().width()-140,20);

  for(int i=0;i<MAX_RECEIVER_SLOTS;i++) {
    edit_receiver_label[i]->setGeometry(15,64+22*i,20,20);
    edit_receiver_box[i]->setGeometry(50,64+22*i,320,20);
  }

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditChassis::BuildReceiverLists()
{
  Chassis::Type type=(Chassis::Type)edit_type_box->currentItemData().toInt();
  std::vector<int> receiver_ids;

  //
  // Current Receiver Selections
  //
  for(int i=0;i<Chassis::slotQuantity(type);i++) {
    receiver_ids.push_back(edit_receiver_box[i]->currentItemData().toInt());
    edit_receiver_box[i]->clear();
    if(receiver_ids.back()>0) {
      Receiver *rcvr=new Receiver(receiver_ids.back());
      edit_receiver_box[i]->insertItem(-1,rcvr->description(),rcvr->id());
      delete rcvr;
    }
    edit_receiver_box[i]->insertItem(-1,tr("[empty]"),-1);
  }

  //
  // Get Set of Common Available Receivers
  //
  QString sql=QString("select ID from RECEIVERS where ")+
    "CHASSIS_ID is null and ";
  for(unsigned i=0;i<receiver_ids.size();i++) {
    sql+=QString().sprintf("ID!=%d and ",receiver_ids[i]);
  }
  sql=sql.left(sql.length()-5);
  SqlQuery *q=new SqlQuery(sql);
  while(q->next()) {
    Receiver *rcvr=new Receiver(q->value(0).toInt());
    for(int i=0;i<Chassis::slotQuantity(type);i++) {
      edit_receiver_box[i]->insertItem(-1,rcvr->description(),rcvr->id());
    }
    delete rcvr;
  }
  delete q;
}
