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
