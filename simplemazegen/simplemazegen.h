#ifndef SIMPLEMAZEGEN_H
#define SIMPLEMAZEGEN_H

#include <QtGui/QMainWindow>
#include "ui_simplemazegen.h"

class simplemazegen : public QMainWindow
{
	Q_OBJECT

public:
	simplemazegen(QWidget *parent = 0, Qt::WFlags flags = 0);
	~simplemazegen();

private:
	Ui::simplemazegenClass ui;
};

#endif // SIMPLEMAZEGEN_H
