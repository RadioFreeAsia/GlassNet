// streamcmdserver.cpp
//
// Parse commands on connection-oriented protocols.
//
//   (C) Copyright 2012,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "streamcmdserver.h"

StreamCmdConnection::StreamCmdConnection(QTcpSocket *sock)
{
  conn_socket=sock;
}


StreamCmdConnection::~StreamCmdConnection()
{
  delete conn_socket;
}


QTcpSocket *StreamCmdConnection::socket()
{
  return conn_socket;
}


bool StreamCmdConnection::isConnected()
{
  if(conn_socket==NULL) {
    return false;
  }
  return conn_socket->state()==QAbstractSocket::ConnectedState;
}


void StreamCmdConnection::deleteLater()
{
  if(conn_socket!=NULL) {
    conn_socket->close();
  }
}




StreamCmdServer::StreamCmdServer(const std::map<int,QString> &cmd_table,
				 const std::map<int,int> &upper_table,
				 const std::map<int,int> &lower_table,
				 QTcpServer *server,QObject *parent)
  : QObject(parent)
{
  //
  // Tables
  //
  cmd_cmd_table=cmd_table;
  cmd_upper_table=upper_table;
  cmd_lower_table=lower_table;

  //
  // TCP Server
  //
  cmd_server=server;
  connect(cmd_server,SIGNAL(newConnection()),this,SLOT(newConnectionData()));

  //
  // Mappers
  //
  cmd_read_mapper=new QSignalMapper(this);
  connect(cmd_read_mapper,SIGNAL(mapped(int)),this,SLOT(readyReadData(int)));

  cmd_closed_mapper=new QSignalMapper(this);
  connect(cmd_closed_mapper,SIGNAL(mapped(int)),
	  this,SLOT(connectionClosedData(int)));

  cmd_pending_connected_mapper=new QSignalMapper(this);
  connect(cmd_pending_connected_mapper,SIGNAL(mapped(int)),
	  this,SLOT(pendingConnectedData(int)));

  //
  // Garbage Timer
  //
  cmd_garbage_timer=new QTimer(this);
  cmd_garbage_timer->setSingleShot(true);
  connect(cmd_garbage_timer,SIGNAL(timeout()),this,SLOT(collectGarbageData()));
}


StreamCmdServer::~StreamCmdServer()
{
  delete cmd_garbage_timer;
  delete cmd_read_mapper;
  delete cmd_closed_mapper;
  delete cmd_server;
}


QHostAddress StreamCmdServer::localAddress(int id) const
{
  return cmd_connections.at(id)->socket()->localAddress();
}


uint16_t StreamCmdServer::localPort(int id) const
{
  return cmd_connections.at(id)->socket()->localPort();
}


QHostAddress StreamCmdServer::peerAddress(int id) const
{
  return cmd_connections.at(id)->socket()->peerAddress();
}


uint16_t StreamCmdServer::peerPort(int id) const
{
  return cmd_connections.at(id)->socket()->peerPort();
}


void StreamCmdServer::sendCommand(int id,int cmd,const QStringList &args)
{
  QString str=cmd_cmd_table[cmd];
  for(int i=0;i<args.size();i++) {
    str+=QString(" ")+args[i];
  }
  str+="\r\n";
  cmd_connections.at(id)->socket()->write(str.toAscii(),str.length());
}


void StreamCmdServer::sendCommand(int cmd,const QStringList &args)
{
  for(std::map<int,StreamCmdConnection *>::iterator it=cmd_connections.begin();
      it!=cmd_connections.end();it++) {
    if(it->second->isConnected()) {
      sendCommand(it->first,cmd,args);
    }
    else {
      closeConnection(it->first);
    }
  }
}


void StreamCmdServer::connectToHost(const QString &hostname,uint16_t port)
{
  int new_id=-1;
  for(unsigned i=0;i<cmd_pending_sockets.size();i++) {
    if(cmd_pending_sockets[i]==NULL) {
      cmd_pending_sockets[i]=new QTcpSocket(this);
      new_id=i;
      break;
    }
  }
  if(new_id<0) {
    cmd_pending_sockets.push_back(new QTcpSocket(this));
    new_id=cmd_pending_sockets.size()-1;
  }
  connect(cmd_pending_sockets[new_id],SIGNAL(connected()),
	  cmd_pending_connected_mapper,SLOT(map()));
  cmd_pending_connected_mapper->setMapping(cmd_pending_sockets[new_id],new_id);
  cmd_pending_sockets[new_id]->connectToHost(hostname,port);
}


void StreamCmdServer::closeConnection(int id)
{
  cmd_connections.at(id)->deleteLater();
  cmd_garbage_timer->start(1);
}


void StreamCmdServer::newConnectionData()
{
  ProcessNewConnection(cmd_server->nextPendingConnection());
}


void StreamCmdServer::readyReadData(int id)
{
  int n=-1;
  char data[1500];

  n=cmd_connections.at(id)->socket()->read(data,1500);
  for(int i=0;i<n;i++) {
    if(data[i]==13) {
      ProcessCommand(id);
      cmd_connections.at(id)->buffer="";
    }
    else {
      if(data[i]!=10) {
	cmd_connections.at(id)->buffer+=data[i];
      }
    }
  }
}


void StreamCmdServer::connectionClosedData(int id)
{
  emit disconnected(id);
}


void StreamCmdServer::collectGarbageData()
{
  for(std::map<int,StreamCmdConnection *>::iterator it=cmd_connections.begin();
      it!=cmd_connections.end();it++) {
    if(!it->second->isConnected()) {
      delete it->second;
      cmd_connections.erase(it);
    }
  }
}


void StreamCmdServer::pendingConnectedData(int pending_id)
{
  cmd_pending_sockets[pending_id]->disconnect();
  ProcessNewConnection(cmd_pending_sockets[pending_id]);
  cmd_pending_sockets[pending_id]=NULL;
}


void StreamCmdServer::ProcessNewConnection(QTcpSocket *sock)
{
  cmd_connections[sock->socketDescriptor()]=new StreamCmdConnection(sock);
  cmd_read_mapper->setMapping(sock,sock->socketDescriptor());
  connect(sock,SIGNAL(readyRead()),cmd_read_mapper,SLOT(map()));
  cmd_closed_mapper->setMapping(sock,sock->socketDescriptor());
  connect(sock,SIGNAL(disconnected()),cmd_closed_mapper,SLOT(map()));
  emit connected(sock->socketDescriptor());
}


void StreamCmdServer::ProcessCommand(int id)
{
  QStringList cmds=cmd_connections.at(id)->buffer.split(" ");
  for(unsigned i=0;i<cmd_cmd_table.size();i++) {
    if(cmd_cmd_table[i]==cmds[0]) {
      if(((cmd_upper_table[i]<0)||((cmds.size()-1)<=cmd_upper_table[i]))&&
	 ((cmd_lower_table[i]<0)||((cmds.size()-1)>=cmd_lower_table[i]))) {
	QStringList args;
	for(int j=1;j<cmds.size();j++) {
	  args.push_back(cmds[j]);
	}
	emit commandReceived(id,i,args);
      }
    }
  }
}
