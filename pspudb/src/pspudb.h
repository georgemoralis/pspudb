#ifndef PSPUDB_H
#define PSPUDB_H

#include <QtGui/QMainWindow>
#include <QFileDialog>
#include "ui_pspudb.h"
#include "demoTab\filebrowserwidget.h"
#include "applicationsTab\applicationwidget.h"
#include "SavesTab\saveswidget.h"
#include "umdTab\umdwidget.h"

class pspudb : public QMainWindow
{
	Q_OBJECT

public:
	pspudb(QWidget *parent = 0, Qt::WFlags flags = 0);
	~pspudb();
private:
	Ui::pspudbClass ui;
	fileBrowserWidget *filestab;
	applicationWidget *apptab;
	savesWidget *savestab;
	umdWidget *umdtab;

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void on_actionConvert_ISO_CSO_triggered(); //NOTE you don't need to add a signal to QTDesigner just use _triggerred() action for menus
	void on_actionConvert_CSO_ISO_triggered();
	void on_actionConvert_ISO_7SO_triggered();
	void on_actionConvert_7SO_ISO_triggered();
	void on_actionGeneralSettings_triggered();
	void on_actionRename_Tool_triggered();
	void on_actionFixup_UMDs_entries_triggered();
};

#endif // PSPUDB_H
