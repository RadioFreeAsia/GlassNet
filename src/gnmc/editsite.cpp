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

#include "editsite.h"
#include "site.h"

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
  return QSize(350,300);
}
  

int EditSite::exec(int site_id)
{
  edit_site_id=site_id;
  Site *site=new Site(site_id);
  setWindowTitle(tr("GlassNet - Edit Site")+QString().sprintf(" %d",site_id));

  edit_sitename_edit->setText(site->siteName());
  edit_remarks_text->setText(site->remarks());
  delete site;

  return QDialog::exec();
}


void EditSite::okData()
{
  Site *site=new Site(edit_site_id);

  site->setSiteName(edit_sitename_edit->text());
  site->setRemarks(edit_remarks_text->toPlainText());
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
  edit_sitename_edit->setGeometry(65,10,size().width()-75,20);

  edit_remarks_label->setGeometry(15,32,100,20);
  edit_remarks_text->setGeometry(10,52,size().width()-20,66);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
