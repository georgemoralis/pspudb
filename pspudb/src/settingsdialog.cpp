#include "settingsdialog.h"
#include "Settings.h"

settingsdialog::settingsdialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(save()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	needsupdate=false; //we assume that we don't need to scan games
	ui.jpcsppathedit->setText(theSettings->s_jpcspPath);
	ui.pspemupathedit->setText(theSettings->s_pspemuPath);
	ui.pcsppathedit->setText(theSettings->s_pcspPath);
	ui.cpspemupathedit->setText(theSettings->s_cspspemuPath);
	ui.pspepathedit->setText(theSettings->s_pspePath);
	ui.ppsspppathedit->setText(theSettings->s_ppssppPath);
	ui.pspdeviceedit->setText(theSettings->s_pspdevicePath);
	ui.localpathedit->setText(theSettings->s_localdirpath);

	QString umdpath = theSettings->s_umdpaths;
	QStringList paths = umdpath.split(",", QString::SkipEmptyParts);
	if(paths.size()>0)
	{
		for(int i=0; i<paths.size(); i++)
		{
			ui.pathList->addItem(paths[i]);
		}
	}
	tempumdpaths = umdpath; // store the paths at opening so we have can check in save if the path is changed

}

settingsdialog::~settingsdialog()
{

}
void settingsdialog::browsejpcspbutton()
{
	QString dir = QFileDialog::getExistingDirectory(this, 
	    tr("Select jpcsp emu directory"),
        QDir::currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
	if (!dir.isNull() || dir.length()==0)
	{
		theSettings->s_jpcspPath=dir;
	}
	ui.jpcsppathedit->setText(dir);
}
void settingsdialog::browselocalpathbutton()
{
		QString dir = QFileDialog::getExistingDirectory(this, 
	    tr("Select local path directory"),
        QDir::currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
	if (!dir.isNull() || dir.length()==0)
	{
		theSettings->s_localdirpath=dir;
	}
	ui.localpathedit->setText(dir);
	QDir dir2(theSettings->s_localdirpath);
	dir2.mkpath(theSettings->s_localdirpath + "/Applications/GAME");

}
void settingsdialog::browsepcspbutton()
{
	QString dir = QFileDialog::getExistingDirectory(this, 
	    tr("Select pcsp emu directory"),
        QDir::currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
	if (!dir.isNull() || dir.length()==0)
	{
		theSettings->s_pcspPath=dir;
	}
	ui.pcsppathedit->setText(dir);
}
void settingsdialog::browsepspemubutton()
{
	QString dir = QFileDialog::getExistingDirectory(this, 
	    tr("Select pspemu(D) emu directory"),
        QDir::currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
	if (!dir.isNull() || dir.length()==0)
	{
		theSettings->s_pspemuPath=dir;
	}
	ui.pspemupathedit->setText(dir);
}
void settingsdialog::browsecpspemubutton()
{
	QString dir = QFileDialog::getExistingDirectory(this, 
		tr("Select pspemu(C#) emu directory"),
        QDir::currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
	if (!dir.isNull() || dir.length()==0)
	{
		theSettings->s_cspspemuPath=dir;
	}
	ui.cpspemupathedit->setText(dir);
}
void settingsdialog::browsepspebutton()
{
	QString dir = QFileDialog::getExistingDirectory(this, 
		tr("Select pspe4all directory"),
        QDir::currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
	if (!dir.isNull() || dir.length()==0)
	{
		theSettings->s_pspePath=dir;
	}
	ui.pspepathedit->setText(dir);
}
void settingsdialog::browsepspdevicebutton()
{
	QString dir = QFileDialog::getExistingDirectory(this, 
		tr("Select PSP device directory"),
        QDir::currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
	if (!dir.isNull() || dir.length()==0)
	{
		theSettings->s_pspdevicePath=dir;
	}
	ui.pspdeviceedit->setText(dir);
}
void settingsdialog::browseppssppbutton()
{
	QString dir = QFileDialog::getExistingDirectory(this, 
		tr("Select ppsspp directory"),
        QDir::currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
	if (!dir.isNull() || dir.length()==0)
	{
		theSettings->s_ppssppPath=dir;
	}
	ui.ppsspppathedit->setText(dir);
}
void settingsdialog::addUmdDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, 
		tr("Select UMD directory"),
        QDir::currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isNull() || dir.length()==0)
	{
		ui.pathList->addItem(dir);
	}

}
void settingsdialog::removeUmdDir()
{
	qDeleteAll(ui.pathList->selectedItems());
}
void settingsdialog::save()
{
	theSettings->s_jpcspPath= ui.jpcsppathedit->text();
	theSettings->m_ini.setValue("/emupath/jpcsppath", theSettings->s_jpcspPath);
	theSettings->s_pcspPath= ui.pcsppathedit->text();
	theSettings->m_ini.setValue("/emupath/pcsppath", theSettings->s_pcspPath);
	theSettings->s_pspemuPath= ui.pspemupathedit->text();
	theSettings->m_ini.setValue("/emupath/pspemupath", theSettings->s_pspemuPath);
	theSettings->s_cspspemuPath= ui.cpspemupathedit->text();
	theSettings->m_ini.setValue("/emupath/cspspemupath", theSettings->s_cspspemuPath);
	theSettings->s_pspePath= ui.pspepathedit->text();
	theSettings->m_ini.setValue("/emupath/pspepath", theSettings->s_pspePath);
	theSettings->s_ppssppPath= ui.ppsspppathedit->text();
	theSettings->m_ini.setValue("/emupath/ppsspppath", theSettings->s_ppssppPath);


	theSettings->s_pspdevicePath= ui.pspdeviceedit->text();
	theSettings->m_ini.setValue("/device/pspdevpath", theSettings->s_pspdevicePath);
	theSettings->s_localdirpath= ui.localpathedit->text();
	theSettings->m_ini.setValue("/local/localdir", theSettings->s_localdirpath);


	if(theSettings->s_localdirpath.length()>0)
	{
	 //make local dir
	 QDir dir(theSettings->s_localdirpath);
	 dir.mkpath(theSettings->s_localdirpath + "/Demos");
	 dir.mkpath(theSettings->s_localdirpath + "/Applications/GAME");
	 dir.mkpath(theSettings->s_localdirpath + "/Saves");
	}
	//get number of rows
	int numumdrows = ui.pathList->model()->rowCount();
	QList<QString> umdrows;
	if(numumdrows>0)
	{
		for(int i=0; i<numumdrows; i++)
		{
			QListWidgetItem* it=ui.pathList->item(i);
			QString data=it->data(0).toString();
			if(i==0)
			  theSettings->s_umdpaths=data;
			else
			  theSettings->s_umdpaths+= "," + data;
		}
	}
	if(numumdrows==0)//special case. all entries removed , null umdpaths
	{
		theSettings->s_umdpaths="";
	}
	theSettings->m_ini.setValue("/settings/umdpaths", theSettings->s_umdpaths);
	
	if(theSettings->s_umdpaths.length()>1)
	{
		if(tempumdpaths.compare(theSettings->s_umdpaths)!=0)//the paths doesn't match update
		{
			needsupdate=true;
		}
	}
	QDialog::accept();
}
bool settingsdialog::requiresUpdate()
{
	return needsupdate;
}
