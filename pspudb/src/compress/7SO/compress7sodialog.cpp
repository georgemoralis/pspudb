#include "compress7sodialog.h"
#include <QFileDialog>
#include "ext\liblzma\include\lzma.h"

compress7soDialog::compress7soDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi( this );
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	//check for language change
	localeChanged();
}

compress7soDialog::~compress7soDialog()
{

}
bool compress7soDialog::event( QEvent* event )
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
void compress7soDialog::localeChanged()
{
	//NOTE comobox doesn't restored after so hack it like this
	int previous = comboCompression->currentText().toInt();
	retranslateUi( this );
	comboCompression->setCurrentIndex(previous);
}
QStringList compress7soDialog::getFileNames() const
{
    return filePaths;
}
void compress7soDialog::on_pushButtonFile_clicked()
{
	//select files or file
	filePaths = QFileDialog::getOpenFileNames( this, tr( "Choose ISO files to convert" ), QString::null, tr( "ISO Files (*.iso)" ) );
	labelFileSelected->setText(("%1 " + tr( "file(s) selected")).arg(filePaths.size()));
}
int compress7soDialog::getCompression() const
{
	if(extremeCheckBox->isChecked())
	{
	   int compression = comboCompression->currentText().toInt();
	   compression |= LZMA_PRESET_EXTREME;
	   return compression;
	}
	else
	    return comboCompression->currentText().toInt();
}