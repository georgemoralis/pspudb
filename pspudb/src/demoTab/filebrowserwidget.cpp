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

#include "filebrowserwidget.h"
#include <qsettings.h>
#include <QMessageBox>
#include "Settings.h"
#include <qdir.h>
#include <QStandardItemModel>
#include <QProcess>
#include "emulators/jpcsp.h"


fileBrowserWidget::fileBrowserWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QDir dir(theSettings->s_localdirpath);
	dir.mkpath(theSettings->s_localdirpath + "/Demos");

	int source=ui.sourceselectCombo->currentIndex();
	emit RefreshFilesTable(source);

}

fileBrowserWidget::~fileBrowserWidget()
{

}
void fileBrowserWidget::RefreshFilesTable(int index)
{
	ui.JPCSPbutton->setEnabled(true);
	ui.CPSPEMUbutton->setEnabled(true);
	ui.PSPEbutton->setEnabled(true);

	if(index==0) //selection is none clear table
	{
		 model = new QStandardItemModel(0, 5, ui.fileTable);
		 model->setHeaderData(0, Qt::Horizontal, tr("Icon"));
		 model->setHeaderData(1, Qt::Horizontal, tr("Demo Name"));
		 model->setHeaderData(2, Qt::Horizontal, tr("Title"));
		 model->setHeaderData(3, Qt::Horizontal, tr("Disc ID"));
		 model->setHeaderData(4, Qt::Horizontal, tr("FW"));

		 ui.fileTable->setModel(model);
		 model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());//clear data 
		 
	}
	else if(index==1) //local folder
	{
		if(scanLocaldir())
		 setupModel(appLocalList);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
		
	}
}
bool fileBrowserWidget::scanLocaldir()
{
    appLocalList.clear();
	if(!theSettings->s_localdirpath.isEmpty())
    {
		QDir dir(theSettings->s_localdirpath + "/Demos");
		if(dir.exists())
		{
			scanDir(dir.path(),appLocalList);
			return true;
		}
		else 
		{
			QString savedatapath=theSettings->s_localdirpath + "/Demos";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("Demo Folder not found"),
                                     "Local Demos folder not found.Are you sure that " + savedatapath + " exists?");
			
			return false;

		}

    }
	else  //local dir not set
	{
		QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("Local path setting not found"),
                                     "Local path not found. Set the local path in configuration menu");
		return false;
	}
}
void fileBrowserWidget::scanDir(QString dir, QList<files> &list)
{
    QFileInfo fi(dir);
    if (!fi.isDir())
    {
        return;
    }
    
    QDir directory(dir);
    
    QFileInfoList dirList = directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirList.size(); ++i)
    {
    	files app(dirList.at(i).filePath());
		if(app.isValid())
			list.append(app);
    }
}
void fileBrowserWidget::setupModel(QList<files> &list)
{
		 model = new QStandardItemModel(list.size(), 6, ui.fileTable);
     	 model->setHeaderData(0, Qt::Horizontal, tr("Icon"));
		 model->setHeaderData(1, Qt::Horizontal, tr("Demo Name"));
		 model->setHeaderData(2, Qt::Horizontal, tr("Title"));
		 model->setHeaderData(3, Qt::Horizontal, tr("Disc ID"));
		 model->setHeaderData(4, Qt::Horizontal, tr("FW"));

     ui.fileTable->setModel(model);

     QItemSelectionModel *selectionModel = new QItemSelectionModel(model,this);
     ui.fileTable->setSelectionModel(selectionModel);

	// ui.savesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui.fileTable->verticalHeader()->setDefaultSectionSize(40);
	ui.fileTable->setColumnWidth(0,72);
	ui.fileTable->setColumnWidth(1,150);
	ui.fileTable->setColumnWidth(2,180);
	ui.fileTable->setColumnWidth(3,100);
	ui.fileTable->setColumnWidth(4,50);
	ui.fileTable->hideColumn(5);

	 model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());//clear data
	 for (int i = 0; i < list.size(); i++)
	 {
		 model->insertRows(i, 1, QModelIndex());
		 //display icon0 in half size to save some space
		 int icon0width = list[i].getIcon().width();
		 int icon0height = list[i].getIcon().height();
		 model->setData(model->index(i, 0, QModelIndex()),QPixmap(list[i].getIcon().scaled(QSize(icon0width/2,icon0height/2))),Qt::DecorationRole);
		 model->setData(model->index(i, 1, QModelIndex()),list[i].getFileName());
		 model->setData(model->index(i, 2, QModelIndex()),list[i].getDemosTitle());
		 model->setData(model->index(i, 3, QModelIndex()),list[i].getDemosID());
		 model->setData(model->index(i, 4, QModelIndex()),list[i].getDemosFW());
		 model->setData(model->index(i, 5, QModelIndex()),list[i].getPath());
		 
	 }
	 connect(selectionModel, SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex)));
}
void fileBrowserWidget::onCurrentChanged(QModelIndex const &index)
{
	bootbinpath = index.sibling(index.row(), 5).data(Qt::DisplayRole).toString();
}
void fileBrowserWidget::RunJPCSP()
{
	Jpcsp::runJPCSP(Jpcsp::file,bootbinpath,this);
}
void fileBrowserWidget::RunCSPSPEMU()
{
	if(bootbinpath.length()<1)
	  return;
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_cspspemuPath.isNull() || theSettings->s_cspspemuPath.length()==0)
	{
		QMessageBox::critical(this,tr("pspudb"),tr("PSPEMU's (C#) path hasn't been set. Go to Configuration and set the path"));
	   return;
	}
	QDir::setCurrent(theSettings->s_cspspemuPath);//change it
	QProcess launcher(this);
    QStringList arguments;
	arguments << bootbinpath ;

	launcher.startDetached("cspspemu.exe", arguments); 

	QDir::setCurrent( sOldPath );//restore old directory
}
void fileBrowserWidget::RunPCSP()
{
	if(bootbinpath.length()<1)
	  return;
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_pcspPath.isNull() || theSettings->s_pcspPath.length()==0)
	{
       QMessageBox::critical(this,tr("pspudb"),tr("PCSP's path hasn't been set. Go to Configuration and set the path"));
	   return;
	}
	QDir::setCurrent(theSettings->s_pcspPath);//change it
	QProcess launcher(this);
    QStringList arguments;
	arguments << "-file" << bootbinpath;

	launcher.startDetached("pcsp.exe", arguments) ;

	QDir::setCurrent( sOldPath );//restore old directory
}
void fileBrowserWidget::RunPSPE()
{
	if(bootbinpath.length()<1)
	  return;
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_pspePath.isNull() || theSettings->s_pspePath.length()==0)
	{
       QMessageBox::critical(this,tr("pspudb"),tr("PSPE4ALL's path hasn't been set. Go to Configuration and set the path"));
	   return;
	}
	QDir::setCurrent(theSettings->s_pspePath);//change it
	QProcess launcher(this);
    QStringList arguments;
	arguments << "-file" << bootbinpath;

	launcher.startDetached("pspe4all.exe", arguments) ;

	QDir::setCurrent( sOldPath );//restore old directory
}