#include <qsettings.h>
#include "applicationwidget.h"
#include <QFileInfo>
#include <QDir>
#include "Settings.h"
#include <QMessageBox>
#include <QProcess>
#include <QStandardItemModel>


applicationWidget::applicationWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QDir dir(theSettings->s_localdirpath);
	dir.mkpath(theSettings->s_localdirpath + "/Applications/GAME");
	//get combobox
	int source=ui.sourceselectCombo->currentIndex();
	emit RefreshApplicationTable(source);
}

applicationWidget::~applicationWidget()
{

}
 void applicationWidget::setupModel(QList<PSPApplication> &list)
 {
	 model = new QStandardItemModel(list.size(), 7, ui.applicationTable);
     model->setHeaderData(0, Qt::Horizontal, tr("Icon"));
	 model->setHeaderData(1, Qt::Horizontal, tr("Game Dir"));
     model->setHeaderData(2, Qt::Horizontal, tr("Application Dir"));
	 model->setHeaderData(3, Qt::Horizontal, tr("Title"));
	 model->setHeaderData(4, Qt::Horizontal, tr("Disc ID"));
	 model->setHeaderData(5, Qt::Horizontal, tr("FW"));

     ui.applicationTable->setModel(model);

     QItemSelectionModel *selectionModel = new QItemSelectionModel(model,this);
     ui.applicationTable->setSelectionModel(selectionModel);

	// ui.savesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	 ui.applicationTable->verticalHeader()->setDefaultSectionSize(40);
	ui.applicationTable->setColumnWidth(0,72);
	ui.applicationTable->setColumnWidth(1,80);
	ui.applicationTable->setColumnWidth(2,120);
	ui.applicationTable->setColumnWidth(3,180);
	ui.applicationTable->setColumnWidth(4,100);
	ui.applicationTable->setColumnWidth(5,50);
	ui.applicationTable->hideColumn(6);

	 model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());//clear data
	 for (int i = 0; i < list.size(); i++)
	 {
		 model->insertRows(i, 1, QModelIndex());
		 //display icon0 in half size to save some space
		 int icon0width = list[i].getIcon().width();
		 int icon0height = list[i].getIcon().height();
		 model->setData(model->index(i, 0, QModelIndex()),QPixmap(list[i].getIcon().scaled(QSize(icon0width/2,icon0height/2))),Qt::DecorationRole);
		 model->setData(model->index(i, 1, QModelIndex()),list[i].getGamedirectory());
		 model->setData(model->index(i, 2, QModelIndex()),list[i].getApplicationdirectory());
		 model->setData(model->index(i, 3, QModelIndex()),list[i].getAppTitle());
		 model->setData(model->index(i, 4, QModelIndex()),list[i].getAppID());
		 model->setData(model->index(i, 5, QModelIndex()),list[i].getAppFW());
		 model->setData(model->index(i, 6, QModelIndex()),list[i].getPath());
		 
		// model->setData(model->index(i, 4),list[i].getSaveDataDetail());
	 }
	 connect(selectionModel, SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex)));
 }
void applicationWidget::onCurrentChanged(QModelIndex const &index)
{
	bootbinpath = index.sibling(index.row(), 6).data(Qt::DisplayRole).toString();
}

