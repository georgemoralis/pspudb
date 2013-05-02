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

#ifndef APPLICATIONWIDGET_H
#define APPLICATIONWIDGET_H

#include <QWidget>
#include "ui_applicationwidget.h"
#include "pspapplication.h"

class applicationWidget : public QWidget
{
	Q_OBJECT

public:
	applicationWidget(QWidget *parent = 0);
	~applicationWidget();
	void scanDir(QString dir, QList<PSPApplication> &list);
	bool scanJPCSPdir();
	bool scanPCSPdir();
	bool scanPSPEdir();
	bool scanPSPdir();
	bool scanLocaldir();
	bool findApp(QList<PSPApplication> &list, QString path,QString originalpath);
	QString getAppDirectory(QString PSPDirectory);

public slots:
	void RefreshApplicationTable(int index);
	void onCurrentChanged(QModelIndex const &index);
	void RunJPCSP();
	void RunCSPSPEMU();
	void RunPCSP();
	void RunPSPE();
private:
	QString bootbinpath;
	Ui::applicationWidget ui;
	QList<PSPApplication> appJPCSPList;
	QList<PSPApplication> appPCSPList;
	QList<PSPApplication> appPSPEList;
	QList<PSPApplication> appPSPList;
	QList<PSPApplication> appLocalList;
		//table
	 QAbstractItemModel *model;
     QItemSelectionModel *selectionModel;
	 void setupModel(QList<PSPApplication> &list);
};

#endif // APPLICATIONWIDGET_H
