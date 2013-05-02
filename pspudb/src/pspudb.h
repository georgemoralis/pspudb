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

#ifndef PSPUDB_H
#define PSPUDB_H

#include <QtGui/QMainWindow>
#include <QFileDialog>
#include "ui_pspudb.h"
#include "demoTab\filebrowserwidget.h"
#include "applicationsTab\applicationwidget.h"
#include "SavesTab\saveswidget.h"
#include "umdTab\umdwidget.h"

class pspudb : public QMainWindow
{
	Q_OBJECT

public:
	pspudb(QWidget *parent = 0, Qt::WFlags flags = 0);
	~pspudb();
private:
	Ui::pspudbClass ui;
	fileBrowserWidget *filestab;
	applicationWidget *apptab;
	savesWidget *savestab;
	umdWidget *umdtab;

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void on_actionConvert_ISO_CSO_triggered(); //NOTE you don't need to add a signal to QTDesigner just use _triggerred() action for menus
	void on_actionConvert_CSO_ISO_triggered();
	void on_actionConvert_ISO_7SO_triggered();
	void on_actionConvert_7SO_ISO_triggered();
	void on_actionGeneralSettings_triggered();
	void on_actionRename_Tool_triggered();
	void on_actionFixup_UMDs_entries_triggered();
};

#endif // PSPUDB_H
