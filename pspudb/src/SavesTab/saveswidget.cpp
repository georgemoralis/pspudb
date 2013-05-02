#include <qsettings.h>
#include <QStandardItemModel>
#include "saveswidget.h"
#include "Settings.h"
#include <QMessageBox>
#include <QDir>
#include <QMenu>
#include <QErrorMessage>
//#include <filemanager.h>


savesWidget::savesWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//get combobox
	int source=ui.sourceselectCombo->currentIndex();
	emit RefreshSaveTable(source);
		QDir dir(theSettings->s_localdirpath);
	dir.mkpath(theSettings->s_localdirpath + "/Saves");
	if(theSettings->s_pspePath.length()>0)
	   dir.mkpath(QDir::homePath() + "/pspe4all/vfs/ms0/PSP/SAVEDATA");


}

savesWidget::~savesWidget()
{

}
 void savesWidget::setupModel(QList<Savegame> &list)
 {
	 model = new QStandardItemModel(list.size(), 6, ui.savesTable);
     model->setHeaderData(0, Qt::Horizontal, tr("Icon"));
	 model->setHeaderData(1, Qt::Horizontal, tr("Directory"));
     model->setHeaderData(2, Qt::Horizontal, tr("Game Title"));
	 model->setHeaderData(3, Qt::Horizontal, tr("SaveData Title"));
	 model->setHeaderData(4, Qt::Horizontal, tr("SaveData Detail"));

     ui.savesTable->setModel(model);

     QItemSelectionModel *selectionModel = new QItemSelectionModel(model);
     ui.savesTable->setSelectionModel(selectionModel);

	// ui.savesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	 ui.savesTable->verticalHeader()->setDefaultSectionSize(40);
	 ui.savesTable->setColumnWidth(0,72);
	 ui.savesTable->setColumnWidth(1,120);
	 ui.savesTable->setColumnWidth(2,180);
	 ui.savesTable->setColumnWidth(3,180);
	 ui.savesTable->hideColumn(4);
	 ui.savesTable->hideColumn(5);

	 model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());//clear data
	 for (int i = 0; i < list.size(); i++)
	 {
		 model->insertRows(i, 1, QModelIndex());
		 //display icon0 in half size to save some space
		 int icon0width = list[i].getIcon().width();
		 int icon0height = list[i].getIcon().height();
		 model->setData(model->index(i, 0, QModelIndex()),QPixmap(list[i].getIcon().scaled(QSize(icon0width/2,icon0height/2))),Qt::DecorationRole);
		 model->setData(model->index(i, 1, QModelIndex()),list[i].getSaveDirectory());
		 model->setData(model->index(i, 2, QModelIndex()),list[i].getName());
		 model->setData(model->index(i, 3, QModelIndex()),list[i].getSaveDataTitle());
		 
		 model->setData(model->index(i, 4),list[i].getSaveDataDetail());
		 model->setData(model->index(i,5),list[i].getPath());
	 }
	 ui.savesTable->setContextMenuPolicy(Qt::CustomContextMenu);
	 connect(selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(onSelectionChanged(QItemSelection)));
	 connect(ui.savesTable, SIGNAL(customContextMenuRequested( const QPoint& )), this, SLOT(showCollectionContextMenu(const QPoint &)));
 }
 
 void savesWidget::onSelectionChanged(QItemSelection const &selection)
 {
	 QItemSelectionModel *select= NULL;
	 select = ui.savesTable->selectionModel();
	 QModelIndexList list=select->selectedRows();
	 if(list.size()==1)//display only in single selection
		 ui.savedatalabel->setText(list[0].sibling(list[0].row(), 4).data(Qt::DisplayRole).toString()); 
	 else
		ui.savedatalabel->setText("");
 }

//Scans a directory for PSP savegames and adds them on the list
void savesWidget::scanDir(QString dir, QList<Savegame> &list)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   // qDebug() << "Scanning" << dir << "for savegames";
    QFileInfo fi(dir);
    if (!fi.isDir())
    {
        return;
    }
    
    QDir directory(dir);
    
    QFileInfoList dirList = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirList.size(); ++i)
    {
       // try
        //{
            Savegame s(dirList.at(i).filePath());
            list.append(s);
       // }
       // catch(QPSPManagerPBPException e)
       // {
       //     qDebug() << dirList.at(i).filePath() << "is not a savegame";
       // }
    }
	QApplication::restoreOverrideCursor();
}


