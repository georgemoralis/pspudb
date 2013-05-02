#include "umdwidget.h"
#include <qsettings.h>
#include <QMessageBox>
#include "Settings.h"
#include <qdir.h>
#include <QStandardItemModel>
#include <QProcess>
#include "compress\ProgressDialog.h"
#include "WorkerThread.h"
#include "umdTableModel.h"
#include "imageviewer\qimageviewer.h"
#include "emulators/jpcsp.h"
#include "emulators/ppsspp.h"

extern QSqlDatabase dbcollect;
extern QSqlDatabase dbadv;

umdWidget::umdWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);


}

umdWidget::~umdWidget()
{

}
void umdWidget::scanGames()
{
		//delete all games that has avaliable=0 from last time
	QSqlQuery query(dbcollect);
    query.exec("DELETE FROM cache WHERE available = 0");
	//then make all the other existing entry avaliable=0 so we can scan
	query.exec("UPDATE cache SET available = 0");

	//disable emulatur buttons
	ui.JPCSPbutton->setEnabled(false);
	ui.PSPEMUbutton->setEnabled(false);
	ui.PCSPbutton->setEnabled(false);
	ui.CPSPEMUbutton->setEnabled(false);
	ui.PSPEbutton->setEnabled(false);

	if(theSettings->s_umdpaths.length()>0) //still make sure
	{

		QStringList paths = theSettings->s_umdpaths.split(",", QString::SkipEmptyParts);
		QStringList files;
		if(paths.size()>0)
		{
			for(int i=0; i<paths.size(); i++)
			{
				QDir dir(paths[i]);
				QFileInfoList entries(dir.entryInfoList(QStringList() << "*.ISO" << "*.CSO", QDir::Files, QDir::Name|QDir::IgnoreCase));
					QListIterator< QFileInfo > entry(entries);
					if (entry.hasNext())
					{
					   while (entry.hasNext())
					   {
						   QFileInfo fi = entry.next();
						   files << fi.absoluteFilePath();
					   }
					}
			}
			//intialaze the worker thread struct
			WorkerThread::Work work;
			//open the progress dialog
			ProgressDialog* dlg = new ProgressDialog( this ); 
				//give worker thread it's work
			work.task = WorkerThread::ScanUMD;
			//the given files
			work.files = files;

			//the window in which the thread will run
			work.window = dlg;

			dlg->setWork( work );
			connect(dlg, SIGNAL(requestRefresh()), this, SLOT(RefreshUmd()));
		}
	}	
}
void umdWidget::RefreshUmd()
{
	umdTableModel *model = new umdTableModel(ui.umdList);
	umdTableModel *psnmodel = new umdTableModel(ui.psnList);
	QSqlQuery query(dbadv);
	query.exec("ATTACH DATABASE 'userdata.db' AS collection");
    //model->setQuery( "SELECT isospsp.id , advpsp.title , isospsp.discid,advpsp.region,isospsp.firmware ,isospsp.isotype,isospsp.gameid,cache.id FROM collection.cache , isospsp,advpsp WHERE cache.crc32 = isospsp.crc AND isospsp.gameid = advpsp.id AND cache.available =1",dbadv );
	//combine 2 columns in one : http://stackoverflow.com/questions/1003358/sqlite-problem-selecting-two-columns-as-one
	//model->setQuery( "SELECT advpsp.id || \".\" || isospsp.isonumber  AS gameID , advpsp.title , isospsp.discid,advpsp.region,isospsp.firmware ,isospsp.isotype,isospsp.gameid,cache.id FROM collection.cache , isospsp,advpsp WHERE cache.crc32 = isospsp.crc AND isospsp.gameid = advpsp.id AND cache.available =1",dbadv );
	//also adding padding : http://stackoverflow.com/questions/4899832/sqlite-function-to-format-numbers-with-leading-zeroes
	
	//model->setQuery( "SELECT substr('0000'||advpsp.id, length('0000'||advpsp.id)-3, 4) || \".\" || substr('000'||isospsp.isonumber, length('000'||isospsp.isonumber)-2, 3)  AS gameID , advpsp.title , isospsp.discid,advpsp.region,isospsp.firmware ,isospsp.isotype,isospsp.gameid,cache.id,isospsp.id FROM collection.cache , isospsp,advpsp WHERE cache.crc32 = isospsp.crc AND isospsp.gameid = advpsp.id AND cache.available =1",dbadv );
	  model->setQuery( "SELECT substr('0000'||games.releaseNumber, length('0000'||games.releaseNumber)-3, 4) || \".\" || substr('000'||images.isonumber, length('000'||images.isonumber)-2, 3)  AS gameID , games.title , images.discid,games.region,images.firmware ,images.isotype,images.gameid,cache.id,images.id FROM collection.cache , images,games WHERE cache.crc32 = images.crc AND images.gameid = games.id AND cache.available =1 AND games.type=\"UMD\"",dbadv );
	  psnmodel->setQuery( "SELECT substr('0000'||games.releaseNumber, length('0000'||games.releaseNumber)-3, 4) || \".\" || substr('000'||images.isonumber, length('000'||images.isonumber)-2, 3)  AS gameID , games.title , images.discid,games.region,images.firmware ,images.isotype,images.gameid,cache.id,images.id FROM collection.cache , images,games WHERE cache.crc32 = images.crc AND images.gameid = games.id AND cache.available =1 AND games.type=\"PSN\"",dbadv );
	
	
	while (model->canFetchMore())
		model->fetchMore(); //fetch all the entries at once 

	

	m_model = new QSortFilterProxyModel(this);
	m_model->setSourceModel(model);
	ui.umdList->setModel(m_model);
	m_model->setFilterKeyColumn(-1);//search to all columns by default
	emit m_model->sort(0,Qt::AscendingOrder);//emit a ascending order for column 0 by default

	ui.tabWidget->setTabText(0,"UMD ( " +  QString::number(m_model->rowCount()) + " )");

	ui.umdList->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui.umdList->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui.umdList->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui.umdList->header()->setResizeMode(3, QHeaderView::ResizeToContents);
	ui.umdList->header()->setResizeMode(4, QHeaderView::ResizeToContents);
	ui.umdList->header()->setResizeMode(5, QHeaderView::ResizeToContents);

	ui.umdList->hideColumn(6); //hide gameid column
	ui.umdList->hideColumn(7); //hide cacheid column
	ui.umdList->hideColumn(8); //hide isospsp imageID column
	
	m_selectionModel = new QItemSelectionModel(m_model, this);
    ui.umdList->setSelectionModel(m_selectionModel);
	ui.umdList->setContextMenuPolicy(Qt::CustomContextMenu);
	// connect custom context menu
	//connect(ui.umdList, SIGNAL(customContextMenuRequested( const QPoint& )), this, SLOT(showCollectionContextMenu(const QPoint &)));
	connect(m_selectionModel, SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex)));

	ui.umdList->show();

	//psn table
	while (psnmodel->canFetchMore())
		psnmodel->fetchMore(); //fetch all the entries at once 

	mp_model = new QSortFilterProxyModel(this);
	mp_model->setSourceModel(psnmodel);
	ui.psnList->setModel(mp_model);
	mp_model->setFilterKeyColumn(-1);//search to all columns by default
	emit mp_model->sort(0,Qt::AscendingOrder);//emit a ascending order for column 0 by default

	ui.tabWidget->setTabText(1,"PSN ( " +  QString::number(mp_model->rowCount()) + " )");

	ui.psnList->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui.psnList->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui.psnList->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui.psnList->header()->setResizeMode(3, QHeaderView::ResizeToContents);
	ui.psnList->header()->setResizeMode(4, QHeaderView::ResizeToContents);
	ui.psnList->header()->setResizeMode(5, QHeaderView::ResizeToContents);

	ui.psnList->hideColumn(6); //hide gameid column
	ui.psnList->hideColumn(7); //hide cacheid column
	ui.psnList->hideColumn(8); //hide isospsp imageID column
	
	mp_selectionModel = new QItemSelectionModel(mp_model, this);
    ui.psnList->setSelectionModel(mp_selectionModel);
	ui.psnList->setContextMenuPolicy(Qt::CustomContextMenu);
	// connect custom context menu
	//connect(ui.umdList, SIGNAL(customContextMenuRequested( const QPoint& )), this, SLOT(showCollectionContextMenu(const QPoint &)));
	
	connect(mp_selectionModel, SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex)));

	ui.psnList->show();
}
void umdWidget::onCurrentChanged(QModelIndex const &index)
{
	int id =index.sibling(index.row(), 6).data(Qt::DisplayRole).toUInt();
	int cacheid =index.sibling(index.row(), 7).data(Qt::DisplayRole).toUInt();
	QString gameID = index.sibling(index.row(), 0).data(Qt::DisplayRole).toString();
	int imID = index.sibling(index.row(), 8).data(Qt::DisplayRole).toUInt();
	QSqlQuery query(dbadv);
	query.prepare("SELECT games.id,title,type,discid,region,firmware,publisher,genre,language,releaseNumber,multiplayer,description FROM games,images WHERE games.id = images.gameid AND games.id =?");
	query.addBindValue(id);
	query.exec();
	if(query.first())
	{
		QString type = query.value(2).toString();
		QString idc = query.value(9).toString();
	    ui.coverpix->setPixmap(QPixmap("data/img/"+type+"/"+idc+"a.png"));
		ui.screenshotpix->setPixmap(QPixmap("data/img/"+type+"/"+idc+"b.png"));

		ui.collectiontitlelabel->setText(gameID + " - " + query.value(1).toString());
		ui.collectiontypelabel->setText(query.value(2).toString() + " ( releaseNumber: " + query.value(9).toString() + ")");
		ui.collectiondiscidlabel->setText(query.value(3).toString());
		ui.collectionregionlabel->setText(query.value(4).toString());
		ui.collectionfwlabel->setText(query.value(5).toString());
		ui.publishercollectionlabel->setText(query.value(6).toString());
		ui.genrelcollectionlabel->setText(query.value(7).toString());
		ui.languagecollectionlabel->setText(query.value(8).toString());
		QString multi = query.value(10).toString();
		if(!multi.contains("<unkn>") && multi.length()>0 && multi.contains("|"))
		{
			QStringList t= multi.split("| ");
		    ui.multiplayerlabel->setText(t[0] + "\n" + t[1]);
		}
		else
		{
			ui.multiplayerlabel->setText(multi);
		}
		ui.descriptionTextEdit->setHtml(query.value(11).toString());
	}
	//get staff from pspisos table
	QSqlQuery query3(dbadv);
	query3.prepare("SELECT crc,isotype,grouprelease,filesize,bootbinstatus,ebootbinstatus FROM images WHERE id =?");
	query3.addBindValue(imID);
	query3.exec();
	if(query3.first())
	{
		ui.crclabel->setText(query3.value(0).toString());
		ui.collectionisotypelabel->setText(query3.value(1).toString());
		//filesize
		/*int size = query3.value(3).toUInt();
	    float gametoMB = (float)size /1048576;
		QString floatsize;
		floatsize.setNum( gametoMB);
		ui.filesizelabel->setText(query3.value(3).toString()+" (" + floatsize + " MB)");*/
		ui.bootbinstatuslabel->setText(query3.value(4).toString());
		ui.ebootbinstatuslabel->setText(query3.value(5).toString());

	}
	//get path from cache table
	QSqlQuery query2(dbcollect);
	query2.prepare("SELECT path,filesize FROM cache WHERE id =?");
	query2.addBindValue(cacheid);
	query2.exec();
	if(query2.first())
	{
       ui.pathlabel->setText(query2.value(0).toString());
	   int size = query2.value(1).toUInt();
	    float gametoMB = (float)size /1048576;
		QString floatsize;
		floatsize.setNum( gametoMB);
		ui.filesizelabel->setText(query2.value(1).toString()+" (" + floatsize + " MB)");
	}
	//enable emulatur buttons
	ui.JPCSPbutton->setEnabled(true);
	ui.PSPEMUbutton->setEnabled(true);
	ui.PCSPbutton->setEnabled(true);
	ui.CPSPEMUbutton->setEnabled(true);
	ui.PSPEbutton->setEnabled(true);
	ui.PPSSPPbutton->setEnabled(true);

	//image gallery
	m_screenshotsModel = new QStandardItemModel(this);
	ui.listView->setModel(m_screenshotsModel);
	m_screenshotsModel->removeRows(0,m_screenshotsModel->rowCount(QModelIndex()), QModelIndex());//clear data
	ui.listView->show();
	m_imageNamesList.clear();
	disconnect(ui.listView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(imageClicked(QModelIndex)));

	//check if there is a duplicate folder to load
	QSqlQuery query5(dbadv);
	query5.prepare("SELECT games.id,discid,duplicateID FROM games,images WHERE games.id = images.gameid AND games.id =? AND duplicateID!=0");
	query5.addBindValue(id);
	query5.exec();
	QString screenDir;
	if(query5.first())//there is a duplicate ID
	{
		screenDir="duplicate/"+query5.value(2).toString();
	}
	else
	{
		screenDir="standalone/"+ query.value(3).toString();
	}

    QDir screenshotdirectory = QDir("data/screenshots/"+ screenDir);
	if(screenshotdirectory.exists())
	{
		QStringList listOfScreenies;//= screenshotdirectory.entryList(QDir::Files | QDir::NoSymLinks);
		screenshotdirectory.setFilter(QDir::Files | QDir::NoSymLinks);
		QFileInfoList list = screenshotdirectory.entryInfoList();
		if(!list.isEmpty())
		{
			for(int i=0; i<list.size(); i++)
			{
				listOfScreenies.append(list.at(i).absoluteFilePath());
			    QStandardItem* imageitem = new QStandardItem();
				imageitem->setIcon(QIcon(list.at(i).absoluteFilePath()));
				m_screenshotsModel->appendRow(imageitem);
			}
			m_imageNamesList=listOfScreenies;
		}
		if(listOfScreenies.size()>0)
		{			
				connect(ui.listView, SIGNAL(doubleClicked(QModelIndex)), SLOT(imageClicked(QModelIndex)));
		}
	}
		
}
void umdWidget::imageClicked(QModelIndex index)
{
	qImageViewer dialog(m_imageNamesList,index.row(),this);
	dialog.exec();

}
void umdWidget::resetselection()
{
	m_selectionModel->clear();
}
void umdWidget::RunJPCSP()
{
	QString umdpath = ui.pathlabel->text();
	Jpcsp::runJPCSP(Jpcsp::UMD,umdpath,this);
}
void umdWidget::RunPPSSPP()
{
	QString umdpath = ui.pathlabel->text();
	ppsspp::runPPSSPP(ppsspp::UMD,umdpath,this);
}
void umdWidget::RunPCSP()
{
	QString umdpath = ui.pathlabel->text();
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_pcspPath.isNull() || theSettings->s_pcspPath.length()==0)
	{
       QMessageBox::critical(this,tr("pspudb"),tr("PCSP's path hasn't been set. Go to Configuration and set the path"));
	   return;
	}
	QDir::setCurrent(theSettings->s_pcspPath);//change it
	QProcess launcher(this);
    QStringList arguments;
	arguments << "-umd" << umdpath;
	/*if(pcsp_decryptor)*/
		arguments << "-decrypt" << "true";
	/*else
		arguments << "-decrypt" << "false";*/

	launcher.startDetached("pcsp.exe", arguments) ;

	QDir::setCurrent( sOldPath );//restore old directory
}
void umdWidget::RunPSPE()
{
	QString umdpath = ui.pathlabel->text();
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_pspePath.isNull() || theSettings->s_pspePath.length()==0)
	{
       QMessageBox::critical(this,tr("pspudb"),tr("PSPE4ALL's path hasn't been set. Go to Configuration and set the path"));
	   return;
	}
	QDir::setCurrent(theSettings->s_pspePath);//change it
	QProcess launcher(this);
    QStringList arguments;
	arguments << "-umd" << umdpath;

	launcher.startDetached("pspe4all.exe", arguments) ;

	QDir::setCurrent( sOldPath );//restore old directory
}
void umdWidget::RunPSPEMUD()
{
	QString umdpath = ui.pathlabel->text();
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_pspemuPath.isNull() || theSettings->s_pspemuPath.length()==0)
	{
       QMessageBox::critical(this,tr("pspudb"),tr("PSPEMU's path hasn't been set. Go to Configuration and set the path"));
	   return;
	}
	QDir::setCurrent(theSettings->s_pspemuPath);//change it
	QProcess launcher(this);
    QStringList arguments;
	arguments << umdpath;

	launcher.startDetached("pspemu.exe", arguments); 

	QDir::setCurrent( sOldPath );//restore old directory
}
void umdWidget::RunCSPSPEMU()
{
	QString umdpath = ui.pathlabel->text();
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_cspspemuPath.isNull() || theSettings->s_cspspemuPath.length()==0)
	{
		QMessageBox::critical(this,tr("pspudb"),tr("PSPEMU's (C#) path hasn't been set. Go to Configuration and set the path"));
	   return;
	}
	QDir::setCurrent(theSettings->s_cspspemuPath);//change it
	QProcess launcher(this);
    QStringList arguments;
	arguments << umdpath ;

	launcher.startDetached("cspspemu.exe", arguments); 

	QDir::setCurrent( sOldPath );//restore old directory
}