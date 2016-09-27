// gnmc.cpp
//
// gnmc(1) Management client for GlassNet
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

#include <QApplication>
#include <QMessageBox>

#include "globals.h"
#include "gnmc.h"
#include "mysqlconfig.h"

MainWidget::MainWidget(QWidget *parent)
  : QMainWindow(parent)
{
  QString err_msg;

  setWindowTitle(tr("GlassNet Management Client")+" v"+VERSION);

  QFont label_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Open (and perhaps create) Database
  //
  gnmc_config=new Config();
  if(!gnmc_config->openDb(&err_msg)) {
    QMessageBox::warning(this,tr("GlassNet - Error"),
			 tr("Unable to open database")+" ["+err_msg+"].");
    exit(256);
  }
  if(!CheckDb(&err_msg)) {
    QMessageBox::warning(this,tr("GlassNet - Error"),
			 tr("Unable to open database")+" ["+err_msg+"].");
    exit(256);
  }

  //
  // Dialogs
  //
  gnmc_login_dialog=new Login(this);
  if(!gnmc_login_dialog->exec(&gnmc_user_id)) {
    exit(0);
  }
  gnmc_listchassis_dialog=new ListChassis(this);
  gnmc_listreceivers_dialog=new ListReceivers(this);
  gnmc_listsites_dialog=new ListSites(this);
  gnmc_listusers_dialog=new ListUsers(this);

  //
  // Name/Description Labels
  //
  gnmc_name_label=new QLabel(global_user->fullName(),this);
  gnmc_name_label->setFont(label_font);
  gnmc_name_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
  gnmc_description_label=new QLabel(global_user->description(),this);
  gnmc_description_label->setFont(label_font);
  gnmc_description_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

  //
  // Manage Users Button
  //
  gnmc_users_button=new QPushButton(tr("Manage")+"\n"+tr("Users"),this);
  gnmc_users_button->setFont(label_font);
  gnmc_users_button->setEnabled(global_user->userPriv());
  connect(gnmc_users_button,SIGNAL(clicked()),this,SLOT(usersData()));

  //
  // Manage Sites Button
  //
  gnmc_sites_button=new QPushButton(tr("Manage")+"\n"+tr("Sites"),this);
  gnmc_sites_button->setFont(label_font);
  gnmc_sites_button->setEnabled(global_user->sitePriv());
  connect(gnmc_sites_button,SIGNAL(clicked()),
	  gnmc_listsites_dialog,SLOT(exec()));

  //
  // Manage Chassis Button
  //
  gnmc_chassis_button=new QPushButton(tr("Manage")+"\n"+tr("Chassis"),this);
  gnmc_chassis_button->setFont(label_font);
  gnmc_chassis_button->setEnabled(global_user->sitePriv());
  connect(gnmc_chassis_button,SIGNAL(clicked()),
  	  gnmc_listchassis_dialog,SLOT(exec()));

  //
  // Manage Receivers Button
  //
  gnmc_receivers_button=new QPushButton(tr("Manage")+"\n"+tr("Receivers"),this);
  gnmc_receivers_button->setFont(label_font);
  gnmc_receivers_button->setEnabled(global_user->sitePriv());
  connect(gnmc_receivers_button,SIGNAL(clicked()),
  	  gnmc_listreceivers_dialog,SLOT(exec()));

  //
  // Manage Events Button
  //
  gnmc_events_button=new QPushButton(tr("Manage")+"\n"+tr("Events"),this);
  gnmc_events_button->setFont(label_font);
  gnmc_events_button->setEnabled(global_user->eventPriv());
  //  connect(gnmc_events_button,SIGNAL(clicked()),this,SLOT(eventsData()));

  //
  // Close Button
  //
  gnmc_close_button=new QPushButton(tr("Close"),this);
  gnmc_close_button->setFont(label_font);
  connect(gnmc_close_button,SIGNAL(clicked()),qApp,SLOT(quit()));
}


QSize MainWidget::sizeHint() const
{
  return QSize(460,190);
}


void MainWidget::usersData()
{
  gnmc_listusers_dialog->exec();
  gnmc_users_button->setEnabled(global_user->userPriv());
  gnmc_sites_button->setEnabled(global_user->sitePriv());
  gnmc_chassis_button->setEnabled(global_user->sitePriv());
  gnmc_receivers_button->setEnabled(global_user->sitePriv());
  gnmc_events_button->setEnabled(global_user->eventPriv());
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  gnmc_name_label->setGeometry(10,5,size().width(),20);
  gnmc_description_label->setGeometry(10,25,size().width(),20);

  gnmc_users_button->setGeometry(10,54,80,60);
  gnmc_sites_button->setGeometry(100,54,80,60);
  gnmc_chassis_button->setGeometry(190,54,80,60);
  gnmc_receivers_button->setGeometry(280,54,80,60);
  gnmc_events_button->setGeometry(370,54,80,60);

  gnmc_close_button->setGeometry(45,125,size().width()-90,60);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);

  MainWidget *w=new MainWidget();
  w->show();

  return a.exec();
}
