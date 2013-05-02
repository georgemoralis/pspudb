
#include "pspudb.h"
#include "WorkerThread.h"
#include "sqlconnection.h"

#include "compress\CSO\compressdialog.h"
#include "compress\7SO\compress7sodialog.h"
#include "compress\ProgressDialog.h"
#include "settingsdialog.h"
#include "Settings.h"
#include "renametool\renamedialog.h"
#include "fixupgames\fixupgames.h"

#include "databasetools\databaseChecks.h"  //for database fixes


pspudb::pspudb(QWidget *parent, Qt::WFlags flags)
{
	ui.setupUi(this);	
	theSettings = new Settings;
	theSettings->load(); //load general settings

	createConnection();//load up database

	//check if umdpaths have been set
	if(theSettings->s_umdpaths.length()==0)
	{
		QMessageBox::critical(this,"pspudb","No paths for umds have been set.Settings window will open now");
	   	settingsdialog dialog(this);
		dialog.exec();
	}

	umdtab = new umdWidget();
	ui.tabWidget->addTab(umdtab,"Games");
	savestab = new savesWidget();
	ui.tabWidget->addTab(savestab,"Saves");
	apptab = new applicationWidget();
	ui.tabWidget->addTab(apptab,"Aplications");
	filestab= new fileBrowserWidget();
	ui.tabWidget->addTab(filestab,"Demos");

	if(theSettings->s_umdpaths.length()>0)//scan games at startup
	{
		umdtab->scanGames();
	}
	QPoint pos = theSettings->m_ini.value("settings/windowpos", QPoint(50, 0)).toPoint();
    QSize size = theSettings->m_ini.value("settings/windowsize", QSize(858, 605)).toSize();
    resize(size);
    move(pos);

	//loadudbdata();
	//fixupbootbinstatus("udb.ini");
	//checkduplicateCRC();
	//fixupFW();

}

pspudb::~pspudb()
{

}
void pspudb::closeEvent(QCloseEvent *event)
{
  theSettings->m_ini.setValue("settings/windowpos", pos());
  theSettings->m_ini.setValue("settings/windowsize", size());

}

void pspudb::on_actionConvert_ISO_CSO_triggered()
{
	compressDialog dialog(this);
	QStringList filePaths;
	int compressionLevel=0;//Zero means NO-COMPRESSION
	if(dialog.exec())
	{
		filePaths = dialog.getFileNames();
		compressionLevel= dialog.getCompression();
	}
	//if no files selected return;
	if ( filePaths.isEmpty() ) 
	{
		return;
	}
	//open the progress dialog
	ProgressDialog* dlg = new ProgressDialog( this ); 

	//intialaze the worker thread struct
	WorkerThread::Work work;
	//give worker thread it's work
	work.task = WorkerThread::ConvertToCSO;
	//the given files
	work.files = filePaths;
	//the target path (get it from filesfolder) (TODO fix me if we support files from different folders
	work.target = QFileInfo( filePaths.first() ).absolutePath();
	//the window in which the thread will run
	work.window = dlg;
	//the compression level;
	work.compressLevel=compressionLevel;

	dlg->setWork( work );
}
void pspudb::on_actionConvert_CSO_ISO_triggered()
{
	QStringList filePaths = QFileDialog::getOpenFileNames( this, tr( "Choose CSO files to convert" ), QString::null, tr( "CSO Files (*.cso)" ) );

	//if no files selected return;
	if ( filePaths.isEmpty() ) 
	{
		return;
	}
	//open the progress dialog
	ProgressDialog* dlg = new ProgressDialog( this ); 

	//intialaze the worker thread struct
	WorkerThread::Work work;
	//give worker thread it's work
	work.task = WorkerThread::ConvertCsoToIso;
	//the given files
	work.files = filePaths;
	//the target path (get it from filesfolder) (TODO fix me if we support files from different folders
	work.target = QFileInfo( filePaths.first() ).absolutePath();
	//the window in which the thread will run
	work.window = dlg;

	dlg->setWork( work );
}
void pspudb::on_actionConvert_ISO_7SO_triggered()
{
	compress7soDialog dialog(this);
	QStringList filePaths;
	int compressionLevel=0;//Zero means NO-COMPRESSION
	if(dialog.exec())
	{
		filePaths = dialog.getFileNames();
		compressionLevel= dialog.getCompression();
	}
	//if no files selected return;
	if ( filePaths.isEmpty() ) 
	{
		return;
	}
	//open the progress dialog
	ProgressDialog* dlg = new ProgressDialog( this ); 

	//intialaze the worker thread struct
	WorkerThread::Work work;
	//give worker thread it's work
	work.task = WorkerThread::ConvertIsoToCs7;
	//the given files
	work.files = filePaths;
	//the target path (get it from filesfolder) (TODO fix me if we support files from different folders
	work.target = QFileInfo( filePaths.first() ).absolutePath();
	//the window in which the thread will run
	work.window = dlg;
	//the compression level;
	work.compressLevel=compressionLevel;

	dlg->setWork( work );
}
void pspudb::on_actionConvert_7SO_ISO_triggered()
{
	QStringList filePaths = QFileDialog::getOpenFileNames( this, tr( "Choose 7SO files to convert" ), QString::null, tr( "7SO Files (*.7so)" ) );

	//if no files selected return;
	if ( filePaths.isEmpty() ) 
	{
		return;
	}
	//open the progress dialog
	ProgressDialog* dlg = new ProgressDialog( this ); 

	//intialaze the worker thread struct
	WorkerThread::Work work;
	//give worker thread it's work
	work.task = WorkerThread::ConvertCs7ToIso;
	//the given files
	work.files = filePaths;
	//the target path (get it from filesfolder) (TODO fix me if we support files from different folders
	work.target = QFileInfo( filePaths.first() ).absolutePath();
	//the window in which the thread will run
	work.window = dlg;

	dlg->setWork( work );
}
void pspudb::on_actionGeneralSettings_triggered()
{
	settingsdialog dialog(this);
	if(dialog.exec())
	{
		bool needsupdate=dialog.requiresUpdate();//check if we have add/remove umd paths so we need to scan the games again
		if(needsupdate)
		{
			umdtab->scanGames();
		}
	}
}
void pspudb::on_actionRename_Tool_triggered()
{
	renamedialog dialog(this);
    umdtab->resetselection();//clear selection model so when we return from dialog , we have updated paths
	dialog.exec();
}
void pspudb::on_actionFixup_UMDs_entries_triggered()
{
	fixupgames dialog(this);
	 umdtab->resetselection();//clear selection model so when we return from dialog 
	if(dialog.exec())
	{
		QStringList paths = dialog.getpaths();
		if(paths.length()>0)
		{
			//open the progress dialog
			ProgressDialog* dlg = new ProgressDialog( this ); 

			//intialaze the worker thread struct
			WorkerThread::Work work;
			//give worker thread it's work
			if(dialog.fTask == dialog.FixupStatus)
				work.task = WorkerThread::FixupStatus;
			if(dialog.fTask == dialog.FixupImages)
				work.task = WorkerThread::FixupImages;
			if(dialog.fTask == dialog.FixupFW)
				work.task = WorkerThread::FixupFW;
			//the given files
			work.files = paths;

			//the window in which the thread will run
			work.window = dlg;

			dlg->setWork( work );
		}
	}
}
