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
