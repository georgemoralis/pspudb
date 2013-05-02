#ifndef UMDWIDGET_H
#define UMDWIDGET_H

#include <QWidget>
#include <qsortfilterproxymodel.h>
#include <QStandardItemModel>
#include "ui_umdwidget.h"


class umdWidget : public QWidget
{
	Q_OBJECT

public:
	umdWidget(QWidget *parent = 0);
	~umdWidget();
	void scanGames();
	void resetselection();

public slots:
	void RefreshUmd();
	void onCurrentChanged(QModelIndex const &index);
	void imageClicked(QModelIndex);
	void RunJPCSP();
	void RunPCSP();
	void RunPSPE();
	void RunPSPEMUD();
	void RunCSPSPEMU();
	void RunPPSSPP();
private:
	Ui::umdWidget ui;
	QItemSelectionModel   *m_selectionModel;
	QSortFilterProxyModel *m_model;

	QItemSelectionModel   *mp_selectionModel;
	QSortFilterProxyModel *mp_model;

	//screenshot viewer
	QStringList                 m_imageNamesList;
	QStandardItemModel*         m_screenshotsModel;

};

#endif // UMDWIDGET_H
