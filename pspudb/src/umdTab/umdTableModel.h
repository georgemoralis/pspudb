// Copyright (c) 2013- PSPUDB Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/georgemoralis/pspudb and http://www.pspudb.com/

#ifndef UMDSQLMODEL_H
#define UMDSQLMODEL_H

#include <QtGui/QMainWindow>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>

class umdTableModel : public QSqlQueryModel
 {
     Q_OBJECT

 public:
	umdTableModel::umdTableModel(QObject *parent)
     : QSqlQueryModel(parent)
 {
 }
 QVariant umdTableModel::headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role == Qt::DisplayRole)
     {
         if (orientation == Qt::Horizontal) {
             switch (section)
             {
			 case 0: 
				 return QString("GameID");
             case 1:
                 return QString("Title");
             case 2:
                 return QString("Disc ID");
             case 3:
                 return QString("Region");
			 case 4:
                 return QString("FW");
			 case 5:
				 return QString("Type");
             }
         }
     }
     return QVariant();
 }
 QVariant umdTableModel::data(const QModelIndex &index, int role) const
 {
     QVariant value = QSqlQueryModel::data(index, role);
	 if (role == Qt::DecorationRole)
     {
		switch(index.column())
		{
		  case 3:
			  QString region = index.data(Qt::DisplayRole).toString();
			  return QIcon(":/flags/" + region + ".png");
		}
	 }
     return value;
 }
 };

#endif
