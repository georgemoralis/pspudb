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

#include "pspudb.h"
#include "fixupgames.h"
#include <QtSql>

extern QSqlDatabase dbadv;
fixupgames::fixupgames(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);





}

fixupgames::~fixupgames()
{

}

void fixupgames::scanImages()
{
	QSqlQuery query(dbadv);
	query.prepare("SELECT crc32,filesize,path FROM cache WHERE NOT EXISTS (SELECT images.crc FROM images WHERE cache.crc32=images.crc) AND available=1");
	query.exec();
	QSqlQueryModel *model = new QSqlQueryModel(ui.notDatabaseTree);
    model->setQuery(query);
	ui.notDatabaseTree->setModel(model); 
	ui.notDatabaseTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui.notDatabaseTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui.notDatabaseTree->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui.notDatabaseTree->show();
	int row = ui.notDatabaseTree->model()->rowCount();
	if(row>0)
		ui.fixImagesButton->setEnabled(true);
	
}
void fixupgames::fixupImages()
{
	int row = ui.notDatabaseTree->model()->rowCount();
	for(int i=0; i<row; i++)
	{
		paths << ui.notDatabaseTree->model()->sibling(i,2,QModelIndex()).data(Qt::DisplayRole).toString(); 
	}
	
	fTask= FixupImages;
	QDialog::accept();
}
void fixupgames::scanstatus()
{
	QSqlQuery query2(dbadv);
	query2.prepare("SELECT cache.crc32,cache.filesize,cache.path,bootbinstatus,ebootbinstatus FROM cache,images WHERE (bootbinstatus='<UNKN>' OR ebootbinstatus='<UNKN>')  AND cache.crc32=images.crc AND available=1");
	query2.exec();
	QSqlQueryModel *model2 = new QSqlQueryModel(ui.statusTree);
    model2->setQuery(query2);
	ui.statusTree->setModel(model2); 
	ui.statusTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui.statusTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui.statusTree->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui.statusTree->header()->setResizeMode(3, QHeaderView::ResizeToContents);
	ui.statusTree->header()->setResizeMode(4, QHeaderView::ResizeToContents);
	ui.statusTree->show();
	int row = ui.statusTree->model()->rowCount();
	if(row>0)
		ui.fixStatusButton->setEnabled(true);//if there are entries in table enable button
}
void fixupgames::fixupstatus()
{
	
	int row = ui.statusTree->model()->rowCount();
	for(int i=0; i<row; i++)
	{
		paths << ui.statusTree->model()->sibling(i,2,QModelIndex()).data(Qt::DisplayRole).toString(); 
	}
	fTask= FixupStatus;
	QDialog::accept();
}
QStringList fixupgames::getpaths()
{
	return paths;
}
void fixupgames::scanFW()
{
	QSqlQuery query2(dbadv);
	query2.prepare("SELECT cache.crc32,cache.filesize,cache.path,firmware FROM cache,images WHERE firmware='<UNKN>' AND cache.crc32=images.crc AND available=1");
	query2.exec();
	QSqlQueryModel *model2 = new QSqlQueryModel(ui.fwTree);
    model2->setQuery(query2);
	ui.fwTree->setModel(model2); 
	ui.fwTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui.fwTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui.fwTree->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui.fwTree->header()->setResizeMode(3, QHeaderView::ResizeToContents);
	ui.fwTree->show();
	int row = ui.fwTree->model()->rowCount();
	if(row>0)
		ui.fixFirmwareButton->setEnabled(true);//if there are entries in table enable button
}
void fixupgames::fixupFW()
{
	int row = ui.fwTree->model()->rowCount();
	for(int i=0; i<row; i++)
	{
		paths << ui.fwTree->model()->sibling(i,2,QModelIndex()).data(Qt::DisplayRole).toString(); 
	}
	fTask= FixupFW;
	QDialog::accept();
}