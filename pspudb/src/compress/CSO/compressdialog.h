#ifndef COMPRESSDIALOG_H
#define COMPRESSDIALOG_H

#include <QDialog>
#include "ui_compressdialog.h"

class compressDialog : public QDialog, public Ui::compressDialog
{
	Q_OBJECT

public:
	compressDialog(QWidget *parent = 0);
	~compressDialog();

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

#endif // COMPRESSDIALOG_H
