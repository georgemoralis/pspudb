#include "compressdialog.h"
#include <QFileDialog>

compressDialog::compressDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi( this );
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	//check for language change
	localeChanged();
}

compressDialog::~compressDialog()
{

}
bool compressDialog::event( QEvent* event )
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
void compressDialog::localeChanged()
{
	//NOTE comobox doesn't restored after so hack it like this
	int previous = comboBoxCompression->currentText().toInt();
	retranslateUi( this );
	comboBoxCompression->setCurrentIndex(previous);
}
QStringList compressDialog::getFileNames() const
{
    return filePaths;
}
void compressDialog::on_pushButtonFile_clicked()
{
	//select files or file
	filePaths = QFileDialog::getOpenFileNames( this, tr( "Choose ISO files to convert" ), QString::null, tr( "ISO Files (*.iso)" ) );
	labelFileSelected->setText(("%1 " + tr( "file(s) selected")).arg(filePaths.size()));
}
int compressDialog::getCompression() const
{
    return comboBoxCompression->currentText().toInt();
}