void applicationWidget::RefreshApplicationTable(int index)
{
	ui.JPCSPbutton->setEnabled(true);
	ui.CPSPEMUbutton->setEnabled(true);
	ui.PSPEbutton->setEnabled(true);

	//ui.PCSPbutton->setEnabled(true);

	if(index==0) //selection is none clear table
	{
		 model = new QStandardItemModel(0, 6, ui.applicationTable);
		 model->setHeaderData(0, Qt::Horizontal, tr("Icon"));
		 model->setHeaderData(1, Qt::Horizontal, tr("Game Dir"));
		 model->setHeaderData(2, Qt::Horizontal, tr("Application Dir"));
		 model->setHeaderData(3, Qt::Horizontal, tr("Title"));
		 model->setHeaderData(4, Qt::Horizontal, tr("Disc ID"));
		 model->setHeaderData(5, Qt::Horizontal, tr("FW"));

		 ui.applicationTable->setModel(model);
		 model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());//clear data
	}
	else if(index==1) //psp device
	{
		if(scanPSPdir())
		 setupModel(appPSPList);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
	}
	else if(index==2) //local folder
	{
		if(scanLocaldir())
		 setupModel(appLocalList);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
		
	}
	else if(index==3) //jpcsp
	{
		if(scanJPCSPdir())
		{
		 setupModel(appJPCSPList);
		 ui.JPCSPbutton->setEnabled(true);
		}
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
	}
	else if(index==4) //pcsp
	{
		if(scanPCSPdir())
		 setupModel(appPCSPList);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
	}
	else if(index==5) //pcsp
	{
		if(scanPSPEdir())
		 setupModel(appPSPEList);
		else
			emit ui.sourceselectCombo->setCurrentIndex(0);//set index to NONE
	}
}
bool applicationWidget::scanLocaldir()
{
    appLocalList.clear();
	if(!theSettings->s_localdirpath.isEmpty())
    {
		QDir dir(theSettings->s_localdirpath + "/Applications/GAME");
		if(dir.exists())
		{
			scanDir(dir.path(),appLocalList);
			return true;
		}
		else //jpcsp ms0 folder not found
		{
			QString savedatapath=theSettings->s_localdirpath + "/Applications/GAME";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("ms0 Folder not found"),
                                     "Local GAME folder not found.Are you sure that " + savedatapath + " exists?");
			
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
bool applicationWidget::scanJPCSPdir()
{
    appJPCSPList.clear();
	if(!theSettings->s_jpcspPath.isEmpty())
    {
		QDir dir(theSettings->s_jpcspPath + "/ms0/PSP/GAME");
		if(dir.exists())
		{
			scanDir(dir.path(),appJPCSPList);
			return true;
		}
		else //jpcsp ms0 folder not found
		{
			QString savedatapath=theSettings->s_jpcspPath + "/ms0/PSP/GAME";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("ms0 Folder not found"),
                                     "JPCSP GAME folder not found.Are you sure that " + savedatapath + " exists?");
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
bool applicationWidget::scanPCSPdir()
{
    appPCSPList.clear();
	if(!theSettings->s_pcspPath.isEmpty())
    {
		QDir dir(theSettings->s_pcspPath + "/ms0/PSP/GAME");
		if(dir.exists())
		{
			scanDir(dir.path(),appPCSPList);
			return true;
		}
		else //jpcsp ms0 folder not found
		{
			QString savedatapath=theSettings->s_pcspPath + "/ms0/PSP/GAME";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("ms0 Folder not found"),
                                     "PCSP GAME folder not found.Are you sure that " + savedatapath + " exists?");
			return false;

		}

    }
	else  //pcsp dir not set
	{
		QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("PCSP emulator path not found"),
                                     "PCSP emulator not found. Set the path of jpcsp emulator in configuration menu");
		return false;
	}
}
bool applicationWidget::scanPSPEdir()
{
    appPSPEList.clear();
	if(!theSettings->s_pspePath.isEmpty())
    {
		QDir dir(QDir::homePath() + "/pspe4all/vfs/ms0/PSP/GAME");
		if(dir.exists())
		{
			scanDir(dir.path(),appPSPEList);
			return true;
		}
		else //jpcsp ms0 folder not found
		{
			QString savedatapath=QDir::homePath() + "/pspe4all/vfs/ms0/PSP/GAME";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("ms0 Folder not found"),
                                     "PSPE4ALL GAME folder not found.Are you sure that " + savedatapath + " exists?");
			return false;

		}

    }
	else  //pcsp dir not set
	{
		QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("PSPE4ALL emulator path not found"),
                                     "PSPE4ALL emulator not found. Set the path of jpcsp emulator in configuration menu");
		return false;
	}
}
bool applicationWidget::scanPSPdir()
{
	appPSPList.clear();
	if(!theSettings->s_pspdevicePath.isEmpty())
    {
		QString PSPAppDir = getAppDirectory(theSettings->s_pspdevicePath);
		if(PSPAppDir.length()>0)
		{
			scanDir(PSPAppDir, appPSPList);
			return true;
		}
		else //PSP ms0 folder not found
		{
			//QString savedatapath=theSettings->s_pcspPath + "/ms0/PSP/SAVEDATA";
			QMessageBox::StandardButton reply=QMessageBox::critical(this, tr("GAME Folder not found"),
                                     "PSP's GAME folder not found.Are you sure that " + PSPAppDir + " exists?");
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
QString applicationWidget::getAppDirectory(QString PSPDirectory)
{
    QFileInfo fi(PSPDirectory);
    if (!fi.isDir())
    {
        return "";
    }
    
    QString appDir = "game";
    
    
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
                if (pspDirList.at(j).fileName().toLower() == appDir)
                {
                    //Bingo!
                    return pspDirList.at(j).filePath();
                }
            }
        }
    }
    
    return "";
}
//Scans a directory for PSP Applications and adds them on the list
void applicationWidget::scanDir(QString dir, QList<PSPApplication> &list)
{
    QFileInfo fi(dir);
    if (!fi.isDir())
    {
        return;
    }
    
    QDir directory(dir);
    
    QFileInfoList dirList = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirList.size(); ++i)
    {
        QDir appDir(dirList.at(i).filePath());
        QFileInfoList appDirList = appDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
        for (int j = 0; j < appDirList.size(); j++)
        {
            if (appDirList.at(j).fileName().toLower() == "eboot.pbp")
            {
            	//	try
            	//	{
                		PSPApplication app(appDirList.at(j).filePath());
                		//We only add the app if it's a new one
                		//This is the easiest way to get rid of the
                		//multiple pbp problem
               		    if (!findApp(list, app.getPath(),appDirList.at(j).filePath()))
                		{
                    		list.append(app);
                		}
                 // }
               // catch (QPSPManagerPBPException e)
        		//		{
            	//			m_view.showErrorMessage(tr("%1 doesn't seem to be a valid application").arg(appDirList.at(j).filePath()));
        		//		}
            }
        }
    }
}
bool applicationWidget::findApp(QList<PSPApplication> &list, QString path,QString originalpath)
{
    for (int i = 0; i < list.size(); i++)
    {
        if (list[i].getPath() == path)
        {
			if(originalpath.toLower().endsWith("%/eboot.pbp"))//hack if kxploint ends with % it gets scanned second so keep the second entry
		    {	
				list.removeAt(i);
				return false;
		    }
            return true;
        }
    }
    
    return false;
}

