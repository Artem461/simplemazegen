#include <QtGui>
#include "maze.h"

maze::maze(int _mazeWidth, int _mazeHeight, int _currentAlgo, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	mazeWidth = _mazeWidth;
	mazeHeight = _mazeHeight;
	currentAlgo = algorythm (_currentAlgo);
	
	vertexCount = (mazeWidth+1) * (mazeHeight+1);	// число вершин
	vertexSpace = 20;

	srand(time(NULL));	// инициализация рандомайзера

	mazeMatrix = matrix = vector<vector<int>> ( vertexCount,vector<int>(vertexCount,NULL) );
	visited = vector<bool>(vertexCount,false);

	if(currentAlgo == DFS)
	{
		// строим все возможные стенки
		buildMatrix();
		// depth-first search поиск в глубину
		for(int i=0;i<visited.size();i++)
			if(!visited[i])
				dfs(i);
	}
	else if(currentAlgo == HEURISTIC)
	{
		heuriscticBuild();
	}

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
	y=number/(mazeWidth+1);
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

	mazeMatrix = matrix;						// инициализируем конечную матрицу лабиринта - только что созданной закрытой
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
			mazeMatrix[start][vertex] = mazeMatrix[vertex][start] = rand()%2;
			if(!visited[vertex])					
				dfs(vertex);
		}
	}
}

// эвристический алгоритм
void maze::heuriscticBuild()
{
	// магическое число раз
	for(int i=0;i<vertexCount/mazeHeight;i++)
		// для случайной пары вершин - прокладываем стенку от одной к другой
		makeEdge( getVertexNumber(1+rand()%(mazeWidth-1),1+rand()%(mazeHeight-1)) , getVertexNumber(rand()%(mazeWidth),rand()%(mazeHeight)) );
		//makeEdge(rand()%(vertexCount-1),rand()%(vertexCount-1));
}

// строит стену от вершины start к вершине finish
void maze::makeEdge(int start, int finish)
{
	int edge = 1;
	// возможные направления прокладки стены
	enum direction {STAY,LEFT,RIGHT,UP,DOWN};
	direction currentDirection=STAY;

	int nextX=0,nextY=0,endX=0,endY=0;
	// текущая пара вершин для постройки стенки
	int currentVertex=start,nextVertex=start;
	// инициализация координат начальной и конечной вершин
	getVertexCoords(start,nextX,nextY);
	getVertexCoords(finish,endX,endY);
	// массив допустимых направлений движения
	vector<direction> permitDirections(NULL);
	// определение допустимых направлений
	if(nextX < endX)	permitDirections.push_back(RIGHT);
	else if(nextX > endX) permitDirections.push_back(LEFT);

	if(nextY < endY)	permitDirections.push_back(DOWN);
	else if(nextY > endY) permitDirections.push_back(UP);

	while(currentVertex!=finish)
	{
		if(permitDirections.size()>1)
			// если текущая и конечная вершины на одной горизонтали
			if(nextX == endX)
				// убираем из допустимых направлений прокладки "право-лево"
				permitDirections.erase(permitDirections.begin());
			else if(nextY == endY)
				// аналогично для "вверх-вниз"
				permitDirections.pop_back();
		// определяем случайное направление движения из допустимых
		if(permitDirections.size())
			currentDirection = permitDirections.at(rand() % permitDirections.size());
		// в зависимости от выбранного направления определяем следующую вершину
		if(currentDirection==LEFT)
			nextX--;
		else if(currentDirection==RIGHT)
			nextX++;
		else if(currentDirection==UP)
			nextY--;
		else if(currentDirection==DOWN)
			nextY++;

		nextVertex=getVertexNumber(nextX,nextY);
		// строим стенку между текущей и следующей вершинами
		mazeMatrix[currentVertex][nextVertex] = 
		mazeMatrix[nextVertex][currentVertex] = edge;

		currentVertex=nextVertex;
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