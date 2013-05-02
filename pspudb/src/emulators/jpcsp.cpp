#include <QWidget>
#include <qdir.h>
#include <qmessagebox.h>
#include <qprocess.h>
#include <qsettings.h>
#include "jpcsp.h"

#include "Settings.h"

void Jpcsp::runJPCSP(Jpcsp::Mode mode,QString path,QWidget *parent)
{
	if(path.length()<1)
	  return;
	QString sOldPath = QDir::currentPath();//store old directory before change it to jpcsp one
	if(theSettings->s_jpcspPath.isNull() || theSettings->s_jpcspPath.length()==0)
	{
       //QMessageBox::critical(parent,tr("pspudb"),tr("JPCSP's path hasn't been set. Go to Configuration and set the path"));
       QMessageBox::critical(parent,"pspudb","JPCSP's path hasn't been set. Go to Configuration and set the path");
	   return;
	}
	QDir::setCurrent(theSettings->s_jpcspPath);//change it
	QProcess launcher(parent);
    QStringList arguments;

	arguments << "-Xmx1024m";
	arguments << "-XX:MaxPermSize=128m";
	arguments << "-XX:ReservedCodeCacheSize=64m";
	arguments << "-Djava.library.path=lib/windows-x86";
	arguments << "-jar"<< "bin/jpcsp.jar";
	if(mode == Jpcsp::file)
		arguments << "-f" << path;
	if(mode == Jpcsp::UMD)
		arguments << "-u" << path;
	if(theSettings->m_ini.value("/jpcsp/autorun",true).toBool())
       arguments << "-r";
	//launcher.startDetached("C:\\Program Files (x86)\\Java\\jre6\\bin\\java.exe", arguments);
	if(theSettings->m_ini.value("/jpcsp/show_console",true).toBool())
	     launcher.startDetached("C:\\Windows\\System32\\javaw.exe", arguments);   
	else
	   launcher.startDetached("C:\\Windows\\System32\\javaw.exe", arguments);

	QDir::setCurrent( sOldPath );//restore old directory
}