void applicationWidget::RunJPCSP()
{
	//QModelIndex index = selectionModel->currentIndex();
	//QString bootbinpath = index.sibling(index.row(), 6).data(Qt::DisplayRole).toString();
	if(bootbinpath.length()<1)
	  return;
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_jpcspPath.isNull() || theSettings->s_jpcspPath.length()==0)
	{
       QMessageBox::critical(this,tr("pspudb"),tr("JPCSP's path hasn't been set. Go to Configuration and set the path"));
	   return;
	}
	QDir::setCurrent(theSettings->s_jpcspPath);//change it
	QProcess launcher(this);
    QStringList arguments;

	arguments << "-Xmx1024m";
	arguments << "-XX:MaxPermSize=128m";
	arguments << "-XX:ReservedCodeCacheSize=64m";
	arguments << "-Djava.library.path=lib/windows-x86";
	arguments << "-jar"<< "bin/jpcsp.jar";
	arguments << "-f" << bootbinpath;
	if(theSettings->m_ini.value("/jpcsp/autorun",true).toBool())
       arguments << "-r";
	//launcher.startDetached("C:\\Program Files (x86)\\Java\\jre6\\bin\\java.exe", arguments);
	if(theSettings->m_ini.value("/jpcsp/show_console",true).toBool())
	     launcher.startDetached("C:\\Windows\\System32\\javaw.exe", arguments);   
	else
	   launcher.startDetached("C:\\Windows\\System32\\javaw.exe", arguments);

	QDir::setCurrent( sOldPath );//restore old directory
    //arguments << "-u" << umdpath;
	//if (!launcher.startDetached("C:\Program Files (x86)\Java\jre6\bin\java -Xmx1024m -XX:MaxPermSize=128m -XX:ReservedCodeCacheSize=64m -Djava.library.path=jpcsp/lib/windows-x86 -jar jpcsp/bin/jpcsp.jar",arguments))
   // {
     //   QMessageBox::critical(0, tr("PCSP - XMB"), tr("Cannot find an emulator to play %1!").arg(index.sibling(index.row(), 0).data(Qt::DisplayRole).toString()));
    //}
}
void applicationWidget::RunCSPSPEMU()
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
void applicationWidget::RunPCSP()
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
	arguments << "-ms0" << bootbinpath;

	launcher.startDetached("pcsp.exe", arguments) ;

	QDir::setCurrent( sOldPath );//restore old directory
}
void applicationWidget::RunPSPE()
{
	if(bootbinpath.length()<1)
	  return;
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_pspePath.isNull() || theSettings->s_pspePath.length()==0)
	{
       QMessageBox::critical(this,tr("pspudb"),tr("PSPE4all's path hasn't been set. Go to Configuration and set the path"));
	   return;
	}
	QDir::setCurrent(theSettings->s_pspePath);//change it
	QProcess launcher(this);
    QStringList arguments;
	arguments << "-ms0" << bootbinpath;

	launcher.startDetached("pspe4all.exe", arguments) ;

	QDir::setCurrent( sOldPath );//restore old directory
}