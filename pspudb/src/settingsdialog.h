#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QtCore>
#include <QtGui>
#include "ui_settingsdialog.h"

class settingsdialog : public QDialog
{
	Q_OBJECT

public:
	settingsdialog(QWidget *parent = 0);
	~settingsdialog();
	bool requiresUpdate();
public slots:
	void browsejpcspbutton();
	void browsepcspbutton();
	void browsepspemubutton();
	void browsecpspemubutton();
	void browsepspdevicebutton();
	void browselocalpathbutton();
	void browsepspebutton();
	void browseppssppbutton();
	void addUmdDir();
	void removeUmdDir();
	void save();

private:
	Ui::settingsdialog ui;
	QString tempumdpaths;
	bool needsupdate;
};

#endif // SETTINGSDIALOG_H
