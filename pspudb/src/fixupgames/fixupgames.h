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

#ifndef FIXUPGAMES_H
#define FIXUPGAMES_H

#include <QDialog>
#include <QSqlQueryModel>
#include "ui_fixupgames.h"

class fixupgames : public QDialog
{
	Q_OBJECT

public:
	enum fixupTask {
		FixupStatus = 0x01,
		FixupImages = 0x02,
		FixupFW = 0x03
	};

	fixupgames(QWidget *parent = 0);
	~fixupgames();
	QStringList getpaths();
	fixupgames::fixupTask fTask;
public slots:
	void fixupstatus();
	void scanstatus();
	void fixupImages();
	void scanImages();
	void scanFW();
	void fixupFW();
private:
	Ui::fixupgames ui;
	QStringList paths;
};

#endif // FIXUPGAMES_H
