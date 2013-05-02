// Copyright (c) 2013- PSPUDB Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/georgemoralis/pspudb and http://www.pspudb.com/

#include "qimageviewer.h"

#include <QScrollBar>
qImageViewer::qImageViewer(QStringList image_list , int image_index, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	imagelist=image_list;
    index=image_index;
	QImage image(imagelist.at(index));
	ui.imageLabel->setPixmap(QPixmap::fromImage(image));
	ui.imageLabel->adjustSize();
	scaleFactor=1.0;
}

qImageViewer::~qImageViewer()
{

}
void qImageViewer::scaleImage(double factor)
 {
     scaleFactor *= factor;
     ui.imageLabel->resize(scaleFactor * ui.imageLabel->pixmap()->size());

     adjustScrollBar(ui.scrollArea->horizontalScrollBar(), factor);
     adjustScrollBar(ui.scrollArea->verticalScrollBar(), factor);

    // zoomInAct->setEnabled(scaleFactor < 3.0);
     //zoomOutAct->setEnabled(scaleFactor > 0.333);
 }

 void qImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
 {
     scrollBar->setValue(int(factor * scrollBar->value()
                             + ((factor - 1) * scrollBar->pageStep()/2)));
 }
 void qImageViewer::goNextAction()
 {
	 int max = imagelist.size()-1;
	 if(index==max)
	 {
		 QImage image(imagelist.at(index));
		 ui.imageLabel->setPixmap(QPixmap::fromImage(image));
	 }
	 else
	 {
		 index++;
		 QImage image(imagelist.at(index));
		 ui.imageLabel->setPixmap(QPixmap::fromImage(image));
	 }
 }
 void qImageViewer::goPreviousAction()
 {
	 if(index==0)
	 {
		 QImage image(imagelist.at(index));
		 ui.imageLabel->setPixmap(QPixmap::fromImage(image));
	 }
	 else
	 {
		 index--;
		 QImage image(imagelist.at(index));
		 ui.imageLabel->setPixmap(QPixmap::fromImage(image));
	 }
 }
void qImageViewer::zoomInAction()
{
	scaleImage(1.25);
}
void qImageViewer::zoomOutAction()
{
	scaleImage(0.8);
}
void qImageViewer::actualSizeAction()
{
	ui.imageLabel->adjustSize();
    scaleFactor = 1.0;
}
