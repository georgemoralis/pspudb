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
