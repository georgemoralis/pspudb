#define VERSION "v2.00"

#include "pspudb.h"
#include <QtGui/QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	pspudb w;
	w.setWindowTitle("pspudb " VERSION);
	w.show();
	return a.exec();
}
