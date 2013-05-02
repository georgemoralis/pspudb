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

#ifndef COMPRESS7SODIALOG_H
#define COMPRESS7SODIALOG_H

#include <QDialog>
#include "ui_compress7sodialog.h"

class compress7soDialog : public QDialog, public Ui::compress7soDialog
{
	Q_OBJECT

public:
	compress7soDialog(QWidget *parent = 0);
	~compress7soDialog();

	virtual bool event( QEvent* event );

	QStringList getFileNames() const;
    int			getCompression() const;

protected:
	void localeChanged();

private:
	QStringList filePaths;

private slots:
    void on_pushButtonFile_clicked();
};

#endif // COMPRESS7SODIALOG_H
