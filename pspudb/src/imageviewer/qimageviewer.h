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

#ifndef QIMAGEVIEWER_H
#define QIMAGEVIEWER_H

#include <QDialog>
#include "ui_qimageviewer.h"

class qImageViewer : public QDialog
{
	Q_OBJECT

public:
	qImageViewer(QStringList imagelist , int index=0,QWidget *parent = 0);
	~qImageViewer();
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
private:
	Ui::qImageViewer ui;
	QStringList imagelist;
    int index;
	double scaleFactor;

public slots:
	void goNextAction();
	void goPreviousAction();
	void zoomInAction();
	void zoomOutAction();
	void actualSizeAction();
};

#endif // QIMAGEVIEWER_H
