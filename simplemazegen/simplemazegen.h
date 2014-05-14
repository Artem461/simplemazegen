#ifndef SIMPLEMAZEGEN_H
#define SIMPLEMAZEGEN_H

#include <QtGui/QMainWindow>
#include "ui_simplemazegen.h"

#include "maze.h"

#include <qtextcodec.h>
#include <qstring.h>


class simplemazegen : public QMainWindow
{
	Q_OBJECT

public:
	simplemazegen(QWidget *parent = 0, Qt::WFlags flags = 0);
	~simplemazegen();

private:
	Ui::simplemazegenClass ui;

	int selectedMazeWidth,selectedMazeHeight;

private slots:
	void setWidthValue();
	void setHeightValue();

	void onGenerateBtnClicked();
};

#endif // SIMPLEMAZEGEN_H
