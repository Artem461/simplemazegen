#ifndef MAZE_H
#define MAZE_H

#include <QDialog>
#include "ui_maze.h"
#include <ctime>
#include <vector>
#include <utility>

using namespace std;
class maze : public QDialog
{
	Q_OBJECT

public:
	maze(int _mazeWidth=2, int _mazeHeight=2, int _currentAlgo = DFS, QWidget *parent = 0);
	~maze();

private:
	Ui::maze ui;

	// current generate algorythm
	enum algorythm {DFS,HEURISTIC};
	algorythm currentAlgo;
	// ������� ���������
	int mazeWidth, mazeHeight;
	// ������� ��������� ��� ������ ��������� ��������� � ��������� ����������
	vector<vector<int>> matrix,mazeMatrix;
	// ���������� ������� ��� ������ � �������
	vector<bool> visited;
	// ����� ������
	int vertexCount;
	// ������������ ����� ���������
	int vertexSpace;
	
	void buildMatrix();
	int getVertexNumber(int currentXPos, int currentYPos);
	void getVertexCoords(int number, int &x, int &y);
	
	void dfs(int start);
	void heuriscticBuild();

	void makeEdge(int start, int finish);
	void makeReachable(int start, int finish);

	void paintEvent(QPaintEvent *);
	void drawMaze(QPainter *painter);
};

#endif // MAZE_H
