// editsite.cpp
//
// Edit a GlassNet Site
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

#include "chassis.h"
#include "editsite.h"

EditSite::EditSite(QWidget *parent)
  : QDialog(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);
  setMinimumSize(sizeHint());

  //
  // Name
  //
  edit_sitename_label=new QLabel(tr("Name")+":",this);
  edit_sitename_label->setFont(bold_font);
  edit_sitename_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_sitename_edit=new QLineEdit(this);

  //
  // Remarks
  //
  edit_remarks_label=new QLabel(tr("Remarks"),this);
  edit_remarks_label->setFont(bold_font);
  edit_remarks_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  edit_remarks_text=new QTextEdit(this);
  edit_remarks_text->setAcceptRichText(false);

  //
  // Slot List
  //
  edit_slots_label=new QLabel(tr("Chassis"),this);
  edit_slots_label->setFont(bold_font);
  edit_slots_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  for(int i=0;i<MAX_RECEIVER_SLOTS;i++) {
    edit_chassis_label[i]=new QLabel(QString().sprintf("%d",i+1),this);
    edit_chassis_label[i]->setFont(bold_font);
    edit_chassis_label[i]->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    edit_chassis_box[i]=new ComboBox(this);
    connect(edit_chassis_box[i],SIGNAL(activated(int)),
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


EditSite::~EditSite()
{
}


QSize EditSite::sizeHint() const
{
  return QSize(500,310);
}
  

int EditSite::exec(int *site_id)
{
  edit_site_id=site_id;
  if(*site_id<0) {
    setWindowTitle(tr("GlassNet - Edit Site")+" "+tr("[new]"));
    edit_sitename_edit->setText(tr("[new site]"));
    edit_remarks_text->setText("");
  }
  else {
    Site *site=new Site(*site_id);
    setWindowTitle(tr("GlassNet - Edit Site")+
		   QString().sprintf(" %d",*site_id));
    edit_sitename_edit->setText(site->siteName());
    edit_remarks_text->setText(site->remarks());
    for(int i=0;i<MAX_CHASSIS_SLOTS;i++) {
      edit_chassis_box[i]->clear();
      if(Chassis::exists(*edit_site_id,i)) {
	Chassis *chassis=new Chassis(*edit_site_id,i);
	edit_chassis_box[i]->
	  insertItem(-1,chassis->description(),chassis->id());
	delete chassis;
      }
      else {
	edit_chassis_box[i]->insertItem(-1,"[none]",Chassis::TypeNone);
      }
    }
    delete site;
  }
  BuildChassisLists();

  return QDialog::exec();
}


void EditSite::slotActivatedData(int n)
{
  BuildChassisLists();
}


void EditSite::okData()
{
  if(*edit_site_id<0) {
    *edit_site_id=Site::create();
  }
  Site *site=new Site(*edit_site_id);
  site->setSiteName(edit_sitename_edit->text());
  site->setRemarks(edit_remarks_text->toPlainText());

  QString sql=QString("update CHASSIS set ")+
    "SITE_ID=null,"+
    "SLOT=null where "+
    QString().sprintf("SITE_ID=%d",*edit_site_id);
  SqlQuery::run(sql);

  for(int i=0;i<MAX_CHASSIS_SLOTS;i++) {
    sql=QString("update CHASSIS set ")+
      QString().sprintf("SITE_ID=%d,",*edit_site_id)+
      QString().sprintf("SLOT=%d where ",i)+
      QString().sprintf("ID=%d",
			edit_chassis_box[i]->currentItemData().toInt());
    SqlQuery::run(sql);
  }
  delete site;

  done(true);
}


void EditSite::cancelData()
{
  done(false);
}


void EditSite::resizeEvent(QResizeEvent *e)
{
  edit_sitename_label->setGeometry(10,10,50,20);
  edit_sitename_edit->setGeometry(65,10,200,20);

  edit_remarks_label->setGeometry(10,34,100,20);
  edit_remarks_text->setGeometry(10,56,size().width()-20,66);

  edit_slots_label->setGeometry(10,128,size().width()-20,20);
  for(int i=0;i<MAX_CHASSIS_SLOTS;i++) {
    edit_chassis_label[i]->setGeometry(15,150+22*i,20,20);
    edit_chassis_box[i]->setGeometry(50,150+22*i,320,20);
  }

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditSite::BuildChassisLists()
{
  std::vector<int> chassis_ids;

  //
  // Current Chassis Selections
  //
  for(int i=0;i<MAX_CHASSIS_SLOTS;i++) {
    chassis_ids.push_back(edit_chassis_box[i]->currentItemData().toInt());
    edit_chassis_box[i]->clear();
    if(chassis_ids.back()>0) {
      Chassis *chassis=new Chassis(chassis_ids.back());
      edit_chassis_box[i]->insertItem(-1,chassis->description(),chassis->id());
      delete chassis;
    }
    edit_chassis_box[i]->insertItem(-1,tr("[none]"),-1);
  }

  //
  // Get Set of Common Available Chassis
  //
  QString sql=QString("select ID from CHASSIS where ")+
    "SITE_ID is null and ";
  for(unsigned i=0;i<chassis_ids.size();i++) {
    sql+=QString().sprintf("ID!=%d and ",chassis_ids[i]);
  }
  sql=sql.left(sql.length()-5);
  SqlQuery *q=new SqlQuery(sql);
  while(q->next()) {
    Chassis *chassis=new Chassis(q->value(0).toInt());
    for(int i=0;i<MAX_CHASSIS_SLOTS;i++) {
      edit_chassis_box[i]->insertItem(-1,chassis->description(),chassis->id());
    }
    delete chassis;
  }
  delete q;
}
