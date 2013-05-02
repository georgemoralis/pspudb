#ifndef QIMAGEVIEWER_H
#define QIMAGEVIEWER_H

#include <QDialog>
#include "ui_qimageviewer.h"

class qImageViewer : public QDialog
{
	Q_OBJECT

public:
	qImageViewer(QStringList imagelist , int index=0,QWidget *parent = 0);
	~qImageViewer();
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
private:
	Ui::qImageViewer ui;
	QStringList imagelist;
    int index;
	double scaleFactor;

public slots:
	void goNextAction();
	void goPreviousAction();
	void zoomInAction();
	void zoomOutAction();
	void actualSizeAction();
};

#endif // QIMAGEVIEWER_H
