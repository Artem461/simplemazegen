#include "simplemazegen.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	simplemazegen w;
	w.show();
	return a.exec();
}
