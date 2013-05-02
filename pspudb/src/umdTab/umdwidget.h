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

#ifndef UMDWIDGET_H
#define UMDWIDGET_H

#include <QWidget>
#include <qsortfilterproxymodel.h>
#include <QStandardItemModel>
#include "ui_umdwidget.h"


class umdWidget : public QWidget
{
	Q_OBJECT

public:
	umdWidget(QWidget *parent = 0);
	~umdWidget();
	void scanGames();
	void resetselection();

public slots:
	void RefreshUmd();
	void onCurrentChanged(QModelIndex const &index);
	void imageClicked(QModelIndex);
	void RunJPCSP();
	void RunPCSP();
	void RunPSPE();
	void RunPSPEMUD();
	void RunCSPSPEMU();
	void RunPPSSPP();
private:
	Ui::umdWidget ui;
	QItemSelectionModel   *m_selectionModel;
	QSortFilterProxyModel *m_model;

	QItemSelectionModel   *mp_selectionModel;
	QSortFilterProxyModel *mp_model;

	//screenshot viewer
	QStringList                 m_imageNamesList;
	QStandardItemModel*         m_screenshotsModel;

};

#endif // UMDWIDGET_H
