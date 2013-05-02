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

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <qdialog.h>
#include "ui_ProgressDialog.h"
#include "..\WorkerThread.h"
#include "pspudb.h"


class ProgressDialog : public QDialog, public Ui::ProgressDialog
{
	Q_OBJECT
	
public:
	ProgressDialog( QWidget* parent = 0 );
	virtual ~ProgressDialog();
	
	virtual bool event( QEvent* event );
	
	void setWork( const WorkerThread::Work& work );


public slots:
	virtual void done( int r );

protected:
	WorkerThread* mThread;
	WorkerThread::Work work;
	QTime mElapsed;
	pspudb* parentwindow;
	
	void closeEvent( QCloseEvent* event );
	void localeChanged();
	void doConnections();

protected slots:
	void thread_started();
	void thread_message( const QString& text );
	void thread_log( const QString& text );
	void thread_jobFinished( const QString& filename);
	void thread_currentProgressChanged( int value, int maximum);
	void thread_globalProgressChanged( int value, int maximum );
	void thread_canceled();
	void thread_finished();
	void on_cbDetails_toggled();
	void updateSpace();

signals:
	void jobFinished( const QString& filename );
	void finished();
	void requestRefresh();
};

#endif // PROGRESSDIALOG_H
