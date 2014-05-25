#include "simplemazegen.h"

simplemazegen::simplemazegen(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));

	connect(ui.heightSlider,SIGNAL(valueChanged(int)),this,SLOT(setHeightValue()));
	connect(ui.widthSlider,SIGNAL(valueChanged(int)),this,SLOT(setWidthValue()));
	connect(ui.generateButton,SIGNAL(clicked(bool)),this,SLOT(onGenerateBtnClicked()));

	setWidthValue();
	setHeightValue();
}

simplemazegen::~simplemazegen()
{

}

void simplemazegen::setWidthValue()	// width slider - ползунок ширины
{
	ui.widthLineEdit->setText(QString::number(ui.widthSlider->value()));
	selectedMazeWidth = ui.widthSlider->value();
}

void simplemazegen::setHeightValue() // height slider - ползунок высоты
{
	ui.heigthLineEdit->setText(QString::number(ui.heightSlider->value()));
	selectedMazeHeight = ui.heightSlider->value();
}

void simplemazegen::onGenerateBtnClicked() // make maze! - создать лабиринт по клику
{
	maze(selectedMazeWidth,selectedMazeHeight, ui.algorythmComboBox->currentIndex()).exec();
}