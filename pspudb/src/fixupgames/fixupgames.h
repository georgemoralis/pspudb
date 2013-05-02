#ifndef FIXUPGAMES_H
#define FIXUPGAMES_H

#include <QDialog>
#include <QSqlQueryModel>
#include "ui_fixupgames.h"

class fixupgames : public QDialog
{
	Q_OBJECT

public:
	enum fixupTask {
		FixupStatus = 0x01,
		FixupImages = 0x02,
		FixupFW = 0x03
	};

	fixupgames(QWidget *parent = 0);
	~fixupgames();
	QStringList getpaths();
	fixupgames::fixupTask fTask;
public slots:
	void fixupstatus();
	void scanstatus();
	void fixupImages();
	void scanImages();
	void scanFW();
	void fixupFW();
private:
	Ui::fixupgames ui;
	QStringList paths;
};

#endif // FIXUPGAMES_H
