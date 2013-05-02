#ifndef FILEBROWSERWIDGET_H
#define FILEBROWSERWIDGET_H

#include <QWidget>
#include "ui_filebrowserwidget.h"
#include "files.h"

class fileBrowserWidget : public QWidget
{
	Q_OBJECT

public:
	fileBrowserWidget(QWidget *parent = 0);
	~fileBrowserWidget();
	bool scanLocaldir();
	void scanDir(QString dir, QList<files> &list);

public slots:
	void RefreshFilesTable(int index);
	void onCurrentChanged(QModelIndex const &index);
	void RunJPCSP();
	void RunCSPSPEMU();
	void RunPCSP();
	void RunPSPE();
private:
	Ui::fileBrowserWidget ui;
	QList<files> appLocalList;
	QString bootbinpath;

			//table
	 QAbstractItemModel *model;
     QItemSelectionModel *selectionModel;
	 void setupModel(QList<files> &list);
};

#endif // FILEBROWSERWIDGET_H