bool savesWidget::scanJPCSPdir()
{
    m_saveJPCSPList.clear();
	if(!theSettings->s_jpcspPath.isEmpty())
    {
		QDir dir(theSettings->s_jpcspPath + "/ms0/PSP/SAVEDATA");
		if(dir.exists())
		{
			scanDir(dir.path(),m_saveJPCSPList);
			return true;
		}
		else //jpcsp ms0 folder not found
		{
			QString savedatapath=theSettings->s_jpcspPath + "/ms0/PSP/SAVEDATA";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("ms0 Folder not found"),
                                     "JPCSP savedata folder not found.Are you sure that " + savedatapath + " exists?");
			return false;

		}

    }
	else  //jpcsp dir not set
	{
		QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("JPCSP emulator path not found"),
                                     "JPCSP emulator not found. Set the path of jpcsp emulator in configuration menu");
		return false;
	}
}
bool savesWidget::scanPCSPdir()
{
	m_savePCSPList.clear();
	if(!theSettings->s_pcspPath.isEmpty())
    {
		QDir dir(theSettings->s_pcspPath + "/ms0/PSP/SAVEDATA");
		if(dir.exists())
		{
			scanDir(dir.path(),m_savePCSPList);
			return true;
		}
		else //jpcsp ms0 folder not found
		{
			QString savedatapath=theSettings->s_pcspPath + "/ms0/PSP/SAVEDATA";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("ms0 Folder not found"),
                                     "PCSP savedata folder not found.Are you sure that " + savedatapath + " exists?");
			return false;

		}

    }
	else  //jpcsp dir not set
	{
		QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("PCSP emulator path not found"),
                                     "PCSP emulator not found. Set the path of pcsp emulator in configuration menu");
		return false;
	}
}
bool savesWidget::scanPSPEdir()
{
	m_savePSPEList.clear();
	if(!theSettings->s_pspePath.isEmpty())
    {
		QDir dir(QDir::homePath() + "/pspe4all/vfs/ms0/PSP/SAVEDATA");
		if(dir.exists())
		{
			scanDir(dir.path(),m_savePSPEList);
			return true;
		}
		else //jpcsp ms0 folder not found
		{
			QString savedatapath=QDir::homePath() + "/pspe4all/vfs/ms0/PSP/SAVEDATA";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("ms0 Folder not found"),
                                     "PSPE4ALL savedata folder not found.Are you sure that " + savedatapath + " exists?");
			return false;

		}

    }
	else  //jpcsp dir not set
	{
		QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("PSPE4ALL emulator path not found"),
                                     "PSPE4ALL emulator not found. Set the path of pcsp emulator in configuration menu");
		return false;
	}
}
bool savesWidget::scanLocalDir()
{
	 m_savePSPLocal.clear();
	if(!theSettings->s_localdirpath.isEmpty())
    {
		QDir dir(theSettings->s_localdirpath + "/Saves");
		if(dir.exists())
		{
			scanDir(dir.path(),m_savePSPLocal);
			return true;
		}
		else //jpcsp ms0 folder not found
		{
			QString savedatapath=theSettings->s_localdirpath + "/Saves";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("Saves Folder not found"),
                                     "Local Saves folder not found.Are you sure that " + savedatapath + " exists?");
			
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
bool savesWidget::scanPSPdir()
{
	m_savePSPList.clear();
	if(!theSettings->s_pspdevicePath.isEmpty())
    {
		QString PSPSaveDir = getPSPSavegameDirectory(theSettings->s_pspdevicePath);
		if(PSPSaveDir.length()>0)
		{
			scanDir(PSPSaveDir, m_savePSPList);
			return true;
		}
		else //PSP ms0 folder not found
		{
			//QString savedatapath=theSettings->s_pcspPath + "/ms0/PSP/SAVEDATA";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("ms0 Folder not found"),
                                     "PSP's savedata folder not found.Are you sure that " + PSPSaveDir + " exists?");
			return false;

		}

    }
	else  //pspdev dir not set
	{
		QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("PSP device path not found"),
                                     "PSP device not found. Set the path of PSP device in configuration menu");
		return false;
	}
	
}

