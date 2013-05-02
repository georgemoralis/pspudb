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

#include <QSettings>
#include "renamedialog.h"
#include "Settings.h"
#include <QDir>
#include <QtSql>
#include <QMessageBox>
#include <QMenu>
#include <QTextBrowser>

extern QSqlDatabase dbadv;
extern QSqlDatabase dbcollect;

renamedialog::renamedialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	createMenus();
	reloadContents();
}

renamedialog::~renamedialog()
{

}
void renamedialog::reloadContents()
{
	ui.umdlist->clear();
	QString umdpath = theSettings->s_umdpaths;
	QStringList paths = umdpath.split(",", QString::SkipEmptyParts);
	if(paths.size()>0)
	{
		for(int i=0; i<paths.size(); i++)
		{
			subReloadContents(paths[i]);
		}
	}

}
void renamedialog::subReloadContents(QString path)
{
	QDir directory (path);
	QFileInfoList entries = directory.entryInfoList((QDir::NoDotAndDotDot | QDir::Files), (QDir::Name | QDir::IgnoreCase));
	
	for (int i = 0; i < entries.count(); ++i) 
	{
		QListWidgetItem * item = new QListWidgetItem(ui.umdlist);
		item->setText(entries.at(i).fileName());
		item->setCheckState(Qt::Checked);
		item->setData(Qt::UserRole, entries.at(i).absoluteFilePath());
		if(entries.at(i).suffix()=="iso" || entries.at(i).suffix()=="ISO")
			item->setIcon(QIcon(QString::fromUtf8("://pspudb/images/iso.png")));
		if(entries.at(i).suffix()=="cso" || entries.at(i).suffix()=="CSO")
			item->setIcon(QIcon(QString::fromUtf8(":/pspudb/images/cso.png")));
	}
	emit selectnoneAction();
}
void renamedialog::selectallAction()
{
	for (int i = 0; i < ui.umdlist->count(); ++i) 
	{
		ui.umdlist->item(i)->setCheckState(Qt::Checked);
	}
}
void renamedialog::selectnoneAction()
{
	for (int i = 0; i < ui.umdlist->count(); ++i) 
	{
		ui.umdlist->item(i)->setCheckState(Qt::Unchecked);
	}
}
void renamedialog::createMenus()
{
	QMenu * menu = new QMenu (ui.variableButton);
	ui.variableButton->setMenu(menu);

	QAction * gameid_act = new QAction(menu);
	gameid_act->setText(tr("(%id) Game ID"));
	connect(gameid_act, SIGNAL(triggered()), this, SLOT(addgameid()));
	menu->addAction(gameid_act);
	QAction * ison_act = new QAction(menu);
	ison_act->setText(tr("(%in) Iso number"));
	connect(ison_act, SIGNAL(triggered()), this, SLOT(addisonumber()));
	menu->addAction(ison_act);
	QAction * title_act = new QAction(menu);
	title_act->setText(tr("(%tl) Game Title"));
	connect(title_act, SIGNAL(triggered()), this, SLOT(addtitle()));
	menu->addAction(title_act);
	QAction * discid_act = new QAction(menu);
	discid_act->setText(tr("(%dc) Disc ID"));
	connect(discid_act, SIGNAL(triggered()), this, SLOT(adddiscid()));
	menu->addAction(discid_act);
	QAction * region_act = new QAction(menu);
	region_act->setText(tr("(%rg) Region"));
	connect(region_act, SIGNAL(triggered()), this, SLOT(addregion()));
	menu->addAction(region_act);
	QAction * regionsmall_act = new QAction(menu);
	regionsmall_act->setText(tr("(%rs) Region small (US,JP,EU)"));
	connect(regionsmall_act, SIGNAL(triggered()), this, SLOT(addregionsmall()));
	menu->addAction(regionsmall_act);
	QAction * fw_act = new QAction(menu);
	fw_act->setText(tr("(%fw) Firmware"));
	connect(fw_act, SIGNAL(triggered()), this, SLOT(addfw()));
	menu->addAction(fw_act);

	QAction * type_act = new QAction(menu);
	type_act->setText(tr("(%tp) Type (UMD or PSN)"));
	connect(type_act, SIGNAL(triggered()), this, SLOT(addtype()));
	menu->addAction(type_act);

	QAction * extension_act = new QAction(menu);
	extension_act->setText(tr("(%ex) Extension (.cso,.iso)"));
	connect(extension_act, SIGNAL(triggered()), this, SLOT(addextension()));
	menu->addAction(extension_act);


	//templates menu
	QMenu * menu2 = new QMenu (ui.templatesButton);
	ui.templatesButton->setMenu(menu2);
	QAction * t1_act = new QAction(menu);
	t1_act->setText(tr("<TYPE> - <Game ID>.<Isonumber> - <Game title> (<Region small>)<Extension>"));
	connect(t1_act, SIGNAL(triggered()), this, SLOT(addt1()));
	menu2->addAction(t1_act);
	QAction * t2_act = new QAction(menu);
	t2_act->setText(tr("<TYPE> - <Game ID>.<Isonumber> - <Game title> (<discID>)<Extension>"));
	connect(t2_act, SIGNAL(triggered()), this, SLOT(addt2()));
	menu2->addAction(t2_act);

	QAction * t3_act = new QAction(menu);
	t3_act->setText(tr("<Game ID>.<Isonumber> - <Game title> (<discID>) (<Type>)<Extension>"));
	connect(t3_act, SIGNAL(triggered()), this, SLOT(addt3()));
	menu2->addAction(t3_act);
}
void renamedialog::addt1()
{
	ui.nameEdit->clear();
	ui.nameEdit->setText("%tp - %id.%in - %tl (%rs)%ex");
}
void renamedialog::addt2()
{
	ui.nameEdit->clear();
	ui.nameEdit->setText("%tp - %id.%in - %tl (%dc)%ex");
}
void renamedialog::addt3()
{
	ui.nameEdit->clear();
	ui.nameEdit->setText("%id.%in - %tl (%dc) (%tp)%ex");
}
void renamedialog::addgameid()
{
	ui.nameEdit->setText(QString("%1%2").arg(ui.nameEdit->text()).arg("%id"));
}
void renamedialog::addisonumber()
{
	ui.nameEdit->setText(QString("%1%2").arg(ui.nameEdit->text()).arg("%in"));
}
void renamedialog::addtitle()
{
	ui.nameEdit->setText(QString("%1%2").arg(ui.nameEdit->text()).arg("%tl"));
}
void renamedialog::addextension()
{
	ui.nameEdit->setText(QString("%1%2").arg(ui.nameEdit->text()).arg("%ex"));
}
void renamedialog::adddiscid()
{
	ui.nameEdit->setText(QString("%1%2").arg(ui.nameEdit->text()).arg("%dc"));
}
void renamedialog::addregion()
{
	ui.nameEdit->setText(QString("%1%2").arg(ui.nameEdit->text()).arg("%rg"));
}
void renamedialog::addregionsmall()
{
	ui.nameEdit->setText(QString("%1%2").arg(ui.nameEdit->text()).arg("%rs"));
}
void renamedialog::addfw()
{
	ui.nameEdit->setText(QString("%1%2").arg(ui.nameEdit->text()).arg("%fw"));
}
void renamedialog::addtype()
{
	ui.nameEdit->setText(QString("%1%2").arg(ui.nameEdit->text()).arg("%tp"));
}
void renamedialog::startAction()
{
	if(ui.nameEdit->text()=="") return;//if no new rule is set for renaming , return;
	renamed_count = 0;
    failed_files.clear();
    
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	subRename(/*theSettings->m_ini.value("umdgamespath").toString(), */ui.nameEdit->text());
	reloadContents();
    QApplication::restoreOverrideCursor();
	
    if (failed_files.count() != 0) {
        errorDialog();
    }
	QMessageBox::information(this, "pspudb", tr("%1 file(s) renamed").arg(renamed_count));
}
void renamedialog::subRename(/*QString dir_path, */QString new_name)
{
	//QDir directory (dir_path);
	
	for (int i = 0; i < ui.umdlist->count(); ++i) {
        QString file_name = new_name;
		QString filepath = ui.umdlist->item(i)->data(Qt::UserRole).toString();
		QFileInfo file_info (filepath);
		if (ui.umdlist->item(i)->checkState() == Qt::Unchecked) continue;
		QSqlQuery query(dbadv);
		query.prepare("SELECT games.releaseNumber ,images.isonumber , games.title , images.discid,games.region,images.firmware ,images.isotype, cache.id , games.type FROM collection.cache , images,games WHERE cache.crc32 = images.crc AND images.gameid = games.id AND cache.available =1 AND cache.path=?");
		query.addBindValue(filepath);
		query.exec();
		int adv_id=0;
		int iso_number=0;
		QString title;
		QString discid;
		QString region;
		QString firmware;
		QString type;

		int cache_id = 0;
        if(query.first())
		{
			adv_id=query.value(0).toUInt();
			iso_number=query.value(1).toUInt();
			title=query.value(2).toString();
			discid=query.value(3).toString();
			region=query.value(4).toString();
			firmware=query.value(5).toString();

			cache_id= query.value(7).toUInt();
			type = query.value(8).toString();

		}
		else
		{
			failed_files << "NOT DATA IN DATABASE FOR: " +file_info.absoluteFilePath(); continue; //if no data found in database better ignore it
		}
		file_name.replace("%id", QString::number(adv_id).prepend("0000").right(4));
		file_name.replace("%in", QString::number(iso_number).prepend("000").right(3));
		file_name.replace("%tl", title);
		file_name.replace("%dc", discid);
		file_name.replace("%rg", region);
		file_name.replace("%fw", firmware);
		file_name.replace("%ex", QString(".%1").arg(file_info.suffix()));
		file_name.replace("%tp", type);

		//fix short region
		QString shortregion;
		if(region=="USA")  shortregion = "US";
		if(region=="Australia") shortregion="AU";
		if(region=="China") shortregion="CN";
		if(region=="Europe") shortregion="EU";
		if(region=="France") shortregion="FR";
		if(region=="Germany") shortregion="DE";
		if(region=="Italy") shortregion="IT";
		if(region=="Japan") shortregion="JP";
		if(region=="Netherlands") shortregion="NL";
		if(region=="South Korea") shortregion="KS";
		if(region=="Spain") shortregion="ES";
		if(region=="Sweden") shortregion="SE";
		if(region=="Greece") shortregion="GR";
		if(region=="Russia") shortregion="RU";

		file_name.replace("%rs", shortregion);

		file_name.replace(":" , "-"); //bug if there is a : in filename
		file_name.replace("*" , " ");//no * allowed
		file_name.replace("?" , " ");//no ? allowed
		file_name.replace("/" , " ");//no / allowed
		file_name.replace("|" , " ");//no | allowed
		file_name.replace("<" , " ");//no < allowed
		file_name.replace(">" , " ");//no > allowed

		QFile file (file_info.absoluteFilePath());	
		QFileInfo renameinfo(file_info.absolutePath()); 
		QString renamefile = QString("%1/%2").arg(renameinfo.absoluteFilePath()).arg(file_name);
		if (!file.rename(renamefile)) {
            failed_files << "CAN'T RENAME: " +file_info.absoluteFilePath();
		} 
		else 
		{ 
			renamed_count++; 
			//add the new path to database

			QSqlQuery query9(dbadv);
			query9.prepare("UPDATE cache SET path =? where id=?");
			query9.addBindValue(renamefile);
			query9.addBindValue(cache_id);
			query9.exec();
		}
		qApp->processEvents();//maybe stop freezing
	}
}
void renamedialog::errorDialog()
{
    QDialog * error_dial = new QDialog(this);
    error_dial->setWindowTitle(tr("Failed to rename files"));
    QGridLayout * error_glayout = new QGridLayout(error_dial);
    QLabel * label1 = new QLabel (error_dial);
    label1->setText(tr("Failed to rename files:"));
    error_glayout->addWidget(label1, 0, 0);
    QTextBrowser * error_tbw = new QTextBrowser (error_dial);
    QString errors_str;
    for (int i = 0; i < failed_files.count(); ++i) {
        errors_str.append(QString("%1<br>").arg(failed_files.at(i)));
    }
    error_tbw->setHtml(errors_str);
    error_glayout->addWidget(error_tbw, 1, 0);
    QHBoxLayout * hlayout = new QHBoxLayout;
    hlayout->addStretch();
    QPushButton * error_quit_btn = new QPushButton (error_dial);
    error_quit_btn->setText(tr("Close"));
    QObject::connect(error_quit_btn, SIGNAL(released()), error_dial, SLOT(close()));
    hlayout->addWidget(error_quit_btn);
    error_glayout->addLayout(hlayout, 2, 0);
    error_dial->show();
}