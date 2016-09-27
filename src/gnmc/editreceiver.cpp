// editreceiver.cpp
//
// Edit a GlassNet Receiver
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

#include "db.h"
#include "editreceiver.h"
#include "globals.h"
#include "receiver.h"

EditReceiver::EditReceiver(QWidget *parent)
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
  for(int i=1;i<Receiver::TypeLast;i++) {
    edit_type_box->insertItem(-1,Receiver::typeString((Receiver::Type)i),
			      (Receiver::Type)i);
  }

  //
  // MAC Address
  //
  edit_mac_label=new QLabel(tr("MAC Address")+":",this);
  edit_mac_label->setFont(bold_font);
  edit_mac_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_mac_edit=new QLineEdit(this);
  edit_mac_edit->setMaxLength(17);

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


EditReceiver::~EditReceiver()
{
}


QSize EditReceiver::sizeHint() const
{
  return QSize(350,125);
}
  

int EditReceiver::exec(int *receiver_id)
{
  edit_receiver_id=receiver_id;
  if(*receiver_id>0) {
    setWindowTitle(tr("GlassNet - Edit Receiver")+
		   QString().sprintf(" %d",*receiver_id));
    Receiver *rcvr=new Receiver(*receiver_id);
    edit_type_box->setCurrentItemData(rcvr->type());
    edit_mac_edit->setText(rcvr->macAddress());
    delete rcvr;
  }
  else {
    setWindowTitle(tr("GlassNet - Edit Receiver [new]"));
    edit_mac_edit->setText("00:00:00:00:00:00");
  }
  return QDialog::exec();
}


void EditReceiver::okData()
{
  if(!Receiver::isMacAddress(edit_mac_edit->text())) {
    QMessageBox::warning(this,tr("GlassNet - Format Error"),
			 tr("The MAC address entry")+
			 " \""+edit_mac_edit->text()+"\" "+
			 tr("is mal-formatted."));
    return;
  }
  if(*edit_receiver_id<0) {
    *edit_receiver_id=Receiver::create((Receiver::Type)edit_type_box->currentItemData().toInt(),edit_mac_edit->text());
  }
  else {
    Receiver *rcvr=new Receiver(*edit_receiver_id);
    rcvr->setType((Receiver::Type)edit_type_box->currentItemData().toInt());
    rcvr->setMacAddress(edit_mac_edit->text());
    delete rcvr;
  }

  done(true);
}


void EditReceiver::cancelData()
{
  done(false);
}


void EditReceiver::resizeEvent(QResizeEvent *e)
{
  edit_type_label->setGeometry(10,10,120,20);
  edit_type_box->setGeometry(135,10,size().width()-160,20);

  edit_mac_label->setGeometry(10,32,120,20);
  edit_mac_edit->setGeometry(135,32,size().width()-160,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
