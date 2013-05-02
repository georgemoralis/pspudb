#include "ProgressDialog.h"
#include <QPushButton>
#include <QTimer>
#include <QDesktopWidget>
#include <QDebug>

ProgressDialog::ProgressDialog( QWidget* parent )
	: QDialog( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose ); //if you want the widget to be deleted when it is closed, create it with the Qt::WA_DeleteOnClose flag
	cbDetails->setChecked( false );
	dbbButtons->button( QDialogButtonBox::Ok )->setEnabled( false );
	dbbButtons->button( QDialogButtonBox::Cancel )->setEnabled( false );
	//intialaze worker thread
	mThread = new WorkerThread( this );
	doConnections();
	//check for language change
	localeChanged();
}

ProgressDialog::~ProgressDialog()
{

}

bool ProgressDialog::event( QEvent* event )
{
	switch ( event->type() ) {
		case QEvent::LocaleChange:
			localeChanged();
			break;
		default:
			break;
	}
	
	return QDialog::event( event );
}

void ProgressDialog::setWork( const WorkerThread::Work& work )
{
	open();
	
	this->work = work;
	if ( !mThread->setWork( work ) ) {
		deleteLater();
	}
}

void ProgressDialog::done( int r )
{
	if ( mThread->isRunning() ) {
		return;
	}

	QDialog::done( r );
}

void ProgressDialog::closeEvent( QCloseEvent* event )
{
	if ( mThread->isRunning() ) {
		event->ignore();
		return;
	}
	
	QDialog::closeEvent( event );
}

void ProgressDialog::localeChanged()
{
	retranslateUi( this );
}

void ProgressDialog::doConnections()
{
	connect( dbbButtons->button( QDialogButtonBox::Ok ), SIGNAL( clicked() ), this, SLOT( close() ) );
	connect( dbbButtons->button( QDialogButtonBox::Cancel ), SIGNAL( clicked() ), mThread, SLOT( stop() ) );
	connect( mThread, SIGNAL( started() ), this, SLOT( thread_started() ) );
	connect( mThread, SIGNAL( message( const QString& ) ), this, SLOT( thread_message( const QString& ) ) );
	connect( mThread, SIGNAL( log( const QString& ) ), this, SLOT( thread_log( const QString& ) ) );
	connect( mThread, SIGNAL( jobFinished( const QString& ) ), this, SLOT( thread_jobFinished( const QString& ) ) );
	connect( mThread, SIGNAL( currentProgressChanged( int, int) ), this, SLOT( thread_currentProgressChanged( int, int) ) );
	connect( mThread, SIGNAL( globalProgressChanged( int, int ) ), this, SLOT( thread_globalProgressChanged( int, int ) ) );
	connect( mThread, SIGNAL( canceled() ), this, SLOT( thread_canceled() ) );
	connect( mThread, SIGNAL( finished() ), this, SLOT( thread_finished() ) );
}

void ProgressDialog::thread_started()
{
	mElapsed.restart();
	dbbButtons->button( QDialogButtonBox::Cancel )->setEnabled( true );
}

void ProgressDialog::thread_message( const QString& text )
{
	lCurrentInformations->setText( text );
	pteErrors->appendPlainText( text );
}

void ProgressDialog::thread_log( const QString& text )
{
	pteErrors->appendPlainText( text );
	
	if ( !cbDetails->isChecked() ) {
		cbDetails->toggle();
	}
}

void ProgressDialog::thread_jobFinished( const QString& filename )
{
	const QString text = QString( "%1 '%2' completed" )
		.arg( WorkerThread::taskToLabel( mThread->task() ) )
		.arg( filename );

	pteErrors->appendPlainText( text );
	
	emit jobFinished( filename );


}

void ProgressDialog::thread_currentProgressChanged( int value, int maximum)
{
	pbCurrent->setMaximum( maximum );
	pbCurrent->setValue( value );
}

void ProgressDialog::thread_globalProgressChanged( int value, int maximum )
{
	pbGlobal->setMaximum( maximum );
	pbGlobal->setValue( value );
}

void ProgressDialog::thread_canceled()
{
	lGlobalInformations->setText( tr( "Cancel requested, the process will stop after the current operation." ) );
}

void ProgressDialog::thread_finished()
{
	QTime time( 0, 0, 0, 0 );
	time = time.addMSecs( mElapsed.elapsed() );
	
	lCurrentInformations->setText( "Tasks finished." );
	lGlobalInformations->setText( tr( "All Tasks finished in %1" ).arg( time.toString() ) );
	dbbButtons->button( QDialogButtonBox::Ok )->setEnabled( true );
	dbbButtons->button( QDialogButtonBox::Cancel )->setEnabled( false );
	
	emit finished();
	if(work.task == 0x14 /*WorkerThread::Task::ScanUMD*/) //if we are in scanning mode close the dialog after scannning
	{
		emit requestRefresh();
		close();
	}
}

void ProgressDialog::on_cbDetails_toggled()
{
	QTimer::singleShot( 0, this, SLOT( updateSpace() ) );
}

void ProgressDialog::updateSpace()
{
	QWidget* widget = parentWidget();
	QRect rect = geometry();
	
	if ( !widget ) {
		widget = QApplication::desktop();
	}
	
	widget = widget->window();
	
	if ( !cbDetails->isChecked() ) {
		rect.setHeight( minimumSizeHint().height() );
	}
	
	rect.moveCenter( widget->mapToGlobal( widget->rect().center() ) );
#if defined( Q_OS_MAC )
	rect.moveTop( widget->mapToGlobal( widget->rect().topLeft() ).y() );
#endif
	resize( rect.size() );
	move( rect.topLeft() );
}
