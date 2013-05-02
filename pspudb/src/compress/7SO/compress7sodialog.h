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
