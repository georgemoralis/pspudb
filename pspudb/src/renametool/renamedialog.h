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

#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>
#include "ui_renamedialog.h"

class renamedialog : public QDialog
{
	Q_OBJECT

public:
	renamedialog(QWidget *parent = 0);
	~renamedialog();
	void reloadContents();
	void subReloadContents(QString path);
	void subRename(/*QString dir_path,*/ QString new_name);

	//menu buttons
	void createMenus();
public slots:
	void selectallAction();
	void selectnoneAction();
	void startAction();
	void errorDialog();

	//menu slots
	void addgameid();
	void addisonumber();
	void addtitle();
	void addextension();
	void adddiscid();
	void addregion();
	void addfw();
	void addregionsmall();
	void addtype();

	void addt1();
	void addt2();
	void addt3();

private:
	Ui::renamedialog ui;
	QStringList failed_files;
    int renamed_count;
};

#endif // RENAMEDIALOG_H
