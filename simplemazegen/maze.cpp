#include <QtGui>
#include "maze.h"

maze::maze(int _mazeWidth, int _mazeHeight, bool _needToBePassed, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	mazeWidth = _mazeWidth;
	mazeHeight = _mazeHeight;

	givePass = _needToBePassed;
	
	vertexCount = (mazeWidth+1) * (mazeHeight+1);	// число вершин
	vertexSpace = 20;

	srand(time(NULL));	// инициализация рандомайзера

	matrix = vector<vector<int>> ( vertexCount,vector<int>(vertexCount,NULL) );
	visited = vector<bool>(vertexCount,false);

	buildMatrix();

	// если лабиринт должен быть проходимым
	if(givePass)
		makePath();
	
	// инициализируем конечную матрицу лабиринта
	mazeMatrix = matrix;

	// depth-first search 
	for(int i=0;i<visited.size();i++)
		if(!visited[i])
			dfs(i);
	
	// задаем оптимальные размеры окна с учетом параметров графа
	this->resize(mazeWidth*vertexSpace+55,mazeHeight*vertexSpace+15);
}

maze::~maze()
{

}

// вычисляет номер вершины по позициям матрицы смежности
int maze::getVertexNumber(int currentXPos, int currentYPos)
{
	return currentYPos*(mazeWidth+1)+currentXPos;
}

// вычисляет позиции вершины в матрице по номеру
void maze::getVertexCoords(int number, int &x, int &y)
{
	x=number%(mazeWidth+1);
	y=0;
	while(number > (mazeWidth+1))
	{
		number -= (mazeWidth+1);
		y++;
	}
}

// построение матрицы смежности для графа полностью закрытого лабиринта
void maze::buildMatrix()
{
	// здесь строим матрицу смежности, включая в неё возможные ребра
	int vertex=0,vTop=0,vBottom=0,vLeft=0,vRight=0;

	for(int i = 1; i < mazeWidth; i++)			
	{
		for(int j = 1; j < mazeHeight; j++)
		{
			vertex = getVertexNumber(i,j);		// для текущей вершины

			vLeft	= getVertexNumber(i-1,j);	// вычисляем номера
			vRight	= getVertexNumber(i+1,j);	// всех смежных
			vTop	= getVertexNumber(i,j-1);	// с ней
			vBottom = getVertexNumber(i,j+1);	// вершин

			matrix[vertex]	[vLeft]		= 
			matrix[vertex]	[vRight]	= 
			matrix[vertex]	[vTop]		= 
			matrix[vertex]	[vBottom]	= 
			matrix[vLeft]	[vertex]	= 
			matrix[vRight]	[vertex]	= 
			matrix[vTop]	[vertex]	= 
			matrix[vBottom]	[vertex]	= 1;	// и указываем что вершина соединена ребрами со всеми смежными
		}
	}	
}

// add a couple random paths from entrance to exit
void maze::makePath()
{
	enum direction {RIGHT,DOWN};
	direction currentDirection=DOWN;

	int currentX=0,currentY=0,nextX=0,nextY=0;	// cell-of-maze coordinates
	int currentVertex=0,nextVertex=0;

	for(int i=0;i<mazeWidth-1 + mazeHeight-1;i++)
	{
		currentVertex=0;
		nextVertex=0;

		if(currentX == mazeWidth-1)
			currentDirection = DOWN;
		else if(currentY == mazeHeight-1)
			currentDirection = RIGHT;
		else
			currentDirection = direction(rand()%2);
	
		if(currentDirection==RIGHT)
		{
			nextX++;
			currentVertex = getVertexNumber(currentX+1,currentY+1);
			nextVertex = getVertexNumber(currentX+1,currentY+1-1);
			currentX=nextX;
		}
		else if(currentDirection=DOWN)
		{
			nextY++;
			currentVertex = getVertexNumber(currentX+1,currentY+1);
			nextVertex = getVertexNumber(currentX+1-1,currentY+1);
			currentY=nextY;
		}

		matrix[currentVertex][nextVertex] = 
		matrix[nextVertex][currentVertex] = NULL;
	}
}

// поиск в глубину с открытием случайных тупиков лабиринта
void maze::dfs(int start)
{
	int vertex=0;
	visited[start]=true;
	for(vertex=1;vertex<vertexCount;vertex++)
	{
		if(matrix[start][vertex])
		{
			mazeMatrix[start][vertex] = rand()%2;
			if(!visited[vertex])					
				dfs(vertex);
		}
	}
}

// насройка "холста" и рисовальщика
void maze::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setViewport( 25, 5, (mazeWidth+1)*vertexSpace, mazeHeight*vertexSpace );
	
	drawMaze(&painter);
}

// отрисовка лабиринта
void maze::drawMaze(QPainter *painter)
{
	QPen pen(palette().windowText() , 2.5);
	QColor color(Qt::black);
	// настройка ручки с кисточкой
	painter->setPen(pen);
	painter->setBrush(palette().window());
	// нужна магия, без неё никак
	int magicDelta = 5;
	// сохраняем габариты холста
	int x=painter->viewport().x();
	int y=painter->viewport().y();
	int w=painter->viewport().width()+magicDelta;
	int h=painter->viewport().height()+magicDelta;

	painter->drawLine(x,y,w,y);					// top border верхняя граница
	painter->drawLine(x,y+vertexSpace,x,h);		// left border левая граница
	painter->drawLine(x,h,w,h);					// bottom border нижняя граница
	painter->drawLine(w,y,w,h-vertexSpace);		// right border правая граница

	for(int i=1;i<mazeWidth;i++)				// vertexes вершины
	{
		for(int j=1;j<mazeHeight;j++)
		{
			painter->drawLine(x+i*vertexSpace-2,y+j*vertexSpace,x+i*vertexSpace+2,y+j*vertexSpace);
			painter->drawLine(x+i*vertexSpace,y+j*vertexSpace-2,x+i*vertexSpace,y+j*vertexSpace+2);
		}
	}

	int xVertex1=0,xVertex2=0,yVertex1=0,yVertex2=0;	// координаты вершин для отрисовки ребер
	for(int i=1;i<mazeMatrix.size()-1;i++)
	{
		for(int j=i;j<mazeMatrix[0].size()-1;j++)
		{
			if(mazeMatrix[i][j])						// если есть ребро
			{
				getVertexCoords(i,xVertex1,yVertex1);	// вычислить по индексам (номерам) в матрице
				getVertexCoords(j,xVertex2,yVertex2);	// координаты вершин
				if((xVertex1==xVertex2) || (yVertex1==yVertex2))	// если ребро соединяет вершины строго по горизонтали или вертикали - 
				{
					painter->drawLine(x+xVertex1*vertexSpace,y+yVertex1*vertexSpace,x+xVertex2*vertexSpace,y+yVertex2*vertexSpace);	// отрисовка ребра
				}
			}
		}
	}
}