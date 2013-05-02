#ifndef SAVESWIDGET_H
#define SAVESWIDGET_H

#include <QWidget>
#include "ui_saveswidget.h"
#include "savegame.h"

class savesWidget : public QWidget
{
	Q_OBJECT

public:
	savesWidget(QWidget *parent = 0);
	~savesWidget();

	void scanDir(QString dir, QList<Savegame> &list);
	bool scanJPCSPdir();
	bool scanPCSPdir();
	bool scanPSPEdir();
	QString getPSPSavegameDirectory(QString PSPDirectory);
	bool scanPSPdir();
	bool scanLocalDir();
	bool askDelete(int size);
	void deleteSaves(QList<QString> paths);
	bool askAppOverwrite(QString appName);
	void copySaves(QList<QString> paths,QString emu);

public slots:
	void RefreshSaveTable(int index);
	void onSelectionChanged(QItemSelection const &selection);
	 void showCollectionContextMenu(const QPoint &);
private:
	Ui::savesWidget ui;
	QList<Savegame> m_saveJPCSPList;
	QList<Savegame> m_savePCSPList;
	QList<Savegame> m_savePSPEList;
	QList<Savegame> m_savePSPList;
	QList<Savegame> m_savePSPLocal;

	//table
	 QAbstractItemModel *model;
     QItemSelectionModel *selectionModel;
	 void setupModel(QList<Savegame> &list);
};

#endif // SAVESWIDGET_H
