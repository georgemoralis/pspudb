#ifndef APPLICATIONWIDGET_H
#define APPLICATIONWIDGET_H

#include <QWidget>
#include "ui_applicationwidget.h"
#include "pspapplication.h"

class applicationWidget : public QWidget
{
	Q_OBJECT

public:
	applicationWidget(QWidget *parent = 0);
	~applicationWidget();
	void scanDir(QString dir, QList<PSPApplication> &list);
	bool scanJPCSPdir();
	bool scanPCSPdir();
	bool scanPSPEdir();
	bool scanPSPdir();
	bool scanLocaldir();
	bool findApp(QList<PSPApplication> &list, QString path,QString originalpath);
	QString getAppDirectory(QString PSPDirectory);

public slots:
	void RefreshApplicationTable(int index);
	void onCurrentChanged(QModelIndex const &index);
	void RunJPCSP();
	void RunCSPSPEMU();
	void RunPCSP();
	void RunPSPE();
private:
	QString bootbinpath;
	Ui::applicationWidget ui;
	QList<PSPApplication> appJPCSPList;
	QList<PSPApplication> appPCSPList;
	QList<PSPApplication> appPSPEList;
	QList<PSPApplication> appPSPList;
	QList<PSPApplication> appLocalList;
		//table
	 QAbstractItemModel *model;
     QItemSelectionModel *selectionModel;
	 void setupModel(QList<PSPApplication> &list);
};

#endif // APPLICATIONWIDGET_H
