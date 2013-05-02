#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>
#include "ui_renamedialog.h"

class renamedialog : public QDialog
{
	Q_OBJECT

public:
	renamedialog(QWidget *parent = 0);
	~renamedialog();
	void reloadContents();
	void subReloadContents(QString path);
	void subRename(/*QString dir_path,*/ QString new_name);

	//menu buttons
	void createMenus();
public slots:
	void selectallAction();
	void selectnoneAction();
	void startAction();
	void errorDialog();

	//menu slots
	void addgameid();
	void addisonumber();
	void addtitle();
	void addextension();
	void adddiscid();
	void addregion();
	void addfw();
	void addregionsmall();
	void addtype();

	void addt1();
	void addt2();
	void addt3();

private:
	Ui::renamedialog ui;
	QStringList failed_files;
    int renamed_count;
};

#endif // RENAMEDIALOG_H
