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

#ifndef FILEBROWSERWIDGET_H
#define FILEBROWSERWIDGET_H

#include <QWidget>
#include "ui_filebrowserwidget.h"
#include "files.h"

class fileBrowserWidget : public QWidget
{
	Q_OBJECT

public:
	fileBrowserWidget(QWidget *parent = 0);
	~fileBrowserWidget();
	bool scanLocaldir();
	void scanDir(QString dir, QList<files> &list);

public slots:
	void RefreshFilesTable(int index);
	void onCurrentChanged(QModelIndex const &index);
	void RunJPCSP();
	void RunCSPSPEMU();
	void RunPCSP();
	void RunPSPE();
private:
	Ui::fileBrowserWidget ui;
	QList<files> appLocalList;
	QString bootbinpath;

			//table
	 QAbstractItemModel *model;
     QItemSelectionModel *selectionModel;
	 void setupModel(QList<files> &list);
};

#endif // FILEBROWSERWIDGET_H
