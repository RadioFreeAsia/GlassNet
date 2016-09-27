// sqltablemodel.cpp
//
// Two dimensional, SQL-based data model for Rivendell.
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

#include <QColor>

#include "db.h"
#include "chassis.h"
#include "receiver.h"
#include "sqltablemodel.h"

SqlTableModel::SqlTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  model_columns=0;
}


SqlTableModel::~SqlTableModel()
{
}


QFont SqlTableModel::font() const
{
  return model_font;
}


void SqlTableModel::setFont(const QFont &font)
{
  model_font=font;
}


int SqlTableModel::columnCount(const QModelIndex &index) const
{
  return model_columns;
}


int SqlTableModel::rowCount(const QModelIndex &index) const
{
  return model_display_datas.size();
}


QVariant SqlTableModel::data(const QModelIndex &index,int role) const
{
  QVariant value;

  switch(role) {
  case Qt::DisplayRole:
    value=model_display_datas[index.row()][index.column()];
    switch(fieldType(index.column())) {
    case SqlTableModel::LengthType:
      return value;
      //      return QVariant(RDGetTimeLength(value.toInt(),false,true));

    case SqlTableModel::ColorTextType:
      return value;

    case SqlTableModel::AudioLevelType:
      return QVariant(QString().sprintf("%d",value.toInt()/100));

    case SqlTableModel::BooleanType:
      if(value.toBool()) {
	return tr("Yes");
      }
      return tr("No");

    case SqlTableModel::ChassisType:
      return QVariant(Chassis::typeString((Chassis::Type)value.toInt()));

    case SqlTableModel::ReceiverType:
      return QVariant(Receiver::typeString((Receiver::Type)value.toInt()));

    case SqlTableModel::NumericType:
    case SqlTableModel::DefaultType:
      if(value.isNull()) {
	return QVariant(tr("[none]"));
      }
      return value;
    }
    break;

  case Qt::ForegroundRole:
    switch(fieldType(index.column())) {
    case SqlTableModel::ColorTextType:
      return QVariant(model_display_datas[index.row()][model_field_key_columns.at(index.column())].value<QColor>());

    default:
      break;
    }
    break;

  case Qt::FontRole:
    switch(fieldType(index.column())) {
    case SqlTableModel::ColorTextType:
      return QVariant(QFont(font().family(),font().pointSize(),QFont::Bold));

    default:
      break;
    }

  case Qt::TextAlignmentRole:
    switch(fieldType(index.column())) {
    case SqlTableModel::AudioLevelType:
    case SqlTableModel::NumericType:
      return QVariant(Qt::AlignVCenter|Qt::AlignRight);

    case SqlTableModel::BooleanType:
      return QVariant(Qt::AlignCenter);

    default:
      break;
    }
    break;

  default:
    break;
  }
  return QVariant();
}


QVariant SqlTableModel::data(int row,int column,int role) const
{
  return data(index(row,column),role);
}


void SqlTableModel::setQuery(const QString &sql)
{
  model_display_datas.clear();

  SqlQuery *q=new SqlQuery(sql);
  model_columns=q->columns();
  while(q->next()) {
    std::vector<QVariant> row;
    for(int i=0;i<q->columns();i++) {
      row.push_back(q->value(i));
    }
    model_display_datas.push_back(row);
  }
  delete q;
  model_sql=sql;
  emit layoutChanged();
}


QVariant SqlTableModel::headerData(int section,Qt::Orientation orient,
					int role) const
{
  if((role==Qt::DisplayRole)&&(orient==Qt::Horizontal)) {
    if(GetHeader(section).isValid()) {
      return model_headers.at(section);
    }
    return QVariant(QString().sprintf("%d",section));
  }
  if((role==Qt::SizeHintRole)&&(orient==Qt::Vertical)) {
    return QVariant(QSize());
  }
  return QAbstractItemModel::headerData(section,orient,role);
}


bool SqlTableModel::setHeaderData(int section,Qt::Orientation orient,
				       const QVariant &value,int role)
{
  if(((role==Qt::DisplayRole)||(role==Qt::EditRole))&&
     (orient==Qt::Horizontal)) {
    model_headers[section]=value;
    emit headerDataChanged(orient,section,section);
  }
  return QAbstractItemModel::setHeaderData(section,orient,value,role);
}


SqlTableModel::FieldType SqlTableModel::fieldType(int section) const
{
  try {
    return model_field_types.at(section);
  }
  catch (...) {
    return SqlTableModel::DefaultType;
  }
}


void SqlTableModel::setFieldType(int section,SqlTableModel::FieldType type,
				   int key_col)
{
  model_field_types[section]=type;
  model_field_key_columns[section]=key_col;
}


void SqlTableModel::update()
{
  if(!model_sql.isEmpty()) {
    setQuery(model_sql);
  }
}


bool SqlTableModel::insertRows(int row,const QString &sql)
{
  if((row<0)||(row>(int)model_display_datas.size())) {
    return false;
  }
  SqlQuery *q=new SqlQuery(sql);
  if(q->size()>0) {
    beginInsertRows(QModelIndex(),row,row+q->size()-1);
    while(q->next()) {
      std::vector<QVariant> row;
      for(int i=0;i<model_columns;i++) {
	row.push_back(q->value(i));
      }
      model_display_datas.push_back(row);
    }
    endInsertRows();
  }
  delete q;
  return true;
}


bool SqlTableModel::removeRows(int row,int count,const QModelIndex &parent)
{
  if((row+count)>(int)model_display_datas.size()) {
    return false;
  }
  beginRemoveRows(QModelIndex(),row,row+count-1);
  for(int i=0;i<count;i++) {
    model_display_datas.erase(model_display_datas.begin()+row);
  }
  endRemoveRows();
  return true;
}


QVariant SqlTableModel::GetHeader(int section) const
{
  try {
    return model_headers.at(section);
  }
  catch(...) {
    return QVariant();
  }
  return QVariant();
}