QString savesWidget::getPSPSavegameDirectory(QString PSPDirectory)
{
    QFileInfo fi(PSPDirectory);
    if (!fi.isDir())
    {
        return "";
    }
    
    QString saveDir = "savedata";
    
    QDir directory(PSPDirectory);

    QFileInfoList dirList;
    dirList = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (int i = 0; i < dirList.size(); ++i) 
    {
        if (dirList.at(i).fileName().toLower() == "psp")
        {
            QDir pspdir(dirList.at(i).filePath());
            QFileInfoList pspDirList = pspdir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (int j = 0; j < pspDirList.size(); j++)
            {
                if (pspDirList.at(j).fileName().toLower() == saveDir)
                {
                    //Bingo!
                    return pspDirList.at(j).filePath();
                }
            }
        }
    }
    
    return "";
}
void savesWidget::RefreshSaveTable(int index)
{
	if(index==0) //selection is none clear table
	{
	     model = new QStandardItemModel(0, 4, ui.savesTable);
		 model->setHeaderData(0, Qt::Horizontal, tr("Icon"));
		 model->setHeaderData(1, Qt::Horizontal, tr("Directory"));
		 model->setHeaderData(2, Qt::Horizontal, tr("Game Title"));
		 model->setHeaderData(3, Qt::Horizontal, tr("SaveData Title"));

		 ui.savesTable->setModel(model);
		 model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());//clear data
	}
	else if(index==1) //psp device
	{
		if(scanPSPdir())
		 setupModel(m_savePSPList);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
	}
	else if(index==2) //Local folder
	{
		if(scanLocalDir())
		 setupModel(m_savePSPLocal);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
	}
	else if(index==3) //jpcsp selection
	{
		if(scanJPCSPdir())
		 setupModel(m_saveJPCSPList);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
	}
	else if(index==4) //pcsp selection
	{
		if(scanPCSPdir())
		 setupModel(m_savePCSPList);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
	}
	else if(index==5) //pcsp selection
	{
		if(scanPSPEdir())
		 setupModel(m_savePSPEList);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
	}
	ui.savedatalabel->setText("");//clear text
}
int hack2=0;
void savesWidget::showCollectionContextMenu(const QPoint & pnt)//test code
{
	/*if(hack2==1)
	{
		hack2=0;
		return;
	}
	else
	{
		hack2++;
	}*/
	QPoint globalPos = ui.savesTable->mapToGlobal(pnt);
	
/*	QModelIndex index=ui.collectionList->indexAt(pnt);//get model index

	int cacheid =index.sibling(index.row(), 8).data(Qt::DisplayRole).toUInt();
	QString path;
	QSqlQuery query2(dbcollect);
	query2.prepare("SELECT path FROM cache WHERE id =?");
	query2.addBindValue(cacheid);
	query2.exec();
	if(query2.first())
	{
       path=query2.value(0).toString();
	}*/
	QMenu contextmenu;
	//
    QItemSelectionModel *select= NULL;
	select = ui.savesTable->selectionModel();
	QModelIndexList list=select->selectedRows();
	if(list.size()==0)
		return;
	if(list.size()==1)
	{
		QMenu* m2=contextmenu.addMenu("Copy File to");
		if(ui.sourceselectCombo->currentIndex()==1)
		{
			m2->addAction("Local Folder");
			m2->addAction("JPCSP");
			m2->addAction("PCSP");
			m2->addAction("PSPE4ALL");
		}
		if(ui.sourceselectCombo->currentIndex()==2)
		{
			m2->addAction("PSP Device");
			m2->addAction("JPCSP");
			m2->addAction("PCSP");
			m2->addAction("PSPE4ALL");
		}
		if(ui.sourceselectCombo->currentIndex()==3)
		{
			m2->addAction("PSP Device");
			m2->addAction("Local Folder");
			m2->addAction("PCSP");
			m2->addAction("PSPE4ALL");
		}
		if(ui.sourceselectCombo->currentIndex()==4)
		{
			m2->addAction("PSP Device");
			m2->addAction("Local Folder");
			m2->addAction("JPCSP");
			m2->addAction("PSPE4ALL");
		}
		if(ui.sourceselectCombo->currentIndex()==5)
		{
			m2->addAction("PSP Device");
			m2->addAction("Local Folder");
			m2->addAction("JPCSP");
			m2->addAction("PCSP");
		}
		contextmenu.addAction("Delete File");
	}
	else
	{
		QMenu* m2=contextmenu.addMenu("Copy Files to");
		if(ui.sourceselectCombo->currentIndex()==1)
		{
			m2->addAction("Local Folder");
			m2->addAction("JPCSP");
			m2->addAction("PCSP");
			m2->addAction("PSPE4ALL");
		}
		if(ui.sourceselectCombo->currentIndex()==2)
		{
			m2->addAction("PSP Device");
			m2->addAction("JPCSP");
			m2->addAction("PCSP");
			m2->addAction("PSPE4ALL");
		}
		if(ui.sourceselectCombo->currentIndex()==3)
		{
			m2->addAction("PSP Device");
			m2->addAction("Local Folder");
			m2->addAction("PCSP");
			m2->addAction("PSPE4ALL");
		}
		if(ui.sourceselectCombo->currentIndex()==4)
		{
			m2->addAction("PSP Device");
			m2->addAction("Local Folder");
			m2->addAction("JPCSP");
			m2->addAction("PSPE4ALL");
		}
		if(ui.sourceselectCombo->currentIndex()==5)
		{
			m2->addAction("PSP Device");
			m2->addAction("Local Folder");
			m2->addAction("JPCSP");
			m2->addAction("PCSP");
		}
		contextmenu.addAction("Delete Files");
	}

	//contextmenu.addAction("Compress to cso");
	/*if(path.endsWith("iso",Qt::CaseInsensitive))
	{
		contextmenu.addAction("Compress to cso");
	}
	else if(path.endsWith("cso",Qt::CaseInsensitive))
	{
		contextmenu.addAction("Convert to iso");
	}*/
	QAction* selectedItem = contextmenu.exec(globalPos);
	if(selectedItem==NULL) return;
	if(selectedItem->text()=="Delete File")
	{
		if (askDelete(1))
        {
			QList<QString> paths;
			QString cacheid =list[0].sibling(list[0].row(), 5).data(Qt::DisplayRole).toString();
			paths.append(cacheid);
			deleteSaves(paths);
		}
	}
	if(selectedItem->text()=="Delete Files")
	{
		if (askDelete(2))
        {
			QList<QString> paths;
			for(int i=0; i<list.size(); i++)
			{
			  QString cacheid =list[i].sibling(list[i].row(), 5).data(Qt::DisplayRole).toString();
			  paths.append(cacheid);
			}
			deleteSaves(paths);
		}
	}
	if(selectedItem->text()=="PCSP")
	{
			QList<QString> paths;
			for(int i=0; i<list.size(); i++)
			{
			  QString cacheid =list[i].sibling(list[i].row(), 5).data(Qt::DisplayRole).toString();
			  paths.append(cacheid);
			}
			copySaves(paths,"PCSP");
	}
	if(selectedItem->text()=="JPCSP")
	{
			QList<QString> paths;
			for(int i=0; i<list.size(); i++)
			{
			  QString cacheid =list[i].sibling(list[i].row(), 5).data(Qt::DisplayRole).toString();
			  paths.append(cacheid);
			}
			copySaves(paths,"JPCSP");
	}
	if(selectedItem->text()=="PSPE4ALL")
	{
			QList<QString> paths;
			for(int i=0; i<list.size(); i++)
			{
			  QString cacheid =list[i].sibling(list[i].row(), 5).data(Qt::DisplayRole).toString();
			  paths.append(cacheid);
			}
			copySaves(paths,"PSPE4ALL");
	}
	if(selectedItem->text()=="Local Folder")
	{
			QList<QString> paths;
			for(int i=0; i<list.size(); i++)
			{
			  QString cacheid =list[i].sibling(list[i].row(), 5).data(Qt::DisplayRole).toString();
			  paths.append(cacheid);
			}
			copySaves(paths,"Local Folder");
	}
	if(selectedItem->text()=="PSP Device")
	{
			QList<QString> paths;
			for(int i=0; i<list.size(); i++)
			{
			  QString cacheid =list[i].sibling(list[i].row(), 5).data(Qt::DisplayRole).toString();
			  paths.append(cacheid);
			}
			copySaves(paths,"PSP Device");
	}

	

}
bool savesWidget::askDelete(int size)
{
	QString message;
	if(size==1)
	  message = tr("Are you sure you want to delete the select item?");
	else
	  message = tr("Are you sure you want to delete the selected items?");
    switch(QMessageBox::question(this, 
        tr("Delete"), 
        message,
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No))
    {
        case QMessageBox::Yes:
            return true;
            break;

        case QMessageBox::No:
            return false;
            break;

        default:
            return false;
    }
}
void savesWidget::deleteSaves(QList<QString> paths)
{
    for (int i = 0; i < paths.size(); i++)
    {
		QDir app(paths.at(i));
/*        if (!FileManager::deleteDir(app.absolutePath())) //TODO
        {
			QErrorMessage *message= new QErrorMessage(this);
			message->showMessage(tr("Error deleting %1").arg(paths.at(i)));
        }*/
    }
	if(ui.sourceselectCombo->currentIndex()==1)
		emit RefreshSaveTable(1);
	if(ui.sourceselectCombo->currentIndex()==2)
		emit RefreshSaveTable(2);
	if(ui.sourceselectCombo->currentIndex()==3)
		emit RefreshSaveTable(3);
	if(ui.sourceselectCombo->currentIndex()==4)
		emit RefreshSaveTable(4);
	if(ui.sourceselectCombo->currentIndex()==5)
		emit RefreshSaveTable(5);
}
bool savesWidget::askAppOverwrite(QString appName)
{
    switch(QMessageBox::question(this, 
        tr("Overwrite"), 
        tr("Save %1 already exists, do you wish to overwrite it?").arg(appName),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No))
    {
        case QMessageBox::Yes:
            return true;
            break;
            
        case QMessageBox::No:
            return false;
            break;
            
        default:
            return false;
    }
}
void savesWidget::copySaves(QList<QString> paths,QString emu)
{
	    for (int i = 0; i < paths.size(); i++)
        {
			QDir sourceDir(paths.at(i));

			QString destdirectory;
			if(emu=="PCSP")
			{
				destdirectory = theSettings->s_pcspPath + "/ms0/PSP/SAVEDATA" + "/" + sourceDir.dirName();
			}
			if(emu=="PSPE4ALL")
			{
				destdirectory = QDir::homePath() + "/pspe4all/vfs/ms0/PSP/SAVEDATA" + "/" + sourceDir.dirName();
			}
			if(emu=="JPCSP")
			{
				destdirectory = theSettings->s_jpcspPath + "/ms0/PSP/SAVEDATA" + "/" + sourceDir.dirName();
			}
			if(emu=="Local Folder")
			{
				destdirectory = theSettings->s_localdirpath + "/Saves" + "/" + sourceDir.dirName();
			}
			if(emu=="PSP Device")
			{
				destdirectory = theSettings->s_pspdevicePath + "/PSP/SAVEDATA" + "/" + sourceDir.dirName();
			}
            QDir destDir(destdirectory);
  /*          if (destDir.exists())  //TODO
            {
                if (askAppOverwrite(destdirectory))
                {
                    FileManager::deleteDir(destDir.absolutePath());
                    if (!FileManager::copyFolder(sourceDir, destDir, true))
                    {
						QErrorMessage *message= new QErrorMessage(this);
						message->showMessage(tr("Error moving %1").arg(paths.at(i)));
                    }
                }
            }
            else
            {
                if (!FileManager::copyFolder(sourceDir, destDir, true))
                {
                    	QErrorMessage *message= new QErrorMessage(this);
						message->showMessage(tr("Error moving %1").arg(paths.at(i)));
                }
            }*/
        }
}